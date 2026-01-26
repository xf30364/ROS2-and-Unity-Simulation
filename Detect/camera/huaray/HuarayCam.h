/**
 * @file HuarayCam.h
 * @brief Huaray参数类和驱动类
 * @author BG2EDG (928330305@qq.com)
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2022-04-16 <td>BG2EDG  <td>First Stable Version
 * <tr><td>2022-05-25 <td>BG2EDG  <td>HuarayParams增加onGet成员变量
 * <tr><td>2022-06-06 <td>BG2EDG  <td>加入快速版reset，不用断流重置参数
 * <tr><td>2022-06-21 <td>BG2EDG  <td>加入自动搜索Dahua/Huaray设备Idx功能
 * <tr><td>2023-01-08 <td>GL      <td>加入指定SN码功能
 * </table>
 */
#pragma once
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <thread>

#include "CamBase.h"
#include "IMVApi.h"

namespace hitcrt::camera {

/**
 * @brief 相机参数类，用于相机参数设置和存储
 * @author BG2EDG (928330305@qq.com)
 */
class HuarayParams : public CameraParams {
   public:
    HuarayParams() = default;
    HuarayParams(const CallBack& onCall, const int id, const Mode mode,
                 const int width, const int height, const int blackLevel,
                 const int brightness, const int digitalShift,
                 const int sharpness, const double exposureTime,
                 const double gamma, const double gainRaw,
                 const std::vector<double>& balanceRatio,
				 const std::string& sn = "",
                 const std::function<void()>& offLineFunc = noUse,
                 const std::function<void()>& onLineFunc = noUse,
                 const std::function<void()>& onGet = noUse);
    HuarayParams(const CallBack& onCall, const int id, const Mode mode,
                 const std::string& path,
                 const std::function<void()>& offLineFunc = noUse,
                 const std::function<void()>& onLineFunc = noUse,
                 const std::function<void()>& onGet = noUse);
    HuarayParams(const HuarayParams& cameraParams);

    // 格式化输出
    void show();
    // setters
    void set(const CallBack& onCall, const int id, const Mode mode,
             const std::string& path,
             const std::function<void()>& offLineFunc = noUse,
             const std::function<void()>& onLineFunc = noUse,
             const std::function<void()>& onGet = noUse);
    void set(const CallBack& onCall, const int id, const Mode mode,
             const int width, const int height, const int blackLevel,
             const int brightness, const int digitalShift, const int sharpness,
             const double exposureTime, const double gamma,
             const double gainRaw, const std::vector<double>& balanceRatio,
			 const std::string& sn = "",
             const std::function<void()>& offLineFunc = noUse,
             const std::function<void()>& onLineFunc = noUse,
             const std::function<void()>& onGet = noUse);  
	
	void setSN(const std::string& sn);
    void setWidth(const int value);
    void setHeight(const int value);
    void setBlackLevel(const int value);
    void setBrightness(const int value);
    void setDigitalShift(const int value);
    void setSharpness(const int value);
    void setExposureTime(const double value);
    void setGamma(const double value);
    void setGainRaw(const double value);
    void setBalanceRatio(const std::vector<double>& values);
    void setOffLineFunc(const std::function<void()>& offLineFunc);
    void setOnLineFunc(const std::function<void()>& onLineFunc);
    void setOnGet(const std::function<void()>& onGet);
    // getters
	const std::string SN() const;
    const int width() const;
    const int height() const;
    const int blackLevel() const;
    const int brightness() const;
    const int digitalShift() const;
    const int sharpness() const;
    const double exposureTime() const;
    const double gamma() const;
    const double gainRaw() const;
    const std::vector<double>& balanceRatio() const;
    const std::function<void()> offLineFunc() const;
    const std::function<void()> onLineFunc() const;
    const std::function<void()> onGet() const;

    // 用于占位
    static void noUse(){};

   private:
	std::string m_cameraSN = "";  // 相机SN码

    int m_width = 0;
    int m_height = 0;
    int m_blackLevel = 0;
    int m_brightness = 0;
    int m_digitalShift = 0;
    int m_sharpness = 0;
    double m_exposureTime = 0;
    double m_gamma = 0;
    double m_gainRaw = 0;
    std::vector<double> m_balanceRatio = {0, 0, 0};

    std::function<void()> m_offLineFunc = noUse;
    std::function<void()> m_onLineFunc = noUse;
    // onGet在进入回调后立即执行，onCall在图像转成BGR8后执行
    // 从抓图到onGet函数调用时间在微妙数量级，可以忽略不计
    std::function<void()> m_onGet = noUse;
};

/**
 * @brief 相机驱动类，调用IMVApi.h提供接口控制相机
 * @author BG2EDG (928330305@qq.com)
 */
class Huaray : public Camera {
   public:
    // 流统计信息：图像错误的帧数，丢包的帧数，正常获取的帧数，帧率，带宽(Mbps)
    using StreamStat = std::tuple<uint, uint, uint, double, double>;

    Huaray() = default;
    Huaray(const HuarayParams& cameraParams);
    //析构函数默认调用terminate
    //防止用户忘记调用导致内存泄漏
    ~Huaray();

