
Directive-based Languages eXtensions (DLX)
==========================================

This module provides abstraction to describe a *Directive-based Languages eXtensions* (DLX).
We call DLX, a subclass of language that can be defined for directive.
OpenMP, OpenACC, HMPP, and many other language using directives 
In a DLX, directive are of the form: `language construct clause_1(param, ...) ... clause_n(...)`.
Where:
 * `language` is a label representing the name of the language ("omp", "acc", "hmpp")
 * `construct` is a label and it represents a function of the language
 * `clause_X` are labels too, they are arguments to the construct
 * `param` can be anything... particularly it can a piece of c or c++ code

For more information on how to use the DLX module, you can look at the exemples at [DLX/MFB/MDCG Demos](../../../projects/demos-dlx-mfb-mdcg).
In this mini-project we demonstrate how the three modules [DLX](.), [MFB](../../midend/MFB), and [MDCG](../../midend/MDCG) can be used together to build *DLXs*.

