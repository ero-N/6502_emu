#ifndef CPU_H
#define CPU_H

#include "Bus.h"
#include <cstdint>

#define DEBUG

class CPU
{
public:
	enum class AddrMode
	{
		ACCUMULATOR,
		IMMEDIATE,
		ZEROPAGE,
		ZEROPAGEX,
		ZEROPAGEY,
		ABSOLUTE,
		ABSOLUTEX,
		ABSOLUTEY,
		INDIRECT,
		INDIRECTX,
		INDIRECTY,
		IMPLIED,
	};

	// 定义成员函数指针类型
	// 说明：void 表示不返回值，(CPU::*) 表示这是 CPU 类的成员函数
	typedef void (CPU::*OpFunction)();

	CPU(Bus *);

	void reset(); // 复位
	void step();  // 执行一跳指令
	struct Opcode
	{
		OpFunction name;		 // 指令函数，如 &CPU::LDA
		CPU::AddrMode addr_mode; // 寻址模式函数，如 AddrMode::IMMEDIATE
		uint8_t cycles;			 // 该指令消耗的周期
		bool over_page_mode;	 // 是否跨页会+1周期
	};

	// 预填充 256 个指令映射表
	const Opcode OPTABL[256] = {
		/* 0x00 */ {&CPU::BRK, AddrMode::IMPLIED, 7, 0},
		/* 0x01 */ {&CPU::ORA, AddrMode::INDIRECTX, 6, 0},
		/* 0x02 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x03 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x04 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x05 */ {&CPU::ORA, AddrMode::ZEROPAGE, 3, 0},
		/* 0x06 */ {&CPU::ASL, AddrMode::ZEROPAGE, 5, 0},
		/* 0x07 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x08 */ {&CPU::PHP, AddrMode::IMPLIED, 3, 0},
		/* 0x09 */ {&CPU::ORA, AddrMode::IMMEDIATE, 2, 0},
		/* 0x0A */ {&CPU::ASL, AddrMode::ACCUMULATOR, 2, 0},
		/* 0x0B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x0C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x0D */ {&CPU::ORA, AddrMode::ABSOLUTE, 4, 0},
		/* 0x0E */ {&CPU::ASL, AddrMode::ABSOLUTEX, 6, 0},
		/* 0x0F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x10 */ {&CPU::BPL, AddrMode::IMMEDIATE, 2, 0},
		/* 0x11 */ {&CPU::ORA, AddrMode::INDIRECTY, 5, 1},
		/* 0x12 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x13 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x14 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x15 */ {&CPU::ORA, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x16 */ {&CPU::ASL, AddrMode::ZEROPAGEX, 6, 0},
		/* 0x17 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x18 */ {&CPU::CLC, AddrMode::IMPLIED, 2, 0},
		/* 0x19 */ {&CPU::ORA, AddrMode::ABSOLUTEY, 4, 1},
		/* 0x1A */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x1B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x1C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x1D */ {&CPU::ORA, AddrMode::ABSOLUTEX, 4, 1},
		/* 0x1E */ {&CPU::ASL, AddrMode::ABSOLUTEY, 7, 0},
		/* 0x1F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x20 */ {&CPU::JSR, AddrMode::ABSOLUTE, 6, 0},
		/* 0x21 */ {&CPU::AND, AddrMode::INDIRECTX, 6, 0},
		/* 0x22 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x23 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x24 */ {&CPU::BIT, AddrMode::ZEROPAGE, 3, 0},
		/* 0x25 */ {&CPU::AND, AddrMode::ZEROPAGE, 3, 0},
		/* 0x26 */ {&CPU::ROL, AddrMode::ZEROPAGE, 5, 0},
		/* 0x27 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x28 */ {&CPU::PLP, AddrMode::IMPLIED, 4, 0},
		/* 0x29 */ {&CPU::AND, AddrMode::IMMEDIATE, 2, 0},
		/* 0x2A */ {&CPU::ROL, AddrMode::ACCUMULATOR, 2, 0},
		/* 0x2B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x2C */ {&CPU::BIT, AddrMode::ABSOLUTE, 4, 0},
		/* 0x2D */ {&CPU::AND, AddrMode::ABSOLUTE, 4, 0},
		/* 0x2E */ {&CPU::ROL, AddrMode::ABSOLUTE, 6, 0},
		/* 0x2F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x30 */ {&CPU::BMI, AddrMode::IMMEDIATE, 2, 0},
		/* 0x31 */ {&CPU::AND, AddrMode::INDIRECTY, 5, 1},
		/* 0x32 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x33 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x34 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x35 */ {&CPU::AND, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x36 */ {&CPU::ROL, AddrMode::ZEROPAGEX, 6, 0},
		/* 0x37 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x38 */ {&CPU::SEC, AddrMode::IMPLIED, 2, 0},
		/* 0x39 */ {&CPU::AND, AddrMode::ABSOLUTEY, 4, 1},
		/* 0x3A */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x3B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x3C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x3D */ {&CPU::AND, AddrMode::ABSOLUTEX, 4, 1},
		/* 0x3E */ {&CPU::ROL, AddrMode::ABSOLUTEX, 7, 0},
		/* 0x3F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x40 */ {&CPU::RTI, AddrMode::IMPLIED, 6, 0},
		/* 0x41 */ {&CPU::EOR, AddrMode::INDIRECTX, 6, 0},
		/* 0x42 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x43 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x44 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x45 */ {&CPU::EOR, AddrMode::ZEROPAGE, 3, 0},
		/* 0x46 */ {&CPU::LSR, AddrMode::ZEROPAGE, 5, 0},
		/* 0x47 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x48 */ {&CPU::PHA, AddrMode::IMPLIED, 3, 0},
		/* 0x49 */ {&CPU::EOR, AddrMode::IMMEDIATE, 2, 0},
		/* 0x4A */ {&CPU::LSR, AddrMode::ACCUMULATOR, 2, 0},
		/* 0x4B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x4C */ {&CPU::JMP, AddrMode::ABSOLUTE, 3, 0},
		/* 0x4D */ {&CPU::EOR, AddrMode::ABSOLUTE, 4, 0},
		/* 0x4E */ {&CPU::LSR, AddrMode::ABSOLUTE, 6, 0},
		/* 0x4F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x50 */ {&CPU::BVC, AddrMode::IMMEDIATE, 2, 0},
		/* 0x51 */ {&CPU::EOR, AddrMode::INDIRECTY, 5, 1},
		/* 0x52 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x53 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x54 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x55 */ {&CPU::EOR, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x56 */ {&CPU::LSR, AddrMode::ZEROPAGEX, 6, 0},
		/* 0x57 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x58 */ {&CPU::CLI, AddrMode::IMPLIED, 2, 0},
		/* 0x59 */ {&CPU::EOR, AddrMode::ABSOLUTEY, 4, 1},
		/* 0x5A */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x5B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x5C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x5D */ {&CPU::EOR, AddrMode::ABSOLUTEX, 4, 1},
		/* 0x5E */ {&CPU::LSR, AddrMode::ABSOLUTEX, 7, 0},
		/* 0x5F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x60 */ {&CPU::RTS, AddrMode::IMPLIED, 6, 0},
		/* 0x61 */ {&CPU::ADC, AddrMode::INDIRECTX, 6, 0},
		/* 0x62 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x63 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x64 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x65 */ {&CPU::ADC, AddrMode::ZEROPAGE, 3, 0},
		/* 0x66 */ {&CPU::ROR, AddrMode::ZEROPAGE, 5, 0},
		/* 0x67 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x68 */ {&CPU::PLA, AddrMode::IMPLIED, 4, 0},
		/* 0x69 */ {&CPU::ADC, AddrMode::IMMEDIATE, 2, 0},
		/* 0x6A */ {&CPU::ROR, AddrMode::ACCUMULATOR, 2, 0},
		/* 0x6B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x6C */ {&CPU::JMP, AddrMode::INDIRECT, 5, 0},
		/* 0x6D */ {&CPU::ADC, AddrMode::ABSOLUTE, 4, 0},
		/* 0x6E */ {&CPU::ROR, AddrMode::ABSOLUTE, 6, 0},
		/* 0x6F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x70 */ {&CPU::BVS, AddrMode::IMMEDIATE, 2, 0},
		/* 0x71 */ {&CPU::ADC, AddrMode::INDIRECTY, 5, 1},
		/* 0x72 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x73 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x74 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x75 */ {&CPU::ADC, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x76 */ {&CPU::ROR, AddrMode::ZEROPAGEX, 6, 0},
		/* 0x77 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x78 */ {&CPU::SEI, AddrMode::IMPLIED, 2, 0},
		/* 0x79 */ {&CPU::ADC, AddrMode::ABSOLUTEY, 4, 1},
		/* 0x7A */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x7B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x7C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x7D */ {&CPU::ADC, AddrMode::ABSOLUTEX, 4, 1},
		/* 0x7E */ {&CPU::ROR, AddrMode::ABSOLUTEX, 7, 0},
		/* 0x7F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x80 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x81 */ {&CPU::STA, AddrMode::INDIRECTX, 6, 0},
		/* 0x82 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x83 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x84 */ {&CPU::STY, AddrMode::ZEROPAGE, 3, 0},
		/* 0x85 */ {&CPU::STA, AddrMode::ZEROPAGE, 3, 0},
		/* 0x86 */ {&CPU::STX, AddrMode::ZEROPAGE, 3, 0},
		/* 0x87 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x88 */ {&CPU::DEY, AddrMode::IMPLIED, 2, 0},
		/* 0x89 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x8A */ {&CPU::TXA, AddrMode::IMPLIED, 2, 0},
		/* 0x8B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x8C */ {&CPU::STY, AddrMode::ABSOLUTE, 4, 0},
		/* 0x8D */ {&CPU::STA, AddrMode::ABSOLUTE, 4, 0},
		/* 0x8E */ {&CPU::STX, AddrMode::ABSOLUTE, 4, 0},
		/* 0x8F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x90 */ {&CPU::BCC, AddrMode::IMMEDIATE, 2, 0},
		/* 0x91 */ {&CPU::STA, AddrMode::INDIRECTY, 6, 0},
		/* 0x92 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x93 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x94 */ {&CPU::STY, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x95 */ {&CPU::STA, AddrMode::ZEROPAGEX, 4, 0},
		/* 0x96 */ {&CPU::STX, AddrMode::ZEROPAGEY, 4, 0},
		/* 0x97 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x98 */ {&CPU::TYA, AddrMode::IMPLIED, 2, 0},
		/* 0x99 */ {&CPU::STA, AddrMode::ABSOLUTEY, 5, 0},
		/* 0x9A */ {&CPU::TXS, AddrMode::IMPLIED, 2, 0},
		/* 0x9B */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x9C */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x9D */ {&CPU::STA, AddrMode::ABSOLUTEX, 5, 0},
		/* 0x9E */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0x9F */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xA0 */ {&CPU::LDY, AddrMode::IMMEDIATE, 2, 0},
		/* 0xA1 */ {&CPU::LDA, AddrMode::INDIRECTX, 6, 0},
		/* 0xA2 */ {&CPU::LDX, AddrMode::IMMEDIATE, 2, 0},
		/* 0xA3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xA4 */ {&CPU::LDY, AddrMode::ZEROPAGE, 3, 0},
		/* 0xA5 */ {&CPU::LDA, AddrMode::ZEROPAGE, 3, 0},
		/* 0xA6 */ {&CPU::LDX, AddrMode::ZEROPAGE, 3, 0},
		/* 0xA7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xA8 */ {&CPU::TAY, AddrMode::IMPLIED, 2, 0},
		/* 0xA9 */ {&CPU::LDA, AddrMode::IMMEDIATE, 2, 0},
		/* 0xAA */ {&CPU::TAX, AddrMode::IMPLIED, 2, 0},
		/* 0xAB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xAC */ {&CPU::LDY, AddrMode::ABSOLUTE, 4, 0},
		/* 0xAD */ {&CPU::LDA, AddrMode::ABSOLUTE, 4, 0},
		/* 0xAE */ {&CPU::LDX, AddrMode::ABSOLUTE, 4, 0},
		/* 0xAF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xB0 */ {&CPU::BCS, AddrMode::IMMEDIATE, 2, 0},
		/* 0xB1 */ {&CPU::LDA, AddrMode::INDIRECTY, 5, 1},
		/* 0xB2 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xB3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xB4 */ {&CPU::LDY, AddrMode::ZEROPAGEX, 4, 0},
		/* 0xB5 */ {&CPU::LDA, AddrMode::ZEROPAGEX, 4, 0},
		/* 0xB6 */ {&CPU::LDX, AddrMode::ZEROPAGEY, 4, 0},
		/* 0xB7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xB8 */ {&CPU::CLV, AddrMode::IMPLIED, 2, 0},
		/* 0xB9 */ {&CPU::LDA, AddrMode::ABSOLUTEY, 4, 1},
		/* 0xBA */ {&CPU::TSX, AddrMode::IMPLIED, 2, 0},
		/* 0xBB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xBC */ {&CPU::LDY, AddrMode::ABSOLUTEX, 4, 1},
		/* 0xBD */ {&CPU::LDA, AddrMode::ABSOLUTEX, 4, 1},
		/* 0xBE */ {&CPU::LDX, AddrMode::ABSOLUTEY, 4, 1},
		/* 0xBF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xC0 */ {&CPU::CPY, AddrMode::IMMEDIATE, 2, 0},
		/* 0xC1 */ {&CPU::CMP, AddrMode::INDIRECTX, 6, 0},
		/* 0xC2 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xC3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xC4 */ {&CPU::CPY, AddrMode::ZEROPAGE, 3, 0},
		/* 0xC5 */ {&CPU::CMP, AddrMode::ZEROPAGE, 3, 0},
		/* 0xC6 */ {&CPU::DEC, AddrMode::ZEROPAGE, 5, 0},
		/* 0xC7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xC8 */ {&CPU::INY, AddrMode::IMPLIED, 2, 0},
		/* 0xC9 */ {&CPU::CMP, AddrMode::IMMEDIATE, 2, 0},
		/* 0xCA */ {&CPU::DEX, AddrMode::IMPLIED, 2, 0},
		/* 0xCB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xCC */ {&CPU::CPY, AddrMode::ABSOLUTE, 4, 0},
		/* 0xCD */ {&CPU::CMP, AddrMode::ABSOLUTE, 4, 0},
		/* 0xCE */ {&CPU::DEC, AddrMode::ABSOLUTE, 6, 0},
		/* 0xCF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xD0 */ {&CPU::BNE, AddrMode::IMMEDIATE, 2, 0},
		/* 0xD1 */ {&CPU::CMP, AddrMode::INDIRECTY, 5, 1},
		/* 0xD2 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xD3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xD4 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xD5 */ {&CPU::CMP, AddrMode::ZEROPAGEX, 4, 0},
		/* 0xD6 */ {&CPU::DEC, AddrMode::ZEROPAGEX, 6, 0},
		/* 0xD7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xD8 */ {&CPU::CLD, AddrMode::IMPLIED, 2, 0},
		/* 0xD9 */ {&CPU::CMP, AddrMode::ABSOLUTEY, 4, 1},
		/* 0xDA */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xDB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xDC */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xDD */ {&CPU::CMP, AddrMode::ABSOLUTEX, 4, 1},
		/* 0xDE */ {&CPU::DEC, AddrMode::ABSOLUTEX, 7, 0},
		/* 0xDF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE0 */ {&CPU::CPX, AddrMode::IMMEDIATE, 2, 0},
		/* 0xE1 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE2 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE4 */ {&CPU::CPX, AddrMode::ZEROPAGE, 3, 0},
		/* 0xE5 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE6 */ {&CPU::INC, AddrMode::ZEROPAGE, 5, 0},
		/* 0xE7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xE8 */ {&CPU::INX, AddrMode::IMPLIED, 2, 0},
		/* 0xE9 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xEA */ {&CPU::NOP, AddrMode::IMPLIED, 2, 0},
		/* 0xEB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xEC */ {&CPU::CPX, AddrMode::ABSOLUTE, 4, 0},
		/* 0xED */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xEE */ {&CPU::INC, AddrMode::ABSOLUTE, 6, 0},
		/* 0xEF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF0 */ {&CPU::BEQ, AddrMode::IMMEDIATE, 2, 0},
		/* 0xF1 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF2 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF3 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF4 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF5 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF6 */ {&CPU::INC, AddrMode::ZEROPAGEX, 6, 0},
		/* 0xF7 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xF8 */ {&CPU::SED, AddrMode::IMPLIED, 2, 0},
		/* 0xF9 */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xFA */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xFB */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xFC */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xFD */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
		/* 0xFE */ {&CPU::INC, AddrMode::ABSOLUTEX, 7, 0},
		/* 0xFF */ {&CPU::ERR, AddrMode::IMPLIED, 1, 0},
	};
	uint8_t get_A();
	uint8_t get_X();
	uint8_t get_Y();
	uint8_t get_status();
	uint8_t get_opcode();
	uint16_t get_PC();

private:
	Bus *bus; // 连接的总线

	// 寄存器
	uint8_t A, X, Y;	   // 累加器和变址寄存器
	uint8_t stkptr;		   // 栈指针$0100–$01FF
	uint16_t pc;		   // 程序计数器 (Program Counter)
	uint8_t status;		   // 状态标志位 (N, V, -, B, D, I, Z, C)
	uint8_t cycls_remain;  // 剩余时钟周期
	uint16_t data_address; // 操作数据的地址
	AddrMode addr_mode;	   // 当前操作的寻址模式
	uint8_t opcode;

	// 辅助函数
	uint8_t fetch();						   // 取指并移动 PC
	uint16_t get_address(CPU::AddrMode, bool); // 计算地址
	void update_NZflag(uint8_t);
	void set_carry();
	void clr_carry();
	bool get_carry();
	void set_overflow();
	void clr_overflow();
	bool get_overflow();
	void set_zero();
	void clr_zero();
	bool get_zero();
	void set_negative();
	void clr_negative();
	bool get_negative();
	void set_interrupt();
	void clr_interrupt();
	bool get_interrupt();
	void set_decimal();
	void clr_decimal();
	bool get_decimal();
	void push_stack(uint8_t);
	uint8_t pop_stack();
	void ADC(); // ADd with Carry
	void AND(); // bitwise AND with accumulator
	void ASL(); // Arithmetic Shift Left
	void BIT(); // test BITs
	void BPL(); // Branch on PLus
	void BMI(); // Branch on MInus
	void BVC(); // Branch on oVerflow Clear
	void BVS(); // Branch on oVerflow Set
	void BCC(); // Branch on Carry Clear
	void BCS(); // Branch on Carry Set
	void BNE(); // Branch on Not Equal
	void BEQ(); // Branch on EQual
	void BRK(); // BReaK
	void CMP(); // CoMPare accumulator
	void CPX(); // ComPare X register
	void CPY(); // ComPare Y register
	void DEC(); // DECrement memory
	void EOR(); // bitwise Exclusive OR
	void CLC(); // CLear Carry
	void SEC(); // SEt Carry
	void CLI(); // CLear Interrupt
	void SEI(); // SEt Interrupt
	void CLV(); // CLear oVerflow
	void CLD(); // CLear Decimal
	void SED(); // SEt Decimal
	void INC(); // INCrement memory
	void JMP(); // JuMP
	void JSR(); // Jump to SubRoutine
	void LDA(); // LoaD Accumulator
	void LDX(); // LoaD X register
	void LDY(); // LoaD Y register
	void LSR(); // Logical Shift Right
	void NOP(); // No OPeration
	void ORA(); // bitwise OR with Accumulator
	void TAX(); // Transfer A to X
	void TXA(); // Transfer X to A
	void DEX(); // DEcrement X
	void INX(); // INcrement X
	void TAY(); // Transfer A to Y
	void TYA(); // Transfer Y to A
	void DEY(); // DEcrement Y
	void INY(); // INcrement Y
	void ROL(); // ROtate Left
	void ROR(); // ROtate Right
	void RTI(); // ReTurn from Interrupt
	void RTS(); // ReTurn from Subroutine
	void SBC(); // SuBtract with Carry
	void STA(); // STore Accumulator
	void TXS(); // Transfer X to Stack ptr
	void TSX(); // Transfer Stack ptr to X
	void PHA(); // PusH Accumulator
	void PLA(); // PuLl Accumulator
	void PHP(); // PusH Processor status
	void PLP(); // PuLl Processor status
	void STX(); // STore X register
	void STY(); // STore Y register
	void ERR(); // Illegal instruction
};

#endif