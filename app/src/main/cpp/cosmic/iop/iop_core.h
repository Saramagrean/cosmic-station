#pragma once
#include <common/types.h>
#include <mio/mem_pipe.h>

#include <iop/iop_info.h>
#include <iop/iop_cop.h>
namespace cosmic::iop {
    struct IoCache {
        u32 data;
        u32 tag;
        bool isValid;
    };

    class IoMipsCore {
    public:
        IoMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe);
        void resetIop();
        void pulse(u32 cycles);
        u32 fetchByPc();

        void intByIntC(bool isInt);
        void handleException(u8 code);
        u32 incPc();
        void jumpTo(u8 effectiveGpr);
        void branchIf(bool cond, i32 offset);

        std::array<u32, 32> ioGPRs;
        std::array<u8, 1024 * 1> ioScratch;
        std::array<IoCache, 0x400> instCache;
        u32 cacheCtrl;
        u32 cacheHit, cacheMiss;
        std::shared_ptr<mio::MemoryPipe> iopMem;

        static u32 translateAddr(u32 address);
        bool isPcUncached(u32 pc) const;
        static bool isRoRegion(u32 address);

        template <typename T>
        T iopRead(u32 address) {
            address = translateAddr(address);
            if (isRoRegion(address)) {
                if constexpr (sizeof(T) == 4)
                    return PipeRead<T>(iopMem, address & 0x1fffffff, mio::IopDev);
            }
            u32 prime{iopPrivateAddrSolver(address & 0x1fffffff)};
            return PipeRead<T>(iopMem, prime, mio::IopDev);
        }
        template <typename T>
        void iopWrite(u32 address, u32 value) {
            address = translateAddr(address);
            if (isRoRegion(address)) {
                if constexpr (sizeof(T) == 4)
                    PipeWrite<T>(iopMem, address & 0x1fffffff, value, mio::IopDev);
                return;
            }
            u32 privateAddr{iopPrivateAddrSolver(address & 0x1fffffff)};
            PipeWrite<T>(iopMem, privateAddr, value, mio::IopDev);
        }

        u32 hi, lo;
        u32 lastPc,
            ioPc,
            waitPc;
        i64 cyclesToIo;
        IopCop cop;
        bool onBranch{false};
        u8 branchDelay{};
        i32 mathDelay{};

        void takeBranchIf(bool take, i32 pcAddr);
        u8 irqSpawned;
    private:
        static u32 iopPrivateAddrSolver(u32 address);

        std::unique_ptr<IopExecVe> interpreter;
    };
}

