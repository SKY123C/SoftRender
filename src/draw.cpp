#include "draw.h"
#include <cmath>
#include <iostream>

void Paint::drawLine(int ax, int ay, int bx, int by, TGAImage &image, TGAColor color)
{
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x=ax; x<=bx; x++) {
        if (steep) // if transposed, de−transpose
            image.set(y, x, color);
        else
            image.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
        }
    }
}

void Paint::Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TGAImage &image, TGAColor color)
{
    int bbMinX = std::min({x1, x2, x3});
    int bbMinY = std::min({y1, y2, y3});
    int bbMaxX = std::max({x1, x2, x3});
    int bbMaxY = std::max({y1, y2, y3});
    for (int x=bbMinX; x<=bbMaxX; x++) {
        for (int y=bbMinY; y<=bbMaxY; y++) {
            if (checkPixelInTriangle(x, y, x1, y1, x2, y2, x3, y3)) {
                image.set(x, y, color);
            }
        }
    }
}

void Paint::drawPixelByCamera(Camera& camera, std::vector<TriangleData>& triangles, int width, int height, TGAImage& image, TGAColor color)
{
    Eigen::Matrix<float, 4, 4> viewportMatrix;
    viewportMatrix <<
        width / 2.0f, 0, 0, width / 2.0f,
        0, height / 2.0f, 0, height / 2.0f,
        0, 0, 1.0f, 0,
        0, 0, 0, 0
    ;
    Eigen::Matrix<float, 4, 4> viewM = camera.getViewMatrix();
    for (auto& triangle : triangles) {
        std::cout<<triangle.pointArray.size() << std::endl;
        for (const auto& point : triangle.pointArray) {
            Eigen::Matrix<float,4,1> worldPoint(point.x(), point.y(), point.z(), 1.0f);
            Eigen::Matrix<float,4,1> pointInCameraSpace = viewM * worldPoint;
            Eigen::Matrix<float,4,1> cameraPoint = camera.getProjectionMatrix() * pointInCameraSpace;
            // Perspective divide
            cameraPoint = cameraPoint / cameraPoint.w();
            Eigen::Matrix<float, 4, 1> a = viewportMatrix * cameraPoint;
  
            triangle.pointArrayInScreenSpace.push_back(Eigen::Vector3d(a.x(), a.y(), a.z()));
            //image.set(a.x(), a.y(), color);
        }
        Triangle(triangle.pointArrayInScreenSpace[0].x(), triangle.pointArrayInScreenSpace[0].y(),
                 triangle.pointArrayInScreenSpace[1].x(), triangle.pointArrayInScreenSpace[1].y(),
                 triangle.pointArrayInScreenSpace[2].x(), triangle.pointArrayInScreenSpace[2].y(),
                 image, color);

    }
}

void Paint::test()
{

    Eigen::Matrix<float, 4, 4> viewM;
    viewM << 1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, -5,
             0, 0, 0, 1;
    Eigen::Matrix<float, 4, 1> worldPoint(0,0,0, 1);
    Eigen::Matrix<float, 4, 1> newPoint = viewM * worldPoint;
    std::cout << "Transformed Point: " << newPoint.transpose() << std::endl;
}

bool Paint::checkPixelInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
{
    // 高斯面积公式
    // 顺时针为正数，注意给出的三个坐标也是按照顺时针
    double area1 = 0.5*((y1+py)*(x1-px) + (y1+y2)*(x2-x1) + (y2+py)*(px-x2));
    double area2 = 0.5*((y2+py)*(x2-px) + (y2+y3)*(x3-x2) + (y3+py)*(px-x3));
    double area3 = 0.5*((y3+py)*(x3-px) + (y3+y1)*(x1-x3) + (y1+py)*(px-x1));
    if (area1 < 0 || area2 < 0 || area3 < 0) {
        return false;
    }
    return true;
}
