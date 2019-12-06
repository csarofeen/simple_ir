#pragma once
#include <iostream>
#include "IRMutator.h"

namespace Fuser{

class ClearMutator : public IRMutator {
public:
    /** These methods should call 'include' on the children to only
     * visit them if they haven't been visited already. */
    // @{
    Expr visit(const IntImm *val){
        return IntImm::make(0);
    };

};



}//namespace Fuser