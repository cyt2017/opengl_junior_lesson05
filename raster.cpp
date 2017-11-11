#include "raster.h"

Raster::Raster(int w,int h,void* buffer)
{
    _texture        =   0;
    _width          =   w;
    _height         =   h;
    _buffer         =   (uint*)buffer;
    memset(&_poitionPointer,0,  sizeof(_poitionPointer));
    memset(&_colorPointer,  0,  sizeof(_colorPointer));
    memset(&_uvPointer,     0,  sizeof(_uvPointer));

    _defaultColorPointer._size  =   4;
    _defaultColorPointer._type  =   DT_BYTE;
    _defaultColorPointer._stride=   sizeof(CELL::Rgba);
    _defaultColorPointer._data  =   _defaultColorArray;

    _defaultUVPointer._size     =   2;
    _defaultUVPointer._type     =   DT_FLOAT;
    _defaultUVPointer._stride   =   sizeof(CELL::float2);
    _defaultUVPointer._data     =   _detaultUVArray;

    _matModel                   =   CELL::matrix4(1);
    _matView                    =   CELL::matrix4(1);
    _matProj                    =   CELL::matrix4(1);
}

Raster::~Raster()
{

}
void Raster::clear()//!清空窗口
{
    memset(_buffer,0,_width * _height * sizeof(CELL::Rgba));
}
//!把图像贴到窗口上
void Raster::drawImage( int startX,int startY,const IMAGE* image )
{
    int left    =   CELL::tmax<int>(startX,0);
    int top     =   CELL::tmax<int>(startY,0);

    int right   =   CELL::tmin<int>(startX + image->w(),_width);
    int bottom  =   CELL::tmin<int>(startY + image->h(),_height);

    for (int x = left ; x <  right ; ++ x)
    {
        for (int y = top ; y <  bottom ; ++ y)
        {
            CELL::Rgba    color   =   image->setPixel(x - left,y - top);
            setPixelEx(x,y,color);
        }
    }
}
//!画图，目前只实现了画三角形
void Raster::drawArrays( DRAWMODE pri,int start,int count )
{
    if (_poitionPointer._data == 0)
    {
        return;
    }

    //!提取数据中的位置坐标，颜色和纹理坐标，并根据这些信息画图
    DateElementDes  colorPointerdesc    =   _colorPointer;
    DateElementDes  uvPointerdesc       =   _uvPointer;
    if (colorPointerdesc._data == 0)
    {
        colorPointerdesc    =   _defaultColorPointer;
    }
    if (uvPointerdesc._data == 0)
    {
        uvPointerdesc       =   _defaultUVPointer;
    }
    char*   posData =   (char*)_poitionPointer._data;
    char*   cData   =   (char*)colorPointerdesc._data;
    char*   uvData  =   (char*)uvPointerdesc._data;

    _matProjView    =   _matProj * _matView;

    CELL::matrix4 matPVT  =   _matProjView.transpose();

    _frust.loadFrustum(matPVT);

    for(int i = start ;i < start + count; i += 3)
    {
        float*  fData   =   (float*)posData;
        CELL::float3  p01 (fData[0],fData[1],fData[2]);

        posData +=  _poitionPointer._stride;
        fData   =   (float*)(posData);


        CELL::float3  p11 (fData[0],fData[1],fData[2]);
        posData +=  _poitionPointer._stride;
        fData   =   (float*)(posData);



        CELL::float3  p21 (fData[0],fData[1],fData[2]);
        posData +=  _poitionPointer._stride;

        p01     =   p01 * _matModel;
        p11     =   p11 * _matModel;
        p21     =   p21 * _matModel;

//             if (   _frust.pointInFrustum(p01)
//                 || _frust.pointInFrustum(p11)
//                 || _frust.pointInFrustum(p21)
//                 )
        if (true)
        {
            p01     =   piplineTransform(p01);
            p11     =   piplineTransform(p11);
            p21     =   piplineTransform(p21);
            //! 转化为屏幕坐标
            CELL::int2    p0(p01.x,p01.y);
            CELL::int2    p1(p11.x,p11.y);
            CELL::int2    p2(p21.x,p21.y);

            CELL::Rgba    c0 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;
            CELL::Rgba    c1 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;
            CELL::Rgba    c2 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;

            float*  pUV     =   (float*)uvData;
            CELL::float2  uv0 (pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;
            pUV     =   (float*)uvData;
            CELL::float2  uv1(pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;
            pUV     =   (float*)uvData;
            CELL::float2  uv2(pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;


            Ege eges[3]  =
            {
                Ege(p0.x,p0.y,c0,  uv0, p1.x,p1.y,c1,  uv1),
                Ege(p1.x,p1.y,c1,  uv1, p2.x,p2.y,c2,  uv2),
                Ege(p2.x,p2.y,c2,  uv2, p0.x,p0.y,c0,  uv0),
            };
            drawTrianle(eges);

            if (_colorPointer._data == 0)
            {
                cData   =   (char*)colorPointerdesc._data;
            }
            if (_uvPointer._data == 0 )
            {

                uvData  =   (char*)uvPointerdesc._data;
            }
        }
    }
}
//!管线变换,世界坐标转换成屏幕坐标
CELL::float3 Raster::piplineTransform(CELL::float3 pos)
{
    CELL::float4  world(pos.x,pos.y,pos.z,1);

    CELL::float4  screen  =   _matProjView * world;
    if (screen.w == 0.0f)
    {
        return false;
    }
    screen.x    /=  screen.w;
    screen.y    /=  screen.w;
    screen.z    /=  screen.w;

    // map to range 0 - 1
    screen.x    =   screen.x * 0.5f + 0.5f;
    screen.y    =   screen.y * 0.5f + 0.5f;
    screen.z    =   screen.z * 0.5f + 0.5f;

    // map to viewport
    screen.x    =   screen.x * _viewPort.x;
    screen.y    =   _height - screen.y * _viewPort.y;

    return  CELL::float3(screen.x,screen.y,screen.z);
}
//!绘制三角形，一般画图时，是以一个三角形为一个元
void Raster::drawTrianle(Ege eges[])
{
    int iMax    =   0;
    int length  =   eges[0]._y2 - eges[0]._y1;

    for (int i = 1 ;i < 3 ; ++ i)
    {
        int len =   eges[i]._y2 - eges[i]._y1;
        if (len > length)
        {
            length  =   len;
            iMax    =   i;
        }
    }
    int iShort1 =   (iMax + 1)%3;
    int iShort2 =   (iMax + 2)%3;

    drawEge(eges[iMax],eges[iShort1],_texture);
    drawEge(eges[iMax],eges[iShort2],_texture);
}

void Raster::drawEge(const Ege &e1, const Ege &e2, IMAGE *image)
{
    float   yOffset1    =   e1._y2 - e1._y1;
    if (yOffset1 == 0)
    {
        return;
    }

    float   yOffset     =   e2._y2 - e2._y1;
    if (yOffset == 0)
    {
        return;
    }
    float   xOffset =   e2._x2 - e2._x1;
    float   scale   =   0;
    float   step    =   1.0f/yOffset;
    int     startY  =   CELL::tmax<int>(e2._y1,0);
    int     endY    =   CELL::tmin<int>(e2._y2,_height);
    scale   +=  (startY - e2._y1)/yOffset;


    float   xOffset1    =   e1._x2 - e1._x1;
    float   scale1      =   (float)(startY - e1._y1)/yOffset1;
    float   step1       =   1.0f/yOffset1;


    for (int y = startY ; y < endY ; ++ y)
    {
        int     x1      =   e1._x1 + (int)(scale1 * xOffset1);
        int     x2      =   e2._x1 + (int)(scale * xOffset);
        CELL::Rgba4Byte    color2  =   CELL::colorLerp(e2._color1,e2._color2,scale);
        CELL::Rgba4Byte    color1  =   CELL::colorLerp(e1._color1,e1._color2,scale1);

        CELL::float2  uvStart =   CELL::uvLerp(e1._uv1,e1._uv2,scale1);
        CELL::float2  uvEnd   =   CELL::uvLerp(e2._uv1,e2._uv2,scale);

        Span    span(x1,x2,y,color1,color2,uvStart,uvEnd);
        drawSpan(span,image);

        scale   +=  step;
        scale1  +=  step1;

    }
}

void Raster::drawSpan(const Span &span, IMAGE *image)
{
    float   length  =   span._xEnd - span._xStart;
    float   scale   =   0;
    float   step    =   1.0f/length;

    int     startX  =   CELL::tmax<int>(span._xStart,0);
    int     endX    =   CELL::tmin<int>(span._xEnd,_width);
    scale   +=  (startX - span._xStart)/length;

    for (int x = startX ; x < endX; ++ x)
    {
        //Rgba    color   =   colorLerp(span._colorStart,span._colorEnd,scale);

        CELL::float2  uv      =   CELL::uvLerp(span._uvStart,span._uvEnd,scale);

        CELL::Rgba4Byte    pixel   =   image->setUVPixel(uv.x,uv.y);

        scale   +=  step;

        setPixelEx(x,span._y,pixel);
    }
}

void Raster::setPixelEx(unsigned x, unsigned y, CELL::Rgba color)
{
    _buffer[y * _width + x]   =   color._color;
}

CELL::Rgba Raster::getPixel(unsigned x, unsigned y)
{
    return  CELL::Rgba(_buffer[y * _width + x]);
}

void Raster::bindTexture( IMAGE* image )
{
    _texture    =   image;
}

void Raster::textureCoordPointer( int size,DATETYPE type,int stride,const void* data )
{
    _uvPointer._size        =   size;
    _uvPointer._type        =   type;
    _uvPointer._stride      =   stride;
    _uvPointer._data        =   data;
}

void Raster::colorPointer( int size,DATETYPE type,int stride,const void* data )
{
    _colorPointer._size     =   size;
    _colorPointer._type     =   type;
    _colorPointer._stride   =   stride;
    _colorPointer._data     =   data;
}

void Raster::vertexPointer( int size,DATETYPE type,int stride,const void* data )
{
    _poitionPointer._size   =   size;
    _poitionPointer._type   =   type;
    _poitionPointer._stride =   stride;
    _poitionPointer._data   =   data;
}



void Raster::loadMatrix( const CELL::matrix4& mat )
{
    _matModel   =   mat;
}

void Raster::loadIdentity()
{
    _matModel   =   CELL::matrix4(1);
}

void Raster::loadProjMatrix( const CELL::matrix4& mat )
{
    _matProj    =   mat;
}
void Raster::loadProjIdentity( )
{
    _matProj    =   CELL::matrix4(1);
}


void Raster::loadViewMatrix( const CELL::matrix4& mat )
{
    _matView    =   mat;
}
void Raster::loadViewIdentity( )
{
    _matView    =   CELL::matrix4(1);
}

void Raster::setPerspective( float fovy, float aspect, float zNear, float zFar )
{
    _matProj    =   CELL::perspective<float>(fovy,aspect,zNear,zFar);
}
//!设置视角
void Raster::lookat( CELL::float3 const & eye,CELL::float3 const & center,CELL::float3 const & up )
{
    _matView    =   CELL::lookAt(eye,center,up);
}


void Raster::setView( const CELL::matrix4& viewMat )
{
    _matView    =   viewMat;
}


void Raster::setViewPort( int x,int y,int w,int h )
{
    _viewPort.x =   w;
    _viewPort.y =   h;
}

//! Span跨度,这里表示水平线，由两个不同的x值和相同的y位置坐标，颜色，文理一起组成
Span::Span(int xStart, int xEnd, int y, CELL::Rgba4Byte colorStart, CELL::Rgba4Byte colorEnd, CELL::float2 uvStart, CELL::float2 uvEnd)
{
    if (xStart < xEnd)
    {
        _xStart     =   xStart;
        _xEnd       =   xEnd;
        _colorStart =   colorStart;
        _colorEnd   =   colorEnd;

        _uvStart    =   uvStart;
        _uvEnd      =   uvEnd;

        _y          =   y;
    }
    else
    {
        _xStart     =   xEnd;
        _xEnd       =   xStart;

        _colorStart =   colorEnd;
        _colorEnd   =   colorStart;

        _uvStart    =   uvEnd;
        _uvEnd      =   uvStart;
        _y          =   y;
    }
}
//!ege预计级别当量，这里表示线,由位置坐标，颜色，文理坐标组成
Ege::Ege(int x1, int y1, CELL::Rgba4Byte color1, CELL::float2 uv1, int x2, int y2, CELL::Rgba4Byte color2, CELL::float2 uv2)
{
    if (y1 < y2)
    {
        _x1     =   x1;
        _y1     =   y1;
        _uv1    =   uv1;
        _color1 =   color1;

        _x2     =   x2;
        _y2     =   y2;
        _uv2    =   uv2;
        _color2 =   color2;
    }
    else
    {
        _x1     =   x2;
        _y1     =   y2;
        _uv1    =   uv2;
        _color1 =   color2;

        _x2     =   x1;
        _y2     =   y1;
        _uv2    =   uv1;
        _color2 =   color1;
    }
}
