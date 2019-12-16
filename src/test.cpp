#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"
#include "Printer.h"
#include "Mutators.h"
#include "LoopFuser.h"

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
  auto A = get_tensor(3, "A");
  auto B = get_tensor(3, "B");
  auto C = get_tensor(3, "C");

  Expr my_add = Add::make(A, B);
  Expr result = Set::make(C, my_add);
  std::cout<<"Printing a Stmt: "<<result<<std::endl;

  LoopTranslate loop_nest_writer;
  Expr loop_nest = loop_nest_writer.visit(result.as<Set>());
  std::cout<<loop_nest<<std::endl;

  std::vector<Expr> fors = findAll<For>(loop_nest);

  Expr fused = LoopFuser::fuse(loop_nest, fors[0], fors[1]);
  std::cout<<"Fused:\n"<<fused<<std::endl;
  fors = findAll<For>(fused);
  Expr fused2 = LoopFuser::fuse(fused, fors[0], fors[1]);
  std::cout<<"Fused2:\n"<<fused2<<std::endl;

  fors = findAll<For>(fused2);
  auto Split = LoopSplitter::split(fused2, fors[0], IntImm::make(128));
  std::cout<<"Split: "<<Split<<std::endl;
  std::cout<<"\nDone"<<std::endl;

}
