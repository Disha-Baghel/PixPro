#include <iostream>
#include "PixPro.h"

int main (int argc, char* argv[]) {

    TGA::Header header;
    TGA::TGA_Image TGA_image;
    
    TGA_image.load_header("./Images/dragon.tga", header);

    TGA_image.show_header(header);

    auto image = TGA::read_pixel_data("./Images/dragon.tga", header);

    TGA::show_pixel_data(header, image);

    return 0;
} 