#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace TGA {

    struct TGA_Color {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    #pragma pack(push, 1)
    typedef struct {

        uint8_t img_ID;
        uint8_t color_map_type;
        uint8_t image_type;

        // color map specification
        uint16_t first_entry_index;
        uint16_t color_map_length;
        uint8_t color_map_entry_size;

        // image specification
        uint16_t x_origin;
        uint16_t y_origin;
        uint16_t image_width;
        uint16_t image_height;
        uint8_t pixel_depth;
        uint8_t image_descriptor;

    } Header; // Header struct for TGA file format
    #pragma pack(pop)

    void load_header(const std::string& filename, Header &header);

    void show_header(Header &header);

    std::vector<TGA_Color> read_pixel_data(const std::string& filename,Header &header);

    void show_pixel_data(const Header& header, const std::vector<TGA_Color> &image);
}