#include "timage.h"

TImage::TImage() {}

TImage::TImage(const std::uint16_t &width, const std::uint16_t &height) {
    m_width = width;
    m_height = height;

    m_buffer = (std::uint8_t *)malloc(sizeof(std::uint8_t) * width * height * 3);
    memset(m_buffer, 0x00, width * height * 3);
}

TImage::TImage(const std::string &filename) {
    read(filename);
}

TImage::~TImage() {
    delete[] m_buffer;
}

bool TImage::write(const std::string &file_name) const {
    std::ofstream out;
    out.open(file_name, std::ios::binary);

    if (!out.is_open()) {
        std::cout << "Can't write file: " << file_name << std::endl;
        out.close();
        return false;
    }

    TImageHeader *header = new TImageHeader;
    header->id_length = 0x00;
    header->colour_map_type = 0x00;
    header->data_type = 0x02;
    header->colour_map_origin = 0x0000;
    header->colour_map_length = 0x0000;
    header->colour_map_depth = 0x00;
    header->x_origin = 0x0000;
    header->y_origin = 0x0000;
    header->width = m_width;
    header->height = m_height;
    header->bits_per_pixel = 0x18;
    header->image_descriptor = 0x00;

    out.write(reinterpret_cast<const char *>(header), sizeof(TImageHeader));
    if (!out) {
        std::cout << "Can't write file: " << file_name << std::endl;
        out.close();
        return false;
    }

    out.write(reinterpret_cast<const char *>(m_buffer), sizeof(std::uint8_t) * m_width * m_height * 3);
    if (!out) {
        std::cout << "Can't write file: " << file_name << std::endl;
        out.close();
        return false;
    }

    delete header;
    out.close();
    return true;
}

bool TImage::read(const std::string &file_name) {
    std::ifstream in;
    in.open(file_name, std::ios::binary);

    if (!in.is_open()) {
        std::cout << "Can't read file: " << file_name << std::endl;
        in.close();
        return false;
    }

    std::uint8_t id_len;
    std::uint8_t cmap_type;
    std::uint8_t data_type;

    in.read((char *)&id_len, sizeof(std::uint8_t));
    in.read((char *)&cmap_type, sizeof(std::uint8_t));
    in.read((char *)&data_type, sizeof(std::uint8_t));

    in.seekg(12, std::ios::beg);
    in.read((char *)&m_width, sizeof(std::uint16_t));
    in.read((char *)&m_height, sizeof(std::uint16_t));

    // Allocate enough space for the uncompressed format
    m_buffer = (std::uint8_t *)malloc(sizeof(std::uint8_t) * m_width * m_height * 3);

    // Check the file version
    // Get the file signature
    in.seekg(-18, std::ios::end);
    char c_signature[17];
    c_signature[16] = 0x00;
    in.read(c_signature, sizeof(char) * 16);
    std::string signature(c_signature);

    if (signature.compare(0, 15, "TRUEVISION-XFILE")) {
        if (data_type == 0x0A) {
            in.seekg(-26, std::ios::end);
            std::uint32_t ext_addr, dev_addr;
            in.read((char *)&ext_addr, sizeof(std::uint32_t));
            in.read((char *)&dev_addr, sizeof(std::uint32_t));

            if (cmap_type != 0x00 || id_len != 0x00)
                std::cout << "Colour map not supported: " << file_name << std::endl;
            else {
                std::uint32_t image_data_len;
                if (ext_addr != 0 && dev_addr != 0) {
                    image_data_len = std::min(ext_addr, dev_addr);
                } else {
                    in.seekg(0, std::ios::beg);
                    int file_start = (int)in.tellg();
                    in.seekg(0, std::ios::end);
                    image_data_len = (int)in.tellg() - file_start;
                }

                image_data_len -= 18;  // 18 Bytes are used for the header.

                in.seekg(18, std::ios::beg);

                std::uint32_t bytes_eaten = 0;
                std::uint32_t buffer_i = 0;
                while (bytes_eaten < image_data_len) {
                    std::uint8_t rep_field;
                    in.read((char *)&rep_field, sizeof(std::uint8_t));

                    std::uint8_t rep = rep_field & 0x7F;
                    if (rep_field & 0x80) {  // The packet is a run length packet
                        std::uint8_t pixel_v[3];
                        in.read((char *)&pixel_v, sizeof(std::uint8_t) * 3);
                        bytes_eaten += 4;

                        for (int rep_i = 0; rep_i < rep + 1; rep_i++) {
                            // We flip the intel order of bytes to rgb
                            m_buffer[buffer_i] = pixel_v[0];
                            m_buffer[buffer_i + 1] = pixel_v[1];
                            m_buffer[buffer_i + 2] = pixel_v[2];
                            buffer_i += 3;
                        }
                    } else {  // Raw data packet
                        bytes_eaten += 1 + (rep * 3);
                        for (int rep_i = 0; rep_i < rep + 1; rep_i++) {
                            in.read((char *)&m_buffer[buffer_i], sizeof(std::uint8_t) * 3);
                            buffer_i += 3;
                        }
                    }
                }
            }
        } else {
            // Uncompressed format not supported for new TGA format.
            std::cout << "RLE compression for old TGA format not supported: " << file_name << std::endl;
        }
    } else {  // The old format
        if (data_type == 0x0A) {
            std::cout << "RLE compression for old TGA format not supported: " << file_name << std::endl;
        } else {
            in.seekg(18, std::ios::beg);
            in.read((char *)m_buffer, sizeof(std::uint8_t) * m_width * m_height * 3);
        }
    }

    return true;
}

void TImage::setPixel(const int &x, const int &y, const TColour &colour) {
    const int pixel_start = ((m_width * y) + x) * 3;

    m_buffer[pixel_start + 0] = colour.m_b;
    m_buffer[pixel_start + 1] = colour.m_g;
    m_buffer[pixel_start + 2] = colour.m_r;
}

void TImage::setColour(const TColour &colour) {
    for (int i = 0; i < m_height * m_width; i++) {
        m_buffer[i * 3] = colour.m_b;
        m_buffer[i * 3 + 1] = colour.m_g;
        m_buffer[i * 3 + 2] = colour.m_r;
    }
}

const std::pair<uint16_t, uint16_t> TImage::get_dimensions() const {
    return std::pair<u_int16_t, u_int16_t>(m_width, m_height);
}

bool TImage::is_pixel_in_bounds(const int &x, const int &y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

// TColour

TColour::TColour(const std::uint8_t &r, const std::uint8_t &g, const std::uint8_t &b) {
    m_r = r;
    m_g = g;
    m_b = b;
}
