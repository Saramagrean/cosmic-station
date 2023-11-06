#pragma once

#include <thread>
namespace zenith::console {
    class EmuVM;

    class EmuThread {
    public:
        EmuThread(EmuVM& vm);
        void runVM();

    private:
        static void vmMain(EmuVM& vm);
        std::thread vmt;
    };
}
