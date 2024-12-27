#ifndef	MODEL_H
#define	MODEL_H

#include "Geometry.h"
//#include "Renderer.h"
#include <vector>

class Model
{
protected:
	Model(Geometry* T) : T(T) , mTransform(Matrix4::identity()){};
	void virtual draw(/*Renderer& r*/) = 0;
	Geometry* T;
	Matrix4 mTransform;
public:
	virtual ~Model() { delete T; T = nullptr; };

	Geometry* onDraw(const Matrix4 viewProjectionMatrix)
	{
		const Matrix4 fTransform = viewProjectionMatrix*mTransform ;
		return (!T->isClippedByBBox(fTransform)) ? T->applyTransformation(fTransform) : nullptr;
	}
	
	void modifiyTransformation(const Matrix4& tMat){
		mTransform = tMat*mTransform;
	}

	BBox getGeormetryBbox()
	{
		return this->T->getBBox();
		
	}
	void virtual print() = 0;
};

#endif