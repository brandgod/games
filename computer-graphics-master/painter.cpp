#include "painter.h"
#include <iostream>
#include <omp.h>
#include "utils.h"

/**
 * @brief 互斥锁，防止关键操作被线程共享
*/
std::mutex mtx;

Matrix4f Painter::generateTransformationMatrix()
{
	// 模型变换直接在矩阵中写出来就行

	// 视角变换
	// 先计算出lookAt与upDirection叉乘得到的向量（朝向相机右侧）
	Vec3f right = (cam.lookAt^cam.upDirection).normalize();
	// 对视角的变换应该是围绕相机进行，所以要先把相机移动到原点，相应的，所有其他物体都要移动
	//viewTransform.set(0, 3, -cam.position.x);
	//viewTransform.set(1, 3, -cam.position.y);
	//viewTransform.set(2, 3, -cam.position.z);
	viewTransform.set(0, 0, right.x);
	viewTransform.set(0, 1, right.y);
	viewTransform.set(0, 2, right.z);
	viewTransform.set(1, 0, cam.upDirection.x);
	viewTransform.set(1, 1, cam.upDirection.y);
	viewTransform.set(1, 2, cam.upDirection.z);
	viewTransform.set(2, 0, -cam.lookAt.x);
	viewTransform.set(2, 1, -cam.lookAt.y);
	viewTransform.set(2, 2, -cam.lookAt.z);
	Matrix4f viewMove = Matrix4f::Identity();
	viewMove.set(0, 3, -cam.position.x);
	viewMove.set(1, 3, -cam.position.y);
	viewMove.set(2, 3, -cam.position.z);
	viewTransform = viewTransform * viewMove;
	// 透视投影矩阵
	Matrix4f persProjection = Matrix4f::Identity();
	float near = -(cam.position.z-1);
	float far = -(cam.position.z+1);
	persProjection.set(2, 2, (near + far)/near);
	persProjection.set(2, 3, -far);
	persProjection.set(3, 2, 1 / near);
	persProjection.set(3, 3, 0);
	// 因为模型已经在标准立方体里，所以不再进行正交投影
	Matrix4f orthoProjection = Matrix4f::Identity();
	//Matrix4f orthoProjectionLeft = Matrix4f::Identity();

	Matrix4f adjust = Matrix4f::Identity();
	//adjust.set(2, 2, )
	//adjust.set(2, 3, -(near+far));

	//std::cout << orthoProjection << persProjection << viewTransform << modelTransform;

	finalTransformation = adjust * orthoProjection * persProjection * viewTransform * modelTransform;
	return finalTransformation;
}

Vec3f Painter::generateTransformation(Vec3f pointVec)
{
	Matrix4f point(pointVec, 1);
	Vec3f newp = (finalTransformation * point).getVec3();
	//std::cout << newp;
	//std::cout << finalTransformation << point << (finalTransformation * point) << newp;
	return newp;
}

void Painter::drawLine(int x0, int y0, int z0, int x1, int y1, int z1, TGAColor color, float intensity) {
	// 这个画法画出来的线条，如果每次y增长的长度大于x（x默认每次增加1）
	// 则y轴会不连续，因此需要将其倒转一下
	bool isSteep = false;

	// 高大于宽时倒转
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		isSteep = true;
	}
	// 让直线的绘制永远是从左到右
	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
		std::swap(z0, z1);
	}

	// 取消浮点数限制
	int dx = x1 - x0, dy = std::abs(y0 - y1), dz = std::abs(z0-z1);
	int epsilon = 0, epsilonz = 0;
	int y = y0, z = z0;
	int width = image.get_width();
	int height = image.get_height();
	for (int x = x0; x <= x1; x++) {
		if (isSteep) Painter::drawPoint(y, x, z, color, intensity, -1); // 如果倒转过，绘制时需要再倒转回来
		else Painter::drawPoint(x, y, z, color, intensity, -1);
		// 因为像素只能一个一个增长，而x每次必定增长1，因此让误差大于dx后使y增长1
		epsilon += dy;
		epsilonz += dz;

		if (epsilon >= dx) {
			y += (y0 < y1) ? 1 : -1;
			epsilon -= dx;
		}

		if (epsilonz >= dx) {
			z += (z0 < z1) ? 1 : -1;
			epsilonz -= dx;
		}
	}
}

void Painter::drawModelMesh(TGAColor color, bool isFilled=false)
{
	if (model == nullptr) {
		std::cerr << "没有给模型赋值！" << std::endl;
		return;
	}

	omp_set_num_threads(8);

	int width = image.get_width();
	int height = image.get_height();
	// 绘制过程并行化
	if (!isFilled) {
#pragma omp parallel for
		for (int i = 0; i < model->nfaces(); i++) { // 对每个面进行绘制
			std::vector<int> face = model->face(i);
			for (int j = 0; j < 3; j++) {
				// 绘制轮廓就是画三条线
				Vec3f v0 = model->vert(face[j]);
				Vec3f v1 = model->vert(face[(j + 1) % 3]);
				int x0 = (v0.x + 1.) * width / 2.;
				int y0 = (v0.y + 1.) * height / 2.;
				int x1 = (v1.x + 1.) * width / 2.;
				int y1 = (v1.y + 1.) * height / 2.;
				int z0 = (v0.z + 1.) * 800 / 2;
				int z1 = (v1.z + 1.) * 800 / 2;
				//std::cout << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
				Painter::drawLine(x0, y0, z0, x1, y1, z1, color, 1);
			}
		}
	}
	else {
		// 模型面数
		int nfaces = model->nfaces();
		// 进度条
		ProgressBar progress("渲染模型中", nfaces);
#pragma omp parallel for
		for (int i = 0; i < nfaces; i++) { // 绘制过程并行化
			std::vector<int> face = model->face(i);
			// 在世界中的坐标（规范化，在-1，1范围内）
			Vec3f world_coords[3];
			// 在屏幕中的坐标（需经过变换）
			Vec3f projection_coords[3];
			for (int j = 0; j < 3; j++) {
				world_coords[j] = model->vert(face[j]);
				world_coords[j] = generateTransformation(world_coords[j]);
				world_coords[j].z = -24 / (world_coords[j].z + 10);
				// 进行变换（将坐标从-1，1变换成0到长宽高的范围
				projection_coords[j] = Vec3f((world_coords[j].x + 1.) * width / 2., (world_coords[j].y + 1.) * height / 2., (world_coords[j].z + 1.) * depth / 2);
			}
			Triangle<float> tri(projection_coords[0], projection_coords[1], projection_coords[2]);
			// 计算三角形法向量（简单算法）
			Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
			// 归一化
			n.normalize();
			// 设置光照
			float intensity = n * light;
			if (intensity > 0) {
				Painter::drawTriangle(tri, color, intensity, false, i);
			}

			progress.updateProgressBar();
		}
	}
}

