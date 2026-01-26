using UnityEngine;
using ROS2;
using UnityEngine.Rendering;
using Unity.Collections;
using System.Collections.Concurrent;

public class UnityCamPubV1 : MonoBehaviour
{
    [Header("ROS2 Settings")]
    [SerializeField] private string imageTopic = "/image_raw";
    [SerializeField] private string infoTopic = "/camera_info";
    [SerializeField] private string frameId = "camera_optical_frame";

    [Header("Performance")]
    [SerializeField, Range(1, 60)] private int publishRate = 30;
    [SerializeField] private bool enableAsyncReadback = true;

    // ROS2 Components
    private ROS2UnityComponent ros2Unity;
    private ROS2Node ros2Node;
    private IPublisher<sensor_msgs.msg.Image> imagePub;
    private IPublisher<sensor_msgs.msg.CameraInfo> infoPub;

    // Camera Components
    private Camera cam;
    private float fx, fy, cx, cy;

    // Thread-safe queues
    private readonly ConcurrentQueue<ImageData> pendingImages = new ConcurrentQueue<ImageData>();
    private readonly ConcurrentQueue<RenderTexture> rtPool = new ConcurrentQueue<RenderTexture>();

    // Frame control
    private int frameCounter = 0;
    private bool isInitialized = false;

    private struct ImageData
    {
        public byte[] Data;
        public builtin_interfaces.msg.Time Timestamp;
        public int Width;
        public int Height;
    }

    void Start()
    {
        Initialize();
    }

    void OnEnable()
    {
        RenderPipelineManager.endCameraRendering += OnCameraRender;
    }

    void OnDisable()
    {
        RenderPipelineManager.endCameraRendering -= OnCameraRender;
        Cleanup();
    }

    void Update()
    {
        // Initialize ROS2 when ready
        if (!isInitialized && ros2Unity != null && ros2Unity.Ok())
        {
            InitializeROS2();
        }

        // Process completed images on main thread
        ProcessPendingImages();
    }

    #region Initialization
    private void Initialize()
    {
        // Get camera
        cam = GameObject.Find(name + "/base_link/yaw_link/pitch_link/camera").GetComponent<Camera>();
        Vector3 worldPos = cam.transform.position;
	Debug.Log("Camera world position: " + worldPos);
	GameObject armor = GameObject.Find("Robot/base_link/chassis_link/armor_base_b");
        Vector3 armorPos = armor.transform.position;
	Debug.Log("armorPos world position: " + armorPos);
	int W = 1280, H = 1024;
	var rt = new RenderTexture(W, H, 24, RenderTextureFormat.ARGB32);
	rt.Create();
	cam.targetTexture = rt;
        if (cam == null || cam.targetTexture == null)
        {
            Debug.LogError("Camera or target texture not found!");
            return;
        }

        // Calculate camera intrinsics
        CalculateIntrinsics();

        // Get ROS2 component
        ros2Unity = GetComponent<ROS2UnityComponent>();
        if (ros2Unity == null)
        {
            Debug.LogError("ROS2UnityComponent not found!");
            return;
        }
    }

