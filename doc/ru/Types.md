# Типы

## Type

Type - любой произвольный пользовательский тип.

## Wrapper

Wrapper - тип-обертка, наделяющий содержимый тип дополнительными особенностями.

Сигнатура класса

```
template < typename ValueType, typename ValueTool >
class Wrapper;
```

* `ValueType` - иходный тип данных.
* `ValueTool` - инструмент, реализующий особенности применения.

Тип реализует всевозможные реализации конструктора и оператора присвоения, а также унарные и бинарные операторы. Реализация `Wrapper` перенаправляет все вызовы содержащемуся внутри экземпляру значения `ValueType`, вызвав метод guardHolder до и метод unguardHolder после доступа к экземпляру значения `ValueType`.

### ValueType

На тип `ValueType` не накладывается никаких ограничений (??? м.б., что не является ссылкой ???).
Это может быть и пользовательский тип `Type` и обертка Wrapper.
В качестве типа `ValueType` не могут быть указаны финальные обертки типа Property/Variable и др.

### ValueTool

Тип `ValueTool` должен удолветворять следующим ограничениям:

* обязательно определять подтип `HolderType`
* обязательно реализовывать статические методы `guardHolder/unguardHolder`
* обязательно реализовывать статические методы `value`

#### HolderType

`HolderType` реализует логику владения экземплярами значений `ValueType` (управляет их временем жизни, опроеделяет способ размещения в памяти и др.).
Интерфейс `HolderType` содержит всевозможные реализации конструктора и оператора присвоения, а также деструктор.

#### Методы guardHolder/unguardHolder

Методы guardHolder/unguardHolder обеспечивают поведение, необходимое для реализации необходимых свойств.
При этом должно гарантироваться, что внутренний экземпляр значения `ValueType` не будет заменен на другой экземпляр между вызовами методов guardHolder/unguardHolder.
Само значение экземпляра `ValueType` может быть изменено.


#### Методы value

Методы value предоставляют доступ к экземпляру значения ValueType в виде ссылок различного вида.

# Синтаксис

Использование типа `Wrapper< Type >` или даже `Wrapper< ... < Wrapper< Type > ... >` вместо Type является "прозрачным" за исключением следующих моментов:

* нет явного преобразования к типу `Type` или ссылке на него;
* вызов методов производится не через `operator .`, а через `operator ->` (пока в стандарт C++ не добавлено предложение N4477 или P0352).


Пусть, например, исходный тип выглядит так

```cpp
class Type
{
    int m_int;
    double m_double;
    char[1024] m_chars;

public:
    void setIntData ( int value )
    {
        m_int = value;
    }

    int intData () const
    {
        return m_int;
    }

    //...

    char operator [] ( size_t i ) const
    {
        return m_chars[i];
    }

    Type & operator -= ( const Type & other )
    {
        m_int -= other.m_int;
        m_double -= other.m_double;
        for ( int i = 0; i < 1024; ++i )
            m_chars[i] -= other.m_chars[i];
        return *this;
    }
};
```

Тогда использование значений `Wrapper< Type >' вместо значений `Type` будет выглядеть так

```
// Type type_value;
Wrapper< Type > inst_value;

// type_value.setIntData( 10 );
inst_value->setIntData( 10 );

// char ch = type_value[ i ];
char ch = inst_value[ i ]

// type_value -= other_type_value;
inst_value -= other_type_value;
inst_value -= other_inst_value;


```