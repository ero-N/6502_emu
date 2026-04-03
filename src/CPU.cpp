#include "CPU.h"
#include <iostream>

CPU::CPU(Bus *b)
	: bus(b)
{
	reset(); // 构造时直接复位
}

void CPU::reset()
{
	// 6502 真正的复位会从 0xFFFC 读取 PC 的起始地址
	// 跑测试集时我们可以直接硬编码 PC = 0x0400
	pc = 0x0400;
	A = X = Y = 0;
	stkptr = 0xFF;
	status = 0x00;
	cycls_remain = 0;
}

uint8_t CPU::fetch()
{
	return bus->read(pc++);
}

void CPU::update_NZflag(uint8_t data)
{
	if (data == 0) // Zero
		set_zero();
	else
		clr_zero();
	if (data >= 0x80) // Negative
		set_negative();
	else
		clr_negative();
}

uint16_t CPU::get_address(CPU::AddrMode mode, bool over_page_mode)
{
	switch (mode)
	{
	case AddrMode::IMMEDIATE: // 立即数，跟在指令后面
		return pc++;
	case AddrMode::ZEROPAGE: // 指令后面跟低8位地址
		return fetch() & 0x00FF;
	case AddrMode::ZEROPAGEX:
		return (fetch() + X) & 0x00FF;
	case AddrMode::ZEROPAGEY:
		return (fetch() + Y) & 0x00FF;
	case AddrMode::ABSOLUTE: // 指令后面跟完整的16位地址，先低后高
	{
		uint16_t lo = fetch();
		uint16_t hi = fetch();
		return (hi << 8) | lo;
	}
	case AddrMode::ABSOLUTEX:
	{
		uint16_t lo = fetch();
		uint16_t hi = fetch();
		// 跨页+1时钟周期
		return ((hi << 8) | lo) + X;
	}
	case AddrMode::ABSOLUTEY:
	{
		uint16_t lo = fetch();
		uint16_t hi = fetch();
		// 跨页+1时钟周期
		return ((hi << 8) | lo) + Y;
	}
	case AddrMode::INDIRECT: // 间接，指定地址的地址，从地址的地址的读16位地址
	{
		uint8_t lo = fetch();
		uint16_t hi = fetch();
		return (hi << 8) | lo;
	}
	case AddrMode::INDIRECTX: // 先+X后取值
	{
		uint8_t ptr = (fetch() + X) & 0x00FF;
		uint16_t lo = bus->read(ptr++);
		uint16_t hi = bus->read(ptr & 0x00FF);
		return (hi << 8) | lo;
	}
	case AddrMode::INDIRECTY: // 先取值后+Y
	{
		uint8_t ptr = fetch();
		uint16_t lo = bus->read(ptr++);
		uint16_t hi = bus->read(ptr & 0x00FF);
		// 跨页+1时钟周期
		return ((hi << 8) | lo) + Y;
		//   ... 其他模式
	}
	default:
		std::cout << "Illegal Address mode! \n";
		break;
	}
	return 0;
}

void CPU::step()
{
	opcode = fetch(); // 操作码

	addr_mode = OPTABL[opcode].addr_mode;
	cycls_remain += OPTABL[opcode].cycles;
	if (addr_mode != AddrMode::ACCUMULATOR && addr_mode != AddrMode::IMPLIED)
		data_address = get_address(addr_mode, OPTABL[opcode].over_page_mode);
	(this->*OPTABL[opcode].name)();
}

void CPU::set_carry()
{
	status |= 1;
}

void CPU::clr_carry()
{
	status &= 0xFE;
}

bool CPU::get_carry()
{
	return status & 1;
}

void CPU::set_overflow()
{
	status |= 0x40;
}

void CPU::clr_overflow()
{
	status &= 0xBF;
}

bool CPU::get_overflow()
{
	return status & 0x40;
}

void CPU::set_zero()
{
	status |= 0x02;
}

void CPU::clr_zero()
{
	status &= 0xFD;
}

bool CPU::get_zero()
{
	return status & 0x02;
}

void CPU::set_negative()
{
	status |= 0x80;
}

void CPU::clr_negative()
{
	status &= 0x7F;
}

bool CPU::get_negative()
{
	return status & 0x80;
}

void CPU::set_interrupt()
{
	status |= 0x04;
}

void CPU::clr_interrupt()
{
	status &= 0xFB;
}

bool CPU::get_interrupt()
{
	return status & 0x04;
}

void CPU::set_decimal()
{
	status |= 0x08;
}

void CPU::clr_decimal()
{
	status &= 0xF7;
}

bool CPU::get_decimal()
{
	return status & 0x08;
}

