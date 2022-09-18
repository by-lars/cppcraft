#pragma once
#include "Core/Base.h"
#include "World/Material.h"
#include "Data/Handle.h"
#include <glm/glm.hpp>

namespace ZuneCraft {
	class Chunk {

	public:
		const static int WIDTH = 32;
		const static int HEIGHT = 128;
		const static int VOLUME_SIZE = WIDTH * WIDTH * HEIGHT;
		const static int AREA_SIZE = WIDTH * WIDTH;

		Chunk();
		~Chunk();

		const glm::ivec2& GetIndex() const;
		void SetIndex(const glm::ivec2& index);

		const glm::vec3& GetWorldPosition() const;
		const glm::vec3 GetWorldPositionCentered() const;

		void Load(const glm::ivec2& index);
		void Unload();
		void Update();

	private:
		void GenTerrain();
		void GenMeshAndUpload();

		uint8_t m_Voxels[WIDTH][HEIGHT][WIDTH];
		Id m_MeshHandle;

		glm::ivec2 m_Index;
		glm::vec3 m_WorldPostion;
	};
}
