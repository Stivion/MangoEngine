#include "RigidbodyComponent.h"

Mango::RigidbodyComponent::RigidbodyComponent(b2Body* body) : _body(body)
{
	_body->SetType(b2_dynamicBody);
}

void Mango::RigidbodyComponent::SetDynamic(bool isDynamic)
{
	_isDynamic = isDynamic;
	_body->SetType(_isDynamic ? b2_dynamicBody : b2_staticBody);
}

void Mango::RigidbodyComponent::SetTransform(glm::vec2 position, float angleRadians)
{
	_body->SetTransform({ position.x, position.y }, angleRadians);
}

void Mango::RigidbodyComponent::SetFixture(b2FixtureDef fixture)
{
	_fixture = _body->CreateFixture(&fixture);
}

void Mango::RigidbodyComponent::DestroyFixture()
{
	_body->DestroyFixture(_fixture);
}

void Mango::RigidbodyComponent::ApplyForce(glm::vec2 force)
{
	_body->ApplyForceToCenter({ force.x, force.y }, true);
}
