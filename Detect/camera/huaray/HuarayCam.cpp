/**
 * @file HuarayCam.cpp
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
 * <tr><td>2022-05-15 <td>BG2EDG  <td>BlackLevel设置前把BlackLevelAuto设为Off
 * <tr><td>2022-05-25 <td>BG2EDG  <td>HuarayParams增加onGet函数
 * <tr><td>2022-06-06 <td>BG2EDG  <td>加入快速版reset，不用断流重置参数
 * <tr><td>2022-06-21 <td>BG2EDG  <td>加入自动搜索Dahua/Huaray设备Idx功能
 * <tr><td>2023-01-08 <td>GL      <td>加入指定SN码功能
 * </table>
 */
#include "HuarayCam.h"
using namespace hitcrt::camera;
namespace hitcrt::camera {
// ============================== Huaray Params ==============================
HuarayParams::HuarayParams(const hitcrt::camera::CallBack& onCall, const int id, const Mode mode,
                           const int width, const int height, const int blackLevel, const int brightness,
                           const int digitalShift, const int sharpness, const double exposureTime,
                           const double gamma, const double gainRaw, const std::vector<double>& balanceRatio,
                           const std::string& sn, const std::function<void()>& offLineFunc,
                           const std::function<void()>& onLineFunc, const std::function<void()>& onGet)
    : CameraParams(onCall, id, mode),
      m_cameraSN(sn),
      m_width(width),
      m_height(height),
      m_blackLevel(blackLevel),
      m_brightness(brightness),
      m_digitalShift(digitalShift),
      m_sharpness(sharpness),
      m_exposureTime(exposureTime),
      m_gamma(gamma),
      m_gainRaw(gainRaw),
      m_balanceRatio(balanceRatio),
      m_offLineFunc(offLineFunc),
      m_onLineFunc(onLineFunc),
      m_onGet(onGet) {}

HuarayParams::HuarayParams(const CallBack& onCall, const int id, const Mode mode, const std::string& path,
                           const std::function<void()>& offLineFunc, const std::function<void()>& onLineFunc,
                           const std::function<void()>& onGet)
    : CameraParams(onCall, id, mode, ParamSource::FILE, path),
      m_offLineFunc(offLineFunc),
      m_onLineFunc(onLineFunc),
      m_onGet(onGet) {}

HuarayParams::HuarayParams(const HuarayParams& cameraParams)
    : CameraParams(cameraParams),
      m_cameraSN(cameraParams.SN()),
      m_width(cameraParams.width()),
      m_height(cameraParams.height()),
      m_blackLevel(cameraParams.blackLevel()),
      m_brightness(cameraParams.brightness()),
      m_digitalShift(cameraParams.digitalShift()),
      m_sharpness(cameraParams.sharpness()),
      m_exposureTime(cameraParams.exposureTime()),
      m_gamma(cameraParams.gamma()),
      m_gainRaw(cameraParams.gainRaw()),
      m_balanceRatio(cameraParams.balanceRatio()),
      m_offLineFunc(cameraParams.offLineFunc()),
      m_onLineFunc(cameraParams.onLineFunc()),
      m_onGet(cameraParams.onGet()) {}

/**
 * @brief 相机参数信息格式化输出
 * @author BG2EDG (928330305@qq.com)
 */
void HuarayParams::show() {
    std::cout << "----- Huaray Camera -----" << std::endl;
    std::cout << "SN Number   " << m_cameraSN << std::endl;
    std::cout << "Parameters    Values    " << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Mode          " << modeStr() << std::endl;
    std::cout << "Width         " << m_width << std::endl;
    std::cout << "Height        " << m_height << std::endl;
    std::cout << "BlackLevel    " << m_blackLevel << std::endl;
    std::cout << "Brightness    " << m_brightness << std::endl;
    std::cout << "DigitalShift  " << m_digitalShift << std::endl;
    std::cout << "Sharpness     " << m_sharpness << std::endl;
    std::cout << "ExposureTime  " << m_exposureTime << std::endl;
    std::cout << "Gamma         " << m_gamma << std::endl;
    std::cout << "GainRaw       " << m_gainRaw << std::endl;
    std::cout << "Balance Red   " << m_balanceRatio[0] << std::endl;
    std::cout << "Balance Green " << m_balanceRatio[1] << std::endl;
    std::cout << "Balance Blue  " << m_balanceRatio[2] << std::endl;
    std::cout << "----- Huaray Camera -----" << std::endl;
}

// setters
void HuarayParams::set(const CallBack& onCall, const int id, const Mode mode, const int width,
                       const int height, const int blackLevel, const int brightness, const int digitalShift,
                       const int sharpness, const double exposureTime, const double gamma,
                       const double gainRaw, const std::vector<double>& balanceRatio, const std::string& sn,
                       const std::function<void()>& offLineFunc, const std::function<void()>& onLineFunc,
                       const std::function<void()>& onGet) {
    CameraParams::set(onCall, id, mode);
    setSN(sn);
    setWidth(width);
    setHeight(height);
    setBlackLevel(blackLevel);
    setBrightness(brightness);
    setDigitalShift(digitalShift);
    setSharpness(sharpness);
    setExposureTime(exposureTime);
    setGamma(gamma);
    setGainRaw(gainRaw);
    setBalanceRatio(balanceRatio);
    setOffLineFunc(offLineFunc);
    setOnLineFunc(onLineFunc);
    setOnGet(onGet);
}

void HuarayParams::set(const CallBack& onCall, const int id, const Mode mode, const std::string& path,
                       const std::function<void()>& offLineFunc, const std::function<void()>& onLineFunc,
                       const std::function<void()>& onGet) {
    CameraParams::set(onCall, id, mode, ParamSource::FILE, path);
    setOffLineFunc(offLineFunc);
    setOnLineFunc(onLineFunc);
    setOnGet(onGet);
}
void HuarayParams::setSN(const std::string& sn) { m_cameraSN = sn; };
void HuarayParams::setWidth(const int value) { m_width = value; };
void HuarayParams::setHeight(const int value) { m_height = value; };
void HuarayParams::setBlackLevel(const int value) { m_blackLevel = value; };
void HuarayParams::setBrightness(const int value) { m_brightness = value; }
void HuarayParams::setDigitalShift(const int value) { m_digitalShift = value; }
void HuarayParams::setSharpness(const int value) { m_sharpness = value; }
void HuarayParams::setExposureTime(const double value) { m_exposureTime = value; }
void HuarayParams::setGamma(const double value) { m_gamma = value; }
void HuarayParams::setGainRaw(const double value) { m_gainRaw = value; };
void HuarayParams::setBalanceRatio(const std::vector<double>& values) { m_balanceRatio = values; };
void HuarayParams::setOffLineFunc(const std::function<void()>& offLineFunc) { m_offLineFunc = offLineFunc; }
void HuarayParams::setOnLineFunc(const std::function<void()>& onLineFunc) { m_onLineFunc = onLineFunc; }
void HuarayParams::setOnGet(const std::function<void()>& onGet) { m_onGet = onGet; }

// getters
const std::string HuarayParams::SN() const { return m_cameraSN; };
const int HuarayParams::width() const { return m_width; };
const int HuarayParams::height() const { return m_height; };
const int HuarayParams::blackLevel() const { return m_blackLevel; };
const int HuarayParams::brightness() const { return m_brightness; };
const int HuarayParams::digitalShift() const { return m_digitalShift; };
const int HuarayParams::sharpness() const { return m_sharpness; };
const double HuarayParams::exposureTime() const { return m_exposureTime; };
const double HuarayParams::gamma() const { return m_gamma; };
const double HuarayParams::gainRaw() const { return m_gainRaw; };
const std::vector<double>& HuarayParams::balanceRatio() const { return m_balanceRatio; };
const std::function<void()> HuarayParams::offLineFunc() const { return m_offLineFunc; };
const std::function<void()> HuarayParams::onLineFunc() const { return m_onLineFunc; }
const std::function<void()> HuarayParams::onGet() const { return m_onGet; }

// ============================== Huaray Drivers ==============================
// APIs

/**
 * @brief 用相机参数直接构造，则立即执行初始化相机
 * @param[in] cameraParams  相机参数
 * @author BG2EDG (928330305@qq.com)
 */
Huaray::Huaray(const HuarayParams& cameraParams) { initiate(cameraParams); }

/**
 * @brief 初始化
 * @param[in] cameraParams  相机参数
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::initiate(const HuarayParams& cameraParams) {
    if (!findDevices()) {
        return false;
    }
    uint camera_id = 0;
    if (cameraParams.SN() == "auto" || cameraParams.SN() == "") {  // 如果使用索引打开
        camera_id = cameraParams.id();
    } else {
        camera_id = getSNIndex(cameraParams, m_devList);
        if (camera_id == 0) {  // 如果找不到对应SN码的相机
            std::cout << "[Huaray Camera]:Not find Device with SN:" << cameraParams.SN() << std::endl;
            return false;
        }
    }

    if (!open(camera_id)) {
        return false;
    }

    if (!set(cameraParams)) {
        return false;
    }

    if (!start(cameraParams)) {
        return false;
    }

    return true;
}

/**
 * @brief 相机参数重置
 * @param[in] cameraParams     新参数，参数设置
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::reset(const HuarayParams& cameraParams) {
    if (m_devHandle == NULL) {
        return false;
    }
    if (m_userDataPtr == nullptr) {
        return false;
    }

    if (!stop(std::get<1>(*m_userDataPtr))) {
        return false;
    }

    if (!set(cameraParams)) {
        return false;
    }

    if (!start(cameraParams)) {
        return false;
    }

    return true;
}

/**
 * @brief 快速版reset，重置部分常用参数，不断流
 * @param[in] cameraParams  新参数，参数设置
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::resetLite(const HuarayParams& cameraParams) {
    if (m_devHandle == NULL) {
        return false;
    }

    return setParamsLite(cameraParams);
}

/**
 * @brief 相机关闭及回收
 * @param[in] cameraParams  My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::terminate() {
    if (!stop(std::get<1>(*m_userDataPtr))) {
        return false;
    }
    if (!close()) {
        return false;
    }

    destroy();

    return true;
}

/**
 * @brief 相机参数获取
 * @return std::unique_ptr<HuarayParams>
 * @author BG2EDG (928330305@qq.com)
 */
std::unique_ptr<HuarayParams> Huaray::get() {
    if (m_devHandle == NULL) {
        std::vector<double> balanceRatio = {0.0, 0.0, 0.0};
        return std::make_unique<HuarayParams>(CameraParams::noUse, 0, STREAM, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0,
                                              balanceRatio, "", HuarayParams::noUse, HuarayParams::noUse);
    }
    auto mode = getMode();
    int width = getIntValue("Width");
    int height = getIntValue("Height");
    int blackLevel = getIntValue("BlackLevel");
    int brightness = getIntValue("Brightness");
    int digitalShift = getIntValue("DigitalShift");
    int sharpness = getIntValue("Sharpness");
    double exposureTime = getDoubleValue("ExposureTime");
    double gamma = getDoubleValue("Gamma");
    double gainRaw = getDoubleValue("GainRaw");
    auto balanceRatio = getBalanceRatio();

    return std::make_unique<HuarayParams>(
        CameraParams::noUse, m_cameraIndex + 1, mode, width, height, blackLevel, brightness, digitalShift,
        sharpness, exposureTime, gamma, gainRaw, balanceRatio, "", HuarayParams::noUse, HuarayParams::noUse);
}

/**
 * @brief 设备打开状态查询
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::isOpen() {
    if (m_devHandle == NULL) {
        return false;
    }
    return IMV_IsOpen(m_devHandle);
}

/**
 * @brief 设备抓图状态查询
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::isGrabbing() {
    if (m_devHandle == NULL) {
        return false;
    }
    return IMV_IsGrabbing(m_devHandle);
}

/**
 * @brief 设备实时抓图帧率查询
 * @return double
 * @author BG2EDG (928330305@qq.com)
 */
double Huaray::fps() {
    if (!isGrabbing()) {
        std::cerr << "Get fps failed! It's not grabbing!" << std::endl;
        return 0;
    }
    IMV_StreamStatisticsInfo streamStatsInfo;
    int ret = IMV_GetStatisticsInfo(m_devHandle, &streamStatsInfo);
    if (IMV_OK != ret) {
        std::cerr << "Get fps failed! ErrorCode[" << ret << "]";
        return 0;
    }
    return (streamStatsInfo).u3vStatisticsInfo.fps;
}

/**
 * @brief 设备流统计信息查询
 * @return Huaray::StreamStat
 * @author BG2EDG (928330305@qq.com)
 */
Huaray::StreamStat Huaray::stat() {
    if (!isGrabbing()) {
        std::cerr << "Get stat failed! It's not grabbing!" << std::endl;
        return std::make_tuple((uint)0, (uint)0, (uint)0, 0.0, 0.0);
    }
    IMV_StreamStatisticsInfo streamStatsInfo;
    int ret = IMV_GetStatisticsInfo(m_devHandle, &streamStatsInfo);
    if (IMV_OK != ret) {
        std::cerr << "Get fps failed! ErrorCode[" << ret << "]";
        return std::make_tuple((uint)0, (uint)0, (uint)0, 0.0, 0.0);
    }
    // 图像错误的帧数
    uint imageError = streamStatsInfo.u3vStatisticsInfo.imageError;
    // 丢包的帧数
    uint lostPacketBlock = streamStatsInfo.u3vStatisticsInfo.lostPacketBlock;
    // 正常获取的帧数
    uint imageReceived = streamStatsInfo.u3vStatisticsInfo.imageReceived;
    // 帧率
    double fps = streamStatsInfo.u3vStatisticsInfo.fps;
    // 带宽(Mbps)
    double bandwidth = streamStatsInfo.u3vStatisticsInfo.bandwidth;

    return std::make_tuple(imageError, lostPacketBlock, imageReceived, fps, bandwidth);
}

/**
 * @brief 重置设备流统计信息
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::resetStat() {
    if (!isGrabbing()) {
        std::cerr << "Reset stat failed! It's not grabbing!" << std::endl;
        return 0;
    }
    int ret = IMV_ResetStatisticsInfo(m_devHandle);
    if (IMV_OK != ret) {
        std::cerr << "Reset stat failed! ErrorCode[" << ret << "]";
        return false;
    }
    return true;
}

/**
 * @brief 用于多线程取图
 * @return std::tuple<bool, TimePoint, std::unique_ptr<cv::Mat>>
 * @author BG2EDG (928330305@qq.com)
 */
std::tuple<bool, TimePoint, std::unique_ptr<cv::Mat>> Huaray::getFrameImage() {
    auto timeStamp = std::chrono::steady_clock::now();

    // 获取一帧图像
    if (!isGrabbing()) {
        return std::make_tuple(false, timeStamp, nullptr);
    }
    m_ret = IMV_GetFrame(m_devHandle, &m_frame, TIMEOUT_MS);

    if (IMV_OK != m_ret) {
        printf("Get frame failed! ErrorCode[%d]\n", m_ret);
        return std::make_tuple(false, timeStamp, nullptr);
    }
    // if (m_devHandle == NULL) {
    //     printf("devHandle is NULL\n");
    //     return std::make_tuple(false, timeStamp, nullptr);
    // }

    auto framePair = cvtMatBGR8(m_frame, nullptr);

    releaseFrame();

    return std::make_tuple(framePair.first, timeStamp, move(framePair.second));
}

/**
 * @brief 获取帧触发数
 * @return int64_t
 * @author BG2EDG (928330305@qq.com)
 */
int64_t Huaray::getTriggerCnt() {
    if (m_devHandle == NULL) {
        return 0;
    }
    int64_t frameTriggerCount = 0;
    int ret = IMV_GetIntFeatureValue(m_devHandle, "FrameTriggerCount", &frameTriggerCount);
    if (IMV_OK != ret) {
        printf("Get FrameTriggerCount value failed! ErrorCode[%d]\n", ret);
        return 0;
    }
    return frameTriggerCount;
}

/**
 * @brief 获取帧触发丢失数
 * @return int64_t
 * @author BG2EDG (928330305@qq.com)
 */
int64_t Huaray::getTriggerLost() {
    if (m_devHandle == NULL) {
        return 0;
    }
    int64_t frameTriggerLostCount = 0;
    int ret = IMV_GetIntFeatureValue(m_devHandle, "FrameTriggerLostCount", &frameTriggerLostCount);
    if (IMV_OK != ret) {
        printf("Get FrameTriggerLostCount value failed! ErrorCode[%d]\n", ret);
        return 0;
    }
    return frameTriggerLostCount;
}

/**
 * @brief 触发计数清0
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::resetTriggerCnt() {
    if (m_devHandle == NULL) {
        return false;
    }
    int ret = IMV_ExecuteCommandFeature(m_devHandle, "FrameTriggerCountReset");
    if (IMV_OK != ret) {
        printf("Execute FrameTriggerCountReset failed! ErrorCode[%d]\n", ret);
        return false;
    }
    return true;
}

Huaray::~Huaray() { terminate(); }

// ============================== Huaray Drivers ==============================
// Implementations

/**
 * @brief 查找所有相机设备
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::findDevices() {
    // IMV_DeviceList deviceInfoList;
    m_ret = IMV_EnumDevices(&m_devList, interfaceTypeAll);
    if (m_ret != IMV_OK) {
        printf("Enumeration devices failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    if (m_devList.nDevNum < 1) {
        printf("no camera\n");
        return false;
    }

    // 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址）
    // Print camera info (Index, Type, Vendor, Model, Serial number,
    // DeviceUserID, IP Address)
    displayDeviceInfo(m_devList);
    return true;
};

/**
 * @brief 打开指定设备，默认打开第1个
 * @param[in] id 设备编号，大华打印输出信息是从1开始的，此处遵循此原则
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::open(const uint id) {
    m_cameraIndex = id == 0 ? getFirstHuarayID() : id - 1;
    if (m_cameraIndex == 4294967295) {
        return false;
    }

    // 创建设备句柄
    // Create Device Handle
    m_ret = IMV_CreateHandle(&m_devHandle, modeByIndex, (void*)&m_cameraIndex);
    if (IMV_OK != m_ret) {
        printf("Create devHandle failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    // 打开相机
    // Open camera
    m_ret = IMV_Open(m_devHandle);
    if (IMV_OK != m_ret) {
        printf("Open camera failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 关闭设备
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::close() {
    if (!isOpen()) {
        return true;
    }
    m_ret = IMV_Close(m_devHandle);
    if (IMV_OK != m_ret) {
        printf("Close camera failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    return true;
}

/**
 * @brief 启动图像采集
 * @param[in] cameraParams  My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::start(const CameraParams& cameraParams) {
    // 不同模式可能开启抓图的操作不同，但暂时是一致的
    return startGrabbing();
}

/**
 * @brief 停止图像采集
 * @param[in] cameraParams  My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::stop(const CameraParams& cameraParams) {
    // 不同模式可能停止抓图的操作不同，但暂时是一致的
    return stopGrabbing();
}

/**
 * @brief 相机设置
 * @param[in] cameraParams  My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::set(const HuarayParams& cameraParams) {
    if (m_devHandle == NULL) {
        return false;
    }
    // 设置成像参数
    setParams(cameraParams);
    return setOperation(cameraParams);
}

/**
 * @brief 释放图像缓存
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::releaseFrame() {
    m_ret = IMV_ReleaseFrame(m_devHandle, &m_frame);
    if (IMV_OK != m_ret) {
        printf("Release frame failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 断线后重连相机，会重开相机并设置参数
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::retry() {
    int k = 0;
    int ret = IMV_OK;

    // 关闭相机
    IMV_Close(m_devHandle);

    do {
        ret = IMV_Open(m_devHandle);
        if (IMV_OK != ret) {
            printf("Retry open camera failed! ErrorCode[%d]\n", ret);
        } else {
            printf("Retry open camera successfully!\n");
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        k++;

    } while (k < MAX_RETRY_TIMES);

    return reset(std::get<1>(*m_userDataPtr));
}

/**
 * @brief 销毁设备句柄
 * @author BG2EDG (928330305@qq.com)
 */
void Huaray::destroy() {
    if (m_devHandle != NULL) {
        IMV_DestroyHandle(m_devHandle);
        m_devHandle = NULL;
    }
}

/**
 * @brief 数据帧回调函数
 * @param[in] pFrame        数据帧的指针
 * @param[in] pUser         用户数据的指针
 * @author BG2EDG (928330305@qq.com)
 */

void Huaray::onGetFrame(IMV_Frame* pFrame, void* pUser) {
    auto timeStamp = std::chrono::steady_clock::now();

    if (pFrame == NULL) {
        printf("pFrame is NULL\n");
        return;
    }
    UserData* pOnCalllData = (UserData*)pUser;
    std::get<1>(*pOnCalllData).onGet()();

    // auto devHandle = std::get<0>(*pOnCalllData);
    // if (devHandle == NULL) {
    //     printf("devHandle is NULL\n");
    //     return;
    // }

    // 转成BGR8格式的cv::Mat
    auto framePair = cvtMatBGR8(*pFrame, nullptr);
    if (framePair.first) {
        std::get<1>(*pOnCalllData).onCall()(timeStamp, *framePair.second);
    }

    return;
}

/**
 * @brief 注册数据帧回调函数
 * @param[in] pUserData   My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::attachGrabbing(UserData* pUserData) {
    m_ret = IMV_AttachGrabbing(m_devHandle, onGetFrame, pUserData);
    if (IMV_OK != m_ret) {
        printf("Attach grabbing failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 开始拉流
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::startGrabbing() {
    m_ret = IMV_StartGrabbing(m_devHandle);
    if (IMV_OK != m_ret) {
        printf("Start grabbing failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 停止拉流
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::stopGrabbing() {
    if (!isGrabbing()) {
        return true;
    }
    m_ret = IMV_StopGrabbing(m_devHandle);
    if (IMV_OK != m_ret) {
        printf("Stop grabbing failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 设备连接状态事件回调函数
 * @param[in] pUserData 回调时传入的用户数据
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::attachConnection(UserData* pUserData) {
    m_ret = IMV_SubscribeConnectArg(m_devHandle, onDeviceLinkNotify, pUserData);
    if (IMV_OK != m_ret) {
        printf("SubscribeConnectArg failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 连接事件通知回调函数
 * @param[in] pConnectArg   连接事件信息
 * @param[in] pUser         用户数据
 * @author BG2EDG (928330305@qq.com)
 */
void Huaray::onDeviceLinkNotify(const IMV_SConnectArg* pConnectArg, void* pUser) {
    int ret = IMV_OK;
    IMV_DeviceInfo devInfo;
    UserData* pUserData = (UserData*)pUser;
    auto devHandle = std::get<0>(*pUserData);

    if (NULL == devHandle) {
        printf("devHandle is NULL!");
        return;
    }

    memset(&devInfo, 0, sizeof(devInfo));
    ret = IMV_GetDeviceInfo(devHandle, &devInfo);
    if (IMV_OK != ret) {
        printf("Get device info failed! ErrorCode[%d]\n", ret);
        return;
    }

    // 断线通知
    if (offLine == pConnectArg->event) {
        printf("------cameraKey[%s] : OffLine------\n", devInfo.cameraKey);
        Huaray::deviceOffLine(*pUserData);
    }
    // 上线通知
    else if (onLine == pConnectArg->event) {
        printf("------cameraKey[%s] : OnLine------\n", devInfo.cameraKey);
        Huaray::deviceOnLine(*pUserData);
    }
}

/**
 * @brief 断线通知处理，先停止拉流，然后执行用户定义的断连函数
 * @param[in] onConnectData 用户数据
 * @author BG2EDG (928330305@qq.com)
 */
void Huaray::deviceOffLine(const UserData& onConnectData) {
    auto devHandle = std::get<0>(onConnectData);
    IMV_StopGrabbing(devHandle);
    std::get<1>(onConnectData).offLineFunc()();
}

/**
 * @brief 上线通知处理,先调用用户定义的函数，然后实施重连尝试
 * @param[in] onConnectData 用户数据
 * @author BG2EDG (928330305@qq.com)
 */
void Huaray::deviceOnLine(const UserData& onConnectData) {
    std::get<1>(onConnectData).onLineFunc()();
    std::get<2>(onConnectData)();
}

/**
 * @brief OpenCV实现：图像格式转换为BGR8，以cv::Mat存储
 * @param[in] frame         图像帧
 * @param[in] devHandle     设备句柄，实际没用，为了和sdk版的接口统一
 * @return std::pair<bool, std::unique_ptr<cv::Mat>>
 * @author BG2EDG (928330305@qq.com)
 */
std::pair<bool, std::unique_ptr<cv::Mat>> Huaray::cvtMatBGR8(const IMV_Frame& frame,
                                                             const IMV_HANDLE devHandle) {
    // 用数据指针建Mat，不发生数据拷贝
    cv::Mat temp = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, CV_8UC1, (uint8_t*)frame.pData);
    cv::Mat frame_temp(temp.rows, temp.cols, CV_8UC3);
    cv::cvtColor(temp, frame_temp, cv::COLOR_BayerBG2BGR_EA);
    return std::make_pair(true, std::make_unique<cv::Mat>(frame_temp));
}

