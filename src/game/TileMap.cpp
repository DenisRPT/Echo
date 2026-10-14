#include "game/TileMap.h"
#include "graphics/Renderer2D.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

TileMap::TileMap(int width, int height, std::uint32_t seed)
	: m_width(width), m_height(height) {
	if (width < 8 || height < 8) {
		throw std::invalid_argument("TileMap dimensions must be at least 8x8");
	}
	generate(seed);
}

bool TileMap::isSolid(int x, int y) const {
	if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
		return true;
	}
	return m_tiles[static_cast<std::size_t>(index(x, y))] == Wall;
}

void TileMap::generate(std::uint32_t seed) {
	constexpr int kMaxAttempts = 50;
	constexpr float kWallChance = 0.45f;
	constexpr int kSmoothSteps = 5;
	const int minFloor = m_width * m_height / 4; // At least 25% of the map must be floor tiles

	m_tiles.assign(static_cast<std::size_t>(m_width) * m_height, Wall);

	int largest = 0;
	for (int attempt = 0; attempt < kMaxAttempts; ++attempt) {
		m_seed = seed + static_cast<std::uint32_t>(attempt);
		std::mt19937 rng(m_seed);
		std::uniform_real_distribution<float> chance(0.0f, 1.0f);

		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; ++x) {
				const bool border = (x == 0 || y == 0 || x == m_width - 1 || y == m_height - 1);
				const bool wall = border || chance(rng) < kWallChance;
				m_tiles[static_cast<std::size_t>(index(x, y))] = wall ? Wall : Floor;	
			}
		}

		for(int i=0; i < kSmoothSteps; ++i) {
			smooth();
		}

		largest = keepLargestRegion();
		if(largest >= minFloor) {
			return;
		}
	}

	// If we reach here, we failed to generate a valid map after kMaxAttempts , so we will create a simple open area in the center of the map
	if (largest == 0) {
		const int cx = m_width / 2;
		const int cy = m_height / 2;
		for(int dy = -2; dy <= 2; ++dy) {
			for(int dx = -2; dx <= 2; ++dx) {
				m_tiles[static_cast<std::size_t>(index(cx + dx, cy + dy))] = Floor;
			}
		}
		m_spawn = glm::vec2(cx + 0.5f, cy + 0.5f);
	}
}

void TileMap::smooth() {
	std::vector<Tile> next = m_tiles;
	for (int y = 1; y < m_height - 1; ++y) {
		for (int x = 1; x < m_width - 1; ++x) {
			int walls = 0;
			for (int dy = -1; dy <= 1; ++dy) {
				for (int dx = -1; dx <= 1; ++dx) {
					if((dx!=0 || dy != 0) && isSolid(x + dx, y + dy)) {
						++walls;
					}
				}
			}
			if (walls > 4) next[static_cast<std::size_t>(index(x, y))] = Wall;
			else if (walls < 4) next[static_cast<std::size_t>(index(x, y))] = Floor;
		}
	}
	m_tiles.swap(next);
}

int TileMap::keepLargestRegion() {
	const int total = m_width * m_height;
	std::vector<int> label(static_cast<std::size_t>(total), -1);
	std::vector<int> sizes;
	std::vector<int> stack;

	for (int start = 0; start < total; ++start) {
		if (m_tiles[static_cast<std::size_t>(start)] != Floor || label[static_cast<std::size_t>(start)] != -1) {
			continue;
		}

		const int id = static_cast<int>(sizes.size());
		sizes.push_back(0);
		label[static_cast<std::size_t>(start)] = id;
		stack.push_back(start);

		while (!stack.empty()) {
			const int cur = stack.back();
			stack.pop_back();
			++sizes[static_cast<std::size_t>(id)];

			const int x = cur % m_width;
			const int y = cur / m_width;
			const int nx[4] = { x - 1, x + 1, x, x };
			const int ny[4] = { y, y, y - 1, y + 1 };
			for (int k = 0; k < 4; ++k) {
				if (nx[k] < 0 || ny[k] < 0 || nx[k] >= m_width || ny[k] >= m_height) continue;
				const int ni = index(nx[k], ny[k]);
				if (m_tiles[static_cast<std::size_t>(ni)] == Floor && label[static_cast<std::size_t>(ni)] == -1) {
					label[static_cast<std::size_t>(ni)] = id;
					stack.push_back(ni);
				}
			}
		}
	}

	if(sizes.empty()) {
		return 0;
	}

	const int best = static_cast<int>(std::max_element(sizes.begin(), sizes.end()) - sizes.begin());
	const glm::vec2 mapCenter(m_width * 0.5f, m_height * 0.5f);
	float bestDist = std::numeric_limits<float>::max();

	for (int i = 0; i < total; ++i) {
		if (m_tiles[static_cast<std::size_t>(i)] != Floor) continue;
		if(label[static_cast<std::size_t>(i)] != best) {
			m_tiles[static_cast<std::size_t>(i)] = Wall;
			continue;
		}
		const glm::vec2 c(i % m_width + 0.5f, i / m_width + 0.5f);
		const glm::vec2 d = c - mapCenter;
		const float dist2 = glm::dot(d, d);
		if (dist2 < bestDist) {
			bestDist = dist2;
			m_spawn = c;
		}
	}
	return sizes[static_cast<std::size_t>(best)];
}

void TileMap::draw(Renderer2D& renderer, const glm::vec2& center, float radius) const {
	const int minX = std::max(0, static_cast<int>(std::floor(center.x - radius)));
	const int maxX = std::min(m_width - 1, static_cast<int>(std::floor(center.x + radius)));
	const int minY = std::max(0, static_cast<int>(std::floor(center.y - radius)));
	const int maxY = std::min(m_height - 1, static_cast<int>(std::floor(center.y + radius)));

	//draw only the the close tiles to the player
	for (int y = minY; y < maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			const glm::vec2 c(x + 0.5f, y + 0.5f);
			const float d = glm::length(c - center);
			if (d >= radius) continue;

			float f = 1.0f - d / radius;
			f *= f;
			
			if (isSolid(x, y)) {
				renderer.drawQuad(c, glm::vec2(0.94f), glm::vec4(0.35f, 0.55f, 1.0f, f));
			}
			else {
				renderer.drawQuad(c, glm::vec2(1.0f), glm::vec4(0.05f, 0.12f, 0.25f, f * 0.6f));
			}
		}
	}
}