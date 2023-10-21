#pragma once

#include <types.h>

namespace zenith::eeiv {
enum MipsRegsHw : u8 {
    $zero, // Hardwired 0 into the semiconductor
    $at,
    $v0, $v1, // MIPS compilers use these registers to return values from subroutines
    $a0, $a1, $a2, $a3, // And, of course, function parameter values; the rest are passed by the stack (nothing special)
    $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
    $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,
    $t8, $t9,
    $k0, $k1,
    $gp, // Global pointer (I don't have any information of what this means for now)
    $sp, // Stack pointer
    $fp, // Frame pointer
    $ra, // Return address
};

struct EECoreAssembler {
    // https://usermanual.wiki/Pdf/EECoreInstructionSetManual.986088270/help
    constexpr static u32 lui(const MipsRegsHw dest, u32 imm) {
        constexpr u32 luiOpcode{0x0f << 26};
        auto resultInst{static_cast<u32>(0 | static_cast<u16>(imm >> 16) | dest << 16) | luiOpcode};
        return resultInst;
    }

    constexpr static u32 ori(const MipsRegsHw dest, const MipsRegsHw src, i32 imm) {
        constexpr u32 oriOpcode{0x0d << 26};
        auto resultInst{static_cast<u32>(imm | dest << 16 | src << 21) | oriOpcode};
        return resultInst;
    }

    constexpr static u32 addiu(const MipsRegsHw dest, const MipsRegsHw src, i16 imm) {
        constexpr u32 addiuOpcode{0x09 << 26};
        auto resultInst{static_cast<u32>(static_cast<u16>(imm) | dest << 16 | src << 21) | addiuOpcode};
        return resultInst;
    }

    constexpr static u32 sq(const MipsRegsHw src, const MipsRegsHw base, i16 offset) {
        constexpr u32 sqOpcode{0x1f << 26};
        auto resultInst{static_cast<u32>(static_cast<u16>(offset) | src << 16 | base << 21) | sqOpcode};
        return resultInst;
    }

    constexpr static u32 add(const MipsRegsHw dest, const MipsRegsHw src, const MipsRegsHw src1) {
        auto resultInst{static_cast<u32>(0x20 | dest << 11 | src1 << 16 | src << 21)};
        return resultInst;
    }

    // To load a word from memory as a signed value
    constexpr static u32 lw(const MipsRegsHw dest, const MipsRegsHw base, i16 offset) {
        constexpr auto lwOpcode{static_cast<u32>(0x23 << 26)};
        auto resultInst{static_cast<u32>(static_cast<u16>(offset) | dest << 16 | base << 21) | lwOpcode};
        return resultInst;
    }

    constexpr static u32 _and(const MipsRegsHw dest, const MipsRegsHw src, const MipsRegsHw src1) {
        auto resultInst{static_cast<u32>(0x20 | dest << 11 | src1 << 16 | src << 21)};
        return resultInst;
    }

    constexpr static u32 lq(const MipsRegsHw dest, const MipsRegsHw base, i16 offset) {
        constexpr u32 lqOpcode{static_cast<u32>(0x1e << 26)};
        auto resultInst{static_cast<u32>(static_cast<u16>(offset) | dest << 16 | base << 21) | lqOpcode};
        return resultInst;
    }

    // To store a word data in memory
    constexpr static u32 sw(const MipsRegsHw src, const MipsRegsHw base, i16 offset) {
        constexpr u32 swOpcode{static_cast<u32>(0x2b << 26)};
        auto resultInst{static_cast<u32>(static_cast<u16>(offset) | src << 16 | base << 21) | swOpcode};
        return resultInst;
    }

    constexpr static u32 jalr(u8 address) {
        auto resultInst{static_cast<u32>(0x9 | eeiv::$ra << 11 | address << 21)};
        return resultInst;
    }
    constexpr static u32 eret() {
        auto resultInst{static_cast<u32>(0x18 | 0x10 << 21 | 0x10 << 26)};
        return resultInst;
    }
};
}