/**
 * @brief sdk实现：图像格式转换为BGR8，以cv::Mat存储
 * @param[in] frame         图像帧
 * @param[in] devHandle     设备句柄
 * @return std::pair<bool, std::unique_ptr<cv::Mat>>
 * @author BG2EDG (928330305@qq.com)
 * @attention 当前版本的效果4～8ms且不稳定，后续如有更新要进行对比
 */
std::pair<bool, std::unique_ptr<cv::Mat>> Huaray::sdkCvtMatBGR8(const IMV_Frame& frame,
                                                                const IMV_HANDLE devHandle) {
    int ret = IMV_OK;
    const IMV_EPixelType convertFormat = gvspPixelBGR8;
    IMV_PixelConvertParam stPixelConvertParam;
    unsigned char* pDstBuf = NULL;
    unsigned int nDstBufSize = 0;
    const char* pConvertFormatStr = NULL;

    nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
    pConvertFormatStr = (const char*)"BGR8";
    pDstBuf = (unsigned char*)malloc(nDstBufSize);
    cv::Mat frameImage(frame.frameInfo.height, frame.frameInfo.width, CV_8UC3, (uint8_t*)pDstBuf);

    if (NULL == pDstBuf) {
        printf("malloc pDstBuf failed!\n");
        return std::make_pair(false, nullptr);
    }

    // 图像转换成BGR8
    // convert image to BGR8
    memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
    stPixelConvertParam.nWidth = frame.frameInfo.width;
    stPixelConvertParam.nHeight = frame.frameInfo.height;
    stPixelConvertParam.ePixelFormat = frame.frameInfo.pixelFormat;
    stPixelConvertParam.pSrcData = frame.pData;
    stPixelConvertParam.nSrcDataLen = frame.frameInfo.size;
    stPixelConvertParam.nPaddingX = frame.frameInfo.paddingX;
    stPixelConvertParam.nPaddingY = frame.frameInfo.paddingY;
    stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
    stPixelConvertParam.eDstPixelFormat = convertFormat;
    stPixelConvertParam.pDstBuf = pDstBuf;
    stPixelConvertParam.nDstBufSize = nDstBufSize;

    ret = IMV_PixelConvert(devHandle, &stPixelConvertParam);

    if (IMV_OK == ret) {
        // printf("image convert to %s successfully! nDstDataLen (%u)\n",
        //        pConvertFormatStr, stPixelConvertParam.nDstBufSize);
        return std::make_pair(true, std::make_unique<cv::Mat>(frameImage));
    } else {
        printf("image convert to %s failed! ErrorCode[%d]\n", pConvertFormatStr, ret);
        return std::make_pair(false, nullptr);
    }
}

