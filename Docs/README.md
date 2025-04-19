# Compiler Project Documentation

This directory contains phase documentation for the compiler project.

## Files
```
Docs/
├── CC Phase 1.pdf
└── Project Phase 2 Grammar.pdf
```

## File Details

1. **`CC Phase 1.pdf`**  
   - Covers lexical analyzer implementation  
   - Includes token definitions and transition rules  
   - Documents the finite automaton design

2. **`Project Phase 2 Grammar.pdf`**  
   - Contains LL(1) parsing table  
   - Formal grammar specification  
   - Parsing strategy documentation

## Accessing Files

### Command Line:
```bash
# View PDF contents as text:
pdftotext "CC Phase 1.pdf" - | less
pdftotext "Project Phase 2 Grammar.pdf" -