#ifndef FRAMERATE_H
#define FRAMERATE_H

#include "opencv2/aruco.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

class FrameRate{
public:
    FrameRate(unsigned short rate, unsigned short thres);
    void SetFrameRate(unsigned short rate);
    void UpdateFrameRate(unsigned short rate);
    unsigned short GetCurrentFrameRate(void);
    unsigned short GetPresetFrameRate(void);
    unsigned short IsRateOk(void);
private:
    unsigned short CurrentRate;
    unsigned short PresetRate;
    unsigned short RateThreshold;
};

class Camera : public cv::VideoCapture{
public:
    Camera(int width, int height, int autofocus, int autoexposure) :
        width(width), height(height), autofocus(autofocus), autoexposure(autoexposure){}
    bool Open(int id){
        bool state;
        state = this->open(id);
        this->set(cv::CAP_PROP_FRAME_WIDTH, this->width);
        this->set(cv::CAP_PROP_FRAME_HEIGHT, this->height);
        this->set(cv::CAP_PROP_AUTOFOCUS, this->autofocus);
        this->set(cv::CAP_PROP_AUTO_EXPOSURE, this->autoexposure);
        return state;
    }

    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetAutoExposure(int state) { this->autoexposure = state; }
    void SetAutoFocus(int state) { this->autofocus = state; }

    int GetWidth(void) { return this->width; }
    int GetHeight(void) { return this->height; }
    int GetAutoExposure(void) { return this->autoexposure; }
    int GetAutoFocus(void) { return this->autofocus; }

private:
    int width;
    int height;
    int autofocus;
    int autoexposure;
};

#endif // FRAMERATE_H
