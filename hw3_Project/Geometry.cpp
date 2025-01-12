#include "Geometry.h"

Geometry::Geometry(const std::string& name, const ColorGC& color) : m_name(name) ,m_bBox(), m_objColor(color), hasVertDataNormal(true) , hasPolyDataNormal(true){}

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

ColorGC Geometry::getColor() const
{
	return this->m_objColor;
}

void Geometry::setColor(ColorGC newColor)
{
	this->m_objColor = newColor;
}

std::string Geometry::getName() const{
	return this->m_name;
}

void Geometry::addPolygon(PolygonGC* poli)
{
	this->m_polygons.push_back(poli);
	if (!poli->hasDataNormalLine())
		hasPolyDataNormal = false;
	if(!poli->hasVertsDataNormalLine())
		hasVertDataNormal = false;
	m_bBox.updateBBox(poli->getBbox());
}

Geometry* Geometry::applyTransformation(const Matrix4& tMat, bool flipNormals) const{
	
	Geometry* res = new Geometry(m_name,this->m_objColor);
	for (const auto& poly : m_polygons) {
		if (!poly->isClippedByBBox(tMat)) {
			res->addPolygon(poly->applyTransformation(tMat, flipNormals));
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

void Geometry::backFaceCulling(const Vector3 camera_pos) {
	for (auto& poly : m_polygons)
	{
		Line poly_normal = poly->getCalcNormalLine(nullptr);
		if (Vector3::dot((poly_normal.m_a - camera_pos).normalized(), poly_normal.direction().normalized()) < 0)
		{
			poly->setVisibility(false);
		}
	}
}

void Geometry::createObjBboxLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wireColor) const
{
	std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wireColor);
	lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());	
}

void Geometry::fillGbuffer(gData* gBuffer, int width, int height , RenderMode& rm) const
{
	for (const auto& poly : m_polygons) if (!rm.getRenderCulledFlag() ||rm.getRenderCulledFlag() && poly->isVisible())
	{
		if (rm.getVertexUseDNormalFlag() && !hasVertDataNormal) {
			rm.setVertexUseDNormalFlag();
			rm.setVertexUseCNormalFlag();
		}
		if (rm.getPolygonsUseDNormalFlag() && !hasPolyDataNormal) {
				//message
			rm.setPolygonsUseDNormalFlag();
			rm.setPolygonsUseCNormalFlag();
		}
		poly->fillGbuffer(gBuffer, width, height, rm);
	}
}

void Geometry::resetBounds()
{
	if (m_polygons.empty()) {
		m_bBox = BBox();
		return;
	}
	m_bBox = BBox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	hasPolyDataNormal = true;
	hasVertDataNormal = true;
	for (auto poly : m_polygons) {
		m_bBox.updateBBox(poly->getBbox());
		if (!poly->hasDataNormalLine())
			hasPolyDataNormal = false;
		if (!poly->hasVertsDataNormalLine())
			hasVertDataNormal = false;
	}
	
}

void Geometry::loadLines(std::vector<Line> lines[LineVectorIndex::LAST],
	RenderMode& renderMode, std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const
{
	const BBox unit = BBox::unitBBox();
	const ColorGC* wfClrOverride = renderMode.getRenderOverrideWireColorFlag() ? &renderMode.getWireColor() : &this->m_objColor;
	if (renderMode.getRenderObjBboxFlag()) {
		std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wfClrOverride);
		lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());
	}
	for (const auto& p : m_polygons) {
		if (BBox::bboxCollide(p->getBbox(), unit)) {
			
			p->loadLines(lines, renderMode, SilhoutteMap);			
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

void Geometry::fillBasicSceneColors(const Shader& shader, RenderMode& rm)
{
		for (auto& poly : m_polygons)
		{
			if (rm.getVertexUseDNormalFlag() && !hasVertDataNormal) {
				//message
				rm.setVertexUseDNormalFlag();
				rm.setVertexUseCNormalFlag();
			}
			if (rm.getPolygonsUseDNormalFlag() && !hasPolyDataNormal) {
				//message
				rm.setPolygonsUseDNormalFlag();
				rm.setPolygonsUseCNormalFlag();
			}
			poly->fillBasicSceneColors(shader,rm);
		}
}
