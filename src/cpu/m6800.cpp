// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*** m6800: Portable 6800 class  emulator *************************************
    m68xx.c
    References:
        6809 Simulator V09, By L.C. Benschop, Eindhoven The Netherlands.
        m6809: Portable 6809 emulator, DS (6809 code in MAME, derived from
            the 6809 Simulator V09)
        6809 Microcomputer Programming & Interfacing with Experiments"
            by Andrew C. Staugaard, Jr.; Howard W. Sams & Co., Inc.

    System dependencies:    uint16_t must be 16 bit unsigned int
                            uint8_t must be 8 bit unsigned int
                            uint32_t must be more than 16 bits
                            arrays up to 65536 bytes must be supported
                            machine must be twos complement
*/

#include "m6800.h"

#define VERBOSE 0

#define LOG(x)          \
    do {                \
        if (VERBOSE)    \
            logerror x; \
    } while (0)

#define pPPC m_ppc
#define pPC m_pc
#define pS m_s
#define pX m_x
#define pD m_d

#define PC m_pc.w.l
#define PCD m_pc.d
#define S m_s.w.l
#define SD m_s.d
#define X m_x.w.l
#define D m_d.w.l
#define A m_d.b.h
#define B m_d.b.l
#define CC m_cc

#define EAD m_ea.d
#define EA m_ea.w.l

/* memory interface */

/****************************************************************************/
/* Read a byte from given memory location                                   */
/****************************************************************************/
#define RM(Addr) (this->read_byte(Addr))

/****************************************************************************/
/* Write a byte to given memory location                                    */
/****************************************************************************/
#define WM(Addr, Value) (this->write_byte(Addr, Value))

/****************************************************************************/
/* M6800_RDOP() is identical to M6800_RDMEM() except it is used for reading */
/* opcodes. In case of system with memory mapped I/O, this function can be  */
/* used to greatly speed up emulation                                       */
/****************************************************************************/
#define M_RDOP(Addr) (this->read_byte(Addr))

/****************************************************************************/
/* M6800_RDOP_ARG() is identical to M6800_RDOP() but it's used for reading  */
/* opcode arguments. This difference can be used to support systems that    */
/* use different encoding mechanisms for opcodes and opcode arguments       */
/****************************************************************************/
#define M_RDOP_ARG(Addr) (this->read_byte(Addr))

/* macros to access memory */
#define IMMBYTE(b)       \
    b = M_RDOP_ARG(PCD); \
    PC++
#define IMMWORD(w)                                                 \
    w.d = (M_RDOP_ARG(PCD) << 8) | M_RDOP_ARG((PCD + 1) & 0xffff); \
    PC += 2

#define PUSHBYTE(b) \
    WM(SD, b);      \
    --S
#define PUSHWORD(w) \
    WM(SD, w.b.l);  \
    --S;            \
    WM(SD, w.b.h);  \
    --S
#define PULLBYTE(b) \
    S++;            \
    b = RM(SD)
#define PULLWORD(w)    \
    S++;               \
    w.d = RM(SD) << 8; \
    S++;               \
    w.d |= RM(SD)

/* operate one instruction for */
#define ONE_MORE_INSN()                    \
    {                                      \
        uint8_t ireg;                      \
        pPPC = pPC;                        \
        ireg = M_RDOP(PCD);                \
        PC++;                              \
        (this->*m_insn[ireg])();           \
        increment_counter(m_cycles[ireg]); \
    }

/* CC masks                       HI NZVC
                                7654 3210   */
#define CLR_HNZVC CC &= 0xd0
#define CLR_NZV CC &= 0xf1
#define CLR_HNZC CC &= 0xd2
#define CLR_NZVC CC &= 0xf0
#define CLR_Z CC &= 0xfb
#define CLR_ZC CC &= 0xfa
#define CLR_C CC &= 0xfe

/* macros for CC -- CC bits affected should be reset before calling */
#define SET_Z(a) \
    if (!(a))    \
    SEZ
#define SET_Z8(a) SET_Z((uint8_t)(a))
#define SET_Z16(a) SET_Z((uint16_t)(a))
#define SET_N8(a) CC |= (((a)&0x80) >> 4)
#define SET_N16(a) CC |= (((a)&0x8000) >> 12)
#define SET_H(a, b, r) CC |= ((((a) ^ (b) ^ (r)) & 0x10) << 1)
#define SET_C8(a) CC |= (((a)&0x100) >> 8)
#define SET_C16(a) CC |= (((a)&0x10000) >> 16)
#define SET_V8(a, b, r) CC |= ((((a) ^ (b) ^ (r) ^ ((r) >> 1)) & 0x80) >> 6)
#define SET_V16(a, b, r) CC |= ((((a) ^ (b) ^ (r) ^ ((r) >> 1)) & 0x8000) >> 14)

