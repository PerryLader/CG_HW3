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
	ColorGC lightColor = m_ambient.getColor() * m_ambient.Ipower;
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++) if (m_lightSources[id].enabled)
	{
		Vector3 lightdir;
		switch (m_lightSources[id].type) {
		case LightSourceType::LightSourceType_DIRECTIONAL:
			lightdir = m_lightSources[id].getDir().normalized();
			break;
		case LightSourceType::LightSourceType_POINT:
			lightdir = (pos-m_lightSources[id].getPos()).normalized();
			break;
		default:
			throw;
		}
		Vector3 R = reflect(lightdir, -normal);
		Vector3 V = (m_viewPos - pos).normalized();
		double Ks = m_lightSources[id].Kspec;
		double Kd = m_lightSources[id].Kdiff;
		double Ip = m_lightSources[id].Ipower;
		float lightIntencity = Ip * (Kd * positivesOnly(Vector3::dot(lightdir, -normal)) + Ks * std::pow(positivesOnly(Vector3::dot(R, V)), m_specularityExp));
		ColorGC lightSourceColor = m_lightSources[id].getColor() * lightIntencity;
		lightColor = ColorGC::mixTwoColors( lightColor ,lightSourceColor);
	}
	return ColorGC::mixTwoColors(lightColor , colorBeforeLight);
}

Shader::Shader() {
	m_ambient.Ipower = 0.15;
	m_specularityExp = 2;
}

void Shader::updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp){
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
		m_lightSources[id] = lights[id];
		m_ambient = ambient;
		m_specularityExp = sceneSpecExp;
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
