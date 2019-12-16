#pragma once
#include "IRVisitor.h"
#include "Printer.h"
#include <unordered_map>
#include <set>

namespace Fuser
{

class ValidateLoopsFusible : public IRVisitor
{
bool outer_found = false;
public:
    bool valid = false;
    Expr outer;
    Expr inner;

    void visit(const For *op)
    {
        if (Expr(op).same_as(outer)){
            outer_found = true;
        //outer and inner need to be exclusively next to eachother, nested.
        }else if (Expr(op).same_as(inner) && outer_found){
            valid = true;
        }else{
            outer_found = false;
        }
        IRVisitor::visit( (const Expr*) &(op->body));
    }

    //First bool is if they can be fused
    //Second bool is if outer and inner are swapped (can still be fused)
    static std::pair<bool, bool> check(Expr container, Expr outer_loop, Expr inner_loop)
    {
        ValidateLoopsFusible checker;
        checker.outer = outer_loop;
        checker.inner = inner_loop;

        container.accept(&checker);

        if (checker.valid)
            return std::pair<bool, bool>(checker.valid, false);

        checker.outer = inner_loop;
        checker.inner = outer_loop;
        container.accept(&checker);

        if (checker.valid)
            return std::pair<bool, bool>(checker.valid, true);

        return std::pair<bool, bool>(false, false);
    }
};

class LoopFuser : public IRMutator
{
std::unordered_map<Expr, Expr, ExprHash> replace;
std::set<Expr> remove;
Expr outer, inner;
friend Expr fuse(Expr container, Expr outer_loop, Expr inner_loop);
public:

    Expr visit(const For* op){
        {
            auto itr = replace.find(op);
            if(itr!=replace.end()){
                return IRMutator::mutate(itr->second);
            }
        }
        {
            auto itr = remove.find(op);
            if(itr!=remove.end()){
                return IRMutator::mutate(op->body);
            }
        }        
        return IRMutator::visit(op);
    }

    Expr visit(const Variable* op){
        auto itr = replace.find(op);
        if(itr!=replace.end()){
            return IRMutator::mutate(itr->second);
        }
        return IRMutator::visit(op);
    }

    //for(i=0;i<I;i++)
    //  a[i]
    //  for(j=0;j<J;j++)
    //      b[i, j]

    //for(k=0; k<I*J; k++)
    //a[k/J]
    //b[k/J, k%J]
    static Expr fuse(Expr container, Expr outer_loop, Expr inner_loop){
        auto can_be_fused = ValidateLoopsFusible::check(container, outer_loop, inner_loop);
        if(!can_be_fused.first){
            std::cerr<<"Could not fuse loops. Loops must be adjacent for fusion."<<std::endl;
            return Expr();
        }

        LoopFuser fuser;
        if(!can_be_fused.second){
            fuser.outer = outer_loop;
            fuser.inner = inner_loop;
        }else{
            fuser.outer = inner_loop;
            fuser.inner = outer_loop;
        }
        auto ol = outer_loop.as<For>();
        auto il = inner_loop.as<For>();
        if(ol->min.as<IntImm>()->value != 0
        || il->min.as<IntImm>()->value != 0){
            std::cerr<<"Could not fuse loops. Loop fusion requires loops starting at 0."<<std::endl;
            return Expr();
        }
        std::string new_name = ol->loop_var.as<Variable>()->name + "." + il->loop_var.as<Variable>()->name + ".fused";
        Expr new_loop_var = Variable::make(new_name.c_str());
        Expr fused_loop = For::make(
            IntImm::make(0),
            Mul::make(ol->extent, il->extent),
            new_loop_var,
            ol->body);
        fuser.replace.emplace(ol, fused_loop);
        fuser.replace.emplace(ol->loop_var, Div::make(new_loop_var, ol->extent));
        fuser.replace.emplace(il->loop_var, Mod::make(new_loop_var, il->extent));
        fuser.remove.emplace(inner_loop);
        return fuser.mutate(container);
    }

};


//for(i=0;i<I;i++)
//  a[i]

//for(j=0; j<I/S; j++)
    //for(k=0; k<S ; k++)
        //if(j*S+k<I)
            //a[j*S+k]
class LoopSplitter : public IRMutator
{
friend Expr split(Expr container, Expr loop, Expr split_factor);
Expr loop;
Expr split_factor;
bool loop_found = false;
Expr ol, il;
public:
    Expr split(){
        auto l = loop.as<For>();
        auto lv = l->loop_var.as<Variable>();
        Expr lov = Variable::make( (lv->name+".outer").c_str());
        Expr liv = Variable::make( (lv->name+".inner").c_str());
        //if(lov * split_factor + liv < loop->extent){}
        Expr pred = If::make(LT::make(Add::make( Mul::make(lov, split_factor), liv ), l->extent), l->body);
        il = For::make(IntImm::make(0), split_factor, liv, pred);
        Expr extent = Add::make(l->extent, split_factor);
        extent = Sub::make(extent, IntImm::make(1));
        extent = Div::make(extent, split_factor);
        ol = For::make(IntImm::make(0), extent, lov, il);
        return ol;
    }

    Expr visit(const For* op){
        if(loop.same_as(op)){
            loop_found=true;
            return IRMutator::visit(split().as<For>());
        }
        return IRMutator::visit(op);
    }

    Expr visit(const Variable* op){
        if(loop.as<For>()->loop_var.same_as(op)){
            return Add::make( Mul::make(ol.as<For>()->loop_var , split_factor), il.as<For>()->loop_var );
        }
        return IRMutator::visit(op);
    }

    static Expr split(Expr container, Expr loop, Expr split_factor){

        LoopSplitter splitter;
        splitter.loop = loop;
        splitter.split_factor = split_factor;
        return splitter.mutate(container);
    }

};



} // namespace Fuser
