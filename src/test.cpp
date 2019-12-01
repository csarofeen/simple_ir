#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"

using namespace Fuser;

int main()
{  
  PrintVisitor printer(std::cout);

  std::vector<std::shared_ptr<Node> > size;
  std::vector<std::shared_ptr<Node> > stride;
  
  for(int i=0; i<3; i++){
    size.push_back(Variable::make(
      ("size"+std::to_string(i)).c_str()
    ));

    stride.push_back(Variable::make(
      ("stride"+std::to_string(i)).c_str()
    ));
  
  }

  auto tens = Tensor::make(3, size, stride, "MyTensor");
  
  printer.visit(tens.get());

  //auto A = Variable::make("A");
  //auto B = Variable::make("B");
  //auto C = Variable::make("C");
  //auto div = Div::make(B, Variable::make((size_t)10));
  //auto loop_range = Range::make(A, div);
  //auto iter_var = Variable::make("i");
  //auto my_for = For::make(loop_range, iter_var);
  //printer.visit(my_for.get());
  
}
