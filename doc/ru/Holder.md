# Тип Holder

Каждый инструментарий содержит тип **Holder**, который имеет стандартизованный интерфейс для управления временем жизни, наделением дополнительными свойствами и предоставлением доступа к экземпляру агрегированного значения.

Внутренняя реализация **Holder** должна обеспечивать наличие валидного экземпляра агрегированного значения в момент предоставления доступа, однако может не гарантировать, что последующий доступ будет предоставлен к тому же самому экземпляру агрегированного значения.

В случае выполнения операции переноса (`std::move(..)`) тип **Holder** может находиться в некорректном состоянии.

# Интерфейс Holder

Реализация **Holder** имеет стандартизованный интерфейс следующего вида

```cpp
template < typename _Value >
struct Holder
{
    using ThisType = Holder< _Value >;
    using Value = _Value;

    // Required
    template < typename ... _Arguments >
    Holder ( _Arguments && ... arguments );

    // Required
    Holder ( ThisType && other );
    Holder ( const ThisType && other );
    Holder ( volatile ThisType && other );
    Holder ( const volatile ThisType && other );
    Holder ( ThisType & other );
    Holder ( const ThisType & other );
    Holder ( volatile ThisType & other );
    Holder ( const volatile ThisType & other );

    // Required
    template < typename _OtherValue >
    Holder ( Holder< _OtherValue > && other );
    template < typename _OtherValue >
    Holder ( const Holder< _OtherValue > && other );
    template < typename _OtherValue >
    Holder ( volatile Holder< _OtherValue > && other );
    template < typename _OtherValue >
    Holder ( const volatile Holder< _OtherValue > && other );
    template < typename _OtherValue >
    Holder ( Holder< _OtherValue > & other );
    template < typename _OtherValue >
    Holder ( const Holder< _OtherValue > & other );
    template < typename _OtherValue >
    Holder ( volatile Holder< _OtherValue > & other );
    template < typename _OtherValue >
    Holder ( const volatile Holder< _OtherValue > & other );

    // Optional
    static void guard ( ThisType && );
    static void guard ( const ThisType && );
    static void guard ( volatile ThisType && );
    static void guard ( const volatile ThisType && );
    static void guard ( ThisType & );
    static void guard ( const ThisType & );
    static void guard ( volatile ThisType & );
    static void guard ( const volatile ThisType & );

    // Optional
    static void unguard ( ThisType && );
    static void unguard ( const ThisType && );
    static void unguard ( volatile ThisType && );
    static void unguard ( const volatile ThisType && );
    static void unguard ( ThisType & );
    static void unguard ( const ThisType & );
    static void unguard ( volatile ThisType & );
    static void unguard ( const volatile ThisType & );

    // Required
    static Value && value ( ThisType && holder );
    static const Value && value ( const ThisType && holder );
    static volatile Value && value ( volatile ThisType && holder );
    static const volatile Value && value ( const volatile ThisType && holder );
    static Value & value ( ThisType & holder );
    static const Value & value ( const ThisType & holder );
    static volatile Value & value ( volatile ThisType & holder );
    static const volatile Value & value ( const volatile ThisType & holder );

    // Optional for any operator by [Name]
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( ThisType && other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( const ThisType && other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( volatile ThisType && other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( const volatile ThisType && other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( ThisType & other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( const ThisType & other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( volatile ThisType & other, _Arguments && ... arguments );
    template < typename ... _Arguments >
    static decltype(auto) operator[Name] ( const volatile ThisType & other, _Arguments && ... arguments );
};
```

***Важно!*** *Должны быть реализованы все конструкторы и методы там, где об этом явно написано. В противном случае, компилятор может выбрать неверную интерпретацию методов, что приведет к ошибке компиляции.*

***Примечание:*** *Ранее и далее по тексту обязательные требования отражаются использованием глагола "должен", рекомендации - глаголом "следует", а разрешения - глаголом "может".*

## Конструкторы

### Конструктор инициализации

Конструктор инициализации агрегированного значения по заданным параметрам используется, если количество параметров не равно 1 или тип заданного параметра не является этим же или подобным **Holder**.

```cpp
template < typename ... _Arguments >
Holder ( _Arguments && ... arguments );
```

Если для типа агрегированного значения не предусмотрен подходящий конструктор, то это приведет к ошибке компиляции.

### Конструкторы копирования и перемещения для этого же типа Holder

