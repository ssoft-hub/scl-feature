#include <ScL/Feature/Tool.h>
#include <ScL/Feature/Wrapper.h>
#include <map>

using namespace ScL::Feature;

struct CreditCard
{
    Wrapper<int64_t> m_id;
    Wrapper<int32_t> m_limit;
};

struct UserCard
{
    Wrapper< ::std::string> m_name;
    Wrapper<CreditCard> m_card;
};

using Key = Wrapper< ::std::string, Implicit::Raw>;
using Value = Wrapper<UserCard, Implicit::Raw>;
using Map = ::std::map<Key, Value>;

static Wrapper<Map> global_map;

// Идеальный вариант, "прозрачный" относительно Wrapper.
#ifdef _IDEAL_
void appendIdealVariant()
{
    Wrapper<Key> key = "ideal";
    global_map[key].m_name = "Ideal user";
    global_map[key].m_card.m_id = -1L;
    global_map[key].m_card.m_limit = -1;
}
#endif

// Вариант с прямым последовательным вызовом методов членов класса guard/cguard и access.
// Не совместим со значениями отличными от Wrapper.
#ifdef _GUARD_AS_MEMBER_
void appendZeroVariant()
{
    Wrapper<Key> key = "zero";
    global_map.guard().access()[key].guard().access().m_name = "Zero user";
    global_map.guard().access()[key].guard().access().m_card.guard().access().m_id = 1L;
    global_map.guard().access()[key].guard().access().m_card.guard().access().m_limit = 1;
}
#endif

// Вариант с вызовом глобальных методов guard/cguard с прямым вызовом метода
// члена класса access. Cовместим со значениями отличными от Wrapper.
#ifdef _GUARD_AS_GLOBAL_
void appendFirstVariant()
{
    Wrapper<Key> key = "first";
    guard(guard(global_map).access()[key]).access().m_name = "First user";
    guard(guard(guard(global_map).access()[key]).access().m_card).access().m_id = 1L;
    guard(guard(guard(global_map).access()[key]).access().m_card).access().m_limit = 1;
}
#endif

// Вариант с вызовом глобальных методов guard/cguard и разыменованием.
// Cовместим со значениями отличными от Wrapper.
#ifdef _GUARD_AS_GLOBAL_
void appendSecondVariant()
{
    Wrapper<Key> key = "second";
    (*guard((*guard(global_map))[key])).m_name = "Second user";
    (*guard((*guard((*guard(global_map))[key])).m_card)).m_id = 2L;
    (*guard((*guard((*guard(global_map))[key])).m_card)).m_limit = 22;
}
#endif

// Вариант с двойным разыменованием.
// Cовместим со значениями отличными от Wrapper.
void appendThirdVariant()
{
    Wrapper<Key> key = "third";
    (*&(*&global_map)[*&key]).m_name = "Third user";
    (*&(*&(*&global_map)[*&key]).m_card).m_id = 3L;
    (*&(*&(*&global_map)[*&key]).m_card).m_limit = 333;
}

// Вариант с использованием оператора ->.
// Не совместим со значениями отличными от Wrapper.
void appendFourthVariant()
{
    Wrapper<Key> key = "fourth";
    global_map->operator[](key)->m_name = "Fourth user";
    global_map->operator[](key)->m_card->m_id = 4L;
    global_map->operator[](key)->m_card->m_limit = 4444;
}

// Вариант со смешанным использованием двойного разыменования для доступа
// к значению и оператора -> для доступа к членам класса.
// Не cовместим со значениями отличными от Wrapper.
void appendFifthVariant()
{
    Wrapper<Key> key = "fifth";
    (*&global_map)[key]->m_name = "Fifth user";
    (*&global_map)[key]->m_card->m_id = 5L;
    (*&global_map)[key]->m_card->m_limit = 55555;
}

// Вариант с использованием макроса vGet/cGet.
// Cовместим со значениями отличными от Wrapper.
#ifdef _GET_MACROS_
void appendSixthVariant()
{
    Wrapper<Key> key = "sixth";
    vGet(vGet(global_map)[key]).m_name = "Sixth user";
    vGet(vGet(vGet(global_map)[key]).m_card).m_id = 5L;
    vGet(vGet(vGet(global_map)[key]).m_card).m_limit = 55555;
}
#endif

// Вариант с рефлексией всех операторов.
// Не совместим со значениями отличными от Wrapper.
void appendSeventhVariant()
{
    Wrapper<Key> key = "fifth";
    global_map[key]->m_name = "Fifth user";
    global_map[key]->m_card->m_id = 5L;
    global_map[key]->m_card->m_limit = 55555;
}

int main(int, char **)
{
    appendThirdVariant();
    appendFourthVariant();
    appendFifthVariant();
    appendSeventhVariant();

    return 0;
}
