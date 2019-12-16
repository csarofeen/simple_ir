#pragma once
#include <iostream>
#include <algorithm>
#include "IRMutator.h"

namespace Fuser{

class ClearMutator : public IRMutator {

public:
    /** These methods should call 'include' on the children to only
     * visit them if they haven't been visited already. */
    // @{
    Expr visit(const IntImm *val){
        return IntImm::make(0);
    };

};

class LoopTranslate : public IRMutator{
    const char* loop_names = "ijklmnop";

    //!!WARNING!!
    //loops     is inner most loop first!
    //loop_vars is outer most loop first!
    std::vector<Expr> loops;
    std::vector<Expr> loop_vars;
    

public:
    
    Expr visit(const Tensor *op){
        assert(op->ndims == loops.size());
        return TensorAccessor::make(op, loop_vars);       
    }

    Expr visit(const Set *op){
        if(!op->a.as<Tensor>())
            return IRMutator::visit(op);
        
        assert(loops.size() == 0);
        const Tensor* tensor_ref = op->a.as<Tensor>();

        //Create loops, with unset bodies, we will use these to set tensor 
        for(int i=tensor_ref->shapes.size()-1; i>=0; i--){
            std::string loop_name{loop_names[i]};
            Expr loop_var = Variable::make(loop_name.c_str());
            loop_vars.push_back(loop_var);
            if(i == tensor_ref->shapes.size()-1)
                loops.push_back(For::make(IntImm::make(0), tensor_ref->shapes[i], loop_var, IntImm::make(0)));
            else
                loops.push_back(For::make(IntImm::make(0), tensor_ref->shapes[i], loop_var, IntImm::make(0)));
        }

        std::reverse(loop_vars.begin(), loop_vars.end());

        if(loops.size() == 0)
            return Expr();

        auto A = mutate(op->a);
        auto B = mutate(op->b);
        Expr new_body = op;
        if(!A.same_as(op->a) || !B.same_as(op->b))
            new_body = Set::make(A, B);

        //Make the loops for real, new_body will be the body of inner most loop.
        std::vector<Expr> new_loops;
        Expr prev;
        for(auto expr : loops){
            auto loop = expr.as<For>();
            if(expr.same_as(*loops.begin())){
                prev = For::make(loop->min, loop->extent, loop->loop_var, new_body);
                new_loops.push_back(prev);
            }else{
                prev = For::make(loop->min, loop->extent, loop->loop_var, prev);
                new_loops.push_back(prev);
            }
        }

        loops = std::vector<Expr>();
        
        return new_loops.back();

    }

    LoopTranslate(){}

};

}//namespace Fuser