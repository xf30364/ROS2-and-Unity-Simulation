/**
 * @file Frame.cpp
 * @brief
 * @author BG2EDG (928330305@qq.com)
 * @date 2021-12-18
 *
 * @copyright Copyright (C) 2021, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2021-12-18 <td>BG2EDG  <td>
 * </table>
 */
#include "Frame.h"

namespace hitcrt {
Frame::Frame(const cv::Mat &img, const TimePoint &timeStamp) : m_img(img), m_timeStamp(timeStamp) {
    // m_img = img.clone();
    // m_timeStamp = timeStamp;
    // m_recvInfoPtr = std::make_shared<RecvInfo>(recvInfoPtr);
    m_isUsed = false;
}
// void Frame::set(const cv::Mat &img, const double timeStamp, const
// std::shared_ptr<RecvInfo> &recvInfoPtr)
// {
//     m_img = img.clone();
//     m_timeStamp = timeStamp;
//     m_recvInfoPtr = recvInfoPtr;
//     m_isUsed = false;
// }

// void Frame::copy(const Frame &frame)
// {
//     m_img = frame.m_img.clone();
//     m_timeStamp = frame.m_timeStamp;
//     m_recvInfoPtr = frame.m_recvInfoPtr;
//     m_isUsed = frame.m_isUsed;
// }

bool Frame::empty() const { return m_img.empty(); }

bool Frame::isUsed() const { return m_isUsed; }

const cv::Mat &Frame::image() const { return m_img; }

const TimePoint Frame::timeStamp() const { return m_timeStamp; }
}  // namespace hitcrt