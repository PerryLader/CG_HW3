
#include "Vertex.h"
#include "Polygon.h"

// Constructor
Vertex::Vertex(Vector3 p) : m_point(p), m_dataNormalLine(), m_hasDataNormalLine(false), m_hasCalcNormalLine(false) {}
Vertex::Vertex(Vector3 p, Vector3 n) : m_point(p), m_hasDataNormalLine(true), m_hasCalcNormalLine(false)
{
    m_dataNormalLine = Line(p, (p + (n.normalized() * 0.25)));
}


//getters and setters
void Vertex::setCalcNormalLine()
{
    Vector3 avrageNormal(0, 0, 0);
    for (PolygonGC* t : m_neigberPolygons)
    {
        avrageNormal = avrageNormal + t->getCalcNormalNormolized();
    }
    this->m_calcNormalLine = Line(m_point,
        m_point + ((avrageNormal * (1.0 / m_neigberPolygons.size())).normalized() * 0.25));
    m_hasCalcNormalLine = true;
    //color here???
}
void Vertex::setDataNormalLine(Line normal) {
    m_dataNormalLine = normal;
    m_hasDataNormalLine = true;
}
Line Vertex::getCalcNormalLine()const
{
    if (!m_hasCalcNormalLine)
    {
        throw;
    }
    return m_calcNormalLine;
}
Line Vertex::getDataNormalLine()const 
{
    if (!m_hasDataNormalLine)
        throw std::exception();
    return m_dataNormalLine;
}
std::shared_ptr<Vertex> Vertex::getTransformedVertex(const Matrix4& transformation) const
{
    std::shared_ptr<Vertex> temp(new Vertex((transformation * Vector4::extendOne(this->m_point)).toVector3()));
    if (m_hasCalcNormalLine)
    {
        temp->m_hasCalcNormalLine = true;
        temp->m_calcNormalLine = this->m_calcNormalLine.getTransformedLine(transformation);
    }
    if (m_hasDataNormalLine)
    {
        temp->m_hasDataNormalLine = true;
        temp->m_dataNormalLine = m_dataNormalLine.getTransformedLine(transformation);

    }
    return temp;
}
bool Vertex::hasDataNormalLine() const {
    return m_hasDataNormalLine;
}
Vector3 Vertex::loc() const    // Get location
{
    return m_point;
}



//utils
void Vertex::transformVertex(const Matrix4& transformation)
{
    m_point = ((transformation * Vector4::extendOne(this->m_point)).toVector3());
    if (m_hasCalcNormalLine)
    {
        m_calcNormalLine = m_calcNormalLine.getTransformedLine(transformation);
    }
    if (m_hasDataNormalLine)
    {
        m_dataNormalLine = m_dataNormalLine.getTransformedLine(transformation);

    }
}
bool Vertex::isInsideClipVolume() {
    return m_point.x >= -1 && m_point.x <= 1 &&
        m_point.y >= -1 && m_point.y <= 1 &&
        m_point.z >= -1 && m_point.z <= 1;
}
void Vertex::print() {
    std::cout << "Vertex Located at: " << m_point << std::endl;
}
void Vertex::addNeigberPolygon(PolygonGC* poly) { m_neigberPolygons.push_back(poly); }
