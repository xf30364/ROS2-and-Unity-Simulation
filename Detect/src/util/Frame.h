/**
 * @file Frame.h
 * @brief
 * @author BG2EDG (928330305@qq.com)
 * @date 2021-12-17
 *
 * @copyright Copyright (C) 2021, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2021-12-17 <td>BG2EDG  <td>
 * </table>
 */

#pragma once

// #include <sys/time.h>
#include <chrono>
#include <ctime>
#include <opencv2/opencv.hpp>

#include "Basic.h"

namespace hitcrt {
class Frame {
   public:
    Frame(const cv::Mat &img, const TimePoint &timeStamp);
    // void set(const cv::Mat &img, const double grab_time, const
    // std::shared_ptr<RecvInfo> &recvInfoPtr); void copy(const Frame &frame);
    bool empty() const;
    // setters
    void setUsed() { m_isUsed = true; }
    // getters
    bool isUsed() const;
    const cv::Mat &image() const;
    const TimePoint timeStamp() const;

   private:
    const cv::Mat m_img;          //原图
    const TimePoint m_timeStamp;  //抓图时间
    bool m_isUsed = false;
};

}  // namespace hitcrt