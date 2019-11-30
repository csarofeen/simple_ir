#include <iostream>
#include <string>
#include <memory>

#include "IR.h"
#include "Visitors.h"

int main()
{
  Variable *A = new Variable("A");
  Variable *B = new Variable("B");
  Variable *C = new Variable("C");
  Node *div = new Div(B, C);
  Range *loop_range = new Range(A, div);
  Variable *iter_var = new Variable("i");
  For *my_for = new For(loop_range, iter_var);
  PrintVisitor printer;
  my_for->accept(printer);
}
