#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include "tool/CELLMath.hpp"
#include <FreeImage.h>

class IMAGE
{
protected:
    int     _width;
    int     _height;
    uint*   _pixel;
    int     _wrapType;
public:
    IMAGE(int w,int h,void* data);
    ~IMAGE();

    void    setWrapType(int type);
    int     w() const;
    int     h() const;

    CELL::Rgba4Byte    setPixel(int x,int y) const;
    CELL::Rgba4Byte    setUVPixel(float u,float v);

public:
    static  IMAGE*  loadFromFile(const char*fileName);

};

#endif // IMAGE_H
