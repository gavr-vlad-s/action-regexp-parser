/*
    File:    types_for_slr_tables.h
    Created: 09 August 2017 at 13:53 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TYPES_FOR_SLR_TABLES_H
#define TYPES_FOR_SLR_TABLES_H
#include <map>
#include <utility>
#include <cstddef>

template<typename NT>
struct Rule_info{
    NT      nt;  /* target for reducing */
    uint8_t len; /* rule length */
};

#define ANY ((uint8_t)(-1));

struct GOTO_entry{
    uint8_t from;
    uint8_t to;
};


enum class Parser_action_name{
    OK, Shift, Reduce, Reduce_without_back
};

struct Parser_action_info{
    uint16_t kind : 3;
    uint16_t arg  : 13;
};

#ifndef SHIFT
#define SHIFT(t)  {Parser_action_name::Shift,  t}
#define REDUCE(r) {Parser_action_name::Reduce, r}
#define ACCESS    {Parser_action_name::OK,     0}
#endif

template<typename NT>
using State_and_terminal  = std::pair<size_t, NT>;

template<typename NT>
using Parser_action_table = std::map<State_and_terminal<NT>, Parser_action_info>;

template<typename Lex_traits>
struct SLR_parser_tables{
    Rule_info<typename Lex_traits::Non_terminal_t>*           rules;
    GOTO_entry**                                              goto_table;
    Parser_action_table<typename Lex_traits::Non_terminal_t>* action_table
};
#endif