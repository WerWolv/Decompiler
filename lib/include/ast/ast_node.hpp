#pragma once

#include <dc.hpp>

#include <memory>
#include <vector>
#include <string>

namespace dc::decomp { class Visitor; }

namespace dc::ast {

    class ASTNode {
    public:
        virtual void accept(dc::decomp::Visitor &visitor) = 0;
    };

    template<typename T>
    std::unique_ptr<ASTNode> create(auto && ... params) {
        return std::unique_ptr<ASTNode>(new T(std::forward<decltype(params)>(params)...));
    }

    std::vector<std::unique_ptr<ASTNode>> asVector(auto && ... nodes) {
        std::vector<std::unique_ptr<ASTNode>> result;

        (result.push_back(std::move(nodes)), ...);

        return result;
    }

    class ASTNodeIntegerLiteral : public ASTNode {
    public:
        ASTNodeIntegerLiteral(u32 value) : m_value(value) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr u32 getValue() const { return this->m_value; }

    private:
        u32 m_value;
    };

    class ASTNodeRegister : public ASTNode {
    public:
        ASTNodeRegister(std::string registerName) : m_registerName(std::move(registerName)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::string& getRegisterName() const { return this->m_registerName; }

    private:
        std::string m_registerName;
    };

    class ASTNodeFlag : public ASTNode {
    public:
        ASTNodeFlag(std::string flagName) : m_flagName(std::move(flagName)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::string& getFlagName() const { return this->m_flagName; }

    private:
        std::string m_flagName;
    };

    class ASTNodeJump : public ASTNode {
    public:
        ASTNodeJump(std::unique_ptr<ASTNode> &&destination) : m_destination(std::move(destination)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getDestination() const { return this->m_destination; }

    private:
        std::unique_ptr<ASTNode> m_destination;
    };

    class ASTNodeAssignment : public ASTNode {
    public:
        ASTNodeAssignment(std::unique_ptr<ASTNode> &&source, std::unique_ptr<ASTNode> &&destination) : m_source(std::move(source)), m_destination(std::move(destination)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getSource() const { return this->m_source; }
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getDestination() const { return this->m_destination; }

    private:
        std::unique_ptr<ASTNode> m_source, m_destination;
    };

    class ASTNodeUnaryArithmetic : public ASTNode {
    public:
        enum class Operator {
            Negate,
            BoolNot,
            BitNot,
            Reference,
            Dereference
        };
    public:
        ASTNodeUnaryArithmetic(std::unique_ptr<ASTNode> &&operand, Operator op)
                : m_operand(std::move(operand)), m_operator(op) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getOperand() const { return this->m_operand; }
        [[nodiscard]] constexpr Operator getOperator() const { return this->m_operator; }

    private:
        std::unique_ptr<ASTNode> m_operand;
        Operator m_operator;
    };

    class ASTNodeBinaryArithmetic : public ASTNode {
    public:
        enum class Operator {
            Add,
            Subtract,
            Multiply,
            Divide,
            Modulus,
            ShiftLeftLogical,
            ShiftRightLogical,
            ShiftRightArithmetical,
            RotateLeft,
            RotateRight,
            BoolAnd,
            BoolOr,
            BoolXor,
            BoolEqual,
            BoolNotEqual,
            BoolGreaterThan,
            BoolLessThan,
            BoolGreaterThanOrEqual,
            BoolLessThanOrEqual,
            BitAnd,
            BitOr,
            BitXor
        };
    public:
        ASTNodeBinaryArithmetic(std::unique_ptr<ASTNode> &&lhs, std::unique_ptr<ASTNode> &&rhs, Operator op)
            : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)), m_operator(op) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getLeftHandSide() const { return this->m_lhs; }
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getRightHandSide() const { return this->m_rhs; }
        [[nodiscard]] constexpr Operator getOperator() const { return this->m_operator; }

    private:
        std::unique_ptr<ASTNode> m_lhs, m_rhs;
        Operator m_operator;
    };

    class ASTNodeConditional : public ASTNode {
    public:
        ASTNodeConditional(std::unique_ptr<ASTNode> &&condition, std::vector<std::unique_ptr<ASTNode>> &&trueBlock, std::vector<std::unique_ptr<ASTNode>> &&falseBlock)
                : m_condition(std::move(condition)), m_trueBlock(std::move(trueBlock)), m_falseBlock(std::move(falseBlock)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getCondition() const { return this->m_condition; }
        [[nodiscard]] constexpr const std::vector<std::unique_ptr<ASTNode>>& getTrueBlock() const { return this->m_trueBlock; }
        [[nodiscard]] constexpr const std::vector<std::unique_ptr<ASTNode>>& getFalseBlock() const { return this->m_falseBlock; }

    private:
        std::unique_ptr<ASTNode> m_condition;
        std::vector<std::unique_ptr<ASTNode>> m_trueBlock, m_falseBlock;
    };

    class ASTNodeControlFlowStatement : public ASTNode {
    public:
        enum class Type {
            Break,
            Continue,
            Return
        };

        ASTNodeControlFlowStatement(Type type) : m_type(type) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const Type getType() const { return this->m_type; }

    private:
        Type m_type;
    };

    class ASTNodeAssembly : public ASTNode {
    public:
        ASTNodeAssembly(std::string assembly) : m_assembly(std::move(assembly)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::string& getAssembly() const { return this->m_assembly; }

    private:
        std::string m_assembly;
    };

    class ASTNodeFunctionCall : public ASTNode {
    public:
        ASTNodeFunctionCall(std::unique_ptr<ASTNode>&& destination) : m_destination(std::move(destination)) {}

        void accept(dc::decomp::Visitor &visitor) override;
        [[nodiscard]] constexpr const std::unique_ptr<ASTNode>& getDestination() const { return this->m_destination; }

    private:
        std::unique_ptr<ASTNode> m_destination;
    };

}