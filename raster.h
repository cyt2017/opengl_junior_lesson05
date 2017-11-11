#ifndef RASTER_H
#define RASTER_H

#include "tool/CELLMath.hpp"
#include "image.h"

enum    DRAWMODE
 {
     DM_POINTS       =   0,
     DM_LINES        =   1,
     DM_LINE_LOOP    =   2,
     DM_LINE_STRIP   =   3,
     DM_TRIANGES     =   4,
 };

 enum    DATETYPE
 {
     DT_BYTE,
     DT_FLOAT,
     DT_DOUBLE,
 };

 struct  DateElementDes
 {
     int         _size;
     DATETYPE    _type;
     int         _stride;
     const void* _data;
 };

 class   Span
 {
 public:
     int     _xStart;
     int     _xEnd;
     CELL::Rgba4Byte    _colorStart;
     CELL::Rgba4Byte    _colorEnd;

     CELL::float2  _uvStart;
     CELL::float2  _uvEnd;

     int     _y;

 public:
     Span(int xStart,int xEnd,int y,CELL::Rgba4Byte colorStart,CELL::Rgba4Byte colorEnd,CELL::float2 uvStart,CELL::float2 uvEnd);
 };

 class   Ege
 {
 public:
     int     _x1;
     int     _y1;
     CELL::float2  _uv1;
     CELL::Rgba4Byte    _color1;

     int     _x2;
     int     _y2;
     CELL::float2  _uv2;
     CELL::Rgba4Byte    _color2;

     Ege(int x1,int y1,CELL::Rgba4Byte color1,CELL::float2 uv1,int x2,int y2,CELL::Rgba4Byte color2,CELL::float2 uv2);
 };


class Raster
{
public:
    uint*   _buffer;
    int     _width;
    int     _height;
    CELL::Rgba4Byte    _color;

    IMAGE*  _texture;

    CELL::matrix4 _matModel;
    CELL::matrix4 _matView;
    CELL::matrix4 _matProj;
    CELL::matrix4 _matProjView;
    CELL::float2  _viewPort;
    CELL::Frustum _frust;

    DateElementDes      _poitionPointer;
    DateElementDes      _colorPointer;
    DateElementDes      _uvPointer;

    DateElementDes      _defaultColorPointer;
    DateElementDes      _defaultUVPointer;
    CELL::Rgba4Byte     _defaultColorArray[3];
    CELL::float2              _detaultUVArray[3];

public:
    Raster(int w,int h,void* buffer);
    ~Raster();

    void    clear();

    struct  Vertex
    {
        CELL::int2    p0;
        CELL::float2  uv0;
        CELL::Rgba4Byte    c0;

        CELL::int2    p1;
        CELL::float2  uv1;
        CELL::Rgba4Byte    c1;

        CELL::int2    p2;
        CELL::float2  uv2;
        CELL::Rgba4Byte    c2;

    };

    void    drawImage(int startX,int startY,const IMAGE* image);
public:

    void    loadViewMatrix(const CELL::matrix4& mat);
    void    loadViewIdentity();

    void    loadProjMatrix(const CELL::matrix4& mat);
    void    loadProjIdentity();

    /**
    *   生成投影矩阵
    */
    void    setPerspective(float fovy, float aspect, float zNear, float zFar);
    /**
    *   生成观察矩阵
    */
    void    lookat(CELL::float3 const & eye,CELL::float3 const & center,CELL::float3 const & up);

    void    setView(const CELL::matrix4& viewMat);

    void    setViewPort(int x,int y,int w,int h);
    /**
    *   模型矩阵操作函数
    */

    void    loadMatrix(const CELL::matrix4& mat);

    void    loadIdentity();

    void    vertexPointer(int size,DATETYPE type,int stride,const void* data);

    void    colorPointer(int size,DATETYPE type,int stride,const void* data);

    void    textureCoordPointer(int size,DATETYPE type,int stride,const void* data);

    void    bindTexture(IMAGE* image);

    void    drawArrays(DRAWMODE pri,int start,int count);

protected:

    CELL::float3  piplineTransform(CELL::float3 pos);
    void    drawTrianle(Ege eges[3]);


    void    drawEge(const Ege& e1,const Ege& e2,IMAGE* image);

    void    drawSpan(const Span& span,IMAGE* image);
    void    drawLine(CELL::float2 pt1,CELL::float2 pt2,CELL::Rgba color1,CELL::Rgba color2);

    inline  void    setPixelEx(unsigned x,unsigned y,CELL::Rgba color);

    inline  CELL::Rgba    getPixel(unsigned x,unsigned y);
//    inline  void    setPixel(unsigned x,unsigned y,Rgba color)
//    {
//        if (x >= _width ||  y >= _height)
//        {
//            return;
//        }
//        _buffer[y * _width + x]   =   color._color;
//    }
};

#endif // RASTER_H
