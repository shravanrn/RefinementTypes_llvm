My notes
========
This repository modifies the llvm compiler to add a pass supporting the implementation of refinement types

External libraries required 
============================
Install the z3 solver https://github.com/Z3Prover/z3/releases
To test that z3 is installed correctly and added to the path, run the command 'z3 -h' in terminal/command prompt
You should see output similar to 'Z3 [version ...]. (C) Copyright ...'

Install liquid-fixpoint https://github.com/ucsd-progsys/liquid-fixpoint
To test that fixpoint is installed correctly and added to the path, run the command 'z3 -h' in terminal/command prompt
You should see output similar to 'fixpoint Copyright ...'


Building
========
The original instructions on how to build clang and llvm from https://clang.llvm.org/get_started.html work fine, except use the following code repos for clang and llvm instead of those mentioned in the link. 

In short, the get_started link asks you to do the following

1) Checkout
--------
Checkout https://github.com/shravanrn/RefinementTypes_llvm to a folder 'llvm'
Checkout https://github.com/shravanrn/RefinementTypes_clang to a the folder 'llvm/tools'

2) Make build
-----------
Create a new directory for the build
Run cmake to generate the build for you platform. More about CMake (https://llvm.org/docs/CMake.html) 
After this,
	If Windows, build with Visual studio
	If Unix like system, build with make

You have now compiled a custom version of clang that includes the support for refinement verfication.

Testing
========
Examples of using refinements provided in the examples folder
examples_refinement/safe - Examples of verifications that show that a piece of code is SAFE
examples_refinement/unsafe - Examples of verifications that show that a piece of code is UNSAFE

To run an example, compile with the following command
buildfolder/clang -mllvm -fixpoint-path=full/Path/To/fixpoint examples_refinement/safe/intPointerTest.c

Note: To get the value for full/Path/To/fixpoint
	On windows, run 'where fixpoint' on the command prompt
	On Unix like systems, run 'which fixpoint' in bash

Note: a generally useful command is to compile in O0 and output llvm code instead of a binary
buildfolder/clang -S -O0 -mllvm -fixpoint-path=full/Path/To/fixpoint -emit-llvm examples_refinement/safe/intPointerTest.c

The core constraint generator aka VariablesEnvironmentLib
==========================================================
This core constraints generation is a language neutral library which allows the user to declare variables and express constraints is located in RefinementTypes_llvm/lib/Transforms/LiquidTypes/VariablesEnvironment.cpp

An example of using this outside LLVM is given in 
RefinementTypes_llvm/tools/liquidTypes-varibleEnvLib-example/VaribleEnvLibExample.cpp

This is the library used in an LLVM pass that allows to generate constraints in order to verify refinements specified by the user.

--------------------------------------------Original Readme------------------------------------------------------------------------
Low Level Virtual Machine (LLVM)
================================

This directory and its subdirectories contain source code for LLVM,
a toolkit for the construction of highly optimized compilers,
optimizers, and runtime environments.

LLVM is open source software. You may freely distribute it under the terms of
the license agreement found in LICENSE.txt.

Please see the documentation provided in docs/ for further
assistance with LLVM, and in particular docs/GettingStarted.rst for getting
started with LLVM and docs/README.txt for an overview of LLVM's
documentation setup.

If you are writing a package for LLVM, see docs/Packaging.rst for our
suggestions.

