#ifndef	POLYGON_H
#define	POLYGON_H
#include <vector>
#include <set>
#include "Vertex.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "iritprsr.h"
#include "ColorGC.h"
#include <memory>
const uint32_t RENDER_SHAPE = 1;
const uint32_t RENDER_POLYGONS_CALC_NORMALS = 2;
const uint32_t RENDER_POLYGONS_NORMALS_FROM_DATA = 4;
const uint32_t RENDER_CALC_VETICES_NORMALS = 8;
const uint32_t RENDER_DATA_VETICES_NORMALS = 16;
const uint32_t RENDER_OBJ_BBOX = 32;
const uint32_t RENDER_POLYGONS_BBOX = 64;
const uint32_t RENDER_OVERRIDER_WIRE_COLOR = 128;
const uint32_t RENDER_OVERRIDER_NORMAL_COLOR = 256;


//might be useful
class BBox {
    Vector3 m_minBounds, m_maxBounds;
public:
    BBox() : m_minBounds(Vector3(FLT_MAX, FLT_MAX, FLT_MAX)), m_maxBounds(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)){};
    BBox(const Vector3& minBound, const Vector3& maxBound) : m_minBounds(minBound), m_maxBounds(maxBound) {};
    Vector3 getMax()const;
    Vector3 getMin() const;
    void updateBBox(const Vector3& vert);
    void updateBBox(const BBox& box);
    bool bboxCollide(const BBox& bbox) const;
    BBox transformBBox(const Matrix4& tmat) const;
    static bool bboxCollide(const BBox& bbox1, const BBox& bbox2);
    static BBox unitBBox() { return BBox(-Vector3::one(), Vector3::one()); }
    std::vector<Line> getLinesOfBbox(const ColorGC& bBoxColor) const;
    void toPrint() const;
};

//might be useful

class RenderMode {
private:
    uint32_t flags = 1;
public:
    bool getRenderShape() const { return RENDER_SHAPE & flags; }
    bool getRenderPolygonsCalcNormal() const { return RENDER_POLYGONS_CALC_NORMALS & flags; }
    bool getRenderPolygonsNormalFromData() const { return RENDER_POLYGONS_NORMALS_FROM_DATA & flags; }
    bool getRenderCalcVertivesNormal() const { return RENDER_CALC_VETICES_NORMALS & flags; }
    bool getRenderDataVertivesNormal() const { return RENDER_DATA_VETICES_NORMALS & flags; }
    bool getRenderObjBbox() const { return RENDER_OBJ_BBOX & flags; }
    bool getRenderPolygonsBbox()  const { return RENDER_POLYGONS_BBOX & flags; }
    bool getRenderOverrideWireColor() const { return RENDER_OVERRIDER_WIRE_COLOR & flags; }
    bool getRenderOverrideNormalColor() const { return RENDER_OVERRIDER_NORMAL_COLOR & flags; }

    void setRenderShape() { flags ^= RENDER_SHAPE; }
    void setRenderPolygonsCalcNormal() { flags ^= RENDER_POLYGONS_CALC_NORMALS; }
    void setRenderPolygonsNormalFromData() { flags ^= RENDER_POLYGONS_NORMALS_FROM_DATA; }
    void setRenderCalcVertivesNormal() { flags ^= RENDER_CALC_VETICES_NORMALS; }
    void setRenderDataVertivesNormal() { flags ^= RENDER_DATA_VETICES_NORMALS; }
    void setRenderObjBbox() { flags ^= RENDER_OBJ_BBOX; }
    void setRenderPolygonsBbox() { flags ^= RENDER_POLYGONS_BBOX; }
    void setRenderOverrideWireColor() { flags ^= RENDER_OVERRIDER_WIRE_COLOR; }
    void setRenderOverrideNormalColor() { flags ^= RENDER_OVERRIDER_NORMAL_COLOR; }

    void unSetAll() { flags = 0; }

};


enum LineVectorIndex {
    SHAPES = 0,
    POLY_CALC_NORNAL = 1,
    POLY_DATA_NORNAL = 2,
    VERTICES_CALC_NORMAL = 3,
    VERTICES_DATA_NORMAL = 4,
    OBJ_BBOX = 5,
    POLY_BBOX = 6,
    LAST = 7
};

class PolygonGC {
private:
    std::vector<std::shared_ptr< Vertex>> m_vertices; // List of vertices
    ColorGC m_color;                // Color of the polygon
    BBox m_bbox;
    Line m_calcNormalLine;
    Line m_dataNormalLine;
    bool m_hasDataNormal;
    void updateBounds(const Vertex& vert);
    void resetBounds();
    Vector3 calculateNormal() const;
public:
    // Constructor with a default color
    PolygonGC(ColorGC color);
    void setCalcAndDataNormalLines( Vector3 dataNormal);
    void setCalcNormalLines();
    Vector3 getCalcNormalNormolized();
    Vector3 getDataNormalNormolized();

    bool isClippedByBBox(const Matrix4& tMat) const;
    bool hasDataNormalLine() const;
    Line getCalcNormalLine(const ColorGC* overridingColor) const;
    Line getDataNormalLine(const ColorGC* overridingColor) const;
    void setColor(const ColorGC& newColor);
    const ColorGC& getColor() const;    
    void addVertex(std::shared_ptr<Vertex> vertex);    
    void clip();
    bool isBehindCamera() const;
    size_t vertexCount() const;
    void printVertices() const;
    void printBounds() const;
    void printColor() const;
    PolygonGC* applyTransformation(const Matrix4& transformation) const;
    void loadEdgesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadBboxLinesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadVertNLinesFromData(std::vector<Line>& container, const ColorGC* overridingColor)const;
    void loadVertNLinesFromCalc(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wfClrOverride,
    const ColorGC* nrmClrOverride, RenderMode& renderMode) const;
    BBox getBbox() const;
    ~PolygonGC()=default;
};
#endif