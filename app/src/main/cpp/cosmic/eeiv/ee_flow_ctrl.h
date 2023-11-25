#pragma once

#include <common/types.h>
namespace cosmic::eeiv {
    class EEPC {
    public:
        EEPC() = default;
        EEPC(u32 pc) : pcValue(pc) {}
        auto operator++(i32 lastPC) {
            return pcValue += 4;
        }
        auto operator--(i32 lastPC) {
            return pcValue -= 4;
        }
        auto operator*() {
            return pcValue;
        }
        u32 pcValue{};
    };

    struct EEFlowCtrl {
        u32 cyclesToWaste;
        u32 cycles;
    };
}