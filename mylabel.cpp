#include "mylabel.h"

//实现鼠标事件，在已有的label上，重写鼠标事件来实现
//!使用枚举，设置鼠标的状态，以及当前鼠标的位置坐标
MYLABEL::MYLABEL(QWidget *parent) : QLabel(parent)
{
    leftB = false;
    rightB = false;
}

void MYLABEL::mousePressEvent(QMouseEvent *ev)
{
    MOUSEMODE type;
    if(ev->button() == Qt::LeftButton && !leftB)
    {
        pos = CELL::float2(ev->x(),ev->y());
        leftB = true;
        type = M_LEFTDOWN;
    }
    else if(ev->button() == Qt::RightButton && !rightB )
    {
        pos = CELL::float2(ev->x(),ev->y());
        rightB = true;
        type = M_RIGHTDOWN;
    }
    emit sendMouseData(type,pos,pos);
}

void MYLABEL::mouseMoveEvent(QMouseEvent *ev)
{
    MOUSEMODE type;
    CELL::float2 p(ev->x(),ev->y());
    type = M_MOVE;
    emit sendMouseData(type,pos,p);
}

void MYLABEL::mouseReleaseEvent(QMouseEvent *ev)
{
    CELL::float2 p(ev->x(),ev->y());
    MOUSEMODE type;
    if(ev->button() == Qt::LeftButton && leftB)
    {
        leftB = false;
        type = M_LEFTUP;
    }
    else if(ev->button() == Qt::RightButton && rightB )
    {
        rightB = false;
        type = M_RIGHTUP;
    }
    emit sendMouseData(type,pos,p);
}

void MYLABEL::wheelEvent(QWheelEvent *event)
{
   emit sendMouseData(event->delta());
}
