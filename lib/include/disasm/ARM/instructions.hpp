#pragma once

#include <tuple>
#include <disasm/instruction.hpp>

namespace dc::disasm::arm::v7::thumb {

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue>
    struct InstructionARMBase : public Instruction<MnemonicValue, PatternValue, std::endian::little> { };

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue>
    struct InstructionARM : public InstructionARMBase<MnemonicValue, PatternValue> {
        using Parent = InstructionARMBase<MnemonicValue, PatternValue>;

        template<auto First, auto ... Rest>
        static constexpr auto Deref() {
            return [](const auto &bytes) { return fmt::format("{}", formatImpl<First, Rest...>(bytes)); };
        }

        template<typename Placeholder>
        constexpr static auto R() {
            return [](const auto &bytes) { return fmt::format("R{}", Placeholder::get(bytes)); };
        }

        template<typename Placeholder>
        constexpr static auto RWriteBack() {
            return [](const auto &bytes) { return fmt::format("R{}!", Placeholder::get(bytes)); };
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

        template<u32 Value>
        constexpr static auto Imm() {
            return [](const auto &bytes) { return fmt::format("#{}", Value); };
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

    struct InstrBIC : public InstructionARM<"bic", "010000'1110'mmm'nnn"> {
        using m = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrBKPT : public InstructionARM<"bkpt", "1011'1110'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<Imm<imm8>>(bytes);
        }
    };

    struct InstrBLX : public InstructionARM<"blx", "010001'11'1'mmmm'xxx"> {
        using m = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<m>>(bytes);
        }
    };

    struct InstrBX : public InstructionARM<"bx", "010001'11'0'mmmm'xxx"> {
        using m = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<m>>(bytes);
        }
    };

