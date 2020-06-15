#pragma once

#include "expr.h"

class CallGraphVisitor {
  public:
    virtual ~CallGraphVisitor() {}
    virtual void Caller(FunctionAST *) {}
    virtual void Process(FunctionAST *) {}
    virtual void VarDecl(VarDeclAST *) {}
};

class CallGraphPrinter : public CallGraphVisitor {
  public:
    virtual void Process(FunctionAST *f);
    virtual void Caller(FunctionAST *f);
};

void CallGraph(ExprAST *ast, CallGraphVisitor &visitor);
void BuildClosures(ExprAST *ast);
