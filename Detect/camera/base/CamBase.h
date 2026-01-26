/**
 * @file Camera.h
 * @brief 相机参数类和相机驱动类接口
 * @author BG2EDG (928330305@qq.com)
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2022-04-16 <td>BG2EDG  <td>初步接口
 * </table>
 */
#pragma once
#include <functional>
#include <opencv2/core.hpp>
#include <string>
#include <utility>
#include <vector>

namespace hitcrt::camera {

// 四种模式：拉流回调，拉流多线程，外部触发，软触发
enum Mode { STREAM = 0, STREAM_MULTITHREAD, LINE, SOFT, UNKNOWN };
// 加载参数来源
enum ParamSource { PARAM = 0, FILE };
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
// 帧回调函数的函数对象，必须使用如下函数签名
// using CallBack = std::function<void(const int64_t, const cv::Mat&)>;
using CallBack = std::function<void(const TimePoint&, const cv::Mat&)>;

/**
 * @brief 相机参数类，用于存储和传递参数
 * @author BG2EDG (928330305@qq.com)
 */
class CameraParams {
   public:
    // CameraParams() = default;
    CameraParams(const CallBack& onCall = noUse, const uint id = 0,
                 const Mode mode = STREAM, const ParamSource paramSrc = PARAM,
                 const std::string& path = "")
        : m_onCall(onCall),
          m_id(id),
          m_mode(mode),
          m_paramSrc(paramSrc),
          m_path(path){};
    CameraParams(const CameraParams& cameraParams)
        : m_onCall(cameraParams.onCall()),
          m_id(cameraParams.id()),
          m_mode(cameraParams.mode()),
          m_paramSrc(cameraParams.paramSrc()),
          m_path(cameraParams.path()){};

    // setters
    void set(const CallBack& onCall, const uint id = 0,
             const Mode mode = STREAM, const ParamSource paramSrc = PARAM,
             const std::string& path = "") {
        setID(id);
        setMode(mode);
        setParamSource(paramSrc);
        setOnCall(onCall);
        setPath(path);
    }
    void setID(const uint id) { m_id = id; }
    void setMode(const Mode mode) { m_mode = mode; }
    void setParamSource(const ParamSource paramSrc) { m_paramSrc = paramSrc; }
    void setOnCall(const CallBack onCall) { m_onCall = onCall; }
    void setPath(const std::string& path) { m_path = path; }
    // getters
    const uint id() const { return m_id; }
    const Mode mode() const { return m_mode; }
    const ParamSource paramSrc() const { return m_paramSrc; }
    const std::string modeStr() const {
        switch (m_mode) {
            case Mode::STREAM:
                return "Stream";
            case Mode::STREAM_MULTITHREAD:
                return "Stream MultiThread";
            case Mode::LINE:
                return "Line";
            case Mode::SOFT:
                return "Software";
            case Mode::UNKNOWN:
                return "Unknown";
        }
    }
    const CallBack onCall() const { return m_onCall; }
    const std::string path() const { return m_path; }
    static void noUse(const TimePoint&, const cv::Mat&) {}

   protected:
    uint m_id = 0;
    Mode m_mode = STREAM;
    ParamSource m_paramSrc = PARAM;
    CallBack m_onCall = noUse;
    std::string m_path = "";
};

/**
 * @brief 相机驱动类，用于直接改变相机参数
 * @author BG2EDG (928330305@qq.com)
 */
class Camera {
   public:
    //找设备
    virtual bool findDevices() = 0;
    //开启相机
    virtual bool open(const uint id) = 0;
    //用CameraParam设置信息，但与输入的CameraParams有关，暂时没用
    // virtual bool set(const CameraParams& cameraParams) = 0;
    //从文件读取配置，但与输入的CameraParams有关，暂时没用
    // virtual bool set(const std::string& path) = 0;
    //向相机查询当前参数，但与输出的CameraParams有关，暂时没用
    // virtual CameraParams get() = 0;
    //开始采集数据
    virtual bool start(const CameraParams& cameraParams) = 0;
    //停止采集数据
    virtual bool stop(const CameraParams& cameraParams) = 0;
    //关闭相机
    virtual bool close() = 0;
};

}  // namespace hitcrt::camera