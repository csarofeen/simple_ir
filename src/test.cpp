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
  
  auto A = get_tensor(3, "A");
  auto B = get_tensor(3, "B");
  auto C = get_tensor(3, "C");

  Expr my_add = Add::make(A, B);
  Expr result = Set::make(C, my_add);
  std::cout<<"Printing a Stmt: ";
  result.accept(&printer);
  std::cout<<std::endl;

  LoopTranslate loop_nest_writer;
  Expr loop_nest = loop_nest_writer.visit(result.as<Set>());
  loop_nest.accept(&printer);

  std::cout<<"\nDone"<<std::endl;

}
