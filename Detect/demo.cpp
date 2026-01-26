#include "HuarayCam.h"
#include "ArmorDetectorNN.h"
#include "ArmorBase.h"
#include <memory>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/stitching/warpers.hpp>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// Ros2 仿真
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/types.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

// 配置为你电脑上生成引擎的路径
#define modelpath "/home/fx/Detect/7.29.engine"
#define conf_thres 0.4
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
struct frameTime {
  cv::Mat m_frame;
  TimePoint m_time;
};
void SIGINTHandler(int sigNum) {
  std::cout << "\nInterrupt signal SIGINT received.\n";
  std::cout << "Close Files......" << std::endl;
  sleep(0.5); // 延时1s给各个线程时间关闭文件
  std::cout << "Finish!" << std::endl;
  // hitcrt::Param::sysLog.closeFile();
  sleep(0.5); // 休眠2s等待其他线程结束
  exit(sigNum);
}

template <typename T> class ThreadSafeQueue {
public:
  ThreadSafeQueue()
      : stopProcessing(false) {}

  void push(const T &value) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 如果队列满了，丢弃最旧的元素，或者根据需求进行处理
    if (queue_.size() >= max_size_) {
      queue_.pop(); // 这里使用丢弃最旧数据的策略
    }

    queue_.push(value);
    not_empty_condition_.notify_all(); // 通知消费者线程数据已更新
  }

  void pop(T &value) {
    std::unique_lock<std::mutex> lock(mutex_);

    // 如果队列为空，消费者线程会阻塞等待
    not_empty_condition_.wait(
        lock, [this] { return !queue_.empty() || stopProcessing.load(); });
    if (stopProcessing.load()) {
      return;
    }
    value = queue_.front();
    queue_.pop();
  }

  bool empty() {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  void stop() {
    stopProcessing = true;
    not_empty_condition_.notify_all();
  }

private:
  std::queue<T> queue_;
  size_t max_size_ =1;
  mutable std::mutex mutex_;
  std::condition_variable not_empty_condition_;
  std::atomic<bool> stopProcessing;
};

// 自定义的类
class RobotDemo {
   public:
    RobotDemo() {
        // 初始化装甲板检测器
        m_detector =
            std::make_shared<hitcrt::ArmorDetectorNN>(modelpath, conf_thres);
        initROS2();
    }
    ~RobotDemo() {
      m_queue.stop(); // 停止队列阻塞
      if (rclcpp::ok()) {
        rclcpp::shutdown();
      }
      if (m_ros2SpinThread.joinable()) {
        m_ros2SpinThread.join(); // 等待线程安全结束
      }
      cv::destroyAllWindows();
    }
    // 用于帧回调的成员函数
    void apply(const hitcrt::camera::TimePoint& timeStamp,
               const cv::Mat& frameImage) {
      cv::Mat image = frameImage.clone();
      // 创建帧对象用于检测
      hitcrt::Frame frame(image, timeStamp);
      // 创建接收信息（设置敌方颜色）
      hitcrt::RecvInfoBase recvInfo(0.0, 0.0, 0.0, 25.0, hitcrt::RED, true);

      // 创建ROI（使用全图）
      hitcrt::ROI roi;

      // 执行装甲板检测
      std::vector<hitcrt::Armor> armors;
      bool detected = m_detector->apply(frame, recvInfo, roi, armors);

      // 在图像上绘制检测结果
      if (detected) {
        drawArmors(image, armors);
      }
        
        // 显示时间信息
        std::stringstream ss;
        auto duration = timeStamp - startTime;
        ss << "TimeStamp: " << (double)duration.count() / std::nano::den;
        cv::putText(image, ss.str(), cv::Point2i(20, 40),
                    cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0));
        
