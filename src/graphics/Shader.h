#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator = (Shader&& other) noexcept;

	void bind() const;

	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;

	unsigned int id() const { return m_id; }

private:
	int uniformLocation(const std::string& name) const;

	static std::string readFile(const std::string& path);
	static unsigned int compile(unsigned int type, const std::string& source, const std::string& debugName);

	unsigned int m_id = 0;
	std::string m_name;
	mutable std::unordered_map<std::string, int> m_uniformCache;
};