Конструкторы копирования и перемещения для этого же типа **Holder** используются при полном соответствии типа параметра данному типу **Holder**.

```cpp
Holder ( ThisType && other );
Holder ( const ThisType && other );
Holder ( volatile ThisType && other );
Holder ( const volatile ThisType && other );
Holder ( ThisType & other );
Holder ( const ThisType & other );
Holder ( volatile ThisType & other );
Holder ( const volatile ThisType & other );
```

Для пользовательских типов **Holder** должна быть обязательно обеспечена реализация всех этих конструкторов, иначе вместо них на этапе компиляции будет использован конструктор инициализации агрегированного значения по заданным параметрам, что может привести к ошибке компиляции.

***Заметка:*** *Данный набор конструкторов не получается реализовать в виде шаблона, так как в этом случае компилятор считает данный вид конструкторов удаленными (= deleted).*

### Конструкторы копирования и перемещения для совместимого типа Holder< _OtherValue >

Конструкторы копирования и перемещения для совместимого типа **Holder< _OtherValue >** используются при совместимости типа параметра данному типу **Holder**. Типы **Holder** являются совместимыми, если тип агрегированного экземпляра значения параметра **Holder< _OtherValue >** является тем же самым или наследником от типа экземпляра агрегированного значения этого **Holder**, с учетом cv модификаторов (const/volatile).

```cpp
template < typename _OtherValue >
Holder ( Holder< _OtherValue > && other );
template < typename _OtherValue >
Holder ( const Holder< _OtherValue > && other );
template < typename _OtherValue >
Holder ( volatile Holder< _OtherValue > && other );
template < typename _OtherValue >
Holder ( const volatile Holder< _OtherValue > && other );
template < typename _OtherValue >
Holder ( Holder< _OtherValue > & other );
template < typename _OtherValue >
Holder ( const Holder< _OtherValue > & other );
template < typename _OtherValue >
Holder ( volatile Holder< _OtherValue > & other );
template < typename _OtherValue >
Holder ( const volatile Holder< _OtherValue > & other );
```

Для пользовательских типов **Holder** должна быть обязательно обеспечена реализация всех этих конструкторов, иначе вместо них на этапе компиляции будет использован конструктор инициализации агрегированного значения по заданным параметрам, что может привести к ошибке компиляции.

## Методы применения особенностей

Данный набор методов предусматривает включение **guard()** и отключение **unguard()** дополнительных особенностей к внутреннему агрегированному экземпляру значения. В зависимости от типа ссылки на значение **Holder** (rvalue/lvalue) данные методы позволяют реализовать разные способы применения свойств.

```cpp
static void guard ( ThisType && );
static void guard ( const ThisType && );
static void guard ( volatile ThisType && );
static void guard ( const volatile ThisType && );
static void guard ( ThisType & );
static void guard ( const ThisType & );
static void guard ( volatile ThisType & );
static void guard ( const volatile ThisType & );

static void unguard ( ThisType && );
static void unguard ( const ThisType && );
static void unguard ( volatile ThisType && );
static void unguard ( const volatile ThisType && );
static void unguard ( ThisType & );
static void unguard ( const ThisType & );
static void unguard ( volatile ThisType & );
static void unguard ( const volatile ThisType & );
```

Обычно (но не обязательно) эти методы вызываются в совокупности с методом **value()**, в следующем порядке - **guard()**, **value()**, **unguard()**.
Реализация данных методов является опциональной. В случае отсутствия их реализации применение дополнительных особенностей не происходит.

При их реализации следует убедится, что применение дополнительных особенностей реализовано для всех необходимых вариантов использования (mutable/constant/volatile, rvalue/lvalue).

При необходимости, методы могут быть реализованы в виде шаблона, например, так:

```cpp
template < typename _HolderRefer >
static void guard ( _HolderRefer && holder );
template < typename _HolderRefer >
static void unguard ( _HolderRefer && holder );
```


## Доступ к экземпляру агрегированного значения

Доступ к экземпляру агрегированного значения осуществляется с помощью методов **value()**.

```cpp
static Value && value ( ThisType && holder );
static const Value && value ( const ThisType && holder );
static volatile Value && value ( volatile ThisType && holder );
static const volatile Value && value ( const volatile ThisType && holder );
static Value & value ( ThisType & holder );
static const Value & value ( const ThisType & holder );
static volatile Value & value ( volatile ThisType & holder );
static const volatile Value & value ( const volatile ThisType & holder );
```

