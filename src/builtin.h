#pragma once

#include "namedobject.h"
#include "stack.h"
#include "visitor.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#pragma clang diagnostic pop

#include <string>
#include <vector>

class ExprAST;

namespace Builtin {
class BuiltinFunctionBase {
  public:
    BuiltinFunctionBase(const std::vector<ExprAST *> &a) : args(a) {}
    virtual llvm::Value *    CodeGen(llvm::IRBuilder<> &builder) = 0;
    virtual Types::TypeDecl *Type() const = 0;
    virtual bool             Semantics() = 0;
    virtual void             accept(ASTVisitor &v);
    virtual ~BuiltinFunctionBase() {}

  protected:
    std::vector<ExprAST *> args;
};

bool                 IsBuiltin(std::string funcname);
void                 InitBuiltins();
BuiltinFunctionBase *CreateBuiltinFunction(std::string name, std::vector<ExprAST *> &args);
} // namespace Builtin
