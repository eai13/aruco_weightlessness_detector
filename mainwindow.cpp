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

#include "config.h"
#include "framerate.h"

FrameRate CameraRate(1000 / CAMERA_CALLBACK_PERIOD, CAMERA_FRAMERATE_THRESHOLD);

int StartStopFlag = 0;

std::ofstream LogFile;

QStringListModel * MarkerList;

cv::VideoWriter VideoFile;
cv::VideoCapture camera;
cv::Mat VideoFrame;

QTimer * CameraTimer = NULL;
QTimer * UIUpdateTimer = NULL;

cv::Ptr<cv::aruco::DetectorParameters> MarkerParameters;
cv::Ptr<cv::aruco::Dictionary> MarkerDictionary;
std::vector<int> MarkerIDs;

std::vector<std::vector<cv::Point2f>> MarkerCorners, MarkersRejectedCandidates;

cv::Point2i DetectedMarkers[50];
int AvailableMarkers[50] = { 0 };

int bench_count = 0;

void MainWindow::CameraCallback(void){
    QTimer::singleShot(CAMERA_CALLBACK_PERIOD, this, SLOT(CameraCallback()));
    if (camera.isOpened()){
        camera >> VideoFrame;
        MarkerIDs.clear();
        cv::aruco::detectMarkers(VideoFrame, MarkerDictionary, MarkerCorners, MarkerIDs, MarkerParameters, MarkersRejectedCandidates);

        QStringList StrList = MarkerList->stringList();

        for (int iter = 0; iter < MarkerIDs.size(); iter++){
            if (AvailableMarkers[MarkerIDs[iter]] == 1){
                DetectedMarkers[MarkerIDs[iter]].x = (MarkerCorners[iter][0].x + MarkerCorners[iter][1].x + MarkerCorners[iter][2].x + MarkerCorners[iter][3].x) / 4;
                DetectedMarkers[MarkerIDs[iter]].y = (MarkerCorners[iter][0].y + MarkerCorners[iter][1].y + MarkerCorners[iter][2].y + MarkerCorners[iter][3].y) / 4;

                for (auto iter2 = StrList.begin(); iter2 != StrList.end(); iter2++){
                    if (strncmp(iter2->toStdString().c_str(), std::to_string(MarkerIDs[iter]).c_str(), std::to_string(MarkerIDs[iter]).length()) == 0){
                        *iter2 = QString::fromStdString(std::to_string(MarkerIDs[iter])) +
                                " [ " + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].x)) + " ; "
                                + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].y)) + " ]";
                    }
                }
            }
        }
        MarkerList->setStringList(StrList);

        cv::aruco::drawDetectedMarkers(VideoFrame, MarkerCorners, MarkerIDs);
        if (StartStopFlag == 1){
            if (LogFile.is_open()){
                for (auto iter = StrList.begin(); iter != StrList.end(); iter++)
                    LogFile << iter->toStdString().c_str() << CSV_SEPARATOR;
                LogFile << std::endl;
            }
            if (VideoFile.isOpened())
                VideoFile.write(VideoFrame);
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
    CameraRate.UpdateFrameRate(bench_count);
    std::cout << CameraRate.GetCurrentFrameRate() << std::endl;
    bench_count = 0;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    MarkerList = new QStringListModel;
    ui->listView_markerslist->setModel(MarkerList);

    MarkerParameters = cv::aruco::DetectorParameters::create();
    MarkerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

#ifdef ARUCO_GENERATE_MARKERS
    cv::Mat img;
    for (int i = 0; i < 50; i++){
        cv::aruco::drawMarker(MarkerDictionary, i, 200, img);
        cv::imwrite(std::to_string(i) + ".bmp", img);
    }
#endif

    CameraTimer = new QTimer();

    QTimer::singleShot(CAMERA_CALLBACK_PERIOD, this, SLOT(CameraCallback()));
//    connect(CameraTimer, SIGNAL(timeout()), this, SLOT(CameraCallback()));
//    CameraTimer->start(CAMERA_CALLBACK_PERIOD);

    UIUpdateTimer = new QTimer();
    connect(UIUpdateTimer, SIGNAL(timeout()), this, SLOT(UIUpdateCallback()));
    UIUpdateTimer->start(1000);
}

MainWindow::~MainWindow(){
    if (camera.isOpened())
        camera.release();
    delete ui;
}

void MainWindow::on_pushButton_camerachoose_clicked(){
    if (camera.isOpened()) camera.release();
    camera.open(ui->comboBox_camerachoose->currentIndex());
    camera.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
    camera.set(cv::CAP_PROP_AUTOFOCUS, CAMERA_AUTOFOCUS);
    camera.set(cv::CAP_PROP_AUTO_EXPOSURE, CAMERA_AUTOEXPOSURE);
}

void MainWindow::on_pushButton_choosearuco_clicked(void){
    QStringList StrList = MarkerList->stringList();
    for (auto iter = StrList.begin(); iter != StrList.end(); iter++){
        if (strncmp(iter->toStdString().c_str(), ui->lineEdit_markerID->text().toStdString().c_str(), iter->length()) == 0){
            return;
        }
    }
    StrList.append(ui->lineEdit_markerID->text());
    AvailableMarkers[ui->lineEdit_markerID->text().toInt()] = 1;
    MarkerList->setStringList(StrList);

    ui->lineEdit_markerID->setText("");
}

void MainWindow::on_pushButton_removearuco_clicked(){
    QStringList StrList = MarkerList->stringList();
    for (auto iter = StrList.begin(); iter != StrList.end(); iter++){
        if (strncmp(iter->toStdString().c_str(), ui->lineEdit_markerID->text().toStdString().c_str(), ui->lineEdit_markerID->text().length()) == 0){
            StrList.erase(iter);
            AvailableMarkers[iter->toInt()] = 0;
            MarkerList->setStringList(StrList);
            break;
        }
    }

    ui->lineEdit_markerID->setText("");
}

void MainWindow::on_pushButton_experimentstart_clicked(){
    if (StartStopFlag == 0){
        QDir directory = QDir::current();
        directory.mkdir(ui->lineEdit_experimentname->text());

        if (!(VideoFile.isOpened()))
            VideoFile.open(ui->lineEdit_experimentname->text().toStdString() + "/" + ui->lineEdit_experimentname->text().toStdString() + VIDEO_FORMAT,
                           cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 1000 / CAMERA_CALLBACK_PERIOD,
                           cv::Size(camera.get(cv::CAP_PROP_FRAME_WIDTH), camera.get(cv::CAP_PROP_FRAME_HEIGHT)));
        if (!(LogFile.is_open())) LogFile.open(ui->lineEdit_experimentname->text().toStdString() + "/" + ui->lineEdit_experimentname->text().toStdString() + CSV_TAIL);
        ui->pushButton_experimentstart->setText("Stop");
        StartStopFlag = 1;
    }
    else{
        StartStopFlag = 0;
        if (VideoFile.isOpened()) VideoFile.release();
        if (LogFile.is_open()) LogFile.close();
        ui->pushButton_experimentstart->setText("Start");
    }
}
