#pragma once
#include <iostream>
#include <algorithm>
#include "IRMutator.h"

namespace Fuser{

//Dummy mutator.
class ClearMutator : public IRMutator {

public:
    Expr visit(const IntImm *val){
        return IntImm::make(0);
    };

};

class JITLower : public IRMutator {

public:

    Expr visit(const JITTensor *tensor){
        if(tensor->origin.is<Null>())
            return Tensor::make(tensor, Null::make());
        
        return Tensor::make(tensor, mutate(tensor->origin));
    }

};


}//namespace Fuser