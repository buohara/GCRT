#include "gcrtvk.h"
#include "utils.h"

using namespace std;
using namespace glm;

struct RendererVK
{
    VkInstance instance;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    uint32_t queueFamilyCount;
    vector<VkQueueFamilyProperties> queueFamilyProperties;

    uint32_t graphicsQueueIdx   = VK_NULL_HANDLE;
    uint32_t computeQueueIdx    = VK_NULL_HANDLE;
    uint32_t copyQueueIdx       = VK_NULL_HANDLE;

    HWND hWnd;

    VkSwapchainKHR swapChain    = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool   = VK_NULL_HANDLE;
    VkSubmitInfo submitInfo;
    VkSemaphore presentComplete;
    VkSemaphore submitComplete;
    VkQueue queue;

    VkFormat colorFormat;
    VkFormat depthFormat;
    VkColorSpaceKHR colorSpace;
    VkCommandPool cmdPool;

    uint32_t queueNodeIndex     = UINT32_MAX;
    uint32_t scSize;
    uint32_t curSCBuf;

    vector<VkImage> scImages;
    vector<VkImageView> scImageViews;
    vector<VkFramebuffer> frameBuffers;

    VkImage zImage;
    VkDeviceMemory zMem;
    VkImageView zView;

    vector<VkCommandBuffer> drawCmdBuffers;
    vector<VkFence> fences;

    uint32_t FindProperties(
        uint32_t memoryTypeBitsRequirement,
        VkMemoryPropertyFlags requiredProperties
    );
    
    void Init();
    
    void CreateLogicalDevice(
        VkPhysicalDeviceFeatures enabledFeatures, 
        vector<const char*> enabledExtensions,
        bool useSwapChain = true,
        VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT
    );
    
    void CreateVkInstance();
    void CreateRenderWindow(HINSTANCE hInstance);
    void GetPresentSurface(HINSTANCE hInstance);
    void CreateCommandPool(uint32_t queueIdx);
    void CreateSwapChain();
    void CreateCommandBuffers();
    void CreateFenceObjects();
    
    void Render();

    VkCommandBuffer GetCommandBuffer(bool begin);
    void FlushCommandBuffer(VkCommandBuffer cmdBuf);
};