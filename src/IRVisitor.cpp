#include "IRVisitor.h"
#include "IR.h"

namespace Fuser{

  void IRVisitor::visit(const Expr* node){
    node->accept(this);
  }

#define EMPTY_VISIT(T) \
void IRVisitor::visit(const T*) {}

EMPTY_VISIT(IntImm)
EMPTY_VISIT(Variable)
EMPTY_VISIT(Thread)

void IRVisitor::visit(const Tensor *op){
    throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
}

void IRVisitor::visit(const JITTensor *op){

    for(const auto shape : op->shapes)
        shape.accept(this);

    for(const auto stride : op->strides)
        stride.accept(this);
}

void IRVisitor::visit(const TensorAccessor *op){
    op->tensor.accept(this);
    for(const auto var : op->indexers)
        var.accept(this);
}

void IRVisitor::visit(const For *op){
    op->loop_var.accept(this);
    op->range.accept(this);
    op->body.accept(this);
}

void IRVisitor::visit(const Range *op){
    op->min.accept(this);
    op->extent.accept(this);
}

void IRVisitor::visit(const If *op){
    op->pred.accept(this);
    op->body.accept(this);
}

void IRVisitor::visit(const Attr *op){
    op->body.accept(this);
    op->value.accept(this);
}

void IRVisitor::visit(const Block *op){
    for(auto &expr : op->exprs)
        expr.accept(this);
}

void IRVisitor::visit(const Merge *op){
    op->original_domain.accept(this);
}

void IRVisitor::visit(const Split *op){
    op->original_domain.accept(this);
    op->factor.accept(this);
}

void IRVisitor::visit(const Reorder *op){
    throw std::runtime_error(std::string("Not implemented. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
}

void IRVisitor::visit(const BinaryOp *op){
    op->a.accept(this);
    op->b.accept(this);
}

void IRVisitor::visit(const TensorDomain *op){
    for(const auto& r : op->domain){
        r.accept(this);
    }
}

void IRVisitor::visit(const Null *op){
    throw std::runtime_error(std::string("Found Null op in visitor. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
}

}

