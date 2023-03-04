#include "Input.h"

std::set<Mango::Key> Mango::Input::_pressedKeys;

std::set<Mango::Key> Mango::Input::GetPressedKeys()
{
	return _pressedKeys;
}

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
