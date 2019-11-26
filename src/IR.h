#pragma once
#include <string>
#include <iostream>
#include <memory>

class IRVisitor;

enum class IRType {
		   null,
		   For,
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

class Variable;
class For: public IR
{
  
public:

  For(Variable* min_, Variable* extent_):IR(IRType::For){
    min = std::shared_ptr<Variable>(min_);
    extent = std::shared_ptr<Variable>(extent_);
  }  
  
  void accept(IRVisitor &v);  
  std::shared_ptr<Variable> min, extent;
  IRType type = IRType::For;
  static constexpr IRType static_type = IRType::For;
};

class Variable: public IR
{
public:
  std::string name = "";
  Variable(std::string name):name(name),IR(IRType::Variable){}
  void accept(IRVisitor &v);
  static constexpr IRType static_type = IRType::Variable;
};
