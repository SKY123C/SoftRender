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
    TriangleData() = default;

    void addPoint(double x, double y, double z)
    {
        pointArray.push_back(Eigen::Vector3d(x, y, z));
    }

public:
    std::vector<Eigen::Vector3d> pointArray;
    std::vector<Eigen::Vector3d> pointArrayInScreenSpace;
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
        Eigen::Matrix<float, 3, 3> rotationXMatrix;
        rotationXMatrix <<
            1.0, 0.0, 0.0,
            0.0, cos(rotation[0]), -sin(rotation[0]),
            0.0, sin(rotation[0]), cos(rotation[0])
        ;
        Eigen::Matrix<float, 3,3> rotationYMatrix;
        rotationYMatrix <<
            cos(rotation[1]), 0.0, sin(rotation[1]),
            0.0, 1.0, 0.0,
            -sin(rotation[1]), 0.0, cos(rotation[1])
        ;
        Eigen::Matrix<float, 3, 3> rotationZMatrix;
        rotationZMatrix <<
            cos(rotation[2]), -sin(rotation[2]), 0.0,
            sin(rotation[2]), cos(rotation[2]), 0.0,
            0.0, 0.0, 1.0
        ;
        Eigen::Matrix<float, 3, 1> initLookAtVector(0, 0, -1);
        Eigen::Matrix<float, 3, 1> lookAtVector = rotationZMatrix * rotationYMatrix * rotationXMatrix * initLookAtVector;
        lookAtVector.normalize();
        Eigen::Matrix<float, 3, 1> initUpVector(0, 1, 0);
        Eigen::Matrix<float, 3, 1> upVector =  rotationZMatrix * rotationYMatrix * rotationXMatrix * initUpVector;
        upVector.normalize();
        Eigen::Matrix<float, 3, 1> rightVector = lookAtVector.cross(upVector);
        rightVector.normalize();
        Eigen::Matrix<float, 4,4> rotationMatrix;
        rotationMatrix <<
            rightVector(0), rightVector(1), rightVector(2), 0.0f,
            upVector(0), upVector(1), upVector(2), 0.0f,
            -lookAtVector(0), -lookAtVector(1), -lookAtVector(2), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        ;

        return rotationMatrix * translationMatrix;
    }

    Eigen::Matrix<float, 4, 4> getProjectionMatrix(Eigen::Matrix<float,4,1>& inPointMatrix)
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
            0.0f, 0.0f, farClipPlane + nearClipPlane, -farClipPlane * nearClipPlane,
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
            0, 0, 2.0f / (nearClipPlane - farClipPlane), 0,
            0, 0, 0, 1.0f
        ;
        //out = worldPointMatrix * getViewMatrix() * perspToOrthoMatrix * orthoMatrixT * orthoMatrixS;
        return orthoMatrixS * orthoMatrixT * perspToOrthoMatrix;
    }


public:
    double nearClipPlane = 0.100;
    double farClipPlane = 10000.0;
    float focalLength = 30;
    float fov = 60.0 * M_PI / 180.0; // field of view in degrees
    float aspect = 16.0f / 9.0f; // width / height
    //double position[3] = {1.698,14.182,27.176};
    //double rotation[3] = {-22.2 * M_PI / 180.0, 2.8 * M_PI / 180.0, 0}; // pitch, yaw, roll
    double rotation[3] = {340 * M_PI / 180.0, 25.2 * M_PI / 180.0, 0 * M_PI / 180.0}; // pitch, yaw, roll
    double position[3] = {2.163,2.227,2.178};
};

class DRAW_API Paint
{
public:
    Paint(int w, int h): width(w), height(h){};

    // Function to draw a line from (x0, y0) to (x1, y1)
    void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

    void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage& image, TGAColor color);

    void drawPixelByCamera(Camera& camera, std::vector<TriangleData>& triangles, int width, int height, TGAImage& image, TGAColor color);

    void setSize(int w, int h) {
        width = w;
        height = h;
    }
    void test();
private:
    int width;
    int height;
    bool checkPixelInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
};