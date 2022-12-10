#include <vector>
#include <vulkan/vulkan.h>

namespace Mango 
{
    struct SwapChainSupportDetails
    {
    public:
        VkSurfaceCapabilitiesKHR surfaceCapabilities {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& renderSurface);
    private:
        SwapChainSupportDetails() = default;
    };

}
