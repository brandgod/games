#pragma once

#include <vector>
#include "../../../../Games101/tinyrender/tinyrenderer-f6fecb7ad493264ecd15e230411bfb1cca539a12/geometry.h"
#include <iostream>

template <class T>
class Matrix4 {
private:
	T* matrix;

protected:
	static void throwException(std::string info) {
		try {
			throw info;
		}
		catch (std::string e) {
			std::cerr << e << std::endl;
			exit(1);
		}
	}

public:
	// size_x代表有几行，size_y代表有几列
	int size_x, size_y;
	int tail;

	/**
	 * @brief 根据点坐标初始化成4行1列的齐次坐标
	 * @param point 点
	*/
	Matrix4(Vec3<T> point, T flag) {
		size_x = 4;
		size_y = 1;
		tail = 0;
		matrix = new T[4];
		matrix[0] = point.x;
		matrix[1] = point.y;
		matrix[2] = point.z;
		matrix[3] = flag;
	}

	/**
	 * @brief 初始化成x行y列大小的零矩阵
	 * @param size_x 行
	 * @param size_y 列
	*/
	/*Matrix4(int size_x, int size_y) {
		this->size_x = size_x;
		this->size_y = size_y;
		matrix = new T[size_x * size_y];
		for (int i = 0; i < size_x * size_y; i++) {
			matrix[i] = 0;
		}
	}*/

	/**
	 * @brief 初始化成4行4列大小的零矩阵
	*/
	Matrix4() {
		size_x = 4;
		size_y = 4;
		tail = 0;
		matrix = new T[16];
		for (int i = 0; i < 16; i++) {
			matrix[i] = 0;
		}
	}

	/**
	 * @brief 取索引处数值
	 * @param x 横坐标索引（从0开始）
	 * @param y 纵坐标索引（从1开始）
	 * @return
	*/
	T get(int x, int y) { return matrix[x * size_y + y]; }

	Vec3<T> getVec3() {
		if (size_y == 1) 
			return matrix[3] != 0 ? Vec3<T>((T)(matrix[0]/(float)matrix[3]), (T)(matrix[1]/ (float)matrix[3]), (T)(matrix[2]/ (float)matrix[3])): Vec3<T>(matrix[0], matrix[1], matrix[2]);
		else throwException("Matrix4 Error: 不是一个点/向量坐标！");
	}

	bool isPoint() {
		if (size_y == 1) return matrix[size_x * size_y - 1] == 1 ? true : false;
		else throwException("Matrix4 Error: 不是一个点/向量坐标！");
	}

	void set(int x, int y, T val) { matrix[x * size_y + y] = val; }
	void set(T val) { matrix[tail++] = val; }

	static Matrix4<T> Identity() {
		Matrix4<T> id;
		id.set(0, 0, 1);
		id.set(1, 1, 1);
		id.set(2, 2, 1);
		id.set(3, 3, 1);
		return id;
	}

	void reset() {
		tail = 0;
		for (int i = 0; i < size_x * size_y; i++) {
			matrix[i] = 0;
		}
	}

	/*std::string toString() {
		std::string str = "数组内容如下：\n";
		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y; j++) {
				if (j != size_y - 1) {
					str += std::to_string(matrix[i * size_y + j]) + " ";
				}
				else {
					str += std::to_string(matrix[i * size_y + j]) + "\n";
				}
			}
		}
		return str;
	};*/

	friend Matrix4<T>& operator>> (Matrix4<T>& mar, T val) {
		mar.set(val);
		return mar;
	}

	friend std::ostream& operator<<(std::ostream& out, const Matrix4<T>& mar) {
		out << "数组内容如下：\n";
		for (int i = 0; i < mar.size_x; i++) {
			for (int j = 0; j < mar.size_y; j++) {
				if (j != mar.size_y - 1) {
					out << mar.matrix[i * mar.size_y + j] << " ";
				}
				else {
					out << mar.matrix[i * mar.size_y + j] << "\n";
				}
			}
		}
		return out;
	}

	friend Matrix4<T> operator*(Matrix4<T> matrixA, Matrix4<T> matrixB) {
		int by = matrixB.size_y;
		if (by == 1) {
			Vec3<T> vec;
			T fg = 0;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < matrixA.size_y; j++) {
					if (i < 3) vec.raw[i] += matrixA.get(i, j) * matrixB.get(j, 0);
					else {
						fg += matrixA.get(i, j) * matrixB.get(j, 0);
						//std::cout << matrixA.get(i, j) << " " << matrixB.get(i, 0);
					}
				}
			}
			//std::cout << fg;
			return Matrix4<T>(vec, fg);
		}
		else {
			if (by == 4) {
				Matrix4<T> mat;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						T res = 0;
						for (int k = 0; k < 4; k++) {
							if (matrixA.size_y == 1) res += matrixA.get(i, 0) * matrixB.get(k, j);
							else res += matrixA.get(i, k) * matrixB.get(k, j);
						}
						mat.set(i, j, res);
					}
				}
				return mat;
			}
			else {
				return Matrix4<T>::Identity();
			}
		}
	}

	friend Matrix4<T> operator+(Matrix4<T> matrixA, Matrix4<T> matrixB) {
		int ay = matrixA.size_y, by = matrixA.size_y;
		if (ay == 1 and by == 1) {
			Vec3<T> vec(matrixA.get(0, 0) + matrixB.get(0, 0), matrixA.get(1, 0) + matrixB.get(1, 0), matrixA.get(2, 0) + matrixB.get(2, 0));
			return Matrix4<T>(vec, matrixA.get(3, 0) + matrixB.get(3, 0));
		}
		else if (ay == 4 and by == 4) {
			Matrix4<T> mat;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					mat.set(i, j, matrixA.get(i, j) + matrixB.get(i, j));
				}
			}
			return mat;
		}
		else Matrix4<T>::throwException("Matrix4 Error: 相加维度不同！");
	}

	friend Matrix4<T> operator-(Matrix4<T> matrix) {
		if (matrix.size_y == 1) {
			return Matrix4<T>(Vec3<T>(matrix.getVec3() * (-1)), matrix.get(3, 0));
		}
		else {
			Matrix4<T> mat;
			for (int i = 0; i < matrix.size_x; i++) {
				for (int j = 0; j < matrix.size_y; j++) {
					mat.set(i, j, -matrix.get(i, j));
				}
			}
			return mat;
		}
	}
};

typedef Matrix4<int> Matrix4i;
typedef Matrix4<float> Matrix4f;
