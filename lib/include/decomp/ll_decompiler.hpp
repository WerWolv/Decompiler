#pragma once

#include <decomp/decompiler.hpp>
#include <span>

#include <ast/ast_node.hpp>

#include <fmt/format.h>

namespace dc::decomp {

    class LowLevelDecompiler : public Visitor {
    public:
        void visit(ast::ASTNodeIntegerLiteral &node) {
            fmt::print("0x{:02X}", node.getValue());
        }

        void visit(ast::ASTNodeJump &node) {
            fmt::print("goto ");
            node.getDestination()->accept(*this);
        }

        void visit(ast::ASTNodeBinaryArithmetic &node) {
            node.getLeftHandSide()->accept(*this);

            switch (node.getOperator()) {
                using enum ast::ASTNodeBinaryArithmetic::Operator;
                case Add:                       fmt::print(" + ");  break;
                case Subtract:                  fmt::print(" - ");  break;
                case Multiply:                  fmt::print(" * ");  break;
                case Divide:                    fmt::print(" / ");  break;
                case Modulus:                   fmt::print(" % ");  break;
                case ShiftLeftLogical:          fmt::print(" <<L "); break;
                case ShiftRightLogical:         fmt::print(" >>L "); break;
                case ShiftRightArithmetical:    fmt::print(" >>A "); break;
                case RotateLeft:                fmt::print(" <<< "); break;
                case RotateRight:               fmt::print(" >>> "); break;
                case BoolAnd:                   fmt::print(" && "); break;
                case BoolOr:                    fmt::print(" || "); break;
                case BoolXor:                   fmt::print(" ^^ "); break;
                case BitAnd:                    fmt::print(" & "); break;
                case BitOr:                     fmt::print(" | "); break;
                case BitXor:                    fmt::print(" ^ "); break;
                case BoolEqual:                 fmt::print(" == "); break;
                case BoolNotEqual:              fmt::print(" != "); break;
                case BoolGreaterThan:           fmt::print(" > "); break;
                case BoolLessThan:              fmt::print(" < "); break;
                case BoolGreaterThanOrEqual:    fmt::print(" >= "); break;
                case BoolLessThanOrEqual:       fmt::print(" <= "); break;
            }

            node.getRightHandSide()->accept(*this);

        }

        void visit(ast::ASTNodeUnaryArithmetic &node) {
            switch (node.getOperator()) {
                using enum ast::ASTNodeUnaryArithmetic::Operator;
                case Negate: fmt::print("-"); break;
                case BitNot: fmt::print("~"); break;
                case BoolNot: fmt::print("!"); break;
                case Reference: fmt::print("&"); break;
                case Dereference: fmt::print("*"); break;
            }

            node.getOperand()->accept(*this);

        }

        void visit(ast::ASTNodeRegister &node) {
            fmt::print("{}", node.getRegisterName());
        }

        void visit(ast::ASTNodeAssignment &node) {
            node.getDestination()->accept(*this);
            fmt::print(" = ");
            node.getSource()->accept(*this);
        }

        void visit(ast::ASTNodeFlag &node) {
            fmt::print("FLAGS.{}", node.getFlagName());
        }

        void visit(ast::ASTNodeConditional &node) {
            fmt::print("if (");
            node.getCondition()->accept(*this);
            fmt::print(") {{\n");

            for (auto &bodyNode : node.getTrueBlock()) {
                fmt::print("    ");
                bodyNode->accept(*this);
                fmt::print("\n");
            }

            fmt::print("}}");
            if (const auto &falseBody = node.getFalseBlock(); !falseBody.empty()) {
                fmt::print(" else {{\n");
                for (auto &bodyNode : falseBody) {
                    fmt::print("    ");
                    bodyNode->accept(*this);
                    fmt::print("\n");
                }
                fmt::print("}}");
            }
        }

        void visit(ast::ASTNodeControlFlowStatement &node) {
            switch (node.getType()) {
                using enum ast::ASTNodeControlFlowStatement::Type;
                case Return: fmt::print("return"); break;
                case Break: fmt::print("break"); break;
                case Continue: fmt::print("continue"); break;
            }
        }

        void visit(ast::ASTNodeAssembly &node) {
            fmt::print("asm volatile {{ {} }}", hlp::trim(node.getAssembly()));
        }

        void visit(ast::ASTNodeFunctionCall &node) {
            fmt::print("sub_");
            node.getDestination()->accept(*this);
            fmt::print("()");
        }

    };

}