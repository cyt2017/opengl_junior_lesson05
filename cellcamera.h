#ifndef CELLCAMERA_H
#define CELLCAMERA_H

#include "tool/CELLMath.hpp"
class CELLCamera
{
public:
    CELL::float3      _eye;
    CELL::float3      _target;
    CELL::float3      _up;

    CELL::float3      _right;

    CELL::float3      _dir;
    CELL::matrix4     _matView;

    CELL::matrix4     _matProj;
    CELL::matrix4     _matWorld;
    CELL::float2      _viewSize;
public:
    CELLCamera(const CELL::float3& target = CELL::float3(0,0,0),const CELL::float3& eye = CELL::float3(0,100,100),const CELL::float3& right = CELL::float3(1,0,0));
    ~CELLCamera()
    {}

    CELL::float3 getEye() const;
    /**
    *   设置眼睛的位置
    */
    void    setEye(CELL::float3 val);

    CELL::float3 getTarget() const;

    void    setTarget(CELL::float3 val);
    void    setRight(CELL::float3 val);

    CELL::float3 getUp() const;
    void    setUp(CELL::float3 val);
    CELL::float3  getDir() const;

    CELL::float3  getRight() const;
    void    setViewSize(const CELL::float2& viewSize);
    void    setViewSize(float x,float y);

    CELL::float2  getViewSize();

    void    setProject(const CELL::matrix4& proj);
    const CELL::matrix4& getProject() const;
    const CELL::matrix4&  getView() const;

    /**
    *   正交投影
    */
    void    ortho( float left, float right, float bottom, float top, float zNear, float zFar );
    /**
    *   透视投影
    */
    void    perspective(float fovy, float aspect, float zNear, float zFar);

     /**
    *   世界坐标转化为窗口坐标
    */
    bool    project( const CELL::float4& world, CELL::float4& screen );


    /**
    *   世界坐标转化为窗口坐标
    */
    CELL::float2  worldToScreen( const CELL::float3& world);

    /**
    *   窗口坐标转化为世界坐标
    */
    CELL::float3  screenToWorld(const CELL::float2& screen);

    CELL::float3  screenToWorld(float x,float y);


    /**
    *   窗口坐标转化为世界坐标
    */
    bool    unProject( const CELL::float4& screen, CELL::float4& world );


    CELL::Ray createRayFromScreen(int x,int y);
    /**
    *   下面的函数的功能是将摄像机的观察方向绕某个方向轴旋转一定的角度
    *   改变观察者的位置，目标的位置不变化
    */
    virtual void    rotateViewY(float angle);

    virtual void    rotateViewX(float angle);

    virtual void    update();
};

#endif // CELLCAMERA_H
