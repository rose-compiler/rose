using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Runtime.CompilerServices;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

public class UniqueId
{
  ConditionalWeakTable<ISymbol, Object> memory;
  int                                   curr;

  UniqueId(ConditionalWeakTable<ISymbol, Object> mem, int initval)
  : base()
  {
    memory = mem;
    curr = 1;
  }

  public int Get(ISymbol n)
  {
    return (int) memory.GetValue(n, unused => ++curr );
  }

  public static UniqueId Create()
  {
    return new UniqueId(new ConditionalWeakTable<ISymbol, Object>(), 1);
  }
}

public static class SuperExtension
{
  public static void Accept(this SyntaxNode n, CSharpSyntaxVisitor vis)
  {
    (n as CSharpSyntaxNode).Accept(vis);
  }
}


public class DeclarationFinder : SymbolVisitor
{
  SemanticModel semanticModel;
  UniqueId      idgen;

  public
  DeclarationFinder(SemanticModel sm, UniqueId id)
  {
    semanticModel = sm;
    idgen         = id;
  }

  public static
  void Assert(bool Cond)
  {
    if (!Cond) throw new Exception("assertion failure");
  }

  public override void DefaultVisit(ISymbol n)
  {
    Console.WriteLine("** unknown symbol type: " + n.GetType());

    // base.DefaultVisit(n);
    Assert(false);
  }

  public override void VisitLocal(ILocalSymbol n)
  {
    Console.WriteLine("local: " + idgen.Get(n));

    csharpBuilder.refVarParamDecl(idgen.Get(n));
  }

  public override void VisitParameter(IParameterSymbol n)
  {
    Console.WriteLine("param: " + idgen.Get(n));

    csharpBuilder.refVarParamDecl(idgen.Get(n));
  }

  public override void VisitMethod(IMethodSymbol n)
  {
    Console.WriteLine("func: " + idgen.Get(n));

    csharpBuilder.refFunDecl(idgen.Get(n));
  }

  public override void VisitField(IFieldSymbol n)
  {
    csharpBuilder.refVarParamDecl(idgen.Get(n));
  }
}


public class BaseBuilder : CSharpSyntaxVisitor
{
  int           Level;
  SemanticModel semanticModel;
  UniqueId      idgen;
  TypeSyntax    tyctx;

  public
  BaseBuilder(SemanticModel sm)
  : base()
  {
    Level = 0;
    semanticModel = sm;
    idgen = UniqueId.Create();
    tyctx = null;
  }

  public
  BaseBuilder(BaseBuilder sub)
  : base()
  {
    Level = sub.Level + 1;
    idgen = sub.idgen;
    semanticModel = sub.semanticModel;
    tyctx = sub.tyctx;
  }

  public BaseBuilder(BaseBuilder sub, TypeSyntax ty)
  : base()
  {
    Level = sub.Level + 1;
    idgen = sub.idgen;
    semanticModel = sub.semanticModel;
    tyctx = ty;
  }

  public static
  Func<int, bool> AssertNumChildren(int min, int max)
  {
    return y => (min <= y) && (y <= max);
  }

  public static
  Func<int, bool> AssertNumChildren(int x)
  {
    return y => x == y;
  }


  public static
  void Assert(bool Cond)
  {
    if (!Cond) throw new Exception("assertion failure");
  }

  public
  void WriteLine(String msg)
  {
    var indents = new String('\t', Level);

    Console.WriteLine(indents + msg);
  }

  public
  void PrintTokens(SyntaxNode n)
  {
    var cnt = 0;

    foreach (var tok in n.DescendantTokens())
      WriteLine(n.Kind().ToString() + (++cnt) + ": " + tok);
  }


  static public
  String GetStringToken(SyntaxNode n, int pos, Func<int, bool> Check = null)
  {
    var Tokens = n.DescendantTokens();
    var Cnt = Tokens.Count();

    Assert(Cnt > pos && (Check == null || Check(Cnt)));
    return Tokens.ElementAt(pos).ToString();
  }

  static public
  Builder TraverseChildren<Builder>(SyntaxNode n, Builder builder, Func<int, bool> Check = null)
    where Builder : BaseBuilder
  {
    int                     Cnt = 0;

    foreach (var child in n.ChildNodes())
    {
      child.Accept(builder);
      ++Cnt;
    }

    Assert(Check == null || Check(Cnt));
    return builder;
  }

  static public
  Builder TraverseSequence<Builder, T>(IEnumerable<T> Seq, Builder builder, Func<int, bool> Check = null)
    where Builder : BaseBuilder
    where T : SyntaxNode
  {
    foreach (var elem in Seq)
      elem.Accept(builder);

    return builder;
  }

