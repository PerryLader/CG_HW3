#include "Polygon.h"
#include <afxwin.h>

/////////////////////////////
//do youw want us          //
//to implement             //
//matrix ops using         //
//OpenCl?? answer          //
//yes my man!BJ also?      //
//Yes my friend! coffee?   //
//sure things,but coffe    //
//makes Pop                //
/////////////////////////////
void BBox::toPrint() const{
    std::cout << "Boudning Box: " << m_minBounds <<", " << m_maxBounds << std::endl;
}
Vector3 BBox::getMax() const
{
    return m_maxBounds;
}
Vector3 BBox::getMin() const
{
    return m_minBounds;
}
void BBox::updateBBox(const Vector3& vert) {
    m_minBounds.x = min(m_minBounds.x, vert.x);
    m_minBounds.y = min(m_minBounds.y, vert.y);
    m_minBounds.z = min(m_minBounds.z, vert.z);
    m_maxBounds.x = max(m_maxBounds.x, vert.x);
    m_maxBounds.y = max(m_maxBounds.y, vert.y);
    m_maxBounds.z = max(m_maxBounds.z, vert.z);
}
void BBox::updateBBox(const BBox& box)
{
    updateBBox(box.m_maxBounds);
    updateBBox(box.m_minBounds);
}
bool BBox::bboxCollide(const BBox& bbox) const{
    return BBox::bboxCollide(*this, bbox);
}
bool BBox::bboxCollide(const BBox& bbox1, const BBox& bbox2) {
    // Check for overlap along the x-axis
    bool xOverlap = (bbox1.m_minBounds.x <= bbox2.m_maxBounds.x) && (bbox1.m_maxBounds.x >= bbox2.m_minBounds.x);

    // Check for overlap along the y-axis
    bool yOverlap = (bbox1.m_minBounds.y <= bbox2.m_maxBounds.y) && (bbox1.m_maxBounds.y >= bbox2.m_minBounds.y);

    // Check for overlap along the z-axis
    bool zOverlap = (bbox1.m_minBounds.z <= bbox2.m_maxBounds.z) && (bbox1.m_maxBounds.z >= bbox2.m_minBounds.z);

    // Bounding boxes collide if they overlap along all three axes
    return xOverlap && yOverlap && zOverlap;
}
BBox BBox::transformBBox(const Matrix4& tmat) const {
    BBox res;
    res.updateBBox((tmat * Vector4::extendOne(m_maxBounds)).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_minBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_maxBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_maxBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_minBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_minBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_maxBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(m_maxBounds)).toVector3());
    return res;
}
std::vector<Line> BBox::getLinesOfBbox(const ColorGC& bBoxColor) const
{
    Vector3 corners[8] = {
       {m_minBounds.x, m_minBounds.y, m_minBounds.z},
       {m_minBounds.x, m_minBounds.y, m_maxBounds.z},
       {m_minBounds.x, m_maxBounds.y, m_minBounds.z},
       {m_minBounds.x, m_maxBounds.y, m_maxBounds.z},
       {m_maxBounds.x, m_minBounds.y, m_minBounds.z},
       {m_maxBounds.x, m_minBounds.y, m_maxBounds.z},
       {m_maxBounds.x, m_maxBounds.y, m_minBounds.z},
       {m_maxBounds.x, m_maxBounds.y, m_maxBounds.z}
    };
    std::vector<Line> lines = {
        // Bottom face
        {corners[0], corners[1],bBoxColor},
        {corners[1], corners[5],bBoxColor},
        {corners[5], corners[4],bBoxColor},
        {corners[4], corners[0],bBoxColor},

        // Top face
        {corners[2], corners[3],bBoxColor},
        {corners[3], corners[7],bBoxColor},
        {corners[7], corners[6],bBoxColor},
        {corners[6], corners[2],bBoxColor},

        // Vertical edges
        {corners[0], corners[2],bBoxColor},
        {corners[1], corners[3],bBoxColor},
        {corners[4], corners[6],bBoxColor},
        {corners[5], corners[7],bBoxColor}
    };
    return lines;

}

