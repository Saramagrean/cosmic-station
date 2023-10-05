#include <app.h>
#include <java/device_handler.h>

#include <eeiv/ee_engine.h>
#include <eeiv/cop0.h>

#include <eeiv/fuji/cached_interpreter.h>
#include <eeiv/tokyo3/tokyo3_arm64_jitter.h>

namespace zenith::eeiv {
    EEMipsCore::EEMipsCore(const std::shared_ptr<link::GlobalMemory>& global)
        : glbRDRAM(global),
          eeTLB(std::make_shared<TLBCache>(global)) {

        GPRs = new eeRegister[countOfGPRs];
        eeNearCache = new EECacheLine[countOfCacheLines];

        auto globalStates{device->getServiceState()};
        proCPUMode = static_cast<EEExecutionMode>(*globalStates.lock()->cpuExecutor);

        if (proCPUMode == EEExecutionMode::CachedInterpreter)
            eeExecutor = std::make_unique<fuji::EEInterpreter>(*this);
        else if (proCPUMode == EEExecutionMode::JitRe)
            eeExecutor = std::make_unique<tokyo3::EEArm64Jitter>(*this);
    }

    EEMipsCore::~EEMipsCore() {
        delete[] GPRs;
    }

    void EEMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        eePC = 0xbfc00000;
        virtTable = cop0.mapVirtualTLB(eeTLB);

        constexpr auto invLane01Cache{static_cast<u32>(1<<31)};
        // Invalidating all cache lines
        for (u8 line{}; line < countOfCacheLines; line++) {
            eeNearCache[line].tags[0] = invLane01Cache;
            eeNearCache[line].tags[1] = invLane01Cache;

            eeNearCache[line].lfu[0] = false;
            eeNearCache[line].lfu[1] = false;
        }

        // Cleaning up all registers, including the $zero register
        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            static auto gprs{reinterpret_cast<u64*>(GPRs)};
            static uint64x1x4_t zero{};
            // Writing 256 bits (32 bytes) per write call

            vst1_u64_x4(gprs + regRange, zero);
            vst1_u64_x4(gprs + regRange + 2, zero);
            vst1_u64_x4(gprs + regRange + 4, zero);
            vst1_u64_x4(gprs + regRange + 6, zero);
        }

    }
}
