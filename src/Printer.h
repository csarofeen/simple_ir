#pragma once
#include <ostream>
#include "IRVisitor.h"
#include "IR.h"

namespace Fuser
{

std::ostream &operator<<(std::ostream &os, const std::vector<int> &my_set);

std::ostream &operator<<(std::ostream &os, const Expr &e);

template <typename T>
std::ostream &operator<<(std::ostream &os, const ExprNode<T> *s)
{
    return os << Expr(s);
}

class PrintVisitor : public IRVisitor
{
protected:
    std::ostream &os;

    void visit(Expr op)
    {
        IRVisitor::visit((const Expr *)&op);
    }

    int indent_count;

    void indent()
    {
        for (int i = 0; i < indent_count; i++)
            os << "  ";
    }

public:
    PrintVisitor(std::ostream &os) : os(os), indent_count(0) {}

    void visit(const Variable *op)
    {
        os << op->name;
    }

    void visit(const IntImm *val)
    {
        os << val->value;
    };


    std::unordered_map<BinaryOpType, std::string> bin_op_map = {
        {BinaryOpType::Add, "+"},
        {BinaryOpType::Sub, "-"},
        {BinaryOpType::Mul, "*"},
        {BinaryOpType::Div, "/"},
        {BinaryOpType::Mod, "%"},
        {BinaryOpType::LT,  "<"},
    };

    void visit(const BinaryOp* op)
    {
        op->a.accept(this);
        if(bin_op_map.find(op->op_type) == bin_op_map.end())
            throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
        os<<" "<<bin_op_map[op->op_type]<<" ";
        op->b.accept(this);
    }

    void visit(const Tensor *op){

        if(op->origin.is<Null>()){
            os<<op->name;
        }else{
            op->origin.accept(this);
            os<<"\n";
            os<<op->name;
        }
        /*
        if(!op->origin.is<Null>()){
            if(op->origin.is<BinaryOp>()){

            }else{

            }
            os<<op->name;
            os<<" = ";
            op->origin.accept(this);
            os<<"\n";
        }
        os<<op->name;
        */
    }


    void visit(const JITTensor *op)
    {   
        os << "{ " << op->name << " [";
        for (const auto shape : op->shapes)
        {
            visit(shape);
            if (!shape.same_as(*(op->shapes.end() - 1)))
                os << ", ";
        }
        os << "] (";

        for (const auto stride : op->strides)
        {
            visit(stride);
            if (!stride.same_as(*(op->strides.end() - 1)))
                os << ", ";
        }
        os << ") } ";

        if(!op->origin.is<Null>()){
            os<<" = ";
            op->origin.accept(this);
        }

    }

    void visit(const TensorAccessor *op)
    {
        const JITTensor *tensor = op->tensor.as<JITTensor>();
        os << tensor->name << "(";

        for (const auto ind : op->indexers)
        {
            visit(ind);

            if (!ind.same_as(*(op->indexers.end() - 1)))
                os << ", ";
        }
        os << ") ";
    }

    void visit(const For *op)
    {
        indent();
        os << "For (";
        visit(op->loop_var);
        os << " in ";
        visit(op->range);
        os << "){\n";
        indent_count++;
        visit(op->body);
        indent_count--;
        indent();
        os << "}\n";
    }

    void visit(const If *op)
    {
        indent();
        os << "if (";
        visit(op->pred);
        os << "){\n";
        indent_count++;
        visit(op->body);
        indent_count--;
        indent();
        os << "}\n";
    }

    void visit(const Range *op)
    {
        os << "r[";
        visit(op->min);
        os << " : ";
        visit(op->extent);
        os << "]";
    }


    void visit(const Attr *op)
    {

        switch (op->attr_type)
        {
        case (Attr::ATTR_TYPE::ThreadBinding):
            os << "//Attr thread ";
            visit(op->value);
            os << " bound to " << op->body.as<For>()->range.as<Range>()->extent;
            break;
        case (Attr::ATTR_TYPE::Null):
            os << "Unknown attribute: ";
            break;
        }
        os << "\n";
        visit(op->body);
    }

    void visit(const Thread *op)
    {
        switch (op->thread_type)
        {
        case (Thread::THREAD_TYPE::TIDx):
            os << "threadIdx.x";
            break;
        case (Thread::THREAD_TYPE::TIDy):
            os << "threadIdx.y";
            break;
        case (Thread::THREAD_TYPE::TIDz):
            os << "threadIdx.z";
            break;
        case (Thread::THREAD_TYPE::BIDx):
            os << "blockIdx.x";
            break;
        case (Thread::THREAD_TYPE::BIDy):
            os << "blockIdx.y";
            break;
        case (Thread::THREAD_TYPE::BIDz):
            os << "blockIdx.z";
            break;
        }
    }

    void visit(const Block* op){
        for(const auto& expr: op->exprs){
            expr.accept(this);
        }
    }

    void visit(const Split* op){
        op->original_domain.accept(this);
        os<<"\n";
        indent();
        os<<"Split on axis: "<<op->axis<< " by factor: ";
        op->factor.accept(this);
        os<<"\n";
        indent();
    }

    void visit(const Merge* op){
        op->original_domain.accept(this);
        os<<"\n";
        indent();
        os<<"Merge on axis: "<<op->axis;
        os<<"\n";
        indent();
    }

    void visit(const Reorder* op){
        op->original_domain.accept(this);
        os<<"\n";
        indent();
        os<<"Reordering with map: "<<op->pos2axis<<"\n";
        indent();
    }

    void visit(const TensorDomain* op){
        if(!op->origin.is<Null>()){
            op->origin.accept(this);
        }
        os << "TensorDomain{ ";
        for(auto it = op->domain.begin(); it != op->domain.end(); ++it){
            (*it).accept(this);
            if(it != --op->domain.end())
                os<<", ";
        }
        os<<" }";
    }

    void visit(const Null* op){
        os<<"NULL";
    }

};

} // namespace Fuser