// Update min and max bounds
void PolygonGC::updateBounds(const Vertex& vert) {
    m_bbox.updateBBox(vert.loc());
}
bool PolygonGC::isClippedByBBox(const Matrix4& tMat) const {
    return !BBox::bboxCollide(getBbox().transformBBox(tMat), BBox::unitBBox());
}
// reset min and max bounds
void PolygonGC::resetBounds() {
    if (m_vertices.empty()) {
        m_bbox = BBox();
        return;
    }
    m_bbox = BBox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    for (auto vertex : m_vertices) {
        m_bbox.updateBBox(vertex->loc());
    }
}

// Constructor with a default color
PolygonGC::PolygonGC(ColorGC color) :m_toDraw(true), m_color(color), m_hasDataNormal(false){
    resetBounds();
}

void PolygonGC::setCalcAndDataNormalLines( Vector3 dataNormal)
{
    Vector3 centerPoint(0, 0, 0);
    for (const auto& v : m_vertices)
    {
        centerPoint += v->loc();
    }
    centerPoint /= m_vertices.size();
    m_calcNormalLine= Line(centerPoint, centerPoint + (calculateNormal().normalized() * 0.25),m_color);
    m_dataNormalLine = Line(centerPoint, centerPoint + (dataNormal.normalized() * 0.25), m_color);
}

void PolygonGC::setCalcNormalLines()
{
    Vector3 centerPoint(0, 0, 0);
    for (const auto& v : m_vertices)
    {
        centerPoint += v->loc();
    }
    centerPoint /= m_vertices.size();
    m_calcNormalLine = Line(centerPoint, centerPoint + (calculateNormal().normalized() * 0.25));
    m_hasDataNormal = false;
}

Vector3 PolygonGC::getCalcNormalNormolized()
{
    return (m_calcNormalLine.m_b - m_calcNormalLine.m_a).normalized();
}

Vector3 PolygonGC::getDataNormalNormolized()
{
    if (!hasDataNormalLine())
    {
        throw std::exception();
    }
    return (m_dataNormalLine.m_b - m_dataNormalLine.m_a).normalized();;
}




// Set the color of the polygon
void PolygonGC::setColor(const ColorGC& newColor) {   
    m_color = newColor;
}

// Get the color of the polygon
const ColorGC& PolygonGC::getColor() const
{
    return m_color;
}

void PolygonGC::addVertex(std::shared_ptr<Vertex> vertex) {
    if (vertex)
    {
        m_vertices.push_back(vertex);        
        updateBounds(*vertex);
    }
}

// Get the number of vertices
size_t PolygonGC::vertexCount() const{
    return m_vertices.size();
}


// Print all vertices
void PolygonGC::printVertices() const{

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::cout << "                  Vertex[" << i << "]: ";
        m_vertices.at(i)->print();
        std::cout << "\n";
    }
}

// Print bounds
void PolygonGC::printBounds() const{
    m_bbox.toPrint();
}

bool PolygonGC::isBehindCamera() const{
    for (size_t i = 0; i < m_vertices.size(); ++i)
        if (m_vertices[i]->loc().z > 0)
            return false;
    return true;
}
// Print polygon color
void PolygonGC::printColor() const{
    std::cout << m_color.toHex();   
}

