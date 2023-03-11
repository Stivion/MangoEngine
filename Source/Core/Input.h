#pragma once

#include <glm/glm.hpp>

#include <set>

namespace Mango
{
	enum class MouseAction
	{
		None = 0,
		Press,
		Release
	};

	enum class MouseButton
	{
		None = 0,
		Left,
		Right
	};

	enum class KeyAction
	{
		None = 0,
		Press,
		Release,
		Repeat
	};

	enum class Key
	{
		None = 0,
		W,
		A,
		S,
		D,
		SPACE,
		ARROW_UP,
		ARROW_DOWN,
		ARROW_LEFT,
		ARROW_RIGHT,
		Q,
		E,
		R
	};

	class Input
	{
	public:
		Input() = delete;
		Input(const Input&) = delete;
		Input operator=(const Input&) = delete;

		static void StopHandlingInput();
		static void ResumeHandlingInput();

		static std::set<Mango::Key> GetPressedKeys() { return _pressedKeys; }
		static std::set<Mango::MouseButton> GetPressedMouseButtons() { return _pressedMouseButtons; }
		static glm::vec2 GetMouseCursorPosition() { return _cursorPosition; }

		static void KeyCallback(Mango::Key key, Mango::KeyAction action);
		static void MouseCallback(Mango::MouseButton button, Mango::MouseAction action);
		static void CursorPositionCallback(float x, float y);

	private:
		static std::set<Mango::Key> _pressedKeys;
		static std::set<Mango::MouseButton> _pressedMouseButtons;
		static glm::vec2 _cursorPosition;
		static bool _handlingStopped;
	};
}
