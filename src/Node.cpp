#include "Node.h"
#include "IRVisitor.h"

void This::accept(IRVisitor &v)
{
  v.visit(this);
}

void That::accept(IRVisitor &v)
{
  v.visit(this);
}

void TheOther::accept(IRVisitor &v)
{
  v.visit(this);
}
