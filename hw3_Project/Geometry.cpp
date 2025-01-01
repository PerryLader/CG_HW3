#include "Geometry.h"

Geometry::Geometry(const std::string& name, const ColorGC& color) : m_name(name) ,m_bBox(), m_objColor(color){}

// Destructor
Geometry::~Geometry() {
	for (PolygonGC* polygon : m_polygons) {
		delete polygon;
	}
	m_polygons.clear();
}

BBox Geometry::getBBox() const{
	return m_bBox;
}
std::string Geometry::getName() const{
	return this->m_name;
}

void Geometry::addPolygon(PolygonGC* poli)
{
	this->m_polygons.push_back(poli);
	m_bBox.updateBBox(poli->getBbox());
}

Geometry* Geometry::applyTransformation(const Matrix4& tMat) const{
	
	Geometry* res = new Geometry(m_name,this->m_objColor);
	for (const auto& poly : m_polygons) {
		if (!poly->isClippedByBBox(tMat)) {
			res->addPolygon(poly->applyTransformation(tMat));
		}
	}
	return res;
}
void Geometry::calcVertxNormal()
{
	for (std::pair<Vector3, std::shared_ptr<Vertex>> t : m_map)
	{
		t.second->setCalcNormalLine();
	}
}

void Geometry::backFaceCulling(const Matrix4 &invViewMatrix) {
    //const Vector3 camera_vec = Vector3::unitZ();
	Vector3 temp(invViewMatrix.m[3][0], invViewMatrix.m[3][1], invViewMatrix.m[3][2]);
	for (auto& poly : m_polygons)
	{
		if (Vector3::dot(temp, -poly->getCalcNormalNormolized()) <= 0)
		{
			poly->setToDraw(false);
		}

	}
	
}

void Geometry::createObjBboxLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wireColor) const
{
	std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wireColor);
	lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());	
}

void Geometry::draw(uint32_t* buffer, float* zBuffer, int width, int hight) const
{
	for (auto& poly : m_polygons)
	{
		if(poly->getToDraw())
		{
			poly->draw(buffer, zBuffer, width, hight);
		}
	}
}

void Geometry::resetBounds()
{

	if (m_polygons.empty()) {
		m_bBox = BBox();
		return;
	}
	m_bBox = BBox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	for (auto poly : m_polygons) {
		m_bBox.updateBBox(poly->getBbox());
	}
	
}

void Geometry::loadLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC& wireColor, const ColorGC& normalColor, RenderMode& renderMode) const
{
	const BBox unit = BBox::unitBBox();
	const ColorGC* wfClrOverride = renderMode.getRenderOverrideWireColor() ? &wireColor : &this->m_objColor;
	const ColorGC* nrmClrOverride = renderMode.getRenderOverrideNormalColor() ? &normalColor : &this->m_objColor;
	if (renderMode.getRenderObjBbox()) {
		std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wfClrOverride);
		lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());
	}
	for (const auto& p : m_polygons) {
		if (BBox::bboxCollide(p->getBbox(), unit)) {
			if (p->getToDraw())
			{
				p->loadLines(lines, wfClrOverride, nrmClrOverride, renderMode);
			}
		}
	}
}

void Geometry::clip() {
	for (PolygonGC* temp : m_polygons)
		temp->clip();
	this->resetBounds();
}
bool Geometry::isClippedByBBox(const Matrix4& tMat) const {
	return !BBox::bboxCollide(getBBox().transformBBox(tMat), BBox::unitBBox());
}
void Geometry::print() const
{
	int i = 0;
	for (const PolygonGC* temp : m_polygons)
	{
		std::cout << "		Polygon[" << i << "] vertices:" << std::endl;
		temp->printVertices();
		i++;
	}
}
