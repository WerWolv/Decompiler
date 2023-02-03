#include <ast/ast_node.hpp>

#include <decomp/decompiler.hpp>

namespace dc::ast {

    void ASTNodeIntegerLiteral::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeJump::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeBinaryArithmetic::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeRegister::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeAssignment::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeUnaryArithmetic::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeFlag::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeConditional::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeControlFlowStatement::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeAssembly::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

    void ASTNodeFunctionCall::accept(dc::decomp::Visitor &visitor) {
        visitor.visit(*this);
    }

}