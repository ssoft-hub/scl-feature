#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <ScL/Feature/Reflection/Std.h>
#include <ScL/Feature/Wrapper.h>
#include <ScL/Feature/Tool.h>
#include <ScL/Utility/Qualifier.h>

using Key = ::std::string;
using Value = ::std::pair<::std::string, int>;
using Map = ::std::map<::std::string, ::std::pair<::std::string, int>>;

using AtomicMutexMap = ::ScL::Feature::Wrapper<Map, ::ScL::Feature::ThreadSafe::Atomic>;
using RecursiveMutexMap = ::ScL::Feature::Wrapper<Map, ::ScL::Feature::ThreadSafe::RecursiveMutex>;
using SharedMutexMap = ::ScL::Feature::Wrapper<Map, ::ScL::Feature::ThreadSafe::SharedMutex>;

using DefaultMap = ::ScL::Feature::Wrapper<Map, ::ScL::Feature::Inplace::Default>;
using ImplicitMap = ::ScL::Feature::Wrapper<Map, ::ScL::Feature::Implicit::Raw>;

template <typename _MapType>
void func( _MapType & test_map )
{
    for (size_t i = 0; i < 100000; ++i)
    {
        test_map.at("apple")->second++;
        test_map.find("potato")->second.second++;
    }
}

/*
 * Пример взят и модифицирован из
 * https://habrahabr.ru/post/328348/
 * http://coliru.stacked-crooked.com/a/5def728917274b22
 *
 * Итоговые значения равные 100000 означают, что каждое сложение в каждом
 * из 10 потоков было выполнено потокобезопасно.
 * Промежуточные значения не кратные 10000 говорят о том, что потоки исполнялись
 * параллельно или псевдопараллельно, т.е. прерывались посреди любой из операции
 * и в это время исполнялся другой поток.
 */
template <typename _MapType>
void example()
{
    ::std::cout << "Start" << ::std::endl;

    _MapType test_map;
    test_map["apple"]->first = "fruit";
    test_map["potato"]->first = "vegetable";

    ::std::vector<::std::thread> threads(::std::thread::hardware_concurrency());
    for (auto &thread : threads)
        thread = ::std::thread(func<_MapType>, ::std::ref(test_map));
    for (auto &thread : threads)
        thread.join();

    auto read_ptr = &::ScL::asConst(test_map);
    ::std::cout
        << "potato is " << read_ptr->at("potato").first
        << " " << read_ptr->at("potato").second
        << ", apple is " << read_ptr->at("apple").first
        << " " << read_ptr->at("apple").second
        << ::std::endl;

    ::std::cout << "Finish" << ::std::endl;
}

int main(int, char **)
{
    //example<Map>();
    // Параллельно, но не атомарно.
    example<DefaultMap>();
    // Параллельно, но не атомарно.
    example<ImplicitMap>();
    // Псевдопараллельно с блокировками, атомарно.
    example<AtomicMutexMap>();
    // Псевдопараллельно с блокировками, атомарно.
    example<RecursiveMutexMap>();
    // Псевдопараллельно с блокировками, атомарно.
    example<SharedMutexMap>();
    return 0;
}
