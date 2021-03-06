The Metaprogramming Tools (MPT) package is a header-only C++ library which offers general utilities to handle compile-time expressions.
Each header in the package corresponds to a non-independent library:
- [arfunctors](arfunctors_8hpp.html): arithmetic and relational functors
- [keywords](keywords_8hpp.html): objects configurable by keywords
- [members](members_8hpp.html): wrappers and checkers for class members
- [signature](signature_8hpp.html): handle (member) function and functor signatures
- [smart_enum](smart_enum_8hpp.html): smart enumeration types
- [soa](soa_8hpp.html): containers with struct-of-arrays memory layout
- [typed_any](typed_any_8hpp.html): typed *any* objects
- [types](types_8hpp.html): handle types in template parameter lists
- [values](values_8hpp.html): handle values in template argument lists

The package is built on top of the standard library using the C++20 features.