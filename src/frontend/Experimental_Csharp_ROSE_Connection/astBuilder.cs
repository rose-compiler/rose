using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

class UniqueId
{
  ConditionalWeakTable<SyntaxNode, Object> memory;
  int                                      curr;

  UniqueId(ConditionalWeakTable<SyntaxNode, Object> mem, int initval)
  : base()
  {
    memory = mem;
    curr = 1;
  }

  public int Get(SyntaxNode n)
  {
    return (int) memory.GetValue(n, unused => ++curr );
  }

  public static UniqueId Create()
  {
    return new UniqueId(new ConditionalWeakTable<SyntaxNode, Object>(), 1);
  }
}

public static class SuperExtension
{
  public static void Accept(this SyntaxNode n, CSharpSyntaxVisitor vis)
  {
    (n as CSharpSyntaxNode).Accept(vis);
  }
}


public class BaseBuilder : CSharpSyntaxVisitor
{
  int      Level;
  UniqueId idgen;

  public
  BaseBuilder()
  : base()
  {
    Level = 0;
    idgen = UniqueId.Create();
  }

  public
  BaseBuilder(BaseBuilder sub)
  : base()
  {
    Level = sub.Level + 1;
    idgen = sub.idgen;
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
  Builder TraverseScopedChildren<Builder>(SyntaxNode n, SeqKind kind, Builder builder, Func<int, bool> Check = null)
    where Builder : BaseBuilder  
  {
    csharpBuilder.beginSeq(kind);
    Builder res = TraverseChildren(n, builder, Check);
    csharpBuilder.closeSeq();

    return res;
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

    TraverseChildren(n, this);
  }

  public
  int GetUid(SyntaxNode n)
  {
    return idgen.Get(n);
  }
}

public class NameBuilder : BaseBuilder
{
  int cntNameComponents;
  
  public NameBuilder()
  : base()
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

public class TypeBuilder : BaseBuilder
{
  public TypeBuilder(BaseBuilder sub)
  : base(sub)
  {}

  public override void VisitPredefinedType(PredefinedTypeSyntax n)
  {
    csharpBuilder.predefinedType(GetStringToken(n, 0, AssertNumChildren(1)));
  }
}

public class ParamlistBuilder : BaseBuilder
{
  public ParamlistBuilder(BaseBuilder sub)
  : base(sub)
  {}

  public override void VisitParameterList(ParameterListSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));
  }
}

public class ExprBuilder : BaseBuilder
{
  TypeSyntax type;
  
  public
  ExprBuilder(BaseBuilder sub, TypeSyntax ty)
  : base(sub)
  {
    type = ty;
  }

  public
  ExprBuilder(ExprBuilder sub)
  : base(sub)
  {
    type = sub.type;
  }

  void BuildType(CSharpSyntaxNode n)
  {
    n.Accept(new TypeBuilder(this));
  }

  public override
  void VisitEqualsValueClause(EqualsValueClauseSyntax n)
  {
    BuildType(type);
    TraverseChildren(n, new ExprBuilder(this), AssertNumChildren(1));
      
    csharpBuilder.valueInitializer();
  }

  public override
  void VisitLiteralExpression(LiteralExpressionSyntax n)
  {
    BuildType(type);
    TraverseChildren(n, new ExprBuilder(this), AssertNumChildren(0));

    csharpBuilder.literal(GetStringToken(n, 0, AssertNumChildren(1)));    
  }
}

public class VarBuilder : BaseBuilder
{
  TypeSyntax type;
  
  public
  VarBuilder(BaseBuilder sub, TypeSyntax ty)
  : base(sub)
  {
    type = ty;
  }

  void BuildType(CSharpSyntaxNode n)
  {
    n.Accept(new TypeBuilder(this));
  }

  public override
  void VisitVariableDeclarator(VariableDeclaratorSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    int uid = GetUid(n);

    csharpBuilder.name(n.Identifier.ToString());
    BuildType(type);
    csharpBuilder.varDecl(uid);

    // defer to later
    TraverseNode(n.Initializer, new ExprBuilder(this, type));
    csharpBuilder.initVarDecl(uid);
  }
}


public class AstBuilder : BaseBuilder
{  
  public AstBuilder()
  : base()
  {}

  public AstBuilder(BaseBuilder sub)
  : base(sub)
  {}

  void BuildType(CSharpSyntaxNode n)
  {
    n.Accept(new TypeBuilder(this));
  }

  void TraverseChildren(SyntaxNode n, Func<int, bool> Check = null)
  {
    TraverseChildren(n, new AstBuilder(this), Check);
  }

  void TraverseScopedChildren(SyntaxNode n, SeqKind kind, Func<int, bool> Check = null)
  {
    TraverseScopedChildren(n, kind, new AstBuilder(this), Check);
  }

  void TraverseParamlist(ParameterListSyntax n, int uidDecl)
  {
    csharpBuilder.stageMethodDecl(uidDecl);

    TraverseScopedChildren(n, SeqKind.PARAMETERSEQ, new ParamlistBuilder(this));
  }

  void TraverseBody(BlockSyntax n, int uidDecl)
  {
    csharpBuilder.stageMethodDecl(uidDecl);
    
    TraverseScopedChildren(n, SeqKind.METHODBODYSEQ);
  }

  public override void VisitUsingDirective(UsingDirectiveSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    NameBuilder sub = TraverseChildren(n, new NameBuilder(this));
    csharpBuilder.usingDirective(GetUid(n), sub.numNameComponents());
  }

  public override void VisitClassDeclaration(ClassDeclarationSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + " @" + n.Identifier + "  #" + GetUid(n));

    csharpBuilder.name(n.Identifier.ToString());
    csharpBuilder.classDecl(GetUid(n));

    TraverseScopedChildren(n, SeqKind.CLASSMEMBERSEQ);
  }

  public override void VisitVariableDeclaration(VariableDeclarationSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    //~ int uid = GetUid(n);
    TraverseSequence(n.Variables, new VarBuilder(this, n.Type));
  } 

  public override void VisitMethodDeclaration(MethodDeclarationSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + " @" + n.Identifier + "  #" + GetUid(n));

    int uid = GetUid(n);

    csharpBuilder.name(n.Identifier.ToString());
    BuildType(n.ReturnType);
    csharpBuilder.methodDecl(uid);

    // \todo defer building parameters and methods
    TraverseParamlist(n.ParameterList, uid);
    TraverseBody(n.Body, uid);
  }

  public override void VisitLocalDeclarationStatement(LocalDeclarationStatementSyntax n)
  {
    // \todo do we need to handle this is a sequence of InitializedName objects? 
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    TraverseChildren(n);
  }
  
  public override void VisitCompilationUnit(CompilationUnitSyntax n)
  {
    WriteLine(n.Kind() + " " + n.GetType() + "  #" + GetUid(n));

    TraverseScopedChildren(n, SeqKind.GLOBALSCOPESEQ);
  }
}


