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

## Selection and return type

A call resolves in two steps: it first picks a **direction** (which generated
overload is viable), then a **path** (how that overload reaches the value).

### Direction (binary operators)

| Expression | Overload chosen | Reflects |
|------------|-----------------|----------|
| `w op x` (wrapper on the left) | member | `value op x` |
| `x op w` (wrapper on the right, `x` not a wrapper) | reverse-operand friend | `x op value` |
| `w1 op w2` (both wrappers) | member — the reverse friend is constrained out via `is_wrapper_v` | `value1 op w2` |

A member (wrapper-left) overload is viable **only when the wrapped value actually
has a matching `operator op`** for that cv-ref qualifier; otherwise it is
constrained out. Fundamental value types (`int`, `double`, …) have no member
operators, so for them the wrapper-left forms — including `w op x` and every
compound assignment `w op= x` — are **not** generated; only the reverse friends
(`x op w`) apply, because `x op value` is valid built-in syntax.

> `scl::wrapper`'s own copy/move assignment `=` is the one exception to the
> fundamental-type rule: `w = value` assigns the wrapped value for fundamental and
> class value types alike, because it is not a reflected member operator but a
> dedicated wrapper operator using expression assignment. See
> [wrapper](../wrapper/wrapper.md).

### Path (within the chosen overload)

| Path | Active when | Calls | Returns |
|------|-------------|-------|---------|
| *executor-override* | `Executor::operator_<name>(exec, args...)` exists | that static member directly, with the **raw** operands | the override's return type |
| *execute-path* | no executor override | `Executor::execute(exec, callable, exec, args...)`; the callable applies `scl::wrapper_cast` to wrapper arguments and resolves the value inside `execute()` | the reflected call's return type |

Every overload is declared `decltype(auto)`: the wrapper returns the override's
result (override path) or the wrapped value's operator result (execute path)
verbatim — it never rewraps the result. There is no executor-override path for the
reverse-operand friends; the `operator_<name>` convention applies to the
wrapper-left member overloads only.

---

## Constraint: distinct return types

All operator families share the same constraint as `SCL_REFLECT_METHOD`: the
value type's `operator op` overloads for different cv-ref qualifiers must return
distinct types, so that `SCL_HAS_QUALIFIED_METHOD` can tell them apart by return
type when selecting the qualifier-matching overload.

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

### Mandatory-member compound assignment

```cpp
struct Counter {
    Counter & operator+=(int) &;
};

template <typename W, typename E>
class scl::feature::reflect<W, E, Counter>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=, plus_assign)
};

scl::wrapper<Counter> c;
c += 42;   // OK — reflects Counter::operator+=
```

> Plain `=` is different. On `scl::wrapper` the copy/move/converting assignment is
> the wrapper's **own** operator (it must suppress the implicitly-deleted copy/move
> assignment; see [wrapper](../wrapper/wrapper.md)), which hides any reflected
> `operator=`. `SCL_REFLECT_MEMBER_BINARY_OPERATOR(=, assign)` therefore governs
> assignment only for a custom proxy type that inherits the reflected operators
> without declaring its own `operator=`, not for `scl::wrapper`. The compound
> assignments (`+=`, `*=`, …) and `->` are reflected normally on every wrapper.

---

## See also

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
