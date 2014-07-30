Kernel from Loop Tree
=====================

This module uses the [Multiple File Builder (MFB)](../../midend/MFB) to build computation kernel from a description of the loops.
It is a template library that can be specialized for different input languages, and target languages and runtimes.

[RoseACC](http://github.com/tristanvdb/RoseACC-workspace) implements one specialization of KLT.
In RoseACC, the input language is OpenACC, the target language is OpenCL C, and the target runtime is libOpenACC (RoseACC own implementation of OpenACC).

