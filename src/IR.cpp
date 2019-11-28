#include "IR.h"
#include "IRVisitor.h"

void For::accept(IRVisitor &v){
  v.visit(this);
}

void Variable::accept(IRVisitor &v){
  v.visit(this);
}

void IR::accept(IRVisitor &v){
  v.visit(this);
}

void Range::accept(IRVisitor &v){
  v.visit(this);
}