    // 调用接口
    // 相机初始化
    bool initiate(const HuarayParams& cameraParams);
    // 参数重置,必须保证之前设置过参数
    bool reset(const HuarayParams& cameraParams);
    // 参数重置快速版,只对部分常用参数重置，不会断流
    bool resetLite(const HuarayParams& cameraParams);
    // 相机关闭
    bool terminate();
    // 向相机查询参数
    std::unique_ptr<HuarayParams> get();
    //存相机参数，输出.mvcfg文件，以xml格式编写
    bool saveConfig(const std::string& path);
    // 判断相机是否开启，如有其他程序占用也会返回false
    bool isOpen();
    // 判断相机是否正常抓图
    bool isGrabbing();
    // 多线程取图，使用时另开线程,该部分未经过运行测试，使用前请测试功能是否正常
    std::tuple<bool, TimePoint, std::unique_ptr<cv::Mat>> getFrameImage();
    // 硬件触发计数
    int64_t getTriggerCnt();
    int64_t getTriggerLost();
    bool resetTriggerCnt();
    // 硬件性能信息
    // 相机抓图的帧率，不受回调影响
    double fps();
    // 流统计信息：图像错误的帧数，丢包的帧数，正常获取的帧数，帧率，带宽(Mbps)
    StreamStat stat();
    // 重置统计信息
    bool resetStat();

   protected:
    // 用于断线重连回调传参
    using UserData =
        std::tuple<IMV_HANDLE, HuarayParams, std::function<bool()>>;
    using UserDataPtr = std::unique_ptr<UserData>;
    // 具体功能接口，临时修改或增加功能请继承此类再使用
    // 根据SDK例程修改，使用前一定看清前提
    // 找设备
    virtual bool findDevices() override;
    //开启相机
    virtual bool open(const uint id = 1) override;
    //关闭相机
    virtual bool close() override;
    //开始采集数据
    virtual bool start(const CameraParams& cameraParams) override;
    //停止采集数据
    virtual bool stop(const CameraParams& cameraParams) override;
    // 销毁设备句柄
    void destroy();
    // 释放m_frame
    bool releaseFrame();

    //相机设置
    virtual bool set(const HuarayParams& cameraParams);    //设置全部
    bool setParams(const HuarayParams& cameraParams);      //设置成像参数
    bool setParamsLite(const HuarayParams& cameraParams);  //设置部分参数
    bool setOperation(const HuarayParams& cameraParams);   //设置相机回调
    //设置成像参数
    bool setWidth(const int value);
    bool setHeight(const int value);
    bool setBlackLevel(const int value);
    bool setBrightness(const int value);
    bool setDigitalShift(const int value);
    bool setSharpness(const int value);
    bool setExposureTime(const double value);
    bool setGamma(const double value);
    bool setGainRaw(const double value);
    bool setBalanceRatio(const std::vector<double>& values);
    bool saveMVCFG(const std::string& path);
    bool loadMVCFG(const std::string& path);  //加载相机参数
    bool startGrabbing();                     //拉流
    bool stopGrabbing();                      //停止拉流
    bool setContinuous();           //设置连续抓图，关闭触发
    bool setLineTrigger();          //设置外部触发，待测试
    bool setBufferSize(uint size);  // 设置缓存帧数,1~32，暂未使用

    // 清空数据缓存
    bool clearFrameBuffer();
    // 数据相关
    // 驱动函数实现，速度慢且不稳，4～8ms
    static std::pair<bool, std::unique_ptr<cv::Mat>> sdkCvtMatBGR8(
        const IMV_Frame& frame, const IMV_HANDLE devHandle);
    // OpenCV实现，速度较快1ms
    static std::pair<bool, std::unique_ptr<cv::Mat>> cvtMatBGR8(
        const IMV_Frame& frame, const IMV_HANDLE devHandle);

    // 帧回调相关
    std::unique_ptr<UserData> m_userDataPtr;
    bool attachGrabbing(UserData* pUserData);
    static void onGetFrame(IMV_Frame* pFrame, void* pUser);

    // 重连回调相关
    bool attachConnection(UserData* pUserData);
    static void onDeviceLinkNotify(const IMV_SConnectArg* pConnectArg,
                                   void* pUser);
    static void deviceOffLine(const UserData& userData);
    static void deviceOnLine(const UserData& userData);
    virtual bool retry();

    // 中间函数
    bool setEnumSymbol(const std::string& enumName,
                       const std::string& enumValue);
    std::string getEnumSymbol(const std::string& enumName);
    bool setBoolValue(const std::string& featureName, const bool value);
    bool getBoolValue(const std::string& featureName);
    bool setIntValue(const std::string& featureName, const int value);
    int getIntValue(const std::string& featureName);
    bool setDoubleValue(const std::string& featureName, const double value);
    double getDoubleValue(const std::string& featureName);
    void displayDeviceInfo(IMV_DeviceList deviceInfoList);  //基本信息展示
	uint getSNIndex(const HuarayParams &cameraParams, IMV_DeviceList deviceInfoList);  // 获取SN码对应的相机索引
    std::vector<double> getBalanceRatio();
    Mode getMode();
    uint getFirstHuarayID();  // 自动找Dahua/Huaray相机Idx

    uint m_cameraIndex = 0;
    // m_ret使用的想法：
    // 会对用户操作产生致命影响的，用m_ret记录错误代码
    // setParams不成功相机顶多参数不对，且结果太多，不必要记录错误代码
    // setOperation不成功相机开机了也没用，如果报错了应该记录
    int m_ret = IMV_OK;
    IMV_HANDLE m_devHandle;
    IMV_Frame m_frame;
    IMV_DeviceList m_devList;

    //外部触发：上升沿:RisingEdge,下降沿:FallingEdge
    const std::string TRIGGER_EDGE = "RisingEdge";
    //断线重连的最大尝试次数
    const uint MAX_RETRY_TIMES = 10;
    //获取一张图片的最长等待时间
    const uint TIMEOUT_MS = 5;
    //缓冲区大小（1～32，默认是8）
    const uint BUFFER_COUNT = 8;  //暂未使用
};
}  // namespace hitcrt::camera