    private void InitializeROS2()
    {
        try
        {
            ros2Node = ros2Unity.CreateNode("unity_camera_publisher");
            imagePub = ros2Node.CreateSensorPublisher<sensor_msgs.msg.Image>(imageTopic);
            infoPub = ros2Node.CreateSensorPublisher<sensor_msgs.msg.CameraInfo>(infoTopic);
            isInitialized = true;
            Debug.Log($"ROS2 camera publisher initialized (Rate: {publishRate}Hz)");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"Failed to initialize ROS2: {e.Message}");
        }
    }

    private void CalculateIntrinsics()
    {
        float vfov = cam.fieldOfView * Mathf.Deg2Rad;
	float H = cam.targetTexture.height;
	float W = cam.targetTexture.width;
	fy = H / (2f * Mathf.Tan(vfov / 2f));
	fx = fy * (W / H);     // 或 fx = fy * cam.aspect（但以 W/H 更一致）
	cx = (W - 1) / 2f;
	cy = (H - 1) / 2f;
	 
    }
    #endregion

    #region Camera Rendering Callback
    private void OnCameraRender(ScriptableRenderContext context, Camera camera)
    {
        if (camera != cam || !isInitialized) return;

        // Rate limiting
        frameCounter++;
        //if (frameCounter % Mathf.RoundToInt(60f / publishRate) != 0) return;

        try
        {
            // Get timestamp
            var timestamp = new builtin_interfaces.msg.Time();
            ros2Node.clock.UpdateROSClockTime(timestamp);

            // Get or create render texture
            if (!rtPool.TryDequeue(out RenderTexture tmpRT))
            {
                tmpRT = new RenderTexture(cam.targetTexture.width, cam.targetTexture.height, 0,
                    RenderTextureFormat.ARGB32);
            }

            // Flip Y axis for ROS coordinate system
            Graphics.Blit(cam.targetTexture, tmpRT, new Vector2(1, -1), new Vector2(0, 1));

            if (enableAsyncReadback)
            {
                // Async GPU readback with callback
                AsyncGPUReadback.Request(tmpRT, 0, TextureFormat.RGB24,
                    (AsyncGPUReadbackRequest request) => OnGPUReadbackComplete(request, tmpRT, timestamp));
            }
            else
            {
                // Fallback: Synchronous readback (for debugging)
                ProcessFrameSynchronously(tmpRT, timestamp);
                ReturnToPool(tmpRT);
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"Camera render error: {e.Message}");
        }
    }

    private void OnGPUReadbackComplete(AsyncGPUReadbackRequest request, RenderTexture rt,
        builtin_interfaces.msg.Time timestamp)
    {
        // This callback executes in a background thread

        if (request.hasError)
        {
            Debug.LogWarning("GPU readback failed");
            ReturnToPool(rt);
            return;
        }

        try
        {
            // Copy data from NativeArray to byte[] (required for ROS message)
            var nativeData = request.GetData<byte>();
            byte[] imageData = new byte[nativeData.Length];
            nativeData.CopyTo(imageData);

            // Queue for main thread processing
            pendingImages.Enqueue(new ImageData
            {
                Data = imageData,
                Timestamp = timestamp,
                Width = request.width,
                Height = request.height
            });

            // Return render texture to pool
            ReturnToPool(rt);
        }
        catch (System.Exception e)
        {
            Debug.LogError($"Readback processing error: {e.Message}");
            ReturnToPool(rt);
        }
    }

    private void ProcessFrameSynchronously(RenderTexture rt, builtin_interfaces.msg.Time timestamp)
    {
        // Synchronous fallback (blocks main thread, not recommended for production)
        Texture2D tex = new Texture2D(rt.width, rt.height, TextureFormat.RGB24, false);
        RenderTexture.active = rt;
        tex.ReadPixels(new Rect(0, 0, rt.width, rt.height), 0, 0);
        tex.Apply();
        RenderTexture.active = null;

        pendingImages.Enqueue(new ImageData
        {
            Data = tex.GetRawTextureData(),
            Timestamp = timestamp,
            Width = rt.width,
            Height = rt.height
        });

        Destroy(tex);
    }
    #endregion

    #region Main Thread Processing
    private void ProcessPendingImages()
    {
        // Process all pending images on main thread
        while (pendingImages.TryDequeue(out ImageData imageData))
        {
            PublishImage(imageData);
        }
    }

    private void PublishImage(ImageData imageData)
    {
        if (!isInitialized || imagePub == null) return;

        try
        {
            // Create and publish image message
            var imageMsg = CreateImageMessage(imageData);
            imagePub.Publish(imageMsg);

            // Create and publish camera info
            var infoMsg = CreateCameraInfoMessage(imageMsg.Header);
            infoPub.Publish(infoMsg);
        }
        catch (System.Exception e)
        {
            Debug.LogError($"Publish error: {e.Message}");
        }
    }

    private sensor_msgs.msg.Image CreateImageMessage(ImageData imageData)
    {
        return new sensor_msgs.msg.Image
        {
            Header = new std_msgs.msg.Header
            {
                Frame_id = frameId,
                Stamp = imageData.Timestamp
            },
            Height = (uint)imageData.Height,
            Width = (uint)imageData.Width,
            Encoding = "rgb8",
            Step = (uint)(imageData.Width * 3),
            Data = imageData.Data
        };
    }

    private sensor_msgs.msg.CameraInfo CreateCameraInfoMessage(std_msgs.msg.Header header)
    {
        var cameraInfo = new sensor_msgs.msg.CameraInfo
        {
            Header = header,
            Height = (uint)cam.pixelHeight,
            Width = (uint)cam.pixelWidth,
            Distortion_model = "plumb_bob",
            D = new double[] { 0, 0, 0, 0, 0 },
            // P = new double[] { fx, 0, cx, 0, 0, fy, cy, 0, 0, 0, 1, 0 }
        };
        if (cameraInfo.K != null && cameraInfo.K.Length >= 9)
        {
            cameraInfo.K[0] = fx;  // fx
            cameraInfo.K[1] = 0;   // 0
            cameraInfo.K[2] = cx;  // cx
            cameraInfo.K[3] = 0;   // 0
            cameraInfo.K[4] = fy;  // fy
            cameraInfo.K[5] = cy;  // cy
            cameraInfo.K[6] = 0;   // 0
            cameraInfo.K[7] = 0;   // 0
            cameraInfo.K[8] = 1;   // 1
        }
        return cameraInfo;
    }
    #endregion

    #region Resource Management
    private void ReturnToPool(RenderTexture rt)
    {
        if (rt != null)
        {
            rtPool.Enqueue(rt);
        }
    }

    private void Cleanup()
    {
        // Clear pending images
        while (pendingImages.TryDequeue(out _)) { }

        // Cleanup render texture pool
        while (rtPool.TryDequeue(out RenderTexture rt))
        {
            if (rt != null)
            {
                rt.Release();
                Destroy(rt);
            }
        }
    }

    void OnDestroy()
    {
        Cleanup();
    }
    #endregion

    #region Editor Helpers
#if UNITY_EDITOR
    void OnValidate()
    {
        publishRate = Mathf.Clamp(publishRate, 1, 60);
    }
#endif
    #endregion
}
