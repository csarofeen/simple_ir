#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"
#include "Mutators.h"
using namespace Fuser;

Expr get_tensor(int ndims = 0, const char* name = ""){

  std::vector<Expr> size;
  std::vector<Expr> stride;
  
  for(int i=0; i<ndims; i++){
    size.push_back(Variable::make(
      ("size"+std::to_string(i)).c_str()
    ));

    stride.push_back(Variable::make(
      ("stride"+std::to_string(i)).c_str()
    ));
  }

  return Tensor::make(ndims, size, stride, name);

}

int main(){

  std::cout<<"Start"<<std::endl;

  //A print visitor
  PrintVisitor printer(std::cout);

  //Mutator that sets all IntImms to 0;
  ClearMutator clear;


  auto v1 = IntImm::make(20);
  auto v2 = Variable::make("myvar");
  auto A = get_tensor(3, "A");

  Expr my_add = Sub::make(v1, v2);

  auto second = clear.mutate(my_add);
  
  std::cout<<"20 - my var : ";
  my_add.accept(&printer);
  std::cout<<std::endl;

  std::cout<<"Cleared to: "<<std::endl;
  second.accept(&printer);
  std::cout<<std::endl;

  std::cout<<"Printing a tensor: ";
  A.accept(&printer);
  std::cout<<std::endl;

  std::cout<<"Done"<<std::endl;

}
