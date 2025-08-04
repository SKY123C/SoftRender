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
#define M_PI 3.14159265358979323846
#include "Eigen/Dense"

class DRAW_API TriangleData
{
public:
    TriangleData(double* worldPointArray)
    {
        for (int i = 0; i < 3; ++i) {
            point[i] = worldPointArray[i];
        }
    }

public:
    double point[3] = {0};
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
    // rotation order is xyz
    Eigen::Matrix<float, 4, 4> getViewMatrix()
    {
        Eigen::Matrix<float, 4, 4> translationMatrix;
        translationMatrix <<

            1.0f, 0.0f, 0.0f, -position[0],
            0.0f, 1.0f, 0.0f, -position[1],
            0.0f, 0.0f, 1.0f, -position[2],
            0.0f, 0.0f, 0.0f, 1.0f
        ;
        Eigen::Matrix<float, 4, 4> rotationXMatrix;
        rotationXMatrix <<
            1.0, 0.0, 0.0, 0.0,
            0.0, cos(-rotation[0]), -sin(-rotation[0]), 0.0,
            0.0, sin(-rotation[0]), cos(-rotation[0]), 0.0,
            0.0, 0.0, 0.0, 1.0
        ;
        Eigen::Matrix<float, 4, 4> rotationYMatrix;
        rotationYMatrix <<
            cos(-rotation[1]), 0.0, sin(-rotation[1]), 0.0,
            0.0, 1.0, 0.0, 0.0,
            -sin(-rotation[1]), 0.0, cos(-rotation[1]), 0.0,
            0.0, 0.0, 0.0, 1.0
        ;
        Eigen::Matrix<float, 4, 4> rotationZMatrix;
        rotationZMatrix <<
            cos(-rotation[2]), -sin(-rotation[2]), 0.0, 0.0,
            sin(-rotation[2]), cos(-rotation[2]), 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        ;

        return translationMatrix * rotationZMatrix * rotationYMatrix * rotationXMatrix;
    }

    Eigen::Matrix<float, 4, 4> getProjectionMatrix(std::vector<double>& WorldPoint)
    {
        Eigen::Matrix<float, 4, 1> out;
        float tan = std::tan(fov / 2.0);
        float top = nearClipPlane * tan;
        float bottom = -top;
        float right = aspect * nearClipPlane * tan;
        float left = -right;
        double projection[16] = {0};
        Eigen::Matrix<float, 4, 4> perspToOrthoMatrix;
        perspToOrthoMatrix <<
            nearClipPlane, 0.0f, 0.0f, 0.0f,
            0.0f, nearClipPlane, 0.0f, 0.0f,
            0.0f, 0.0f, WorldPoint[2] + nearClipPlane, -WorldPoint[2] * nearClipPlane,
            0.0f, 0.0f, 1.0f, 0.0f;
        Eigen::Matrix<float, 4, 4> orthoMatrixT;
        orthoMatrixT <<
            1,0,0, - (left + right) / 2.0f,
            0,1,0, - (top + bottom) / 2.0f,
            0,0,1, - (farClipPlane + nearClipPlane) / 2.0f,
            0,0,0, 1.0f;
        Eigen::Matrix<float, 4, 4> orthoMatrixS;
        orthoMatrixS <<
            2.0f / (right - left), 0, 0, 0,
            0, 2.0f / (top - bottom), 0, 0,
            0, 0, -2.0f / (nearClipPlane - farClipPlane), 0,
            0, 0, 0, 1.0f;
        
        Eigen::Matrix<float, 4, 1> worldPointMatrix;
        worldPointMatrix << WorldPoint[0], WorldPoint[1], WorldPoint[2], 1.0f;
        //out = worldPointMatrix * getViewMatrix() * perspToOrthoMatrix * orthoMatrixT * orthoMatrixS;
        return perspToOrthoMatrix * orthoMatrixT * orthoMatrixS;
    }


public:
    double nearClipPlane = 0.100;
    double farClipPlane = 10000.0;
    float focalLength = 30;
    float fov = 60.0 * M_PI / 180.0; // field of view in degrees
    float aspect = 16.0f / 9.0f; // width / height
    double position[3] = {-4.537, 2.427, -3.419};
    double rotation[3] = {-23.138, 233, 0}; // pitch, yaw, roll
};

class DRAW_API Paint
{
public:
    Paint() = default;

    // Function to draw a line from (x0, y0) to (x1, y1)
    void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

    void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& image, TGAColor color);

    void drawPixelByCamera(Camera& camera, std::vector<TriangleData>& triangles, int width, int height, TGAImage& image, TGAColor color);


private:

    bool checkPixelInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
};