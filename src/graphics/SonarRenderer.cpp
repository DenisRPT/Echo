#include "graphics/SonarRenderer.h"
#include <game/Sonar.h>
#include <graphics/Texture.h>
#include <glad/glad.h>
#include <algorithm>
#include <string>

SonarRenderer::SonarRenderer()
	: m_shader("shaders/sonar.vert", "shaders/sonar.frag") {
	glGenVertexArrays(1, &m_vao);
}

SonarRenderer::~SonarRenderer() {
	glDeleteVertexArrays(1, &m_vao);
}

void SonarRenderer::draw(const Sonar& sonar, const Texture& mapTexture, const glm::mat4& viewProjection, float timeAhead) {
	const auto& pulses = sonar.pulses();
	const int count = static_cast<int>(std::min(pulses.size(), Sonar::kMaxPulses));

	m_shader.bind();
	m_shader.setMat4("uInvViewProj", glm::inverse(viewProjection));
	m_shader.setFloat("uSpeed", Sonar::kSpeed);
	m_shader.setFloat("uMaxRadius", Sonar::kMaxRadius);
	m_shader.setFloat("uPulseCount", count);

	for (int i = 0; i < count; ++i) {
		const SonarPulse& p = pulses[static_cast<std::size_t>(i)];
		m_shader.setVec4("uPulses[" + std::to_string(i) + "]", glm::vec4(p.origin, p.age + timeAhead, p.strength));

	}

	mapTexture.bind(0);
	m_shader.setInt("uMap", 0);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}