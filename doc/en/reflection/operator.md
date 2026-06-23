# Operator reflection macros

Generates proxy operator overloads that forward calls from a wrapper to the held
object through the executor.

- Header: `#include <scl/feature/reflection/operator.h>`

---

## Macro families

| Macro | Overloads generated | Use case |
|-------|---------------------|----------|
| `SCL_REFLECT_BINARY_OPERATOR(op, name)` | 24 member + 8 reverse-operand friend | symmetric binary operators (`+`, `==`, `<`, …) |
| `SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)` | 8 member only | prefix unary operators (`-`, `++`, `&`, …) |
| `SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)` | 8 member only | postfix unary operators (`++`, `--`) |
| `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)` | 24 member only | mandatory-member operators (`=`, `*=`, … ) |
| `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(op, name)` | 8 member only | mandatory-member prefix unary (`->`) |
| `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR(op, name)` | 8 member only | mandatory-member postfix unary |
| `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)` | 24 friend only | ADL-only binary operators |
| `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(op, name)` | 8 friend only | ADL-only prefix unary |
| `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(op, name)` | 8 friend only | ADL-only postfix unary |
| `SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name)` | 24 member only | `[]`, `()` (must be members in C++20) |

`SCL_REFLECT_TYPE` must be declared in the same class body before any operator macro.

---

## Symmetric binary operators (`SCL_REFLECT_BINARY_OPERATOR`)

`SCL_REFLECT_BINARY_OPERATOR` generates:

1. **Member overloads** (24) — selected when the wrapper is the **left** operand
   (`w op x`); they reflect `value.operator op(x)`.
2. **Reverse-operand hidden-friend overloads** (8, one per cv-ref qualifier) — found via
   ADL when the wrapper is the **right** operand (`x op w`); they reflect `x op value`.

The reverse friend is constrained out when the left operand is itself a wrapper
(`is_wrapper_v`), so `w1 op w2` resolves to the member overload without ambiguity. A
non-wrapper left operand cannot bind the wrapper parameter of the member overload, so each
direction has exactly one viable candidate.

```cpp
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_BINARY_OPERATOR(+, plus)        // w + x  (member)  AND  x + w  (reverse friend)
    SCL_REFLECT_BINARY_OPERATOR(==, equal_to)   // x == w (reverse friend); w == x may also work
                                                // via C++20 reversed candidates (compiler-dependent)
};
```

> Note: there is no reverse executor-override path — the `operator_<name>(exec, …)` override
> convention applies to the wrapper-left member overloads only. The reverse direction still
> routes through `Executor::execute`, so cross-cutting behaviour applies on both sides.
>
> Unary operators (`SCL_REFLECT_PREFIX_UNARY_OPERATOR` /
> `SCL_REFLECT_POSTFIX_UNARY_OPERATOR`) have a single operand and therefore no reverse case;
> they generate member overloads only. Use `SCL_REFLECT_FRIEND_*` for an ADL-only unary
> operator.

---

## Member-only variants (`SCL_REFLECT_MEMBER_*`)

The C++ standard ([over.oper]) requires `operator=`, `operator->`, and all
compound-assignment operators to be non-static member functions.  Generating
hidden-friend free-function overloads for them would be ill-formed.

Use `SCL_REFLECT_MEMBER_*` macros for these operators:

```cpp
SCL_REFLECT_MEMBER_BINARY_OPERATOR(=,   assign)
SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=,  plus_assign)
SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(->, arrow)
```

`reflect_operators` (the default base class of `scl::wrapper`) uses `SCL_REFLECT_MEMBER_*`
for `=`, `*=`, `/=`, `%=`, `+=`, `-=`, `<<=`, `>>=`, `&=`, `|=`, `^=`, and `->`.

---

## Friend-only variants (`SCL_REFLECT_FRIEND_*`)

Generates only hidden-friend free functions, with no member counterpart.

Use when the operator must be ADL-findable but must NOT be callable via
`w.operator op(...)` member syntax. Do not combine with a `SCL_REFLECT_*_OPERATOR`
macro for the same `name` — both emit the same class-level helpers, causing
duplicate member definitions.

---

## Dispatch (all families)

Each generated overload participates in the same two-path dispatch:

| Path | Active when | Action |
|------|-------------|--------|
| *executor-override* | `Executor::operator_<name>(exec, args...)` exists | calls that static member directly |
| *execute-path* | no executor override | calls `Executor::execute(exec, callable, value, args...)` |

---

## Constraint: distinct return types

All operator families share the same constraint as `SCL_REFLECT_METHOD`: overloads
for different cv-ref qualifiers must return distinct types so that
`SCL_HAS_QUALIFIED_METHOD` can distinguish them.

---

## Examples

### Symmetric equality via combined macro

```cpp
struct Length { bool operator==(Length) const &; };

template <typename W, typename E>
class scl::feature::reflect<W, E, Length>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_BINARY_OPERATOR(==, equal_to)
};

scl::wrapper<Length> a, b;
bool r1 = (a == b);          // member overload (both wrappers)
bool r2 = (a == Length{});   // member overload (wrapper on left)
bool r3 = (Length{} == a);   // reverse-operand hidden friend (wrapper on right)
```

### Mandatory-member assignment

```cpp
struct Counter {
    Counter & operator=(int) &;
};

template <typename W, typename E>
class scl::feature::reflect<W, E, Counter>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_MEMBER_BINARY_OPERATOR(=, assign)
};

scl::wrapper<Counter> c;
c = 42;   // OK — member operator=
```

---

## See also

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
