//
// Created by motya on 28.03.2025.
//

#ifndef CRYPT_AST_H
#define CRYPT_AST_H

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unistd.h>

template<typename T>
using unique_ptr = std::unique_ptr<T>;


namespace ast {
    enum class VarType {
        UNKNOWN,
        INT,
        //:)
    };

    enum class BinaryOpType {
        ADD = 0,
        SUB,
        MUL,
        DIV,
        MOD,
        ASSIGN,
        EQ,
        LE,
        LS,
        GR,
        GE,
        AND,
        OR,
        UNKNOWN
    };

    enum class UnaryOpType {
        MINUS,
    };

    enum class NodeType {
        Block,
        FunctionDef,
        FunctionCall,
        ArrayGet,
        Member,
        Return,
        IntLit,
        StringLit,
        Var,
        If,
        While,
        For,
        UnaryMinus,
        BinaryPlus,
        BinaryMul,
        BinaryDiv,
        BinaryMinus,
        Assign, // ?
    };

    class Node {
    public:
        virtual ~Node() = default;

        virtual std::string to_str1() const;

        virtual NodeType get_type() const = 0;

        // upcasting unique_ptr<Derived> to unique_ptr<Node> with honest copying objects
        virtual std::unique_ptr<Node> clone_upcasting() const = 0;

        virtual std::unique_ptr<Node> move_upcasting() = 0;

        bool operator==(const Node &other) const;

        bool operator==(const Node *) const;

        bool operator!=(const Node *) const;
    };

    class FunctionSignature : public Node {
    public:
        std::string name;
        std::vector<std::string> params;

        FunctionSignature() = default;

        FunctionSignature(std::string name, std::string first_param) : name(std::move(name)),
                                                                       params({std::move(first_param)}) {
        }

        std::string to_str1() const override {
            throw "todo";
        }

        NodeType get_type() const override {
            throw "todo";
        }

        unique_ptr<Node> clone_upcasting() const override {
            throw "todo";
        }

        unique_ptr<Node> move_upcasting() override {
            auto new_fn_sign = std::make_unique<FunctionSignature>();
            new_fn_sign->name = std::move(name);
            new_fn_sign->params = std::move(params);
            return new_fn_sign;
        }

        //TODO
    };

    class Block : public Node {
    public:
        std::vector<std::unique_ptr<Node> > lines;

        NodeType get_type() const override { return NodeType::Block; }

        std::unique_ptr<Node> clone_upcasting() const override {
            auto new_block = std::make_unique<Block>();
            // new_block->lines = lines; // honest vec copy?
            // new_block->lines.assign(lines.size;
            new_block->lines.reserve(lines.size());
            for (const auto &line: lines) {
                // Use const reference
                new_block->lines.push_back(line->clone_upcasting());
            }
            return new_block;
        }

        std::unique_ptr<Node> move_upcasting() override {
            auto new_block = std::make_unique<Block>();
            new_block->lines = std::move(lines);
            return new_block;
        }
    };

    class FunctionDef : public Node {
    public:
        std::unique_ptr<Node> signature;
        std::unique_ptr<Node> block;

        NodeType get_type() const override { return NodeType::FunctionDef; }

        FunctionDef() = default;

        FunctionDef(FunctionSignature sig, std::unique_ptr<Node> block) : signature(
                                                                              std::make_unique<FunctionSignature>(sig)),
                                                                          block(std::move(block)) {
        }

        FunctionDef(std::unique_ptr<Node> sig, std::unique_ptr<Node> block) : signature(std::move(sig)),
                                                                              block(std::move(block)) {
        }

        std::unique_ptr<Node> clone_upcasting() const override {
            throw "todo";
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<FunctionDef>(std::move(signature), std::move(block));
        }
    };

    class FunctionCall : public Node {
    public:
        std::unique_ptr<Node> name_expr;
        std::vector<std::unique_ptr<Node> > args;

        explicit FunctionCall(std::unique_ptr<Node> name_expr) : name_expr(std::move(name_expr)) {
        }

        NodeType get_type() const override { return NodeType::FunctionCall; }

        std::unique_ptr<Node> clone_upcasting() const override {
            auto new_call = std::make_unique<FunctionCall>(name_expr->clone_upcasting());
            new_call->args.reserve(args.size());
            for (const auto &arg: args) {
                new_call->args.push_back(arg->clone_upcasting());
            }
            return new_call;
        }

        std::unique_ptr<Node> move_upcasting() override {
            auto new_call = std::make_unique<FunctionCall>(std::move(name_expr));
            new_call->args = std::move(args);
            return new_call;
        }
    };