void CPU::push_stack(uint8_t data)
{
	bus->write(0x0100 | stkptr--, data);
}

uint8_t CPU::pop_stack()
{
	return bus->read(0x0100 | ++stkptr);
}

void CPU::ADC()
{
	uint16_t data;
	uint16_t res;

	if (get_decimal())
	{
		// To be done
	}
	else
	{
		data = bus->read(data_address);
		res = data + A + get_carry();
		if (res > 0x00FF)
			set_carry();
		else
			clr_carry();
		if (((uint16_t)A ^ res) & (~((uint16_t)A ^ data)) & 0x0080) // 只有A和data符号相同，res符号不同的时候overflow
			set_overflow();
		else
			clr_overflow();
		A = res;
		update_NZflag(A);
	}
}

void CPU::AND()
{
	A |= bus->read(data_address);
	update_NZflag(A);
}

void CPU::ASL()
{
	if (addr_mode != AddrMode::ACCUMULATOR)
		A = bus->read(data_address);
	if (A >> 7)
		set_carry();
	else
		clr_carry();
	A <<= 1;
	update_NZflag(A);
	if (addr_mode != AddrMode::ACCUMULATOR)
		bus->write(data_address, A);
}

void CPU::BIT()
{
	uint16_t data;

	data = bus->read(data_address);
	if (A | data)
		clr_zero();
	else
		set_zero();

	if (data >> 6)
		set_negative();
	else
		clr_negative();

	if (data >> 5)
		set_overflow();
	else
		clr_overflow();
}