const uint8_t m6800_cpu_device::flags8i[256] = /* increment */
    {
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0a, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
    };

const uint8_t m6800_cpu_device::flags8d[256] = /* decrement */
    {
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
    };

#define SET_FLAGS8I(a)           \
    {                            \
        CC |= flags8i[(a)&0xff]; \
    }
#define SET_FLAGS8D(a)           \
    {                            \
        CC |= flags8d[(a)&0xff]; \
    }

/* combos */
#define SET_NZ8(a) \
    {              \
        SET_N8(a); \
        SET_Z8(a); \
    }
#define SET_NZ16(a) \
    {               \
        SET_N16(a); \
        SET_Z16(a); \
    }
#define SET_FLAGS8(a, b, r) \
    {                       \
        SET_N8(r);          \
        SET_Z8(r);          \
        SET_V8(a, b, r);    \
        SET_C8(r);          \
    }
#define SET_FLAGS16(a, b, r) \
    {                        \
        SET_N16(r);          \
        SET_Z16(r);          \
        SET_V16(a, b, r);    \
        SET_C16(r);          \
    }

/* for treating an uint8_t as a signed int16_t */
#define SIGNED(b) ((int16_t)(b & 0x80 ? b | 0xff00 : b))

/* Macros for addressing modes */
#define DIRECT IMMBYTE(EAD)
#define IMM8 EA = PC++
#define IMM16    \
    {            \
        EA = PC; \
        PC += 2; \
    }
#define EXTENDED IMMWORD(m_ea)
#define INDEXED                            \
    {                                      \
        EA = X + (uint8_t)M_RDOP_ARG(PCD); \
        PC++;                              \
    }

/* macros to set status flags */
#if defined(SEC)
#undef SEC
#endif
#define SEC CC |= 0x01
#define CLC CC &= 0xfe
#define SEZ CC |= 0x04
#define CLZ CC &= 0xfb
#define SEN CC |= 0x08
#define CLN CC &= 0xf7
#define SEV CC |= 0x02
#define CLV CC &= 0xfd
#define SEH CC |= 0x20
#define CLH CC &= 0xdf
#define SEI CC |= 0x10
#define CLI CC &= ~0x10

/* macros for convenience */
#define DIRBYTE(b)   \
    {                \
        DIRECT;      \
        b = RM(EAD); \
    }
#define DIRWORD(w)       \
    {                    \
        DIRECT;          \
        w.d = RM16(EAD); \
    }
#define EXTBYTE(b)   \
    {                \
        EXTENDED;    \
        b = RM(EAD); \
    }
#define EXTWORD(w)       \
    {                    \
        EXTENDED;        \
        w.d = RM16(EAD); \
    }

#define IDXBYTE(b)   \
    {                \
        INDEXED;     \
        b = RM(EAD); \
    }
#define IDXWORD(w)       \
    {                    \
        INDEXED;         \
        w.d = RM16(EAD); \
    }

/* Macros for branch instructions */
#define BRANCH(f)            \
    {                        \
        IMMBYTE(t);          \
        if (f) {             \
            PC += SIGNED(t); \
        }                    \
    }
#define NXORV ((CC & 0x08) ^ ((CC & 0x02) << 2))
#define NXORC ((CC & 0x08) ^ ((CC & 0x01) << 3))

/* include the opcode functions */
#include "6800ops.hxx"

uint8_t m6800_cpu_device::read_byte(offs_t address) {
    return memory_map->read(address & 0xFFFF);
}

void m6800_cpu_device::write_byte(offs_t address, uint8_t data) {
    // TODO: Better memory mapping to protect ROM / simulate unmapped memory
    this->memory_map->write(address & 0xFFFF, data & 0xFF);
}

void m6800_cpu_device::write_bytes(offs_t address, uint8_t* data, size_t size) {
    for (int i = 0; i < size; i++) {
        this->memory_map->write(address + i, data[i]);
    }
}

