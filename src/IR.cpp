#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"
#include "Printer.h"

#include <algorithm>
#include <set>
#include <ostream>

namespace Fuser{

//For tensor names if one isn't provided, probably want the same for Variables.
int JITTensor::name_count = 0;
int Tensor::name_count = 0;

  Expr Tensor::make(
    const JITTensor *_jittensor,
    Expr origin
  ){
    Tensor *node = new Tensor;
    node->jittensor_ = _jittensor;
    node->name = _jittensor->name.substr(0, 3) == "jit" ? _jittensor->name.substr(3, _jittensor->name.size()-3) : _jittensor->name;
    node->ndims = _jittensor->ndims;
    node->origin = origin;
    for(int i=0; i<_jittensor->ndims; ++i){
      node->domain.push_back(
          Range::make(IntImm::make(0), _jittensor->shapes[i])
      );
    }
    
    return node;
  }

}//Fuser namespace
