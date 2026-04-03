#include "Bus.h"
#include "CPU.h"
#include <iostream>
#ifdef DEBUG
#include <queue>
struct node
{
	uint16_t pc;
	uint8_t opcode, A, X, Y, status;
};

#endif

int main(int argc, char **argv)
{

	if (argc < 1)
	{
		std::cout << "Please input the bin file!\n";
		return 0;
	}
	Bus bus;
	CPU cpu(&bus);
	cpu.reset();

#ifdef DEBUG
	std::queue<node> buf;
#endif

	std::string path = argv[1];

	// TODO: 这里写读取 .bin 文件到 bus.ram 的代码
	bool flg = bus.load_bin(path, 0x0000);
	if (flg)
	{
		while (true)
		{

#ifdef DEBUG
			uint16_t pc = cpu.get_PC();
#endif
			cpu.step();

#ifdef DEBUG
			buf.push({pc, cpu.get_opcode(), cpu.get_A(), cpu.get_X(), cpu.get_Y(), cpu.get_status()});
			if (buf.size() > 100)
				buf.pop();
			if (pc == cpu.get_PC())
			{
				while (buf.size() > 0)
				{
					node tmp;

					tmp = buf.front();
					buf.pop();
					std::cout << std::hex << std::showbase << std::uppercase << tmp.pc << " " << (uint16_t)tmp.opcode << " " << (uint16_t)tmp.A << " " << (uint16_t)tmp.X << " " << (uint16_t)tmp.Y << " " << (uint16_t)tmp.status << "\n";
				}
				system("pause");
			}
#endif
		}
	}
	else
		std::cout << "Can't open" << path << "!\n";
	return 0;
}