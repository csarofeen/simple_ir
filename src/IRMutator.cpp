#include "IRMutator.h"
#include "IR.h"
#include "Printer.h"

namespace Fuser{

IRMutator::IRMutator() {
}

IRMutator::~IRMutator() {
}

Expr IRMutator::mutate(const Expr &e) {
    return e.defined() ? e.get()->mutate_expr(this) : Expr();
}

Expr IRMutator::visit(const IntImm *op) {return op;}
Expr IRMutator::visit(const Variable *op) {return op;}

Expr IRMutator::visit(const Tensor *op){
    throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
}

Expr IRMutator::visit(const JITTensor *op){

    bool modded = false;
    std::vector<Expr> shapes;
    std::vector<Expr> strides;

    for(const auto &shape: op->shapes){
        auto new_shape = mutate(shape);
        shapes.push_back(new_shape);
        if(!shape.same_as(new_shape))
            modded = true;
    }

    for(const auto &stride: op->strides){
        auto new_stride = mutate(stride);
        strides.push_back(new_stride);
        if(!stride.same_as(new_stride))
            modded = true;
    }

    if(modded)
        return JITTensor::make(op->ndims, shapes, strides, op->name.c_str());
    return op;
}

Expr IRMutator::visit(const TensorAccessor *op){
    auto tensor = mutate(op->tensor);
    bool mutated = !tensor.same_as(op->tensor);
    std::vector<Expr> indexers;
    for(const auto &ind: op->indexers){
        auto new_ind = mutate(ind);
        indexers.push_back(new_ind);
        mutated = mutated | !ind.same_as(new_ind);
    }
    if(mutated)
        return TensorAccessor::make(tensor, indexers);
    return op;
}

Expr IRMutator::visit(const For *op){
    Expr range = mutate(op->range);
    Expr loop_var = mutate(op->loop_var);
    Expr body = mutate(op->body);
    if(range.same_as(op->range)
        && loop_var.same_as(op->loop_var)
        && body.same_as(op->body))
        return op;
    return For::make(range, loop_var, body);
}

Expr IRMutator::visit(const If *op){
    Expr pred = mutate(op->pred);
    Expr body = mutate(op->body);
    if(pred.same_as(op->pred)
        && body.same_as(op->body))
        return op;
    return If::make(pred, body);
}

Expr IRMutator::visit(const Range *op){
    Expr min = mutate(op->min);
    Expr extent = mutate(op->extent);
    if(min.same_as(op->min)
       && extent.same_as(op->extent))
        return op;
    return Range::make(min, extent);
}

Expr IRMutator::visit(const Attr *op){
    Expr body = mutate(op->body);
    Expr value = mutate(op->value);
    
    if( body.same_as(op->body)
     && value.same_as(op->value))
        return op;
    return Attr::make(op->attr_type, body, value);
}

Expr IRMutator::visit(const Thread *op){
    return op;
}

Expr IRMutator::visit(const Block *op){
    std::vector<Expr> exprs;
    bool mutated = false;
    for(const auto& expr : op->exprs){
        auto new_expr = mutate(expr);
        exprs.push_back(new_expr);
        if(!expr.same_as(new_expr))
            mutated = true;
    }
    if(mutated)
        return Block::make(exprs);
    return op;
    
}

Expr IRMutator::visit(const Split *op){
    Expr dom = mutate(op->original_domain);
    Expr factor = mutate(op->factor);
    if( dom.same_as(op->original_domain)
     && factor.same_as(op->factor)
    ){
        return op;
    }
    return Split::make(dom, op->axis, factor);
}

Expr IRMutator::visit(const Merge *op){
    Expr dom = mutate(op->original_domain);
    if( dom.same_as(op->original_domain) )
        return op;
    
    return Merge::make(dom, op->axis);
}

Expr IRMutator::visit(const Reorder *op){
    throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
    return op;
}

Expr IRMutator::visit(const BinaryOp *op){
    Expr a = mutate(op->a);
    Expr b = mutate(op->b);
    if(
        !(
             a.same_as(op->a)
          && b.same_as(op->b)
        )
    ){
        return BinaryOp::make(a, b, op->op_type);
    }
    return op;
}

Expr IRMutator::visit(const TensorDomain *op){
    throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
    return op;
}

Expr IRMutator::visit(const Null *op){
    throw std::runtime_error("Null node hit in mutator.");
    return op;
}


}
