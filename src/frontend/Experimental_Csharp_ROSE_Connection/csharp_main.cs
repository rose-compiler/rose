// This is the Csharp program that will call the Roslyn library specific to CodeAnalysis.

// #define IGNORE_OLD_CODE
// #define OUTPUT_PARSE_TREE
// #define OUTPUT_ABSTRACT_SYNTAX_TREE
//~ #define OUTPUT_DOT_GRAPH_OF_ABSTRACT_SYNTAX_TREE

using System;
using System.Collections.Generic;
using RGiesecke.DllExport;
using System.Runtime.InteropServices;

// New code to make this more specific to supporting Roslyn library for code analysis.
using System.Linq;

// DQ (10/18/2017): Adding file I/O support.
using System.IO;

using System.Text;
using System.Threading.Tasks;

// using UsingCollectorCS;

// Failing cases of using declarations.
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;


namespace TestDLL
   {
     class Program
        {
          [DllExport("process", CallingConvention = CallingConvention.Cdecl)]
       // public static void process(UInt64 container)
       // public static void process(System.UInt64 container)
          public static void process(string filename)
             {
            // Test if this type is allowed.
            // StringBuilder sss;
            // StringBuilder sss = filename;

               csharpBuilder.helloFromCxx();

               String filetext = "";

            // Console.WriteLine ("In C# process(System.UInt64) called from ROSE C++: Hello!");

               Console.WriteLine (filename);

               using (FileStream fs = File.Open(filename, FileMode.Open))
                  {
                    byte[] b = new byte[1024];
                    UTF8Encoding temp = new UTF8Encoding(true);

                    while (fs.Read(b,0,b.Length) > 0)
                       {
                      // Console.WriteLine ("In C# process(string) reading file! \n");
                      // Console.WriteLine(temp.GetString(b));

                      // filetext += b;
                         filetext += temp.GetString(b);
                       }
                  }

               Console.WriteLine ("In C# process(string) after reading file");

            // Output the file as text on the console (debugging).
            // Console.WriteLine (filetext);

            // SyntaxTree tree = CSharpSyntaxTree.ParseText("using System;");
               SyntaxTree tree = CSharpSyntaxTree.ParseText(filetext);

               Console.WriteLine ("In C# process(System.UInt64) after parsing C# file");
               var compilation = CSharpCompilation.Create("MyCompilation", syntaxTrees: new[] { tree });

               SemanticModel model = compilation.GetSemanticModel(tree);

               Console.WriteLine ("In C# process(System.UInt64) after compiling C# file");

               var root    = tree.GetRoot() as CSharpSyntaxNode;
               var builder = new AstBuilder(model);

               root.Accept(builder);
               csharpBuilder.basicFinalChecks();

#if IGNORE_NEW_CODE
            var collector = new UsingCollector();
            collector.Visit(root);

            foreach (var directive in collector.Usings)
            {
                Console.WriteLine(directive.Name);
            }
#endif

#if VERBOSE_MODE
               Console.WriteLine ("In C# process(System.UInt64) after outputing the using directives!");
               Console.WriteLine ("In C# process(System.UInt64) generate DOT file of Roslyn AST!");
#endif // VERBOSE_MODE

#if OUTPUT_DOT_GRAPH_OF_ABSTRACT_SYNTAX_TREE
               var buildDotGraph_object = new BuildDotGraph(filename);
               buildDotGraph_object.Visit(root);

            // Call the destructor.
            // buildDotGraph_object.Finalize();
               buildDotGraph_object.CloseOffDotSyntax();
#endif

#if VERBOSE_MODE
               Console.WriteLine ("DONE: C# process(System.UInt64) generate DOT file of Roslyn AST!");
#endif // VERBOSE_MODE

#if OUTPUT_ABSTRACT_SYNTAX_TREE
               var abstractSyntaxTreeTraversal_object = new AbstractSyntaxTreeTraversal();
               abstractSyntaxTreeTraversal_object.Visit(root);
#endif

#if VERBOSE_MODE
               Console.WriteLine ("In C# process(System.UInt64) output Roslyn AST!");
#endif // VERBOSE_MODE

#if OUTPUT_PARSE_TREE
               var parseTreeTraversal_object = new ParseTreeTraversal();
               parseTreeTraversal_object.Visit(root);
#endif

#if VERBOSE_MODE
               Console.WriteLine ("In C# process(System.UInt64) after output of Roslyn AST!");

               Console.WriteLine ("Leaving C# process(System.UInt64)");
#endif // VERBOSE_MODE
             }
        }
   }
