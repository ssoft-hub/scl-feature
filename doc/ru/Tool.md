# Тип Tool

## Введение

Тип **Tool** вспомогательным типом, введенным для удобства работы с **Holder** в составе **Wrapper**.

## Применение

Введением вспомогательного типа **Tool** позволяет упростить определение различных типов **Wrapper** и использовать запись вида

```cpp
Wrapper< Type, Tool > value;
```

вместо

```cpp
Wrapper< Type, Holder< Type > > value;
```

В последней записи требуется корректно указывать параметры шаблона для **Holder**, что является потенциальным местом для ошибок. К тому же, ещё одной функцией **Tool** является возможность дополнительной шаблонной параметризации **Holder**, например, так

```cpp
template < typename _Locker >
struct MyLockTool
{
    template < typename _Value >
    struct Holder
    {
        _Locker m_locker;
        // ...
    };
};

```

и использования в дальнейшем записей в виде

```cpp
Wrapper< Type, MyLockTool< ::std::mutex > > first;
Wrapper< Type, MyLockTool< ::std::recursive_mutex > > second;
Wrapper< Type, MyLockTool< ::std::recursive_timed_mutex > > third;
```
