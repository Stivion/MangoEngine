#include "GameObject.h"

Mango::GameObject::GameObject(Mango::GUID id) : _id(id)
{
}

const void Mango::GameObject::SetPosition(glm::vec3 position)
{
	_position = position;
}
