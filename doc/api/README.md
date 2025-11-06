# API Documentation

This directory contains Doxygen documentation files (.dox) that document the API separately from the source code declarations.

## Purpose

The documentation has been separated from the code declarations to:
1. Keep the source code clean and focused on implementation
2. Allow more detailed documentation without cluttering the code
3. Make it easier to maintain and update documentation independently

## Files

- **Trait.dox** - Documentation for type traits and compatibility checks (src/Trait.h)
- **Wrapper.dox** - Documentation for the Wrapper type (src/Wrapper.h)
- **Tool_Heap_Raw.dox** - Documentation for Heap::Raw tool (src/Tool/Heap/Raw.h)
- **Tool_ThreadSafe_Lock.dox** - Documentation for ThreadSafe::Lock tool (src/Tool/ThreadSafe/Lock.h)
- **Tool_Guard_Featured.dox** - Documentation for Guard::Featured tool (src/Tool/Guard/Featured.h)

## Generating Documentation

To generate the HTML documentation using Doxygen:

```bash
cd doc
doxygen Doxyfile
```

The generated documentation will be in the `doc/doxygen_output/html` directory.

## Documentation Format

The documentation uses standard Doxygen commands:
- `@file` - Documents the file
- `@brief` - Brief description
- `@struct` - Documents a struct
- `@typedef` - Documents a type alias
- `@fn` - Documents a function

## Language

The documentation is written in Russian (Русский), as it matches the existing project documentation.