/* Note: don't use 0 cycles here for invalid opcodes so that we don't */
/* hang in an infinite loop if we hit one */
#define XX 5 // invalid opcode unknown cc
const uint8_t m6800_cpu_device::cycles_6800[256] = {
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    /*0*/ XX, 2, XX, XX, XX, XX, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2,
    /*1*/ 2, 2, XX, XX, XX, XX, 2, 2, XX, 2, XX, 2, XX, XX, XX, XX,
    /*2*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    /*3*/ 4, 4, 4, 4, 4, 4, 4, 4, XX, 5, XX, 10, XX, XX, 9, 12,
    /*4*/ 2, XX, XX, 2, 2, XX, 2, 2, 2, 2, 2, XX, 2, 2, XX, 2,
    /*5*/ 2, XX, XX, 2, 2, XX, 2, 2, 2, 2, 2, XX, 2, 2, XX, 2,
    /*6*/ 7, XX, XX, 7, 7, XX, 7, 7, 7, 7, 7, XX, 7, 7, 4, 7,
    /*7*/ 6, XX, XX, 6, 6, XX, 6, 6, 6, 6, 6, XX, 6, 6, 3, 6,
    /*8*/ 2, 2, 2, XX, 2, 2, 2, 3, 2, 2, 2, 2, 3, 8, 3, 4,
    /*9*/ 3, 3, 3, XX, 3, 3, 3, 4, 3, 3, 3, 3, 4, 6, 4, 5,
    /*A*/ 5, 5, 5, XX, 5, 5, 5, 6, 5, 5, 5, 5, 6, 8, 6, 7,
    /*B*/ 4, 4, 4, XX, 4, 4, 4, 5, 4, 4, 4, 4, 5, 9, 5, 6,
    /*C*/ 2, 2, 2, XX, 2, 2, 2, 3, 2, 2, 2, 2, XX, XX, 3, 4,
    /*D*/ 3, 3, 3, XX, 3, 3, 3, 4, 3, 3, 3, 3, XX, XX, 4, 5,
    /*E*/ 5, 5, 5, XX, 5, 5, 5, 6, 5, 5, 5, 5, XX, XX, 6, 7,
    /*F*/ 4, 4, 4, XX, 4, 4, 4, 5, 4, 4, 4, 4, XX, XX, 5, 6
};

const uint8_t m6800_cpu_device::cycles_nsc8105[256] = {
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    /*0*/ 5, XX, 2, XX, XX, 2, XX, 2, 4, 2, 4, 2, 2, 2, 2, 2,
    /*1*/ 2, XX, 2, XX, XX, 2, XX, 2, XX, XX, 2, 2, XX, XX, XX, XX,
    /*2*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    /*3*/ 4, 4, 4, 4, 4, 4, 4, 4, XX, XX, 5, 10, XX, 9, XX, 12,
    /*4*/ 2, 2, 2, XX, 2, 2, 2, 3, 2, 2, 2, 2, 3, 3, 8, 4,
    /*5*/ 3, 3, 3, XX, 3, 3, 3, 4, 3, 3, 3, 3, 4, 4, 6, 5,
    /*6*/ 5, 5, 5, XX, 5, 5, 5, 6, 5, 5, 5, 5, 6, 6, 8, 7,
    /*7*/ 4, 4, 4, XX, 4, 4, 4, 5, 4, 4, 4, 4, 5, 5, 9, 6,
    /*8*/ 2, XX, XX, 2, 2, 2, XX, 2, 2, 2, 2, XX, 2, XX, 2, 2,
    /*9*/ 2, XX, XX, 2, 2, 2, XX, 2, 2, 2, 2, XX, 2, XX, 2, 2,
    /*A*/ 7, XX, XX, 7, 7, 7, XX, 7, 7, 7, 7, XX, 7, 4, 7, 7,
    /*B*/ 6, XX, XX, 6, 6, 6, XX, 6, 6, 6, 6, 5, 6, 3, 6, 6,
    /*C*/ 2, 2, 2, XX, 2, 2, 2, 3, 2, 2, 2, 2, XX, 3, XX, 4,
    /*D*/ 3, 3, 3, XX, 3, 3, 3, 4, 3, 3, 3, 3, XX, 4, XX, 5,
    /*E*/ 5, 5, 5, XX, 5, 5, 5, 6, 5, 5, 5, 5, 5, 6, XX, 7,
    /*F*/ 4, 4, 4, XX, 4, 4, 4, 5, 4, 4, 4, 4, 4, 5, XX, 6
};
#undef XX // /invalid opcode unknown cc

