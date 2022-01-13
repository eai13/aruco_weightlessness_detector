#include "framerate.h"

FrameRate::FrameRate(unsigned short rate, unsigned short thres){
    this->PresetRate = rate;
    this->RateThreshold = thres;
}

void FrameRate::SetFrameRate(unsigned short rate){
    this->CurrentRate = rate;
}

void FrameRate::UpdateFrameRate(unsigned short rate){
    this->PresetRate = rate;
}

unsigned short FrameRate::GetCurrentFrameRate(void){
    return this->CurrentRate;
}

unsigned short FrameRate::GetPresetFrameRate(void){
    return this->PresetRate;
}

unsigned short FrameRate::IsRateOk(unsigned short current_rate){
    if (((current_rate < this->PresetRate) && (this->PresetRate - current_rate > this->RateThreshold)) ||
        ((current_rate > this->PresetRate) && (current_rate - this->PresetRate > this->RateThreshold))){
        return 0;
    }
    else return 1;
}