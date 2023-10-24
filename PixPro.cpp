#include "PixPro.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <bitset>

#define tga_show_field(field) \
    std::cout << #field << ": " << (int)field << std::endl

#define tga_load_field(file, field)\
    file.read((char*)&field, sizeof(field))

namespace TGA
{

    TGA_Image::TGA_Image() : data(NULL), width(0), height(0), bytespp(0) {}

    TGA_Image::TGA_Image(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp) {
        unsigned long nbytes = width*height*bytespp;
        data = new unsigned char[nbytes];
        memset(data, 0, nbytes);
    }

    TGA_Image::TGA_Image(const TGA_Image &img) {
        width = img.width;
        height  = img.height;
        bytespp = img.bytespp;
        unsigned long nbytes = width*height*bytespp;
        data = new unsigned char[nbytes];
        memcpy(data, img.data, nbytes);
    }

    TGA_Image::~TGA_Image() {
        if(data) delete [] data;
    }

    TGA_Image & TGA_Image::operator=(const TGA_Image &img) {
        if(this != &img) {
            if (data) delete [] data;
            width = img.width;
            height = img.height;
            bytespp = img.bytespp;
            unsigned long nbytes = width*height*bytespp;
            data = new unsigned char[nbytes];
            memcpy(data, img.data, nbytes);
        }
        return *this;
    }


    void TGA_Image::load_header(const std::string &filename, Header &header)
    {
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open())
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        // const size_t header_size = sizeof(header);
        // file.read(reinterpret_cast<char *>(&header), header_size);
        file.read((char *)&header, sizeof(header));   

