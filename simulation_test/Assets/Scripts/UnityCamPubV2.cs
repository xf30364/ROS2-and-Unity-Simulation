using UnityEngine;
using ROS2;
using UnityEngine.Rendering;
using Unity.Collections;
using System.Buffers;
using System.Runtime.InteropServices;
using Unity.Collections.LowLevel.Unsafe;
public class UnityCamPubV2 : MonoBehaviour 
{
    private static readonly byte[] LinearToSRGBTable = new byte[256];
    static UnityCamPubV2()
    {
        for (int i = 0; i < 256; i++)
        {
            float linear = i / 255f;
            // 线性转 sRGB 公式
            float srgb = (linear <= 0.0031308f) ? 
                         (linear * 12.92f) : 
                         (1.055f * Mathf.Pow(linear, 1.0f / 2.4f) - 0.055f);
            LinearToSRGBTable[i] = (byte)Mathf.Clamp(srgb * 255f, 0, 255);
        }
    }

    [Header("Components")]
    [SerializeField] private Camera cam; 

    [Header("ROS2 Settings")]
    [SerializeField] private string imageTopic = "/image_raw";
    [SerializeField] private string frameId = "camera_optical_frame";

    [Header("Performance")]
    [SerializeField, Range(1, 120)] private int publishRate = 100;

    private ROS2UnityComponent ros2Unity;
    private ROS2Node ros2Node;
    private IPublisher<sensor_msgs.msg.Image> imagePub;

    private bool isInitialized = false;
    private float lastPublishTime = 0f;
    private ArrayPool<byte> byteArrayPool = ArrayPool<byte>.Shared;
    
    private Material flipMaterial;
    
    private RenderTexture flippedRT;
    
    private byte[] _persistentBuffer;
    
    void Start()
    {
 	 Application.runInBackground = true;
        // 解锁帧率限制
        Application.targetFrameRate = -1;
        QualitySettings.vSyncCount = 0;
        if (cam == null) cam = GameObject.Find(name + "/base_link/yaw_link/pitch_link/camera").GetComponent<Camera>();
        if (cam == null) {
            Debug.LogError("Error: Camera not found!");
            return;
        }
        if (cam.targetTexture == null)
        {
            cam.targetTexture = new RenderTexture(1280, 1080, 24, RenderTextureFormat.ARGB32);
            ///cam.targetTexture = new RenderTexture(1280, 1080, 24, RenderTextureFormat.ARGB32, RenderTextureReadWrite.sRGB);
     
        }
	flippedRT = new RenderTexture(1280, 1080, 0, RenderTextureFormat.ARGB32);
        flippedRT.Create();
        flipMaterial = new Material(Shader.Find("Hidden/Internal-Colored"));
        
        ros2Unity = GetComponentInParent<ROS2UnityComponent>();
    }

    void Update()
    {
        if (!isInitialized && ros2Unity != null && ros2Unity.Ok())
        {
            ros2Node = ros2Unity.CreateNode("unity_camera_publisher");
            imagePub = ros2Node.CreateSensorPublisher<sensor_msgs.msg.Image>(imageTopic);
            isInitialized = true;
            Debug.Log("ROS2 Camera Publisher Initialized.");
        }
    }

    void OnEnable() => RenderPipelineManager.endCameraRendering += OnCameraRender;
    void OnDisable() => RenderPipelineManager.endCameraRendering -= OnCameraRender;

    private void OnCameraRender(ScriptableRenderContext context, Camera camera)
    {
        if (camera != cam || !isInitialized) return;

        //if (Time.realtimeSinceStartup - lastPublishTime < (1.0f / publishRate)) return;
        //lastPublishTime = Time.realtimeSinceStartup;

        var stamp = new builtin_interfaces.msg.Time();
        ros2Node.clock.UpdateROSClockTime(stamp);

        //Graphics.Blit(cam.targetTexture, flippedRT, new Vector2(1, -1), new Vector2(0, 1));
        AsyncGPUReadback.Request(cam.targetTexture, 0, TextureFormat.RGB24, request => 
        
        //AsyncGPUReadback.Request(flippedRT, 0, TextureFormat.RGB24, request => 
        {
            if (request.hasError) return;
            ExecutePublish(request, stamp);
        });
    }

