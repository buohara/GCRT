#include "gcrtvk.h"

using namespace std;

struct RendererVK
{
    VkInstance instance;
    HWND hWnd;

    VkSwapchainKHR swapChain;
    VkSurfaceKHR surface;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkFormat colorFormat;
    VkColorSpaceKHR colorSpace;

    uint32_t queueNodeIndex = UINT32_MAX;

    void CreateVkInstance();
    void CreateRenderWindow(HINSTANCE hInstance);
    void CreateSwapChain(HINSTANCE hInstance);
};