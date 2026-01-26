/**
 * @file demo.cpp
 * @brief Huaray驱动测试程序
 * @author BG2EDG (928330305@qq.com)
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2022-04-16 <td>BG2EDG  <td>
 * <tr><td>2022-05-15 <td>BG2EDG  <td>Timepoint使用时加命名空间
 * <tr><td>2022-05-15 <td>BG2EDG  <td>onGet函数，进入回调后立即执行
 * </table>
 */
#include "HuarayCam.h"

// 自定义的类
class RobotDemo {
   public:
    // 用于帧回调的成员函数签名必须满足void(const int64_t timeStamp, const
    // cv::Mat& frameImage)
    // timsStamp记录的是从1970时间起的纳秒数，获取方法如下：
    //    auto now = std::chrono::system_clock::now();
    //    auto timeStamp = now.time_since_epoch().count();
    // frameImage是存储RGB8的cv::Mat
    void apply(const hitcrt::camera::TimePoint& timeStamp,
               const cv::Mat& frameImage) {
        cv::Mat image = frameImage.clone();
        std::stringstream ss, ss2;
        auto duration = timeStamp - startTime;
        ss << "TimeStamp: " << (double)duration.count() / std::nano::den;
        cv::putText(image, ss.str(), cv::Point2i(20, 40),
                    cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0));
        auto duration2 = timeStamp - onGetTime;
        ss2 << "Time between grab and onGet: "
            << (double)duration2.count() / std::nano::den;
        cv::putText(image, ss2.str(), cv::Point2i(20, 80),
                    cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0));
        cv::imshow("image", image);
        cv::waitKey(1);
    };
    void onGet() {
        RobotDemo::onGetTime = std::chrono::steady_clock::now();
        // std::cout << "onGet!" << std::endl;
    }
    static hitcrt::camera::TimePoint startTime;
    static hitcrt::camera::TimePoint onGetTime;
};

hitcrt::camera::TimePoint RobotDemo::startTime;
hitcrt::camera::TimePoint RobotDemo::onGetTime;

class ConnectFunctor {
   public:
    // 用于断线重连回调的成员函数签名必须满足void()
    // 相机断线主动调用该函数，建议在里面新开一个线程或以某种方式将断线信息发送给其他线程
    // 去监控相机的open或grab变量，时间过长自动退出程序
    void camOffLine() { std::cout << OFF_STR << " Line!" << std::endl; }
    // 相机重连会先调用该函数，然后调用驱动里的retry()函数重启相机，建议在里面与断线配合
    // 要保证retry成功，即确实isOpen或者isGrabbing，不然应该退出程序
    void camOnLine() { std::cout << ON_STR << " Line!" << std::endl; }

   private:
    // bind使用bind绑定成员函数和对象后，成员函数可以访问到指定对象的成员变量
    const std::string OFF_STR = "Off";
    const std::string ON_STR = "On";
};

void noUse(const hitcrt::camera::TimePoint& timeStamp,
           const cv::Mat& frameImage){};

