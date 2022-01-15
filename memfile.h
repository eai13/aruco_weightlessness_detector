#ifndef MEMFILE_H
#define MEMFILE_H

#define PARAMETER_WIDTH             "CAMW"
#define PARAMETER_HEIGHT            "CAMH"
#define PARAMETER_CAMID             "CAMID"
#define PARAMETER_FRAMERATE         "FRMRT"
#define PARAMETER_FRAMERATE_THRES   "FRMRTTHR"
#define PARAMETER_ARUCO_IDS         "ARUCOID"
#define PARAMETER_AUTOEXPOSURE      "AEXP"
#define PARAMETER_AUTOFOCUS         "AFOC"

#include <fstream>
#include <iostream>
#include <QList>
#include <QStringList>

#include "config.h"

class AppConfigs{
public:
    AppConfigs(std::string filename);
    int GetCameraWidth(void) { return this->camera_width; }
    int GetCameraHeight(void) { return this->camera_height; }
    int GetCameraID(void) { return this->camera_id; }
    int GetFrameRate(void) { return this->framerate; }
    int GetFrameRateThres(void) { return this->framerate_thres; }
    int GetAutoExposure(void) { return this->autoexposure; }
    int GetAutoFocus(void) { return this->autofocus; }
    QStringList GetArucoIDs(void);


    void SetCameraWidth(int width) { this->camera_width = width; }
    void SetCameraHeight(int height) { this->camera_height = height; }
    void SetCameraID(int id) { this->camera_id = id; }
    void SetFrameRate(int framerate) { this->framerate = framerate; }
    void SetFrameRateThres(int framerate_thres) { this->framerate_thres = framerate_thres; }
    void SetAutoExposure(int autoexposure) { this->autoexposure = autoexposure; }
    void SetAutoFocus(int autofocus) { this->autofocus = autofocus; }
    void SetArucoIDs(QStringList aruco_list);

    void Save(std::string filename);

private:
    int camera_width = 1920;
    int camera_height = 1080;
    int camera_id = 0;
    int framerate = 10;
    int framerate_thres = 3;
    int autofocus = 0;
    int autoexposure = 1;
    QList<int> aruco_list;
};

#endif // MEMFILE_H
