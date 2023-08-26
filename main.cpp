#include <iostream>
#include "PixPro.h"

int main () {

    TGA::Header header;
    
    TGA::load_header("./Images/landscape.tga", header);

    TGA::show_header(header);

    return 0;
} 