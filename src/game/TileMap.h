#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

class Renderer2D;

// Game world - 1x1 tile grid
class TileMap {
public:
	TileMap(int width, int height, std::uint32_t seed);

	void generate(std::uint32_t seed);

	int width() const { return m_width; }
	int height() const { return m_height; }

	bool isSolid(int x, int y) const;

	std::vector<unsigned char> toRGBA() const;

	const glm::vec2& spawnPoint() const { return m_spawn; }
	std::uint32_t seed() const { return m_seed; }

	void draw(Renderer2D& renderer, const glm::vec2& center, float radius) const;

private:
	enum Tile : std::uint8_t {
		Floor = 0,
		Wall = 1
	};

	int index(int x, int y) const { return y * m_width + x; }
	void smooth();
	int keepLargestRegion();

	int m_width;
	int m_height;
	std::uint32_t m_seed = 0;
	glm::vec2 m_spawn{ 0.0f };
	std::vector<Tile> m_tiles;

};