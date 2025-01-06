#pragma once
#include "Modules.h"
#include <algorithm>
#include <cmath>


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
    std::vector< LightSource> m_lightSources;
    Vector3 m_viewPos;
	//virtual void scanConvertion() = 0;
public:
    void addLightSource(const LightSource& newLightSource);
    //virtual void draw(std::vector<Geometry*> objs) = 0;
    ColorGC calcLightColor(const Vector3& pos, const Vector3& normal, ColorGC colorBeforeLight)const;

    Shader(float ambiantIntensity, ColorGC ambiantColor, float specularityExp, Vector3 viewPos) ;
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

