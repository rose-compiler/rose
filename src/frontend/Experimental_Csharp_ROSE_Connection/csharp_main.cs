// This is the Csharp program that will call the Roslyn library specific to CodeAnalysis.

// #define IGNORE_OLD_CODE
#define OUTPUT_PARSE_TREE
#define OUTPUT_ABSTRACT_SYNTAX_TREE

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

using UsingCollectorCS;

// Failing cases of using declarations.
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;


namespace TestDLL
   {
     class Program
        {
#if IGNORE_OLD_CODE
          public static List<int> superCalculation()
             {
            // Actual name of the function should be: "dateNowToIntegerList"

               List<int> result = new List<int>();
               string dateString = DateTime.Now.ToString();
               for(int i = 0;i<dateString.Length;i++)
                  {
                    if (Char.IsNumber(dateString[i]) == false) 
                         continue;
                    result.Add(Int32.Parse( dateString[i].ToString() ));
                  }

               return result;
             }

       // [DllImport("./Kazelib.so", EntryPoint = "storeResults")]
       // public extern static void storeResults(System.UInt64 container, int value);
#endif
        
          [DllExport("process", CallingConvention = CallingConvention.Cdecl)]
       // public static void process(UInt64 container)
       // public static void process(System.UInt64 container)
          public static void process(string filename)
             {
            // Test if this type is allowed.
            // StringBuilder sss;
            // StringBuilder sss = filename;

               String filetext = "";

               Console.WriteLine ("In C# process(System.UInt64) called from ROSE C++: Hello! \n");

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

               Console.WriteLine ("In C# process(string) after reading file! \n");

            // Output the file as text on the console (debugging).
            // Console.WriteLine (filetext);

            // SyntaxTree tree = CSharpSyntaxTree.ParseText("using System;");
               SyntaxTree tree = CSharpSyntaxTree.ParseText(filetext);

#if IGNORE_NEW_CODE
            SyntaxTree tree = CSharpSyntaxTree.ParseText(
@"using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;

namespace TopLevel
{
    using Microsoft;
    using System.ComponentModel;

    namespace Child1
    {
        using Microsoft.Win32;
        using System.Runtime.InteropServices;

        class Foo { }
    }

    namespace Child2
    {
        using System.CodeDom;
        using Microsoft.CSharp;

        class Bar { }
    }
}");
#endif

               Console.WriteLine ("In C# process(System.UInt64) after parsing C# string! \n");

// #if IGNORE_NEW_CODE
            var root = (CompilationUnitSyntax)tree.GetRoot();

            var collector = new UsingCollector();
            collector.Visit(root);

            foreach (var directive in collector.Usings)
            {
                Console.WriteLine(directive.Name);
            }
// #endif

               Console.WriteLine ("In C# process(System.UInt64) after outputing the using directives! \n");

#if OUTPUT_ABSTRACT_SYNTAX_TREE
               var abstractSyntaxTreeTraversal_object = new AbstractSyntaxTreeTraversal();
               abstractSyntaxTreeTraversal_object.Visit(root);
#endif

#if OUTPUT_PARSE_TREE
               var parseTreeTraversal_object = new ParseTreeTraversal();
               parseTreeTraversal_object.Visit(root);
#endif

               Console.WriteLine ("In C# process(System.UInt64) after output of parse tree! \n");

#if IGNORE_OLD_CODE
               List<int> results = superCalculation();
               for (int i = 0; i < results.Count; i++)
                  {
                 // storeResults(container, results[i]);
                  }
#endif
               Console.WriteLine ("Leaving C# process(System.UInt64)! \n");
             }
        }
   }

