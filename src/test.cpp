#include <iostream>
#include <string>
#include <memory>

//#include "IR.h"
//#include "Visitors.h"
#include "IntrusivePtr.h"
using namespace Fuser;

enum IRNodeType{
  ADD,
  NULL_TYPE
};


struct IRNode {
    //virtual void accept(IRVisitor *v) const = 0;
    IRNode(IRNodeType t)
        : node_type(t) {
    }
    virtual ~IRNode() = default;
    mutable RefCount ref_count;
    IRNodeType node_type;
};

template<>
inline RefCount &Fuser::ref_count<IRNode>(const IRNode *t) noexcept {
    return t->ref_count;
}

template<>
inline void Fuser::destroy<IRNode>(const IRNode *t) {
    delete t;
}

class Expr;
struct BaseExprNode : public IRNode {
    BaseExprNode(IRNodeType t)
        : IRNode(t) {
    }
};

template<typename T>
struct ExprNode : public BaseExprNode {
    //void accept(IRVisitor *v) const override;
    //Expr mutate_expr(IRMutator *v) const override;
    ExprNode()
        : BaseExprNode(T::_node_type) {
    }
    virtual ~ExprNode() = default;
};


struct IRHandle : public IntrusivePtr<const IRNode> {
    IRHandle() = default;

    IRHandle(const IRNode *p)
        : IntrusivePtr<const IRNode>(p) {
    }

//    void accept(IRVisitor *v) const {
//        ptr->accept(v);
//    }

    template<typename T>
    const T *as() const {
        if (ptr && ptr->node_type == T::_node_type) {
            return (const T *)ptr;
        }
        return nullptr;
    }

    IRNodeType node_type() const {
        return ptr->node_type;
    }
};

struct Expr : public IRHandle {
    /** Make an undefined expression */
    Expr() = default;

    /** Make an expression from a concrete expression node pointer (e.g. Add) */
    Expr(const BaseExprNode *n)
        : IRHandle(n) {
    }

    explicit Expr(int8_t x)
        //: IRHandle(Internal::IntImm::make(Int(8), x)) 
        {
    }
};

struct Add : public ExprNode<Add> {
    static Expr make(){
      Add *node = new Add;
      return node;
    }
    static const IRNodeType _node_type = IRNodeType::ADD;
};

int main(){
  std::cout<<"Start"<<std::endl;
  auto my_add = Add::make();
  std::cout<<"Done"<<std::endl;

}


/* Testing out tensors/general IR structure
int   size_counter = 0;
int stride_counter = 0;
int tensor_counter = 0;

std::shared_ptr<Tensor> get_tensor(const char* name="MyTensor", int ndims = 3){
  std::vector<std::shared_ptr<Node> > size;
  std::vector<std::shared_ptr<Node> > stride;

  for(int i=0; i<3; i++){
    size.push_back(Variable::make(
      ("s"+std::to_string(size_counter++)).c_str()
    ));

    stride.push_back(Variable::make(
      ("st"+std::to_string(stride_counter++)).c_str()
    ));
  
  }

  if(name == "MyTensor")
    name = ("MyTensor" + std::to_string(tensor_counter)).c_str();
  return Tensor::make(3, size, stride, name);

}

int main()
{  
  PrintVisitor printer(std::cout);
  auto T1 = get_tensor("T1", 3);
  auto T2 = get_tensor("T2", 3);
  auto T3 = get_tensor("OutTensor", 3);
  auto eq = Set::make( T3, Add::make(T1, T2) );
  printer.visit(eq.get());
  std::cout<<std::endl;

  //auto A = Variable::make("A");
  //auto B = Variable::make("B");
  //auto C = Variable::make("C");
  //auto div = Div::make(B, Variable::make((size_t)10));
  //auto loop_range = Range::make(A, div);
  //auto iter_var = Variable::make("i");
  //auto my_for = For::make(loop_range, iter_var);
  //printer.visit(my_for.get());
  
}
*/