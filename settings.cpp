#include "settings.h"
#include "ui_settings.h"

#include "mainwindow.h"
#include "memfile.h"
#include "config.h"

extern AppConfigs * ConfigFile;

Settings::Settings(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Settings){
    ui->setupUi(this);

    ui->lineEdit_width->setText(QString::fromStdString(std::to_string(ConfigFile->GetCameraWidth())));
    ui->lineEdit_height->setText(QString::fromStdString(std::to_string(ConfigFile->GetCameraHeight())));
    ui->lineEdit_framerate->setText(QString::fromStdString(std::to_string(ConfigFile->GetFrameRate())));
    ui->lineEdit_framerate_thres->setText(QString::fromStdString(std::to_string(ConfigFile->GetFrameRateThres())));
    if (ConfigFile->GetAutoExposure())
        ui->checkBox_autoexposure->setChecked(true);
    else ui->checkBox_autoexposure->setChecked(false);
    if (ConfigFile->GetAutoFocus())
        ui->checkBox_autofocus->setChecked(true);
    else ui->checkBox_autofocus->setChecked(false);
}

Settings::~Settings()
{
    delete ui;
}
