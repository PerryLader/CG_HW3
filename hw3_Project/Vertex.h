#ifndef	VERTEX_H
#define	VERTEX_H
#include "Matrix4.h"
#include "Vector4.h"
#include <vector>
#include "Line.h"

class PolygonGC;


class Vertex {
private:
    Vector3 m_point;
    Line m_dataNormalLine;    
    bool m_hasDataNormalLine;  
    Line m_calcNormalLine;
    bool m_hasCalcNormalLine;
    std::vector<PolygonGC*> m_neigberPolygons;
    void setDataNormalLine(Line normal);
    
public:
    
    // Constructor
    Vertex(Vector3 p);
    Vertex(Vector3 p, Vector3 n);
    
    

    //getters and setters
    void setCalcNormalLine();
    Line getDataNormalLine() const;
    Line getCalcNormalLine()const;
    bool hasDataNormalLine() const;
    Vector3 loc() const;



    //utils
    std::shared_ptr<Vertex> getTransformedVertex(const Matrix4& transformation)const;
    void transformVertex(const Matrix4& transformation);
    bool isInsideClipVolume();
    void addNeigberPolygon(PolygonGC* poly);
    void print();
    
};


#endif // VERTEX_H
