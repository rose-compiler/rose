using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

// namespace AbstractSyntaxTreeTraversalNamespace
//  {

public class AbstractSyntaxTreeTraversal : CSharpSyntaxWalker
   {
     static int Tabs = 0;
  // NOTE: Make sure you invoke the base constructor with 
  // the correct SyntaxWalkerDepth. Otherwise VisitToken()
  // will never get run.
     public AbstractSyntaxTreeTraversal() : base(SyntaxWalkerDepth.Token)
        {
        }

     public override void Visit(SyntaxNode node)
        {
          Tabs++;

       // Tabs are too much.
       // var indents = new String('\t', Tabs);
          var indents = new String(' ',Tabs*5);

          Console.WriteLine(indents + node.Kind());
          base.Visit(node);
          Tabs--;
        }

     public override void VisitToken(SyntaxToken token)
        {
       // var indents = new String('\t', Tabs);
       // Console.WriteLine(indents + token);
       // base.VisitToken(token);
        }
   }

// }