  static public
  Builder TraverseGroup<Builder>(SyntaxNode n, SeqKind kind, Builder builder, Func<int, bool> Check = null)
    where Builder : BaseBuilder
  {
    csharpBuilder.beginSeq(kind);
    Builder res = TraverseChildren(n, builder, Check);
    csharpBuilder.closeSeq();

    return res;
  }


  static public
  Builder TraverseNode<Builder>(CSharpSyntaxNode n, Builder builder)
    where Builder : BaseBuilder
  {
    n.Accept(builder);
    return builder;
  }

  public override
  void DefaultVisit(SyntaxNode n)
  {
    WriteLine((int)n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    //~ Console.WriteLine(System.Environment.StackTrace);

    Assert(false);
    TraverseChildren(n, this);
  }

  public
  SemanticModel model()
  {
    Assert(semanticModel != null);
    return semanticModel;
  }

  public
  TypeSyntax type_context()
  {
    Assert(tyctx != null);
    return tyctx;
  }

  public
  UniqueId idGenerator()
  {
    return idgen;
  }

  public
  int GetUid(ISymbol n)
  {
    return idgen.Get(n);
  }

  public
  int GetUid(SyntaxNode n)
  {
    ISymbol    sym = model().GetDeclaredSymbol(n);

    if (sym == null)
    {
      WriteLine("** sym(" + n + ") == null **");
      return -1;
    }

    return GetUid(sym);
  }
}

public class NameBuilder : BaseBuilder
{
  int cntNameComponents;

  public NameBuilder()
  : base(null as SemanticModel)
  {
    cntNameComponents = 0;
  }

  public NameBuilder(BaseBuilder sub)
  : base(sub)
  {
    cntNameComponents = 0;
  }

  public override void DefaultVisit(SyntaxNode n)
  {
    Console.WriteLine("** unknown symbol type: " + n.GetType());

    Assert(false);
  }

  public override void VisitIdentifierName(IdentifierNameSyntax n)
  {
    ++cntNameComponents;
    csharpBuilder.name(GetStringToken(n, 0, AssertNumChildren(1)));

    TraverseChildren(n, this, AssertNumChildren(0));
  }

  public override void VisitQualifiedName(QualifiedNameSyntax n)
  {
    TraverseChildren(n, this);
  }

  public int numNameComponents()
  {
    Assert(cntNameComponents > 0);
    return cntNameComponents;
  }
}

public class TypeBuilder : SymbolVisitor
{
  public TypeBuilder(BaseBuilder sub)
  {}

  public override void DefaultVisit(ISymbol n)
  {
    Console.WriteLine("** unknown type symbol: " + n.GetType() + "/" + n);

    throw new Exception("assertion failure");
  }

  public override void VisitNamedType(INamedTypeSymbol n)
  {
    csharpBuilder.predefinedType(""+n);
  }


  //~ public override void VisitPredefinedType(PredefinedTypeSyntax n)
  //~ {
    //~ csharpBuilder.predefinedType();
  //~ }
}

public class ExprBuilder : BaseBuilder
{
  public
  ExprBuilder(BaseBuilder sub, TypeSyntax ty)
  : base(sub, ty)
  {}

  public
  ExprBuilder(BaseBuilder sub)
  : base(sub)
  {}

  void BuildType(SyntaxNode n)
  {
    model().GetTypeInfo(n).Type.Accept(new TypeBuilder(this));
  }

  public override void DefaultVisit(SyntaxNode n)
  {
    Console.WriteLine("** unknown expression type: " + n.GetType());

    Assert(false);
  }

  public override
  void VisitEqualsValueClause(EqualsValueClauseSyntax n)
  {
    BuildType(type_context());
    TraverseChildren(n, new ExprBuilder(this), AssertNumChildren(1));

    csharpBuilder.valueInitializer();
  }

  public override
  void VisitIdentifierName(IdentifierNameSyntax n)
  {
    SymbolInfo sym     = model().GetSymbolInfo(n);
    ISymbol    declsym = sym.Symbol;

    //~ WriteLine("" + sym + "/ " + (declsym != null) + " for " + n);

    if (declsym == null)
    {
      ISymbol last      = null;
      var     candidates = sym.CandidateSymbols.GetEnumerator();
      //~ var reasons = sym.CandidateReason.GetEnumerator();

      while (candidates.MoveNext())
      {
        last = candidates.Current;

        if (declsym == null) declsym = last;
      }

      if (last != declsym)
      {
        WriteLine("** unable to find declaration");
        WriteLine("   first candidate is " + declsym + " for " + n);
        WriteLine("   reason: " + sym.CandidateReason);
        Assert(false);
      }

      Console.WriteLine("WARNING: using unique ambiguous declaration: " + declsym + " for " + n);
    }

    Assert(declsym != null);
    declsym.Accept(new DeclarationFinder(model(), idGenerator()));
  }

