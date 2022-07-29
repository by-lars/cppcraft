#pragma once
#include "Core/Base.h"
#include "Geometry/VoxelStorage.h"
#include "Geometry/Material.h"

#include <glm/glm.hpp>

namespace ZuneCraft {
	class Chunk {
	public:
		Chunk(const glm::ivec2& offset);
		~Chunk();

		const glm::ivec2& GetIndex() const;
		void SetIndex(const glm::ivec2& index);

		const glm::vec3& GetWorldPosition() const;
		const glm::vec3 GetWorldPositionCentered() const;

		void GenTerrain();
		
		uint8_t TryGet(int x, int y, int z);
		void TrySet(int x, int y, int z, BlockType type);

		bool IsVoidAt(int x, int y, int z);

		void Update();
	private:
		VoxelStorage m_Voxels;

		glm::ivec2 m_Index;
		glm::vec3 m_WorldPostion;
	};
}