void Painter::drawTriangle(Triangle<float> tri, TGAColor color, float intensity, bool isOutline=true, int face=-1)
{
	if (!isOutline) {
		// 找到三角形的包围盒（三维）
		auto box = tri.findBoundingBox();
		//std::cout << box[0].x << " " << box[1].x << " " << box[0].y << " " << box[1].y << std::endl;
//#pragma omp parallel for
		for (int i = box[0].x; i <= box[1].x; i++) { // 并行化
			for (int j = box[0].y; j <= box[1].y; j++) {
				if (tri.isInTriangle(Vec2f(i, j))) { // 当此像素点在三角形内时，绘制此点（深度测试均交给点绘制函数）
					Vec3f bcentric = tri.barycentric(Vec2f(i, j));
					if (bcentric.x < 0 || bcentric.y < 0 || bcentric.z < 0) continue;
					float z = tri.t0_3.z * bcentric.x + tri.t1_3.z * bcentric.y + tri.t2_3.z * bcentric.z;
					if (face < 0) Painter::drawPoint(i, j, (int)z, color, intensity, -1);
					else Painter::drawPoint(i, j, (int)z, color, intensity, face);
				}
			}
		}
	}
	else {
		Vec3i t0 = Vec3i(tri.t0.x, tri.t0.y, tri.t0_3.z), t1 = Vec3i(tri.t1.x, tri.t1.y, tri.t1_3.z), t2 = Vec3i(tri.t2.x, tri.t2.y, tri.t2_3.z);
		Painter::drawLine(t0.x, t0.y, t0.z, t1.x, t1.y, t1.z, color, intensity);
		Painter::drawLine(t1.x, t1.y, t1.z, t2.x, t2.y, t2.z, color, intensity);
		Painter::drawLine(t2.x, t2.y, t2.z, t0.x, t0.y, t0.z, color, intensity);
	}
	
}

void Painter::drawPoint(int x, int y, int z, TGAColor color, float intensity, int face=-1)
{
	if (x >= 0 and x <= image.get_width() and y >= 0 and y <= image.get_height()) {
		mtx.lock();
		bool shouldChangeColor = (zbuffer[x * image.get_width() + y] < z);
		mtx.unlock();
		// 进行深度测试，为了防止多线程共享深度测试缓冲，使用了互斥锁
		if (shouldChangeColor) {
			// 缓冲里的深度低于当前点深度时，将当前像素颜色设为当前点颜色
			if (face < 0) {
				mtx.lock();
				if (zbuffer[x * image.get_width() + y] < z) image.set(x, y, color * intensity);
				mtx.unlock();
			}
			else {
				// 当前面的纹理坐标索引
				std::vector<int> text_coord_indexs = model->faceTexture(face);
				// 根据索引得到纹理坐标
				Vec3f t0 = model->textureVert(text_coord_indexs[0]);
				Vec3f t1 = model->textureVert(text_coord_indexs[1]);
				Vec3f t2 = model->textureVert(text_coord_indexs[2]);
				// 获取当前面的坐标，变换成屏幕坐标
				std::vector<int> face_coords = model->face(face);
				Vec3f world_coords[3];
				Vec3f projection_coords[3];
				for (int j = 0; j < 3; j++) {
					world_coords[j] = model->vert(face_coords[j]);
					projection_coords[j] = Vec3f((world_coords[j].x + 1.) * image.get_width() / 2., (world_coords[j].y + 1.) * image.get_height() / 2., (world_coords[j].z + 1.) * depth / 2);
				}
				// 计算重心坐标
				Triangle<float> tri(projection_coords[0], projection_coords[1], projection_coords[2]);
				Vec3f bc_coords = tri.barycentric(Vec2f(x, y));
				if (bc_coords.x < 0 or bc_coords.y < 0 or bc_coords.z < 0) {}
				else {
					float u = t0.x * bc_coords.x + t1.x * bc_coords.y + t2.x * bc_coords.z;
					float v = t0.y * bc_coords.x + t1.y * bc_coords.y + t2.y * bc_coords.z;
					TGAColor diffusedColor = model->diffuse().get((int)(u * model->diffuse().get_width()),
						(int)std::abs(model->diffuse().get_height() - v * model->diffuse().get_height()));
					mtx.lock();
					if (zbuffer[x * image.get_width() + y] < z) 
						image.set(x, y, diffusedColor * intensity);
					mtx.unlock();
				}
			}
			mtx.lock();
			if (zbuffer[x * image.get_width() + y] < z)
				zbuffer[x * image.get_width() + y] = z;
			mtx.unlock();
		}
	}
}

