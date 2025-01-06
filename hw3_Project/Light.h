#pragma once
#include "Modules.h"

class LightParams
{
public:

    //light enabled
    bool enabled;
    //type directional,point,spot
    LightType type;
    //local or view space
    LightSpace space;

    //color 0-255 RGB
    int colorR;
    int colorG;
    int colorB;

    //position
    double posX;
    double posY;
    double posZ;

    //direction
    double dirX;
    double dirY;
    double dirZ;
    
    LightParams():
	enabled(false),type(LIGHT_TYPE_DIRECTIONAL),space(LIGHT_SPACE_VIEW),
	colorR(255),colorG(255),colorB(255),posX(0),posY(0),posZ(0),
	dirX(0),dirY(0),dirZ(0)
    {}

protected:
private:
};