void PolygonGC::clip()
{
    std::set<std::shared_ptr<Vertex> > outscopeVertices;
    std::vector<std::shared_ptr<Vertex> > inscopeVertices;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::shared_ptr<Vertex> v1 = m_vertices[i];
        std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];

        // Check if vertices are inside the clipping volume
        bool v1Inside = v1->isInsideClipVolume();
        bool v2Inside = v2->isInsideClipVolume();

        if (v1Inside && v2Inside) {
            // Both vertices are inside, add v2 to the clipped vertices
            inscopeVertices.push_back(v2);
        }
        else if (v1Inside && !v2Inside) {
            // v1 is inside, v2 is outside, add intersection point 
            std::vector<Vector3> tempVector = Vertex::intersectionVertex(v1, v2);
            //for debugging
            if (tempVector.size() != 1)
            {
                std::cout << "1";
                continue;
            }
            else
            {
                std::shared_ptr<Vertex> tempVertex(new Vertex(*v2.get()));
                tempVertex->setLoc(tempVector[0]);
                inscopeVertices.push_back(tempVertex);
                outscopeVertices.insert(v2);
            }
        }
        else if (!v1Inside && v2Inside) {
            // v1 is outside, v2 is inside, add intersection point and v2
            
            std::vector<Vector3> tempVector = Vertex::intersectionVertex(v1, v2);

            //for debugging
            if (tempVector.size() != 1)
            {
                std::cout << "2";
                continue;
            }
            else
            {
                std::shared_ptr<Vertex> tempVertex(new Vertex(*v1.get()));
                tempVertex->setLoc(tempVector[0]);
                inscopeVertices.push_back(tempVertex);
                inscopeVertices.push_back(v2);
                outscopeVertices.insert(v1);
            }
            
        }
        else if (!v1Inside && !v2Inside)
        {
            
            std::vector<Vector3> tempVector = Vertex::intersectionVertex(v1, v2);
            //for debugging
            if (tempVector.size() == 2)
            {
                // v1 is outside, v2 is outside, add 2 intersection points
                std::shared_ptr<Vertex> tempVertexV2(new Vertex(*v2.get()));
                tempVertexV2->setLoc(tempVector[1]);

                std::shared_ptr<Vertex> tempVertexV1(new Vertex(*v1.get()));
                tempVertexV1->setLoc(tempVector[0]);

                inscopeVertices.push_back(tempVertexV1);
                inscopeVertices.push_back(tempVertexV2);
                
            }
            //for debugging
            else if (tempVector.size() != 0)
            {
                std::cout << tempVector.size()<<std::endl;

            }
            outscopeVertices.insert(v1);
            outscopeVertices.insert(v2);
           
        }
    }
    for (auto& elem : outscopeVertices) {
        //delete elem;
    }
    m_vertices = inscopeVertices;
    this->m_calcNormalLine.clip();
    if (this->m_hasDataNormal)
    {
        this->m_dataNormalLine.clip();
    }
    this->resetBounds();
}
// Function to apply a transformation matrix to all vertices
PolygonGC* PolygonGC::applyTransformation(const Matrix4& transformation, bool flipNormals) const
{
    PolygonGC* newPoly = new PolygonGC(this->m_color);
    for (const auto& vertex : m_vertices) {
        newPoly->addVertex(vertex->getTransformedVertex(transformation, flipNormals));
    }
    newPoly->m_calcNormalLine = this->m_calcNormalLine.getTransformedLine(transformation);
    if (newPoly->m_hasDataNormal)
    {
        newPoly->m_dataNormalLine = this->m_dataNormalLine.getTransformedLine(transformation);
    }
    if (flipNormals)
    {
        newPoly->flipNormals();
    }
    return newPoly;
}


static bool ifEdgeBBOXCutUnitCube(const Vertex& v1, const Vertex& v2) {
    BBox b;
    b.updateBBox(v1.loc());
    b.updateBBox(v2.loc());
    BBox unit = BBox::unitBBox();
    return BBox::bboxCollide(b, unit);
}
void PolygonGC::loadEdgesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const {
    ColorGC line_color = overridingColor == nullptr ? m_color : *overridingColor;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::shared_ptr<Vertex> v1 = m_vertices[i];
        std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];
        if(ifEdgeBBOXCutUnitCube(*v1, *v2))
            container.push_back(Line(v1->loc(), v2->loc(), line_color));
    }
}

