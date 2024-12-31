#pragma once
//#include "Vertex.h"
#include "Vector3.h"
#include "ColorGC.h"
#include "Matrix4.h"

class Line {
public:
    Vector3 m_a, m_b;
    ColorGC m_color;
    // Constructor to create a line from vertex a to vertex b
    Line()=default;
    Line(const Vector3& a, const Vector3& b, ColorGC color=ColorGC(250,0,0));
    // Calculate the direction vector of the line
    Vector3 direction()const;
    Line getTransformedLine(const Matrix4& transformation)const;
    // Calculate the length of the line
    float length()const;
    int yMin()const;
    int yMax()const;
    // Check if two lines intersect, and return the intersection point if they do
    bool clip();
    void draw(uint32_t* m_Buffer, int width, int hight);
    // Print the line
    void print();
    //statics
    static std::pair<bool, Vector3> linePlaneIntercetion(const Line& line, const Vector3& planeNormal, const Vector3& planePoint);
    static bool isPointOnLineBetween(const Line& line, const Vector3& point);

};
