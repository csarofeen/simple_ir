#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"

using namespace Fuser;

int   size_counter = 0;
int stride_counter = 0;
int tensor_counter = 0;

std::shared_ptr<Tensor> get_tensor(const char* name="MyTensor", int ndims = 3){
  std::vector<std::shared_ptr<Node> > size;
  std::vector<std::shared_ptr<Node> > stride;

  for(int i=0; i<3; i++){
    size.push_back(Variable::make(
      ("s"+std::to_string(size_counter++)).c_str()
    ));

    stride.push_back(Variable::make(
      ("st"+std::to_string(stride_counter++)).c_str()
    ));
  
  }

  if(name == "MyTensor")
    name = ("MyTensor" + std::to_string(tensor_counter)).c_str();
  return Tensor::make(3, size, stride, name);

}

int main()
{  
  PrintVisitor printer(std::cout);
  auto T1 = get_tensor("T1", 3);
  auto T2 = get_tensor("T2", 3);
  auto T3 = get_tensor("OutTensor", 3);
  auto eq = Set::make( T3, Add::make(T1, T2) );
  printer.visit(eq.get());
  std::cout<<std::endl;

  //auto A = Variable::make("A");
  //auto B = Variable::make("B");
  //auto C = Variable::make("C");
  //auto div = Div::make(B, Variable::make((size_t)10));
  //auto loop_range = Range::make(A, div);
  //auto iter_var = Variable::make("i");
  //auto my_for = For::make(loop_range, iter_var);
  //printer.visit(my_for.get());
  
}