void PolygonGC::loadLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wfClrOverride,
    const ColorGC* nrmClrOverride, RenderMode& renderMode) const {

    //std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wireColor);
    if (renderMode.getRenderShape()) loadEdgesToContainer(lines[LineVectorIndex::SHAPES], wfClrOverride);
    if (renderMode.getRenderDataVertivesNormal())
    {
        try {
            loadVertNLinesFromData(lines[LineVectorIndex::VERTICES_DATA_NORMAL], nrmClrOverride);
        }
        catch (...) {
            renderMode.setRenderDataVertivesNormal();
            lines[LineVectorIndex::VERTICES_DATA_NORMAL].clear();
            if (!renderMode.getRenderCalcVertivesNormal())
                renderMode.setRenderCalcVertivesNormal();
            AfxMessageBox(_T("This Object wasnt provided with vertice normals!"));
        }
    }
    if (renderMode.getRenderCalcVertivesNormal()) loadVertNLinesFromCalc(lines[LineVectorIndex::VERTICES_CALC_NORMAL], nrmClrOverride);
    if (renderMode.getRenderPolygonsBbox()) loadBboxLinesToContainer(lines[LineVectorIndex::POLY_BBOX], wfClrOverride);
    if (renderMode.getRenderPolygonsNormalFromData()){
        try {
            lines[LineVectorIndex::POLY_DATA_NORNAL].push_back(getDataNormalLine(nrmClrOverride));
        }
        catch (...) {
            renderMode.setRenderPolygonsNormalFromData();
            lines[LineVectorIndex::POLY_DATA_NORNAL].clear();
            if (!renderMode.getRenderPolygonsCalcNormal())
                renderMode.setRenderPolygonsCalcNormal();
            AfxMessageBox(_T("This Object wasnt provided with polygon normals!"));
        }
    }
    if (renderMode.getRenderPolygonsCalcNormal()) lines[LineVectorIndex::POLY_CALC_NORNAL].push_back(getCalcNormalLine(nrmClrOverride));
}



int findIntersectionAndFitToScreen(Line& line, float y,Vector3 &vec, int halfWidth, int halfhight) {

    line.m_a.y = (line.m_a.y * halfhight) + halfhight;
    line.m_b.y = (line.m_b.y * halfhight) + halfhight;
    line.m_a.x = (line.m_a.x * halfWidth) + halfWidth;
    line.m_b.x = (line.m_b.x * halfWidth) + halfWidth;
    line.m_a.xyRound();
    line.m_b.xyRound();
    int diffA = (line.m_a.y - y);
    int epsilon = 2;
    if ((line.m_a.y == line.m_b.y)&&
        (diffA < epsilon) && (diffA > -epsilon))
    {
       
        return 2; //horizontal line

    }
    if ((line.m_a.y - y) * (line.m_b.y - y) > 0) {
        return 0; // No intersection
    }

    float t = (y - line.m_a.y) / (line.m_b.y - line.m_a.y);
    vec = (line.m_a * (1 - t)) + line.m_b * t;
    return 1;
}