void CPU::BPL()
{
	if (!get_negative())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BMI()
{
	if (get_negative())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BVC()
{
	if (!get_overflow())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BVS()
{
	if (get_overflow())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BCC()
{
	if (!get_carry())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BCS()
{
	if (get_carry())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BNE()
{
	if (!get_zero())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BEQ()
{
	if (get_zero())
	{
		int8_t offset = (int8_t)bus->read(data_address);
		uint16_t old_pc = pc;

		cycls_remain++; // 条件成立+1时钟周期
		pc += offset;
		if ((pc & 0xFF00) != (old_pc & 0xFF00)) // 跨页+1时钟周期
			cycls_remain++;
	}
}

void CPU::BRK()
{
	pc++;

	// PC入栈
	push_stack(pc >> 8);
	push_stack(pc & 0x00FF);

	// 状态寄存器压栈，B标志位设为1,Bit 4/5 置 1
	push_stack(status | 0x30);

	// 设置中断屏蔽
	set_interrupt();

	uint8_t lo = bus->read(0xFFFE);
	uint8_t hi = bus->read(0xFFFF);
	pc = ((uint16_t)hi << 8) | lo;
}

void CPU::CMP()
{
	uint8_t data;

	data = bus->read(data_address);
	if (A >= data)
		set_carry();
	else
		clr_carry();

	update_NZflag(A - data); // A-data的结果更新N,Z
}

void CPU::CPX()
{
	uint8_t data;

	data = bus->read(data_address);
	if (X >= data)
		set_carry();
	else
		clr_carry();

	update_NZflag(X - data); // X-data的结果更新N,Z
}

void CPU::CPY()
{
	uint8_t data;

	data = bus->read(data_address);
	if (Y >= data)
		set_carry();
	else
		clr_carry();

	update_NZflag(Y - data); // Y-data的结果更新N,Z
}

void CPU::DEC()
{
	uint8_t data;

	data = bus->read(data_address);
	data--;
	bus->write(data_address, data);

	update_NZflag(data);
}

void CPU::EOR()
{
	A ^= bus->read(data_address);
	update_NZflag(A);
}

void CPU::CLC()
{
	clr_carry();
}

void CPU::SEC()
{
	set_carry();
}

void CPU::CLI()
{
	clr_interrupt();
}

void CPU::SEI()
{
	set_interrupt();
}

void CPU::CLV()
{
	clr_overflow();
}

void CPU::CLD()
{
	clr_decimal();
}

void CPU::SED()
{
	set_decimal();
}

void CPU::INC()
{
	uint8_t data;

	data = bus->read(data_address);
	data++;
	bus->write(data_address, data);

	update_NZflag(data);
}

void CPU::JMP()
{
	if (addr_mode == AddrMode::ABSOLUTE) // 绝对寻址
	{
		pc = data_address;
	}
	else // 间接寻址
	{
		uint8_t lo = bus->read(data_address++);
		uint8_t hi = bus->read((data_address & 0x00FF) ? data_address : data_address - 0x0100);
		pc = ((uint16_t)hi << 8) | lo;
	}
}

void CPU::JSR()
{
	pc--;
	push_stack(pc >> 8);
	push_stack(pc & 0x00FF);
	pc = data_address;
}

void CPU::LDA()
{
	A = bus->read(data_address);
	update_NZflag(A);
}

void CPU::LDX()
{
	X = bus->read(data_address);

	update_NZflag(X);
}

void CPU::LDY()
{
	Y = bus->read(data_address);

	update_NZflag(Y);
}

void CPU::LSR()
{
	if (addr_mode != AddrMode::ACCUMULATOR)
		A = bus->read(data_address);

	if (A & 1)
		set_carry();
	else
		clr_carry();

	A >>= 1;
	update_NZflag(A);

	if (addr_mode != AddrMode::ACCUMULATOR)
		bus->write(data_address, A);
}

void CPU::NOP()
{
}

void CPU::ORA()
{
	A |= bus->read(data_address);
	update_NZflag(A);
}

void CPU::TAX()
{
	X = A;
	update_NZflag(X);
}

void CPU::TXA()
{
	A = X;
	update_NZflag(A);
}

void CPU::DEX()
{
	X--;
	update_NZflag(X);
}

void CPU::INX()
{
	X++;
	update_NZflag(X);
}

void CPU::TAY()
{
	Y = A;
	update_NZflag(Y);
}

void CPU::TYA()
{
	A = Y;
	update_NZflag(A);
}

void CPU::DEY()
{
	Y--;
	update_NZflag(Y);
}

void CPU::INY()
{
	Y++;
	update_NZflag(Y);
}

void CPU::ROL()
{
	uint8_t old_carry;

	if (addr_mode != AddrMode::ACCUMULATOR)
		A = bus->read(data_address);

	old_carry = get_carry();
	if (A >> 7)
		set_carry();
	else
		clr_carry();
	A = (A << 1) | old_carry;
	update_NZflag(A);

	if (addr_mode != AddrMode::ACCUMULATOR)
		bus->write(data_address, A);
}

void CPU::ROR()
{
	uint8_t old_carry;

	if (addr_mode != AddrMode::ACCUMULATOR)
		A = bus->read(data_address);

	old_carry = get_carry();
	if (A & 1)
		set_carry();
	else
		clr_carry();
	A = (old_carry << 7) | (A >> 1);
	update_NZflag(A);

	if (addr_mode != AddrMode::ACCUMULATOR)
		bus->write(data_address, A);
}

void CPU::RTI()
{
	uint8_t lo, hi;

	status = pop_stack() & 0xCF; // 0xCF = 1100 1111 (去掉 Bit 4 和 Bit 5)
	lo = pop_stack();
	hi = pop_stack();
	pc = ((uint16_t)hi << 8) | lo;
}

void CPU::RTS()
{
	uint8_t lo, hi;
	lo = pop_stack();
	hi = pop_stack();
	pc = (((uint16_t)hi << 8) | lo) + 1;
}

void CPU::SBC()
{
	uint16_t data, res;

	data = bus->read(data_address);

	if (get_decimal())
	{
		// To be done
	}
	else
	{
		// A-M-(1-C) = A + (~M) + C
		res = (uint16_t)A + (~data) + get_carry();
		// 正数-负数=负数 或者 负数-正数=正数
		if ((A ^ data) & (A ^ res) & 0x80)
			set_overflow();
		else
			clr_overflow();
		if (res > 0x00FF)
			set_carry();
		else
			clr_carry();
		A = res;
		update_NZflag(A);
	}
}

void CPU::STA()
{
	bus->write(data_address, A);
}

void CPU::TXS()
{
	stkptr = X;
}

void CPU::TSX()
{
	X = stkptr;
	update_NZflag(X);
}

void CPU::PHA()
{
	push_stack(A);
}

void CPU::PLA()
{
	A = pop_stack();
	update_NZflag(A);
}

void CPU::PHP()
{
	push_stack(status | 0x30);
}

void CPU::PLP()
{
	status = pop_stack() & 0xCF; // 0xCF = 1100 1111 (去掉 Bit 4 和 Bit 5)
}

void CPU::STX()
{
	bus->write(data_address, X);
}

void CPU::STY()
{
	bus->write(data_address, Y);
}

void CPU::ERR()
{
	std::cout << "Illegal instruction!\n";
}

uint8_t CPU::get_A()
{
	return A;
}

uint8_t CPU::get_X()
{
	return X;
}

uint8_t CPU::get_Y()
{
	return Y;
}

uint8_t CPU::get_status()
{
	return status;
}

uint8_t CPU::get_opcode()
{
	return opcode;
}

uint16_t CPU::get_PC()
{
	return pc;
}