    struct InstrCBNZ : public InstructionARM<"cbnz", "1011'0'0'i'1'iiiii'nnn"> {
        using imm6 = Placeholder<'i'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, Imm<imm6, 1>>(bytes);
        }
    };

    struct InstrCBZ : public InstructionARM<"cbz", "1011'1'0'i'1'iiiii'nnn"> {
        using imm6 = Placeholder<'i'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, Imm<imm6, 1>>(bytes);
        }
    };

    struct InstrCMNRegister : public InstructionARM<"cmn", "010000'1011'mmm'nnn"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<m>, R<n>>(bytes);
        }
    };

    struct InstrCMPImmediate : public InstructionARM<"cmp", "001'01'nnn'iiiiiiii"> {
        using n    = Placeholder<'n'>;
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, Imm<imm8>>(bytes);
        }
    };

    struct InstrCMPRegisterT1 : public InstructionARM<"cmp", "010000'1010'mmm'nnn"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, R<m>>(bytes);
        }
    };

    struct InstrCMPRegisterT2 : public InstructionARM<"cmp", "010001'01'n'mmmm'nnn"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, R<m>>(bytes);
        }
    };

    struct InstrCPS : public InstructionARM<"cps", "1011'0110'011'e'x'a'i'f"> {
        using a = Placeholder<'a'>;
        using i = Placeholder<'i'>;
        using f = Placeholder<'f'>;

        using enable = Placeholder<'e'>;

        static auto formatFlags(const auto &bytes) {
            std::string result;
            if (a::get(bytes))
                result += 'a';
            if (i::get(bytes))
                result += 'i';
            if (f::get(bytes))
                result += 'f';

            return result;
        }

        static auto disassemble(const auto &bytes) {
            return format(bytes) + (enable::get(bytes) == 0 ? "IE" : "ID") + formatFlags(bytes);
        }
    };

    struct InstrEORRegister : public InstructionARM<"eor", "010000'0001'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrIT : public InstructionARM<"it", "1011'1111'cccc'mmmm"> {
        using cond = Placeholder<'c'>;
        using mask = Placeholder<'m'>;

        static auto formatMask(const auto &bytes) {
            const bool firstCond0 = cond::get(bytes) & 0b0001;
            const auto maskValue = mask::get(bytes);

            if ((maskValue & 0b1111) == 0b1000)
                return std::string();

            bool x = false, y = false, z = false;

            if ((maskValue & 0b0111) == 0b0100) {
                x = true;
                y = false;
                z = false;
            }
            if ((maskValue & 0b0011) == 0b0010) {
                x = true;
                y = true;
                z = false;
            }
            if ((maskValue & 0b0001) == 0b0001) {
                x = true;
                y = true;
                z = true;
            }

            std::string result;
            if (x) result += firstCond0 ? "T" : "E";
            if (y) result += firstCond0 ? "T" : "E";
            if (z) result += firstCond0 ? "T" : "E";

            return result;
        }

        static auto disassemble(const auto &bytes) {
            return format(bytes) + formatMask(bytes) + Cond<cond>()(bytes);
        }
    };

    struct InstrLDM : public InstructionARM<"ldm", "1100'1'nnn'rrrrrrrr"> {
        using register_list = Placeholder<'r'>;
        using n = Placeholder<'n'>;

        static auto formatRegisterList(const auto &bytes) {
            std::string result = "{ ";

            auto registers = register_list::get(bytes);

            for (u8 i = 0; i < 8; i++) {
                if ((registers & (1 << i)) != 0x00)
                    result += fmt::format("R{}, ", i);
            }

            result.pop_back();
            result.pop_back();

            return result + " }";
        }

        static auto disassemble(const auto &bytes) {
            if (register_list::get(bytes) & (1 << n::get(bytes)))
                return format<RWriteBack<n>>(bytes) + formatRegisterList(bytes);
            else
                return format<R<n>>(bytes) + formatRegisterList(bytes);
        }
    };

    struct InstrLDRImmediateT1 : public InstructionARM<"ldr", "011'0'1'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5, 2>>>(bytes);
        }
    };

    struct InstrLDRImmediateT2 : public InstructionARM<"ldr", "1001'1'ttt'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<SP, Imm<imm8, 2>>>(bytes);
        }
    };

    struct InstrLDRLiteral : public InstructionARM<"ldr", "01001'ttt'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Imm<imm8, 2>>(bytes);
        }
    };

    struct InstrLDRRegister : public InstructionARM<"ldr", "0101'100'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrLDRBImmediate : public InstructionARM<"ldrb", "011'1'1'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5>>>(bytes);
        }
    };

    struct InstrLDRBRegister : public InstructionARM<"ldrb", "0101'110'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrLDRHImmediate : public InstructionARM<"ldrh", "1000'1'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5, 1>>>(bytes);
        }
    };

    struct InstrLDRHRegister : public InstructionARM<"ldrh", "0101'101'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrLDRSBRegister : public InstructionARM<"ldrsb", "0101'011'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrLDRSHRegister : public InstructionARM<"ldrsb", "0101'111'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrLSLImmediate : public InstructionARM<"lsl", "000'00'iiiii'mmm'ddd"> {
        using imm5 = Placeholder<'i'>;
        using m    = Placeholder<'m'>;
        using d    = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>, Imm<imm5>>(bytes);
        }
    };

    struct InstrLSLRegister : public InstructionARM<"lsl", "010000'0010'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrLSRImmediate : public InstructionARM<"lsr", "000'01'iiiii'mmm'ddd"> {
        using imm5 = Placeholder<'i'>;
        using m    = Placeholder<'m'>;
        using d    = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>, Imm<imm5>>(bytes);
        }
    };

    struct InstrLSRRegister : public InstructionARM<"lsr", "010000'0011'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrMOVImmediate : public InstructionARM<"mov", "001'00'ddd'iiiiiiii"> {
        using d    = Placeholder<'d'>;
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, Imm<imm8>>(bytes);
        }
    };

    struct InstrMOVRegisterT1 : public InstructionARM<"mov", "010001'10'd'mmmm'ddd"> {
        using d = Placeholder<'d'>;
        using m = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrMOVRegisterT2 : public InstructionARM<"mov", "000'00'00000'mmm'ddd"> {
        using d = Placeholder<'d'>;
        using m = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrMUL : public InstructionARM<"mul", "010000'1101'nnn'mmm"> {
        using n  = Placeholder<'n'>;
        using dm = Placeholder<'m'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dm>, R<n>, R<dm>>(bytes);
        }
    };

    struct InstrMVNRegister : public InstructionARM<"mvn", "010000'1111'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrNOP : public InstructionARM<"nop", "1011'1111'0000'0000"> {
        static auto disassemble(const auto &bytes) {
            return format(bytes);
        }
    };

    struct InstrORRRegister : public InstructionARM<"orr", "010000'1100'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrPop : public InstructionARM<"pop", "1011'1'10'p'rrrrrrrr"> {
        using P = Placeholder<'p'>;
        using register_list = Placeholder<'r'>;

        static auto formatRegisterList(const auto &bytes) {
            std::string result = "{ ";

            auto registers = register_list::get(bytes);
            auto pc = P::get(bytes);

            for (u8 i = 0; i < 8; i++) {
                if ((registers & (1 << i)) != 0x00)
                    result += fmt::format("R{}, ", i);
            }

            if (pc)
                result += "PC, ";

            result.pop_back();
            result.pop_back();

            return result + " }";
        }

        static auto disassemble(const auto &bytes) {
            return format(bytes) + formatRegisterList(bytes);
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

    struct InstrREV : public InstructionARM<"rev", "1011'1010'00'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrREV16 : public InstructionARM<"rev16", "1011'1010'01'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrREVSH : public InstructionARM<"revsh", "1011'1010'11'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrRORRegister : public InstructionARM<"ror", "010000'0111'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrRSBImmediate : public InstructionARM<"rsb", "010000'1001'nnn'ddd"> {
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<n>, Imm<0>>(bytes);
        }
    };

    struct InstrSBCRegister : public InstructionARM<"sbc", "010000'0110'mmm'nnn"> {
        using m  = Placeholder<'m'>;
        using dn = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, R<m>>(bytes);
        }
    };

    struct InstrSEV : public InstructionARM<"sev", "1011'1111'0100'0000"> {
        static auto disassemble(const auto &bytes) {
            return format(bytes);
        }
    };

    struct InstrSTM : public InstructionARM<"stm", "1100'0'nnn'rrrrrrrr"> {
        using register_list = Placeholder<'r'>;
        using n = Placeholder<'n'>;

        static auto formatRegisterList(const auto &bytes) {
            std::string result = "{ ";

            auto registers = register_list::get(bytes);

            for (u8 i = 0; i < 8; i++) {
                if ((registers & (1 << i)) != 0x00)
                    result += fmt::format("R{}, ", i);
            }

            result.pop_back();
            result.pop_back();

            return result + " }";
        }

        static auto disassemble(const auto &bytes) {
            if (register_list::get(bytes) & (1 << n::get(bytes)))
                return format<RWriteBack<n>>(bytes) + formatRegisterList(bytes);
            else
                return format<R<n>>(bytes) + formatRegisterList(bytes);
        }
    };

    struct InstrSTRImmediateT1 : public InstructionARM<"str", "011'0'0'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5, 2>>>(bytes);
        }
    };

    struct InstrSTRImmediateT2 : public InstructionARM<"str", "1001'0'ttt'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<SP, Imm<imm8, 2>>>(bytes);
        }
    };

    struct InstrSTRRegister : public InstructionARM<"str", "0101'000'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrSTRBImmediate : public InstructionARM<"strb", "011'10'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5>>>(bytes);
        }
    };

    struct InstrSTRBRegister : public InstructionARM<"strh", "0101'010'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrSTRHImmediate : public InstructionARM<"strh", "1000'0'iiiii'nnn'ttt"> {
        using imm5 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using t    = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, Imm<imm5, 1>>>(bytes);
        }
    };

    struct InstrSTRHRegister : public InstructionARM<"strh", "0101'001'mmm'nnn'ttt"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using t = Placeholder<'t'>;

        static auto disassemble(const auto &bytes) {
            return format<R<t>, Deref<R<n>, R<m>>>(bytes);
        }
    };

    struct InstrSUBImmediateT1 : public InstructionARM<"sub", "000'11'1'1'iii'nnn'ddd"> {
        using imm3 = Placeholder<'i'>;
        using n    = Placeholder<'n'>;
        using d    = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<n>, Imm<imm3>>(bytes);
        }
    };

    struct InstrSUBImmediateT2 : public InstructionARM<"sub", "001'11'nnn'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;
        using dn   = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<dn>, Imm<imm8>>(bytes);
        }
    };

    struct InstrSUBRegister : public InstructionARM<"sub", "000'11'0'1'mmm'nnn'ddd"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<n>, R<m>>(bytes);
        }
    };

    struct InstrSUBSPMinusImmediate : public InstructionARM<"sub", "1011'0000'1'iiiiiii"> {
        using imm7 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<SP, SP, Imm<imm7, 2>>(bytes);
        }
    };

    struct InstrSVC : public InstructionARM<"svc", "1101'1111'iiiiiiii"> {
        using imm8 = Placeholder<'i'>;

        static auto disassemble(const auto &bytes) {
            return format<Imm<imm8>>(bytes);
        }
    };

    struct InstrSXTB : public InstructionARM<"sxtb", "1011'0010'01'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrSXTH : public InstructionARM<"sxtb", "1011'0010'00'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrTSTRegister : public InstructionARM<"tst", "010000'1000'mmm'nnn"> {
        using m = Placeholder<'m'>;
        using n = Placeholder<'n'>;

        static auto disassemble(const auto &bytes) {
            return format<R<n>, R<m>>(bytes);
        }
    };

    struct InstrUXTB : public InstructionARM<"uxtb", "1011'0010'11'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrUXTH : public InstructionARM<"uxth", "1011'0010'10'mmm'ddd"> {
        using m = Placeholder<'m'>;
        using d = Placeholder<'d'>;

        static auto disassemble(const auto &bytes) {
            return format<R<d>, R<m>>(bytes);
        }
    };

    struct InstrWFE : public InstructionARM<"wfe", "1011'1111'0010'0000"> {
        static auto disassemble(const auto &bytes) {
            return format(bytes);
        }
    };

    struct InstrWFI : public InstructionARM<"wfi", "1011'1111'0011'0000"> {
        static auto disassemble(const auto &bytes) {
            return format(bytes);
        }
    };

    struct InstrYIELD : public InstructionARM<"yield", "1011'1111'0001'0000"> {
        static auto disassemble(const auto &bytes) {
            return format(bytes);
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
                InstrBIC,
                InstrBKPT,
                InstrBLX,
                InstrBX,
                InstrCBNZ,
                InstrCBZ,
                InstrCMNRegister,
                InstrCMPImmediate,
                InstrCMPRegisterT1,
                InstrCMPRegisterT2,
                InstrCPS,
                InstrEORRegister,
                InstrIT,
                InstrLDM,
                InstrLDRImmediateT1,
                InstrLDRImmediateT2,
                InstrLDRLiteral,
                InstrLDRRegister,
                InstrLDRBImmediate,
                InstrLDRBRegister,
                InstrLDRHImmediate,
                InstrLDRHRegister,
                InstrLDRSBRegister,
                InstrLDRSHRegister,
                InstrLSLImmediate,
                InstrLSLRegister,
                InstrLSRImmediate,
                InstrLSRRegister,
                InstrMOVImmediate,
                InstrMOVRegisterT1,
                InstrMOVRegisterT2,
                InstrMUL,
                InstrMVNRegister,
                InstrNOP,
                InstrORRRegister,
                InstrPop,
                InstrPush,
                InstrREV,
                InstrREV16,
                InstrREVSH,
                InstrRORRegister,
                InstrRSBImmediate,
                InstrSBCRegister,
                InstrSEV,
                InstrSTM,
                InstrSTRImmediateT1,
                InstrSTRImmediateT2,
                InstrSTRRegister,
                InstrSTRBImmediate,
                InstrSTRBRegister,
                InstrSTRHImmediate,
                InstrSTRHRegister,
                InstrSUBImmediateT1,
                InstrSUBImmediateT2,
                InstrSUBRegister,
                InstrSUBSPMinusImmediate,
                InstrSXTB,
                InstrSXTH,
                InstrTSTRegister,
                InstrUXTB,
                InstrUXTH,
                InstrWFE,
                InstrWFI,
                InstrYIELD
        >;
    };


}