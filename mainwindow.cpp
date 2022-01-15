#define ONE_SECOND  1000

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/aruco.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <stdint.h>
#include <vector>
#include <string>

#include <QCameraInfo>
#include <QMessageLogger>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QComboBox>
#include <QString>
#include <QTimer>
#include <QtTest/QTest>

#include <iostream>
#include <fstream>

#include <QStringListModel>
#include <QDir>
#include <QTime>

#include "config.h"
#include "framerate.h"
#include "memfile.h"

QTime * Time;

AppConfigs * ConfigFile;

FrameRate * CameraRate;

int StartStopFlag = 0;

std::ofstream LogFile;

QStringListModel * MarkerList;

cv::VideoWriter VideoFile;
Camera * camera;
cv::Mat VideoFrame;

QTimer * UIUpdateTimer = NULL;

cv::Ptr<cv::aruco::DetectorParameters> MarkerParameters;
cv::Ptr<cv::aruco::Dictionary> MarkerDictionary;
std::vector<int> MarkerIDs;

std::vector<std::vector<cv::Point2f>> MarkerCorners, MarkersRejectedCandidates;

cv::Point2i DetectedMarkers[50];
int AvailableMarkers[50] = { 0 };

int bench_count = 0;
int FrameCounter = 0;

void MainWindow::CameraCallback(void){
    QTimer::singleShot(CAMERA_CALLBACK_PERIOD, this, SLOT(CameraCallback()));
    if (camera->isOpened()){
        *camera >> VideoFrame;
        MarkerIDs.clear();
        cv::aruco::detectMarkers(VideoFrame, MarkerDictionary, MarkerCorners, MarkerIDs, MarkerParameters, MarkersRejectedCandidates);

        QStringList StrList = MarkerList->stringList();

        for (int iter = 0; iter < MarkerIDs.size(); iter++){
            if (AvailableMarkers[MarkerIDs[iter]] == 1){
                std::cout << "MarkerAvailable" << std::endl;
                DetectedMarkers[MarkerIDs[iter]].x = (MarkerCorners[iter][0].x + MarkerCorners[iter][1].x + MarkerCorners[iter][2].x + MarkerCorners[iter][3].x) / 4;
                DetectedMarkers[MarkerIDs[iter]].y = (MarkerCorners[iter][0].y + MarkerCorners[iter][1].y + MarkerCorners[iter][2].y + MarkerCorners[iter][3].y) / 4;

                for (auto iter2 = StrList.begin(); iter2 != StrList.end(); iter2++){
                    if (strncmp(iter2->toStdString().c_str(), (MARKER_ID_HEADER + std::to_string(MarkerIDs[iter])).c_str(), (MARKER_ID_HEADER + std::to_string(MarkerIDs[iter])).length()) == 0){
                        *iter2 = MARKER_ID_HEADER + QString::fromStdString(std::to_string(MarkerIDs[iter])) +
                                " ; " + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].x)) + " ; "
                                + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].y));
                    }
                }
            }
        }
        MarkerList->setStringList(StrList);
        ConfigFile->SetArucoIDs(StrList);

        cv::aruco::drawDetectedMarkers(VideoFrame, MarkerCorners, MarkerIDs);
        if (StartStopFlag == 1){
            FrameCounter++;
            if (LogFile.is_open()){
                LogFile << "Time:" << CSV_SEPARATOR << Time->elapsed() << CSV_SEPARATOR;
                LogFile << "Frame:" << CSV_SEPARATOR << FrameCounter << CSV_SEPARATOR;
                for (auto iter = StrList.begin(); iter != StrList.end(); iter++)
                    LogFile << iter->toStdString().c_str() << CSV_SEPARATOR;
                LogFile << std::endl;
            }
            if (VideoFile.isOpened()){
//                cv::rectangle(VideoFrame,
//                              cv::Point(10, 10), cv::Point(300, 90),
//                              cv::Scalar(0, 0, 0), -1);
                cv::putText(VideoFrame,
                            "Time: " + std::to_string((Time->elapsed() / 1000.0)),
                            cv::Point(20, 40),
                            cv::FONT_HERSHEY_SIMPLEX, 0.75,
                            cv::Scalar(255, 255, 255), 2);
                cv::putText(VideoFrame,
                            "Frame: " + std::to_string(FrameCounter),
                            cv::Point(20, 70),
                            cv::FONT_HERSHEY_SIMPLEX, 0.75,
                            cv::Scalar(255, 255, 255), 2);
                VideoFile.write(VideoFrame);
            }
        }

        cv::cvtColor(VideoFrame, VideoFrame, cv::COLOR_BGR2RGB);
        QImage image(VideoFrame.data, VideoFrame.cols, VideoFrame.rows, VideoFrame.step, QImage::Format_RGB888);
        image.setDevicePixelRatio(CAMERA_PIXEL_RATIO);
        ui->label_camimage->setPixmap(QPixmap::fromImage(image));
    }
    bench_count++;
}

