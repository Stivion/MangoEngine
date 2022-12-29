#pragma once

namespace Mango
{
	class Renderer
	{
	public:
		virtual void Draw() = 0;
		virtual void HandleFramebuffersResized() = 0;
		virtual void* GetViewportTextureId() = 0; // TODO: It looks like a bad idea
		virtual ~Renderer() = default;
	};
}