void PolygonGC::draw(uint32_t* buffer, float* zBuffer, int width, int hight) const
{
    //override color?
    //TODO
    std::vector<Line> lineVector;
    this->loadEdgesToContainer(lineVector, nullptr);

    int halfWidth = width / 2;
    int halfhight = hight / 2;
    int yMax = (m_bbox.getMax().y * halfhight) + halfhight;
    int yMin = (m_bbox.getMin().y * halfhight) + halfhight;

    std::sort(lineVector.begin(), lineVector.end(), [](const Line& a, const Line& b) {return a.yMin() < b.yMin();});

    for (int i = yMin; i < yMax; i++)
    {
        std::vector<Vector3> vectors;
        Vector3 samllestVecX(10000, 10000, 10000);
        Vector3  biggestVecX(0, 0, 0);
        int counter = 0;
        for (auto line : lineVector)
        {
            Vector3 tempVec;
          
            int intersectionResault = findIntersectionAndFitToScreen(line, i, tempVec, halfWidth, halfhight);
            if (intersectionResault ==1)
            {
                tempVec.xyRound();
              //  vectors.push_back(tempVec);
                samllestVecX = samllestVecX.x < tempVec.x ? samllestVecX : tempVec;
                biggestVecX = biggestVecX.x > tempVec.x ? biggestVecX : tempVec;
            }
            else if (intersectionResault == 2)
            {

                samllestVecX = samllestVecX.x < line.m_a.x ? samllestVecX : line.m_a;
                biggestVecX = biggestVecX.x > line.m_a.x ? biggestVecX : line.m_a;
                samllestVecX = samllestVecX.x < line.m_b.x ? samllestVecX : line.m_b;
                biggestVecX = biggestVecX.x > line.m_b.x ? biggestVecX : line.m_b;
            }
            counter += intersectionResault;
        }
        if (counter == 4)
        {
            int x = 5;
        }
       // std::sort(vectors.begin(), vectors.end(), [](const Vector3& a, const Vector3& b) {return a.x < b.x;});

        for (int j = samllestVecX.x; j < biggestVecX.x; j++)
        {
            uint32_t* bufferAddr = buffer + ((i * width) + j);
            float* zbufferAddr = zBuffer + ((i * width) + j);
            float t = (j - samllestVecX.x) / (biggestVecX.x - samllestVecX.x);
            float interpolatedZ = (samllestVecX.z * (1 - t)) + t * biggestVecX.z;
            if (interpolatedZ != 0)
            {
                int xxx = 5;
            }
            float zz = zBuffer[(i * width) + j];
            if (zBuffer[(i * width) + j] > interpolatedZ)
            {
                zBuffer[(i * width) + j] = interpolatedZ;
                buffer[(i * width) + j] = this->getColor().getARGB();
            }
        }
    }
}

void PolygonGC::flipNormals()
{    
    this->m_calcNormalLine.flipLine();
    if (m_hasDataNormal)
    {
        m_dataNormalLine.flipLine();
    }
}

// get polygon Bbox
BBox PolygonGC::getBbox() const{
    return m_bbox;
}

Line PolygonGC::getCalcNormalLine(const ColorGC* overridingColor) const
{
    return Line(m_calcNormalLine.m_a, m_calcNormalLine.m_b, overridingColor == nullptr ? m_calcNormalLine.m_color : *overridingColor);

}

Line PolygonGC::getDataNormalLine(const ColorGC* overridingColor) const
{
    if (!hasDataNormalLine())
    {
        throw std::exception();
    }

    return Line(m_dataNormalLine.m_a, m_dataNormalLine.m_b, overridingColor == nullptr ? m_dataNormalLine.m_color : *overridingColor);
}

bool PolygonGC::hasDataNormalLine() const{
    return m_hasDataNormal;
}

void PolygonGC::setToDraw(bool toDraw)
{
    m_toDraw = toDraw;
}

bool PolygonGC::getToDraw() const
{
    return m_toDraw;
}


void PolygonGC::loadVertNLinesFromData(std::vector<Line>& container, const ColorGC* overridingColor)const {
    for (const auto& vert : m_vertices) {
        try {
            container.push_back(vert->getDataNormalLine());
        }
        catch (...) {
            container.clear();
            throw std::exception();
        }
    }
}

void PolygonGC::loadVertNLinesFromCalc(std::vector<Line>&container, const ColorGC* overridingColor) const {
    for (const auto& vert : m_vertices) {
        container.push_back(vert->getCalcNormalLine());
    }
}

void PolygonGC::loadBboxLinesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const
{
    std::vector<Line> bboxLines = m_bbox.getLinesOfBbox(overridingColor == nullptr ? m_color : *overridingColor);
    container.insert(container.end(), bboxLines.begin(), bboxLines.end());
}
Vector3 PolygonGC::calculateNormal() const {
    if (m_vertices.size() < 3)
    {
        throw std::runtime_error("whaht the hell just happend?is it polygon with less then 2 vertices???hemmm?");
    }
    const Vector3 vec1 = m_vertices.at(1)->loc() - m_vertices.at(0)->loc();
    const Vector3 vec2 = m_vertices.at(2)->loc() - m_vertices.at(1)->loc();
    return Vector3::cross(vec2, vec1).normalized();

}