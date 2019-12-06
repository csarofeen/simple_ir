#include "IRVisitor.h"

namespace Fuser{

IRVisitor::~IRVisitor() {
}

IRVisitor::IRVisitor() {
}

void IRVisitor::visit(const IntImm *) {
}

void IRVisitor::visit(const Add *op) {
    op->a.accept(this);
    op->b.accept(this);
}

}