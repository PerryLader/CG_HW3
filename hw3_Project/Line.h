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

    // Check if two lines intersect, and return the intersection point if they do
    bool clip();
    void draw(uint32_t* m_Buffer, int width, int hight);
    // Print the line
    void print();
private:
    bool isInClip();
};
