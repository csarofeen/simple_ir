#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <unordered_map>
#include "IntrusivePtr.h"
#include "IRVisitor.h"
#include "IRMutator.h"

/* * * * *
To add a new node T you must:
IR.h
  Add T to IRNodeType
  Add Impl for:
    struct T : public ExprNode<T> {
      static Expr make(){
        T* new_t = new T;
        return new_t;
    }
    static const IRNodeType _node_type = IRNodeType::T;
};

IRVisitor.h
  class T; // forward declaration;
  Add def: void visit(const T* node)

IRMutator.h
  class T; // forward declaration;
  Add def: Expr visit(const T* node)

IRMutator.cpp
  Add def: Expr visit(const T* node)

IRVisitor.cpp
  Add def: void visit(const T* node)
* * * * */

namespace Fuser{

enum class IRNodeType {
    BinaryOp,
    UnaryOp,
    Set,

    Variable,
    IntImm,

    For,
    If,

    Range,
    Attr,
    Merge,
    Split,
    Reorder,

    TensorDomain,
    TensorAccessor,
    Tensor,
    JITTensor,

    Thread, //Threads that can be bound
    Block,  //A group of exprs
    Null
};

enum class BinaryOpType {
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  LT
};

enum class UnaryOpType {
  SumReduction,
  ProdReduction
};

/** The abstract base classes for a node in the Halide IR. */
struct IRNode {

    /** We use the visitor pattern to traverse IR nodes throughout the
     * compiler, so we have a virtual accept method which accepts
     * visitors.
     */
    virtual void accept(IRVisitor *v) const = 0;
    
    IRNode(IRNodeType t)
        : node_type(t) {
    }
    virtual ~IRNode() = default;

    /** These classes are all managed with intrusive reference
     * counting, so we also track a reference count. It's mutable
     * so that we can do reference counting even through const
     * references to IR nodes.
     */
    mutable RefCount ref_count;

    /** Each IR node subclass has a unique identifier. We can compare
     * these values to do runtime type identification. We don't
     * compile with rtti because that injects run-time type
     * identification stuff everywhere (and often breaks when linking
     * external libraries compiled without it), and we only want it
     * for IR nodes. One might want to put this value in the vtable,
     * but that adds another level of indirection, and for Exprs we
     * have 32 free bits in between the ref count and the Type
     * anyway, so this doesn't increase the memory footprint of an IR node.
     */
    IRNodeType node_type;
};


template<>
inline RefCount &ref_count<IRNode>(const IRNode *t) noexcept {
    return t->ref_count;
}

template<>
inline void destroy<IRNode>(const IRNode *t) {
    delete t;
}
struct Expr; 

struct BaseExprNode : public IRNode {
    BaseExprNode(IRNodeType t)
        : IRNode(t) {
    }
    virtual Expr mutate_expr(IRMutator *v) const = 0;
    //Type type;
};

struct IRHandle : public IntrusivePtr<const IRNode> {
    IRHandle() = default;

    IRHandle(const IRNode *p)
        : IntrusivePtr<const IRNode>(p) {
    }

    /** Dispatch to the correct visitor method for this node. E.g. if
     * this node is actually an Add node, then this will call
     * IRVisitor::visit(const Add *) */
    void accept(IRVisitor *v) const {
        ptr->accept(v);
    }

