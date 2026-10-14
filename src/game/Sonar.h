#pragma once
#include <glm/glm.hpp>
#include <cstddef>
#include <vector>

struct SonarPulse {
	glm::vec2 origin;
	float age;
	float strength;
};

class Sonar {
public:
	static constexpr std::size_t kMaxPulses = 8; // equal to uPulses dim

	static constexpr float kSpeed = 22.0f; // ups 
	static constexpr float kMaxRadius = 30.0f; 
	static constexpr float kTail = 2.0f; //after max radius , fade out
	static constexpr float kCooldown = 0.8f; 
	static constexpr float kLifeTime = kMaxRadius / kSpeed + kTail;

	bool emit(const glm::vec2& origin);

	void update(float dt);

	void clear();

	const std::vector<SonarPulse>& pulses() const { return m_pulses; }

private:
	std::vector<SonarPulse> m_pulses;
	float m_cooldown = 0.0f;
};