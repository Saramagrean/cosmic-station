#include <cassert>
#include <dlfcn.h>

#include <common/global.h>
#include <common/except.h>
#include <gpu/renders.h>
namespace zenith::gpu {
    bool RenderEngine::loadVulkanDriver() {
        auto serviceDriver{*(device->getStates()->customDriver)};
        auto appStorage{*(device->getStates()->appStorage)};

        if (driver)
            dlclose(std::exchange(driver, nullptr));

        if (serviceDriver.starts_with(appStorage)) {}

        if (!driver) {
            // Rolling back to the driver installed on the device
            driver = dlopen(serviceDriver.c_str(), RTLD_LAZY);
            if (!driver)
                driver = dlopen("libvulkan.so", RTLD_LAZY);
            if (!driver)
                throw GPUFail("No valid Vulkan driver was found on the host device");
        }
        vulkanInstanceAddr = bit_cast<PFN_vkGetInstanceProcAddr>(dlsym(driver, "vkGetInstanceProcAddr"));
        assert(driver && vulkanInstanceAddr);

        return true;
    }
    RenderEngine::~RenderEngine() {
        if (driver)
            dlclose(driver);
        vulkanInstanceAddr = nullptr;
    }
}
