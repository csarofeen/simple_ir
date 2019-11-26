#include <iostream>
#include <string>

#include "IR.h"
#include "Visitors.h"

int main()
{
  
  Variable *A = new Variable("A");
  Variable *B = new Variable("B");
  For my_for(A, B);
  PrintVisitor printer;
  my_for.accept(printer);


}
