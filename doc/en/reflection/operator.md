# Operator reflection macros

Generates proxy operator overloads that forward calls from a wrapper to the held
object through the executor.

- Header: `#include <scl/feature/reflection/operator.h>`

---

## Macro families

| Macro | Reflects | Use case |
|-------|----------|----------|
| `SCL_REFLECT_BINARY_OPERATOR(op, name)` | `w op x`, `x op w`, `w1 op w2` | symmetric binary operators (`+`, `<`, …) |
| `SCL_REFLECT_EQUALITY_OPERATOR(op, name)` | `w op x`, `x op w` returning `bool` | `==`, `!=` |
| `SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)` | `op w` | prefix unary operators (`-`, `~`, `++`, …) |
| `SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` | postfix unary operators (`++`, `--`) |
| `SCL_REFLECT_SUBSCRIPT_OPERATOR(op, name)` | `w[i]` | `operator[]`, including a pointer value |
| `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)` | `w op x` | mandatory-member operators (`=`, `*=`, …) |
| `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(op, name)` | `op w` | mandatory-member prefix unary (`->`, `&`, `*`) |
| `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` | mandatory-member postfix unary |
| `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)` | `w op x` via ADL only | ADL-only binary operators |
| `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(op, name)` | `op w` via ADL only | ADL-only prefix unary |
| `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` via ADL only | ADL-only postfix unary |
| `SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name)` | `w(...)` | `()`, `->*` (member-only value side) |

`SCL_REFLECT_TYPE` must be declared in the same class body before any operator macro.

---

## What is reflected

A reflected operator applies the operator to the **wrapped value** as an expression —
`value op arg`, `op value`, `value[index]` — rather than calling a specific member.
Ordinary overload resolution on that expression then selects the value's own operator:

- a **member** operator of the value, when it has one (so a member operator takes
  precedence over a free one); or
- a **free or built-in** operator otherwise.

Because the built-in operators of fundamental and pointer types are reached through this
same expression, they are reflected too: `wrapper<int>{} + 5`, `-w`, `w1 * w2`, and
`wrapper<int*>{arr}[2]` all work. The cv-ref qualifier of the wrapper flows to the value,
so the value's `&` / `const &` / … overloads are picked per the wrapper's qualification.

The argument of a binary/subscript operator is reached one level down under the executor's
guard, so a wrapper argument (`w1 op w2`) is unwrapped once and `guard()`/`unguard()` fire
around the read; a plain argument passes through unchanged.

---

## Direction (binary operators)

| Expression | Overload | Reflects |
|------------|----------|----------|
| `w op x` (wrapper on the left) | member of the wrapper | `value op x` |
| `x op w` (wrapper on the right, `x` not a wrapper) | reverse-operand hidden friend | `x op value` |
| `w1 op w2` (both wrappers) | member of the left wrapper | `value1 op value2` |

The reverse friend is constrained out when the left operand is itself a wrapper
(`is_wrapper_v`), so `w1 op w2` resolves to the left wrapper's overload without ambiguity.

```cpp
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_BINARY_OPERATOR(+, plus)        // w + x, x + w, w1 + w2
    SCL_REFLECT_EQUALITY_OPERATOR(==, equal_to) // w == x, x == w (bool)
};
```

Unary operators have a single operand and therefore no reverse case. `SCL_REFLECT_EQUALITY_OPERATOR`
returns `bool` so the C++20 reversed comparison `x == w` is formed from the wrapper's member.

---

## Path (executor override vs execute)

| Path | Active when | Calls | Returns |
|------|-------------|-------|---------|
| *executor-override* | `Executor::operator_<name>(exec, args...)` exists | that static member directly, with the **raw** operands | the override's return type |
| *execute-path* | no executor override | `Executor::execute(exec, callable, exec, args...)`; the callable applies the operator to the value resolved inside `execute()` | the reflected expression's type |

The override convention applies to the wrapper-left / unary form only; the reverse-operand
friend has no override path but still routes through `Executor::execute`, so cross-cutting
behaviour applies on both sides. Every overload is declared `decltype(auto)` and returns the
result verbatim — it never rewraps.

---

## Member-only variants (`SCL_REFLECT_MEMBER_*`)

The C++ standard ([over.oper]) requires `operator=`, `operator->`, `operator()`, `operator[]`,
and the compound-assignment operators to be non-static member functions **on the wrapper**;
the *value* side may still be a free or built-in operator. `operator&` and `operator*` are kept
member-only as well — a free fallback for `&` would make `&wrapper` reflect `&value` and hijack
the pointer-to-wrapper syntax.

```cpp
SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=, plus_assign)
SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(->, arrow)
```

`reflect_operators` (the default base class of `scl::wrapper`) uses these for `*=`, `/=`, `%=`,
`+=`, `-=`, `<<=`, `>>=`, `&=`, `|=`, `^=`, `->`, `&`, `*`.

---

## Friend-only variants (`SCL_REFLECT_FRIEND_*`)

Reflect the value's **free** operator as a hidden friend of the wrapper: findable via ADL
(`w op x`), never via `w.operator op(...)` member syntax. Do not combine with a
`SCL_REFLECT_*_OPERATOR` macro for the same `name` — both emit the same class-level helpers,
causing duplicate definitions.

---

## Examples

### Symmetric equality

```cpp
struct Length { bool operator==(Length) const &; };

template <typename W, typename E>
class scl::feature::reflect<W, E, Length>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_EQUALITY_OPERATOR(==, equal_to)
};

scl::wrapper<Length> a, b;
bool r1 = (a == b);          // value == value (both wrappers)
bool r2 = (a == Length{});   // value == x     (wrapper on left)
bool r3 = (Length{} == a);   // x == value     (reverse friend)
```

### Fundamental value type

```cpp
scl::wrapper<int> n{40};
int  s = n + 2;   // 42 — built-in int + int, reflected
bool c = n < 50;  // built-in comparison
int  m = -n;      // built-in negation

scl::wrapper<int *> p{arr};
int e = p[2];     // built-in pointer subscript
```

### Mandatory-member compound assignment

```cpp
scl::wrapper<int> c{0};
c += 42;   // reflects int += int through the wrapper's own operator+=
```

> Plain `=` is different. On `scl::wrapper` the copy/move/converting assignment is the wrapper's
> **own** operator (it must suppress the implicitly-deleted copy/move assignment; see
> [wrapper](../wrapper/wrapper.md)), which hides any reflected `operator=`.

---

## See also

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