/**
 * @brief 保存相机属性配置文件，存储格式.mvcfg，书写格式xml
 * @param[in] path          存储路径,绝对和相对均可
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::saveMVCFG(const std::string& path) {
    std::cout << "Start save camera configuration to: " << path << std::endl;
    m_ret = IMV_SaveDeviceCfg(m_devHandle, path.c_str());
    if (IMV_OK != m_ret) {
        printf("Save camera configuration fail! ErrorCode[%d]\n", m_ret);
        return false;
    }
    std::cout << "Finish save camera configuration to: " << path << std::endl;
    return true;
}

/**
 * @brief 加载相机属性配置文件
 * @param[in] path          加载路径，绝对和相对均可
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::loadMVCFG(const std::string& path) {
    std::cout << "Start load camera configuration from: " << path << std::endl;
    // Load configuration of the device
    IMV_ErrorList errorList;
    memset(&errorList, 0, sizeof(IMV_ErrorList));
    int ret = IMV_LoadDeviceCfg(m_devHandle, path.c_str(), &errorList);
    if (IMV_OK != ret) {
        printf("Load camera configuration fail! ErrorCode[%d]\n", ret);
        return false;
    }
    for (uint i = 0; i < errorList.nParamCnt; i++) {
        printf("Error paramName [%s]\n", errorList.paramNameList[i].str);
    }
    std::cout << "Finish load camera configuration from: " << path << std::endl;
    return true;
}

/**
 * @brief 设置成像参数
 * @param[in] cameraParams  相机参数
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::setParams(const HuarayParams& cameraParams) {
    if (cameraParams.paramSrc() == PARAM) {
        bool result = true;
        // 设置成像参数
        result = setWidth(cameraParams.width()) && result;
        result = setHeight(cameraParams.height()) && result;
        result = setBlackLevel(cameraParams.blackLevel()) && result;
        result = setBrightness(cameraParams.brightness()) && result;
        result = setDigitalShift(cameraParams.digitalShift()) && result;
        result = setSharpness(cameraParams.sharpness()) && result;
        result = setExposureTime(cameraParams.exposureTime()) && result;
        result = setGamma(cameraParams.gamma()) && result;
        result = setGainRaw(cameraParams.gainRaw()) && result;
        result = setBalanceRatio(cameraParams.balanceRatio()) && result;
        return result;
    } else {
        return loadMVCFG(cameraParams.path());
    }
}

/**
 * @brief 设置参数精简版，设置常用参数，只支持从cameraParams中设置
 * @param[in] cameraParams  相机参数
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::setParamsLite(const HuarayParams& cameraParams) {
    if (cameraParams.paramSrc() == PARAM) {
        bool result = true;
        // 设置成像参数
        result = setBlackLevel(cameraParams.blackLevel()) && result;
        result = setBrightness(cameraParams.brightness()) && result;
        result = setDigitalShift(cameraParams.digitalShift()) && result;
        result = setSharpness(cameraParams.sharpness()) && result;
        result = setExposureTime(cameraParams.exposureTime()) && result;
        result = setGamma(cameraParams.gamma()) && result;
        result = setGainRaw(cameraParams.gainRaw()) && result;
        result = setBalanceRatio(cameraParams.balanceRatio()) && result;
        return result;
    } else {
        return false;
    }
}

/**
 * @brief 设置回调等相机操作
 * @param[in] cameraParams  My Param doc
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::setOperation(const HuarayParams& cameraParams) {
    // 重连回调接收数据
    m_userDataPtr = std::make_unique<UserData>(m_devHandle, cameraParams, boost::bind(&Huaray::retry, this));

    // 测试Buffer Size，改成1会出问题，没有对此值修改
    // if (setBufferSize(BUFFER_COUNT)) {
    //     std::cout << "Huaray Buffer Size: " << BUFFER_COUNT << std::endl;
    // }

    // 清空缓冲区
    clearFrameBuffer();

    switch (cameraParams.mode()) {
        case Mode::STREAM:
            if (!setContinuous()) {
                return false;
            }
            // 注册回调
            if (!attachGrabbing(m_userDataPtr.get())) {
                return false;
            }
            // 注册重连
            if (!attachConnection(m_userDataPtr.get())) {
                return false;
            }
            break;
        case Mode::STREAM_MULTITHREAD:
            // 外部创建线程取图，这里啥也不干
            break;
        case Mode::LINE:
            // 设置外部触发
            if (!setLineTrigger()) {
                return false;
            }
            // 注册回调
            if (!attachGrabbing(m_userDataPtr.get())) {
                return false;
            }
            // 注册重连
            if (!attachConnection(m_userDataPtr.get())) {
                return false;
            }
            break;
        case Mode::SOFT:
            // TODO 增加软触发
            // 注册回调
            if (!attachGrabbing(m_userDataPtr.get())) {
                return false;
            }
            break;
    }
    return true;
}

/**
 * @brief 设置连续抓图
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::setContinuous() {
    // 设置触发器
    // Set trigger selector to FrameStart
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSelector", "FrameStart");
    if (IMV_OK != m_ret) {
        printf("Set triggerSelector value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    // 关闭触发模式
    // Set trigger mode to Off
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerMode", "Off");
    if (IMV_OK != m_ret) {
        printf("Set triggerMode value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }
    return true;
}

/**
 * @brief 设置外部触发模式
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::setLineTrigger() {
    // 设置触发源为外部触发
    // Set trigger source to Line1
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSource", "Line1");
    if (IMV_OK != m_ret) {
        printf("Set triggerSource value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    // 设置触发器
    // Set trigger selector to FrameStart
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSelector", "FrameStart");
    if (IMV_OK != m_ret) {
        printf("Set triggerSelector value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    // 设置触发模式
    // Set trigger mode to On
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerMode", "On");
    if (IMV_OK != m_ret) {
        printf("Set triggerMode value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    // 设置外触发为:上升沿RisingEdge/下降沿FallingEdge
    // Set trigger activation to RisingEdge(FallingEdge in opposite)
    m_ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerActivation", TRIGGER_EDGE.c_str());
    if (IMV_OK != m_ret) {
        printf("Set triggerActivation value failed! ErrorCode[%d]\n", m_ret);
        return false;
    }

    // 触发计数清0
    // Clear trigger count
    m_ret = IMV_ExecuteCommandFeature(m_devHandle, "FrameTriggerCountReset");
    if (IMV_OK != m_ret) {
        printf("Execute FrameTriggerCountReset failed! ErrorCode[%d]\n", m_ret);
        // 清0失败应该还能跑，所以没return false
    }

    return true;
}

/**
 * @brief 清除帧数据缓存
 * @return true
 * @return false
 * @author BG2EDG (928330305@qq.com)
 */
