# inplace::uninitialized

In-place executor that holds the value in correctly sized and aligned raw storage,
enabling deferred (lazy) construction.

- Header: `#include <scl/feature/executor/inplace/uninitialized.h>`
- Declaration: `template <typename T> struct uninitialized;`
- Namespace: `scl::feature::inplace`

---

## Description

`inplace::uninitialized<T>` reserves correctly sized and aligned storage for a `T`
without constructing it. `access()` returns a reference to that storage via a
`reinterpret_cast`; the behaviour is undefined until a `T` object's lifetime has
begun there.

The storage is **private** and the executor exposes **no** public
`construct()` / `destroy()` and no value-forwarding constructor. Consequently:

- For an implicit-lifetime type (e.g. a trivially copyable `int`) a value is put in
  place by assigning through the wrapper, which begins the object's lifetime.
- For a non-trivial type (e.g. `std::string`) there is currently **no public way**
  to construct or destroy the held object, so such a specialisation is meaningful
  only at the type level — wrapper nesting and trait queries — not for storing a
  live value. (This missing construction API is a known limitation.)

Like `inplace::plain`, it does not provide `guard()`/`unguard()` and has no
synchronisation overhead.

## Interface

```cpp
template <typename T>
struct uninitialized
{
    using value_type = T;

    // Default construction — leaves the storage uninitialised.
    constexpr uninitialized() noexcept = default;

    // Raw-storage copy constructor — 8 overloads, one per cv-ref qualification.
    // Copies the underlying bytes via std::ranges::copy; constrained to
    // std::is_trivially_copyable_v<T>. Does NOT call T's constructor.
    constexpr explicit uninitialized(uninitialized cv_ref other) noexcept;  // × 8

    // The executor carries no assignment of its own — assignment is deleted.
    uninitialized & operator=(uninitialized const &) = delete;
    uninitialized & operator=(uninitialized &&) = delete;

    // Returns a reference to the stored object via reinterpret_cast.
    // Behaviour is undefined if the object's lifetime has not begun.
    template <typename Self>
    static constexpr decltype(auto) access(Self && self);

    // Invokes func(args...) immediately and returns the result.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);

private:
    alignas(T) std::byte m_storage[sizeof(T)];  // private
};
```

## Examples

### Reserving storage

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/uninitialized.h>
#include <string>

// Storage is reserved; no value is constructed.
scl::wrapper<int, scl::feature::inplace::uninitialized> w_int{};
scl::wrapper<std::string, scl::feature::inplace::uninitialized> w_string{};
```

For an implicit-lifetime type (e.g. `int`) a value is put in place by assigning
through the wrapper, which begins the object's lifetime; reading it before a value
has been stored is undefined behaviour. For a non-trivial type (e.g. `std::string`)
there is no public construction path, so such a specialisation is only meaningful at
the type level (nesting, trait queries).

## See also

- [inplace::plain](plain.md)
- [wrapper](../wrapper/wrapper.md)
