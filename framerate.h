#ifndef FRAMERATE_H
#define FRAMERATE_H

#include <QtMath>

class FrameRate{
public:
    FrameRate(unsigned short rate, unsigned short thres);
    void SetFrameRate(unsigned short rate);
    void UpdateFrameRate(unsigned short rate);
    unsigned short GetCurrentFrameRate(void);
    unsigned short GetPresetFrameRate(void);
    unsigned short IsRateOk(unsigned short current_rate);
private:
    unsigned short CurrentRate;
    unsigned short PresetRate;
    unsigned short RateThreshold;
};

#endif // FRAMERATE_H
