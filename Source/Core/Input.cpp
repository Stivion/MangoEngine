#include "Input.h"

std::set<Mango::Key> Mango::Input::_pressedKeys;
std::set<Mango::MouseButton> Mango::Input::_pressedMouseButtons;
glm::vec2 Mango::Input::_cursorPosition;

void Mango::Input::KeyCallback(Mango::Key key, Mango::KeyAction action)
{
	if (key == Key::None || action == KeyAction::None)
	{
		return;
	}

	if (action == Mango::KeyAction::Press)
	{
		_pressedKeys.insert(key);
	}
	else if (action == Mango::KeyAction::Release)
	{
		_pressedKeys.erase(key);
	}
}

void Mango::Input::MouseCallback(Mango::MouseButton button, Mango::MouseAction action)
{
	if (button == MouseButton::None || action == MouseAction::None)
	{
		return;
	}

	if (action == Mango::MouseAction::Press)
	{
		_pressedMouseButtons.insert(button);
	}
	else if (action == Mango::MouseAction::Release)
	{
		_pressedMouseButtons.erase(button);
	}
}

void Mango::Input::CursorPositionCallback(float x, float y)
{
	_cursorPosition = glm::vec2(x, y);
}
