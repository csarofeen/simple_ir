#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"
#include "Mutators.h"
using namespace Fuser;

int main(){
  std::cout<<"Start"<<std::endl;
  auto v1 = IntImm::make(20);
  auto v2 = Variable::make("myvar");

  Expr my_add = Sub::make(v1, v2);

  PrintVisitor printer;
  my_add.accept(&printer);

  std::cout<<"Clear to: "<<std::endl;
  ClearMutator clear;
  auto second = clear.mutate(my_add);
  second.accept(&printer);
  std::cout<<"Done"<<std::endl;



}
