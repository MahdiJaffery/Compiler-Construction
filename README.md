# Compiler Construction Project

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A C++-based implementation of the **Lexical Analyzer** and **LL(1) Syntax Analyzer** components of a compiler, designed for educational and demonstrative purposes in compiler construction.

## Project Structure

```
Compiler/
├── src/
│ ├── Lexer/ # Lexical analyzer implementation
│ │ ├── Lexer.cpp
| | ├── Keywords.txt
| | ├── Source.txt
| | └── Transitions.txt
│ └── Parser/ # Syntax analyzer implementation
│   └── Parser.cpp
├── Docs/
│ ├── Phase 1 Lexical Analysis.pdf
│ └── Phase 2 LL1 Grammar.pdf
├── Tests/ # Test cases
├── Makefile # Build automation
└── README.md # This file
```

## Prerequisites

- GCC/G++ 9.0 or higher
- GNU Make 4.0+
- PDF viewer (for documentation)

## Building the Project

```bash
# Build and run both phases
make

# Build and run only lexer
make lexer

# Build and run only parser
make parser

# Clean generated files
make clean