int main() {
    // 实例化对象
    RobotDemo robot;
    ConnectFunctor func;
    RobotDemo::startTime = std::chrono::steady_clock::now();
    // 相机参数设置
    // 注意此处boost::bind的用法，将成员函数与对象绑定，待输入参数用占位符_1替代
    hitcrt::camera::HuarayParams camParams(
        boost::bind(&RobotDemo::apply, &robot, _1, _2), 1,
        hitcrt::camera::Mode::STREAM, 1280, 1024, 20, 60, 0, 70, 3000.0, 0.7,
        1.0, {1.0, 1.0, 1.0});
    // 参数设置还可通过set方法实现
    camParams.setOffLineFunc(boost::bind(&ConnectFunctor::camOffLine, &func));
    camParams.setOnLineFunc(boost::bind(&ConnectFunctor::camOnLine, &func));
    camParams.setOnGet(boost::bind(&RobotDemo::onGet, &robot));

    // 直接使用HuarayParams对象初始化Huaray对象，则立即执行相机初始化
    hitcrt::camera::Huaray cam(camParams);
    if(cam.isGrabbing()){
        std::cout << "Camera is grabbing!" << std::endl;
    }else{
        std::cout << "Camera is not grabbing!" << std::endl;
        return -1;
    }
    // 也可以先实例化对象，然后使用initiate执行初始化
    // hitcrt::camera::Huaray cam;
    // if(cam.initiate(camParams)){
    //     std::cout << "Camera initiate success!" << std::endl;
    // }else{
    //     std::cout << "Camera initiate failed!" << std::endl;
    //     return -1;
    // }

    // 向相机查询当前参数，返回包含相机信息的std::unique_ptr
    // 此处使用auto自动进行型别推导
    auto param = cam.get();
    // 向终端格式化输出参数
    (*param).show();

    // 阻塞主线程，等到键盘按下
    std::cout << "Press a key to reset params ..." << std::endl;
    getchar();

    // 快速参数重置
    // 前五个参数都不会修改
    hitcrt::camera::HuarayParams camParams3(
        boost::bind(&noUse, _1, _2), 1, hitcrt::camera::Mode::STREAM, 1280,
        1024, 20, 60, 0, 70, 10000.0, 0.7, 1.0, {1.0, 2.0, 1.0});
    cam.resetLite(camParams3);

    // 向相机查询当前参数，返回包含相机信息的std::unique_ptr
    // 此处使用auto自动进行型别推导
    param = cam.get();
    // 向终端格式化输出参数
    (*param).show();

    // 阻塞主线程，等到键盘按下
    std::cout << "Press a key to reset params ..." << std::endl;
    getchar();

    // 用.mvcfg设置相机参数
    std::string path = "config.mvcfg";
    // 保存相机参数
    // cam.saveConfig(path);

    // 用路径设置参数，函数别忘记绑定
    hitcrt::camera::HuarayParams camParams2(
        boost::bind(&RobotDemo::apply, &robot, _1, _2), 1,
        hitcrt::camera::Mode::STREAM, path,
        boost::bind(&ConnectFunctor::camOffLine, &func),
        boost::bind(&ConnectFunctor::camOnLine, &func));
    // 重置参数
    cam.reset(camParams2);

    // 向相机查询当前参数
    auto param2 = cam.get();
    (*param2).show();

    std::cout << "Press a key to continue ..." << std::endl;
    getchar();

    // 判断相机是否开启，如有其他程序占用也会返回false
    std::cout << "IsOpen: " << cam.isOpen() << std::endl;
    // 判断相机是否正常抓图
    std::cout << "IsGrabbing: " << cam.isGrabbing() << std::endl;

    // 硬件性能信息
    // 抓图帧率，不是回调帧率
    std::cout << "fps: " << cam.fps() << std::endl;
    // 流统计信息：图像错误的帧数，丢包的帧数，正常获取的帧数，帧率，带宽(Mbps)
    hitcrt::camera::Huaray::StreamStat stat = cam.stat();
    std::cout << "total before reset: " << std::get<2>(stat) << std::endl;
    // 重置统计信息
    cam.resetStat();
    std::cout << "total after reset: " << std::get<2>(cam.stat()) << std::endl;

    // 多线程取图，使用时另开线程,该部分未经过运行测试，使用前请测试功能是否正常
    auto frameImageTuple = cam.getFrameImage();
    // 硬件触发计数
    // auto cnt = cam.getTriggerCnt();
    // cam.resetTriggerCnt();

    std::cout << "Press a key to exit..." << std::endl;
    getchar();

    // 停止抓图，销毁相机句柄
    // 为防止用户忘记，在cam的析构函数里也设置了这一操作
    // 但最好加上
    cam.terminate();

    return 0;
}
