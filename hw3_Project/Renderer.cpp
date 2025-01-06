#include "Renderer.h"
#include <cstring> // For memset
#include <iostream>
#include <algorithm> // For std::sort

Renderer::Renderer():m_Buffer(nullptr),
m_GBuffer(nullptr),
m_BgBuffer(nullptr),
m_shader(nullptr),
m_width(0),
m_height(0),
m_bgInfo({ bgMode::SOLID,"",ColorGC(230,230,230) }) {
}



Renderer::~Renderer() {
    clear(true);
}
void Renderer::drawWireFrame(std::vector<Line> lines[LineVectorIndex::LAST])
{
   
    for (int i = LineVectorIndex::SHAPES; i < LineVectorIndex::LAST; i++) {        
        for (Line& line : lines[i]) {
            if (line.clip())
            {   
                line.draw(m_Buffer, m_GBuffer, m_width, m_height);
            }
        }
    }
}
void Renderer::drawSolid(std::vector<Geometry*> transformedGeometries,Shader& shader)
{
    //TODO should i use m_wisth and m_hight?and not take this from as parameters?
    for (auto& geo : transformedGeometries)
    {
        geo->fillGbuffer(m_GBuffer, m_width, m_height);
    }
    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            if (m_GBuffer[(y * m_width) + x].polygon)            {
                m_Buffer[(y * m_width) + x] = m_GBuffer[(y * m_width) + x].pixColor.getARGB();
            }
        }
    }

}
void Renderer::drawSilhoutteEdges(const std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap)
{    
    for (auto& pair : SilhoutteMap)
    {
        if (pair.second == EdgeMode::SILHOUTTE)
        {
            pair.first.drawSilhoutte(m_Buffer, m_GBuffer, this->m_width, this->m_height);
        }
    }
}

void Renderer::invalidateBG(const bgInfo& bg_info) {
    m_bgInfo = bg_info;
    delete[] m_BgBuffer;
    m_BgBuffer = (uint32_t*)malloc(sizeof(uint32_t) * m_width * m_height);
    if (m_bgInfo.mode == bgMode::SOLID)
        fillColorBG();
    else
        fillPngBG();
}
bool Renderer::isvalidBG(const bgInfo& bg_info) {
    if (bg_info.mode != m_bgInfo.mode)
        return false;
    if (strcmp(bg_info.pngPath,m_bgInfo.pngPath))
        return false;
    if (bg_info.mode == bgMode::SOLID && bg_info.color.getARGB() != m_bgInfo.color.getARGB())
        return false;
    return true;
}
void Renderer::invalidate(const bgInfo& bg_info, bool force) {
    createBuffers();
    if(force || !isvalidBG(bg_info))
        invalidateBG(bg_info);
}

void Renderer::render(const Camera* camera, int width, int height, const std::vector<Model*> models, RenderMode& renderMode,
    const bgInfo& bg_info, const ColorGC& normalColor, const ColorGC& bBoxColor){

    bool forceAll = false;
    if (getWidth() != width || getHeight() != height) {
        setWidth(width); setHeight(height);
        forceAll = true;
    }
    invalidate(bg_info, forceAll);
    memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t) * m_width * m_height);

    Matrix4 aspectRatioMatrix = Matrix4::scaling(Vector3(1.0f / (width / height), 1.0f, 1.0f));
    const Matrix4 viewProjectionMatrix = aspectRatioMatrix  * camera->getProjectionMatrix() * camera->getViewMatrix();

    // Transform and cull geometry
    std::vector<Geometry*> transformedGeometries;
    std::vector<Line> lines[LineVectorIndex::LAST];
    std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual> SilhoutteMap;
    bool flipNormals = true;//TODO get this parameter from user


    //TODO set default parm
    float ambiantIntensity = 0.5;
    ColorGC ambiantColor(255, 255, 255);
    float specularityExp = 16;
    Matrix4 invViewMatrix = camera->getViewMatrix().inverse();
    Vector3 cameraPos(invViewMatrix.m[3][0], invViewMatrix.m[3][1], invViewMatrix.m[3][2]);

    Shader shader(ambiantIntensity, ambiantColor, specularityExp, cameraPos);
   
    shader.addLightSource(LightSource(true, 1, 0,19,
        ColorGC(255, 255, 255),
        Vector3(-2, -1, -2),
        Vector3(1,1, 1),
        LightSourceType::LightSourceType_POINT));

    for (const auto& model : models) {
        Geometry* transformedGeometry;
        transformedGeometry = model->applyTransformation(viewProjectionMatrix,flipNormals);
        if (transformedGeometry) {
            transformedGeometry->fillVetrexesColor(shader);
            transformedGeometry->clip();            
            transformedGeometry->backFaceCulling(invViewMatrix);
            //TODO set renderMode depends on Silhoute from user            
            transformedGeometry->loadLines(lines, ColorGC(255,255,255,255), normalColor, renderMode, SilhoutteMap);
            transformedGeometries.push_back(transformedGeometry);
        }
    }
    //add axis origin for tests:
   
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(-1, 0, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(1, 0, 0, 1)).toVector3(), ColorGC(255, 0, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, -1, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 1, 0, 1)).toVector3(), ColorGC(0, 255, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, 0, -1, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 0, 1, 1)).toVector3(), ColorGC(0, 0, 255)));
    //the Final draw
    this->drawSilhoutteEdges(SilhoutteMap);
    this->drawSolid(transformedGeometries,shader);
    //this->drawWireFrame(lines);
    

    for (const auto& geom : transformedGeometries) {
        delete geom;
    }

}

void Renderer::clear(bool clearBg) {
    delete[] m_Buffer;
    m_Buffer = nullptr;
    delete[] m_GBuffer;
    m_GBuffer = nullptr;
    if(clearBg){
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
    m_GBuffer = new gData[m_width * m_height]; // Z-buffer
    gData initGdataObj = { FLT_MAX, nullptr, 0, 0, nullptr };
    std::fill(m_GBuffer, m_GBuffer + (m_width * m_height), initGdataObj);
    std::memset(m_Buffer, 0, sizeof(uint32_t) * m_width * m_height);
}
void Renderer::fillColorBG() {
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
            m_BgBuffer[i + j*m_width] = m_bgInfo.color.getARGB();
}

void Renderer::fillPngBG() {
    PngWrapper bgImage(m_bgInfo.pngPath, m_width, m_height);
    if (!bgImage.ReadPng())
    {
        std::cout << "hereeeeeee123";
        throw;
    }
    int imgHeight = bgImage.GetHeight();
    int imgWidth = bgImage.GetWidth();
    if (m_bgInfo.mode == bgMode::REPEATED)
    {
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
            {
                int tempRgbaColor = bgImage.GetValue(x % imgWidth, y % imgHeight);
                tempRgbaColor = (tempRgbaColor >> 8) | (tempRgbaColor << (24));
                m_BgBuffer[x + (y * m_width)] = tempRgbaColor;

            }
    }
    else if (m_bgInfo.mode == bgMode::STREACHED)
    {
        float xScale = static_cast<float>(imgWidth) / m_width;
        float yScale = static_cast<float>(imgHeight) / m_height;
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
            {
                int srcX = static_cast<int>(x * xScale);
                int srcY = static_cast<int>(y * yScale);
                int tempRgbaColor = bgImage.GetValue(srcX, srcY);
                tempRgbaColor = (tempRgbaColor >> 8) | (tempRgbaColor << (24));
                m_BgBuffer[x + (y * m_width)] = tempRgbaColor;
            }
    }
}
