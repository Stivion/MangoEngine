#pragma once

namespace Mango
{
	class Renderer
	{
	public:
		virtual void Draw() = 0;
		/*
		* Basically, renderer structure would be like this;
		* 1. Game World renderer that record all commands to command buffer
		* 2. UI renderer that depends on results of Game World renderer and renders MangoEngine UI and Game World in viewport
		* Both renderers produces a Mango::CommandBuffer as a result of Mango::Renderer::RecordCommandBuffer() method
		* Next renderer takes this command buffers and combines them into an array
		* Then it is submitted to Mango::Renderer::Draw() method, which will submit both command buffers to execution on GPU
		* 
		* Problem is:
		* We need to provide implementation for Mango::Renderer::Draw() for both Game World and UI renderers
		* which doesn't make any sense because Draw method will just execute already recorded command buffers,
		* it doesn't care how they has been recorded.
		* 
		* Which means that we need to have a single Vulkan Renderer which will accept command buffers as a parameter to Draw method.
		* And command buffers recording we need to delegate to two separate classes -> GameWorldCommandBufferRecorder and UICommandBufferRecorder
		* 
		* Maybe like this:
		* abstract class CommandBufferRecorder
		* {
		*     virtual void RecordCommandBuffer() = 0;
		*     virtual Mango::CommandBuffer GetBuffer() = 0;
		* }
		* 
		* class GameWorldCommandBufferRecorder : public CommandBufferRecorder
		* {
		*     void RecordCommandBuffer() override;
		*     Mango::CommandBuffer GetBuffer() override;
		* }
		* 
		* class UICommandBufferRecorder : public CommandBufferRecorder
		* {
		*     void RecordCommandBuffer() override;
		*     Mango::CommandBuffer GetBuffer() override;
		* }
		* 
		* With this structure we abstracted command buffer recording to any class, that is inherited from base CommandBufferRecorder
		* and it will make adding additional recorders easier (maybe we don't need it actually).
		* Also this will remove the need for specifying _isOffscreen flag as right now, because we always want to render on screen.
		* 
		* As an implementation detail - both recorders must be independent from each other, because eventually we want to create a release
		* builds of the game and we don't need to render a game engine interface (and bundle ImGui) and this approach should make it easier
		* in the future.
		* 
		* One more detail - both recorders can get all the data they needed in any way they want, i.e. constructors
		* or fields with GameObjects that exists in the scene, it is implementation-dependent.
		*/
		virtual void HandleFramebuffersResized() = 0;
		virtual ~Renderer() = default;
	};
}
