#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"

int main()
{
  
  Variable *A = new Variable("A");
  Variable *B = new Variable("B");
  Range *loop_range = new Range(A, B);
  Variable *iter_var = new Variable("i");
  For *my_for = new For(loop_range, iter_var);
  PrintVisitor printer;
  my_for->accept(printer);


}
