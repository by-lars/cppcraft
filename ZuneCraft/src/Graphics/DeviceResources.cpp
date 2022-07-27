#include "Core/Base.h"
#include "Graphics/DeviceResources.h"

namespace ZuneCraft {
	uint32_t BufferElement::GetSizeInBytes() const {
		switch (Type) {
		case DataType::FLOAT: 
			return Count * sizeof(float); 
			break;
		case DataType::INT: 
			return Count * sizeof(int); 
			break;
		case DataType::UNSIGNED_BYTE: 
			return Count * sizeof(uint8_t); 
			break;
		}

		ZC_FATAL_ERROR("Unknown Buffer Element Type");
	}

	bool BufferElement::IsIntegerType() const {
		switch (Type) {
		case DataType::INT: 
			return true; 
			break;
		case DataType::FLOAT: 
			return false; 
			break;
		case DataType::UNSIGNED_BYTE: 
			return false; 
			break;
		}

		ZC_FATAL_ERROR("Unknown Buffer Element Type");
	}
}