#pragma once

#include "../../../../Games101/tinyrender/tinyrenderer-909fe20934ba5334144d2c748805690a1fa4c89f/tgaimage.h"
#include "../../../../Games101/tinyrender/tinyrenderer-f6fecb7ad493264ecd15e230411bfb1cca539a12/model.h"
#include "matrix4.h"
//#include <mutex>


class Painter {
public:
	/**
	 * @brief 深度测试缓冲
	*/
	int* zbuffer;
	/**
	 * @brief 图像缓冲
	*/
	TGAImage& image;
	/**
	 * @brief 屏幕空间深度
	*/
	int depth;

	/**
	 * @brief 模型
	*/
	Model* model;
	/**
	 * @brief 模型是否有漫反射纹理贴图
	*/
	bool hasDiffusedTexture;

	/**
	 * @brief 光照
	*/
	Vec3f light;
	
	/**
	 * @brief 相机
	*/
	Camera cam;

	// 目前的立方体就是一个标准立方体，想要进行透视投影、仿射变换的操作可以直接进行
	
	/**
	 * @brief 模型变换矩阵
	*/
	Matrix4f modelTransform = Matrix4f::Identity();
	/**
	 * @brief 视角变换矩阵
	*/
	Matrix4f viewTransform = Matrix4f::Identity();
	/**
	 * @brief 投影矩阵
	*/
	Matrix4f projectionTransform = Matrix4f::Identity();
	/**
	 * @brief 最终生成的矩阵
	*/
	Matrix4f finalTransformation = Matrix4f::Identity();
	
	/*8**********************构造函数***************************************/
	Painter(int* zbuf, TGAImage& img) : zbuffer(zbuf), 
		image(img), depth(800), light(Vec3f(0, 0, -1)), 
		hasDiffusedTexture(false), model(nullptr) {};
	/*8************************end*****************************************/


	/************************负责变换矩阵功能**********************************/
	Matrix4f generateTransformationMatrix();

	Vec3f generateTransformation(Vec3f pointVec);
	/*8***************************end****************************************/

	
	/*8***********************绘制函数*****************************************/
	/**
	 * @brief 布雷森汉姆直线算法
	 * @param x0 
	 * @param y0 
	 * @param z0 
	 * @param x1 
	 * @param y1 
	 * @param z1 
	 * @param color 绘制颜色
	*/
	void drawLine(int x0, int y0, int z0, int x1, int y1, int z1, TGAColor color, float intensity);

	/**
	 * @brief 绘制模型
	 * @param model 模型指针
	 * @param color 绘制颜色
	 * @param isFilled 是否填充三角形指标（false为不填充）
	*/
	void drawModelMesh(TGAColor color, bool isFilled);

	/**
	 * @brief 绘制三角形
	 * @param tri 三角形对象，包含顶点信息
	 * @param color 颜色
	 * @param isOutline 是否为轮廓指标（false则不是轮廓） 
	*/
	void drawTriangle(Triangle<float> tri, TGAColor color, float intensity, bool isOutline, int face);

	/**
	 * @brief 单独绘制一个点，使用了互斥锁保证zbuffer不会被多线程共享
	 * @param x 
	 * @param y 
	 * @param z 
	 * @param color 
	*/
	void drawPoint(int x, int y, int z, TGAColor color, float intensity, int face);
	/*8**************************end******************************************/
};
