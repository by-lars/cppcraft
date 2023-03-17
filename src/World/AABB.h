#pragma once
#include <glm/glm.hpp>

namespace ZuneCraft {
	struct CollisionBox {
		glm::vec3 Pos; //Top-Left Corner of the Box
		glm::vec3 Size;
		glm::vec3 Vel;
	};


	float SweptAABB(CollisionBox box1, CollisionBox box2, glm::vec3* out_normal);
}