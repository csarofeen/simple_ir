#include "Printer.h"

std::ostream& Fuser::operator<<(std::ostream& os, const Expr& e) {
    if(!e.defined())
        return os;

    PrintVisitor printer(os);
    e.accept(&printer);
    return os;
}

std::ostream& Fuser::operator<<(std::ostream &os, const std::vector<int> &my_set){
    os<<"{";
    for(auto it = my_set.begin(); it!=my_set.end(); ++it){
        os<<(*it);
        if(it != --my_set.end())
            os<<", ";
    }
    os<<"}";
}