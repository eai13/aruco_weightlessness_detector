//#define L_UP 0
//#define R_UP 1
//#define L_DW 2
//#define R_DW 3

//#define EXCLUDED cv::Point2i(-1, -1)

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

#include <QStringListModel>

QStringListModel * MarkerList;

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

void MainWindow::CameraCallback(void){
    if (camera.isOpened()){
        camera >> VideoFrame;
        MarkerIDs.clear();
        cv::aruco::detectMarkers(VideoFrame, MarkerDictionary, MarkerCorners, MarkerIDs, MarkerParameters, MarkersRejectedCandidates);

        QStringList StrList = MarkerList->stringList();

        for (int iter = 0; iter < MarkerIDs.size(); iter++){
            if (AvailableMarkers[MarkerIDs[iter]] == 1){
                std::cout << "OKAY 1 " << MarkerIDs[iter] << std::endl;
                DetectedMarkers[MarkerIDs[iter]].x = (MarkerCorners[iter][0].x + MarkerCorners[iter][1].x + MarkerCorners[iter][2].x + MarkerCorners[iter][3].x) / 4;
                DetectedMarkers[MarkerIDs[iter]].y = (MarkerCorners[iter][0].y + MarkerCorners[iter][1].y + MarkerCorners[iter][2].y + MarkerCorners[iter][3].y) / 4;
                std::cout << "OK 2" << std::endl;

                for (auto iter2 = StrList.begin(); iter2 != StrList.end(); iter2++){
                    if (strncmp(iter2->toStdString().c_str(), std::to_string(MarkerIDs[iter]).c_str(), std::to_string(MarkerIDs[iter]).length()) == 0){
                        *iter2 = QString::fromStdString(std::to_string(MarkerIDs[iter])) +
                                " [ " + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].x)) + " ; "
                                + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].y)) + " ]";
//                        StrList[StrList.indexOf(QString::fromStdString(std::to_string(MarkerIDs[iter])), 0)] =
//                                QString::fromStdString(std::to_string(MarkerIDs[iter])) +
//                                " [ " + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].x)) + " ; "
//                                + QString::fromStdString(std::to_string(DetectedMarkers[MarkerIDs[iter]].y)) + " ]";
                    }
                }
                std::cout << "ENDEDED 3" << std::endl;
            }
        }
        MarkerList->setStringList(StrList);
        std::cout << std::endl;

        cv::aruco::drawDetectedMarkers(VideoFrame, MarkerCorners, MarkerIDs);
        cv::cvtColor(VideoFrame, VideoFrame, cv::COLOR_BGR2RGB);
        QImage image(VideoFrame.data, VideoFrame.cols, VideoFrame.rows, VideoFrame.step, QImage::Format_RGB888);
        image.setDevicePixelRatio(2);
        ui->label_camimage->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::UIUpdateCallback(void){
    QList<QCameraInfo> CamAvailable = QCameraInfo::availableCameras();
    ui->comboBox_camerachoose->clear();
    for (auto iter = CamAvailable.begin(); iter != CamAvailable.end(); iter++)
        ui->comboBox_camerachoose->addItem(iter->description());
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    MarkerList = new QStringListModel;
    ui->listView_markerslist->setModel(MarkerList);

    MarkerParameters = cv::aruco::DetectorParameters::create();
    MarkerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    cv::Mat img;
    for (int i = 0; i < 50; i++){
        cv::aruco::drawMarker(MarkerDictionary, i, 200, img);
        cv::imwrite(std::to_string(i) + ".bmp", img);
    }

    CameraTimer = new QTimer();
    connect(CameraTimer, SIGNAL(timeout()), this, SLOT(CameraCallback()));
    CameraTimer->start(20);

    UIUpdateTimer = new QTimer();
    connect(UIUpdateTimer, SIGNAL(timeout()), this, SLOT(UIUpdateCallback()));
    UIUpdateTimer->start(500);
}

MainWindow::~MainWindow(){
    if (camera.isOpened()){
        camera.release();
    }
    delete ui;
}

void MainWindow::on_pushButton_camerachoose_clicked(){
    if (camera.isOpened()) camera.release();
    camera.open(ui->comboBox_camerachoose->currentIndex());
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    camera.set(cv::CAP_PROP_AUTOFOCUS, 0);
    camera.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
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

//    if (!(StrList.contains(ui->lineEdit_markerID->text()))){
//        StrList.append(ui->lineEdit_markerID->text());
//        AvailableMarkers[ui->lineEdit_markerID->text().toInt()] = 1;
//        MarkerList->setStringList(StrList);
//    }
}

void MainWindow::on_pushButton_removearuco_clicked(){
    QStringList StrList = MarkerList->stringList();
    for (auto iter = StrList.begin(); iter != StrList.end(); iter++){
        if (strncmp(iter->toStdString().c_str(), ui->lineEdit_markerID->text().toStdString().c_str(), iter->length()) == 0){
            StrList.erase(iter);
            AvailableMarkers[iter->toInt()] = 0;
            MarkerList->setStringList(StrList);
            break;
        }
    }
//    if (StrList.contains(ui->lineEdit_markerID->text())){
//        StrList.removeOne(ui->lineEdit_markerID->text());
//        AvailableMarkers[ui->lineEdit_markerID->text().toInt()] = 0;
//        MarkerList->setStringList(StrList);
//    }
}
