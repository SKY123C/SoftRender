#include "tgaimage.h"
#include "draw.h"
constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};
int main(int argc, char** argv) {
        constexpr int width  = 1920;
    constexpr int height = 1080;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;
    
    double vertex[24] ={
        -0.500000, -0.500000, 0.500000,
         0.500000, -0.500000, 0.500000,
        -0.500000,  0.500000, 0.500000,
         0.500000,  0.500000, 0.500000,
        -0.500000,  0.500000, -0.500000,
         0.500000,  0.500000, -0.500000,
        -0.500000, -0.500000, -0.500000,
         0.500000, -0.500000, -0.500000
    };
    int indices[36] = {
        1,2,3,
        3,2,4,
        3,4,5,
        5,4,6,
        5,6,7,
        7,6,8,
        7,8,1,
        1,8,2,
        2,8,4,
        4,8,6,
        7,1,5,
        5,1,3
    };
    std::vector<TriangleData> triangles;
    for (int i = 0; i < 36; i += 3) {
        triangles.emplace_back(TriangleData(&vertex[indices[i] * 3]));
    }
    Paint paint;
    Camera camera;
    paint.drawPixelByCamera(camera, triangles, width, height, framebuffer, red);
    //paint.Triangle(ax, ay, bx, by, cx, cy, framebuffer, red);
    // paint.drawLine(ax, ay, bx, by, framebuffer, blue);
    // paint.drawLine(cx, cy, bx, by, framebuffer, green);
    // paint.drawLine(cx, cy, ax, ay, framebuffer, yellow);
    // paint.drawLine(ax, ay, cx, cy, framebuffer, red);

    // framebuffer.set(ax, ay, white);
    // framebuffer.set(bx, by, white);
    // framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}