#pragma once

#ifdef ZC_PLATFORM_ZUNE
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include <glm/glm.hpp>

namespace glm {
	inline std::size_t hash_value(const glm::ivec2& v) {
		boost::hash<int> hash1;
		boost::hash<int> hash2;
		return hash1(v.x) ^ hash2(v.y);
	 }
}