#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"

#include <algorithm>
#include <set>
#include <ostream>

namespace Fuser{

//For tensor names if one isn't provided, probably want the same for Variables.
int JITTensor::name_count = 0;

Expr Merge::make(
    Expr original_domain,
    int axis
){

    const TensorDomain *td = original_domain.as<TensorDomain>();
    if (!(axis >= 0 && axis + 1 < td->domain.size()))
        throw std::runtime_error(std::string("Invalid axis for merge. ") + std::string(__FILE__) + " : " + std::to_string(__LINE__));

    if (axis < 0)
        axis += td->domain.size();

    if (!(td->domain[axis].as<Range>()->min.is<IntImm>() || td->domain[axis].as<Range>()->min.as<IntImm>()->value == 0) || !(td->domain[axis + 1].as<Range>()->min.is<IntImm>() || td->domain[axis + 1].as<Range>()->min.as<IntImm>()->value == 0))
        throw std::runtime_error(std::string("Merge with domain->min != 0 not implemented. ") + std::string(__FILE__) + " : " + std::to_string(__LINE__));

    std::vector<Expr> dom;
    for (int i = 0; i < td->domain.size(); i++)
    {
        if (i != axis && i != axis + 1)
        {
            dom.push_back(
                Range::make(
                    td->domain[i].as<Range>()->min,
                    td->domain[i].as<Range>()->extent));
        }
        else if (i == axis)
        {
            dom.push_back(
                Range::make(
                    IntImm::make(0),
                    Mul::make(td->domain[i].as<Range>()->extent, td->domain[i + 1].as<Range>()->extent)));
        }
    }

    Merge *merge = new Merge;
    merge->original_domain = original_domain;
    merge->axis = axis;

    Expr mergeDomain = TensorDomain::make(
        merge,
        dom);
    return mergeDomain;
}

Expr Split::make(
    Expr original_domain,
    int axis,
    Expr factor
){

    if (!factor.is<IntImm>())
    throw std::runtime_error(std::string("Not implemented. ") + std::string(__FILE__) + " : " + std::to_string(__LINE__));

    const TensorDomain *td = original_domain.as<TensorDomain>();
    if (!(axis >= 0 && axis < td->domain.size()))
        throw std::runtime_error(std::string("Invalid axis for split. ") + std::string(__FILE__) + " : " + std::to_string(__LINE__));

    if (axis < 0)
        axis += td->domain.size();

    if (!(td->domain[axis].as<Range>()->min.is<IntImm>() || td->domain[axis].as<Range>()->min.as<IntImm>()->value == 0))
        throw std::runtime_error(std::string("Not implemented. ") + std::string(__FILE__) + " : " + std::to_string(__LINE__));

    std::vector<Expr> dom;
    for (int i = 0; i < td->domain.size(); i++)
    {
        if (i != axis)
        {
            dom.push_back(
                Range::make(
                    td->domain[i].as<Range>()->min,
                    td->domain[i].as<Range>()->extent));
        }
        else
        {
            dom.push_back(
                Range::make(
                    //TODO: Support split of axis where min!=0
                    IntImm::make(0),
                    Add::make(
                        Div::make(
                            Sub::make(td->domain[i].as<Range>()->extent, IntImm::make(1)),
                            factor),
                        IntImm::make(1))));

            dom.push_back(
                Range::make(
                    //TODO: Support split of axis where min!=0
                    IntImm::make(0),
                    Mod::make(td->domain[i].as<Range>()->extent, factor)));
        }
    }

    Split *split = new Split;
    split->original_domain = original_domain;
    split->axis = axis;
    split->factor = factor;

    Expr splitDomain = TensorDomain::make(
        split,
        dom);
    return splitDomain;
    
}

Expr Reorder::make(
    Expr original_domain,
    std::unordered_map<int, int> axis2pos
){
    //Make sure domain is a domain.
    assert(original_domain.is<TensorDomain>());

    int ndims = original_domain.as<TensorDomain>()->ndims();
    
    //Node to record this reorder operation.
    Reorder *reorder = new Reorder;
    reorder->original_domain = original_domain;
    //Map to save from previous order, to new order.
    reorder->pos2axis = std::vector<int>(ndims, -1);

    //Go through each new old and new position, make sure they're within 0-ndims
    for(std::pair<int, int> elem : axis2pos){
        int old_pos = elem.first;
        int new_pos = elem.second;

        if(old_pos < 0)
            old_pos += ndims;
        if(new_pos < 0)
            new_pos += ndims;

        assert(old_pos >= 0 && old_pos < ndims && new_pos >= 0 && new_pos < ndims);

        if(reorder->pos2axis[new_pos] != -1)
            throw std::runtime_error(std::string("Fatal error: found overlapped positions in reorder. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));

        reorder->pos2axis[new_pos] = old_pos;

    }

    std::set<int> old_positions(reorder->pos2axis.begin(), reorder->pos2axis.end());
    old_positions.erase(-1);
    
    if(old_positions.size() != axis2pos.size())
        throw std::runtime_error(std::string("Fatal error: found overlapped positions in reorder. ") + std::string( __FILE__ ) + " : " + std::to_string(__LINE__));

    
    std::set<int> all_positions;
    for(int i=0; i<ndims; i++) all_positions.insert(i);


    std::set<int> positions_left;
    std::set_difference(
        all_positions.begin(), all_positions.end(),
        old_positions.begin(), old_positions.end(),
        std::inserter(positions_left, positions_left.end())
    );

    //reorder->pos2axis[new_position] = old_position
    auto it = positions_left.begin(); //old positions left
    for(int i=0; i<reorder->pos2axis.size(); i++){
        if(reorder->pos2axis[i] == -1)
            reorder->pos2axis[i] = *it++;
    }

    std::vector<Expr> new_ranges;

    for(int i=0; i<reorder->pos2axis.size(); i++){
        new_ranges.push_back(original_domain.as<TensorDomain>()->domain[
            reorder->pos2axis[i]
        ]);
    }
    
    
    Expr reorderedDomain = TensorDomain::make(
        reorder,
        new_ranges);


    return reorderedDomain;
}

}//Fuser namespace
