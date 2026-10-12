#include "game/Player.h"
#include "core/Input.h"
#include "graphics/Renderer2D.h"
#include "graphics/Texture.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

namespace {
	constexpr float kAccel = 48.0f;
	constexpr float kDrag = 6.0f;
}

Player::Player(const glm::vec2& start) : m_position(start), m_prevPosition(start) {}

glm::vec2 Player::position(float alpha) const {
	return::glm::mix(m_prevPosition, m_position, alpha);
}

void Player::update(float dt, const Input& input) {
	glm::vec2 dir(0.0f);
	if (input.down(GLFW_KEY_W) || input.down(GLFW_KEY_UP)) dir.y += 1.0f;
	if (input.down(GLFW_KEY_S) || input.down(GLFW_KEY_DOWN)) dir.y -= 1.0f;
	if (input.down(GLFW_KEY_D) || input.down(GLFW_KEY_RIGHT)) dir.x += 1.0f;
	if (input.down(GLFW_KEY_A) || input.down(GLFW_KEY_LEFT)) dir.x -= 1.0f;

	if(glm::dot(dir,dir) > 0.0f)
		dir = glm::normalize(dir);

	m_prevPosition = m_position;
	m_velocity += dir * kAccel * dt;
	m_velocity *= std::exp(-kDrag * dt); 
	m_position += m_velocity * dt;
}

void Player::draw(Renderer2D& renderer, const Texture& glow, float alpha, float time) const {
	const glm::vec2 pos = position(alpha);
	const float speed = glm::length(m_velocity);

	//pulsating aura
	const float pulse = 0.85f + 0.15f * std::sin(time * 4.0f);
	renderer.drawQuad(pos, glm::vec2(4.0f * pulse), glm::vec4(0.2f, 0.8f, 1.0f, 0.30f), 0.0f, &glow);

	const float stretch = 1.0f + speed * 0.05f;
	const float angle = std::atan2(m_velocity.y, m_velocity.x);
	renderer.drawQuad(pos, glm::vec2(0.9f * stretch, 0.9f / stretch), glm::vec4(0.8f, 1.0f, 1.0f, 1.0f), angle, &glow);

}