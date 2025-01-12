#include "shader.h"
#include "Polygon.h"
Vector3 reflect(const Vector3& dir, const Vector3& normal)
{
	float dotProduct = Vector3::dot(dir, normal);
	return dir - (normal* 2 * dotProduct);
}
float positivesOnly(float x) { return std::max((float)(0.0),x); }

ColorGC Shader::calcLightColorAtPos(const Vector3& pos, const Vector3& normal, ColorGC colorBeforeLight) const
{
	ColorGC lightColor=m_ambiantColor* m_ambiantIntensity;
	for (int i = 0; i < m_lights; i++)
	{
		Vector3 lightdir;
		switch (m_lightSources[i].m_lightType) {
		case LightSourceType::LightSourceType_DIRECTIONAL:
			lightdir = m_lightSources[i].m_light_Dir.normalized();
			break;
		case LightSourceType::LightSourceType_POINT:
			lightdir = (pos - m_lightSources[i].m_light_Pos).normalized();
			break;
		default:
			throw;
		}
		Vector3 R = reflect(lightdir, -normal);
		Vector3 V = (m_viewPos - pos).normalized();
		double Ks = m_lightSources[i].m_specullarCo;
		double Kd = m_lightSources[i].m_diffuseCo;
		double Ip = m_lightSources[i].m_lightIntencity;
		float lightIntencity = Ip * (Kd * positivesOnly(Vector3::dot(lightdir, -normal)) + Ks * std::pow(positivesOnly(Vector3::dot(R, V)), m_specularityExp));
		ColorGC lightSourceColor = m_lightSources[i].m_light_Color * lightIntencity;
		lightColor = ColorGC::mixTwoColors( lightColor ,lightSourceColor);
	}
	return ColorGC::mixTwoColors(lightColor , colorBeforeLight);
}

Shader::Shader() :
	m_ambiantColor(255, 255, 255),
	m_ambiantIntensity(0.15),
	m_specularityExp(20),
	m_viewPos(0, 0, -2),
	m_lights(0)
{
	addLightSource(LightSource(true, 1, 1, 0,
	    ColorGC(255, 255, 255),
	    Vector3(0, 0, -4),
	    Vector3(0,0, 0),
	    LightSourceType::LightSourceType_POINT));
}

void Shader::applyShading(uint32_t* dest, const gData* gBuffer, int width, int height, const RenderMode& rd) const{
	SHADE_MODE mode = rd.getRenderShadeFlag();
	if (mode == SHADE_MODE::NONE)
		return;
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++) if(gBuffer[(y * width) + x].polygon) {
			ColorGC tmp;
			switch (mode) {
			case SHADE_MODE::GOUROUD:
				tmp = gBuffer[(y * width) + x].pixColor;
				break;
			case SHADE_MODE::PHONG:
				tmp = calcLightColorAtPos(gBuffer[(y * width) + x].pixPos,gBuffer[(y * width) + x].pixNorm, gBuffer[(y * width) + x].polygon->getColor());
				break;
			case SHADE_MODE::SOLID:
				tmp = gBuffer[(y * width) + x].polygon->getSceneColor();
					break;
			}
			dest[(y * width) + x] = tmp.getARGB();
		}
	}
}
