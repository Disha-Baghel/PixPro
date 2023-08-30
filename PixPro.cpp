#include "PixPro.h"
#include <fstream>
#include <vector>
#include <iostream>

#define tga_show_field(field)\
    std::cout << #field << ": " << field << std::endl

namespace TGA {


    void load_header(const std::string& filename, Header &header) {

        std::ifstream file(filename);

        if(!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return ;
        }

        file.read((char*)&header, sizeof(header));

    }

    void show_header(Header &header) {
        tga_show_field(header.img_ID);
        tga_show_field(header.color_map_type);
        tga_show_field(header.image_type);
        tga_show_field(header.first_entry_index);
        tga_show_field(header.color_map_length);
        tga_show_field(header.color_map_entry_size);
        tga_show_field(header.x_origin);
        tga_show_field(header.y_origin);
        tga_show_field(header.image_width);
        tga_show_field(header.image_height);
        tga_show_field(header.pixel_depth);
        tga_show_field(header.image_descriptor);
    }

    void read_pixel_data(const std::string& filename, Header &header) {

        std::ifstream file(filename);

        std::vector<std::vector<uint8_t>> matrix(header.image_height, std::vector<uint8_t>(header.image_width*3));

        //read pixel data
        for(uint16_t i = 0; i < header.image_height; i++){
            for(uint16_t j = 0; j < header.image_width*3; j++) {
                
                char pixelValue;
                                
                file.get(pixelValue);
    
                matrix[i][j] = pixelValue;
            }
        }

        //print pixel_data
        for(uint16_t i = 0; i < header.image_height; i++){
            for(uint16_t j = 0; j < header.image_width*3; j++) {

                std::cout << (int)matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};