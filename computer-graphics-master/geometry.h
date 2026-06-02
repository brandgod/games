#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief 二维向量类模板
 * @tparam t int、float
*/
template <class t> struct Vec2 {
	union {
		struct {t u, v;};

		struct {t x, y;};
		t raw[2];
	};
	Vec2() : x(0), y(0) {}
	Vec2(t _x, t _y) : x(_x),y(_y) {}

	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(x+V.x, y+V.y); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(x-V.x, y-V.y); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(x*f, y*f); }
	/**
	 * @brief 点乘
	 * @param v 
	 * @return 点乘积
	*/
	inline t       operator *(const Vec2<t>& v) const { return x * v.x + y * v.y; }
	/**
	 * @brief 叉乘
	 * @param v 
	 * @return 叉乘积（二维向量叉乘得到一个值）
	*/
	inline t operator ^(const Vec2<t>& v) const { return x * v.y - y * v.x; }

	float dotProduct(Vec2<t> p) { return (float)(x * p.x + y * p.y); };
	float crossProduct(Vec2<t> p) { return (float)(x * p.y - y * p.x); };
	template <class t> friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

/**
 * @brief 三维向量类模板
 * @tparam t int、float
*/
template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	/**
	 * @brief 叉乘
	 * @param v 
	 * @return 一个向量
	*/
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	/**
	 * @brief 点乘
	 * @param v 
	 * @return 一个值
	*/
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class t> friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

// /////////////////////////////////////////////////////////////////////////////

/**
 * @brief 三角形的类模板
 * @tparam T int、float
*/
template <class T> 
class Triangle
{
public:
	Vec2<T> t0, t1, t2;
	Vec3<T> t0_3, t1_3, t2_3;
	Triangle(Vec2<T> p0, Vec2<T> p1, Vec2<T> p2) :t0(p0), t1(p1), t2(p2) {
		t0_3 = Vec3<T>(p0.x, p0.y, 0);
		t1_3 = Vec3<T>(p1.x, p1.y, 0);
		t2_3 = Vec3<T>(p2.x, p2.y, 0);
	};
	Triangle(Vec3<T> p0, Vec3<T> p1, Vec3<T> p2) :t0_3(p0), t1_3(p1), t2_3(p2) {
		t0 = Vec2<T>(p0.x, p0.y);
		t1 = Vec2<T>(p1.x, p1.y);
		t2 = Vec2<T>(p2.x, p2.y);
	};
	virtual ~Triangle() {};

	/**
	 * @brief 判断点p是否在三角形内
	 * @param p 
	 * @return 
	*/
	bool isInTriangle(Vec2<T> p) {
		float dot1 = (float)((p - t0)^(t1 - t0));
		float dot2 = (float)((p - t1)^(t2 - t1));
		float dot3 = (float)((p - t2)^(t0 - t2));

		if (dot1 * dot2 > 0 and dot2 * dot3 > 0 and dot3 * dot1 > 0) return true;

		return false;
	}

	/**
	 * @brief 寻找此三角形的包围盒（三维）
	 * @return 左下右上两个点
	*/
	std::vector<Vec3<T>> findBoundingBox() {
		int max_x = -INT_MAX, max_y = -INT_MAX, min_x = INT_MAX, min_y = INT_MAX;
		max_x = std::max(max_x, (int)t0.x);
		max_x = std::max(max_x, (int)t1.x);
		max_x = std::max(max_x, (int)t2.x);

		min_x = std::min(min_x, (int)t0.x);
		min_x = std::min(min_x, (int)t1.x);
		min_x = std::min(min_x, (int)t2.x);

		max_y = std::max(max_y, (int)t0.y);
		max_y = std::max(max_y, (int)t1.y);
		max_y = std::max(max_y, (int)t2.y);

		min_y = std::min(min_y, (int)t0.y);
		min_y = std::min(min_y, (int)t1.y);
		min_y = std::min(min_y, (int)t2.y);

		int max_z = -INT_MAX, min_z = INT_MAX;

		max_z = std::max(max_z, (int)t0_3.z);
		max_z = std::max(max_z, (int)t1_3.z);
		max_z = std::max(max_z, (int)t2_3.z);

		min_z = std::min(min_z, (int)t0_3.z);
		min_z = std::min(min_z, (int)t1_3.z);
		min_z = std::min(min_z, (int)t2_3.z);
		std::vector<Vec3<T>> box{ Vec3<T>(min_x, min_y, min_z), Vec3<T>(max_x, max_y, max_z)};
		/*std::cout << max_x << " " << min_x << " " << max_y << " " << min_y << std::endl;*/
		return box;
	}

	/**
	 * @brief 计算点p在三角形内的重心坐标
	 * @param p 
	 * @return 一个三维向量（如果任意一维小于0则表示不在三角形内）
	*/
	Vec3f barycentric(Vec2<T> p) {
		// 计算叉积（具体原理看教程第二节）
		Vec3f u = Vec3f(t2.x - t0.x, t1.x - t0.x, t0.x - p.x) ^ Vec3f(t2.y - t0.y, t1.y - t0.y, t0.y - p.y);
		// u.z小于1因为精度问题可以认为变成了0，因此返回负值
		if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
		// 返回重心坐标
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}

private:

};

// //////////////////////////////////////////////////////////////////////////////////////


struct Camera
{
	Vec3f position;
	Vec3f upDirection;
	Vec3f lookAt;

	Camera() {
		position.x = 0;
		position.y = 0;
		position.z = 5;

		upDirection.x = 0;
		upDirection.y = 1;
		upDirection.z = 0;

		lookAt.x = 0;
		lookAt.y = 0;
		lookAt.z = -1;
	}
};

#endif //__GEOMETRY_H__
