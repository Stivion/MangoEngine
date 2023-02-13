#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>

#include <memory>

namespace Mango
{
	class RigidbodyComponent
	{
	public:
		RigidbodyComponent(b2Body* body);

		inline bool IsDynamic() { return _isDynamic; }
		inline glm::vec2 GetPosition() { b2Vec2 position = _body->GetPosition(); return glm::vec2(position.x, position.y); }
		inline float GetAngle() { return _body->GetAngle(); }
		inline b2Body* GetBody() { return _body; }

		void SetDynamic(bool isDynamic);
		void SetTransform(glm::vec2 position, float angleRadians);
		void SetFixture(b2FixtureDef fixture);
		void DestroyFixture();

		void ApplyForce(glm::vec2 force);

	private:
		bool _isDynamic = true;
		b2Body* _body;
		b2Fixture* _fixture = nullptr;
	};
}