Для пользовательских типов **Holder** должна быть обязательно обеспечена реализация всех этих методов, отсутствие хотя бы одного из них может привести к ошибке компиляции.

При необходимости, методы могут быть реализованы в виде шаблона, например, так:

```cpp
template < typename _HolderRefer,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
static decltype(auto) value ( _HolderRefer && holder );
```

## Операторы

Опционально для **Holder** могут быть специализированы реализации любых операторов.
Если специализации явно не определены, то используется реализация по умолчанию, учитывающая результат операции между экземплярами агрегированных значений.

Если результат воздействия оператора на экземпляры агрегированных значений является:
* значением фундаментального или перечисляемого типа, тогда возвращается это значение;
* значением типа **Value**, тогда возвращается **Wrapper< Value, DefaultTool >**;
* ссылкой на значение любого типа **Value**, тогда
    * если **Wrapper** является и правым, и левым операндом, **Wrapper< Value, BothOperandTool >**
    * если **Wrapper** является только левым или правым операндом, **Wrapper< Value, Invokable::UnaryTool >**
    * если **Wrapper** является только левым операндом, **Wrapper< Value, LeftOperandTool >**

Значения типа **Wrapper< Value, BothOperandTool >**, **Wrapper< Value, RightOperandTool >**, **Wrapper< Value, LeftOperandTool >** включают применение дополнительных особенностей к участвующим в операции **Wrapper**, инициируют идентичную операцию между экземплярами агрегированных значений и отключают применение дополнительные особенностей только с вызовом своего деструктора.

В случае специализации операторов в **Holder**, в качестве результата операции над **Wrapper** возвращается соответствующая ссылка на сам **Wrapper**.
Вызов специализаций операторов в **Holder** прозводится без применения каких-либо дополнительных особенностей над операндами, поэтому сама реализация этих специализаций должна предусматривать применения дополнительных особенностей при необходимости.

Для унарных операторов без аргументов интерфейс методов должен выгладеть так:

```cpp
static void operator[Name] ( ThisType && other );
static void operator[Name] ( const ThisType && other );
static void operator[Name] ( volatile ThisType && other );
static void operator[Name] ( const volatile ThisType && other );
static void operator[Name] ( ThisType & other );
static void operator[Name] ( const ThisType & other );
static void operator[Name] ( volatile ThisType & other );
static void operator[Name] ( const volatile ThisType & other );
```

или в виде шаблона:

```cpp
template < typename _HolderRefer,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
static void operator[Name] ( _HolderRefer && holder );
```

где в качестве **[Name]** могут быть использованы:

* **AddressOf** - operator & ();
* **Indirection** - operator * ();
* **PrefixPlus** - operator + ();
* **PrefixMinus** - operator - ();
* **PrefixPlusPlus** - operator ++ ();
* **PrefixMinusMinus** - operator -- ();
* **PostfixPlusPlus** - operator ++ ( int );
* **PostfixMinusMinus** - operator -- ( int );
* **PrefixBitwiseNot** - operator ~ ();
* **PrefixLogicalNot** - operator ! ();

Для унарных операторов с одним аргументом интерфейс методов должен выгладеть так:

```cpp
template < typename _Argument >
static void operator[Name] ( ThisType && other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( const ThisType && other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( volatile ThisType && other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( const volatile ThisType && other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( ThisType & other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( const ThisType & other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( volatile ThisType & other, _Argument && argument );
template < typename _Argument >
static void operator[Name] ( const volatile ThisType & other, _Argument && argument );
```

или в виде шаблона:

```cpp
template < typename _HolderRefer, typename _Argument,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
static void operator[Name] ( _HolderRefer && holder, _Argument && argument );
```

где в качестве **[Name]** могут быть использованы:

* **MemberIndirection** - operator ->* ( _Argument && );
* **Comma** - operator , ( _Argument && );
* **SquareBrackets** - operator [] ( _Argument && );

Для унарных операторов с несколькими аргументами интерфейс методов должен выгладеть так:

```cpp
template < typename ... _Arguments >
static void operator[Name] ( ThisType && other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( const ThisType && other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( volatile ThisType && other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( const volatile ThisType && other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( ThisType & other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( const ThisType & other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( volatile ThisType & other, _Arguments && ... arguments );
template < typename ... _Arguments >
static void operator[Name] ( const volatile ThisType & other, _Arguments && ... arguments );
```

