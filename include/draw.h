#include "tgaimage.h"
#ifdef _WIN32
    #ifdef DRAW_EXPORTS
        #define DRAW_API __declspec(dllexport)
    #else
        #define DRAW_API __declspec(dllimport)
    #endif
#else
    #define DRAW_API
#endif

class DRAW_API Paint
{
public:
    Paint() = default;

    // Function to draw a line from (x0, y0) to (x1, y1)
    void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

    void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& image, TGAColor color);

private:

    bool checkPixelInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
};