#pragma once
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "glm/glm.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

class Renderer2D {
public:
	struct Stats {
		std::uint32_t drawCalls = 0;
		std::uint32_t quads = 0;
	};

	explicit  Renderer2D(std::size_t maxQuads = 10000);
    ~Renderer2D();

	Renderer2D(const Renderer2D&) = delete;
	Renderer2D& operator=(const Renderer2D&) = delete;

	void begin(const glm::mat4& viewProjection);
	void end();

	void drawQuad(const glm::vec2& center, const glm::vec2& size,
		          const glm::vec4& color, float rotation = 0.0f, const Texture* texture = nullptr);

	const Stats& stats() const { return m_stats; }

private:
	struct Vertex {
		glm::vec2 position;
		glm::vec2 uv;
		glm::vec4 color;
	};

	void flush();

	Shader m_shader;
	Texture m_white;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ebo = 0;

	std::size_t m_maxQuads;
	std::vector<Vertex> m_vertices;
	const Texture* m_currentTexture = nullptr;
	glm::mat4 m_viewProjection{ 1.0f };
	Stats m_stats;
};