#ifndef BUS_H
#define BUS_H
#include <array>
#include <cstdint>

class Bus
{
public:
	Bus() { ram.fill(0x00); }

	// 基础读写
	uint8_t read(uint16_t);
	void write(uint16_t, uint8_t);
	bool load_bin(const std::string &, uint16_t);

private:
	std::array<uint8_t, 64 * 1024> ram; // 64KB 内存
};

#endif