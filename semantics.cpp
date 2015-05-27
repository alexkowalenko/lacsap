#include "semantics.h"
#include "expr.h"
#include "astvisitor.h"
#include "trace.h"
#include "token.h"
#include "options.h"

class TypeCheckVisitor : public Visitor
{
public:
    TypeCheckVisitor(Semantics* s) : sema(s) {};
    virtual void visit(ExprAST* expr);
private:
    void CheckBinExpr(BinaryExprAST* b);
    void CheckAssignExpr(AssignExprAST* a);
    void CheckRangeExpr(RangeExprAST* r);
    void CheckSetExpr(SetExprAST* s);
    void CheckArrayExpr(ArrayExprAST* a);
    void CheckBuiltinExpr(BuiltinExprAST* b);
    void CheckCallExpr(CallExprAST* c);
    void Error(const ExprAST* e, const std::string& msg) const;
private:
    Semantics* sema;
};

class SemaFixup
{
public:
    SemaFixup() {}
    virtual void DoIt() = 0;
    virtual ~SemaFixup() {}
};

class SetRangeFixup : public SemaFixup
{
public:
    SetRangeFixup(SetExprAST* s, Types::RangeDecl* r) : expr(s), guessRange(r) {}
    void DoIt() override;
private:
    SetExprAST*       expr;
    Types::RangeDecl* guessRange;
};

void SetRangeFixup::DoIt()
{
    if (!expr->Type()->GetRange())
    {
	Types::SetDecl* sd = llvm::dyn_cast<Types::SetDecl>(expr->Type());
	sd->UpdateRange(guessRange);
    }
}

static bool isNumeric(Types::TypeDecl* t)
{
    switch(t->Type())
    {
    case Types::Integer:
    case Types::Int64:
    case Types::Real:
	return true;
    default:
	return false;
    }
}

static Types::RangeDecl* GetRangeDecl(Types::TypeDecl* ty)
{
    Types::Range* r = ty->GetRange();
    Types::TypeDecl* base;
    if (!r)
    {
	if (!ty->SubType())
	{
	    return 0;
	}
	r = ty->SubType()->GetRange();
    }

    if (ty->isIntegral())
    {
	base = ty;
    }
    else
    {
	base = ty->SubType();
    }

    if (r->Size() > Types::SetDecl::MaxSetSize)
    {
	r = new Types::Range(0, Types::SetDecl::MaxSetSize-1);
    }

    return new Types::RangeDecl(r, base->Type());
}

void TypeCheckVisitor::Error(const ExprAST* e, const std::string& msg) const
{
    std::cerr << e->Loc() << ": " << msg << std::endl;
    sema->AddError();
}

void TypeCheckVisitor::visit(ExprAST* expr)
{
    TRACE();

    if (verbosity > 1)
    {
	expr->dump();
    }

    if (BinaryExprAST* b = llvm::dyn_cast<BinaryExprAST>(expr))
    {
	CheckBinExpr(b);
    }
    else if (AssignExprAST* a = llvm::dyn_cast<AssignExprAST>(expr))
    {
	CheckAssignExpr(a);
    }
    else if (RangeExprAST* r = llvm::dyn_cast<RangeExprAST>(expr))
    {
	CheckRangeExpr(r);
    }
    else if (SetExprAST* s = llvm::dyn_cast<SetExprAST>(expr))
    {
	CheckSetExpr(s);
    }
    else if (ArrayExprAST* a = llvm::dyn_cast<ArrayExprAST>(expr))
    {
	CheckArrayExpr(a);
    }
    else if (BuiltinExprAST* b = llvm::dyn_cast<BuiltinExprAST>(expr))
    {
	CheckBuiltinExpr(b);
    }
    else if (CallExprAST* c = llvm::dyn_cast<CallExprAST>(expr))
    {
	CheckCallExpr(c);
    }
}

