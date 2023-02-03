#pragma once

#include <span>
#include <vector>
#include <tuple>
#include <string>
#include <disasm/instruction.hpp>

namespace dc::disasm::i8051 {

    using namespace dc::ast;
    
    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue, Category Category>
    struct Instruction8051Base : public Instruction<MnemonicValue, PatternValue, Category, std::endian::big> { };

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue, Category Category>
    struct Instruction8051 : public Instruction8051Base<MnemonicValue, PatternValue, Category> {
        constexpr static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return asVector(create<ASTNodeAssembly>(fmt::format("{} {}", MnemonicValue, disassemble(address, bytes))));
        }
    };

    struct InstrNop : public Instruction8051<"nop", "0000'0000", Category::Other> {
        constexpr static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "";
        }

        constexpr static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return { };
        }
    };

    struct InstrAJmp : public Instruction8051<"ajmp", "ppp0'0001'aaaa'aaaa", Category::UnconditionalJump> {
        using a = Placeholder<'a'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", a::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                create<ASTNodeJump>(create<ASTNodeIntegerLiteral>(a::get(bytes)))
            );
        }
    };

    struct InstrLJmp : public Instruction8051<"ljmp", "0000'0010'aaaa'aaaa'aaaa'aaaa", Category::UnconditionalJump> {
        using a = Placeholder<'a'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", a::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeJump>(create<ASTNodeIntegerLiteral>(a::get(bytes)))
            );
        }
    };

    struct InstrSJmp : public Instruction8051<"sjmp", "1000'0000'aaaa'aaaa", Category::UnconditionalJump> {
        using a = Placeholder<'a'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", address + a::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeJump>(create<ASTNodeIntegerLiteral>(address + a::get(bytes)))
            );
        }
    };

    struct InstrRR : public Instruction8051<"rr", "0000'0011", Category::Arithmetic> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "A";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>("A"),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::RotateRight
                            ),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrIncR : public Instruction8051<"inc", "0000'1nnn", Category::RegisterAccess> {
        using n = Placeholder<'n'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("R{}", n::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Add
                            ),
                            create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes)))
                    )
            );
        }
    };

    struct InstrIncDPTR : public Instruction8051<"inc", "1010'0011", Category::RegisterAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("DPTR");
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>("DPTR"),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Add
                            ),
                            create<ASTNodeRegister>("DPTR")
                    )
            );
        }
    };

    struct InstrIncA : public Instruction8051<"inc", "0000'0100", Category::RegisterAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "A";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>("A"),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Add
                            ),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrIncDirect : public Instruction8051<"inc", "0000'0101'dddd'dddd", Category::MemoryAccess> {
        using d = Placeholder<'d'>;
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", d::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeUnaryArithmetic>(create<ASTNodeIntegerLiteral>(d::get(bytes)), ASTNodeUnaryArithmetic::Operator::Dereference),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Add
                            ),
                            create<ASTNodeUnaryArithmetic>(create<ASTNodeIntegerLiteral>(d::get(bytes)), ASTNodeUnaryArithmetic::Operator::Dereference)
                    )
            );
        }
    };

    struct InstrIncIndirect : public Instruction8051<"inc", "0000'011i", Category::MemoryAccess> {
        using i = Placeholder<'i'>;
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("@R{}", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeUnaryArithmetic>(create<ASTNodeRegister>(fmt::format("R{}", i::get(bytes))), ASTNodeUnaryArithmetic::Operator::Dereference),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Add
                            ),
                            create<ASTNodeUnaryArithmetic>(create<ASTNodeRegister>(fmt::format("R{}", i::get(bytes))), ASTNodeUnaryArithmetic::Operator::Dereference)
                    )
            );
        }
    };

    struct InstrJC : public Instruction8051<"jc", "0100'0000'oooo'oooo", Category::ConditionalJump> {
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeFlag>("PSW.C"),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::BoolEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrJNC : public Instruction8051<"jnc", "0101'0000'oooo'oooo", Category::ConditionalJump> {
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeFlag>("PSW.C"),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrJZ : public Instruction8051<"jz", "0111'0000'oooo'oooo", Category::ConditionalJump> {
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>("A"),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrJNZ : public Instruction8051<"jnz", "0111'0000'oooo'oooo", Category::ConditionalJump> {
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeRegister>("A"),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolNotEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    std::string getRegisterName(u8 reg) {
        switch (reg) {
            case 0x00: return "R0";
            case 0x01: return "R1";
            case 0x02: return "R2";
            case 0x03: return "R3";
            case 0x04: return "R4";
            case 0x05: return "R5";
            case 0x06: return "R6";
            case 0x07: return "R7";
            case 0x80: return "P0";
            case 0x81: return "SP";
            case 0x82: return "DPL";
            case 0x83: return "DPH";
            case 0x87: return "PCON";
            case 0x88: return "TCON";
            case 0x89: return "TMOD";
            case 0x8A: return "TL0";
            case 0x8B: return "TL1";
            case 0x8C: return "TH0";
            case 0x8D: return "TH1";
            case 0x90: return "P1";
            case 0x98: return "SCON";
            case 0x99: return "SBUF";
            case 0xA0: return "P2";
            case 0xA8: return "IE";
            case 0xB0: return "P3";
            case 0xB8: return "IP";
            case 0xD0: return "PSW";
            case 0xE0: return "ACC";
            case 0xF0: return "B";
            default: return fmt::format("SFR_{:02X}", reg);
        }
    }

    std::string getBitName(u8 index) {
        if (index >= 0x00 && index <= 0x7F)
            return fmt::format("MEM.{}", index);
        else if (index >= 0x80 && index <= 0x87)
            return fmt::format("P0.{}", index - 0x80);
        else if (index >= 0x90 && index <= 0x9F)
            return fmt::format("P1.{}", index - 0x90);
        else if (index >= 0xA0 && index <= 0xA7)
            return fmt::format("P2.{}", index - 0xA0);
        else if (index >= 0xB0 && index <= 0xB7)
            return fmt::format("P3.{}", index - 0xB0);
        else if (index >= 0xE0 && index <= 0xE7)
            return fmt::format("A.{}", index - 0xE0);
        else if (index >= 0xF0 && index <= 0xF7)
            return fmt::format("B.{}", index - 0xF0);
        else {
            switch (index) {
                // TCON
                case 0x88: return "IT0";
                case 0x89: return "IE0";
                case 0x8A: return "IT1";
                case 0x8B: return "IE1";
                case 0x8C: return "TR0";
                case 0x8D: return "TF0";
                case 0x8E: return "TR1";
                case 0x8F: return "TF1";

                // SCON
                case 0x98: return "RI";
                case 0x99: return "TI";
                case 0x9A: return "RB8";
                case 0x9B: return "TB8";
                case 0x9C: return "REN";
                case 0x9D: return "SM2";
                case 0x9E: return "SM1";
                case 0x9F: return "SM0";

                // IE
                case 0xA8: return "EX0";
                case 0xA9: return "ET0";
                case 0xAA: return "EX1";
                case 0xAB: return "ET1";
                case 0xAC: return "ES";
                case 0xAF: return "EA";

                // IP
                case 0xB8: return "PX0";
                case 0xB9: return "PT0";
                case 0xBA: return "PX1";
                case 0xBB: return "PT1";
                case 0xBC: return "PS";

                // PSW
                case 0xD0: return "P";
                case 0xD1: return "1";
                case 0xD2: return "OV";
                case 0xD3: return "RS0";
                case 0xD4: return "RS1";
                case 0xD5: return "F0";
                case 0xD6: return "AC";
                case 0xD7: return "CY";

                default: return fmt::format("0x{:02X}", index);
            }
        }
    }

    struct InstrJNB : public Instruction8051<"jnb", "0011'0000'bbbb'bbbb'oooo'oooo", Category::ConditionalJump> {
        using b = Placeholder<'b'>;
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, #0x{:02X}", getBitName(b::get(bytes)), o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeFlag>(getBitName(b::get(bytes))),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrJB : public Instruction8051<"jb", "0010'0000'bbbb'bbbb'oooo'oooo", Category::ConditionalJump> {
        using b = Placeholder<'b'>;
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, #0x{:02X}", getBitName(b::get(bytes)), o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeFlag>(getBitName(b::get(bytes))),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolNotEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrCLRBit : public Instruction8051<"clr", "1100'0010'bbbb'bbbb", Category::MemoryAccess> {
        using b = Placeholder<'b'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}", getBitName(b::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(0),
                            create<ASTNodeFlag>(getBitName(b::get(bytes)))
                    )
            );
        }
    };

    struct InstrSETBC : public Instruction8051<"set", "1101'0011", Category::RegisterAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "C";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(1),
                            create<ASTNodeFlag>("C")
                    )
            );
        }
    };

    struct InstrSETBBit : public Instruction8051<"set", "1101'0010'bbbb'bbbb", Category::MemoryAccess> {
        using b = Placeholder<'b'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}", getBitName(b::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(1),
                            create<ASTNodeFlag>(getBitName(b::get(bytes)))
                    )
            );
        }
    };

    struct InstrCLRC : public Instruction8051<"clr", "1100'0011", Category::RegisterAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "C";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(0),
                            create<ASTNodeFlag>("C")
                    )
            );
        }
    };

    struct InstrCLRA : public Instruction8051<"clr", "1110'0100", Category::RegisterAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "A";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(0),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVRegAddrImmediate : public Instruction8051<"mov", "0111'011n'iiii'iiii", Category::MemoryAccess> {
        using n = Placeholder<'n'>;
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("@R{}, #0x{:02X}", n::get(bytes), i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            )
                    )
            );
        }
    };

    struct InstrMOVRegAddrA : public Instruction8051<"mov", "1111'011i", Category::MemoryAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("@R{}, A", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>("A"),
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", i::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            )
                    )
            );
        }
    };

    struct InstrMOVRegAddrDirect : public Instruction8051<"mov", "1010'011i'dddd'dddd", Category::MemoryAccess> {
        using i = Placeholder<'i'>;
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("@R{}, {}", i::get(bytes), getRegisterName(d::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes))),
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(getRegisterName(i::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            )
                    )
            );
        }
    };

    struct InstrMOVAImmediate : public Instruction8051<"mov", "0111'0100'iiii'iiii", Category::RegisterAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("A, #0x{:02X}", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVARegAddr : public Instruction8051<"mov", "1110'011i", Category::RegisterAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("A, @R{}", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVADirect : public Instruction8051<"mov", "1110'0101'dddd'dddd", Category::RegisterAccess> {
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("A, {}", getRegisterName(d::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes))),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVAReg : public Instruction8051<"mov", "1110'1nnn", Category::RegisterAccess> {
        using n = Placeholder<'n'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("A, R{}", getRegisterName(n::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(getRegisterName(n::get(bytes))),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVBitC : public Instruction8051<"mov", "1001'0010'bbbb'bbbb", Category::RegisterAccess> {
        using b = Placeholder<'b'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, C", getBitName(b::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeFlag>("C"),
                            create<ASTNodeFlag>(getBitName(b::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVCBit : public Instruction8051<"mov", "1010'0010'bbbb'bbbb", Category::RegisterAccess> {
        using b = Placeholder<'b'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("C, {}", getBitName(b::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeFlag>(getBitName(b::get(bytes))),
                            create<ASTNodeFlag>("C")
                    )
            );
        }
    };

    struct InstrMOVDirectDirect : public Instruction8051<"mov", "1000'0101'ssss'ssss'dddd'dddd", Category::MemoryAccess> {
        using s = Placeholder<'s'>;
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, {}", getRegisterName(d::get(bytes)), getRegisterName(s::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(getRegisterName(s::get(bytes))),
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVDirectImmediate : public Instruction8051<"mov", "0111'0101'dddd'dddd'iiii'iiii", Category::RegisterAccess> {
        using d = Placeholder<'d'>;
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, #0x{:02}", getRegisterName(d::get(bytes)), i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVDirectRegAddr : public Instruction8051<"mov", "1000'011n'dddd'dddd", Category::RegisterAccess> {
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, @R{}", getRegisterName(d::get(bytes)), n::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            ),
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVDirectA : public Instruction8051<"mov", "1111'0101'dddd'dddd", Category::RegisterAccess> {
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, A", getRegisterName(d::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>("A"),
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVDirectReg : public Instruction8051<"mov", "1000'1nnn'dddd'dddd", Category::RegisterAccess> {
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("{}, R{}", getRegisterName(d::get(bytes)), n::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))),
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVDptrImmediate : public Instruction8051<"mov", "1001'0000'iiii'iiii'iiii'iiii", Category::RegisterAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("DPTR, #0x{:04X}", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeRegister>("DPTR")
                    )
            );
        }
    };

    struct InstrMOVRegImmediate : public Instruction8051<"mov", "0111'1nnn'iiii'iiii", Category::RegisterAccess> {
        using n = Placeholder<'n'>;
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("R{}, #0x{:04X}", n::get(bytes), i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeIntegerLiteral>(i::get(bytes)),
                            create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVRegA : public Instruction8051<"mov", "1111'1nnn", Category::RegisterAccess> {
        using n = Placeholder<'n'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("R{}, A", n::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>("A"),
                            create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes)))
                    )
            );
        }
    };

    struct InstrMOVRegDirect : public Instruction8051<"mov", "1010'1nnn'dddd'dddd", Category::RegisterAccess> {
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("R{}, {}", n::get(bytes), getRegisterName(d::get(bytes)));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>(getRegisterName(d::get(bytes))),
                            create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes)))
                    )
            );
        }
    };

    struct InstrRET : public Instruction8051<"ret", "0010'0010", Category::FunctionReturn> {

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeControlFlowStatement>(ASTNodeControlFlowStatement::Type::Return)
            );
        }
    };

    struct InstrRETI : public Instruction8051<"reti", "0011'0010", Category::FunctionReturn> {

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeControlFlowStatement>(ASTNodeControlFlowStatement::Type::Return)
            );
        }
    };

    struct InstrMOVXRegAddrA : public Instruction8051<"movx", "1111'001i", Category::MemoryAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("@R{}, A", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>("A"),
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", i::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            )
                    )
            );
        }
    };

    struct InstrMOVXADPTRAddr : public Instruction8051<"movx", "1110'0000", Category::MemoryAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "A, @DPTR";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>("DPTR"),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            ),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrMOVXDPTRAddrA : public Instruction8051<"movx", "1111'0000", Category::MemoryAccess> {
        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return "@DPTR, A";
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeRegister>("A"),
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>("DPTR"),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            )
                    )
            );
        }
    };

    struct InstrMOVXARegAddr : public Instruction8051<"movx", "1110'001i", Category::MemoryAccess> {
        using i = Placeholder<'i'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("A, @R{}", i::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeUnaryArithmetic>(
                                    create<ASTNodeRegister>(fmt::format("R{}", i::get(bytes))),
                                    ASTNodeUnaryArithmetic::Operator::Dereference
                            ),
                            create<ASTNodeRegister>("A")
                    )
            );
        }
    };

    struct InstrLCall : public Instruction8051<"lcall", "0001'0010'aaaa'aaaa'aaaa'aaaa", Category::FunctionCall> {
        using a = Placeholder<'a'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", a::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeFunctionCall>(create<ASTNodeIntegerLiteral>(a::get(bytes)))
            );
        }
    };

    struct InstrACall : public Instruction8051<"acall", "aaa1'0001'aaaa'aaaa", Category::FunctionCall> {
        using a = Placeholder<'a'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}", a::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeFunctionCall>(create<ASTNodeIntegerLiteral>(a::get(bytes)))
            );
        }
    };

    struct InstrDJNZDirectOffset : public Instruction8051<"djnz", "1101'0101'dddd'dddd'oooo'oooo", Category::FunctionCall> {
        using d = Placeholder<'d'>;
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("#0x{:02X}, #0x{:02X}", d::get(bytes), address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeUnaryArithmetic>(create<ASTNodeIntegerLiteral>(d::get(bytes)), ASTNodeUnaryArithmetic::Operator::Dereference),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Subtract
                            ),
                            create<ASTNodeUnaryArithmetic>(create<ASTNodeIntegerLiteral>(d::get(bytes)), ASTNodeUnaryArithmetic::Operator::Dereference)
                    ),
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeBinaryArithmetic>(
                                            create<ASTNodeUnaryArithmetic>(create<ASTNodeIntegerLiteral>(d::get(bytes)), ASTNodeUnaryArithmetic::Operator::Dereference),
                                            create<ASTNodeIntegerLiteral>(1),
                                            ASTNodeBinaryArithmetic::Operator::Subtract
                                    ),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolNotEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };

    struct InstrDJNZRegisterOffset : public Instruction8051<"djnz", "1101'1nnn'oooo'oooo", Category::FunctionCall> {
        using n = Placeholder<'n'>;
        using o = Placeholder<'o'>;

        static std::string disassemble(u64 address, std::span<const u8> bytes) {
            return fmt::format("R{}, #0x{:02X}", n::get(bytes), address + o::get(bytes));
        }

        static std::vector<std::unique_ptr<ASTNode>> decompile(u64 address, std::span<const u8> bytes) {
            return asVector(
                    create<ASTNodeAssignment>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeUnaryArithmetic>(create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))), ASTNodeUnaryArithmetic::Operator::Dereference),
                                    create<ASTNodeIntegerLiteral>(1),
                                    ASTNodeBinaryArithmetic::Operator::Subtract
                            ),
                            create<ASTNodeUnaryArithmetic>(create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))), ASTNodeUnaryArithmetic::Operator::Dereference)
                    ),
                    create<ASTNodeConditional>(
                            create<ASTNodeBinaryArithmetic>(
                                    create<ASTNodeBinaryArithmetic>(
                                            create<ASTNodeUnaryArithmetic>(create<ASTNodeRegister>(fmt::format("R{}", n::get(bytes))), ASTNodeUnaryArithmetic::Operator::Dereference),
                                            create<ASTNodeIntegerLiteral>(1),
                                            ASTNodeBinaryArithmetic::Operator::Subtract
                                    ),
                                    create<ASTNodeIntegerLiteral>(0),
                                    ASTNodeBinaryArithmetic::Operator::BoolNotEqual
                            ),
                            asVector(
                                    create<ASTNodeJump>(
                                            create<ASTNodeIntegerLiteral>(address + o::get(bytes) + 2)
                                    )
                            ),
                            asVector())
            );
        }
    };


    struct Architecture {
        constexpr static auto InstructionSizeMin = 1;

        using Instructions = InstructionArray<
                InstrNop,
                InstrAJmp,
                InstrLJmp,
                InstrSJmp,
                InstrRR,
                InstrIncR,
                InstrIncDPTR,
                InstrIncA,
                InstrIncDirect,
                InstrIncIndirect,
                InstrJC,
                InstrJNC,
                InstrJZ,
                InstrJNZ,
                InstrJNB,
                InstrJB,
                InstrCLRBit,
                InstrCLRC,
                InstrCLRA,
                InstrMOVRegAddrImmediate,
                InstrMOVRegAddrA,
                InstrMOVRegAddrDirect,
                InstrMOVAImmediate,
                InstrMOVARegAddr,
                InstrMOVADirect,
                InstrMOVAReg,
                InstrMOVBitC,
                InstrMOVCBit,
                InstrMOVDirectDirect,
                InstrMOVDirectImmediate,
                InstrMOVDirectRegAddr,
                InstrMOVDirectA,
                InstrMOVDirectReg,
                InstrMOVDptrImmediate,
                InstrMOVRegImmediate,
                InstrMOVRegA,
                InstrMOVRegDirect,
                InstrRET,
                InstrRETI,
                InstrSETBC,
                InstrSETBBit,
                InstrMOVXRegAddrA,
                InstrMOVXADPTRAddr,
                InstrMOVXDPTRAddrA,
                InstrMOVXARegAddr,
                InstrLCall,
                InstrACall,
                InstrDJNZDirectOffset,
                InstrDJNZRegisterOffset
        >;
    };


}