    class MemberGet : public Node {
    public:
        std::unique_ptr<Node> owner;
        std::string property;

        std::string to_str1() const override {
            throw "todo";
        }

        NodeType get_type() const override { return NodeType::Member; }

        std::unique_ptr<Node> clone_upcasting() const override {
            throw "todo";
        }

        std::unique_ptr<Node> move_upcasting() override {
            throw "todo";
        }

        MemberGet(std::unique_ptr<Node> owner, std::string property_name) : owner(std::move(owner)),
                                                                            property(std::move(property_name)) {
        }
    };

    class ArrayGet : public Node {
    public:
        std::unique_ptr<Node> name_expr;
        std::unique_ptr<Node> index;

        ArrayGet(unique_ptr<Node> name_expr, std::unique_ptr<Node> index) : name_expr(std::move(name_expr)),
                                                                            index(std::move(index)) {
        }

        NodeType get_type() const override { return NodeType::ArrayGet; }

        std::unique_ptr<Node> clone_upcasting() const override {
            return std::make_unique<ArrayGet>(name_expr->clone_upcasting(),
                                              index->clone_upcasting());
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<ArrayGet>(std::move(name_expr), std::move(index));
        }
    };

    class ReturnStmt : public Node {
    public:
        std::unique_ptr<Node> expr;

        ReturnStmt(std::unique_ptr<Node> expr) : expr(std::move(expr)) {
        }

        NodeType get_type() const override { return NodeType::Return; }

        std::unique_ptr<Node> clone_upcasting() const override {
            return std::make_unique<ReturnStmt>(expr->clone_upcasting());
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<ReturnStmt>(std::move(expr));
        }
    };

    class Program {
    public:
        //function declaration (or const var declaration; TODO will be added later )
        std::vector<std::unique_ptr<FunctionDef> > declarations;
    };

    class IntLitExpr : public Node {
    public:
        const int64_t number;

        explicit IntLitExpr(int64_t val) : number(val) {
        }

        NodeType get_type() const override { return NodeType::IntLit; }

        [[nodiscard]] std::string to_str1() const override;

        std::unique_ptr<Node> clone_upcasting() const override {
            return std::make_unique<IntLitExpr>(number);
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<IntLitExpr>(number); // No move needed for primitive type
        }
    };

    class StringLitExpr : public Node {
    public:
        const std::string val;

        explicit StringLitExpr(std::string val) : val(std::move(val)) {
        }

        NodeType get_type() const override { return NodeType::StringLit; }

        [[nodiscard]] std::string to_str1() const override { throw "todo"; }

        std::unique_ptr<Node> clone_upcasting() const override {
            throw "todo";
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<StringLitExpr>(val);
        }
    };

    //actually any expression with identifier at first
    class VarExpr : public Node {
    public:
        VarType type;
        std::string name;

        explicit VarExpr(std::string name) : name(std::move(name)) {
        }

        std::string to_str1() const override;

        NodeType get_type() const override { return NodeType::Var; }

        std::unique_ptr<Node> clone_upcasting() const override {
            auto new_var = std::make_unique<VarExpr>(name);
            new_var->type = type;
            return new_var;
        }

        std::unique_ptr<Node> move_upcasting() override {
            auto new_var = std::make_unique<VarExpr>(std::move(name));
            new_var->type = type;
            return new_var;
        }
    };

    class IfStmt : public Node {
    public:
        std::unique_ptr<Node> expr;
        std::unique_ptr<Node> etrue = nullptr;
        std::unique_ptr<Node> efalse = nullptr;

        IfStmt(std::unique_ptr<Node> expr, std::unique_ptr<Node> etrue, std::unique_ptr<Node> efalse = nullptr)
            : expr(std::move(expr)), etrue(std::move(etrue)), efalse(std::move(efalse)) {
        }

        NodeType get_type() const override { return NodeType::If; }

        std::unique_ptr<Node> clone_upcasting() const override {
            auto new_if = std::make_unique<IfStmt>(expr->clone_upcasting(),
                                                   etrue->clone_upcasting());
            if (efalse) {
                new_if->efalse = efalse->clone_upcasting();
            }
            return new_if;
        }

        std::unique_ptr<Node> move_upcasting() override {
            auto new_if = std::make_unique<IfStmt>(std::move(expr), std::move(etrue));
            if (efalse) {
                new_if->efalse = std::move(efalse);
            }
            return new_if;
        }
    };

