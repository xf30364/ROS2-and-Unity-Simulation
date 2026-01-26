/**
 * @file ArmorDetector.cpp
 * @brief
 * @author Wang-yicheng (wang-yicheng.992@qq.com)
 * @date 2024-12-12
 *
 * @copyright Copyright (C) 2025, HITCRT_VISION, all rights reserved.
 *
 * @par 修改⽇志:
 * <table>
 * <tr><th>Date <th>Author <th>Description
 * <tr><td>2024-12-12 <td>Wang-yicheng <td>
 * </table>
 */
#include "ArmorDetectorNN.h"

namespace hitcrt {

bool ArmorDetectorNN::apply(const Frame &frame, const RecvInfoBase &recvInfo, const ROI &roi, std::vector<Armor> &armors) {

    armors.clear();
    m_armors.clear();
    m_img = frame.image(); // 浅拷贝

    deploy::Image image(m_img.data, m_img.cols, m_img.rows);
    auto result = m_model->predict(image);

    if (result.num < 1) {
        return false;
    }

    // 确定敌方颜色类别范围
    int classStart = -1, classEnd = -1;
    if (recvInfo.enemyColor() == BLUE) {
        classStart = 0;
        classEnd = 8;
    } else if (recvInfo.enemyColor() == RED) {
        classStart = 9;
        classEnd = 17;
    } else {
        std::cerr << "Error: Invalid enemy color" << std::endl;
        return false;
    }

    for (int i = 0; i < result.num; ++i) {
        Armor armor;
        const auto& keypoints = result.kpts[i];

        // 几何信息填充
        armor.m_topLeft     = cv::Point2f(keypoints[0].x, keypoints[0].y);  // 左上
        armor.m_bottomLeft  = cv::Point2f(keypoints[1].x, keypoints[1].y);  // 左下
        armor.m_bottomRight = cv::Point2f(keypoints[2].x, keypoints[2].y);  // 右下
        armor.m_topRight    = cv::Point2f(keypoints[3].x, keypoints[3].y);  // 右上

        armor.m_centerLeft  = (armor.m_topLeft + armor.m_bottomLeft) * 0.5f;
        armor.m_centerRight = (armor.m_topRight + armor.m_bottomRight) * 0.5f;
        armor.m_centerUV    = (armor.m_centerLeft + armor.m_centerRight) * 0.5f;
        armor.m_height      = std::min(cv::norm(armor.m_topLeft - armor.m_bottomLeft), cv::norm(armor.m_topRight - armor.m_bottomRight));
        armor.m_width       = cv::norm(armor.m_centerLeft - armor.m_centerRight);

        armor.m_confidence  = result.scores[i];
        armor.m_classID     = result.classes[i];  // 记录类别编号
        armor.m_timeStamp   = frame.timeStamp();  // 设置时间戳

        // 映射 pattern 和 size ，不识别基地小装甲板和5号
        switch (armor.m_classID) {
            case 0:  case 9:  armor.m_pattern = Pattern::SENTRY;     armor.m_size = Size::SMALL; break;
            case 1:  case 10: armor.m_pattern = Pattern::HERO;       armor.m_size = Size::LARGE; break;
            case 2:  case 11: armor.m_pattern = Pattern::ENGINEER;   armor.m_size = Size::SMALL; break;
            case 3:  case 12: armor.m_pattern = Pattern::INFANTRY_3; armor.m_size = Size::SMALL; break;
            case 4:  case 13: armor.m_pattern = Pattern::INFANTRY_4; armor.m_size = Size::SMALL; break;
            // case 5:  case 14: armor.m_pattern = Pattern::INFANTRY_5; armor.m_size = Size::SMALL; break;
            case 6:  case 15: armor.m_pattern = Pattern::OUTPOST;    armor.m_size = Size::SMALL; break;
            // case 7:  case 16: armor.m_pattern = Pattern::BASE;       armor.m_size = Size::SMALL; break;
            case 8:  case 17: armor.m_pattern = Pattern::BASE;       armor.m_size = Size::LARGE; break;
            default: armor.m_pattern = Pattern::UNKNOWN;            armor.m_size = Size::SMALL; break;
        }

        m_armors.emplace_back(armor);  // 保存全部结果

        if (armor.m_classID >= classStart && armor.m_classID <= classEnd && Pattern::UNKNOWN != armor.m_pattern/* 不输出UNKNOW装甲板 */ && armor.m_confidence > m_conf) {
            armors.emplace_back(armor);  // 仅输出敌方目标
        }
    }

    size_t before = armors.size();
    filterDuplicateByClassIOU(armors, 0.9f);
    size_t after = armors.size();

    if(after - before != 0){
    std::cout << "Filtered duplicate armors: " << "before:"<< before<< " -  after: "<<after<<" removed." << std::endl;
    }

    return !armors.empty();  // 返回是否找到目标

}

void ArmorDetectorNN::warmup() {}

float ArmorDetectorNN::computeIOU(const cv::Rect& a, const cv::Rect& b) {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);

    int interArea = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    int unionArea = a.area() + b.area() - interArea;

    if (unionArea <= 0) return 0.0f;
    return static_cast<float>(interArea) / static_cast<float>(unionArea);
}

void ArmorDetectorNN::filterDuplicateByClassIOU(std::vector<Armor>& armors, float iou_thresh) {
    std::unordered_map<int, std::vector<Armor>> class_to_armors;

    // 按类别归类
    for (const auto& armor : armors) {
        class_to_armors[armor.m_classID].emplace_back(armor);
    }

    std::vector<Armor> final_result;

    for (auto& [cls, group] : class_to_armors) {
        // 按置信度排序（从高到低）
        std::sort(group.begin(), group.end(), [](const Armor& a, const Armor& b) {
            return a.m_confidence > b.m_confidence;
        });

        std::vector<bool> suppressed(group.size(), false);

        for (int i = 0; i < group.size(); ++i) {
            if (suppressed[i]) continue;
            final_result.push_back(group[i]);

            cv::Rect rect1 = cv::boundingRect(std::vector<cv::Point2f>{
                group[i].m_topLeft, group[i].m_topRight, group[i].m_bottomRight, group[i].m_bottomLeft
            });

            for (int j = i + 1; j < group.size(); ++j) {
                if (suppressed[j]) continue;

                cv::Rect rect2 = cv::boundingRect(std::vector<cv::Point2f>{
                    group[j].m_topLeft, group[j].m_topRight, group[j].m_bottomRight, group[j].m_bottomLeft
                });

                float iou = computeIOU(rect1, rect2);
                if (iou > iou_thresh) {
                    suppressed[j] = true;  // 抑制置信度低的
                }
            }
        }
    }

    armors.swap(final_result);
}


}  // namespace hitcrt