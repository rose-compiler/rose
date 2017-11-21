using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

// namespace ParseTreeTraversalNamespace
//  {

public class ParseTreeTraversal : CSharpSyntaxWalker
   {
     static int Tabs = 0;
  // NOTE: Make sure you invoke the base constructor with 
  // the correct SyntaxWalkerDepth. Otherwise VisitToken()
  // will never get run.
     public ParseTreeTraversal() : base(SyntaxWalkerDepth.Token)
        {
        }
     public override void Visit(SyntaxNode node)
        {
          Tabs++;
          var indents = new String('\t', Tabs);
          Console.WriteLine(indents + node.Kind());
          base.Visit(node);
          Tabs--;
        }

     public override void VisitToken(SyntaxToken token)
        {
          var indents = new String('\t', Tabs);
          Console.WriteLine(indents + token);
          base.VisitToken(token);
        }
   }

// }

