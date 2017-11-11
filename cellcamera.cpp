#include "cellcamera.h"


CELLCamera::CELLCamera(const CELL::float3 &target, const CELL::float3 &eye, const CELL::float3 &right)
{
    _viewSize   =   CELL::float2(256,256);
    _matView    =   CELL::matrix4(1);
    _matProj    =   CELL::matrix4(1);
    _matWorld   =   CELL::matrix4(1);//单位矩阵

    _target     =   target;//目标
    _eye        =   eye;
    _dir        =   CELL::normalize(_target - _eye);//产生一条由镜头到目标的射线,归一化(normalize)
    _right      =   right;
    //!通过镜头和目标产生的射线，假设为Z轴；(right)变量假设为X轴；求出Y轴
    _up         =   CELL::normalize(CELL::cross(_right,_dir));//叉乘(cross)的意义就是通过两个向量来确定一个新的向量,该向量与前两个向量都垂直

}

CELL::float3 CELLCamera::getEye() const
{
    return _eye;
}

void CELLCamera::setEye(CELL::float3 val)
{
    _eye    =   val;
}

CELL::float3 CELLCamera::getTarget() const
{
    return _target;
}

void CELLCamera::setTarget(CELL::float3 val)
{
    _target = val;
}

void CELLCamera::setRight(CELL::float3 val)
{
    _right  =   val;
}

CELL::float3 CELLCamera::getUp() const
{
    return _up;
}

void CELLCamera::setUp(CELL::float3 val)
{
    _up = val;
}

CELL::float3 CELLCamera::getDir() const
{
    return  _dir;
}

CELL::float3 CELLCamera::getRight() const
{
    return  _right;
}

void CELLCamera::setViewSize(const CELL::float2 &viewSize)
{
    _viewSize   =   viewSize;
}

void CELLCamera::setViewSize(float x, float y)
{
    _viewSize   =   CELL::float2(x,y);
}

CELL::float2 CELLCamera::getViewSize()
{
    return  _viewSize;
}

void CELLCamera::setProject(const CELL::matrix4 &proj)
{
    _matProj    =   proj;
}

const CELL::matrix4 &CELLCamera::getProject() const
{
    return  _matProj;
}

const CELL::matrix4 &CELLCamera::getView() const
{
    return  _matView;
}

void CELLCamera::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    //!形成正切投影
    _matProj    =   CELL::ortho(left,right,bottom,top,zNear,zFar);
}

void CELLCamera::perspective(float fovy, float aspect, float zNear, float zFar)
{
    //!形成透视投影
    _matProj    =   CELL::perspective<float>(fovy,aspect,zNear,zFar);
}

bool CELLCamera::project(const CELL::float4 &world, CELL::float4 &screen)
{
    //!由世界坐标转变成屏幕坐标,通过MVP矩阵变换
    screen  =   (_matProj * _matView * _matWorld) * world;
    if (screen.w == 0.0f)
    {
        return false;
    }
    //!进行归一化，4*4是表示成一个齐次坐标系的形式;目前得到x，y，z的范围(-1~1)
    screen.x    /=  screen.w;
    screen.y    /=  screen.w;
    screen.z    /=  screen.w;

    // map to range 0 - 1
    screen.x    =   screen.x * 0.5f + 0.5f;
    screen.y    =   screen.y * 0.5f + 0.5f;
    screen.z    =   screen.z * 0.5f + 0.5f;

    // map to viewport,//!  _viewSize = (256,256)
    screen.x    =   screen.x * _viewSize.x;
    screen.y    =   _viewSize.y - (screen.y * _viewSize.y);
    return  true;
}

CELL::float2 CELLCamera::worldToScreen(const CELL::float3 &world)
{
    CELL::float4  worlds(world.x,world.y,world.z,1);
    CELL::float4  screens;
    project(worlds,screens);
    return  CELL::float2(screens.x,screens.y);
}

CELL::float3 CELLCamera::screenToWorld(const CELL::float2 &screen)
{
    //!屏幕坐标转换到世界坐标
    CELL::float4  screens(screen.x,screen.y,0,1);
    CELL::float4  world;
    unProject(screens,world);
    return  CELL::float3(world.x,world.y,world.z);
}

CELL::float3 CELLCamera::screenToWorld(float x, float y)
{
    CELL::float4  screens(x,y,0,1);
    CELL::float4  world;
    unProject(screens,world);
    return  CELL::float3(world.x,world.y,world.z);
}

bool CELLCamera::unProject(const CELL::float4 &screen, CELL::float4 &world)
{
    CELL::float4 v;
    v.x =   screen.x;
    v.y =   screen.y;
    v.z =   screen.z;
    v.w =   1.0;

    // map from viewport to 0 - 1
    v.x =   (v.x) /_viewSize.x;
    v.y =   (_viewSize.y - v.y) /_viewSize.y;

    // map to range -1 to 1
    v.x =   v.x * 2.0f - 1.0f;
    v.y =   v.y * 2.0f - 1.0f;
    v.z =   v.z * 2.0f - 1.0f;

    //!矩阵的转置(inverse)
    CELL::matrix4  inverse = (_matProj * _matView * _matWorld).inverse();

    v   =   v * inverse;
    if (v.w == 0.0f)
    {
        return false;
    }
    world   =   v / v.w;
    return true;
}

CELL::Ray CELLCamera::createRayFromScreen(int x, int y)
{
    CELL::float4  minWorld;
    CELL::float4  maxWorld;

    CELL::float4  screen(float(x),float(y),0,1);//设置离（eye）近的面
    CELL::float4  screen1(float(x),float(y),1,1);//设置离（eye）远的面

    unProject(screen,minWorld);
    unProject(screen1,maxWorld);
    CELL::Ray     ray;
    ray.setOrigin(CELL::float3(minWorld.x,minWorld.y,minWorld.z));//!设置射线的起点

    CELL::float3  dir(maxWorld.x - minWorld.x,maxWorld.y - minWorld.y, maxWorld.z - minWorld.z);
    ray.setDirection(normalize(dir));//!设置射线的方向
    return  ray;
}

void CELLCamera::rotateViewY(float angle)
{
    //!设置沿Y旋转
    _dir        =   CELL::rotateY<float>(_dir,    angle);
    _up         =   CELL::rotateY<float>(_up,     angle);
    _right      =   CELL::normalize(CELL::cross(_dir,_up));
    float   len =   CELL::length(_eye - _target);
    _eye        =   _target - _dir * len;
    _matView    =   CELL::lookAt(_eye,_target,_up);
}

void CELLCamera::rotateViewX(float angle)
{
    //!设置沿X旋转
    CELL::matrix4 mat(1) ;
    mat.rotate(angle,_right);
    _dir        =   _dir * mat;
    _up         =   _up * mat;
    _right      =   CELL::normalize(cross(_dir,_up));
    float   len =   CELL::length(_eye - _target);
    _eye        =   _target - _dir * len;
    _matView    =   CELL::lookAt(_eye,_target,_up);
}

void CELLCamera::update()
{
    //!更新视角
    _matView    =   CELL::lookAt(_eye,_target,_up);
}
