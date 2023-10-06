#include <iostream>
#include "PixPro.h"

int main (int argc, char* argv[]) {

    TGA::Header header;
    
    TGA::load_header("./Images/dragon.tga", header);

    TGA::show_header(header);

    auto image = TGA::read_pixel_data("./Images/dragon.tga", header);

    TGA::show_pixel_data(header, image);

    return 0;
} 