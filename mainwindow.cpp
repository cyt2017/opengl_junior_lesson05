#include "mainwindow.h"
#include "ui_mainwindow.h"

//判断像素点是否在图像内
CELL::float3  calcIntersectPoint( CELL::Ray& ray )
{
    CELL::float3    pos     =   ray.getOrigin();//getOrigin返回射线的起点
    float           tm      =   abs((pos.y) / ray.getDirection().y);//getDirection返回射线的方向
    CELL::float3    target  =   ray.getPoint(tm);//设置目标
    return  CELL::float3(target.x,0,target.z);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _width = 600;
    _height = 400;
    _buffer = NULL;

    int width = _width;
    int height = _height;
    _buffer = new char[_width*_height*4];
    if(_buffer ==NULL){
        return ;
    }
    memset(_buffer,0,_width*_height*4);

    g_rButtonFlag   =   false;//右键按下标志位
    g_lButtonFlag   =   false;//左键按下标志位
    raster = new Raster(width,height,_buffer);

    g_camera.setViewSize(width,height);
    g_camera.perspective(60,(float)(width)/(float)(height),0.1,10000);
    g_camera.update();

    raster->setViewPort(0,0,width,height);
    raster->setPerspective(60,(float)(width)/(float)(height),0.1,10000);

     img  = IMAGE::loadFromFile("/home/cty/workspace/OPENGL/opengl_junior_lesson05/build/bg.png");
     img1 = IMAGE::loadFromFile("/home/cty/workspace/OPENGL/opengl_junior_lesson05/build/1.jpg");

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timer->start(20);

    connect(ui->label,SIGNAL(sendMouseData(MOUSEMODE,CELL::float2,CELL::float2)),this,SLOT(onReciveMouseData(MOUSEMODE,CELL::float2,CELL::float2)));
    connect(ui->label,SIGNAL(sendMouseData(int)),this,SLOT(onReciveMouseData(int)));
}

MainWindow::~MainWindow()
{
    timer->deleteLater();
    delete []raster;
    delete [] _buffer;
    delete ui;
}

void MainWindow::dispalyImage()
{
    int w = _width;
    int h = _height;
    QImage img = QImage((const uchar *)_buffer,w,h,w*4,QImage::Format_ARGB32);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::onTimeOut()
{

    raster->clear();
    raster->setView(g_camera.getView());

    raster->drawImage(0,0,img);//设置背景图像
    Vertex    vertexs[]   =
    {
        {-1, 0,   1,  0,  0,    CELL::Rgba(255,0,0)},
        { 1, 0,   1,  1,  0,    CELL::Rgba(0,255,0)},
        { 1, 0,  -1,  1,  1,    CELL::Rgba(0,0,255)},

        {-1, 0,   1,  0,  0,    CELL::Rgba(255,0,0)},
        { 1, 0,  -1,  1,  1,    CELL::Rgba(0,0,255)},
        {-1, 0,  -1,  0,  1,    CELL::Rgba(255,255)},
    };

    for (int i = 0 ;i  < 6 ; ++ i)
    {
        vertexs[i].x    *=   100;
        vertexs[i].z    *=   100;

        vertexs[i].u    *=   10;
        vertexs[i].v    *=   10;
    }
    raster->bindTexture(img1);

    raster->vertexPointer(2,DT_FLOAT,      sizeof(Vertex),&vertexs[0].x);
    raster->textureCoordPointer(2,DT_FLOAT,sizeof(Vertex),&vertexs[0].u);
    raster->colorPointer(4,DT_BYTE,        sizeof(Vertex),&vertexs[0].color);

    raster->drawArrays(DM_TRIANGES,0,6);

    dispalyImage();
}

void MainWindow::onReciveMouseData(MOUSEMODE type, CELL::float2 ps, CELL::float2 pe)
{
    switch (type) {
    case M_LEFTDOWN:
       {
            g_lButtonDown.x =   ps.x;
            g_lButtonDown.y =   ps.y;
            g_lButtonFlag   =   true;
       }
        break;
    case M_RIGHTDOWN:
       {
            g_rButtonDown.x =   ps.x;
            g_rButtonDown.y =   ps.y;
            g_rButtonFlag   =   true;
       }
        break;
    case M_LEFTUP:
       {
            g_lButtonFlag   =   false;
       }
        break;
    case M_RIGHTUP:
       {
            g_rButtonFlag   =   false;
       }
        break;
    case M_MOVE:
       {
            int x   =   pe.x;
            int y   =   pe.y;
            if (g_rButtonFlag)
            {
                int     offsetX =   x - g_rButtonDown.x;
                int     offsetY =   y - g_rButtonDown.y;
                g_camera.rotateViewY(offsetX);
                g_camera.rotateViewX(offsetY);
                g_rButtonDown.x =   x;
                g_rButtonDown.y =   y;
            }
            if (g_lButtonFlag)
            {
              /**
              *   首先计算出来一个像素和当前场景的比例
              */
              CELL::Ray   ray0    =   g_camera.createRayFromScreen(x,y);
              CELL::Ray   ray1    =   g_camera.createRayFromScreen(g_lButtonDown.x,g_lButtonDown.y);

              CELL::float3  pos0    =   calcIntersectPoint(ray0);
              CELL::float3  pos1    =   calcIntersectPoint(ray1);

              CELL::float3  offset  =   pos1 - pos0;

              g_lButtonDown   =   CELL::int2(x,y);

              CELL::float3  newEye  =   g_camera.getEye() + offset;
              CELL::float3  newTgt  =   g_camera.getTarget() + offset;


              g_camera.setEye(newEye);
              g_camera.setTarget(newTgt);

              g_camera.update();
            }
       }
        break;
    default:
        break;
    }
}

void MainWindow::onReciveMouseData(int delta)
{
    if (delta > 0)
    {
      float   len =   CELL::length(g_camera._eye-g_camera._target);
              len *=  1.2f;
      g_camera._eye   =   g_camera._target - g_camera._dir * len;
    }
    else
    {
      float   len =   CELL::length(g_camera._eye-g_camera._target);
              len *=  0.9f;
      g_camera._eye   =   g_camera._target - g_camera._dir * len;
    }
    g_camera.update();
}
