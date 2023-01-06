#pragma once

namespace Mango
{
	class Image
	{
	public:
		Image();
		Image(const Image&) = delete;
		Image operator=(const Image&) = delete;
		~Image();
	private:
	};
}
