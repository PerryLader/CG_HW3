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
m_height(0),
m_bgPic({bgPicMode::NONE,std::string("default")}){}



Renderer::~Renderer() {
    clear(true);
}
void Renderer::drawWireFrame(std::vector<Line> lines[LineVectorIndex::LAST])
{
   
    for (int i = LineVectorIndex::SHAPES; i < LineVectorIndex::LAST; i++) {        
        for (Line& line : lines[i]) {
            if (line.clip())
            {               
                //TODO should i use m_wisth and m_hight?and not take this from as parameters?
                line.draw(m_Buffer, m_ZBuffer, this->m_width, this->m_height);
            }
        }
    }
}
void Renderer::drawSolid(std::vector<Geometry*> transformedGeometries)
{
    //TODO should i use m_wisth and m_hight?and not take this from as parameters?
    for (auto& geo : transformedGeometries)
    {
        geo->draw(m_Buffer, m_ZBuffer, m_width, m_height);
    }
}
void Renderer::drawSilhoutteEdges(const std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap)
{    
    for (auto& pair : SilhoutteMap)
    {
        if (pair.second == EdgeMode::SILHOUTTE)
        {
            pair.first.drawSilhoutte(m_Buffer, m_ZBuffer, this->m_width, this->m_height);
        }
    }
}
void Renderer::render(const Camera* camera, int width, int height,const std::vector<Model*> models,  RenderMode& renderMode,
    const ColorGC& bgColor, const ColorGC& normalColor, const ColorGC& wireColor, bgPicstruct bgPic) {

    if (getWidth() != width || getHeight() != height ||
        getBgColor().getARGB() != bgColor.getARGB()||
        m_bgPic.m_bgPicMode!=bgPic.m_bgPicMode || m_bgPic.m_fileLocation != bgPic.m_fileLocation)
    {
        setWidth(width); setHeight(height); setBgColor(bgColor);
        m_bgPic.m_bgPicMode = bgPic.m_bgPicMode;
        m_bgPic.m_fileLocation = bgPic.m_fileLocation;
        if (m_bgPic.m_bgPicMode == bgPicMode::NONE)
        {
            refreshBgColorBuffer();
        }
        else
        {
            refreshBgPicBuffer();
        }
    }
    createBuffers();
    memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t)*width*height);


    Matrix4 aspectRatioMatrix = Matrix4::scaling(Vector3(1.0f / (width / height), 1.0f, 1.0f));
    const Matrix4 viewProjectionMatrix = aspectRatioMatrix  * camera->getProjectionMatrix() * camera->getViewMatrix();

    // Transform and cull geometry
    std::vector<Geometry*> transformedGeometries;
    std::vector<Line> lines[LineVectorIndex::LAST];
    std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual> SilhoutteMap;
    bool flipNormals = false;//TODO get this parameter from user
    for (const auto& model : models) {
        Geometry* transformedGeometry;
        transformedGeometry = model->applyTransformation(viewProjectionMatrix,flipNormals);
        if (transformedGeometry) {
            transformedGeometry->clip();            
            transformedGeometry->backFaceCulling(camera->getViewMatrix().inverse());
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
    this->drawSolid(transformedGeometries);
    this->drawWireFrame(lines);
    

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
    std::fill(m_ZBuffer, m_ZBuffer + (m_width * m_height), FLT_MAX);
    std::memset(m_Buffer, 0, sizeof(uint32_t) * m_width * m_height);
}
void Renderer::refreshBgColorBuffer() {
    delete[] m_BgBuffer;
    m_BgBuffer = new uint32_t[m_width * m_height]; // background RGB buffer
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
            m_BgBuffer[i + j*m_width] = m_bgColor.getARGB();
}

void Renderer::refreshBgPicBuffer()
{
    delete[] m_BgBuffer;
    m_BgBuffer = new uint32_t[m_width * m_height]; // background RGB buffer
    PngWrapper bgImage(m_bgPic.m_fileLocation.c_str(), m_width, m_height);
    if (!bgImage.ReadPng())
    {
        std::cout <<"hereeeeeee123";
        throw;
    }
    int imgHeight= bgImage.GetHeight();
    int imgWidth = bgImage.GetWidth();
    


    if (m_bgPic.m_bgPicMode == bgPicMode::REPEATED)
    {
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
            {
                int tempRgbaColor = bgImage.GetValue(x % imgWidth, y % imgHeight);
                tempRgbaColor = (tempRgbaColor >> 8) | (tempRgbaColor << (24));
                m_BgBuffer[x + (y * m_width)] = tempRgbaColor;

            }
    }
    else if (m_bgPic.m_bgPicMode == bgPicMode::STREACHED)
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


