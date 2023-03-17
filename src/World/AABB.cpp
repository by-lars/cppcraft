#include "World/AABB.h"
#include <algorithm>

namespace ZuneCraft {
	int sign(float a) { return a >= 0 ? 1 : -1; }

	float SweptAABB(CollisionBox box1, CollisionBox box2, glm::vec3* out_normal) {
		glm::vec3 invEntry;
		glm::vec3 invExit;

		if (box1.Vel.x > 0.0f) {
			invEntry.x = box2.Pos.x - (box1.Pos.x + box1.Size.x);
			invExit.x = (box2.Pos.x + box2.Size.x) - box1.Pos.x;
		}
		else {
			invEntry.x = (box2.Pos.x + box2.Size.x) - box1.Pos.x;
			invExit.x = box2.Pos.x - (box1.Pos.x + box1.Size.x);
		}

		if (box1.Vel.y > 0.0f) {
			invEntry.y = box2.Pos.y - (box1.Pos.y + box1.Size.y);
			invExit.y = (box2.Pos.y + box2.Size.y) - box1.Pos.y;
		}
		else {
			invEntry.y = (box2.Pos.y + box2.Size.y) - box1.Pos.y;
			invExit.y = box2.Pos.y - (box1.Pos.y + box1.Size.y);
		}

		if (box1.Vel.z > 0.0f) {
			invEntry.z = box2.Pos.z - (box1.Pos.z + box1.Size.z);
			invExit.z = (box2.Pos.z + box2.Size.z) - box1.Pos.z;
		}
		else {
			invEntry.z = (box2.Pos.z + box2.Size.z) - box1.Pos.z;
			invExit.z = box2.Pos.z - (box1.Pos.z + box1.Size.z);
		}

		glm::vec3 entry = invEntry / box1.Vel;
		glm::vec3 exit = invExit / box1.Vel;

		float entryTime = std::max({ entry.x, entry.y, entry.z });
		float exitTime = std::max({exit.x, exit.y, exit.z});

		//No Collision
		if (entryTime > exitTime || entry.x < 0.0f && entry.y < 0.0f && entry.z < 0.0f || entry.x > 1.0f || entry.y > 1.0f || entry.z > 1.0f) {
			*out_normal = glm::vec3(0);
			return 1.0f;
		}
		else { //There was a collision
			if (entry.x > entry.y) {
				if (entry.x > entry.z) {
					out_normal->x = -sign(box1.Vel.x);
					out_normal->y = 0.0f;
					out_normal->z = 0.0f;
				}
				else {
					out_normal->z = -sign(box1.Vel.z);
					out_normal->x = 0.0f;
					out_normal->y = 0.0f;
				}
			}
			else {
				if (entry.y > entry.z) {
					out_normal->y = -sign(box1.Vel.y);
					out_normal->x = 0.0f;
					out_normal->z = 0.0f;

				}
				else {
					out_normal->z = -sign(box1.Vel.z);
					out_normal->x = 0.0f;
					out_normal->y = 0.0f;
				}
			}
		}

		return entryTime;
	}
}