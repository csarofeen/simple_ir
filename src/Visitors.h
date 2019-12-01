#pragma once
#include "IRVisitor.h"
#include <memory>
#include <iostream>

namespace Fuser{

class PrintVisitor: public IRVisitor
{
std::ostream& os;

public:
PrintVisitor(std::ostream& os):os(os){}

#define PRINTVISITOR_CASE(T) \
  case(IRType::T):           \
    visit(node->as<T>());    \
    break; 

  void visit(const Node* const node){
    switch(node->type){
      PRINTVISITOR_CASE(For)
      PRINTVISITOR_CASE(Range)
      PRINTVISITOR_CASE(Variable)
      PRINTVISITOR_CASE(Mul)
      PRINTVISITOR_CASE(Div)
      PRINTVISITOR_CASE(Add)
      PRINTVISITOR_CASE(Sub)
    }
  }
  
#define PRINTVISITOR_BIN_OP(OP, OP_STR)      \
  virtual void visit(const OP* const node){  \
    os<<"(";                          \
    visit(node->LHS.get());                  \
    os<<" "<<OP_STR<<" ";             \
    visit(node->RHS.get());                  \
    os<<")";                          \
  }

  PRINTVISITOR_BIN_OP(Div, "/")
  PRINTVISITOR_BIN_OP(Mul, "*")
  PRINTVISITOR_BIN_OP(Add, "+")
  PRINTVISITOR_BIN_OP(Sub, "-")

  void visit(const For* const node){
    os<<"For( ";
    visit(node->loop_var.get()->as<Variable>());
    os<<" in ";
    visit(node->range.get()->as<Range>());
    os<<" )"<<std::endl;
  }

  void visit(const Range* const node){
    os<<"[";
    visit(node->min.get()->as<Variable>());
    os<<", ";
    visit(node->extent.get());
    os<<"]";
  }
  
  void visit(const Variable* const node){
    if(!(node->is_imm)){
      os << node->name;
    }else if(node->is_bool){
      os << node->get_bval();
    }else if(node->is_floating){
      os << node->get_fval();
    }else if(node->is_int){
      os << node->get_ival();
    }
  }

 virtual void visit(const Tensor* const node){
   os << node->name << " [";
    for(int i=0; i<node->ndims; i++){
      visit(node->shape(i).get());
      if(i!=node->ndims-1)
        os<<", ";
    }
    os<<"] (";
    for(int i=0; i<node->ndims; i++){
      visit(node->stride(i).get());
      if(i!=node->ndims-1)
        os<<", ";
    }
    os<<")";
 }
};

}//namespace Fuser