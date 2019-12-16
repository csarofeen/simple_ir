#pragma once
#include <ostream>
#include "IRVisitor.h"

namespace Fuser{

class PrintVisitor : public IRVisitor {
std::ostream& os;

void visit(Expr op){
  IRVisitor::visit( (const Expr*) &op);
}

int indent_count;

void indent(){
  for(int i=0; i<indent_count; i++)
    os<<"  ";
}

public:
PrintVisitor(std::ostream& os):os(os),indent_count(0){}

  void visit(const Variable* op){
    os<<op->name; 
  }

  void visit(const IntImm *val){
    os<<val->value;
  };

  #define BINARY_PRINT(TYPE, STRING) \
  void visit(const TYPE *op){ \
    os<<"( "; \
    visit(op->a); \
    os<<" "<<STRING<<" "; \
    visit(op->b); \
    os<<" )"; \
  }

  BINARY_PRINT(Add, "+")
  BINARY_PRINT(Sub, "-")
  BINARY_PRINT(Mul, "*")
  BINARY_PRINT(Div, "/")
  BINARY_PRINT(Mod, "%")
  BINARY_PRINT(LT, "<")

  void visit(const Set *op){
    visit(op->a);
    os<<" = ";
    visit(op->b);
    os<<";\n";
  }

  void visit(const Tensor *op){
    os<<"{ "<<op->name<<" [";
    for(const auto shape : op->shapes){
      visit(shape);
      if(! shape.same_as(*(op->shapes.end()-1)))
        os<<", ";
    }
    os<<"] (";

    for(const auto stride : op->strides){
      visit(stride);
      if(! stride.same_as(*(op->strides.end()-1)))
        os<<", ";
    }
    os<<") } ";
  }

  void visit(const TensorAccessor *op){
    const Tensor *tensor =  op->tensor.as<Tensor>();
    os<<tensor->name<<"(";

    for(const auto ind : op->indexers){
      visit(ind);
      
      if(! ind.same_as(*(op->indexers.end()-1)))
        os<<", ";
    }
    os<<") ";
  }

  void visit(const For *op){
    indent();
    os<<"For (";
    visit(op->loop_var);
    os<<" in ";
    visit(op->min);
    os<<":";
    visit(op->extent);
    os<<"){\n";
    indent_count++;
    if(!op->body.as<For>())
      indent();
    visit(op->body);
    indent_count--;
    if(op->body.as<For>())
      os<<"\n";
    indent();
    os<<"}";
  }

    void visit(const If *op){
    indent();
    os<<"if (";
    visit(op->pred);
    os<<"){\n";
    indent_count++;
    visit(op->body);
    indent_count--;
    indent();
    os<<"}";
  }

};


std::ostream& operator<<(std::ostream& os, const Expr& e);

template<typename T>
std::ostream& operator<<(std::ostream& os, const ExprNode<T>* s) {
    return os << Expr(s);
}

}