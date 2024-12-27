#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include <iomanip>



class Renderer
{
public:
    Renderer();
    ~Renderer();
   // void addModel(Model* model);
    uint32_t* getBuffer() const;
    void render(const Camera* camera, int width, int height, const std::vector<Model*> models, RenderMode& renderMode,
        const ColorGC& bgColor, const ColorGC& normalColor, const ColorGC& bBoxColor);

    void clear(bool clearBgBuffer);

private:
    uint32_t* m_Buffer; // RGB by width by height;
    float* m_ZBuffer; // width by height;
    uint32_t* m_BgBuffer; // width by height;
    Shader* m_shader;
    ColorGC m_bgColor;
    int m_width, m_height;

    void setWidth(int width) { m_width = width;};
    void setHeight(int height) { m_height = height;};
    void setBgColor(const ColorGC& bgColor) { m_bgColor = bgColor;};

    int getWidth() const { return m_width; };
    int getHeight() const { return m_height; };
    ColorGC getBgColor() const { return m_bgColor; };

    void createBuffers();
    void refreshBgBuffer();
    
    
};

#endif // RENDERER_H