или в виде шаблона:

```cpp
template < typename _HolderRefer, typename ... _Arguments,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
static void operator[Name] ( _HolderRefer && holder, _Arguments && ... arguments );
```

где в качестве **[Name]** могут быть использованы:

* **RoundBrackets** - operator () ( _Arguments && ... );

Для бинарных операторов в случае, если экземпляр значения этого типа **Holder** находится слева, а справа находится экземпляр значения не являющийся никаким **Holder**, интерфейс методов должен выгладеть так:

```cpp
template < typename _Right >
static void operator[Name]Left ( ThisType && left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( const ThisType && left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( volatile ThisType && left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( const volatile ThisType && left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( ThisType & left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( const ThisType & left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( volatile ThisType & left, _Right && right );
template < typename _Right >
static void operator[Name]Left ( const volatile ThisType & left, _Right && right );
```

или в виде шаблона:

```cpp
template < typename _HolderLeftRefer, typename _Right,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderLeftRefer > >{} > >
static void operator[Name]Left ( _HolderLeftRefer && left, _Right && right );
```

Для бинарных операторов в случае, если экземпляр значения этого типа **Holder** находится справа, а слева находится экземпляр значения не являющийся никаким **Holder**, интерфейс методов должен выгладеть так:

```cpp
template < typename _Left >
static void operator[Name]Right ( _Left && left, ThisType && right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, const ThisType && right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, volatile ThisType && right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, const volatile ThisType && right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, ThisType & right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, const ThisType & right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, volatile ThisType & right );
template < typename _Left >
static void operator[Name]Right ( _Left && left, const volatile ThisType & right );
```

или в виде шаблона:

```cpp
template < typename _Left, typename _HolderRightRefer,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRightRefer > >{} > >
static void operator[Name]Right ( _Left && left, _HolderRightRefer && right );
```

Для бинарных операторов в случае, если экземпляр значения этого типа **Holder** находится справа, а слева находится экземпляр значения являющийся подобным **Holder** (тем же самым или со значением производного типа), интерфейс методов должен выгладеть так:

```cpp
template < typename _Right >
static void operator[Name] ( ThisType && left, Holder< _Right > && right );
// ... и т.д. все возможные сочетания
```

или в виде шаблона:

```cpp
template < typename _HolderLeftRefer, typename _HolderRightRefer,
    typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderLeftRefer > >{} > >
static void operator[Name] ( _HolderLeftRefer && left, _HolderRightRefer && right );
```

где в качестве **[Name]** могут быть использованы:

* **Equal** - operator == ( _Argument && );
* **NotEqual** - operator != ( _Argument && );
* **Less** - operator < ( _Argument && );
* **LessOrEqual** - operator <= ( _Argument && );
* **Greater** - operator > ( _Argument && );
* **GreaterOrEqual** - operator >= ( _Argument && );
* **Multiply** - operator * ( _Argument && );
* **Divide** - operator / ( _Argument && );
* **Modulo** - operator % ( _Argument && );
* **Addition** - operator + ( _Argument && );
* **Subtraction** - operator - ( _Argument && );
* **LeftShift** - operator << ( _Argument && );
* **RightShift** - operator >> ( _Argument && );
* **BitwiseAnd** - operator & ( _Argument && );
* **BitwiseOr** - operator | ( _Argument && );
* **BitwiseXor** - operator ^ ( _Argument && );
* **LogicalAnd** - operator && ( _Argument && );
* **LogicalOr** - operator || ( _Argument && );
* **Assignment** - operator = ( _Argument && );
* **MultiplyAssignment** - operator *= ( _Argument && );
* **DivideAssignment** - operator /= ( _Argument && );
* **ModuloAssignment** - operator %= ( _Argument && );
* **AdditionAssignment** - operator += ( _Argument && );
* **SubtractionAssignment** - operator -= ( _Argument && );
* **LeftShiftAssignment** - operator <<= ( _Argument && );
* **RightShiftAssignment** - operator >>= ( _Argument && );
* **BitwiseAndAssignment** - operator &= ( _Argument && );
* **BitwiseOrAssignment** - operator |= ( _Argument && );
* **BitwiseXorAssignment** - operator ^= ( _Argument && );

Все другие вариации бинарных операторов разрешаются до использования вышеперечисленных.