void TypeCheckVisitor::CheckBinExpr(BinaryExprAST* b)
{
    TRACE();

    Types::TypeDecl* lty = b->lhs->Type();
    Types::TypeDecl* rty = b->rhs->Type();
    Types::TypeDecl* ty = 0;
    Token::TokenType op = b->oper.GetToken();

    if (op == Token::In)
    {
	if (!lty->isIntegral())
	{
	    Error(b, "Left hand of 'in' expression should be integral.");
	}

	if(Types::SetDecl* sd = llvm::dyn_cast<Types::SetDecl>(rty))
	{
	    assert(sd->SubType() && "Should have a subtype");
	    if (*lty != *sd->SubType())
	    {
		Error(b, "Left hand type does not match constituent parts of set");
	    }
	    if (!sd->GetRange())
	    {
		sd->UpdateRange(GetRangeDecl(lty));
	    }
	}
	else
	{
	    Error(b, "Right hand of 'in' expression should be a set.");
	}
	ty = new Types::BoolDecl;
    }
    if (!ty && lty->Type() == Types::Set && rty->Type() == Types::Set)
    {
	if (SetExprAST* s = llvm::dyn_cast<SetExprAST>(b->lhs))
	{
	    if (s->values.empty())
	    {
		if (rty->SubType())
		{
		    llvm::dyn_cast<Types::SetDecl>(lty)->UpdateSubtype(
			llvm::dyn_cast<Types::SetDecl>(rty)->SubType());
		}
	    }
	}
	if (SetExprAST* s = llvm::dyn_cast<SetExprAST>(b->rhs))
	{
	    if (s->values.empty())
	    {
		if (lty->SubType())
		{
		    llvm::dyn_cast<Types::SetDecl>(rty)->UpdateSubtype(
			llvm::dyn_cast<Types::SetDecl>(lty)->SubType());
		}
	    }
	}
	
	if (*lty->SubType() != *rty->SubType())
	{
	    Error(b, "Set type content isn't the same!");
	}
	if (!lty->GetRange())
	{
	    llvm::dyn_cast<Types::SetDecl>(lty)->UpdateRange(GetRangeDecl(rty));
	}
	if (!rty->GetRange())
	{
	    llvm::dyn_cast<Types::SetDecl>(rty)->UpdateRange(GetRangeDecl(lty));
	}
	ty = rty;
    }

    if (!ty && (op == Token::Plus))
    {
	if (lty->Type() == Types::Char && rty->Type() == Types::Char)
	{
	    ty = new Types::StringDecl(255);
	}
    }

    if (!ty && (op == Token::Divide))
    {
	if (!isNumeric(lty) || !isNumeric(rty))
	{
	    Error(b, "Invalid (non-numeric) type for divide");
	}
	if (lty->isIntegral())
	{
	    ExprAST* e = b->lhs;
	    ty = new Types::RealDecl;
	    b->lhs = new TypeCastAST(e->Loc(), e, ty);
	    lty = ty;
	}
	if (rty->isIntegral())
	{
	    ExprAST* e = b->rhs;
	    if (!ty)
	    {
		ty = new Types::RealDecl;
	    }
	    b->rhs = new TypeCastAST(e->Loc(), e, ty);
	    rty = ty;
	}
	if (!lty->CompatibleType(rty))
	{
	    Error(b, "Incompatible type for divide");
	}
    }

    if (!ty && 
	((llvm::isa<Types::PointerDecl>(lty) && llvm::isa<NilExprAST>(b->rhs)) ||
	 (llvm::isa<Types::PointerDecl>(rty) && llvm::isa<NilExprAST>(b->lhs))) &&
	(op == Token::Equal || op == Token::NotEqual))
    {
	if (llvm::isa<Types::PointerDecl>(lty))
	{
	    ty = lty;
	}
	else
	{
	    ty = rty;
	}
    }

    if (!ty && llvm::isa<Types::RangeDecl>(lty) && llvm::isa<IntegerExprAST>(b->rhs))
    {
	Types::Range* r = lty->GetRange();
	long v = llvm::dyn_cast<IntegerExprAST>(b->rhs)->Int();
	if (r->GetStart() > v || v > r->GetEnd())
	{
	    Error(b, "Value out of range");
	}
	ty = lty;
    }

    if (llvm::isa<Types::RangeDecl>(rty) && llvm::isa<IntegerExprAST>(b->lhs))
    {
	Types::Range* r = rty->GetRange();
	long v = llvm::dyn_cast<IntegerExprAST>(b->lhs)->Int();
	if (r->GetStart() > v || v > r->GetEnd())
	{
	    Error(b, "Value out of range");
	}
	ty = rty;
    }

    if (!ty)
    {
	if ((ty = const_cast<Types::TypeDecl*>(lty->CompatibleType(rty))))
	{
	    if (!ty->isCompound())
	    {
		if (lty != ty)
		{
		    ExprAST* e = b->lhs;
		    b->lhs = new TypeCastAST(e->Loc(), e, ty);
		}
		if (rty != ty)
		{
		    ExprAST* e = b->rhs;
		    b->rhs = new TypeCastAST(e->Loc(), e, ty);
		}
	    }
	}
	else
	{
	    Error(b, "Incompatible type in expression");
	}
    }
    b->UpdateType(ty);
}

