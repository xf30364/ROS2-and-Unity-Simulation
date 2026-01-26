/**
 * @file ArmorBase.h
 * @brief
 * @author LiuZhihao (2488288779@qq.com)
 * @date 2023-12-10
 *
 * @copyright Copyright (C) 2023, HITCRT_VISION, all rights reserved.
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Author  <th>Description
 * <tr><td>2023-12-10 <td>LiuZhihao  <td>
 * <tr><td>2024-12-17 <td>FangHengjie  <td> 给Armor加入了一些新的成员变量，用于yawToR优化的实现
 * </table>
 */

#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

#include "Basic.h"

namespace hitcrt {
/**
 * @brief 灯条
 * @author BG2EDG (928330305@qq.com)
 */
class LightBar {
   public:
    LightBar() {}
    ~LightBar() {}
    float m_length;
    float m_width;
    float m_contourArea;  // 轮廓面积
    float m_colorRatio;   // 颜色比例
    cv::Point2f m_directVec;
    cv::Rect m_boundingRect;
    cv::RotatedRect m_rotatedRect;
    cv::Point2f m_top;     // 上端点
    cv::Point2f m_bottom;  // 下端点
    cv::Point2f m_center;  // 中心点
    void setParam(const float length, const float width, const float contourArea, const float colorRatio,
                  const cv::Point2f directVec, const std::vector<cv::Point> &contour,
                  const std::vector<cv::Point2f> &insidePoints, const cv::RotatedRect rotatedRect,
                  const cv::Point2f top, const cv::Point2f bottom, const cv::Point2f center);
};

/**
 * @brief 装甲板
 * @author BG2EDG (928330305@qq.com)
 */
class Armor {
   public:
    // Raw from Detect (Both NN & Tradition)
    // 装甲板特征点
    cv::Point2f m_topLeft;
    cv::Point2f m_topRight;
    cv::Point2f m_centerLeft; // 左灯条像素平面中心 基本不用 
    cv::Point2f m_centerRight;// 右灯条像素平面中心 基本不用 
    cv::Point2f m_bottomLeft;
    cv::Point2f m_bottomRight;
    cv::Point2f m_centerUV;  // 中心点在图像中的位置
    cv::Point2f m_carCenterUV;
    cv::Point2f m_predictPosUV;

    // 模型推理得到的额外信息
   double m_confidence;
   int m_classID = -1;  // 推理结果的类别编号，-1 表示未赋值

    // 数字编号
    Pattern m_pattern = Pattern::UNKNOWN;
    // int m_pattern = 0;

    // Raw from recv/system
    TimePoint m_timeStamp;
    double m_time = 0;  /// 获取时间，等于抓图时间

    float m_currPitchRAD = 0.0, m_currYawRAD = 0.0, m_currRollRAD = 0.0;  // pithc&yaw当前值

    // Calculate
    float m_width;
    float m_height;
    float m_aspectRatio;                // 宽比高，与屏幕16:9,4:3等定义方式一致
    float m_product = 0;                // 灯条平行度
    Size m_size;                        // 定义一个TYPE型的变量
    double m_horizontalDistance = 0.0;  // 装甲到云台坐标系原点的水平面投影距离

    // Ttj
    float m_dislikeValue;          // 决策
    std::vector<float> m_weights;  // 轨迹匹配权重
    double m_lastR;                // 小陀螺时相邻装甲板数据
    double m_lastY;

    // 与图无关的矩阵换成Eigen矩阵，计算中间变量用double
    Eigen::MatrixXd m_rotVec, m_transVec;
    // 记录两个姿态解，用于后续选择出最有yawToR
    Eigen::MatrixXd m_rotVec1; // 第二个解的旋转向量
    double m_yawToC, m_yawToR, m_pitchToR, m_rollToR, m_yawToR1; // m_yawToR1是第二个解d
    std::vector<float> m_reprojectionErrorVector; // 记录两个解对应的重投影误差
    Eigen::MatrixXd m_pointCH, m_pointGH, m_pointG3, m_pointR3;
    Eigen::Matrix3d m_rotRoll, m_rotYaw, m_rotPitch;
    Eigen::MatrixXd m_R2G, m_G2C;

    Eigen::MatrixXd m_filtMatrix, m_filtMatrixCar;    // 滤波值
    Eigen::MatrixXd m_filtPos;                        // xyz滤波值，单位m
    cv::Point3d m_predictXYZ;                         // xyz预测值，单位m
    double m_calcPitchRAD = 0.0, m_calcYawRAD = 0.0;  // pitch&yaw计算值
    double m_filtPitchDEG = 0.0, m_filtYawDEG = 0.0;  // pitch&yaw滤波值
    double m_predPitchDEG = 0.0, m_predYawDEG = 0.0;  // pitch&yaw预测值
    double m_filtYawToR;
};

}  // namespace hitcrt