        if(!file) {
            std::cerr <<"Error reading TGA header "<< std::endl;
        }      
    }

    void TGA_Image::show_header(Header &header)
    {
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

    std::vector<TGA_Color> TGA_Image::read_pixel_data(const std::string &filename, Header &header)
    {

        std::ifstream file(filename);

        std::vector<TGA_Color> image(header.image_height * header.image_width);

        // read pixel data
        width = header.image_width;
        height = header.image_height;
        bytespp = header.pixel_depth>>3;  
        unsigned long nbytes = width*height*bytespp;
        data = new unsigned char[nbytes];
        if (header.image_type == 2)
        {
            // for (uint16_t i = 0; i < nbytes; i++)
            // {   
            //     TGA_Color color;
            //     file.read((char *)&color, sizeof(TGA_Color));

            //     // for (uint16_t j = 0; j < header.image_width * 3; j += 3)
            //     // {
            //         image.push_back(color);
            //     // }
            // }
            file.read((char*) data, nbytes);
            if(!file.good()) {
                std::cerr << "an error occurred while reading the data\n";
            }
            
            // file.read((char*) image, nbytes);
        }
        else if (header.image_type == 10)
        {
            // int ok = 0;
            // while (!file.eof())
            // {
            //     char chunkheader;
            //     int times;
            //     file.get(chunkheader);
            //     std::cout<<" " << std::bitset<8>(chunkheader);
            //     ok++;

            //     if (chunkheader & (1 << 7))
            //     { // same as if(chunkheader > 128)
            //         times = chunkheader - 127;
            //         TGA_Color color;
            //         file.read((char *)&color, sizeof(TGA_Color));
            //         for (int i = 0; i <= times; i++)
            //         {
            //             image.push_back(color);
                        
            //         }
            //     }
            //     else
            //     {
            //         chunkheader++;


            //         for(int i=0; i<chunkheader; i++) {
            //             TGA_Color color;
            //             file.read((char*)&color, sizeof(TGA_Color));
            //             image.push_back(color);
                        
            //         }
            //     }
            // }
            // std::cout << '\n' << image.size();
            // std::cout << '\n' << ok;


            uint16_t pixelcount = height*width;
            uint16_t currentpixel = 0;
            uint16_t currentbyte = 0;
            TGA_Color colorbuffer;

            do {
                unsigned char chunkheader = 0;
                file.get(reinterpret_cast<char &>(chunkheader));
                if(!file.good()) {
                    std::cerr <<"an error occurred while reading the data\n";
                }
                // std::cout<<" " << std::bitset<8>(chunkheader);
                // std::cout<<" " << (int)chunkheader;

                if(chunkheader<128) {
                    chunkheader++;
                    for(int i=0; i<chunkheader; i++) {
                        file.read((char *) colorbuffer.raw, bytespp);
                        if(!file.good()) {
                            std::cerr <<"an error occurred while reading the header\n";
                        }
                        for(int t=0; t<bytespp; t++) {
                            data[currentbyte++] = colorbuffer.raw[t];
                        }
                        currentpixel++;
                        if(currentpixel>pixelcount) {
                            std::cerr << "1. Too many pixels read\n";
                        }
                    }
                }
                else{
                    chunkheader -=127;
                    file.read((char *) colorbuffer.raw, bytespp);
                    if(!file.good()) {
                        std::cerr <<"an error occurred while reading the header\n";
                    }
                    for(int i=0; i<chunkheader; i++) {
                        for(int t=0; t<bytespp; t++) {
                            data[currentbyte++] = colorbuffer.raw[t];
                        }
                        currentpixel++;
                        if(currentpixel>pixelcount) {
                            std::cerr << "2. Too many pixels read\n";
                        }
                    }
                }

            }while(currentpixel<pixelcount);
        }
        else {
            file.close();
            std::cerr << "unknown file format" << (int) header.image_type << std::endl;
        }

        return image;
    }


    bool TGA_Image::write_tga_file(const char *filename, bool rle) {
        unsigned char developer_area_ref[4] = {0, 0, 0, 0};
        unsigned char extension_area_ref[4] = {0, 0, 0, 0};
        unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
        std::ofstream out;
        out.open(filename, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "can't open file" << filename << "\n";
            out.close();
            return false;
        }
        Header header;
        memset((void*)&header, 0, sizeof(header));
        if (!out.good()) {
            out.close();
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        if (!rle) {
            out.write((char*) data, width*height*bytespp);
            if(!out.good()) {
                std::cerr << "can't unload raw data\n";
                out.close();
                return false;
            }
        }
        else {
            if(!unload_rle_data(out)) {
                std::cerr << "can't unlosd rle data\n";
                out.close();
                return false;
            }
        }
        out.write((char* )developer_area_ref, sizeof(developer_area_ref));
        if (!out.good()) {
            std::cerr <<"can't dump the tga file\n";
            out.close();
            return false;
        }
        out.write((char* )extension_area_ref, sizeof(extension_area_ref));
        if (!out.good()) {
            std::cerr << "can't dump the tga file\n";
            out.close();
            return false;
        }
        out.write((char*) footer, sizeof(footer));
        if(!out.good() ) {
            std::cerr << "can't dump the tga file\n";
            out.close();
            return false;
        }
        out.close();
        return true;
    }


    bool TGA_Image::unload_rle_data(std::ofstream &out) {
        const unsigned char max_chunk_length = 128;
        unsigned long npixels = width*height;
        unsigned long curpix = 0;
        while (curpix<npixels) {
            unsigned long chunkstart = curpix*bytespp;
            unsigned long curbyte = curpix*bytespp;
            unsigned char run_length = 1;
            bool raw = true;
            while (curpix+run_length<npixels && run_length<max_chunk_length) {
                bool succ_eq = true;
                for (int t=0; succ_eq && t<bytespp; t++) {
                    succ_eq = (data[curbyte+t]==data[curbyte+t+bytespp]);
                }
                curbyte += bytespp;
                if (1==run_length) {
                    raw = !succ_eq;
                }
                if(raw && succ_eq) {
                    run_length--;
                    break;
                }
                if(!raw && !succ_eq) {
                    break;
                }
                run_length++;
            }
            curpix += run_length;
            out.put(raw?run_length-1:run_length+127);
            if(!out.good()) {
                std::cerr <<"can't dump the tga file\n";
                return false;
            }
            out.write((char* ) (data+chunkstart), (raw?run_length*bytespp:bytespp));
            if(!out.good()) {
                std::cerr << "can't dump the tga file\n";
                return false;
            }
        }
        return true;
    }

    TGA_Color TGA_Image::get(int x, int y) {
        if(!data || x<0 || y<0 || x>=width || y>=height) {
            return TGA_Color();
        }
    }

    bool TGA_Image::set(int x, int y, TGA_Color c) {
        if(!data || x<0 || y<0 || x>=width || y>=height) {
            return false;
        }
        memcpy(data+(x+y*width)*bytespp, c.raw, bytespp);
        return true;
    }

    int TGA_Image::get_bytespp() {
        return bytespp;
    }

    int TGA_Image::get_width() {
        return width;
    }

    int TGA_Image::get_height() {
        return height;
    }

    bool TGA_Image::flip_horizontally() {
        if(!data) return false;
        int half = width>>1;
        for (int i=0; i<half; i++) {
            for (int j=0; j<height; j++) {
                TGA_Color c1 = get(i, j);
                TGA_Color c2 = get(width-1-i, j);
                set(i, j, c2);
                set(width-1-i, j, c1);
            }
        }
        return true;
    }

    bool TGA_Image::flip_vertically() {
        if (!data) return false;
        unsigned long bytes_per_line = width*bytespp;
        unsigned char *line = new unsigned char[bytes_per_line];
        int half = height>>1;
        for(int j=0; j<half; j++) {
            unsigned long l1 = j*bytes_per_line;
            unsigned long l2 = (height-1-j)*bytes_per_line;
            memmove((void *)line, (void *)(data+l1), bytes_per_line);
            memmove((void *)(data+l1), (void* )(data+l2), bytes_per_line);
            memmove((void *)(data+l2), (void *)line, bytes_per_line);
        }
        delete[] line;
        return true;
    }

    unsigned char *TGA_Image::buffer() {
        return data;
    }

    void TGA_Image::clear() {
        memset((void *)data, 0, width*height*bytespp);
    }

    bool TGA_Image::scale(int w, int h) {
        if (w<=0 || h<=0 || !data) return false;
        unsigned char *tdata = new unsigned char[w*h*bytespp];
        int nscanline = 0;
        int oscanline = 0; 
        int erry = 0;
        unsigned long nlinebytes = w*bytespp;
        unsigned long olinebytes = width*bytespp;
        for (int j=0; j<height; j++) {
            int errx = width-w;
            int nx = -bytespp;
            int ox = -bytespp;
            for (int i=0; i<width; i++) {
                ox += bytespp;
                errx += w;
                while (errx>=(int)width) {
                    errx -= width;
                    nx += bytespp;
                    memcpy(tdata+nscanline+nx,  data+oscanline+ox, bytespp);
                }
            }
            erry += h;
            oscanline += olinebytes;
            while(erry>=(int)height) {
                if(erry>=(int)height<<1) //jump over a scanline
                    memcpy(tdata+nscanline+nlinebytes, tdata+nscanline, nlinebytes);
                erry -= height;
                nscanline += nlinebytes;
            }
        }
        delete [] data;
        data = tdata;
        width = w;
        height = h;
        return true;
    }


    void TGA_Image::show_pixel_data(const Header& header, const std::vector<TGA_Color>& image)
    {

        int width = header.image_width;
        int height = header.image_height;

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed");
        }

        SDL_Window *window = SDL_CreateWindow("Image Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        if (!window)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed");
        }

        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed");
        }

        bool quit = false;
    
        SDL_Event e;
        while (!quit)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            for(int i=0; i<image.size(); i++) {
                auto color = image[i];
                SDL_SetRenderDrawColor(renderer, (uint8_t)color.r, (uint8_t)color.g, (uint8_t)color.b, 255);
                int x = i % width;
                int y = i / width;

                SDL_RenderDrawPoint(renderer, x, y);
            } 
            SDL_RenderPresent(renderer);
        }
    }
}