void TypeCheckVisitor::CheckAssignExpr(AssignExprAST* a)
{
    TRACE();

    Types::TypeDecl* lty = a->lhs->Type();
    Types::TypeDecl* rty = a->rhs->Type();

    if (lty->Type() == Types::Set && rty->Type() == Types::Set)
    {
	assert(lty->GetRange() && lty->SubType() &&
	       "Expected left type to be well defined.");

	if (!rty->GetRange())
	{
	    llvm::dyn_cast<Types::SetDecl>(rty)->UpdateRange(GetRangeDecl(lty));
	}
	if (!rty->SubType())
	{
	    llvm::dyn_cast<Types::SetDecl>(rty)->UpdateSubtype(lty->SubType());
	}
	if (*lty->SubType() != *rty->SubType())
	{
	    Error(a, "Subtypes are different in assignment.");
	}
	else if (*lty->GetRange() != *rty->GetRange())
	{
	    Error(a, "Range mismatch for assignment");
	}
    }

    // Note difference to binary expression: only allowed on rhs!
    if (llvm::isa<Types::PointerDecl>(lty) && llvm::isa<NilExprAST>(a->rhs))
    {
	return;
    }

    if (llvm::isa<Types::RangeDecl>(lty) && llvm::isa<IntegerExprAST>(a->rhs))
    {
	Types::Range* r = lty->GetRange();
	long v = llvm::dyn_cast<IntegerExprAST>(a->rhs)->Int();
	if (r->GetStart() > v || v > r->GetEnd())
	{
	    Error(a, "Value out of range");
	}
	return;
    }

    if (llvm::isa<Types::ArrayDecl>(lty) && 
	!llvm::isa<Types::StringDecl>(lty) && 
	llvm::isa<StringExprAST>(a->rhs))
    {
	StringExprAST* s = llvm::dyn_cast<StringExprAST>(a->rhs);
	Types::ArrayDecl* aty = llvm::dyn_cast<Types::ArrayDecl>(lty);
	if (aty->SubType()->Type() == Types::Char && aty->Ranges().size() == 1)
	{
	    if (aty->Ranges()[0]->GetRange()->Size() == s->Str().size())
	    {
		return;
	    }
	}
	Error(a, "String assignment from incompatible string constant");
    }

    if (lty->AssignableType(rty) == NULL)
    {
	Error(a, "Incompatible type in assignment");
    }
}

void TypeCheckVisitor::CheckRangeExpr(RangeExprAST* r)
{
    TRACE();

    if (*r->low->Type() != *r->high->Type())
    {
	Error(r, "Range should be same type at both ends");
    }
}

void TypeCheckVisitor::CheckSetExpr(SetExprAST* s)
{
    TRACE();

    Types::Range* r;
    if (!(r = s->Type()->GetRange()))
    {
	Types::RangeDecl* rd = GetRangeDecl(s->Type());
	if (s->Type()->SubType())
	{
	    sema->AddFixup(new SetRangeFixup(s, rd));
	}
    }
}

void TypeCheckVisitor::CheckArrayExpr(ArrayExprAST* a)
{
    TRACE();

    for(size_t i = 0; i < a->indices.size(); i++)
    {
	ExprAST* e = a->indices[i];
	Types::RangeDecl* r = a->ranges[i];
	if(llvm::isa<RangeReduceAST>(e))
	    continue;
	if (r->Type() != e->Type()->Type())
	{
	    Error(a, "Incorrect index type");
	}
	if (rangeCheck)
	{
	    a->indices[i] = new RangeCheckAST(e, r);
	}
	else
	{
	    a->indices[i] = new RangeReduceAST(e, r);
	}
    }
}

void TypeCheckVisitor::CheckBuiltinExpr(BuiltinExprAST* b)
{
    TRACE();
    if (!b->bif->Semantics())
    {
	Error(b, "Invalid use of builtin function");
    }
}

void TypeCheckVisitor::CheckCallExpr(CallExprAST* c)
{
    TRACE();
    const PrototypeAST* proto = c->proto;
    if (c->args.size() != proto->args.size())
    {
	Error(c, "Incorrect number of arguments in call to " + c->proto->Name());
	return;
    }
    int idx = 0;
    const std::vector<VarDef>& parg = proto->args;
    for(auto& a : c->args)
    {
	if (const Types::TypeDecl* ty = parg[idx].Type()->CompatibleType(a->Type()))
	{
	    if (*ty != *a->Type())
	    {
		ExprAST* e = a;
		a = new TypeCastAST(e->Loc(), e, ty);
	    }
	    idx++;
	}
	else
	{
	    Error(a, "Incompatible type");
	}
    }
}

void Semantics::AddFixup(SemaFixup* f)
{
    TRACE();
    fixups.push_back(f);
}

void Semantics::RunFixups()
{
    TRACE();

    for(auto f : fixups)
    {
	f->DoIt();
    }
}

void Semantics::Analyse(std::vector<ExprAST*>& ast)
{
    TRACE();

    for(auto& e : ast)
    {
	TypeCheckVisitor tc(this);
	e->accept(tc);
    }
    RunFixups();
}
