#pragma once

namespace Mango
{
	class Renderer
	{
	public:
		virtual void Draw() = 0;
		virtual void HandleFramebuffersResized() = 0;
		virtual ~Renderer() = default;
	};
}
