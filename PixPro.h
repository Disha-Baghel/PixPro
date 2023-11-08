#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include <fstream>

#pragma pack(push, 1)

struct TGA_Header {
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitsperpixel;
    char imagedescriptor;
};
#pragma pack(pop)

struct TGA_Color{
    union 
    {
        struct {
            unsigned char b, g, r, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bytespp;
    
    TGA_Color() :val(0), bytespp(1){}

    TGA_Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A) {}

    TGA_Color(int v, int bpp) : val(v), bytespp(bpp) {}

    TGA_Color(const TGA_Color &c) : val(c.val), bytespp(c.bytespp) {}

    TGA_Color(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
        for (int i=0; i<bpp; i++) {
            raw[i] = p[i];
        }
    }

    TGA_Color &operator = (const TGA_Color &c) {
        if (this != &c) {
            bytespp = c.bytespp;
            val = c.val;
        }
        return *this;
    }
};

class TGA_Image {
    protected:
    unsigned char* data;
    int width;
    int height;
    int bytespp;

    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &our);

    public:
    enum Format {
        GRAYSCALE=1, RGB=3, RGBA=4
    };

    TGA_Image();
    TGA_Image(int w, int h, int bpp);
    TGA_Image(const TGA_Image &img);
    bool read_tga_file(const char *filename);
    bool write_tga_file(const char *filename, bool rle = true);
    bool flip_horizontally();
    bool flip_vertically();
    bool scale(int w, int h);
    TGA_Color get(int w, int h);
    bool set(int x, int y, TGA_Color c);
    ~TGA_Image();
    TGA_Image & operator = (const TGA_Image &img);
    int get_width();
    int get_height();
    int get_bytespp();
    unsigned char *buffer();
    void clear();

};