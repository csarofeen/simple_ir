#pragma once
#include "IRVisitor.h"

class UpVisitor: public IRVisitor
{
    void visit(This *e)
    {
      std::cout << "do Up on " + e->thiss() << '\n';
    }
    void visit(That *e)
    {
      std::cout << "do Up on " + e->that() << '\n';
    }
    void visit(TheOther *e)
    {
      std::cout << "do Up on " + e->theOther() << '\n';
    }
};

class DownVisitor: public IRVisitor
{
    void visit(This *e)
    {
      std::cout << "do Down on " + e->thiss() << '\n';
    }
    void visit(That *e)
    {
      std::cout << "do Down on " + e->that() << '\n';
    }
    void visit(TheOther *e)
    {
      std::cout << "do Down on " + e->theOther() << '\n';
    }
};
