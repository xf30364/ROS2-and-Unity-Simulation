using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;

public class BatchRunner
{
    public static void RunSimulation()
    {
        // 1. 打开你的仿真场景 (请确保路径正确)
        EditorSceneManager.OpenScene("Assets/Scenes/RMUC2023.unity"); 
        
        // 2. 开启你关心的那个优化设置
        EditorSettings.enterPlayModeOptionsEnabled = true;
        EditorSettings.enterPlayModeOptions = EnterPlayModeOptions.DisableDomainReload | EnterPlayModeOptions.DisableSceneReload;

        // 3. 强制进入播放模式
        EditorApplication.isPlaying = true;
        
        Debug.Log(">>> 仿真正式开始：正在监测主线程耗时... <<<");
    }
}
