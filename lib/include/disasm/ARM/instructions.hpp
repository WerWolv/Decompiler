#pragma once

#include <tuple>
#include <disasm/instruction.hpp>

namespace dc::disasm::arm::v7::thumb {

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue>
    struct InstructionARMBase : public Instruction<MnemonicValue, PatternValue, std::endian::little> { };

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue>
    struct InstructionARM : public InstructionARMBase<MnemonicValue, PatternValue> {
        using Parent = InstructionARMBase<MnemonicValue, PatternValue>;

        template<typename Placeholder>
        constexpr static auto R() {
            return [](const auto &bytes) { return fmt::format("R{}", Placeholder::get(bytes)); };
        }

        constexpr static auto SP() {
            return [](const auto &) { return "SP"; };
        }

        constexpr static auto PC() {
            return [](const auto &) { return "PC"; };
        }

        constexpr static auto None() {
            return [](const auto &) { return ""; };
        }

        template<typename Placeholder, size_t Shift = 0>
        constexpr static auto Imm() {
            return [](const auto &bytes) { return fmt::format("#{}", Placeholder::get(bytes) << Shift); };
        }

        template<typename Placeholder, size_t Size, size_t Shift = 0>
        constexpr static auto ImmSigned() {
            return [](const auto &bytes) {
                i64 value = Placeholder::get(bytes) << Shift;
                if ((value & (1 << (Size - 1))) != 0)
                    value = (std::numeric_limits<u64>::max() << Size) | value;

                return fmt::format("#{}", value);
            };
        }

        template<typename Placeholder>
        constexpr static auto Cond() {
            return [](const auto &bytes) {
                auto cond = Placeholder::get(bytes);

                switch (cond) {
                    case 0b0000: return "EQ";
                    case 0b0001: return "NE";
                    case 0b0010: return "CS";
                    case 0b0011: return "CC";
                    case 0b0100: return "MI";
                    case 0b0101: return "PL";
                    case 0b0110: return "VS";
                    case 0b0111: return "VC";
                    case 0b1000: return "HI";
                    case 0b1001: return "LS";
                    case 0b1010: return "GE";
                    case 0b1011: return "LT";
                    case 0b1100: return "GT";
                    case 0b1101: return "LE";
                    case 0b1110: return "";
                    default: return "";
                }
            };
        }

        template<auto First = None, auto ... Rest>
        static constexpr auto format(const auto &bytes) {
            return fmt::format("{} {}", Parent::Mnemonic, formatImpl<First, Rest...>(bytes));
        }

    private:
        template<auto First, auto ... Rest>
        static constexpr auto formatImpl(const auto &bytes) {
            if constexpr (sizeof...(Rest) > 0)
                return fmt::format("{}, {}", First()(bytes), formatImpl<Rest...>(bytes));
            else
                return fmt::format("{}", First()(bytes));
        }
    };

    struct InstrADCRegister : public InstructionARM<"adc", "010000'0101'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        constexpr static auto disassemble(const auto &bytes) {
            return format<R<m>, R<dn>>(bytes);
        }
    };

    struct InstrADDImmediateT1 : public InstructionARM<"adds", "000'11'1'0'iii'nnn'ddd"> {
        using imm3 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using d    = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<n>, Imm<imm3>>(bytes);
        }
    };

    struct InstrADDImmediateT2 : public InstructionARM<"adds", "001'10'nnn'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;
        using dn   = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, Imm<imm8>>(bytes);
        }
    };

    struct InstrADDRegisterT1 : public InstructionARM<"adds", "000'11'0'0'mmm'nnn'ddd"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<m>, R<n>, R<d>>(bytes);
        }
    };

    struct InstrADDRegisterT2 : public InstructionARM<"adds", "010001'00'n'mmmm'nnn"> {
        using dn = Placeholder<'n'>;
        using m  = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrADDSPImmediateT1 : public InstructionARM<"add", "1010'1'ddd'iiiiiiii"> {
        using d    = Placeholder<'d'>;
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, SP, Imm<imm8, 2>>(bytes);
        }
    };

    struct InstrADDSPImmediateT2 : public InstructionARM<"add", "1011'0000'0'iiiiiii"> {
        using imm7 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<SP, SP, Imm<imm7, 2>>(bytes);
        }
    };

    struct InstrADDSPRegisterT1 : public InstructionARM<"add", "01000100'm'1101'mmm"> {
        using dm = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dm>, SP, R<dm>>(bytes);
        }
    };

    struct InstrADDSPRegisterT2 : public InstructionARM<"add", "01000100'1'mmmm'101"> {
        using m = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<SP, R<m>>(bytes);
        }
    };

    struct InstrADR : public InstructionARM<"adr", "1010'0'ddd'iiiiiiii"> {
        using d    = Placeholder<'d'>;
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, Imm<imm8, 2>>(bytes);
        }
    };

    struct InstrANDRegister : public InstructionARM<"and", "010000'0000'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrASRImmediate : public InstructionARM<"asr", "000'10'iiiii'mmm'ddd"> {
        using imm5 = Placeholder<'i'>;
        using m    = Placeholder<'m'>;
        using d    = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>, Imm<imm5>>(bytes);
        }
    };

    struct InstrASRRegister : public InstructionARM<"asrs", "010000'0100'mmm'nnn"> {
        using dn = Placeholder<'n'>;
        using m  = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrBT1 : public InstructionARM<"b", "1101'cccc'iiiiiiii"> {
        using cond = Placeholder<'c'>;
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<Cond<cond>, ImmSigned<imm8, 8, 1>>(bytes);
        }
    };

    struct InstrBT2 : public InstructionARM<"b", "11100'iiiiiiiiiii"> {
        using imm11 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<ImmSigned<imm11, 11, 1>>(bytes);
        }
    };

    struct InstrPush : public InstructionARM<"push", "1011'0'10'm'rrrrrrrr"> {
        using M = Placeholder<'m'>;
        using register_list = Placeholder<'r'>;

        static auto formatRegisterList(const auto &bytes) {
            std::string result = "{ ";

            auto registers = register_list::get(bytes);
            auto lr = M::get(bytes);

            for (u8 i = 0; i < 8; i++) {
                if ((registers & (1 << i)) != 0x00)
                    result += fmt::format("R{}, ", i);
            }

            if (lr)
                result += "LR, ";

            result.pop_back();
            result.pop_back();

            return result + " }";
        }

        static auto disassemble(const auto &bytes) {
            return format(bytes) + formatRegisterList(bytes);
        }
    };

    struct Architecture {
        constexpr static auto InstructionSizeMin = 2;

        using Instructions = InstructionArray<
                InstrADCRegister,
                InstrADDImmediateT1,
                InstrADDImmediateT2,
                InstrADDRegisterT1,
                InstrADDRegisterT2,
                InstrADDSPImmediateT1,
                InstrADDSPImmediateT2,
                InstrADDSPRegisterT1,
                InstrADDSPRegisterT2,
                InstrADR,
                InstrANDRegister,
                InstrASRImmediate,
                InstrASRRegister,
                InstrBT1,
                InstrBT2,
                InstrPush
        >;
    };


}