const m6800_cpu_device::op_func m6800_cpu_device::m6800_insn[0x100] = {
    &m6800_cpu_device::illegl1, &m6800_cpu_device::nop, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::tap, &m6800_cpu_device::tpa,
    &m6800_cpu_device::inx, &m6800_cpu_device::dex, &m6800_cpu_device::clv, &m6800_cpu_device::sev, &m6800_cpu_device::clc, &m6800_cpu_device::sec, &m6800_cpu_device::cli, &m6800_cpu_device::sei,
    &m6800_cpu_device::sba, &m6800_cpu_device::cba, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::tab, &m6800_cpu_device::tba,
    &m6800_cpu_device::illegl1, &m6800_cpu_device::daa, &m6800_cpu_device::illegl1, &m6800_cpu_device::aba, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1,
    &m6800_cpu_device::bra, &m6800_cpu_device::brn, &m6800_cpu_device::bhi, &m6800_cpu_device::bls, &m6800_cpu_device::bcc, &m6800_cpu_device::bcs, &m6800_cpu_device::bne, &m6800_cpu_device::beq,
    &m6800_cpu_device::bvc, &m6800_cpu_device::bvs, &m6800_cpu_device::bpl, &m6800_cpu_device::bmi, &m6800_cpu_device::bge, &m6800_cpu_device::blt, &m6800_cpu_device::bgt, &m6800_cpu_device::ble,
    &m6800_cpu_device::tsx, &m6800_cpu_device::ins, &m6800_cpu_device::pula, &m6800_cpu_device::pulb, &m6800_cpu_device::des, &m6800_cpu_device::txs, &m6800_cpu_device::psha, &m6800_cpu_device::pshb,
    &m6800_cpu_device::illegl1, &m6800_cpu_device::rts, &m6800_cpu_device::illegl1, &m6800_cpu_device::rti, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::wai, &m6800_cpu_device::swi,
    &m6800_cpu_device::nega, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::coma, &m6800_cpu_device::lsra, &m6800_cpu_device::illegl1, &m6800_cpu_device::rora, &m6800_cpu_device::asra,
    &m6800_cpu_device::asla, &m6800_cpu_device::rola, &m6800_cpu_device::deca, &m6800_cpu_device::illegl1, &m6800_cpu_device::inca, &m6800_cpu_device::tsta, &m6800_cpu_device::illegl1, &m6800_cpu_device::clra,
    &m6800_cpu_device::negb, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::comb, &m6800_cpu_device::lsrb, &m6800_cpu_device::illegl1, &m6800_cpu_device::rorb, &m6800_cpu_device::asrb,
    &m6800_cpu_device::aslb, &m6800_cpu_device::rolb, &m6800_cpu_device::decb, &m6800_cpu_device::illegl1, &m6800_cpu_device::incb, &m6800_cpu_device::tstb, &m6800_cpu_device::illegl1, &m6800_cpu_device::clrb,
    &m6800_cpu_device::neg_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::illegl2, &m6800_cpu_device::com_ix, &m6800_cpu_device::lsr_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::ror_ix, &m6800_cpu_device::asr_ix,
    &m6800_cpu_device::asl_ix, &m6800_cpu_device::rol_ix, &m6800_cpu_device::dec_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::inc_ix, &m6800_cpu_device::tst_ix, &m6800_cpu_device::jmp_ix, &m6800_cpu_device::clr_ix,
    &m6800_cpu_device::neg_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::illegl3, &m6800_cpu_device::com_ex, &m6800_cpu_device::lsr_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::ror_ex, &m6800_cpu_device::asr_ex,
    &m6800_cpu_device::asl_ex, &m6800_cpu_device::rol_ex, &m6800_cpu_device::dec_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::inc_ex, &m6800_cpu_device::tst_ex, &m6800_cpu_device::jmp_ex, &m6800_cpu_device::clr_ex,
    &m6800_cpu_device::suba_im, &m6800_cpu_device::cmpa_im, &m6800_cpu_device::sbca_im, &m6800_cpu_device::illegl2, &m6800_cpu_device::anda_im, &m6800_cpu_device::bita_im, &m6800_cpu_device::lda_im, &m6800_cpu_device::sta_im,
    &m6800_cpu_device::eora_im, &m6800_cpu_device::adca_im, &m6800_cpu_device::ora_im, &m6800_cpu_device::adda_im, &m6800_cpu_device::cmpx_im, &m6800_cpu_device::bsr, &m6800_cpu_device::lds_im, &m6800_cpu_device::sts_im,
    &m6800_cpu_device::suba_di, &m6800_cpu_device::cmpa_di, &m6800_cpu_device::sbca_di, &m6800_cpu_device::illegl2, &m6800_cpu_device::anda_di, &m6800_cpu_device::bita_di, &m6800_cpu_device::lda_di, &m6800_cpu_device::sta_di,
    &m6800_cpu_device::eora_di, &m6800_cpu_device::adca_di, &m6800_cpu_device::ora_di, &m6800_cpu_device::adda_di, &m6800_cpu_device::cmpx_di, &m6800_cpu_device::jsr_di, &m6800_cpu_device::lds_di, &m6800_cpu_device::sts_di,
    &m6800_cpu_device::suba_ix, &m6800_cpu_device::cmpa_ix, &m6800_cpu_device::sbca_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::anda_ix, &m6800_cpu_device::bita_ix, &m6800_cpu_device::lda_ix, &m6800_cpu_device::sta_ix,
    &m6800_cpu_device::eora_ix, &m6800_cpu_device::adca_ix, &m6800_cpu_device::ora_ix, &m6800_cpu_device::adda_ix, &m6800_cpu_device::cmpx_ix, &m6800_cpu_device::jsr_ix, &m6800_cpu_device::lds_ix, &m6800_cpu_device::sts_ix,
    &m6800_cpu_device::suba_ex, &m6800_cpu_device::cmpa_ex, &m6800_cpu_device::sbca_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::anda_ex, &m6800_cpu_device::bita_ex, &m6800_cpu_device::lda_ex, &m6800_cpu_device::sta_ex,
    &m6800_cpu_device::eora_ex, &m6800_cpu_device::adca_ex, &m6800_cpu_device::ora_ex, &m6800_cpu_device::adda_ex, &m6800_cpu_device::cmpx_ex, &m6800_cpu_device::jsr_ex, &m6800_cpu_device::lds_ex, &m6800_cpu_device::sts_ex,
    &m6800_cpu_device::subb_im, &m6800_cpu_device::cmpb_im, &m6800_cpu_device::sbcb_im, &m6800_cpu_device::illegl2, &m6800_cpu_device::andb_im, &m6800_cpu_device::bitb_im, &m6800_cpu_device::ldb_im, &m6800_cpu_device::stb_im,
    &m6800_cpu_device::eorb_im, &m6800_cpu_device::adcb_im, &m6800_cpu_device::orb_im, &m6800_cpu_device::addb_im, &m6800_cpu_device::illegl3, &m6800_cpu_device::illegl3, &m6800_cpu_device::ldx_im, &m6800_cpu_device::stx_im,
    &m6800_cpu_device::subb_di, &m6800_cpu_device::cmpb_di, &m6800_cpu_device::sbcb_di, &m6800_cpu_device::illegl2, &m6800_cpu_device::andb_di, &m6800_cpu_device::bitb_di, &m6800_cpu_device::ldb_di, &m6800_cpu_device::stb_di,
    &m6800_cpu_device::eorb_di, &m6800_cpu_device::adcb_di, &m6800_cpu_device::orb_di, &m6800_cpu_device::addb_di, &m6800_cpu_device::illegl2, &m6800_cpu_device::illegl2, &m6800_cpu_device::ldx_di, &m6800_cpu_device::stx_di,
    &m6800_cpu_device::subb_ix, &m6800_cpu_device::cmpb_ix, &m6800_cpu_device::sbcb_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::andb_ix, &m6800_cpu_device::bitb_ix, &m6800_cpu_device::ldb_ix, &m6800_cpu_device::stb_ix,
    &m6800_cpu_device::eorb_ix, &m6800_cpu_device::adcb_ix, &m6800_cpu_device::orb_ix, &m6800_cpu_device::addb_ix, &m6800_cpu_device::illegl2, &m6800_cpu_device::illegl2, &m6800_cpu_device::ldx_ix, &m6800_cpu_device::stx_ix,
    &m6800_cpu_device::subb_ex, &m6800_cpu_device::cmpb_ex, &m6800_cpu_device::sbcb_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::andb_ex, &m6800_cpu_device::bitb_ex, &m6800_cpu_device::ldb_ex, &m6800_cpu_device::stb_ex,
    &m6800_cpu_device::eorb_ex, &m6800_cpu_device::adcb_ex, &m6800_cpu_device::orb_ex, &m6800_cpu_device::addb_ex, &m6800_cpu_device::illegl3, &m6800_cpu_device::illegl3, &m6800_cpu_device::ldx_ex, &m6800_cpu_device::stx_ex
};

