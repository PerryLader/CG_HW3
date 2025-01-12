#pragma once
#include "Modules.h"
#include <algorithm>
#include <cmath>
#include "Light.h"


class Shader
{
private:
    float m_specularityExp;
    LightParams m_lightSources[LightID::MAX_LIGHT];
    LightParams m_ambient;
    Vector3 m_viewPos;
	//virtual void scanConvertion() = 0;
public:
    //virtual void draw(std::vector<Geometry*> objs) = 0;
    ColorGC calcLightColorAtPos(const Vector3& pos, const Vector3& normal, ColorGC colorBeforeLight) const;
    void applyShading(uint32_t* dest, const gData* gBuffer, int width, int height, const RenderMode& rd) const;
    Shader();

    void updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);

    // Getter and Setter for m_viewPos
    Vector3 getViewPos() const {return m_viewPos;}
    void setViewPos(const Vector3& viewPos) {m_viewPos = viewPos;}
};

//class NoShadeShader :
//    public Shader
//{
//private:
//    
//public:
//
//    
//};

