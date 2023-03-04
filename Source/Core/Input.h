#pragma once

#include <set>

namespace Mango
{
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

		static std::set<Mango::Key> GetPressedKeys();

		static void KeyCallback(Mango::Key key, Mango::KeyAction action);

	private:
		static std::set<Mango::Key> _pressedKeys;
	};
}
