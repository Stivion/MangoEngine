#pragma once

#include <glm/glm.hpp>

namespace Mango
{
	class CameraComponent
	{
	public:
		CameraComponent(bool isEditorCamera) : CameraComponent() { _isEditorCamera = isEditorCamera; }
		CameraComponent()
		{
			_nearPlane = 0.1f;
			_farPlane = 10.0f;
			_fovDegrees = 45.0f;
			_isPrimary = false;
			_isEditorCamera = false;
		}

		inline float GetNearPlane() { return _nearPlane; }
		inline float GetFarPlane() { return _farPlane; }
		inline float GetFOV() { return _fovDegrees; }
		inline bool IsPrimary() { return _isPrimary; }
		inline bool IsEditorCamera() { return _isEditorCamera; }

		void SetClippingPlanes(float nearPlane, float farPlane) { _nearPlane = nearPlane; _farPlane = farPlane; }
		void SetFOV(float fovDegrees) { _fovDegrees = fovDegrees; }
		void SetPrimary(bool primary) { _isPrimary = primary; }
		void SetEditorCamera(bool editor) { _isEditorCamera = editor; }

	private:
		float _nearPlane;
		float _farPlane;
		float _fovDegrees;
		bool _isPrimary;

		// Editor only
		bool _isEditorCamera;
	};
}
