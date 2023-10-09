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

        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        file.read((char *)&header, sizeof(header));   
           
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
                char chunkheader = 0;
                file.get(chunkheader);
                if(!file.good()) {
                    std::cerr <<"an error occurred while reading the data\n";
                }

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