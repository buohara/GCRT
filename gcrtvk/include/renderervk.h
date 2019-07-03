#include "gcrtvk.h"
#include "utilsvk.h"
#include "renderpass.h"

using namespace std;
using namespace glm;

struct RendererVK
{
    // VK device objects

    VkInstance instance;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    // Queue objects

    uint32_t queueFamilyCount;
    vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkQueue queue;

    uint32_t graphicsQueueIdx   = VK_NULL_HANDLE;
    uint32_t computeQueueIdx    = VK_NULL_HANDLE;
    uint32_t copyQueueIdx       = VK_NULL_HANDLE;

    // Window/swap chain objects

    HWND hWnd;

    VkSwapchainKHR swapChain    = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkSubmitInfo submitInfo;
    VkSemaphore presentComplete;
    VkSemaphore submitComplete;

    VkFormat colorFormat;
    VkFormat depthFormat;
    VkColorSpaceKHR colorSpace;

    uint32_t queueNodeIndex     = UINT32_MAX;
    uint32_t scSize;
    uint32_t curSCBuf;

    vector<VkImage> scImages;
    vector<VkImageView> scImageViews;
    vector<VkFramebuffer> frameBuffers;

    VkImage zImage;
    VkDeviceMemory zMem;
    VkImageView zView;
    

    RendererVK(HINSTANCE hInstance);  
    void Render(SceneVk &scn);
    void Upload(SceneVk& scn);
    void Add(RenderPassVk& pass) { passes.push_back(pass); }

private:

    void CreateLogicalDevice(
        VkPhysicalDeviceFeatures enabledFeatures, 
        vector<const char*> enabledExtensions,
        bool useSwapChain = true,
        VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT
    );
    
   vector<RenderPassVk> passes;

    void CreateVkDevices();
    void CreateVkInstance();
    void CreateRenderWindow(HINSTANCE hInstance);
    void GetPresentSurface(HINSTANCE hInstance);
    void CreateSwapChain();
    void CreateDepth();
    void SetupFrameBuffer();

private: 

    RendererVK();
};