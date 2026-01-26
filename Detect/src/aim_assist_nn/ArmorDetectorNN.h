#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

#include "ArmorBase.h"
#include "model.hpp"
#include "option.hpp"
#include "result.hpp"

#include "ArmorDetectorGeneral.h"

namespace hitcrt {
class ArmorDetectorNN : public ArmorDetectorGeneral {
   
   public:
    ArmorDetectorNN(const std::string& modelpath, const float conf_thres) 
        : m_modelpath(modelpath),  
          m_conf(conf_thres) {     
        loadModel();
        warmup();
    }

    void loadModel() {

        deploy::InferOption option;
        option.enableSwapRB();

        m_model = std::make_unique<deploy::PoseModel>(m_modelpath, option);
    }
    void warmup();

    virtual bool apply(const Frame &frame, const RecvInfoBase &recvInfo,
                       const ROI &roi, std::vector<Armor> &armors) override;

   private:
    cv::Mat m_img;
    const std::string m_modelpath;
    std::unique_ptr<deploy::PoseModel> m_model;
    float m_conf;
    std::vector<Armor> m_armors;  // 保存所有推理出的装甲板，不筛颜色
    std::vector<std::string> m_labels = {
        "BS", "B1", "B2", "B3", "B4", "B5", "BO", "BSB", "BLB",
        "RS", "R1", "R2", "R3", "R4", "R5", "RO", "RSB", "RLB",
        "OS", "O1", "O2", "O3", "O4", "O5", "OO", "OSB", "OLB"};

    void filterDuplicateByClassIOU(std::vector<Armor>& armors, float iou_thresh = 0.9f);
    float computeIOU(const cv::Rect& a, const cv::Rect& b);
};
}  // namespace hitcrt
