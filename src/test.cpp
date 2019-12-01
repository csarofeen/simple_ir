#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"

using namespace Fuser;

int main()
{  
  auto A = Variable::make("A");
  auto B = Variable::make("B");
  auto C = Variable::make("C");
  auto div = Div::make(B, Variable::make((size_t)10));
  auto loop_range = Range::make(A, div);
  auto iter_var = Variable::make("i");
  auto my_for = For::make(loop_range, iter_var);
  PrintVisitor printer(std::cout);
  printer.visit(my_for.get());
}
