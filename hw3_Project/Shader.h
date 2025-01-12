#pragma once
#include "Modules.h"
#include <algorithm>
#include <cmath>


#define MAX_LIGHTING 7

class LightSource
{
public:
    bool m_enabled;
    float m_lightIntencity;
    float m_diffuseCo;
    float m_specullarCo;
    ColorGC m_light_Color;
    Vector3 m_light_Pos;
    Vector3 m_light_Dir;
    LightSourceType m_lightType;
    LightSource() :
        m_enabled(true), m_lightIntencity(0),m_diffuseCo(0),m_specullarCo(0),
        m_light_Color(255, 255, 255), m_light_Pos(0, 0, 0),
        m_light_Dir(0, 0, 0),m_lightType(LightSourceType::LightSourceType_DIRECTIONAL){}
    

    LightSource(bool m_enabled, float m_lightIntencity, float m_diffuseCo, float m_specullarCo, const ColorGC& m_light_Color, const Vector3& m_light_Pos,
        const Vector3& m_light_Dir, const LightSourceType& m_lightType)
        : m_enabled(m_enabled), m_lightIntencity(m_lightIntencity), m_diffuseCo(m_diffuseCo), m_specullarCo(m_specullarCo), m_light_Color(m_light_Color),
        m_light_Pos(m_light_Pos), m_light_Dir(m_light_Dir), m_lightType(m_lightType)
    {
    }
};

class Shader
{
private:
    float m_ambiantIntensity;
    ColorGC m_ambiantColor;
    float m_specularityExp;
    int m_lights;
    LightSource m_lightSources[MAX_LIGHTING];
    Vector3 m_viewPos;
	//virtual void scanConvertion() = 0;
public:
    //virtual void draw(std::vector<Geometry*> objs) = 0;
    ColorGC calcLightColorAtPos(const Vector3& pos, const Vector3& normal, ColorGC colorBeforeLight) const;
    void applyShading(uint32_t* dest, const gData* gBuffer, int width, int height, const RenderMode& rd) const;
    Shader();

    // Getter and Setter for m_ambiantIntensity
    float getAmbiantIntensity() const { return m_ambiantIntensity;}
    void setAmbiantIntensity(float ambiantIntensity) { m_ambiantIntensity = ambiantIntensity;}

    // Getter and Setter for m_ambiantColor
    ColorGC getAmbiantColor() const { return m_ambiantColor;}
    void setAmbiantColor(const ColorGC& ambiantColor) { m_ambiantColor = ambiantColor;}

    // Getter and Setter for m_specularityExp
    float getSpecularityExp() const { return m_specularityExp;}
    void setSpecularityExp(float specularityExp) { m_specularityExp = specularityExp;}

    // Getter and Setter for m_lightSources
    LightSource* getLightSources() { return m_lightSources;}
    void addLightSource(const LightSource lightSource) { if(m_lights<MAX_LIGHTING) m_lightSources[m_lights++] = lightSource;}
    void clearLightSources(const LightSource lightSource) { m_lights = 0;}

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

