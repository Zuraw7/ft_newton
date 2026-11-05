#ifndef GRAPHICS_RESOLUTION_HPP
#define GRAPHICS_RESOLUTION_HPP

struct Resolution {
    int width;
    int height;
    constexpr static float aspect = 16.0f / 9.0f;

    Resolution(const int w, const int h) : width(w), height(h) {}
};

extern Resolution qHD;
extern Resolution HD;
extern Resolution HDP;
extern Resolution FHD;
extern Resolution QHD;

#endif //GRAPHICS_RESOLUTION_HPP