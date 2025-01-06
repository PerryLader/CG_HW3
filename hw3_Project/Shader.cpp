#include "shader.h"

Vector3 reflect(const Vector3& dir, const Vector3& normal)
{
	float dotProduct = Vector3::dot(dir, normal);
	return dir - (normal* 2 * dotProduct);
}
ColorGC Shader::calcLightColor(const Line& normalLine, ColorGC colorBeforeLight) const
{
	ColorGC lightColor=m_ambiantColor* m_ambiantIntensity;
	for (auto& lightSource : m_lightSources)
	{
		float lightIntencity;
		if (lightSource.m_lightType == LightSourceType::LightSourceType_DIRECTIONAL)
		{
			lightIntencity = lightSource.m_lightIntencity *
/*diffuse*/		((lightSource.m_diffuseCo *
						(std::max(Vector3::dot(
							lightSource.m_light_Dir,
							-normalLine.direction()), (float)0.0)))
					+
/*specullar*/	(lightSource.m_specullarCo *
					(std::pow(Vector3::dot(
/*R*/					reflect(lightSource.m_light_Dir, -normalLine.direction()),
/*V*/					(m_viewPos - normalLine.m_a)),this->m_specularityExp))));	
		}
		else if (lightSource.m_lightType == LightSourceType::LightSourceType_POINT)
		{
			lightIntencity = lightSource.m_lightIntencity *
/*diffuse*/		((lightSource.m_diffuseCo *
					(std::max(Vector3::dot(
						normalLine.m_a- lightSource.m_light_Pos,
						-normalLine.direction()), (float)0.0)))
					+
/*specullar*/	(lightSource.m_specullarCo *
						(std::pow(Vector3::dot(
/*R*/					reflect(normalLine.m_a - lightSource.m_light_Pos, -normalLine.direction()),
/*V*/					(m_viewPos - normalLine.m_a)), this->m_specularityExp))));
		}
		else
		{
			throw;
		}
		ColorGC lightSourceColor = lightSource.m_light_Color*lightIntencity;
		lightColor = lightColor * lightSourceColor;
	
	}
	return lightColor*colorBeforeLight;
}

void Shader::addLightSource(const LightSource& newLightSource)
{
	m_lightSources.push_back(newLightSource);
}

void Shader::fillVetrexesColor(Geometry* geometry) const
{
	for (auto& pair : geometry->m_map)
	{
		pair.second->setColor(this->calcLightColor(
			pair.second->getCalcNormalLine(), geometry->getColor()));
	}
}

Shader::Shader(float ambiantIntensity, ColorGC ambiantColor, float specularityExp, Vector3 viewPos):
	m_ambiantColor(ambiantColor),
	m_ambiantIntensity(ambiantIntensity),
	m_specularityExp(specularityExp),
	m_viewPos(viewPos)
{}

