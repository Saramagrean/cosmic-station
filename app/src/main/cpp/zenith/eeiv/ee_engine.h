#pragma once

#include <os/neon_simd.h>
#include <link/managed_glb_memory.h>
#include <eeiv/cop0.h>
#include <eeiv/high_fast_cache.h>
#include <eeiv/mmu_tlb.h>

#include <eeiv/ee_handler.h>
namespace zenith::eeiv {
    enum class EEExecutionMode : u8 {
        // Increases instruction decoding speed through cache blocks, which is faster
        // than a simple interpreter
        CachedInterpreter,
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe
    };

    class EEMipsCore {
        static constexpr u8 countOfGPRs{32};
        static constexpr u8 countOfCacheLines{128};
    public:
        EEMipsCore(const std::shared_ptr<console::GlobalMemory>& glbRef);
        ~EEMipsCore();

        void resetCore();
        EEExecutionMode proCPUMode{EEExecutionMode::CachedInterpreter};
    private:

        std::shared_ptr<console::GlobalMemory> glbRDRAM;
        union eeRegister {
            eeRegister() {}
            struct {
                os::machVec128 qw{0, 0};
                u64 dw[2];
                u32 words[4];
                u16 hw[8];
            };
            u8 rwBytes[16];
        };

        eeRegister* GPRs;
        EECacheLine* eeNearCache;

        u32 eePC{};
        CoProcessor0 copCPU0;

        std::unique_ptr<TLBCache> eeTLB;
        // Class that provides CPU code execution functionality
        std::unique_ptr<EEExecutor> eeExecutor;
    };
}