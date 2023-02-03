#pragma once

#include <disasm/architecture.hpp>
#include <disasm/instruction.hpp>
#include <disasm/disassembler.hpp>
#include <span>

#include <ast/ast_node.hpp>

namespace dc::decomp {

    class Visitor {
    public:
        virtual void visit(ast::ASTNodeIntegerLiteral &node) = 0;
        virtual void visit(ast::ASTNodeJump &node) = 0;
        virtual void visit(ast::ASTNodeBinaryArithmetic &node) = 0;
        virtual void visit(ast::ASTNodeRegister &node) = 0;
        virtual void visit(ast::ASTNodeAssignment &node) = 0;
        virtual void visit(ast::ASTNodeUnaryArithmetic &node) = 0;
        virtual void visit(ast::ASTNodeFlag &node) = 0;
        virtual void visit(ast::ASTNodeConditional &node) = 0;
        virtual void visit(ast::ASTNodeControlFlowStatement &node) = 0;
        virtual void visit(ast::ASTNodeAssembly &node) = 0;
        virtual void visit(ast::ASTNodeFunctionCall &node) = 0;
    };

    namespace {

        template<std::derived_from<dc::hlp::TypeArrayBase> T, size_t Index>
        std::pair<size_t, std::vector<std::unique_ptr<ast::ASTNode>>> decompile(u64 offset, std::span<const u8> bytes) {
            using Instr = typename T::template Get<Index>;
            using namespace ast;

            if (Instr::Pattern::matches(bytes)) {
                return { Instr::Pattern::getByteCount(), Instr::decompile(offset, bytes) };
            }
            else if constexpr (Index < (T::Size - 1))
                return decompile<T, Index + 1>(offset, bytes);
            else
                return {};
        }

    }



    template<dc::disasm::ArchitectureType T>
    auto decompile(std::span<const u8> bytes) {
        std::vector<std::unique_ptr<ast::ASTNode>> ast;
        size_t offset = 0x00;

        while (offset < bytes.size()) {
            auto begin = bytes.begin() + offset;

            auto [size, nodes] = decompile<typename T::Instructions, 0>(offset, std::span { begin, bytes.end() });
            if (size < T::InstructionSizeMin) {

                offset += 1;

            }
            else {
                for (auto &&node : nodes)
                    ast.emplace_back(std::move(node));

                offset += size;
            }
        }

        return ast;
    }

}