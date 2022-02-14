# Модуль feature

ScL feature является частью инструментария [ScL](https://gitlab.com/ssoft-scl) и предоставляет собой набор средств для добавления дополнительных особенностей пользовательским типам данных посредством применения к ним внешней реализации.

Инструменты модуля позволяют добавить для экземпляра объекта любого типа задать дополнительные особенности, такие как

* строгое определение типов (strong typedef)
* потокобезопасность (thread safety)
* неявное обобщение (implicit shared)
* опциональность (optional)
* способ владения экземпляром объекта
* способ размещения экземпляра объекта
* ленивые или асинхронные вычисления
* адаптацию к произвольному интерфейсу
* и другие

Дополнительные свойства могут быть применены в суперпозиции в любой последовательности.

## Примеры

Строгая типизация

```cpp
// strong typedef
using Url = Wrapper< string, Inplace::Typedef< class url_tag > >;
using File = Wrapper< string, Inplace::Typedef< class file_tag > >;

void load ( Url ) { )
void load ( File ) { )

void example ()
{
    load( Url{ "http://foo.bar" } );
    load( File{ "foobar.txt" } );
}
```

Потокобезопасность

```cpp
// thread safety
using SafeVector = Wrapper< vector< int >, ThreadSafe::RecursiveMutex >;

void func ( SafeVector & container )
{
    auto count = container->size();
    for ( size_t i = 0; i < count; ++i )
        ++container[ i ];
}

void example ()
{
    SafeVector container;
    container.resize( 100000, 0 );

    vector< thread > threads( thread::hardware_concurrency() );
    for ( auto & t : threads )
        t = thread( func, container );
    for ( auto & t : threads )
        t.join();
}
```

Неявное обобщение

```cpp
// implicit shared string
using String = Wrapper< string, Implicit::Raw >;

void example ()
{
    String str{ "Hello Implicit Sharing!" };
    String other = str;                     // sharing "Hello Implicit Sharing!"
    other += " And Bye-Bye!";   // copy on write "Hello Implicit Sharing! And Bye-Bye!"
}
```

Суперпозиция особенностей

```cpp
// safety thread implicit shared string
using String = Wrapper< string, Implicit::Raw, ThreadSafe::RecursiveMutex >;
```

Особенности с дополнительным интерфейсом

```cpp
// optional string
using String = Wrapper< string, Inplace::Optional >;
String str;

cout << str.valueOr( "Hello" ) << endl;  // Hello
str.emplace( "Optional!" );
cout << str.valueOr( "Hello" ) << endl;  // Optional!

str = "And More!";
if ( str )
    cout << str.value( "empty" ) << endl;  // And More!
    
str.reset();
if ( str )
    cout << str.value( "empty" ) << endl;  // empty

```

И многое другое ...

## Директории

* doc - документация
* example - примеры
* include - заголовочные файлы C++
* src - исходные файлы C++
* test - тесты