void MainWindow::UIUpdateCallback(void){
    QList<QCameraInfo> CamAvailable = QCameraInfo::availableCameras();
    ui->comboBox_camerachoose->clear();
    for (auto iter = CamAvailable.begin(); iter != CamAvailable.end(); iter++)
        ui->comboBox_camerachoose->addItem(iter->description());
    CameraRate->UpdateFrameRate(bench_count);
    ui->label_fps->setText(" FPS: " + QString::fromStdString(std::to_string(CameraRate->GetCurrentFrameRate())));
    if (CameraRate->IsRateOk()) ui->label_fps->setStyleSheet("QLabel { background-color: #55ff00; }");
    else ui->label_fps->setStyleSheet("QLabel { background-color: yellow; }");
    bench_count = 0;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    Time = new QTime;

    ConfigFile = new AppConfigs("config.ar");

    MarkerList = new QStringListModel;
    QStringList strlist = ConfigFile->GetArucoIDs();
    ui->listView_markerslist->setModel(MarkerList);
    MarkerList->setStringList(strlist);
    for (auto iter = strlist.begin(); iter != strlist.end(); iter++)
        AvailableMarkers[iter->remove(0, 10).toInt()] = 1;

    CameraRate = new FrameRate(ConfigFile->GetFrameRate(), ConfigFile->GetFrameRateThres());

    camera = new Camera(ConfigFile->GetCameraHeight(),
                        ConfigFile->GetCameraWidth(),
                        ConfigFile->GetAutoFocus(),
                        ConfigFile->GetAutoExposure());
    camera->Open(ConfigFile->GetCameraID());

    MarkerParameters = cv::aruco::DetectorParameters::create();
    MarkerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    QTimer::singleShot(ONE_SECOND / CameraRate->GetPresetFrameRate(), this, SLOT(CameraCallback()));

    UIUpdateTimer = new QTimer();
    connect(UIUpdateTimer, SIGNAL(timeout()), this, SLOT(UIUpdateCallback()));
    UIUpdateTimer->start(1000);

    connect(ui->actionGenerate_Markers, SIGNAL(triggered()), this, SLOT(GenerateMarkers()));
    connect(ui->actionSave_Configurations, SIGNAL(triggered()), this, SLOT(SaveConfigurations()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(Exit()));
}

MainWindow::~MainWindow(){
    if (camera->isOpened())
        camera->release();

    delete ui;
}

void MainWindow::on_pushButton_camerachoose_clicked(){
    if (camera->isOpened()) camera->release();
    camera->Open(ui->comboBox_camerachoose->currentIndex());
}

void MainWindow::on_pushButton_choosearuco_clicked(void){
    QStringList StrList = MarkerList->stringList();
    if (ui->lineEdit_markerID->text().length() == 0) return;
    for (auto iter = StrList.begin(); iter != StrList.end(); iter++){
        if (strncmp(iter->toStdString().c_str(), (MARKER_ID_HEADER + ui->lineEdit_markerID->text().toStdString()).c_str(), iter->length()) == 0){
            ui->lineEdit_markerID->clear();
            return;
        }
    }
    StrList.append(MARKER_ID_HEADER + ui->lineEdit_markerID->text());
    AvailableMarkers[ui->lineEdit_markerID->text().toInt()] = 1;
    std::cout << "MarkerSet" << std::endl;
    MarkerList->setStringList(StrList);

    ui->lineEdit_markerID->clear();
}

void MainWindow::on_pushButton_removearuco_clicked(){
    QStringList StrList = MarkerList->stringList();
    if (ui->lineEdit_markerID->text().length() == 0) return;
    for (auto iter = StrList.begin(); iter != StrList.end(); iter++){
        if (strncmp(iter->toStdString().c_str(), (MARKER_ID_HEADER + ui->lineEdit_markerID->text().toStdString()).c_str(), (MARKER_ID_HEADER + ui->lineEdit_markerID->text()).length()) == 0){
            StrList.erase(iter);
            AvailableMarkers[iter->toInt()] = 0;
            MarkerList->setStringList(StrList);
            break;
        }
    }

    ui->lineEdit_markerID->clear();
}

void MainWindow::on_pushButton_experimentstart_clicked(){
    if (StartStopFlag == 0){
        QDir directory = QDir::current();
        directory.mkdir(ui->lineEdit_experimentname->text());
        std::cout << "here 1" << std::endl;
        if (!(VideoFile.isOpened()))
            VideoFile.open(ui->lineEdit_experimentname->text().toStdString() + "/" + ui->lineEdit_experimentname->text().toStdString() + VIDEO_FORMAT,
                           cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), CameraRate->GetPresetFrameRate(),
                           cv::Size(camera->get(cv::CAP_PROP_FRAME_WIDTH), camera->get(cv::CAP_PROP_FRAME_HEIGHT))); //camera->GetWidth(), camera->GetHeight()));
        std::cout << "here 2" << std::endl;
        if (!(LogFile.is_open())) LogFile.open(ui->lineEdit_experimentname->text().toStdString() + "/" + ui->lineEdit_experimentname->text().toStdString() + CSV_TAIL);
        ui->pushButton_experimentstart->setText("Stop");
        std::cout << "here 3" << std::endl;
        StartStopFlag = 1;
        Time->restart();
    }
    else{
        FrameCounter = 0;
        StartStopFlag = 0;
        if (VideoFile.isOpened()) VideoFile.release();
        if (LogFile.is_open()) LogFile.close();
        ui->pushButton_experimentstart->setText("Start");
    }
}

void MainWindow::GenerateMarkers(){
    QDir directory = QDir::current();
    directory.mkdir("Aruco Markers/");
    cv::Mat img;
    for (int i = 0; i < 50; i++){
        cv::aruco::drawMarker(MarkerDictionary, i, 200, img);
        cv::imwrite("Aruco Markers/" + std::to_string(i) + ".bmp", img);
    }
}

void MainWindow::SaveConfigurations(){
    ConfigFile->Save("config.ar");
}

void MainWindow::Exit(){
    this->~MainWindow();
}
