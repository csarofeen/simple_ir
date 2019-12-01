#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

/*
  Adding a new IR node:
  1) Add a new IRType class in this file
  2) Add the new IR node in this file, inherit from IR
  3) Add accept method in IR.cpp
  4) Define visit method in IRVisitor.h
*/

namespace Fuser{

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

  template<typename T> const T* const as() const {
    if (T::static_type == type){
      return dynamic_cast<const T* const>(this);
    }else{
      std::cout<<(int) T::static_type<<" =? "<< (int) type<<std::endl;
    }
    return nullptr;
  }
};

//Starting with really basic const values here. There's a better version of this
//in the JIT that I want to track down and use.
class Variable: public Node{

  const double fval = 0.0;
  const size_t ival = 0;
  const bool bval = false;

 public:
  const bool is_imm = false;
  const bool is_floating = false;
  const bool is_int = false;
  const bool is_bool = false;


  int bytes(){
    return (bits() + 7) / 8;
  }
  virtual int bits(){return 0;};

  Variable(const char* name):name(name), Node(IRType::Variable){}

  explicit Variable(size_t val):is_imm(true), is_int(true), ival(val), Node(IRType::Variable){}
  explicit Variable(double val):is_imm(true), is_floating(true), ival(val), Node(IRType::Variable){}
  explicit Variable(bool val):is_imm(true), is_bool(true), ival(val), Node(IRType::Variable){}

  int get_ival() const{
    assert(is_imm && is_int);
    return ival;
  }

  double get_fval() const{
    assert(is_imm && is_floating);
    return fval;
  }

  bool get_bval() const{
    assert(is_imm && is_bool);
    return bval;
  }

  void accept(IRVisitor &v);
  
  static const std::shared_ptr<Variable> make(const char* name){
    return std::make_shared<Variable>(name);
  }

  static const std::shared_ptr<Variable> make(size_t val){
    return std::make_shared<Variable>(val);
  }

  static const std::shared_ptr<Variable> make(double val){
    return std::make_shared<Variable>(val);
  }

  static const std::shared_ptr<Variable> make(bool val){
    return std::make_shared<Variable>(val);
  }

  std::string name = "";
  static constexpr IRType static_type = IRType::Variable;
};

class Range: public Node{
public:
  Range(const std::shared_ptr<Node> min_, const std::shared_ptr<Node> extent_):Node(IRType::Range), min(min_), extent(extent_){}

  void accept(IRVisitor &v);
  
  static const std::shared_ptr<Range> make(const std::shared_ptr<Node> min_, const std::shared_ptr<Node> extent_){
    return std::make_shared<Range>(min_, extent_);
  }

  std::shared_ptr<Node> min, extent;
  static constexpr IRType static_type = IRType::Range;
};


class For: public Node{
public:
  For(const std::shared_ptr<Range> range_, const std::shared_ptr<Variable> loop_var_):Node(IRType::For), range(range_), loop_var(loop_var_){}

  void accept(IRVisitor &v);

  static const std::shared_ptr<For> make(const std::shared_ptr<Range> range_, const std::shared_ptr<Variable> loop_var_){
    return std::make_shared<For>(range_, loop_var_);
  }
  
  std::shared_ptr<Range> range;
  std::shared_ptr<Variable> loop_var;
  static constexpr IRType static_type = IRType::For;
};

#define BINARYOP(OP, OP_STR)                                                                                      \
class OP: public Node{                                                                                            \
public:                                                                                                           \
  OP(const std::shared_ptr<Node> LHS_, const std::shared_ptr<Node> RHS_):Node(IRType::OP), LHS(LHS_), RHS(RHS_){} \
                                                                                                                  \
  void accept(IRVisitor &v);                                                                                      \
                                                                                                                  \
  static const std::shared_ptr<OP> make(const std::shared_ptr<Node> LHS_, const std::shared_ptr<Node> RHS_){      \
    return std::make_shared<OP>(LHS_, RHS_);                                                                      \
  }                                                                                                               \
                                                                                                                  \
  std::shared_ptr<Node> LHS;                                                                                      \
  std::shared_ptr<Node> RHS;                                                                                      \
  static constexpr IRType static_type = IRType::OP;                                                               \
  static constexpr char op_string[] = OP_STR;                                                                     \
};

BINARYOP(Mul, "*")
BINARYOP(Div, "/")
BINARYOP(Add, "+")
BINARYOP(Sub, "-")


}//Fuser namspace