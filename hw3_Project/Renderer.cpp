#include "Renderer.h"
#include <cstring> // For memset
#include <iostream>
#include <algorithm> // For std::sort

Renderer::Renderer():m_Buffer(nullptr),
m_ZBuffer(nullptr),
m_BgBuffer(nullptr),
m_shader(nullptr),
m_bgColor(),
m_width(0),
m_height(0){}

Renderer::~Renderer() {
    clear(true);
}

void Renderer::render(const Camera* camera, int width, int height,const std::vector<Model*> models,  RenderMode& renderMode,
    const ColorGC& bgColor, const ColorGC& normalColor, const ColorGC& wireColor) {
    const char* debuging = bgColor.toHex().c_str();

    if (getWidth() != width || getHeight() != height || getBgColor().getARGB() != bgColor.getARGB()) {
        setWidth(width); setHeight(height); setBgColor(bgColor);
        refreshBgBuffer();
    }
    createBuffers();
    memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t)*width*height);
    // Combine view and projection matrices

    float aspectRatio = width / height;
    Matrix4 aspectRatioMatrix = Matrix4::scaling(Vector3(1.0f / aspectRatio, 1.0f, 1.0f));
    Matrix4 view = camera->getViewMatrix();
    Matrix4 proj = camera->getProjectionMatrix();
    const Matrix4 viewProjectionMatrix = aspectRatioMatrix  *  proj * view;

    // Transform and cull geometry
    std::vector<Geometry*> transformedGeometries;
    std::vector<Line> lines[LineVectorIndex::LAST];
    int i = 0;
    for (const auto& model : models) {
        Geometry* transformedGeometry;
        transformedGeometry = model->onDraw(viewProjectionMatrix);
        if (transformedGeometry) {
            transformedGeometry->loadLines(lines, wireColor, normalColor, renderMode);
            transformedGeometries.push_back(transformedGeometry);
        }
    }
    //add axis origin for tests:
    Vector4 e = viewProjectionMatrix * Vector4(0, 0, -1, 1);
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(-1, 0, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(1, 0, 0, 1)).toVector3(), ColorGC(255, 0, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, -1, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 1, 0, 1)).toVector3(), ColorGC(0, 255, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, 0, -1, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 0, 1, 1)).toVector3(), ColorGC(0, 0, 255)));
    //the Final draw
    for (std::vector<Line>& singleTypeLine : lines) {
        for (Line& line : singleTypeLine){
            // if (edge.isVisible()) {          
            if (line.clip())
            {
                line.draw(m_Buffer, this->m_width, this->m_height);
            }
        // }
        }
    }
    for (const auto& geom : transformedGeometries) {
        delete geom;
    }

}

void Renderer::clear(bool clearBgBuffer) {
    delete[] m_Buffer;
    m_Buffer = nullptr;
    delete[] m_ZBuffer;
    m_ZBuffer = nullptr;
    if (clearBgBuffer) {
        delete[] m_BgBuffer;
        m_BgBuffer = nullptr;
    }
}
uint32_t* Renderer::getBuffer() const{
    return m_Buffer;
}
void Renderer::createBuffers() {
    clear(false);
    m_Buffer = new uint32_t[m_width * m_height]; // RGB buffer
    m_ZBuffer = new float[m_width * m_height]; // Z-buffer
    std::memset(m_ZBuffer, 0, sizeof(float) * m_width * m_height);
    std::memset(m_Buffer, 0, sizeof(uint32_t) * m_width * m_height);
}
void Renderer::refreshBgBuffer() {
    delete[] m_BgBuffer;
    m_BgBuffer = new uint32_t[m_width * m_height]; // background RGB buffer
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
            m_BgBuffer[i + j*m_width] = m_bgColor.getARGB();
}

//
//void Renderer::fillBackgroundColor(const ColorGC& bg_color) {
//    const auto& color = bg_color.getRGBA();
//    std::memset(m_Buffer, 0, sizeof(color) * 4 * width * height);
//}

