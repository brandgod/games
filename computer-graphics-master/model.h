#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "../tinyrenderer-909fe20934ba5334144d2c748805690a1fa4c89f/tgaimage.h"

class Model {
private:
	/**
	 * @brief 顶点坐标数组
	*/
	std::vector<Vec3f> verts_;

	std::vector<Vec3f> texture_verts_;

	std::vector<Vec3f> vert_norms_;
	/**
	 * @brief 面数组
	*/
	std::vector<std::vector<int> > faces_;

	std::vector<std::vector<int>> face_texture_index_;

	std::vector<std::vector<int>> vert_norm_index_;

	TGAImage diffusemap{};         // diffuse color texture
	TGAImage normalmap{};          // normal map texture
	TGAImage specularmap{};        // specular map texture

	void load_texture(const std::string filename, const std::string suffix, TGAImage& img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int ntextureVerts();
	int nNorms();
	Vec3f vert(int idx);
	Vec3f textureVert(int idx);
	Vec3f vertNorm(int idx);
	std::vector<int> face(int idx);
	std::vector<int> faceTexture(int idx);
	std::vector<int> faceVertNorm(int idx);

	TGAImage diffuse() { return diffusemap; }
	TGAImage specular() { return specularmap; }
};

#endif //__MODEL_H__
