/*
    File:    slr_parser.h
    Created: 07 August 2017 at 13:17 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SLR_PARSER_H
#define SLR_PARSER_H
#include <memory>
#include "../include/myconcepts.h"
#include "../include/multipop_stack.h"

template<typename Lexem_type>
struct Attributes{
    union{
        Lexem_type li;
        struct{
            size_t begin_index;
            size_t end_index;
        } indeces;
    };
};

template<typename Lexem_type>
struct Stack_elem{
    //! State number of the SLR(1)-parser.
    size_t                 st_num;
    //! Attributes of the grammar symbol corresponding to the state.
    Attributes<Lexem_type> attr;
};


enum class Parser_action_name{
    OK, Shift, Reduce, Reduce_without_back
};

struct Parser_action_info{
    uint16_t kind : 3;
    uint16_t arg  : 13;
};

template<typename Rule_type, size_t N>
struct Rule_traits{
    using Rule_t                = Rule_type;
    static const size_t max_len = N;
};

template<typename Lexem_type, Integral T, Integral NT>
struct Lexem_traits{
    using Lexem_t        = Lexem_type;
    using Terminal_t     = T;
    using Non_terminal_t = NT;
};

template<typename R_traits, typename Lex_traits, typename S>
class SLR_parser{
public:
    SLR_parser<R_traits, Lex_traits, S>()                       = default;
    SLR_parser(const SLR_parser<R_traits, Lex_traits, S>& orig) = default;
    SLR_parser(const std::shared_ptr<S>& scaner_) :
        parser_stack(Multipop_stack<Stack_elem<Lex_traits::Lexem_t>>()),
        scaner(scaner_) {}

    virtual ~SLR_parser<R_traits, Lex_traits, S>()              = default;

    void shift(size_t shifted_state, Lex_traits::Lexem_t e);
    void reduce_without_back(R_traits::Rule_t r);
    void reduce(R_traits::Rule_t r);

    virtual void checker(Lex_traits::Lexem_t l){}
protected:
    size_t                                          current_state;
    Lex_traits::Lexem_t                             li;
    Lex_traits::Terminal_t                          t;
    Multipop_stack<Stack_elem<Lex_traits::Lexem_t>> parser_stack;
    std::shared_ptr<S>                              scaner;
    Stack_elem<Lex_traits::Lexem_t>                 rule_body[R_traits::max_len]
};

template<typename R_traits, typename Lex_traits, typename S>
void shift(size_t shifted_state, Lex_traits::Lexem_t e)
{
    using SE = Stack_elem<Lex_traits::Lexem_t>;
    SE selem;
    selem.st_num  = shifted_state;
    selem.attr.li = e;
    parser_stack.push(selem);
    checker(e);
}

template<typename R_traits, typename Lex_traits, typename S>
void reduce_without_back(R_traits::Rule_t r)
{
//     size_t rule_len = rules[r].len;
//     parser_stack.get_elems_from_top(rule_body, rule_len);
//     generate_command(r);
//
//     Stack_elem se;
//     se.attr    = (this->*attrib_calc[r])();
//     parser_stack.multi_pop(rule_len);
//     Stack_elem top_elem = parser_stack.top();
//     se.st_num           = next_state(top_elem.st_num, rules[r].nt);
//     parser_stack.push(se);
}

template<typename R_traits, typename Lex_traits, typename S>
void SLR_parser<R_traits, Lex_traits, S>::reduce(R_traits::Rule_t r)
{
    reduce_without_back(r);
    scaner->back();
}
#endif