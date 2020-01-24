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
  std::vector<Expr> r = {
    Range::make(IntImm::make(0), IntImm::make(4)),
    Range::make(IntImm::make(0), IntImm::make(8))
  };

  Expr td = TensorDomain::make(Null::make(), r);

  Expr split = Split::make(td, 1, IntImm::make(3));
  std::cout<<"split"<<split<<std::endl<<std::endl;

  Expr merge = Merge::make(split, 1);
  std::cout<<"merge"<<merge<<std::endl;

  std::vector<Expr> r2 = {
    Range::make(IntImm::make(0), IntImm::make(4)),
    Range::make(IntImm::make(0), IntImm::make(8)),
    Range::make(IntImm::make(0), IntImm::make(16))
  };
  
  Expr td2 = TensorDomain::make(Null::make(), r2);

  std::unordered_map<int, int> reorder_map;
  reorder_map[0] = 2;
  reorder_map[2] = 0;
  
  Expr reordered = Reorder::make(td2, reorder_map);
  std::cout<<"Reordered: "<<reordered<<std::endl;
  std::cout<<"\nDone"<<std::endl;

}