  public override
  void VisitLiteralExpression(LiteralExpressionSyntax n)
  {
    TraverseChildren(n, new ExprBuilder(this), AssertNumChildren(0));

    BuildType(n);
    csharpBuilder.literal(GetStringToken(n, 0, AssertNumChildren(1)));
  }

  public override
  void VisitBinaryExpression(BinaryExpressionSyntax n)
  {
    BuildType(type_context());
    TraverseChildren(n, new ExprBuilder(this), AssertNumChildren(2));

    Console.WriteLine("BINARY: " + n.OperatorToken);
    csharpBuilder.binary("" + n.OperatorToken);
  }

  public override
  void VisitInvocationExpression(InvocationExpressionSyntax n)
  {
    n.Expression.Accept(new ExprBuilder(this));

    BuildType(type_context()); // for expression list
    TraverseGroup(n.ArgumentList, SeqKind.EXPRLISTSEQ, new ExprBuilder(this));

    csharpBuilder.binary("()");
  }

  public override
  void VisitArgument(ArgumentSyntax n)
  {
    n.Expression.Accept(new ExprBuilder(this));
  }
}

public class DeferredBuilder : BaseBuilder
{
  AstBuilder astbuilder;

  public
  DeferredBuilder(AstBuilder sub)
  : base(sub)
  {
    astbuilder = sub;
  }

  public override
  void DefaultVisit(SyntaxNode n)
  {
    WriteLine("** unknown symbol type: " + n.GetType());

    // base.DefaultVisit(n);
    Assert(false);
  }

  public override
  void VisitVariableDeclarator(VariableDeclaratorSyntax n)
  {
    int uid = GetUid(n);

    WriteLine(">> VisitVariableDeclarator");
    // defer to later
    TraverseNode(n.Initializer, new ExprBuilder(this));

    csharpBuilder.initVarParamDecl(uid);
    WriteLine("<< VisitVariableDeclarator");
  }

  public override
  void VisitParameter(ParameterSyntax n)
  {
    int uid = GetUid(n);

    WriteLine(">> VisitParam");
    TraverseNode(n.Default, new ExprBuilder(this));
    csharpBuilder.initVarParamDecl(uid);
    WriteLine("<< VisitParam");
  }

  public override
  void VisitMethodDeclaration(MethodDeclarationSyntax n)
  {
    int uid = GetUid(n);

    WriteLine(">> VisitMethod");
    astbuilder.TraverseBody(n.Body, n.ReturnType, uid);
    WriteLine("<< VisitMethod");
  }
}


public class AstBuilder : BaseBuilder
{
  List<SyntaxNode> deferred;

  public AstBuilder(SemanticModel sm)
  : base(sm)
  {
    deferred = new List<SyntaxNode>();
  }

  public AstBuilder(BaseBuilder sub)
  : base(sub)
  {
    deferred = new List<SyntaxNode>();
  }

  public AstBuilder(BaseBuilder sub, TypeSyntax ty)
  : base(sub, ty)
  {
    deferred = new List<SyntaxNode>();
  }

  public
  List<SyntaxNode> Deferred() { return deferred; }

  void BuildType(TypeSyntax n)
  {
    model().GetTypeInfo(n).Type.Accept(new TypeBuilder(this));
  }

  void BuildType(SyntaxNode n)
  {
    model().GetTypeInfo(n).Type.Accept(new TypeBuilder(this));
  }

  void TraverseChildren(SyntaxNode n, Func<int, bool> Check = null)
  {
    TraverseChildren(n, new AstBuilder(this), Check);
  }

  void TraverseGroup(SyntaxNode n, SeqKind kind, Func<int, bool> Check = null)
  {
    AstBuilder sub = TraverseGroup(n, kind, new AstBuilder(this), Check);

    deferred.AddRange(sub.Deferred());
  }

  void TraverseParamlist(ParameterListSyntax n, int uidDecl)
  {
    csharpBuilder.stageMethodDecl(uidDecl);

    AstBuilder sub = TraverseGroup(n, SeqKind.PARAMETERSEQ, new AstBuilder(this));

    deferred.AddRange(sub.Deferred());
  }

