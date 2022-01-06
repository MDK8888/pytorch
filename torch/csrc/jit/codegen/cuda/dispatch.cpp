#include <torch/csrc/jit/codegen/cuda/fusion.h>
#include <torch/csrc/jit/codegen/cuda/ir_all_nodes.h>
#include <torch/csrc/jit/codegen/cuda/type.h>

#include <torch/csrc/jit/codegen/cuda/dispatch.h>

namespace torch {
namespace jit {
namespace fuser {
namespace cuda {

template <typename T>
T* ptr(T& obj) {
  return &obj;
}

template <typename T>
T* ptr(T* obj) {
  return obj;
}

/*
 * Generic dispatch for any handler that does not modify the IR directly.
 * For example we may want to walk the graph to construct a topologically sorted
 * set of exprs. This doesn't modify the IR directly. We also use this to print
 * the IR itself.
 * This dispatch is paired with a class that implements the functions:
 * template <typenname node_type>
 * int handler(node_type* node)
 *
 * handler should call:
 * dispatch(this, node_to_dispatch)
 *
 * It could also implement:
 * int handler(Statement* stmt){
 *   dispatch(this, stmt);
 * }
 *
 * And therefore dispatch should never call:
 * ptr(mutator)->mutate(this->as<Statement>());
 */

template <typename T>
void Val::dispatch(T handler, Val* val) {
  switch (*(val->getValType())) {
    case ValType::Scalar:
      switch (*(val->getDataType())) {
        case DataType::Bool:
          ptr(handler)->handle(val->as<Bool>());
          return;
        case DataType::Double:
          ptr(handler)->handle(val->as<Double>());
          return;
        case DataType::Int:
          ptr(handler)->handle(val->as<Int>());
          return;
        default:
          break;
      }
      break;
    case ValType::NamedScalar:
      ptr(handler)->handle(val->as<NamedScalar>());
      return;

    case ValType::IterDomain:
      ptr(handler)->handle(val->as<IterDomain>());
      return;
    case ValType::TensorDomain:
      ptr(handler)->handle(val->as<TensorDomain>());
      return;
    case ValType::TensorView:
      ptr(handler)->handle(val->as<TensorView>());
      return;
    case ValType::Predicate:
      ptr(handler)->handle(val->as<kir::Predicate>());
      return;
    case ValType::TensorIndex:
      ptr(handler)->handle(val->as<kir::TensorIndex>());
      return;
    default:
      break;
  }
  TORCH_INTERNAL_ASSERT(false, "Unknown valtype in dispatch!");
}

template <typename T>
void Expr::dispatch(T handler, Expr* expr) {
  switch (*(expr->getExprType())) {
    case ExprType::UnaryOp:
      ptr(handler)->handle(expr->as<UnaryOp>());
      return;
    case ExprType::BinaryOp:
      ptr(handler)->handle(expr->as<BinaryOp>());
      return;
    case ExprType::TernaryOp:
      ptr(handler)->handle(expr->as<TernaryOp>());
      return;
    case ExprType::ReductionOp:
      ptr(handler)->handle(expr->as<ReductionOp>());
      return;
    case ExprType::WelfordOp:
      ptr(handler)->handle(expr->as<WelfordOp>());
      return;
    case ExprType::BroadcastOp:
      ptr(handler)->handle(expr->as<BroadcastOp>());
      return;

    case ExprType::Split:
      ptr(handler)->handle(expr->as<Split>());
      return;
    case ExprType::Merge:
      ptr(handler)->handle(expr->as<Merge>());
      return;
    case ExprType::TransposeOp:
      ptr(handler)->handle(expr->as<TransposeOp>());
      return;
    case ExprType::ShiftOp:
      ptr(handler)->handle(expr->as<ShiftOp>());
      return;
    case ExprType::GatherOp:
      ptr(handler)->handle(expr->as<GatherOp>());
      return;
    case ExprType::ViewOp:
      ptr(handler)->handle(expr->as<ViewOp>());
      return;

    case ExprType::Allocate:
      ptr(handler)->handle(expr->as<kir::Allocate>());
      return;
    case ExprType::Sync:
      ptr(handler)->handle(expr->as<kir::Sync>());
      return;
    case ExprType::InitMagicZero:
      ptr(handler)->handle(expr->as<kir::InitMagicZero>());
      return;
    case ExprType::UpdateMagicZero:
      ptr(handler)->handle(expr->as<kir::UpdateMagicZero>());
      return;
    case ExprType::ForLoop:
      ptr(handler)->handle(expr->as<kir::ForLoop>());
      return;
    case ExprType::IfThenElse:
      ptr(handler)->handle(expr->as<kir::IfThenElse>());
      return;
    case ExprType::GridReduction:
      ptr(handler)->handle(expr->as<kir::GridReduction>());
      return;
    case ExprType::GridBroadcast:
      ptr(handler)->handle(expr->as<kir::GridBroadcast>());
      return;
    case ExprType::GridWelford:
      ptr(handler)->handle(expr->as<kir::GridWelford>());
      return;
    default:
      TORCH_INTERNAL_ASSERT(false, "Unknown exprtype in dispatch!");
  }
}

template <typename T>
void Statement::dispatch(T handler, Statement* stmt) {
  if (stmt->isVal()) {
    ptr(handler)->handle(stmt->as<Val>());
  } else if (stmt->isExpr()) {
    ptr(handler)->handle(stmt->as<Expr>());
  } else
    TORCH_INTERNAL_ASSERT(false, "Unknown stmttype in dispatch!");
}

template <typename T>
void Val::constDispatch(T handler, const Val* val) {
  switch (*(val->getValType())) {
    case ValType::Scalar:
      switch (*(val->getDataType())) {
        case DataType::Bool:
          ptr(handler)->handle(val->as<Bool>());
          return;
        case DataType::Double:
          ptr(handler)->handle(val->as<Double>());
          return;
        case DataType::Int:
          ptr(handler)->handle(val->as<Int>());
          return;
        default:
          break;
      }
      break;
    case ValType::NamedScalar:
      ptr(handler)->handle(val->as<NamedScalar>());
      return;

    case ValType::IterDomain:
      ptr(handler)->handle(val->as<IterDomain>());
      return;
    case ValType::TensorDomain:
      ptr(handler)->handle(val->as<TensorDomain>());
      return;
    case ValType::TensorView:
      ptr(handler)->handle(val->as<TensorView>());
      return;
    case ValType::Predicate:
      ptr(handler)->handle(val->as<kir::Predicate>());
      return;
    case ValType::TensorIndex:
      ptr(handler)->handle(val->as<kir::TensorIndex>());
      return;
    default:
      break;
  }
  TORCH_INTERNAL_ASSERT(false, "Unknown valtype in dispatch!");
}

template <typename T>
void Expr::constDispatch(T handler, const Expr* expr) {
  switch (*(expr->getExprType())) {
    case ExprType::UnaryOp:
      ptr(handler)->handle(expr->as<UnaryOp>());
      return;
    case ExprType::BinaryOp:
      ptr(handler)->handle(expr->as<BinaryOp>());
      return;
    case ExprType::TernaryOp:
      ptr(handler)->handle(expr->as<TernaryOp>());
      return;
    case ExprType::ReductionOp:
      ptr(handler)->handle(expr->as<ReductionOp>());
      return;
    case ExprType::WelfordOp:
      ptr(handler)->handle(expr->as<WelfordOp>());
      return;
    case ExprType::BroadcastOp:
      ptr(handler)->handle(expr->as<BroadcastOp>());
      return;

    case ExprType::Split:
      ptr(handler)->handle(expr->as<Split>());
      return;
    case ExprType::Merge:
      ptr(handler)->handle(expr->as<Merge>());
      return;
    case ExprType::TransposeOp:
      ptr(handler)->handle(expr->as<TransposeOp>());
      return;
    case ExprType::ShiftOp:
      ptr(handler)->handle(expr->as<ShiftOp>());
      return;
    case ExprType::GatherOp:
      ptr(handler)->handle(expr->as<GatherOp>());
      return;
    case ExprType::ViewOp:
      ptr(handler)->handle(expr->as<ViewOp>());
      return;

    case ExprType::Allocate:
      ptr(handler)->handle(expr->as<kir::Allocate>());
      return;
    case ExprType::Sync:
      ptr(handler)->handle(expr->as<kir::Sync>());
      return;
    case ExprType::InitMagicZero:
      ptr(handler)->handle(expr->as<kir::InitMagicZero>());
      return;
    case ExprType::UpdateMagicZero:
      ptr(handler)->handle(expr->as<kir::UpdateMagicZero>());
      return;
    case ExprType::ForLoop:
      ptr(handler)->handle(expr->as<kir::ForLoop>());
      return;
    case ExprType::IfThenElse:
      ptr(handler)->handle(expr->as<kir::IfThenElse>());
      return;
    case ExprType::GridReduction:
      ptr(handler)->handle(expr->as<kir::GridReduction>());
      return;
    case ExprType::GridBroadcast:
      ptr(handler)->handle(expr->as<kir::GridBroadcast>());
      return;
    case ExprType::GridWelford:
      ptr(handler)->handle(expr->as<kir::GridWelford>());
      return;
    default:
      TORCH_INTERNAL_ASSERT(false, "Unknown exprtype in dispatch!");
  }
}

template <typename T>
void Statement::constDispatch(T handler, const Statement* stmt) {
  if (stmt->isVal()) {
    ptr(handler)->handle(stmt->as<Val>());
  } else if (stmt->isExpr()) {
    ptr(handler)->handle(stmt->as<Expr>());
  } else
    TORCH_INTERNAL_ASSERT(false, "Unknown stmttype in dispatch!");
}

/*
 * Generic mutatorDispatch for any handler that modifies the IR. This could be
 * a transformation on loop structures, or parallelizing a loop. This
 * mutatorDispatch is paired with a class that implements the functions
 * template <typenname node_type> Statement* mutate(node_type* node) mutate
 * should call (statement* node_to_dispatch)->mutatorDispatch() It could also
 * implement Statement* mutate(Statement* stmt){ stmt->mutatorDispatch(this);
 * }
 * And therefore dispatch should never call:
 *   ptr(mutator)->mutate(this->as<Statement>());
 */
template <typename T>
Statement* Val::mutatorDispatch(T mutator, Val* val) {
  switch (*(val->getValType())) {
    case ValType::Scalar:
      switch (*(val->getDataType())) {
        case DataType::Bool:
          return ptr(mutator)->mutate(val->as<Bool>());
        case DataType::Double:
          return ptr(mutator)->mutate(val->as<Double>());
        case DataType::Int:
          return ptr(mutator)->mutate(val->as<Int>());
        default:
          break;
      }
      break;
    case ValType::NamedScalar:
      return ptr(mutator)->mutate(val->as<NamedScalar>());

    case ValType::IterDomain:
      return ptr(mutator)->mutate(val->as<IterDomain>());
    case ValType::TensorDomain:
      return ptr(mutator)->mutate(val->as<TensorDomain>());
    case ValType::TensorView:
      return ptr(mutator)->mutate(val->as<TensorView>());
    case ValType::Predicate:
      return ptr(mutator)->mutate(val->as<kir::Predicate>());
    case ValType::TensorIndex:
      return ptr(mutator)->mutate(val->as<kir::TensorIndex>());
    default:
      break;
  }
  TORCH_INTERNAL_ASSERT(false, "Unknown valtype in dispatch!");
}

template <typename T>
Statement* Expr::mutatorDispatch(T mutator, Expr* expr) {
  switch (*(expr->getExprType())) {
    case ExprType::UnaryOp:
      return ptr(mutator)->mutate(expr->as<UnaryOp>());
    case ExprType::BinaryOp:
      return ptr(mutator)->mutate(expr->as<BinaryOp>());
    case ExprType::TernaryOp:
      return ptr(mutator)->mutate(expr->as<TernaryOp>());
    case ExprType::ReductionOp:
      return ptr(mutator)->mutate(expr->as<ReductionOp>());
    case ExprType::WelfordOp:
      return ptr(mutator)->mutate(expr->as<WelfordOp>());
    case ExprType::BroadcastOp:
      return ptr(mutator)->mutate(expr->as<BroadcastOp>());

    case ExprType::Split:
      return ptr(mutator)->mutate(expr->as<Split>());
    case ExprType::Merge:
      return ptr(mutator)->mutate(expr->as<Merge>());
    case ExprType::TransposeOp:
      return ptr(mutator)->mutate(expr->as<TransposeOp>());
    case ExprType::ShiftOp:
      return ptr(mutator)->mutate(expr->as<ShiftOp>());
    case ExprType::GatherOp:
      return ptr(mutator)->mutate(expr->as<GatherOp>());
    case ExprType::ViewOp:
      return ptr(mutator)->mutate(expr->as<ViewOp>());

    case ExprType::Allocate:
      return ptr(mutator)->mutate(expr->as<kir::Allocate>());
    case ExprType::Sync:
      return ptr(mutator)->mutate(expr->as<kir::Sync>());
    case ExprType::InitMagicZero:
      return ptr(mutator)->mutate(expr->as<kir::InitMagicZero>());
    case ExprType::UpdateMagicZero:
      return ptr(mutator)->mutate(expr->as<kir::UpdateMagicZero>());
    case ExprType::ForLoop:
      return ptr(mutator)->mutate(expr->as<kir::ForLoop>());
    case ExprType::IfThenElse:
      return ptr(mutator)->mutate(expr->as<kir::IfThenElse>());
    case ExprType::GridReduction:
      return ptr(mutator)->mutate(expr->as<kir::GridReduction>());
    case ExprType::GridBroadcast:
      return ptr(mutator)->mutate(expr->as<kir::GridBroadcast>());
    case ExprType::GridWelford:
      return ptr(mutator)->mutate(expr->as<kir::GridWelford>());
    default:
      TORCH_INTERNAL_ASSERT(false, "Unknown exprtype in dispatch!");
  }
}

template <typename T>
Statement* Statement::mutatorDispatch(T mutator, Statement* stmt) {
  if (stmt->isVal()) {
    return ptr(mutator)->mutate(stmt->as<Val>());
  }
  if (stmt->isExpr()) {
    return ptr(mutator)->mutate(stmt->as<Expr>());
  }
  TORCH_INTERNAL_ASSERT(false, "Unknown stmttype in dispatch!");
}

/*
 * Handler template instantiations. These should only have to be done on base
 * classes. Actual visitors/mutators should inhereit from these classes and call
 * ->dispatch(this) to avoid needing an explicit instantiation.
 */
template void Statement::dispatch(OptOutDispatch&, Statement*);
template void Statement::dispatch(OptOutDispatch*, Statement*);
template void Val::dispatch(OptOutDispatch&, Val*);
template void Val::dispatch(OptOutDispatch*, Val*);
template void Expr::dispatch(OptOutDispatch&, Expr*);
template void Expr::dispatch(OptOutDispatch*, Expr*);

template void Statement::dispatch(OptInDispatch, Statement*);
template void Statement::dispatch(OptInDispatch*, Statement*);
template void Val::dispatch(OptInDispatch, Val*);
template void Val::dispatch(OptInDispatch*, Val*);
template void Expr::dispatch(OptInDispatch, Expr*);
template void Expr::dispatch(OptInDispatch*, Expr*);

template void Statement::constDispatch(OptOutConstDispatch&, const Statement*);
template void Statement::constDispatch(OptOutConstDispatch*, const Statement*);
template void Val::constDispatch(OptOutConstDispatch&, const Val*);
template void Val::constDispatch(OptOutConstDispatch*, const Val*);
template void Expr::constDispatch(OptOutConstDispatch&, const Expr*);
template void Expr::constDispatch(OptOutConstDispatch*, const Expr*);

template void Statement::constDispatch(OptInConstDispatch&, const Statement*);
template void Statement::constDispatch(OptInConstDispatch*, const Statement*);
template void Val::constDispatch(OptInConstDispatch&, const Val*);
template void Val::constDispatch(OptInConstDispatch*, const Val*);
template void Expr::constDispatch(OptInConstDispatch&, const Expr*);
template void Expr::constDispatch(OptInConstDispatch*, const Expr*);

template Statement* Statement::mutatorDispatch(OptOutMutator&, Statement*);
template Statement* Statement::mutatorDispatch(OptOutMutator*, Statement*);
template Statement* Val::mutatorDispatch(OptOutMutator&, Val*);
template Statement* Val::mutatorDispatch(OptOutMutator*, Val*);
template Statement* Expr::mutatorDispatch(OptOutMutator&, Expr*);
template Statement* Expr::mutatorDispatch(OptOutMutator*, Expr*);

void OptOutDispatch::handle(Statement* s) {
  Statement::dispatch(this, s);
}

void OptOutDispatch::handle(Expr* e) {
  Expr::dispatch(this, e);
}

void OptOutDispatch::handle(Val* v) {
  Val::dispatch(this, v);
}

void OptOutConstDispatch::handle(const Statement* s) {
  Statement::constDispatch(this, s);
}

void OptOutConstDispatch::handle(const Expr* e) {
  Expr::constDispatch(this, e);
}

void OptOutConstDispatch::handle(const Val* v) {
  Val::constDispatch(this, v);
}

Statement* OptOutMutator::mutate(Statement* s) {
  return Statement::mutatorDispatch(this, s);
}

Statement* OptOutMutator::mutate(Expr* e) {
  return Expr::mutatorDispatch(this, e);
}

Statement* OptOutMutator::mutate(Val* v) {
  // If value is already mutated, return the mutation
  if (mutations.find(v) != mutations.end())
    return mutations[v];
  return Val::mutatorDispatch(this, v);
}

Statement* OptOutMutator::mutateAsVal(Val* v) {
  return mutate(v);
}

void OptOutMutator::registerMutation(Val* val, Val* mutation) {
  TORCH_INTERNAL_ASSERT(
      mutations.find(val) == mutations.end(),
      " The same value is incorrectly being mutated twice.",
      " One mutation per mutation pass is allowed.");
  mutations[val] = mutation;
}

void OptInConstDispatch::unhandled(const Statement* stmt) {
  if (stmt->isExpr()) {
    TORCH_INTERNAL_ASSERT(
        false, "Handle not overriden for ", stmt->getExprType().value(), ".");
  } else if (stmt->isVal()) {
    TORCH_INTERNAL_ASSERT(
        false, "Handle not overriden for ", stmt->getValType().value(), ".");
  } else {
    TORCH_INTERNAL_ASSERT("Unrecognized statement type.");
  }
}

void OptInDispatch::unhandled(Statement* stmt) {
  if (stmt->isExpr()) {
    TORCH_INTERNAL_ASSERT(
        false, "Handle not overriden for ", stmt->getExprType().value(), ".");
  } else if (stmt->isVal()) {
    TORCH_INTERNAL_ASSERT(
        false, "Handle not overriden for ", stmt->getValType().value(), ".");
  } else {
    TORCH_INTERNAL_ASSERT("Unrecognized statement type.");
  }
}

// Vals
void OptOutConstDispatch::handle(const Bool* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const Double* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const Int* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const NamedScalar* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const IterDomain* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const TensorDomain* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const TensorView* stmt) {
  unhandled(stmt);
}

void OptOutConstDispatch::handle(const kir::Predicate* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::TensorIndex* stmt) {
  unhandled(stmt);
}

// Exprs
void OptOutConstDispatch::handle(const UnaryOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const BinaryOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const TernaryOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const ReductionOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const WelfordOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const BroadcastOp* stmt) {
  unhandled(stmt);
}

void OptOutConstDispatch::handle(const Split* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const Merge* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const TransposeOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const ShiftOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const GatherOp* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const ViewOp* stmt) {
  unhandled(stmt);
}

void OptOutConstDispatch::handle(const kir::Allocate* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::Sync* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::InitMagicZero* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::UpdateMagicZero* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::ForLoop* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::IfThenElse* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::GridReduction* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::GridBroadcast* stmt) {
  unhandled(stmt);
}
void OptOutConstDispatch::handle(const kir::GridWelford* stmt) {
  unhandled(stmt);
}

void OptOutDispatch::unhandled(Statement*) {}

// Vals
void OptOutDispatch::handle(Bool* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(Double* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(Int* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(NamedScalar* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(IterDomain* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(TensorDomain* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(TensorView* stmt) {
  unhandled(stmt);
}

void OptOutDispatch::handle(kir::Predicate* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::TensorIndex* stmt) {
  unhandled(stmt);
}

// Exprs
void OptOutDispatch::handle(UnaryOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(BinaryOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(TernaryOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(ReductionOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(WelfordOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(BroadcastOp* stmt) {
  unhandled(stmt);
}

void OptOutDispatch::handle(Split* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(Merge* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(TransposeOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(ShiftOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(GatherOp* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(ViewOp* stmt) {
  unhandled(stmt);
}

void OptOutDispatch::handle(kir::Allocate* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::Sync* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::InitMagicZero* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::UpdateMagicZero* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::ForLoop* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::IfThenElse* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::GridReduction* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::GridBroadcast* stmt) {
  unhandled(stmt);
}
void OptOutDispatch::handle(kir::GridWelford* stmt) {
  unhandled(stmt);
}

} // namespace cuda
} // namespace fuser
} // namespace jit
} // namespace torch