bool Huaray::clearFrameBuffer() {
    if (m_devHandle == NULL) {
        return false;
    }
    int ret = IMV_ClearFrameBuffer(m_devHandle);
    if (IMV_OK != ret) {
        printf("Clear frame buffer failed! ErrorCode[%d]\n", ret);
        return false;
    }
    return true;
}

// setters&getters：修改相机参数或向相机查询参数
bool Huaray::setWidth(const int value) { return setIntValue("Width", value); }
bool Huaray::setHeight(const int value) { return setIntValue("Height", value); }
bool Huaray::setBlackLevel(const int value) {
    if (!setEnumSymbol("BlackLevelAuto", "Off")) {
        return false;
    }
    return setIntValue("BlackLevel", value);
}
bool Huaray::setBrightness(const int value) { return setIntValue("Brightness", value); }
bool Huaray::setDigitalShift(const int value) { return setIntValue("DigitalShift", value); }
bool Huaray::setSharpness(const int value) {
    if (!setEnumSymbol("SharpnessEnabled", "On")) {
        return false;
    }
    return setIntValue("Sharpness", value);
}

bool Huaray::setExposureTime(const double value) { return setDoubleValue("ExposureTime", value); }
bool Huaray::setGamma(const double value) { return setDoubleValue("Gamma", value); }
bool Huaray::setGainRaw(const double value) { return setDoubleValue("GainRaw", value); }
bool Huaray::setBalanceRatio(const std::vector<double>& values) {
    int ret = IMV_OK;
    bool result = true;

    if (!setEnumSymbol("BalanceWhiteAuto", "Off")) {
        return false;
    }

    std::string selectorName = "BalanceRatioSelector";
    std::string valueName = "BalanceRatio";
    for (uint64_t i = 0; i < 3; i++) {
        ret = IMV_SetEnumFeatureValue(m_devHandle, selectorName.c_str(), i);
        if (IMV_OK != ret) {
            std::cerr << "Set " << valueName << " " << i << " failed! ErrorCode[" << ret << "]" << std::endl;
            continue;
        }
        result = result && setDoubleValue(valueName, values[i]);
    }

    return result;
}

