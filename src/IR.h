#pragma once
#include <string>
#include <iostream>
#include <memory>

/*
  Adding a new IR node:

  1) Add a new IRType class in this file
  2) Add the new IR node in this file, inherit from IR
  3) Add accept method in IR.cpp
  4) Define visit method in IRVisitor.h
*/

class IRVisitor;

enum class IRType{
  null,
  For,
  Range,
  Variable,
  Mul,
  Div,
  Add,
  Sub,
  KnownVar //For hard-coding values (int, float, etc.)
};

class Node{
protected:
  Node(IRType t = IRType::null):type(t){}

public:
  virtual void accept(class IRVisitor &v);

  static constexpr IRType static_type = IRType::For;
  const IRType type;

  template<typename T> T* as(){
    if (T::static_type == type){
      return dynamic_cast<T*>(this);
    }else{
      std::cout<<(int) T::static_type<<" =? "<< (int) type<<std::endl;
    }
    return nullptr;
  }
};


//Starting with some very basic types.
enum class CType{
  Int, //Default size_t
  Float, //Default single precision
  Bool
};

class Variable: public Node{
 public:
  std::string name = "";
  Variable(std::string name):name(name), Node(IRType::Variable){}
  void accept(IRVisitor &v);
  static constexpr IRType static_type = IRType::Variable;
};

class Range: public Node{
public:
  Range(Node* min_, Node* extent_):Node(IRType::Range){
    min = std::shared_ptr<Node>(min_);
    extent = std::shared_ptr<Node>(extent_);
  }

  void accept(IRVisitor &v);
  std::shared_ptr<Node> min, extent;
  static constexpr IRType static_type = IRType::Range;
};


class For: public Node{
public:

  For(Range* range_, Variable* loop_var_):Node(IRType::For){
    range = std::shared_ptr<Range>(range_);
    loop_var = std::shared_ptr<Variable>(loop_var_);
  }

  void accept(IRVisitor &v);
  std::shared_ptr<Range> range;
  std::shared_ptr<Variable> loop_var;
  static constexpr IRType static_type = IRType::For;
};


#define BINARYOP(OP, OP_STR)                            \
class OP: public Node{                                  \
  public:                                               \
    OP(Node* LHS_, Node* RHS_):Node(IRType::OP){        \
      LHS = std::shared_ptr<Node>(LHS_);                \
      RHS = std::shared_ptr<Node>(RHS_);                \
    }                                                   \
  void accept(IRVisitor &v);                            \
  std::shared_ptr<Node> LHS;                            \
  std::shared_ptr<Node> RHS;                            \
  static constexpr IRType static_type = IRType::OP;     \
  static constexpr char op_string[] = OP_STR;           \
};

BINARYOP(Mul, "*")
BINARYOP(Div, "/")
BINARYOP(Add, "+")
BINARYOP(Sub, "-")
