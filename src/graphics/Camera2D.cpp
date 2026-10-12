#include "graphics/Camera2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace {
	constexpr float kMaxShake = 1.0f;
	constexpr float kTraumaDecay = 1.5f; // per second
}

Camera2D::Camera2D(float viewHeight) : m_viewHeight(viewHeight) {}

void Camera2D::update(float dt, const glm::vec2& target) {
	m_prevPosition = m_position;
	m_prevShake = m_shake;

	//interpolation independent of framerate
	const float t = 1.0f - std::exp(-m_sharpness * dt);
	m_position += (target - m_position) * t;

	//screen shake
	m_time += dt;
	m_trauma = std::max(0.0f, m_trauma - kTraumaDecay * dt);
	const float s = m_trauma * m_trauma * kMaxShake / 1.5f;
	m_shake = glm::vec2(
		std::sin(m_time * 47.0f) + 0.5f * std::sin(m_time * 91.0f + 1.3f),
		std::sin(m_time * 53.0f + 2.1f) + 0.5f * std::sin(m_time * 83.0f + 4.7f)) * s;
}

void Camera2D::snapTo(const glm::vec2& position) {
	m_position = m_prevPosition = position;
}

void Camera2D::addTrauma(float amount) {
	m_trauma = std::min(1.0f, m_trauma + amount);
}

void Camera2D::setZoom(float zoom) {
	m_zoom = std::max(0.1f, zoom);
}

glm::mat4 Camera2D::viewProjection(float aspect, float alpha) const {
	const glm::vec2 pos = glm::mix(m_prevPosition, m_position, alpha)
		+ glm::mix(m_prevShake, m_shake, alpha);
	const float halfH = 0.5f * m_viewHeight / m_zoom;
	const float halfW = halfH * aspect;
	return glm::ortho(pos.x - halfW, pos.x + halfW, pos.y - halfH, pos.y + halfH);
}