#pragma once
#include <glm/glm.hpp>

class Input;
class Renderer2D;
class Texture;

class Player {
public:
	explicit Player(const glm::vec2& start = glm::vec2(0.0f));

	void update(float dt, const Input& input);
	void draw(Renderer2D& renderer, const Texture& glow, float alpha, float time) const;

	const glm::vec2& position() const { return m_position; }
	glm::vec2 position(float alpha) const;
	const glm::vec2& velocity() const { return m_velocity; }

private:
	glm::vec2 m_position;
	glm::vec2 m_prevPosition;
	glm::vec2 m_velocity{ 0.0f };
};