const m6800_cpu_device::op_func m6800_cpu_device::nsc8105_insn[0x100] = {
    &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::nop, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::tap, &m6800_cpu_device::illegl1, &m6800_cpu_device::tpa,
    &m6800_cpu_device::inx, &m6800_cpu_device::clv, &m6800_cpu_device::dex, &m6800_cpu_device::sev, &m6800_cpu_device::clc, &m6800_cpu_device::cli, &m6800_cpu_device::sec, &m6800_cpu_device::sei,
    &m6800_cpu_device::sba, &m6800_cpu_device::illegl1, &m6800_cpu_device::cba, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::tab, &m6800_cpu_device::illegl1, &m6800_cpu_device::tba,
    &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::daa, &m6800_cpu_device::aba, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1,
    &m6800_cpu_device::bra, &m6800_cpu_device::bhi, &m6800_cpu_device::brn, &m6800_cpu_device::bls, &m6800_cpu_device::bcc, &m6800_cpu_device::bne, &m6800_cpu_device::bcs, &m6800_cpu_device::beq,
    &m6800_cpu_device::bvc, &m6800_cpu_device::bpl, &m6800_cpu_device::bvs, &m6800_cpu_device::bmi, &m6800_cpu_device::bge, &m6800_cpu_device::bgt, &m6800_cpu_device::blt, &m6800_cpu_device::ble,
    &m6800_cpu_device::tsx, &m6800_cpu_device::pula, &m6800_cpu_device::ins, &m6800_cpu_device::pulb, &m6800_cpu_device::des, &m6800_cpu_device::psha, &m6800_cpu_device::txs, &m6800_cpu_device::pshb,
    &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::rts, &m6800_cpu_device::rti, &m6800_cpu_device::illegl1, &m6800_cpu_device::wai, &m6800_cpu_device::illegl1, &m6800_cpu_device::swi,
    &m6800_cpu_device::suba_im, &m6800_cpu_device::sbca_im, &m6800_cpu_device::cmpa_im, &m6800_cpu_device::illegl1, &m6800_cpu_device::anda_im, &m6800_cpu_device::lda_im, &m6800_cpu_device::bita_im, &m6800_cpu_device::sta_im,
    &m6800_cpu_device::eora_im, &m6800_cpu_device::ora_im, &m6800_cpu_device::adca_im, &m6800_cpu_device::adda_im, &m6800_cpu_device::cmpx_im, &m6800_cpu_device::lds_im, &m6800_cpu_device::bsr, &m6800_cpu_device::sts_im,
    &m6800_cpu_device::suba_di, &m6800_cpu_device::sbca_di, &m6800_cpu_device::cmpa_di, &m6800_cpu_device::illegl1, &m6800_cpu_device::anda_di, &m6800_cpu_device::lda_di, &m6800_cpu_device::bita_di, &m6800_cpu_device::sta_di,
    &m6800_cpu_device::eora_di, &m6800_cpu_device::ora_di, &m6800_cpu_device::adca_di, &m6800_cpu_device::adda_di, &m6800_cpu_device::cmpx_di, &m6800_cpu_device::lds_di, &m6800_cpu_device::jsr_di, &m6800_cpu_device::sts_di,
    &m6800_cpu_device::suba_ix, &m6800_cpu_device::sbca_ix, &m6800_cpu_device::cmpa_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::anda_ix, &m6800_cpu_device::lda_ix, &m6800_cpu_device::bita_ix, &m6800_cpu_device::sta_ix,
    &m6800_cpu_device::eora_ix, &m6800_cpu_device::ora_ix, &m6800_cpu_device::adca_ix, &m6800_cpu_device::adda_ix, &m6800_cpu_device::cmpx_ix, &m6800_cpu_device::lds_ix, &m6800_cpu_device::jsr_ix, &m6800_cpu_device::sts_ix,
    &m6800_cpu_device::suba_ex, &m6800_cpu_device::sbca_ex, &m6800_cpu_device::cmpa_ex, &m6800_cpu_device::illegl1, &m6800_cpu_device::anda_ex, &m6800_cpu_device::lda_ex, &m6800_cpu_device::bita_ex, &m6800_cpu_device::sta_ex,
    &m6800_cpu_device::eora_ex, &m6800_cpu_device::ora_ex, &m6800_cpu_device::adca_ex, &m6800_cpu_device::adda_ex, &m6800_cpu_device::cmpx_ex, &m6800_cpu_device::lds_ex, &m6800_cpu_device::jsr_ex, &m6800_cpu_device::sts_ex,
    &m6800_cpu_device::nega, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::coma, &m6800_cpu_device::lsra, &m6800_cpu_device::rora, &m6800_cpu_device::illegl1, &m6800_cpu_device::asra,
    &m6800_cpu_device::asla, &m6800_cpu_device::deca, &m6800_cpu_device::rola, &m6800_cpu_device::illegl1, &m6800_cpu_device::inca, &m6800_cpu_device::illegl1, &m6800_cpu_device::tsta, &m6800_cpu_device::clra,
    &m6800_cpu_device::negb, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::comb, &m6800_cpu_device::lsrb, &m6800_cpu_device::rorb, &m6800_cpu_device::illegl1, &m6800_cpu_device::asrb,
    &m6800_cpu_device::aslb, &m6800_cpu_device::decb, &m6800_cpu_device::rolb, &m6800_cpu_device::illegl1, &m6800_cpu_device::incb, &m6800_cpu_device::illegl1, &m6800_cpu_device::tstb, &m6800_cpu_device::clrb,
    &m6800_cpu_device::neg_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::illegl1, &m6800_cpu_device::com_ix, &m6800_cpu_device::lsr_ix, &m6800_cpu_device::ror_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::asr_ix,
    &m6800_cpu_device::asl_ix, &m6800_cpu_device::dec_ix, &m6800_cpu_device::rol_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::inc_ix, &m6800_cpu_device::jmp_ix, &m6800_cpu_device::tst_ix, &m6800_cpu_device::clr_ix,
    &m6800_cpu_device::neg_ex, &m6800_cpu_device::illegl1, &m6800_cpu_device::stx_nsc, &m6800_cpu_device::com_ex, &m6800_cpu_device::lsr_ex, &m6800_cpu_device::ror_ex, &m6800_cpu_device::illegl1, &m6800_cpu_device::asr_ex,
    &m6800_cpu_device::asl_ex, &m6800_cpu_device::dec_ex, &m6800_cpu_device::rol_ex, &m6800_cpu_device::btst_ix, &m6800_cpu_device::inc_ex, &m6800_cpu_device::jmp_ex, &m6800_cpu_device::tst_ex, &m6800_cpu_device::clr_ex,
    &m6800_cpu_device::subb_im, &m6800_cpu_device::sbcb_im, &m6800_cpu_device::cmpb_im, &m6800_cpu_device::illegl1, &m6800_cpu_device::andb_im, &m6800_cpu_device::ldb_im, &m6800_cpu_device::bitb_im, &m6800_cpu_device::stb_im,
    &m6800_cpu_device::eorb_im, &m6800_cpu_device::orb_im, &m6800_cpu_device::adcb_im, &m6800_cpu_device::addb_im, &m6800_cpu_device::illegl1, &m6800_cpu_device::ldx_im, &m6800_cpu_device::illegl1, &m6800_cpu_device::stx_im,
    &m6800_cpu_device::subb_di, &m6800_cpu_device::sbcb_di, &m6800_cpu_device::cmpb_di, &m6800_cpu_device::illegl1, &m6800_cpu_device::andb_di, &m6800_cpu_device::ldb_di, &m6800_cpu_device::bitb_di, &m6800_cpu_device::stb_di,
    &m6800_cpu_device::eorb_di, &m6800_cpu_device::orb_di, &m6800_cpu_device::adcb_di, &m6800_cpu_device::addb_di, &m6800_cpu_device::illegl1, &m6800_cpu_device::ldx_di, &m6800_cpu_device::illegl1, &m6800_cpu_device::stx_di,
    &m6800_cpu_device::subb_ix, &m6800_cpu_device::sbcb_ix, &m6800_cpu_device::cmpb_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::andb_ix, &m6800_cpu_device::ldb_ix, &m6800_cpu_device::bitb_ix, &m6800_cpu_device::stb_ix,
    &m6800_cpu_device::eorb_ix, &m6800_cpu_device::orb_ix, &m6800_cpu_device::adcb_ix, &m6800_cpu_device::addb_ix, &m6800_cpu_device::adcx_im, &m6800_cpu_device::ldx_ix, &m6800_cpu_device::illegl1, &m6800_cpu_device::stx_ix,
    &m6800_cpu_device::subb_ex, &m6800_cpu_device::sbcb_ex, &m6800_cpu_device::cmpb_ex, &m6800_cpu_device::illegl1, &m6800_cpu_device::andb_ex, &m6800_cpu_device::ldb_ex, &m6800_cpu_device::bitb_ex, &m6800_cpu_device::stb_ex,
    &m6800_cpu_device::eorb_ex, &m6800_cpu_device::orb_ex, &m6800_cpu_device::adcb_ex, &m6800_cpu_device::addb_ex, &m6800_cpu_device::addx_ex, &m6800_cpu_device::ldx_ex, &m6800_cpu_device::illegl1, &m6800_cpu_device::stx_ex
};

