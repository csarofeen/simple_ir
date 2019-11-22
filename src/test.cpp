#include <iostream>
#include <string>

#include "Node.h"
#include "Visitors.h"

int main()
{
  Node *list[] = 
  {
    new This(), new That(), new TheOther()
  };

  UpVisitor up;
  DownVisitor down;

  for (int i = 0; i < 3; i++)
    list[i]->accept(up);

  for (int i = 0; i < 3; i++)
    list[i]->accept(down);

}
