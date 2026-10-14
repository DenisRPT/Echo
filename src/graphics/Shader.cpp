#include "graphics/Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

std::string Shader::readFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if(!file){
		throw std::runtime_error("Failed to open shader file: " + path);
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

unsigned int Shader::compile(unsigned int type, const std::string& source, const std::string& debugName) {
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint ok = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		std::string log(len > 0 ? static_cast<size_t>(len) : 1u, '\0');
		glGetShaderInfoLog(shader, len, nullptr, log.data());
		glDeleteShader(shader);
		throw std::runtime_error("Failed to compile shader (" + debugName + "): " + log);
	}
	return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: m_name(vertexPath + " + " + fragmentPath) {
	const std::string vsSource = readFile(vertexPath);
	const std::string fsSource = readFile(fragmentPath);

	GLuint vs = compile(GL_VERTEX_SHADER, vsSource, vertexPath);
	GLuint fs = 0;
	try {
		fs = compile(GL_FRAGMENT_SHADER, fsSource, fragmentPath);
	} catch (...) {
		glDeleteShader(vs);
		throw;
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, vs);
	glAttachShader(m_id, fs);
	glLinkProgram(m_id);

	glDetachShader(m_id, vs);
	glDetachShader(m_id, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	GLint ok = 0;
	glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
	if (!ok) {
		GLint len = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
		std::string log(len > 0 ? static_cast<size_t>(len) : 1u, '\0');
		glGetProgramInfoLog(m_id, static_cast<GLsizei>(log.size()), nullptr, log.data());
		glDeleteProgram(m_id);
		m_id = 0;
		throw std::runtime_error("Failed to link shader program (" + m_name + "): " + log);
	}
}

Shader::~Shader() {
	if (m_id) {
		glDeleteProgram(m_id);
		m_id = 0;
	}
}

Shader::Shader(Shader&& other) noexcept
	: m_id(other.m_id), m_name(std::move(other.m_name)), m_uniformCache(std::move(other.m_uniformCache)) {
	other.m_id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		if (m_id)
			glDeleteProgram(m_id);
		m_id = std::exchange(other.m_id, 0);
		m_name = std::move(other.m_name);
		m_uniformCache = std::move(other.m_uniformCache);
	}
	return *this;
}

void Shader::bind() const {
	glUseProgram(m_id);
}

int Shader::uniformLocation(const std::string& name) const {
	auto it = m_uniformCache.find(name);
	if (it != m_uniformCache.end())
		return it->second;
	const int location = glGetUniformLocation(m_id, name.c_str());
	if (location == -1) {
		std::cerr << "Warning: uniform '" << name << "' not found in shader program (" << m_name << ")\n";
	}
	m_uniformCache[name] = location;
	return location;
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(uniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(uniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec4v(const std::string& name, int count, const glm::vec4* values) const {
	glUniform4fv(uniformLocation(name), count, glm::value_ptr(values[0]));
}