m6800_cpu_device::m6800_cpu_device(MemoryMapManager* memory_map) {
    this->memory_map = memory_map;
    verbose = false;
    m_insn = m6800_insn;
    m_cycles = cycles_6800;
}

uint32_t m6800_cpu_device::RM16(uint32_t Addr) {
    uint32_t result = RM(Addr) << 8;
    return result | RM((Addr + 1) & 0xffff);
}

void m6800_cpu_device::WM16(uint32_t Addr, PAIR* p) {
    WM(Addr, p->b.h);
    WM((Addr + 1) & 0xffff, p->b.l);
}

/* IRQ enter */
void m6800_cpu_device::enter_interrupt(const char* message, uint16_t irq_vector) {
    // LOG((message));
    if (m_wai_state & (M6800_WAI | M6800_SLP)) {
        if (m_wai_state & M6800_WAI)
            m_icount -= 4;
        m_wai_state &= ~(M6800_WAI | M6800_SLP);
    } else {
        PUSHWORD(pPC);
        PUSHWORD(pX);
        PUSHBYTE(A);
        PUSHBYTE(B);
        PUSHBYTE(CC);
        m_icount -= 12;
    }
    SEI;
    PCD = RM16(irq_vector);
}

/* check the IRQ lines for pending interrupts */
void m6800_cpu_device::CHECK_IRQ_LINES() {
    // TODO: IS3 interrupt

    if (m_nmi_pending) {
        if (m_wai_state & M6800_SLP)
            m_wai_state &= ~M6800_SLP;

        m_nmi_pending = false;
        enter_interrupt("take NMI\n", 0xfffc);
    } else {
        if (m_irq_state[M6800_IRQ_LINE] != CLEAR_LINE) { /* standard IRQ */
            if (m_wai_state & M6800_SLP)
                m_wai_state &= ~M6800_SLP;

            if (!(CC & 0x10)) {
                enter_interrupt("take IRQ1\n", 0xfff8);
                // standard_irq_callback(M6800_IRQ_LINE);
            }
        } else if (!(CC & 0x10))
            m6800_check_irq2();
    }
}