        // 显示检测结果统计
        std::stringstream ss2;
        ss2 << "Armors detected: " << armors.size();
        cv::putText(image, ss2.str(), cv::Point2i(20, 120),
                    cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0));
        
        cv::imshow("Armor Detection", image);
        cv::waitKey(1);
    };
    
    void onGet() {
        RobotDemo::onGetTime = std::chrono::steady_clock::now();
    }
    
    static hitcrt::camera::TimePoint startTime;
    static hitcrt::camera::TimePoint onGetTime;

    // 绘制装甲板检测结果
    void drawArmors(cv::Mat& image, const std::vector<hitcrt::Armor>& armors) {
        for (size_t i = 0; i < armors.size(); ++i) {
            const auto& armor = armors[i];
            
            // 装甲板的四个角点
            std::vector<cv::Point2f> corners = {
                armor.m_topLeft,     // 左上
                armor.m_topRight,    // 右上  
                armor.m_bottomRight, // 右下
                armor.m_bottomLeft   // 左下
            };
            
            // 绘制装甲板边界框
            cv::Scalar color;
            if (armor.m_pattern == hitcrt::Pattern::UNKNOWN) {
                color = cv::Scalar(255, 255, 255);  // 白色表示未知
            } else {
                color = cv::Scalar(0, 255, 0);  // 绿色表示检测到的装甲板
            }
            
            // 绘制四边形边界
            for (int j = 0; j < 4; j++) {
                cv::line(image, corners[j], corners[(j + 1) % 4], color, 2);
            }
            
            // 绘制角点
            for (int j = 0; j < 4; j++) {
                cv::circle(image, corners[j], 4, cv::Scalar(0, 0, 255), -1);  // 红色圆点表示角点
                // 标注角点序号
                cv::putText(image, std::to_string(j), corners[j] + cv::Point2f(5, -5),
                           cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            }
            
            // 绘制中心点
            cv::circle(image, armor.m_centerUV, 4, cv::Scalar(255, 0, 0), -1);  // 蓝色中心点
            
            // 添加信息标签
            std::string label = "ID:" + std::to_string(armor.m_classID) + 
                               " C:" + std::to_string(armor.m_confidence).substr(0, 4);
            
            // 根据装甲板类型添加不同颜色
            cv::Scalar textColor;
            switch (armor.m_pattern) {
                case hitcrt::Pattern::HERO:
                    textColor = cv::Scalar(0, 165, 255);  // 橙色
                    label += " HERO";
                    break;
                case hitcrt::Pattern::INFANTRY_3:
                case hitcrt::Pattern::INFANTRY_4:
                    textColor = cv::Scalar(255, 255, 0);  // 青色
                    label += " INF";
                    break;
                case hitcrt::Pattern::SENTRY:
                    textColor = cv::Scalar(255, 0, 255);  // 紫色
                    label += " SENTRY";
                    break;
                case hitcrt::Pattern::ENGINEER:
                    textColor = cv::Scalar(0, 255, 255);  // 黄色
                    label += " ENG";
                    break;
                case hitcrt::Pattern::OUTPOST:
                    textColor = cv::Scalar(128, 128, 128);  // 灰色
                    label += " OUTPOST";
                    break;
                case hitcrt::Pattern::BASE:
                    textColor = cv::Scalar(255, 0, 0);  // 蓝色
                    label += " BASE";
                    break;
                default:
                    textColor = cv::Scalar(255, 255, 255);  // 白色
                    label += " UNKNOWN";
                    break;
            }
            
            // 在装甲板上方显示标签
            cv::Point2f textPosition = armor.m_topLeft;
            if (armor.m_topLeft.y < armor.m_bottomLeft.y) {
                textPosition.y -= 10;  // 如果左上角在上方，就在上方显示
            } else {
                textPosition.y += 20;  // 否则在下方显示
            }
            
            cv::putText(image, label, textPosition,
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 2);
            
            // 绘制左右中心点连线
            cv::line(image, armor.m_centerLeft, armor.m_centerRight, 
                    cv::Scalar(255, 255, 0), 1);  // 青色连线
        }
    }
    void initROS2() {
      // 检查ROS2是否已经初始化
      if (!rclcpp::ok()) {
        rclcpp::init(0, nullptr);
      }
      // 创建节点
      m_simulationImageNode =
          std::make_shared<rclcpp::Node>("hitcrtVisionNode");

      // 兼容Ros2ForUnity通信规则
      auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
      qos.best_effort();

      // 创建订阅者 - 图像话题
      m_simulationImageSub_ =
          m_simulationImageNode->create_subscription<sensor_msgs::msg::Image>(
              "/image_raw", qos,
              std::bind(&RobotDemo::ros2ImageCallback, this,
                        std::placeholders::_1));

      // 启动spin线程
      m_ros2SpinThread = std::thread([this]() {
        try {
          rclcpp::spin(m_simulationImageNode);
        } catch (const std::exception &e) {
          // 捕获潜在异常
        }
      });
    }

    void ros2ImageCallback(const sensor_msgs::msg::Image::ConstSharedPtr &msg) {

      try {

        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        TimePoint timePoint = std::chrono::steady_clock::now();

        // 压入队列

        m_queue.push(frameTime{cv_ptr->image.clone(), timePoint});

      } catch (cv_bridge::Exception &e) {

        return;
      }
    }
    void ros2SpinThread() { rclcpp::spin(m_simulationImageNode); }

    std::shared_ptr<hitcrt::ArmorDetectorNN> m_detector;
    std::shared_ptr<rclcpp::Node> m_simulationImageNode;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr m_simulationImageSub_;
    std::thread m_ros2SpinThread;
    std::mutex m_imageMutex;
    cv::Mat image;
    ThreadSafeQueue<frameTime> m_queue;
};

hitcrt::camera::TimePoint RobotDemo::startTime;
hitcrt::camera::TimePoint RobotDemo::onGetTime;

int main() {
  signal(SIGINT, SIGINTHandler); // 获取ctrl+C信号，也是回调机制

  rclcpp::init(0, nullptr);
  // 实例化对象
  
  RobotDemo robot;
  frameTime frameT;
  while (rclcpp::ok()) {
    robot.m_queue.pop(frameT);
    if (frameT.m_frame.empty())
      continue;

    robot.apply(frameT.m_time, frameT.m_frame);
  }
  

  if (rclcpp::ok()) {
    rclcpp::shutdown();
  }
    return 0;
}


