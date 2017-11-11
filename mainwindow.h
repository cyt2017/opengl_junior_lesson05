#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include "tool/CELLMath.hpp"
#include "image.h"
#include "raster.h"
#include "cellcamera.h"
#include "mylabel.h"

namespace Ui {
class MainWindow;
}

struct  Vertex
{
    float       x,y,z;
    float       u,v;
    CELL::Rgba  color;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int _width;
    int _height;
    void *_buffer;
    CELLCamera          g_camera;
    CELL::int2          g_rButtonDown;
    bool                g_rButtonFlag;
    CELL::int2          g_lButtonDown;
    bool                g_lButtonFlag;
    Raster              *raster;

    QTimer *timer;
    IMAGE *img ;
    IMAGE *img1;
    void dispalyImage();



protected slots:
    void onTimeOut();
    void onReciveMouseData(MOUSEMODE type,CELL::float2 ps,CELL::float2 pe);
    void onReciveMouseData(int delta);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
