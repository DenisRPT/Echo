#pragma once
#include <array>

class Window;

//
class Input {
public:
	static constexpr int kKeyCount = 349; // GLFW_KEY_LAST is 348, so + 1

	void update(const Window& window);

	bool down(int key) const;
	bool pressed(int key) const;
	bool released(int key) const;

private:
	static bool valid(int key) { return key >= 0 && key < kKeyCount; }

	std::array<bool, kKeyCount> m_current;
	std::array<bool, kKeyCount> m_previous;
};