#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"
#include "Printer.h"
#include "Mutators.h"
#include "LoopTransforms.h"
#include "CUDA_Lower.h"
#include "Arith.h"

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

  return JITTensor::make(ndims, size, stride, name);

}

int main(){

  std::cout<<"Start"<<std::endl;

  Expr jitA = get_tensor(3, "A");
  Expr jitB = get_tensor(3, "B");
  Expr jitD = get_tensor(3, "D");

  std::cout<<jitA<<std::endl;  

  Expr C = add(jitA, jitB);
  Expr E = mul(C, jitD);
  std::cout<<E<<std::endl;
  std::cout<<"Try to replace jit tensors:"<<JITLower().mutate(E.as<JITTensor>())<<std::endl;

  std::cout<<"\nDone"<<std::endl;

}


//Tensor 
// -> origin -> expr -> inputs
// -> Tensor Domain -> IRanges, RRanges (iteration and reduction)
// -> Iterators
// -> Predicate