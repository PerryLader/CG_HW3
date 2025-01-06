#pragma once
#include "Modules.h"




//class LightParams
//{
//public:
//
//    //light enabled
//    bool enabled;
//    //type directional,point,spot
//    LightType type;
//    //local or view space
//    LightSpace space;
//    //color 0-255 RGB
//    ColorGC light_Color;
//
//        //position
//    Vector3 light_Pos;
//
//    //direction
//    Vector3 light_Dir;
//
//
//    LightParams() :
//        enabled(false), type(LIGHT_TYPE_DIRECTIONAL), space(LightSpace),
//        colorR(255), colorG(255), colorB(255), posX(0), posY(0), posZ(0),
//        dirX(0), dirY(0), dirZ(0)
//    {
//    }
//
//protected:
//private:
//};



class Shader
{
private:
	virtual void scanConvertion() = 0;
public:
    //virtual void draw(std::vector<Geometry*> objs) = 0;
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

