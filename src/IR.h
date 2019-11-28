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
5)

 */

class IRVisitor;

enum class IRType {
		   null,
		   For,
		   Range,
		   Variable
};

class IR
{
protected:
  const IRType type;
  IR(IRType t = IRType::null):type(t){}
  
public:
  virtual void accept(class IRVisitor &v);

  static constexpr IRType static_type = IRType::For;

  template<typename T> T* as(){
    if (T::static_type == type){
      return dynamic_cast<T*>(this);
    }else{
      std::cout<<(int) T::static_type<<" =? "<< (int) type<<std::endl;
    }
    return nullptr;
  }
};


class Variable: public IR
{
public:
  std::string name = "";
  Variable(std::string name):name(name),IR(IRType::Variable){}
  void accept(IRVisitor &v);
  static constexpr IRType static_type = IRType::Variable;
};


class Range: public IR
{
public:
  Range(Variable* min_, Variable* extent_):IR(IRType::Range){
    min = std::shared_ptr<Variable>(min_);
    extent = std::shared_ptr<Variable>(extent_);
  }  
  
  void accept(IRVisitor &v);
  std::shared_ptr<Variable> min, extent;
  static constexpr IRType static_type = IRType::Range;
};


class For: public IR
{
  
public:

  For(Range* range_, Variable* loop_var_):IR(IRType::For){
    range = std::shared_ptr<Range>(range_);
    loop_var = std::shared_ptr<Variable>(loop_var_);
  }  
  
  void accept(IRVisitor &v);  
  std::shared_ptr<Range> range;
  std::shared_ptr<Variable> loop_var;
  static constexpr IRType static_type = IRType::For;
};

