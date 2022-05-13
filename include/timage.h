#ifndef TIMAGE_H
#define TIMAGE_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#pragma pack(push, 1)
typedef struct TImageHeader {
    std::uint8_t id_length;
    std::uint8_t colour_map_type;
    std::uint8_t data_type;
    std::uint16_t colour_map_origin;
    std::uint16_t colour_map_length;
    std::uint8_t colour_map_depth;
    std::uint16_t x_origin, y_origin;
    std::uint16_t width, height;
    std::uint8_t bits_per_pixel;
    std::uint8_t image_descriptor;
} TImageHeader;
#pragma pack(pop)

class TColour {
   public:
    TColour(const std::uint8_t &r, const std::uint8_t &g, const std::uint8_t &b);
    std::uint8_t m_r, m_g, m_b;
    TColour operator*(float c) const {
        TColour r(m_r * c, m_g * c, m_b * c);
        return r;
    }
};

class TImage {
   public:
    TImage();
    TImage(const TImage &other);
    TImage(const std::uint16_t &width, const std::uint16_t &height);
    TImage(const std::string &filename);
    ~TImage();
    bool write(const std::string &fileName) const;
    bool read(const std::string &filename);
    void setPixel(const int &x, const int &y, const TColour &colour);
    void setColour(const TColour &colour);
    TColour getColour(const std::uint16_t x, const std::uint16_t y) const;
    const std::pair<uint16_t, uint16_t> get_dimensions() const;
    bool is_pixel_in_bounds(const int &x, const int &y) const;

   private:
    std::uint8_t *m_buffer;
    std::uint16_t m_width,
        m_height;
};

inline TColour TImage::getColour(const std::uint16_t x, const std::uint16_t y) const {
    const int offset = (m_width * y) + x;
    return TColour(m_buffer[(3 * offset) + 2], m_buffer[(3 * offset) + 1], m_buffer[(3 * offset)]);
}

#endif  // TIMAGE_H