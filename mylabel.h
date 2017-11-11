#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "tool/CELLMath.hpp"

enum MOUSEMODE
{
    M_LEFTDOWN,
    M_RIGHTDOWN,
    M_LEFTUP,
    M_RIGHTUP,
    M_MOVE,
    M_WHEELDOWN,
    M_WHEELUP,
};

typedef struct
{
    MOUSEMODE type;
    CELL::float2 pStart;
    CELL::float2 pEnd;
}MOUSEDATA;
class MYLABEL : public QLabel
{
    Q_OBJECT
public:
    explicit MYLABEL(QWidget *parent = 0);

    bool leftB;
    bool rightB;
    CELL::float2 pos;

    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *event);
signals:
    void sendMouseData(MOUSEMODE type,CELL::float2 ps,CELL::float2 pe);
    void sendMouseData(int);
public slots:
};

#endif // MYLABEL_H
