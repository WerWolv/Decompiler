#include <disasm/disassembler.hpp>
#include <disasm/ARM/instructions.hpp>

int main() {
    std::array<dc::u8, 4> bytes = { 0b0101'1011, 0b0000'1111, 0b0101'1010, 0b1111'0000 };

    for (const auto &line : dc::disasm::disassemble<dc::disasm::arm::Instructions>(bytes)) {
        printf("%s\n", line.c_str());
    }
}