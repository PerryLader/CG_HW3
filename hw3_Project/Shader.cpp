#include "shader.h"

Vector3 reflect(const Vector3& dir, const Vector3& normal)
{
	float dotProduct = Vector3::dot(dir, normal);
	return dir - (normal* 2 * dotProduct);
}
ColorGC Shader::calcLightColor(const Vector3& pos, const Vector3& normal, ColorGC colorBeforeLight) const
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
						lightSource.m_light_Dir.normalized(),
						normal), (float)0.0)))
					+
/*specullar*/	(lightSource.m_specullarCo *
					(std::pow(std::max(Vector3::dot(
/*R*/					reflect(lightSource.m_light_Dir.normalized(), -normal),
/*V*/					(m_viewPos - pos).normalized()),(float)0),this->m_specularityExp))));	
		}
		else if (lightSource.m_lightType == LightSourceType::LightSourceType_POINT)
		{
			lightIntencity = lightSource.m_lightIntencity *
/*diffuse*/		((lightSource.m_diffuseCo *
					(std::max(Vector3::dot(
						(pos - lightSource.m_light_Pos).normalized(),
						normal), (float)0.0)))
					+
/*specullar*/	(lightSource.m_specullarCo *
						(std::pow(std::max(Vector3::dot(
/*R*/					reflect((pos - lightSource.m_light_Pos).normalized(), -normal),
/*V*/					(m_viewPos - pos).normalized()), (float)0.0),this->m_specularityExp))));
		}
		else
		{
			throw;
		}
		ColorGC lightSourceColor = lightSource.m_light_Color*lightIntencity;
		lightColor = ColorGC::mixTwoColors( lightColor ,lightSourceColor);
	
	}
	return ColorGC::mixTwoColors(lightColor,colorBeforeLight);
}

void Shader::addLightSource(const LightSource& newLightSource)
{
	m_lightSources.push_back(newLightSource);
}



Shader::Shader(float ambiantIntensity, ColorGC ambiantColor, float specularityExp, Vector3 viewPos):
	m_ambiantColor(ambiantColor),
	m_ambiantIntensity(ambiantIntensity),
	m_specularityExp(specularityExp),
	m_viewPos(viewPos)
{}

