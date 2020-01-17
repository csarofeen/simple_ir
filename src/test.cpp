#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"
#include "Printer.h"
#include "Mutators.h"
#include "LoopTransforms.h"
#include "CUDA_Lower.h"

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

  std::cout<<std::endl;
  std::cout<<"\nDone"<<std::endl;

}
