#pragma once
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

#include "ArmorBase.h"
#include "Frame.h"

namespace hitcrt {
class ArmorDetectorBase;
class ArmorAssemblerBase;
class BarDetectorBase;

class ArmorDetectorGeneral {
   public:
    virtual bool apply(const Frame &frame, const RecvInfoBase &recvInfo, const ROI &roi,
                       std::vector<Armor> &armors) = 0; 
};

}  // namespace hitcrt