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

void IRVisitor::visit(const Sub *op) {
    op->a.accept(this);
    op->b.accept(this);
}

void IRVisitor::visit(const Mul *op) {
    op->a.accept(this);
    op->b.accept(this);
}

void IRVisitor::visit(const Div *op) {
    op->a.accept(this);
    op->b.accept(this);
}

}