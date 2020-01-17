#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"

namespace Fuser{

//For tensor names if one isn't provided, probably want the same for Variables.
int JITTensor::name_count = 0;

}//Fuser namespace
