#include <Bus.h>
#include <fstream>
#include <vector>

uint8_t Bus::read(uint16_t addr)
{
	return ram[addr];
}

void Bus::write(uint16_t addr, uint8_t data)
{
	ram[addr] = data;
}

bool Bus::load_bin(const std::string &path, uint16_t target_addr)
{
	// 必须用 std::ios::binary 模式打开
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file.is_open())
		return false;

	std::streamsize size = file.tellg(); // 获取文件大小
	file.seekg(0, std::ios::beg);		 // 回到文件开头

	// 读取到临时 buffer
	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size))
	{
		// 将数据拷贝进 std::array<uint8_t, 65536> ram
		for (int i = 0; i < size; ++i)
		{
			if (target_addr + i < ram.size())
			{
				ram[target_addr + i] = static_cast<uint8_t>(buffer[i]);
			}
		}
		return true;
	}
	return false;
}