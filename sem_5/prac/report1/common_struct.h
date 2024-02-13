#ifndef COMMON_STRUCT
#define COMMON_STRUCT

#ifdef __riscv64__
#include "map.h"
#endif
#include "compose_keys.hxx"

//Номера структур данных в SPE
enum Structures : uint32_t {
    null            = 0,
    tables_pnum     = 1     //Таблица 3
};


#ifdef __riscv64__
//Задание даипазонов и курсоров
template<typename Range>
struct reverse {
        Range r;
        [[gnu::always_inline]] reverse(Range r) : r(r) {}
        [[gnu::always_inline]] auto begin() {return r.rbegin();}
        [[gnu::always_inline]] auto end() {return r.rend();}
};

template<typename K, typename V>
struct Handle {
        bool ret_val;
        K k{get_result_key<K>()};
        V v{get_result_value<V>()};
        [[gnu::always_inline]] Handle(bool ret_val) : ret_val(ret_val) {
        }

        [[gnu::always_inline]] operator bool() const {
                return ret_val;
        }

        [[gnu::always_inline]] K key() const {
                return k;
        }

        [[gnu::always_inline]] V value() const {
                return v;
        }
};
#endif

//////////////////////////////////////
// Описание формата ключа и значения
//////////////////////////////////////


struct tables {
    int struct_number;
    constexpr tables(int struct_number) : struct_number(struct_number) {}
 STRUCT(key)
 {
    uint64_t     ip      :64;
 };

 //Запись для формирования значений
 STRUCT(val)
 {
     uint64_t 	 val     :64;
 };
 //Обязательная типизация
 #ifdef __riscv64__
    DEFINE_DEFAULT_KEYVAL(key, val)
 #endif
};

constexpr tables TABLES(Structures::tables_pnum);

#endif //COMMON_STRUCT
