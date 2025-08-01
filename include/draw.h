#include "tgaimage.h"
#include <cmath>
#ifdef _WIN32
    #ifdef DRAW_EXPORTS
        #define DRAW_API __declspec(dllexport)
    #else
        #define DRAW_API __declspec(dllimport)
    #endif
#else
    #define DRAW_API
#endif

class DRAW_API Matrix
{
public:
    Matrix(double* InMatrix)
    {
        for (int i = 0; i < 16; ++i) {
            matrix[i] = InMatrix[i];
        }
    }
    Matrix(Matrix&& other)
    {
        for (int i = 0; i < 16; ++i) {
            matrix[i] = other.matrix[i];
        }
    }
    Matrix() = default;
    Matrix operator *(const Matrix& other)
    {
        Matrix outResult;
        double result[16] = {0};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    result[i * 4 + j] += matrix[i * 4 + k] * other.matrix[k * 4 + j];
                }
            }
        }
        return outResult;
    }
private:
    double matrix[16] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
};

class DRAW_API Camera
{
public:
    Camera(double * inPosition){
        for (int i = 0; i < 3; ++i) {
            position[i] = inPosition[i];
        }
    };
    Camera() = default;
    // rotation direction is xyz
    Matrix getViewMatrix()
    {

        double viewMatrix[16] = {
            1, 0, 0, -position[0],
            0, 1, 0, -position[1],
            0, 0, 1, -position[2],
            0, 0, 0, 1
        };
        Matrix matrix(viewMatrix);
        
        return matrix;
    }

    
public:
    float focalLength = 30;
    double position[3] = {0.458, 7.041, 7.714};
    double rotation[3] = {-25.538, 62.6, 0}; // pitch, yaw, roll
};

class DRAW_API Paint
{
public:
    Paint() = default;

    // Function to draw a line from (x0, y0) to (x1, y1)
    void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

    void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& image, TGAColor color);

    void getPixelByCamera(Camera& camera);

private:

    bool checkPixelInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
};