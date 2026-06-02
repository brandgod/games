#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } 
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f, t, n;
            int idx, text_idx, norm_idx;
            iss >> trash;
            while (iss >> idx >> trash >> text_idx >> trash >> norm_idx) {
                idx--, text_idx--, norm_idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                t.push_back(text_idx);
                n.push_back(norm_idx);
            }
            faces_.push_back(f);
            face_texture_index_.push_back(t);
            vert_norm_index_.push_back(t);
        }
        else if (!line.compare(0, 3, "vt ")) {
            std::string head;
            iss >> head;
            Vec3f t;
            for (int i = 0; i < 3; i++) iss >> t.raw[i];
            texture_verts_.push_back(t);
        }
        else if (!line.compare(0, 3, "vn ")) {
            std::string head;
            iss >> head;
            Vec3f n;
            for (int i = 0; i < 3; i++) iss >> n.raw[i];
            vert_norms_.push_back(n);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << texture_verts_.size() << " vn# " << vert_norms_.size() << std::endl;

    load_texture(filename, "_diffuse.tga", diffusemap);
    //load_texture(filename, "_nm_tangent.tga", normalmap);
    //load_texture(filename, "_spec.tga", specularmap);
}

void Model::load_texture(std::string filename, const std::string suffix, TGAImage& img) {
    size_t dot = filename.find_last_of(".");
    if (dot == std::string::npos) return;
    std::string texfile = filename.substr(0, dot) + suffix;
    std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::ntextureVerts()
{
    return texture_verts_.size();
}

int Model::nNorms()
{
    return vert_norms_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::faceTexture(int idx)
{
    return face_texture_index_[idx];
}

std::vector<int> Model::faceVertNorm(int idx)
{
    return vert_norm_index_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::textureVert(int idx)
{
    return texture_verts_[idx];
}

Vec3f Model::vertNorm(int idx)
{
    return vert_norms_[idx];
}