Mode Huaray::getMode() {
    if ("Off" == getEnumSymbol("TriggerMode")) {
        return Mode::STREAM;
    } else if ("On" == getEnumSymbol("TriggerMode")) {
        if ("Line1" == getEnumSymbol("TriggerSource")) {
            return Mode::LINE;
        } else if ("Software" == getEnumSymbol("TriggerSource")) {
            return Mode::SOFT;
        } else {
            return Mode::UNKNOWN;
        }
    }
    return Mode::UNKNOWN;
}

bool Huaray::setBufferSize(uint size) {
    if (size < 1) {
        size = 1;
    } else if (size > 32) {
        size = 32;
    }
    int ret = IMV_SetBufferCount(m_devHandle, size);
    if (IMV_OK != ret) {
        std::cerr << "Set Buffer Size failed! ErrorCode[" << ret << "]" << std::endl;
        return false;
    }
    return true;
}

std::vector<double> Huaray::getBalanceRatio() {
    int ret = IMV_OK;
    std::vector<double> balanceRatio;
    balanceRatio.reserve(3);
    std::string selectorName = "BalanceRatioSelector";
    std::string valueName = "BalanceRatio";
    for (uint64_t i = 0; i < 3; i++) {
        ret = IMV_SetEnumFeatureValue(m_devHandle, selectorName.c_str(), i);
        if (IMV_OK != ret) {
            std::cerr << "Get " << valueName << " " << i << " failed! ErrorCode[" << ret << "]" << std::endl;
            balanceRatio.emplace_back(NULL);
            continue;
        }
        balanceRatio.emplace_back(getDoubleValue(valueName));
    }

    return balanceRatio;
}

