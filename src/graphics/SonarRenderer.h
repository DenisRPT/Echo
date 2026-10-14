#pragma once
#include "graphics/Shader.h"
#include <glm/glm.hpp>

class Sonar;
class Texture;

//draws illuminated world , in a single pass
class SonarRenderer {
public:
	SonarRenderer();
	~SonarRenderer();

	SonarRenderer(const SonarRenderer&) = delete;
	SonarRenderer& operator = (const SonarRenderer&) = delete;

	void draw(const Sonar& sonar, const Texture& mapTexture, const glm::mat4& viewProjection, float timeAhead);

private:
	Shader m_shader;
	unsigned int m_vao = 0;
};