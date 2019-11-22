#include <string>
#pragma once

class IRVisitor;

class Node
{
  public:
    virtual void accept(class IRVisitor &v) = 0;
};


class This: public Node
{
  public:
    void accept(IRVisitor &v);
    std::string thiss()
    {
        return "This";
    }
};

class That: public Node
{
  public:
    void accept(IRVisitor &v);
    std::string that()
    {
        return "That";
    }
};

class TheOther: public Node
{
  public:
    void accept(IRVisitor &v);
    std::string theOther()
    {
        return "TheOther";
    }
};
