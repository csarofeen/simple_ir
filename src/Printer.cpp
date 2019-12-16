#include "Printer.h"

std::ostream& Fuser::operator<<(std::ostream& os, const Expr& e) {
    PrintVisitor printer(os);
    e.accept(&printer);
    return os;
}
