#ifndef	GEOMETRY_H
#define	GEOMETRY_H
#include "Polygon.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "Camera.h"
#include "Shader.h"


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
	ColorGC getColor()const;
	void setColor(ColorGC newColor);


	//UTILS
	void fillGbuffer(gData* gBuffer, int width, int height) const;
	void resetBounds();
	void loadLines(std::vector<Line> lines[LineVectorIndex::LAST],const ColorGC& bBoxColor, const ColorGC& normalColor, RenderMode& renderMode,
		std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const;
	void addPolygon(PolygonGC* poli);
	Geometry* applyTransformation(const Matrix4& tMat, bool flipNormals) const;
	void calcVertxNormal();
	void backFaceCulling(const Matrix4& invViewMatrix);
	void clip();
	bool isClippedByBBox(const Matrix4& tMat) const;
	void print() const;
	void fillVetrexesColor(const Shader& shader);
	
	
};

#endif