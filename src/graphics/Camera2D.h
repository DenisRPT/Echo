#pragma once
#include <glm/glm.hpp>

class Camera2D {
public:
	//viewheight = the height of camera view in wu
	explicit Camera2D(float viewHeight = 20.0f);

	void update(float dt, const glm::vec2& target); //follows tarhet , updates shake

	void snapTo(const glm::vec2& position); //teleport without interpolation
	void addTrauma(float amount); //screen shake
	void setZoom(float zoom);

	glm::mat4 viewProjection(float aspect, float alpha) const; //next frame for interpolation

private:
	glm::vec2 m_position{ 0.0f };
	glm::vec2 m_prevPosition{ 0.0f };
	glm::vec2 m_shake{ 0.0f };
	glm::vec2 m_prevShake{ 0.0f };

	float m_viewHeight;
	float m_zoom = 1.0f;
	float m_trauma = 0.0f;
	float m_time = 0.0f;
	float m_sharpness = 6.0f; //how tight the camera follows the target
};