void m6800_cpu_device::increment_counter(int amount) {
    m_icount -= amount;
}

void m6800_cpu_device::EAT_CYCLES() {
    increment_counter(m_icount);
}

void m6800_cpu_device::device_start() {
    m_pc.d = 0;
    m_s.d = 0;
    m_x.d = 0;
    m_d.d = 0;
    m_cc = 0;
    m_wai_state = 0;
    m_irq_state[0] = m_irq_state[1] = m_irq_state[2] = 0;

    // TODO:
    // set_icountptr(m_icount);
}

// void m6800_cpu_device::state_string_export(const device_state_entry &entry, std::string &str) const
// {
// 	switch (entry.index())
// 	{
// 		case STATE_GENFLAGS:
// 			str = string_format("%c%c%c%c%c%c%c%c",
// 				m_cc & 0x80 ? '?':'.',
// 				m_cc & 0x40 ? '?':'.',
// 				m_cc & 0x20 ? 'H':'.',
// 				m_cc & 0x10 ? 'I':'.',
// 				m_cc & 0x08 ? 'N':'.',
// 				m_cc & 0x04 ? 'Z':'.',
// 				m_cc & 0x02 ? 'V':'.',
// 				m_cc & 0x01 ? 'C':'.');
// 			break;
// 	}
// }