    class ForStmt : public Node {
    public:
        std::unique_ptr<Node> init;
        std::unique_ptr<Node> cond;
        std::unique_ptr<Node> inc;
        std::unique_ptr<Block> body;

        ForStmt() = default;

        //        ForStmt(std::unique_ptr<Node> init, std::unique_ptr<Node> cond, std::unique_ptr<Node> inc,
        //                std::unique_ptr<Block> body) : init(std::move(init)),
        //                cond(std::move(cond)),
        //                inc(std::move(inc)),
        //                body(std::move(body)) {}

        NodeType get_type() const override { return NodeType::For; }

        std::unique_ptr<Node> clone_upcasting() const override {
            throw "todo";
        }

        unique_ptr<Node> move_upcasting() override {
            // return std::unique_ptr<ForStmt>(
            //     std::move(init),
            //     std::move(cond),
            //     std::move(inc),
            //     std::move(body)
            //     );
            throw "todo";
        }
    };

    class WhileStmt : public Node {
    public:
        std::unique_ptr<Node> expr;
        std::unique_ptr<Node> body;

        WhileStmt(std::unique_ptr<Node> expr, std::unique_ptr<Node> body) : expr(std::move(expr)),
                                                                            body(std::move(body)) {
        }

        NodeType get_type() const override { return NodeType::While; }

        std::unique_ptr<Node> clone_upcasting() const override {
            auto new_while = std::make_unique<WhileStmt>(expr->clone_upcasting(),
                                                         std::unique_ptr<Block>(
                                                             static_cast<Block *>(body->clone_upcasting().
                                                                 release())));
            return new_while;
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<WhileStmt>(std::move(expr), std::move(body));
        }
    };

    template<UnaryOpType type>
    class UnaryExpr : public Node {
    public:
        std::unique_ptr<Node> expr;

        explicit UnaryExpr(std::unique_ptr<Node> expr) : expr(std::move(expr)) {
        }

        std::string to_str1() const override { return "-(" + expr->to_str1() + ")"; }

        inline NodeType get_type() const override {
            switch (type) {
                case UnaryOpType::MINUS:
                    return NodeType::UnaryMinus;
            }
            throw std::runtime_error("not implemented");
        }

        std::unique_ptr<Node> clone_upcasting() const override {
            return std::make_unique<UnaryExpr>(expr->clone_upcasting());
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<UnaryExpr>(std::move(expr));
        }
    };

    template<BinaryOpType type>
    class BinaryExpr : public Node {
    public:
        unique_ptr<Node> l, r;

        BinaryExpr(unique_ptr<Node> l, unique_ptr<Node> r) : l(std::move(l)), r(std::move(r)) {
        }

        NodeType get_type() const override {
            switch (type) {
                case BinaryOpType::ADD:
                    return NodeType::BinaryPlus;
                case BinaryOpType::MUL:
                    return NodeType::BinaryMul;
                case BinaryOpType::DIV:
                    return NodeType::BinaryDiv;
                case BinaryOpType::SUB:
                    return NodeType::BinaryMinus;
                case BinaryOpType::ASSIGN:
                    return NodeType::Assign;
                default:
                    throw std::runtime_error("not implemented");
            }
        }

        static std::string operatorString() {
            switch (type) {
                case BinaryOpType::ADD:
                    return "+";
                case BinaryOpType::SUB:
                    return "-";
                case BinaryOpType::MUL:
                    return "*";
                case BinaryOpType::DIV:
                    return "/";
                default:
                    throw std::invalid_argument("never see me");;
            }
        }

        std::string to_str1() const override {
            return "(" + l->to_str1() + operatorString() + r->to_str1() + ")";
        }

        std::unique_ptr<Node> clone_upcasting() const override {
            return std::make_unique<BinaryExpr>(l->clone_upcasting(),
                                                r->clone_upcasting());
        }

        std::unique_ptr<Node> move_upcasting() override {
            return std::make_unique<BinaryExpr>(std::move(l), std::move(r));
        }
    };

    using AddExpr = BinaryExpr<BinaryOpType::ADD>;
    using DivExpr = BinaryExpr<BinaryOpType::DIV>;
    using MulExpr = BinaryExpr<BinaryOpType::MUL>;
    using SubExpr = BinaryExpr<BinaryOpType::SUB>;
}


namespace detail {
    inline std::string get_type(const ast::VarType type) {
        switch (type) {
            case ast::VarType::UNKNOWN:
                return "unknown";
            case ast::VarType::INT:
                return "int";
            default:
                throw std::runtime_error("detail::get_type of ast::VarType with such type unimplemented");
        }
    }
}

#endif //CRYPT_AST_H
