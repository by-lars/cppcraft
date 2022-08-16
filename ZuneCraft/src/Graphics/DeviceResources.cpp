#include "Core/Base.h"
#include "Graphics/DeviceResources.h"

namespace ZuneCraft {
	FramebufferAttachement::FramebufferAttachement(TextureFormat format, AttachementType type, bool writeOnly) {
		Format = format;
		Type = type;
		WriteOnly = writeOnly;
	}
}