void m6800_cpu_device::device_reset() {
    m_cc = 0xc0;
    SEI; /* IRQ disabled */
    PCD = RM16(0xfffe);
    reset_line = 1;
    m_wai_state = 0;
    m_nmi_state = 0;
    m_nmi_pending = 0;
    m_irq_state[M6800_IRQ_LINE] = 0;
}

// void m6800_cpu_device::execute_set_input(int irqline, int state)
// {
// 	switch (irqline)
// 	{
// 	case INPUT_LINE_NMI:
// 		if (!m_nmi_state && state != CLEAR_LINE)
// 			m_nmi_pending = true;
// 		m_nmi_state = state;
// 		break;

// 	default:
// 		LOG(("set_irq_line %d,%d\n", irqline, state));
// 		m_irq_state[irqline] = state;
// 		break;
// 	}
// }

void m6800_cpu_device::pre_execute_run() {
    if (reset_line == 0) {
        device_reset();
    }

    // if (_isHalted)
    // {
    // 	if (State.Halt == 1)
    // 	{
    // 		_isHalted = false;
    // 		State.BusAvailable = 1;
    // 	}
    // 	else
    // 	{
    // 		return 5;
    // 	}
    // }
    // else
    // {
    // 	if (State.Halt == 0)
    // 	{
    // 		_isHalted = true;
    // 		State.BusAvailable = 0;
    // 	}
    // }

    // CheckInterrupts();

    // return 0;
}

/****************************************************************************
 * Execute cycles CPU cycles. Return number of cycles really executed
 ****************************************************************************/
void m6800_cpu_device::execute_run() {
    uint8_t ireg;

    CHECK_IRQ_LINES(); /* HJB 990417 */

    CLEANUP_COUNTERS();

    do {
        if (m_wai_state & (M6800_WAI | M6800_SLP)) {
            EAT_CYCLES();
        } else {
            pPPC = pPC;
            if (check_breakpoint(m_pc.d)) {
                break;
            }
            // debugger_instruction_hook(PCD);
            ireg = M_RDOP(PCD);
            PC++;
            (this->*m_insn[ireg])();
            increment_counter(m_cycles[ireg]);
        }
    } while (m_icount > 0);
}

void m6800_cpu_device::execute_step() {
    uint8_t ireg;

    CHECK_IRQ_LINES(); /* HJB 990417 */

    CLEANUP_COUNTERS();

    if (m_wai_state & (M6800_WAI | M6800_SLP)) {
        EAT_CYCLES();
    } else {
        pPPC = pPC;
        // debugger_instruction_hook(PCD);
        ireg = M_RDOP(PCD);
        PC++;
        (this->*m_insn[ireg])();
        increment_counter(m_cycles[ireg]);
    }
}

CpuStatus m6800_cpu_device::get_status() {
    return CpuStatus { m_pc.d, m_s.d, m_x.d, m_d.b.h, m_d.b.l, m_cc };
}
