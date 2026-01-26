/**
 * @file Basic.h
 * @brief 基本数据类型定义
 * @author BG2EDG (928330305@qq.com)
 * @date 2022-04-20
 *
 * @copyright Copyright (C) 2022, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2022-04-20 <td>BG2EDG  <td>
* <tr><td>2024-12-17 <td>FangHengjie <td> 数字识别区分3 4 5 号装甲板的大小
* <tr><td>2025-4-10 <td>ChenShaohan <td> 追加打符所需枚举类型
 * </table>
 */

#pragma once

#include <boost/any.hpp>
#include <chrono>
#include <cmath>
#include <ctime>
#include <map>
#include <opencv2/opencv.hpp>

namespace hitcrt {
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
typedef enum Color  // 定义一个名字为color的类型
{ RED = 0,
  BLUE } Color;

/// 装甲板尺寸（大/小）
typedef enum Size  // 定义一个名字为size的类型
{ SMALL = 0,
  LARGE } Size;

typedef enum Pattern {
    UNKNOWN = 0,
    HERO,
    ENGINEER,
    INFANTRY_3,
    INFANTRY_4,
    INFANTRY_5,
    OUTPOST,
    SENTRY,
    BASE,
    INFANTRY_3_BALANCE, // 3 号大装甲
    INFANTRY_4_BALANCE, // 4 号大装甲
    INFANTRY_5_BALANCE  // 5 号大装甲
} Pattern;

/**
 * @brief
 * 把最常用的变量拿出来单独命名,其余的用boost::any用户指定.自己指定的效率有点低，想用多态实现但不太会
 * @author BG2EDG (928330305@qq.com)
 */
class RecvInfoBase {
    friend class TestRecvInfo;

   public:
    RecvInfoBase(const float pitch, const float yaw, const float roll, const float firingSpeed,
                 const Color enemyColor, const bool useVision);
    // void set(const float pitch, const float yaw, const float firingSpeed,
    //          const Color m_enemyColor, const bool useVision);

    const float pitch() const { return m_pitch; }
    const float yaw() const { return m_yaw; }
    const float roll() const { return m_roll; }
    const float firingSpeed() const { return m_firingSpeed; }
    const Color enemyColor() const { return m_enemyColor; }
    const bool useVision() const { return m_useVision; }

   private:
    const float m_pitch;
    const float m_yaw;
    const float m_roll;
    const float m_firingSpeed;
    const Color m_enemyColor;
    const bool m_useVision;
};

/**
 * @copyright Copyright (C) 2021, HITCRT_VISION, all rights reserved.
 * @brief Region Of Interst
 * @author BG2EDG (928330305@qq.com)
 */
class ROI {
   public:
    ROI() = default;
    ROI(const cv::Rect2i &rect) { m_rect = rect; }
    ROI(const int x, const int y, const int w, const int h) { m_rect = cv::Rect2i(x, y, w, h); }
    ROI(const ROI &roi) { m_rect = roi.rect(); }
    const int x() const { return m_rect.tl().x; }
    const int y() const { return m_rect.tl().y; }
    const cv::Point2i tl() const { return m_rect.tl(); }
    const cv::Point2i br() const { return m_rect.br(); }
    const cv::Rect2i &rect() const { return m_rect; }
    void set(const cv::Rect2i &rect) { m_rect = rect; };

    void refresh();

   private:
    cv::Rect2i m_rect;
};
}  // namespace hitcrt