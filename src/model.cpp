#include "model.h"

Model::Model(const std::string &file_name) {
    Model::read(file_name);
}

void Model::read(const std::string &file_name) {
    std::ifstream in_file;
    in_file.open(file_name);

    if (in_file.fail()) {
        in_file.close();
        std::cout << "Failure loading model: " << file_name << std::endl;
        return;
    }

    int num_faces = 0;
    int num_vertices = 0;
    int num_textures_uvs = 0;
    int num_normals = 0;

    std::string line;
    while (!in_file.eof()) {
        std::getline(in_file, line);
        std::istringstream iss(line.c_str());

        if (!line.compare(0, 2, "f "))
            num_faces++;
        else if (!line.compare(0, 2, "v "))
            num_vertices++;
        else if (!line.compare(0, 2, "vt"))
            num_textures_uvs++;
        else if (!line.compare(0, 2, "vn"))
            num_normals++;
    }

    m_indices = Eigen::MatrixXi(num_faces, 9);  // Same indexing as in .OBJ files
    m_vertices = Eigen::MatrixXd(num_vertices, 3);
    m_texture_uvs = Eigen::MatrixXd(num_textures_uvs, 3);
    m_normals = Eigen::MatrixXd(num_normals, 3);

    in_file.clear();
    in_file.seekg(0, std::ios::beg);
    num_faces = 0;
    num_vertices = 0;
    num_textures_uvs = 0;
    num_normals = 0;

    while (!in_file.eof()) {
        std::getline(in_file, line);
        std::istringstream iss(line.c_str());

        char trash;
        if (!line.compare(0, 2, "f ")) {
            iss >> trash;

            int c, t, n;
            int i = 0;
            while (iss >> c >> trash >> t >> trash >> n) {
                m_indices(num_faces, (3 * i)) = c;
                m_indices(num_faces, (3 * i) + 1) = t;
                m_indices(num_faces, (3 * i) + 2) = n;
                i++;
            }
            num_faces++;
        } else if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            iss >> m_vertices(num_vertices, 0);
            iss >> m_vertices(num_vertices, 1);
            iss >> m_vertices(num_vertices, 2);
            num_vertices++;
        } else if (!line.compare(0, 2, "vt")) {
            iss >> trash >> trash;
            iss >> m_texture_uvs(num_textures_uvs, 0);
            iss >> m_texture_uvs(num_textures_uvs, 1);
            iss >> m_texture_uvs(num_textures_uvs, 2);
            num_textures_uvs++;
        } else if (!line.compare(0, 2, "vn")) {
            iss >> trash >> trash;
            iss >> m_normals(num_normals, 0);
            iss >> m_normals(num_normals, 1);
            iss >> m_normals(num_normals, 2);
            num_normals++;
        }
    }

    std::cout << "Loaded model: " << file_name << std::endl;
    std::cout << "F: " << num_faces << " V: " << num_vertices << " VT: " << num_textures_uvs << " VN: " << num_normals << std::endl;
}

const int Model::size() const {
    return m_indices.rows();
}

const Eigen::Matrix<int, 9, 1> Model::indexAt(const int i) const {
    return m_indices.row(i);
}

const Eigen::Matrix<double, 3, 1> Model::vertexAt(const int i) const {
    return m_vertices.row(i);
}

const Eigen::Matrix<double, 3, 1> Model::textureUVAt(const int i) const {
    return m_texture_uvs.row(i);
}

const Eigen::Matrix<double, 3, 1> Model::normalAt(const int i) const {
    return m_normals.row(i);
}
