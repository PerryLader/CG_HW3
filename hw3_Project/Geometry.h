#ifndef	GEOMETRY_H
#define	GEOMETRY_H
#include "Polygon.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "Camera.h"

struct VectorKeyHash {
	std::size_t operator()(const Vector3 key) const {

		std::hash<float> hasher;
		return hasher(key.x) ^ (hasher(key.y) << 1) ^ (hasher(key.z) << 2);
	}
};
struct VectorKeyEqual {
	bool operator()(const Vector3& lhs,
		const Vector3& rhs) const {
		return lhs == rhs;
	}
};

class Geometry
{
private:
	std::vector<PolygonGC*> m_polygons;
	std::string m_name;
	BBox m_bBox;
	ColorGC m_objColor;

	void createObjBboxLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wireColor) const;

public:
	std::unordered_map<Vector3, std::shared_ptr<Vertex>, VectorKeyHash, VectorKeyEqual> m_map;

	//CONTRUCTOR
	Geometry(const std::string& name, const ColorGC& color);
	~Geometry();


	//GETTERS AND SETTERS
	std::string getName() const;
	BBox getBBox() const;


	//UTILS
	void draw(uint32_t* buffer, float* zBuffer, int width, int hight)const;
	void resetBounds();		
	void loadLines(std::vector<Line> lines[LineVectorIndex::LAST],const ColorGC& bBoxColor, const ColorGC& normalColor, RenderMode& renderMode,
		std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const;
	void Geometry::addPolygon(PolygonGC* poli);
	Geometry* applyTransformation(const Matrix4& tMat, bool flipNormals) const;
	void calcVertxNormal();
	void backFaceCulling(const Matrix4& invViewMatrix);
	void clip();
	bool isClippedByBBox(const Matrix4& tMat) const;
	void print() const;
	
};

#endif