#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "memfile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void CameraCallback(void);
    void UIUpdateCallback(void);
private slots:
    void on_pushButton_camerachoose_clicked(void);
    void on_pushButton_choosearuco_clicked(void);
    void on_pushButton_removearuco_clicked(void);
    void on_pushButton_experimentstart_clicked(void);
    void GenerateMarkers(void);
    void SaveConfigurations(void);
    void Exit(void);
    void CameraSettings(void);
};

#endif // MAINWINDOW_H
