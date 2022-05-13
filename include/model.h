#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <Eigen/Dense>

class Model {
   public:
    Model(const std::string& file_name);
    const int size() const;
    const Eigen::Matrix<int, 9, 1> indexAt(const int i) const;
    const Eigen::Matrix<double, 3, 1> vertexAt(const int i) const;
    const Eigen::Matrix<double, 3, 1> textureUVAt(const int i) const;
    const Eigen::Matrix<double, 3, 1> normalAt(const int i) const;

   private:
    Eigen::MatrixXi m_indices;
    Eigen::MatrixXd m_vertices;
    Eigen::MatrixXd m_texture_uvs;
    Eigen::MatrixXd m_normals;
    void read(const std::string& file_name);
};

#endif  // MODEL_H