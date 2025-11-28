# SimCL — Simulation C Language (MVP Compiler + VM)

This is the minimal SimCL compiler + bytecode VM + scientific runtime.

## Requirements
- ANSI C compiler (cc, gcc, clang in C89 mode)
- POSIX environment recommended

## Build

make


## Run

./simcl <file.simcl>


## Project Structure
The project contains:
- Lexer
- Parser
- AST
- Semantic analysis
- IR + optimizer
- Bytecode generator
- VM
- Scientific runtime
- Standard library
- Example programs in /tests

## Status
This is Phase 1 (file skeleton only).