    /** Downcast this ir node to its actual type (e.g. Add, or
     * Select). This returns nullptr if the node is not of the requested
     * type. Example usage:
     *
     * if (const Add *add = node->as<Add>()) {
     *   // This is an add node
     * }
     */
    template<typename T>
    const T *as() const {
      if (ptr && ptr->node_type == T::_node_type) {
          return (const T *)ptr;
      }
      throw std::runtime_error(std::string("Bad node type cast. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));
      return nullptr;
    }

    template<typename T>
    bool is() const {
      if (ptr && ptr->node_type == T::_node_type) {
          return true;
      }
      return false;
    }

    IRNodeType node_type() const {
        return ptr->node_type;
    }
};

struct Expr : public IRHandle {

  //Did not take the type system from Halide, it is there.

    /** Make an undefined expression */
    Expr() = default;

    /** Make an expression from a concrete expression node pointer (e.g. Add) */
    Expr(const BaseExprNode *n)
        : IRHandle(n) {
    }

    bool operator== (const Expr& other) const{
      return same_as(other);
    }
    
    bool operator<(const Expr& other) const {
      return this->get() < other.get();
    }

    /** Override get() to return a BaseExprNode * instead of an IRNode * */
    const BaseExprNode *get() const {
        return (const BaseExprNode *)ptr;
    }

};

template<typename T>
struct ExprNode : public BaseExprNode {
    void accept(IRVisitor *v) const override {
      v->visit(static_cast<const T*>(this));
    };
    Expr mutate_expr(IRMutator *v) const override {
      return v->visit(static_cast<const T*>(this));
    }
    ExprNode()
        : BaseExprNode(T::_node_type) {
    }
    virtual ~ExprNode() = default;
};

struct ExprHash {
    size_t operator()(const Expr &a) const {
        return std::hash<size_t>()((size_t) (a.get()));
    }
};

// TODO(REVIEW): should we move all the nodes below somehwere else?
// so adding a new node is not rebuilding the universe.

struct Null : public ExprNode<Null> {
  public:
    static Expr make(){ return new Null;}
    static const IRNodeType _node_type = IRNodeType::Null;
};

struct IntImm : public ExprNode<IntImm> {
    int64_t value;

    static const IntImm *make(int64_t value){//Type t, int64_t value) {
        IntImm *node = new IntImm;
        node->value = value;
        return node;
    }

    static const IRNodeType _node_type = IRNodeType::IntImm;
};

struct BinaryOp : public ExprNode<BinaryOp> {
    Expr a, b;

    static Expr make(Expr a, Expr b, BinaryOpType op_type){
      BinaryOp *op = new BinaryOp;
      op->a = a;
      op->b = b;
      op->op_type = op_type;
      return op;
    }
    BinaryOpType op_type;
    static const IRNodeType _node_type = IRNodeType::BinaryOp;
};

struct Variable : public ExprNode<Variable> {
  std::string name;

  static Expr make(const char* name){
    Variable* node = new Variable;
    node->name = name;
    return node;
  }

  static const IRNodeType _node_type = IRNodeType::Variable;

};

struct Range: public ExprNode<Range>{
public:

static Expr make(Expr min, Expr extent, bool reduction_domain = false){
    Range *node = new Range;
    node->min = min;
    node->extent = extent;
    node->reduction = reduction_domain;
    return node;
}

  static const IRNodeType _node_type = IRNodeType::Range;
  Expr min, extent;
  bool reduction;
};

struct JITTensor: public ExprNode<JITTensor>{
static int name_count;

public:

static Expr make(
  int ndims,
  std::vector<Expr> shapes,
  std::vector<Expr> strides,
  const char* name = ""
  ){
    JITTensor *node = new JITTensor;
    node->ndims = ndims;
    node->shapes = shapes;
    node->strides = strides;
    node->origin = Null::make();
    node->name = name == "" ? "jittensor"+std::to_string(name_count++) : name;
    return node;
}

static Expr make(
    const JITTensor *ref,
    Expr origin
  ){
    JITTensor *node = new JITTensor;
    node->origin = origin;
    node->ndims = ref->ndims;
    node->shapes = ref->shapes;
    node->strides = ref->strides;
    node->name = "jittensor"+std::to_string(name_count++);
    return node;
}

  std::vector<Expr> shapes;
  std::vector<Expr> strides;
    std::string name = "";
  Expr origin;
  static const IRNodeType _node_type = IRNodeType::JITTensor;
  int ndims = 0;

};


struct Tensor: public ExprNode<Tensor>{
static int name_count;

public:

  static Expr make(
    const JITTensor *_jittensor,
    Expr origin = Null::make()
  );

static Expr make(
    const Tensor *ref,
    Expr origin
  ){
    Tensor *node = new Tensor;
    node->origin = origin;
    node->ndims = ref->ndims;
    node->jittensor_ = ref->jittensor_;
    node->domain = ref->domain;
    node->name = "tensor"+std::to_string(name_count++);
    return node;
}

  std::vector<Expr> domain;
  std::vector<Expr> indexers;
  Expr jittensor_;
  std::string name = "";
  Expr origin;
  static const IRNodeType _node_type = IRNodeType::Tensor;
  int ndims = 0;
};

struct TensorAccessor: public ExprNode<TensorAccessor>{

public:
static Expr make(Expr tensor, std::vector<Expr> indexers){
    TensorAccessor *node = new TensorAccessor;
    node->tensor = tensor;
    node->indexers = indexers;
    return node;
}

  static const IRNodeType _node_type = IRNodeType::TensorAccessor;
  Expr tensor;
  std::vector<Expr> indexers;
};

struct TensorDomain : public ExprNode<TensorDomain>{
  public:
  static Expr make(
    Expr origin,
    std::vector<Expr> domain
  ){
    TensorDomain *td = new TensorDomain();
    td->origin = origin;
    td->domain = domain;
    return td;
  }

  int ndims() const {return domain.size();}  
  Expr origin;
  std::vector<Expr> domain;
  static const IRNodeType _node_type = IRNodeType::TensorDomain;
};

struct Split : public ExprNode<Split>{
  public:
    static const IRNodeType _node_type = IRNodeType::Split;

    Expr original_domain;
    int axis;
    Expr factor;
    
  static Expr make(
      Expr original_domain,
      int axis,
      Expr factor
  ){
      Split *split = new Split;
      split->original_domain = original_domain;
      split->axis = axis;
      split->factor = factor;
      return split;  
  }

};

struct Merge : public ExprNode<Merge>{
public:
  static const IRNodeType _node_type = IRNodeType::Merge;

  Expr original_domain;
  int axis;
  
  static Expr make(
      Expr original_domain,
      int axis
  ){
      Merge *merge = new Merge;
      merge->original_domain = original_domain;
      merge->axis = axis;
      return merge;
  }

};

struct Reorder : public ExprNode<Reorder>{
public:
  static const IRNodeType _node_type = IRNodeType::Reorder;

  Expr original_domain;
  //pos2axis[new_position] = old_position
  std::vector<int> pos2axis;
  

  static Expr make(
      Expr original_domain,
      std::vector<int> pos2axis
  ){
      Reorder *reorder = new Reorder;
      reorder->original_domain = original_domain;
      reorder->pos2axis = pos2axis;
      return reorder;
  }

};

struct For: public ExprNode<For>{
public:
static Expr make(Expr range, Expr loop_var, Expr body){
    For *node = new For;
    node->range = range;
    node->loop_var = loop_var;
    node->body = body;
    return node;
}
  static const IRNodeType _node_type = IRNodeType::For;
  Expr range, body, loop_var;
};

struct Thread: public ExprNode<Thread>{
public:
enum class THREAD_TYPE{
  TIDx,
  TIDy,
  TIDz,
  BIDx,
  BIDy,
  BIDz,
  Null
};


static Expr make(THREAD_TYPE thread_type){
    auto node = new Thread;
    node->thread_type = thread_type;
    return node;
}
  
  THREAD_TYPE thread_type = THREAD_TYPE::Null;
  static const IRNodeType _node_type = IRNodeType::Thread;

};

struct Attr: public ExprNode<Attr>{
public:
enum class ATTR_TYPE{
  ThreadBinding,
  Null
};

static Expr make(ATTR_TYPE attribute_type, Expr body, Expr value){
    Attr *node = new Attr;
    node->attr_type = attribute_type;
    node->body = body;
    node->value = value;
    return node;
}
  
  static const IRNodeType _node_type = IRNodeType::Attr;
  //Type of attribute
  ATTR_TYPE attr_type = ATTR_TYPE::Null;
  //The attribute we're settinng
  Expr value;
  //What we're making an attribute for
  Expr body;

};


struct If: public ExprNode<If>{
public:
static Expr make(Expr pred, Expr body){
    If *node = new If;
    node->pred = pred;
    node->body = body;
    return node;
}
  static const IRNodeType _node_type = IRNodeType::If;
  Expr pred, body;
};

class Block : public ExprNode<Block> {
 public:
  static Expr make(const std::vector<Expr>& exprs) { return new Block(exprs); }
  explicit Block(const std::vector<Expr>& exprs) : exprs(exprs) {}
  std::vector<Expr> exprs;
  static const IRNodeType _node_type = IRNodeType::Block;

};

}//Fuser namspace
