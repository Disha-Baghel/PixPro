#include <iostream>
#include "PixPro.h"

int main () {

    TGA::Header header;
    
    TGA::load_header("./Images/landscape.tga", header);

    TGA::show_header(header);

    TGA::read_pixel_data("./Images/landscape.tga", header);

    return 0;
} 