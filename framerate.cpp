#include "framerate.h"

FrameRate::FrameRate(unsigned short rate, unsigned short thres){
    this->PresetRate = rate;
    this->RateThreshold = thres;
}

void FrameRate::SetFrameRate(unsigned short rate){
    this->PresetRate = rate;
}

void FrameRate::UpdateFrameRate(unsigned short rate){
    this->CurrentRate = rate;
}

unsigned short FrameRate::GetCurrentFrameRate(void){
    return this->CurrentRate;
}

unsigned short FrameRate::GetPresetFrameRate(void){
    return this->PresetRate;
}

unsigned short FrameRate::IsRateOk(void){
    if (((this->CurrentRate < this->PresetRate) && (this->PresetRate - this->CurrentRate > this->RateThreshold)) ||
        ((this->CurrentRate > this->PresetRate) && (this->CurrentRate - this->PresetRate > this->RateThreshold))){
        return 0;
    }
    else return 1;
}
