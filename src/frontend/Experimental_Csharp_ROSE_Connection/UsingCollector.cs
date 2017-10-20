using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

namespace UsingCollectorCS
   {
     class UsingCollector : CSharpSyntaxWalker
        {
          public readonly List<UsingDirectiveSyntax> Usings = new List<UsingDirectiveSyntax>();

          public override void VisitUsingDirective(UsingDirectiveSyntax node)
             {
            // if (node.Name.ToString() != "System" && !node.Name.ToString().StartsWith("System."))
            //    {
            //      this.Usings.Add(node);
            //    }
               this.Usings.Add(node);
             }
        }
   }
