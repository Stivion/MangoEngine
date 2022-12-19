#pragma once

namespace Mango
{
	class Editor
	{
	public:
		Editor();
		Editor(const Editor&) = delete;
		Editor operator=(const Editor&) = delete;
		~Editor();

	private:
		void InitializeImGui();
	};
}
