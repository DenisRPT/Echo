#include <game/Sonar.h>
#include <algorithm>

bool Sonar::emit(const glm::vec2& origin) {
	if (m_cooldown > 0.0f || m_pulses.size() >= kMaxPulses) {
		return false;
	}

	m_pulses.push_back({ origin,0.0f,1.0f });
	m_cooldown = kCooldown;
	return true;
}

void Sonar::update(float dt) {
	for (SonarPulse& p : m_pulses) {
		p.age += dt;
	}
	m_pulses.erase(
		std::remove_if(m_pulses.begin(), m_pulses.end(), [](const SonarPulse& p) {				return p.age >= kLifeTime;
			}), m_pulses.end());
	m_cooldown = std::max(0.0f, m_cooldown - dt);
}

void Sonar::clear() {
	m_pulses.clear();
	m_cooldown = 0.0f;
}