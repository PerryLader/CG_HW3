#pragma once
#include <iomanip>
#include "Vector3.h"
#include "ColorGC.h"

enum LightID
{
    LIGHT_ID_AMBIENT = -1,
    LIGHT_ID_1 = 0,
    LIGHT_ID_2,
    LIGHT_ID_3,
    LIGHT_ID_4,
    LIGHT_ID_5,
    LIGHT_ID_6,
    LIGHT_ID_7,
    LIGHT_ID_8,
    MAX_LIGHT
} ;

enum LightType
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT
} ;

enum LightSpace
{
    LIGHT_SPACE_VIEW=0,
    LIGHT_SPACE_LOCAL
};





 class PolygonGC;
 


enum EdgeMode {
    VISIBLE = 0,
    NO_VISIBLE = 1,
    SILHOUTTE = 2
};

typedef struct GData {
    float z_indx;
    const PolygonGC* polygon;
    ColorGC pixColor;
    Vector3 pixNorm;
    GData* next;
} gData;

const uint32_t RENDER_SHAPE = 1;
const uint32_t RENDER_POLYGONS_CALC_NORMALS = 2;
const uint32_t RENDER_POLYGONS_NORMALS_FROM_DATA = 4;
const uint32_t RENDER_CALC_VETICES_NORMALS = 8;
const uint32_t RENDER_DATA_VETICES_NORMALS = 16;
const uint32_t RENDER_OBJ_BBOX = 32;
const uint32_t RENDER_POLYGONS_BBOX = 64;
const uint32_t RENDER_OVERRIDER_WIRE_COLOR = 128;
const uint32_t RENDER_OVERRIDER_NORMAL_COLOR = 256;
const uint32_t REBDER_SILHOUTTE_COLOR = 512;


class RenderMode {
private:
    uint32_t flags = 513;
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
    bool getRenderSilhoutteColor() const { return REBDER_SILHOUTTE_COLOR & flags; }

    void setRenderShape() { flags ^= RENDER_SHAPE; }
    void setRenderPolygonsCalcNormal() { flags ^= RENDER_POLYGONS_CALC_NORMALS; }
    void setRenderPolygonsNormalFromData() { flags ^= RENDER_POLYGONS_NORMALS_FROM_DATA; }
    void setRenderCalcVertivesNormal() { flags ^= RENDER_CALC_VETICES_NORMALS; }
    void setRenderDataVertivesNormal() { flags ^= RENDER_DATA_VETICES_NORMALS; }
    void setRenderObjBbox() { flags ^= RENDER_OBJ_BBOX; }
    void setRenderPolygonsBbox() { flags ^= RENDER_POLYGONS_BBOX; }
    void setRenderOverrideWireColor() { flags ^= RENDER_OVERRIDER_WIRE_COLOR; }
    void setRenderOverrideNormalColor() { flags ^= RENDER_OVERRIDER_NORMAL_COLOR; }
    void setRenderSilhoutteColor() { flags ^= REBDER_SILHOUTTE_COLOR; }

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
enum bgPicMode {
    NONE = 0,
    STREACHED = 1,
    REPEATED = 2
};
struct bgPicstruct {
    bgPicMode m_bgPicMode;
    std::string m_fileLocation;
};
enum CAMERA_TYPE
{
    ORTHOGONAL = 0,
    PRESPECTIVE = 1,
    COUNT = 2
};