#include "interpreter.hpp"

Evaluator::Evaluator() : vars(std::make_shared<Scope<lvalue>>()) {}

stdx::any Evaluator::run(std::vector<std::shared_ptr<Stmt>> &stmt) {
  stdx::any last;
  for (auto &it : stmt) {
    last = run(*it);
  }
  return last;
}
stdx::any Evaluator::run(Stmt &stmt) { return stmt.accept(*this); }

stdx::any Evaluator::visitExpressionStmt(ExpressionStmt &stmt) {
  return stmt.expr->accept(*this);
}
stdx::any Evaluator::visitBlock(Block &block) {
  auto ps = vars;
  vars = std::make_shared<Scope<lvalue>>(vars);
  for (auto &stmt : block.stmts) {
    run(*stmt);
  }
  vars = ps;
  return nullptr;
}
stdx::any Evaluator::visitFun(Fun &) { return nullptr; }
stdx::any Evaluator::visitIf(If &stmt) {
  auto cond = stmt.cond->accept(*this);
  if (isTruthy(cond)) {
    stmt.ifTrue->accept(*this);
  } else if (stmt.ifFalse != nullptr) {
    stmt.ifFalse->accept(*this);
  }
  return nullptr;
}
stdx::any Evaluator::visitPrint(Print &stmt) {
  auto val = stmt.expr->accept(*this);
  std::cout << toString(val) << std::endl;
  return nullptr;
}
stdx::any Evaluator::visitWhile(While &stmt) {
  while (isTruthy(stmt.cond->accept(*this))) {
    stmt.body->accept(*this);
  }
  return nullptr;
}
stdx::any Evaluator::visitVarDecl(VarDecl &decl) {
  auto val = stdx::any_cast<lvalue>(decl.init->accept(*this));
  vars->insert(decl.ident, val);
  return nullptr;
}
stdx::any Evaluator::visitBinop(Binop &op) {
  if (op.op == BinopType::ASSIGN) {
    auto target = std::dynamic_pointer_cast<Variable>(op.lhs);
    if (!target)
      throw "Can't assign to that, stupid";
    double rhs = stdx::any_cast<double>(op.rhs->accept(*this));
    (*vars)[target->ident] = rhs;
    return rhs;
  }
  double lhs = stdx::any_cast<double>(op.lhs->accept(*this));
  double rhs = stdx::any_cast<double>(op.rhs->accept(*this));
  switch (op.op) {
  case BinopType::ADD:
    return lhs + rhs;
  case BinopType::SUB:
    return lhs - rhs;
  case BinopType::MUL:
    return lhs * rhs;
  case BinopType::DIV:
    return lhs / rhs;

  case BinopType::GT:
    return lhs > rhs;
  case BinopType::GE:
    return lhs >= rhs;
  case BinopType::LT:
    return lhs < rhs;
  case BinopType::LE:
    return lhs <= rhs;
  default:
    return 0;
  }
}
stdx::any Evaluator::visitUnop(Unop &) { return nullptr; }
stdx::any Evaluator::visitLiteral(Literal &op) { return op.value; }
stdx::any Evaluator::visitVariable(Variable &v) { return (*vars)[v.ident]; }
stdx::any Evaluator::visitCall(Call &) { return nullptr; }