  public void TraverseBody(BlockSyntax n, TypeSyntax ty, int uidDecl)
  {
    csharpBuilder.stageMethodDecl(uidDecl);

    AstBuilder sub = TraverseGroup(n, SeqKind.METHODBODYSEQ, new AstBuilder(this, ty));

    deferred.AddRange(sub.Deferred());
  }

  void HandleExpr(SyntaxNode n)
  {
    n.Accept(new ExprBuilder(this));
  }

  void HandleDeferred()
  {
    while (deferred.Count != 0)
    {
      DeferredBuilder  db = new DeferredBuilder(this);
      List<SyntaxNode> curr = deferred;

      deferred = new List<SyntaxNode>();

      foreach (var n in curr)
        n.Accept(db);
    }
  }

  // expression nodes get deferred to ExprHandler
  public override void VisitIdentifierName(IdentifierNameSyntax n) { HandleExpr(n); }
  public override void VisitBinaryExpression(BinaryExpressionSyntax n) { HandleExpr(n); }

  // declarations

  public override
  void VisitUsingDirective(UsingDirectiveSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    NameBuilder sub = TraverseChildren(n, new NameBuilder(this));
    csharpBuilder.usingDirective(GetUid(n), sub.numNameComponents());
  }

  public override
  void VisitClassDeclaration(ClassDeclarationSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + " @" + n.Identifier + "  #" + GetUid(n));

    csharpBuilder.name(n.Identifier.ToString());
    csharpBuilder.classDecl(GetUid(n));

    TraverseGroup(n, SeqKind.CLASSMEMBERSEQ);

    HandleDeferred();
  }

  public override
  void VisitVariableDeclarator(VariableDeclaratorSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    int uid = GetUid(n);

    TypeSyntax ty = (n.Parent as VariableDeclarationSyntax).Type;

    csharpBuilder.name(n.Identifier.ToString());
    //~ BuildType(type_context());
    BuildType(ty);
    csharpBuilder.varDecl(uid);

    // defer to later
    if (n.Initializer != null)
      deferred.Add(n);
  }

  public override void VisitParameter(ParameterSyntax n)
  {
    int uid = GetUid(n);

    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    csharpBuilder.name(n.Identifier.ToString());
    BuildType(n.Type);
    csharpBuilder.paramDecl(uid);

    // defer to later
    if (n.Default != null)
      deferred.Add(n);
  }

  public override
  void VisitVariableDeclaration(VariableDeclarationSyntax n)
  {
    // Like in C, a variable declaration can declare multiple variables.
    //   i.e., VariableDeclaratorSyntax nodes
    // Similar to how ROSE handles similar C++ declarations,
    //   we create separate declarations in the ROSE AST for each variable.
    TraverseSequence(n.Variables, new AstBuilder(this));
  }

  public override
  void VisitReturnStatement(ReturnStatementSyntax n)
  {
    ExpressionSyntax expr = n.Expression;
    int              args = 0;

    if (expr != null)
    {
      expr.Accept(new ExprBuilder(this));
      args = 1;
    }

    csharpBuilder.returnStmt(args);
  }

  public override
  void VisitMethodDeclaration(MethodDeclarationSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + " @" + n.Identifier + "  #" + GetUid(n));

    int uid = GetUid(n);

    csharpBuilder.name(n.Identifier.ToString());
    BuildType(n.ReturnType);
    csharpBuilder.methodDecl(uid);

    // \todo defer building parameters and methods
    TraverseParamlist(n.ParameterList, uid);

    if (n.Body != null)
      deferred.Add(n);
    // TraverseBody(n.Body, n.ReturnType, uid);
  }

  public override
  void VisitLocalDeclarationStatement(LocalDeclarationStatementSyntax n)
  {
    // \todo do we need to handle this is a sequence of InitializedName objects?
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    TraverseChildren(n);
  }

  public override
  void VisitFieldDeclaration(FieldDeclarationSyntax n)
  {
    TraverseChildren(n, new AstBuilder(this));
  }

  public override
  void VisitIfStatement(IfStatementSyntax n)
  {
    TraverseGroup(n, SeqKind.IFSTMT, AssertNumChildren(2,3));

    //~ n.Statement.Accept(new AstBuilder(this));
    //~ if (n.Else != null) n.Else.Accept(new AstBuilder(this));
  }


  public override
  void VisitCompilationUnit(CompilationUnitSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    TraverseGroup(n, SeqKind.GLOBALSCOPESEQ);
  }
}
