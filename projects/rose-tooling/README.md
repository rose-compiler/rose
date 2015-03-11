ROSE's Tooling Language
=======================

ROSE's Tooling Language is a [Directive-based Language eXtension (DLX)](../../src/frontend/DLX/) for source code analysis and transformations.
It enables a finer grain of control than applying analysis and transformation at the file scope.

## Tooling DLX



### *set*

Propagate a const value to replace a scalar variable. Applied in the scope of the target variable.

### *init*

Takes a file (CSV, SQLITE) and generates initializer. 

### *callgraph*

Marks functions we want to appear in the call graph. Edges between functions with the same tag are emphasized.

### *outline*

Outline the following statement in its own function. If 'file' is provided the outlined function is *appended* to the file.

### *replay*

Replay first outline the following statement. Then it provides runtime support to save the outlined function inputs and outputs. It enables to optimize a kernel in isolation.

### *grapher*

Generates GraphViz representation of a sub-AST

