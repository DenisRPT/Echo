#pragma once

class Texture {
public:
	enum class Filter {Nearest, Linear};

	Texture(int width, int height, const unsigned char* rgba, Filter filter = Filter::Linear);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator =(const Texture&) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator =(Texture&& other) noexcept;

	void bind(unsigned int slot = 0) const;

	int width() const { return m_width; }
	int height() const { return m_height; }

private:
	unsigned int m_id = 0;
	int m_width = 0;
	int m_height = 0;
};