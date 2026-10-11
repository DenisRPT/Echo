#include "graphics/Renderer2D.h"
#include <glad/glad.h>
#include <cmath>
#include <cstddef>

namespace {
	// A single white pixel used for rendering colored quads without a texture.
	const unsigned char kWhitePixel[4] = { 255, 255, 255, 255 };
}

Renderer2D::Renderer2D(std::size_t maxQuads)
	:m_shader("shaders/quad.vert", "shaders/quad.frag"), m_white(1, 1, kWhitePixel), m_maxQuads(maxQuads){
		m_vertices.reserve(m_maxQuads * 4);

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);

		glBindVertexArray(m_vao);

		//Dynamic vbo
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_maxQuads * 4 * sizeof(Vertex)), nullptr, GL_DYNAMIC_DRAW);

		std::vector<std::uint32_t> indices(m_maxQuads * 6);
		for (std::size_t i = 0; i < m_maxQuads; ++i) {
			const std::uint32_t base = static_cast<std::uint32_t>(i * 4);
			indices[i * 6 + 0] = base + 0;
			indices[i * 6 + 1] = base + 1;
			indices[i * 6 + 2] = base + 2;
			indices[i * 6 + 3] = base + 2;
			indices[i * 6 + 4] = base + 3;
			indices[i * 6 + 5] = base + 0;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)), indices.data(), GL_STATIC_DRAW);
		
		const GLsizei stride = sizeof(Vertex);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, position)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, color)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

}

Renderer2D::~Renderer2D() {
	// Delete GL resources created by this renderer.
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
	if (m_ebo) {
		glDeleteBuffers(1, &m_ebo);
		m_ebo = 0;
	}
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

void Renderer2D::begin(const glm::mat4& viewProjection) {
	m_viewProjection = viewProjection;
	m_stats = Stats{};
	m_vertices.clear();
	m_currentTexture = nullptr;
}

void Renderer2D::end() {
	flush();
}

void Renderer2D::drawQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec4& color, float rotation, const Texture* texture) {
	const Texture* tex = texture ? texture : &m_white;

	//full batch flush if texture changes
	if (m_vertices.size() / 4 >= m_maxQuads || (m_currentTexture && m_currentTexture != tex)) {
		flush();
	}

	m_currentTexture = tex;

	const glm::vec2 half = size * 0.5f;
	const glm::vec2 corners[4] = {
		{-half.x, -half.y},
		{ half.x, -half.y},
		{ half.x,  half.y},
		{-half.x,  half.y}
	};
	const glm::vec2 uvs[4] = { {0.f,0.f}, {1.f,0.f}, {1.f,1.f}, {0.f,1.f} };

	const float c = std::cos(rotation);
	const float s = std::sin(rotation);

	for(int i=0; i<4 ;++i){
		const glm::vec2 p = corners[i];
		const glm::vec2 rotated(p.x * c - p.y * s, p.x * s + p.y * c);
		m_vertices.push_back({ center + rotated, uvs[i], color });
	}
	++m_stats.quads;
}

void Renderer2D::flush() {
	if (m_vertices.empty()) return;
	const std::size_t quadCount = m_vertices.size() / 4;

	m_shader.bind();
	m_shader.setMat4("uViewProjection", m_viewProjection);
	m_shader.setInt("uTexture", 0);
	m_currentTexture->bind(0);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), m_vertices.data());
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(quadCount * 6), GL_UNSIGNED_INT, nullptr);
	++m_stats.drawCalls;
	m_vertices.clear();
}


	