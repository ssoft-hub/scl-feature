# Design: `scl::feature::implicit::polymorphic`

**Issue:** [#19 — design: clone mechanism for implicit::polymorphic executor](https://gitlab.com/ssoft-scl/scl-feature/-/issues/19)
**Milestone:** v1.0.0
**Status:** Decided — awaiting implementation

---

## Overview

`implicit::polymorphic<Value, Counter>` is an executor for the `scl::wrapper` system that provides:

- heap allocation with value semantics (copy-on-write),
- polymorphic storage (stores `Value*` but can hold any type that derives from `Value`),
- reference counting with configurable counter type,
- runtime type introspection (RTTI).

It fills the gap between `heap::shared` (no polymorphism, no RTTI) and a raw `shared_ptr<Base>` (no value semantics).

---

## Design Questions and Decisions

### Q1: Intrusive vs. external clone

**Decision: intrusive — `cloneable<Derived>` CRTP base.**

The user's base class derives from `scl::feature::implicit::cloneable<Derived>`. The CRTP base exposes a non-virtual `clone()` method that delegates to a virtual `do_clone()`. The default `do_clone()` performs `return new Derived{static_cast<Derived const&>(*this)}`; the user overrides it only when the default copy constructor is insufficient.

```cpp
// cloneable.h (src/scl/feature/implicit/cloneable.h)
namespace scl::feature::implicit
{
    class base_counted
    {
    public:
        virtual ~base_counted() = default;
        virtual base_counted* do_clone() const = 0;
    };

    template <typename Derived>
    class cloneable : public base_counted
    {
    public:
        base_counted* do_clone() const override
        {
            return new Derived{static_cast<Derived const&>(*this)};
        }
    };
}
```

User code:

```cpp
struct Shape : scl::feature::implicit::cloneable<Shape>
{
    int x, y;
    // do_clone() provided by CRTP default — just copy-constructs Shape
};

struct Circle : Shape
{
    double radius;
    // override do_clone() because Circle has extra state
    base_counted* do_clone() const override { return new Circle{*this}; }
};
```

**Why not Option B (external clone via `std::function`):** `std::function` carries heap allocation overhead, prevents `noexcept` move, and provides no RTTI path. Option B can be provided later as a separate `implicit::polymorphic_any` if needed.

---

### Q2: RTTI scope

**Decision: include full RTTI in v1.0.0.**

`implicit::polymorphic<Value, Counter>` exposes three RTTI methods:

| Method | Return | Semantics |
|---|---|---|
| `type_id()` | `std::type_info const&` | dynamic type of the stored object |
| `is_a<T>()` | `bool` | true if `dynamic_cast<T*>(m_base) != nullptr` |
| `dynamic_value_cast<T>()` | `T*` | pointer cast, returns `nullptr` on failure |

```cpp
wrapper<Shape, feature::implicit::polymorphic> s = Circle{...};

s->type_id();                          // typeid(Circle)
s->is_a<Circle>();                     // true
Circle* c = s->dynamic_value_cast<Circle>(); // non-null
```

Rationale: RTTI is the primary differentiator over `heap::shared`. Including it in v1.0.0 avoids a breaking API change later.

---

### Q3: Atomic vs. non-atomic reference counter

**Decision: template parameter with `std::atomic<std::int32_t>` default.**

```cpp
template <typename Value, typename Counter = std::atomic<std::int32_t>>
class polymorphic { ... };
```

Users in single-threaded contexts can opt in to a cheaper non-atomic counter:

```cpp
wrapper<Shape, feature::implicit::polymorphic<Shape, std::int32_t>> s = ...;
```

The `Counter` type must satisfy an `Incrementable` / `Decrementable` concept (to be defined in the implementation issue) covering `operator++`, `operator--`, and comparison with zero.

Rationale: matches the pattern established by `boost::intrusive_ptr`. Thread-safe by default; zero-cost opt-out for hot single-threaded paths.

---

### Q4: Multiple inheritance — pointer offset

**Decision: store `base_counted* m_base` and `Value* m_value` as separate pointers.**

When `Value` inherits `base_counted` through a non-first base, the byte addresses of `m_base` and `m_value` differ. After cloning via the virtual `do_clone()` (which returns `base_counted*`), the new `Value*` is recovered by replaying the original byte offset.

```cpp
// Stored state:
base_counted* m_base;   // points to the base_counted sub-object
Value*        m_value;  // points to the Value sub-object (may differ from m_base)

// Clone operation:
base_counted* new_base = m_base->do_clone();
std::ptrdiff_t offset  = reinterpret_cast<std::byte*>(m_value)
                       - reinterpret_cast<std::byte*>(m_base);
Value* new_value = reinterpret_cast<Value*>(
    reinterpret_cast<std::byte*>(new_base) + offset);
```

This is safe because the layout of a derived object is fixed at compile time; the offset computed from the original object is identical for the clone. The approach requires no second virtual dispatch and no user cooperation beyond deriving from `cloneable<Derived>`.

Example layout that motivates the two-pointer design:

```cpp
struct Logger { ... };

struct TrackedShape
    : Logger                                           // first base — non-counted
    , scl::feature::implicit::cloneable<TrackedShape>  // second base — base_counted
{
    int x, y;
};
```

Here `base_counted*` points past the `Logger` sub-object; `Value*` points to the start of `TrackedShape`. Without the offset, the recovered `Value*` would be wrong.

---

## Public API Sketch

```cpp
namespace scl::feature::implicit
{
    // --- cloneable.h ---

    class base_counted
    {
    public:
        virtual ~base_counted() = default;
        virtual base_counted* do_clone() const = 0;

        void add_ref() noexcept;   // increments counter
        bool release() noexcept;  // decrements; returns true when count reaches zero
    };

    template <typename Derived>
    class cloneable : public base_counted
    {
    public:
        base_counted* do_clone() const override;
    };


    // --- polymorphic.h ---

    template <typename Value, typename Counter = ::std::atomic<::std::int32_t>>
    class polymorphic
    {
    public:
        using value_type = Value;

        // Construction: takes ownership of a heap-allocated Value.
        // Typical use via a make helper or direct new expression.
        template <typename Derived = Value, typename... Args>
        static polymorphic make(Args&&... args);

        // Copy: share ownership (COW — clone deferred to guard())
        polymorphic(polymorphic const& other) noexcept;
        polymorphic(polymorphic& other) noexcept;

        // Move: transfer ownership
        polymorphic(polymorphic&& other) noexcept;

        // Executor protocol
        template <typename Self>
        static decltype(auto) value(Self&& self);

        template <typename Self, typename Func, typename... Args>
        static decltype(auto) execute(Self&& self, Func&& func, Args&&... args);

        // guard() — clone if reference count > 1 (COW trigger on mutable access)
        template <typename Self>
        static void guard(Self&& self);

        template <typename Self>
        static void unguard(Self&&) noexcept;

        // RTTI observers
        [[nodiscard]] ::std::type_info const& type_id() const noexcept;

        template <typename T>
        [[nodiscard]] bool is_a() const noexcept;

        template <typename T>
        [[nodiscard]] T* dynamic_value_cast() noexcept;

        // Reference count observer (for testing / diagnostics)
        [[nodiscard]] long use_count() const noexcept;

    private:
        base_counted* m_base{};
        Value*        m_value{};
    };

} // namespace scl::feature::implicit
```

---

## Files to create (implementation issue)

| File | Purpose |
|---|---|
| `src/scl/feature/implicit/cloneable.h` | `base_counted` + `cloneable<Derived>` CRTP |
| `src/scl/feature/implicit/polymorphic.h` | `polymorphic<Value, Counter>` executor |
| `test/implicit_polymorphic_gtest.cpp` | GoogleTest suite |
| `example/implicit_polymorphic_example.cpp` | Usage example |

`src/scl/feature/executors.h` should be updated to include the new header.