bool Huaray::setBoolValue(const std::string& featureName, const bool value) {
    int ret = IMV_OK;
    ret = IMV_SetBoolFeatureValue(m_devHandle, featureName.c_str(), value);
    if (IMV_OK != ret) {
        std::cerr << "Set " << featureName << "'s (bool) value failed! ErrorCode[" << ret << "]" << std::endl;
        return false;
    }
    return true;
}

bool Huaray::getBoolValue(const std::string& featureName) {
    int ret = IMV_OK;
    bool value;
    ret = IMV_GetBoolFeatureValue(m_devHandle, featureName.c_str(), &value);
    if (IMV_OK != ret) {
        std::cerr << "Get " << featureName << "'s (bool) value failed! ErrorCode[" << ret << "]" << std::endl;
        value = NULL;
    }
    return value;
}

bool Huaray::setIntValue(const std::string& featureName, const int value) {
    int ret = IMV_OK;
    int64_t setValue = value;
    ret = IMV_SetIntFeatureValue(m_devHandle, featureName.c_str(), setValue);
    if (IMV_OK != ret) {
        std::cerr << "Set " << featureName << "'s (int) value failed! ErrorCode[" << ret << "]" << std::endl;
        return false;
    }
    return true;
}

int Huaray::getIntValue(const std::string& featureName) {
    int ret = IMV_OK;
    int64_t value;
    ret = IMV_GetIntFeatureValue(m_devHandle, featureName.c_str(), &value);
    if (IMV_OK != ret) {
        std::cerr << "Get " << featureName << "'s (int) value failed! ErrorCode[" << ret << "]" << std::endl;
        value = NULL;
    }
    return value;
}