    private void ExecutePublish1(AsyncGPUReadbackRequest request, builtin_interfaces.msg.Time timestamp)
    {
       /* byte[] data = request.GetData<byte>().ToArray(); 

        var imageMsg = new sensor_msgs.msg.Image
        {
            Header = new std_msgs.msg.Header { Frame_id = frameId, Stamp = timestamp },
            Height = (uint)request.height,
            Width = (uint)request.width,
            Encoding = "rgb8",
            Step = (uint)(request.width * 3),
            Data = data
        };*/
	if (request.hasError) return;

    var nativeData = request.GetData<byte>();
    int width = request.width;
    int height = request.height;
    int rowSize = width * 3; // RGB24
    
    // 1. 初始化或复用持久化缓冲区
    if (_persistentBuffer == null || _persistentBuffer.Length != nativeData.Length)
    {
        _persistentBuffer = new byte[nativeData.Length];
    }

    // 2. 在子线程进行行反转拷贝
    // 这种内存访问模式对 CPU 缓存非常友好 (Sequential Write)
    for (int y = 0; y < height; y++)
    {
        int srcRow = y * rowSize;
        int destRow = (height - 1 - y) * rowSize;
        NativeArray<byte>.Copy(nativeData, srcRow, _persistentBuffer, destRow, rowSize);
    }

    // 3. 组装并发布消息
    // 注意：这里的消息创建依然在子线程，这是允许的
    var imageMsg = new sensor_msgs.msg.Image
    {
        Header = new std_msgs.msg.Header 
        { 
            Frame_id = frameId, 
            Stamp = timestamp 
        },
        Height = (uint)height,
        Width = (uint)width,
        Encoding = "rgb8",
        Step = (uint)rowSize,
        Data = _persistentBuffer // 直接引用缓冲区
    };

    imagePub.Publish(imageMsg);
    }


    private void ExecutePublish(AsyncGPUReadbackRequest request, builtin_interfaces.msg.Time timestamp)
    {
        if (request.hasError) return;

        // 获取 GPU 回传的原始数据 (NativeArray)
        var nativeData = request.GetData<byte>();
        int width = request.width;
        int height = request.height;
        int rowSize = width * 3;
        int totalSize = height * rowSize;

        if (_persistentBuffer == null || _persistentBuffer.Length != totalSize)
        {
            _persistentBuffer = new byte[totalSize];
        }

        // 2. 使用指针进行极速处理
        unsafe 
        {
            // 获取 NativeArray 的原始指针
            byte* pSrcBase = (byte*)nativeData.GetUnsafeReadOnlyPtr();
            
            // 固定托管数组地址，防止 GC 移动
            fixed (byte* pDestBase = _persistentBuffer)
            fixed (byte* pTable = LinearToSRGBTable)
            {
                for (int y = 0; y < height; y++)
                {
                    // 数据源行索引（正常顺序）
                    int srcRowOffset = y * rowSize;
                    // 目标行索引（上下翻转）
                    int destRowOffset = (height - 1 - y) * rowSize;

                    byte* pSrcRow = pSrcBase + srcRowOffset;
                    byte* pDestRow = pDestBase + destRowOffset;

                    // 逐像素处理并查表修复亮度
                    for (int x = 0; x < rowSize; x++)
                    {
                        pDestRow[x] = pTable[pSrcRow[x]];
                    }
                }
            }
        }

        // 3. 组装并发布消息
        var imageMsg = new sensor_msgs.msg.Image
        {
            Header = new std_msgs.msg.Header { Frame_id = frameId, Stamp = timestamp },
            Height = (uint)height,
            Width = (uint)width,
            Encoding = "rgb8",
            Step = (uint)rowSize,
            Data = _persistentBuffer 
        };

        imagePub.Publish(imageMsg);
    }
}


