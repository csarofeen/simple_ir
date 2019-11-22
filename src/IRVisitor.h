#pragma once
#include "Node.h"

class IRVisitor
{
  public:
    virtual void visit(This *e) = 0;
    virtual void visit(That *e) = 0;
    virtual void visit(TheOther *e) = 0;
};