bool Huaray::setDoubleValue(const std::string& featureName, const double value) {
    int ret = IMV_OK;
    ret = IMV_SetDoubleFeatureValue(m_devHandle, featureName.c_str(), value);
    if (IMV_OK != ret) {
        std::cerr << "Set " << featureName << "'s (double) value failed! ErrorCode[" << ret << "]"
                  << std::endl;
        return false;
    }
    return true;
}

double Huaray::getDoubleValue(const std::string& featureName) {
    int ret = IMV_OK;
    bool result = true;
    double value;
    ret = IMV_GetDoubleFeatureValue(m_devHandle, featureName.c_str(), &value);
    if (IMV_OK != ret) {
        std::cerr << "Get " << featureName << "'s (double) value failed! ErrorCode[" << ret << "]"
                  << std::endl;
        value = NULL;
    }
    return value;
}

bool Huaray::setEnumSymbol(const std::string& enumName, const std::string& enumValue) {
    int ret = IMV_SetEnumFeatureSymbol(m_devHandle, enumName.c_str(), enumValue.c_str());

    if (IMV_OK != ret) {
        std::cerr << "Set " << enumName << "'s (Enum) value failed! ErrorCode[" << ret << "]" << std::endl;
        return false;
    }
    return true;
}

std::string Huaray::getEnumSymbol(const std::string& enumName) {
    IMV_String value;
    int ret = IMV_GetEnumFeatureSymbol(m_devHandle, enumName.c_str(), &value);
    if (IMV_OK != ret) {
        return NULL;
    }
    std::string val = value.str;
    return val;
}

