#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace TGA {

    struct TGA_Color {
        union {
            struct {uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;};
            uint8_t raw[4];
            uint32_t val;
        };
        int bytespp;
        TGA_Color() : val(0), bytespp(1) {}

        TGA_Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4){}

        TGA_Color(int v, int bpp) : val(v), bytespp(bpp) {}

        TGA_Color(const TGA_Color &c) : val(c.val), bytespp(c.bytespp) {}

        TGA_Color(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
            for(int i=0; i<bpp; i++) {
                raw[i] = p[i];
            }
        }

        TGA_Color & operator = (const TGA_Color &c) {
            if (this != &c) {
                bytespp = c.bytespp;
                val = c.val;
            }
            return *this;
        }

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

    

    

    class TGA_Image {
        protected: 
        
        unsigned char* data;
        int width;
        int height;
        int bytespp;

        public:
        enum Format {
            GRAYSCALE=1, RGB=3, RGBA=4
        };
        TGA_Image();
        TGA_Image(int w, int h, int bpp);
        TGA_Image(const TGA_Image &img);
        

        TGA_Color get(int x, int y);
        bool set(int x, int y, TGA_Color c);
        ~TGA_Image();
        TGA_Image & operator = (const TGA_Image &img);
        int get_width();
        int get_height();
        int get_bytespp();
        unsigned char *buffer();
        void clear();
        void load_header(const std::string& filename, Header &header);

        void show_header(Header &header);
        
        std::vector<TGA_Color> read_pixel_data(const std::string& filename,Header &header);

        void show_pixel_data(const Header& header, const std::vector<TGA_Color> &image);
    };
}