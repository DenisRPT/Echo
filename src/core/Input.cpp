#include "core/Input.h"
#include "core/Window.h"
#include "GLFW/glfw3.h"

static_assert(GLFW_KEY_LAST < Input::kKeyCount, "Input::kKeyCount is too small for GLFW_KEY_LAST");

void Input::update(const Window& window) {
	m_previous = m_current;
	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
		m_current[key] = window.keyDown(key);
	}
}

bool Input::down(int key) const {
	return valid(key) && m_current[key];
}

bool Input::pressed(int key) const {
	return valid(key) && m_current[key] && !m_previous[key];
}

bool Input::released(int key) const {
	return valid(key) && !m_current[key] && m_previous[key];
}