// *********** 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址）
// Print camera info (Index, Type, Vendor, Model, Serial number,
// DeviceUserID, IP Address)
void Huaray::displayDeviceInfo(IMV_DeviceList deviceInfoList) {
    IMV_DeviceInfo* pDevInfo = NULL;
    unsigned int cameraIndex = 0;
    char vendorNameCat[11];
    char cameraNameCat[16];

    // 打印Title行
    // Print title line
    printf(
        "\nIdx Type Vendor     Model      S/N             DeviceUserID    "
        "IP "
        "Address    \n");
    printf(
        "------------------------------------------------------------------"
        "----"
        "--------\n");

    for (cameraIndex = 0; cameraIndex < deviceInfoList.nDevNum; cameraIndex++) {
        pDevInfo = &deviceInfoList.pDevInfo[cameraIndex];
        // 设备列表的相机索引  最大表示字数：3
        // Camera index in device list, display in 3 characters
        printf("%-3d", cameraIndex + 1);

        // 相机的设备类型（GigE，U3V，CL，PCIe）
        // Camera type
        switch (pDevInfo->nCameraType) {
            case typeGigeCamera:
                printf(" GigE");
                break;
            case typeU3vCamera:
                printf(" U3V ");
                break;
            case typeCLCamera:
                printf(" CL  ");
                break;
            case typePCIeCamera:
                printf(" PCIe");
                break;
            default:
                printf("     ");
                break;
        }

        // 制造商信息  最大表示字数：10
        // Camera vendor name, display in 10 characters
        if (strlen(pDevInfo->vendorName) > 10) {
            memcpy(vendorNameCat, pDevInfo->vendorName, 7);
            vendorNameCat[7] = '\0';
            strcat(vendorNameCat, "...");
            printf(" %-10.10s", vendorNameCat);
        } else {
            printf(" %-10.10s", pDevInfo->vendorName);
        }

        // 相机的型号信息 最大表示字数：10
        // Camera model name, display in 10 characters
        printf(" %-10.10s", pDevInfo->modelName);

        // 相机的序列号 最大表示字数：15
        // Camera serial number, display in 15 characters
        printf(" %-15.15s", pDevInfo->serialNumber);

        // 自定义用户ID 最大表示字数：15
        // Camera user id, display in 15 characters
        if (strlen(pDevInfo->cameraName) > 15) {
            memcpy(cameraNameCat, pDevInfo->cameraName, 12);
            cameraNameCat[12] = '\0';
            strcat(cameraNameCat, "...");
            printf(" %-15.15s", cameraNameCat);
        } else {
            printf(" %-15.15s", pDevInfo->cameraName);
        }

        // GigE相机时获取IP地址
        // IP address of GigE camera
        if (pDevInfo->nCameraType == typeGigeCamera) {
            printf(" %s", pDevInfo->DeviceSpecificInfo.gigeDeviceInfo.ipAddress);
        }

        printf("\n");
    }
    return;
}

/**
 * @brief 获取设备列表中第一个生产商为Dahua或Huaray的设备id
 * @return uint 设备id，从0开始
 * @author BG2EDG (928330305@qq.com)
 */
uint Huaray::getFirstHuarayID() {
    IMV_DeviceInfo* pDevInfo = NULL;

    for (uint cameraIndex = 0; cameraIndex < m_devList.nDevNum; cameraIndex++) {
        pDevInfo = &m_devList.pDevInfo[cameraIndex];

        std::string vName(&pDevInfo->vendorName[0], &pDevInfo->vendorName[strlen(pDevInfo->vendorName)]);
        if ("Dahua Technology" == vName || "Huaray Technology" == vName) {
            std::cout << "Auto Find DaHua/Huaray Idx: " << cameraIndex + 1 << std::endl;
            return cameraIndex;
        }
    }
    return (uint)4294967295;
}

/**
 * @brief 根据SN码获取对应相机的索引，如果未找到返回0
 * @author GL (1748462079@qq.com)
 * @param cameraParams 输入的相机参数，只需要其中的SN
 * @param deviceInfoList 设备列表
 * @return
 * @note 这里设计为cameraParams作为参数的原因是因为打开一款相机一般需要：
 * vendor(生产厂商)、product(产品id)和sn码指定，为了可扩展性使用完整的param传入
 */
uint Huaray::getSNIndex(const HuarayParams& cameraParams, IMV_DeviceList deviceInfoList) {
    IMV_DeviceInfo* pDevInfo = NULL;
    unsigned int cameraIndex = 0;
    for (cameraIndex = 0; cameraIndex < deviceInfoList.nDevNum; cameraIndex++) {
        pDevInfo = &deviceInfoList.pDevInfo[cameraIndex];
        // 设备列表的相机索引  最大表示字数：3
        // Camera index in device list, display in 3 characters
        if (pDevInfo->serialNumber == cameraParams.SN()) {
            return cameraIndex + 1;
        }
    }
    return (uint)0;
}

}  // namespace hitcrt::camera