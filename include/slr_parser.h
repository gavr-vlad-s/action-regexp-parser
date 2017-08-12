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
#include <map>
#include <utility>
#include "../include/myconcepts.h"
#include "../include/multipop_stack.h"
#include "../include/types_for_slr_tables.h"

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

template<typename R_traits, typename Lex_traits, typename S, typename Container>
class SLR_parser{
public:
    SLR_parser<R_traits, Lex_traits, S, Container>()                       = default;
    SLR_parser(const SLR_parser<R_traits, Lex_traits, S, Container>& orig) = default;
    SLR_parser(const std::shared_ptr<S>&            scaner_,
               const SLR_parser_tables<Lex_traits>& tables) : scaner(scaner_)
    {
        rules        = tables.rules;
        goto_table   = tables.goto_table;
        action_table = tables.action_table;
    }

    virtual ~SLR_parser<R_traits, Lex_traits, S, Container>()              = default;

    void slr_parsing(Container& buf);

protected:
    using Lexem_type        = typename Lex_traits::Lexem_t;
    using Non_terminal_type = typename Lex_traits::Non_terminal_t;
    using Terminal_type     = typename Lex_traits::Terminal_t;
    using Rule_type         = typename R_traits::Rule_t;
    using SE                = Stack_elem<Lexem_type>;

    size_t                                  current_state;
    Lexem_type                              li;
    Terminal_type                           t;
    Multipop_stack<SE>                      parser_stack;
    std::shared_ptr<S>                      scaner;
    SE                                      rule_body[R_traits::max_len];
    Container                               buf_;

    virtual void                   checker(Lexem_type l)               = 0;
    virtual void                   generate_command(Rule_type r)       = 0;
    virtual Attributes<Lexem_type> attrib_calc(Rule_type r)            = 0;
    virtual Terminal_type          lexem2terminal(const Lexem_type& l) = 0;
    virtual Parser_action_info     error_hadling(size_t s)             = 0;

private:
    void shift(size_t shifted_state, Lexem_type e);
    void reduce_without_back(Rule_type r);
    void reduce(Rule_type r);

    Rule_info<Non_terminal_type>*       rules;
    GOTO_entry**                        goto_table;
    Parser_action_table<Terminal_type>* action_table;

    size_t next_state(size_t s, Non_terminal_type n);
};

template<typename R_traits, typename Lex_traits, typename S, typename Container>
size_t SLR_parser<R_traits, Lex_traits, S, Container>::
    next_state(size_t s, Non_terminal_type n)
{
    size_t cs;
    GOTO_entry  current_entry;
    GOTO_entry* goto_for_n = goto_table[static_cast<size_t>(n)];
    while((cs = (current_entry = *goto_for_n++).from) != ANY){
        if(cs == s){
            return current_entry.to;
        }
    }
    goto_for_n--;
    return goto_for_n -> to;
}

template<typename R_traits, typename Lex_traits, typename S, typename Container>
void SLR_parser<R_traits, Lex_traits, S, Container>::
    shift(size_t shifted_state, Lexem_type e)
{
    SE selem;
    selem.st_num  = shifted_state;
    selem.attr.li = e;
    parser_stack.push(selem);
    checker(e);
}

template<typename R_traits, typename Lex_traits, typename S, typename Container>
void SLR_parser<R_traits, Lex_traits, S, Container>::
    reduce_without_back(Rule_type r)
{
    auto   r_info   = rules[static_cast<size_t>(r)];
    size_t rule_len = r_info.len;
    parser_stack.get_elems_from_top(rule_body, rule_len);
    generate_command(r);

    using SE = Stack_elem<Lexem_type>;

    SE se;
    se.attr         = attrib_calc(r);
    parser_stack.multi_pop(rule_len);
    SE top_elem     = parser_stack.top();
    se.st_num       = next_state(top_elem.st_num, r_info.nt);
    parser_stack.push(se);
}

template<typename R_traits, typename Lex_traits, typename S, typename Container>
void SLR_parser<R_traits, Lex_traits, S, Container>::reduce(Rule_type r)
{
    reduce_without_back(r);
    scaner->back();
}

template<typename R_traits, typename Lex_traits, typename S, typename Container>
void SLR_parser<R_traits, Lex_traits, S, Container>::slr_parsing(Container& buf)
{
    buf_ = buf;

    parser_stack.clear();

    SE initial_elem;
    initial_elem.st_num                   = 0;
    initial_elem.attr.indeces.begin_index = 0;
    initial_elem.attr.indeces.end_index   = 0;
    parser_stack.push(initial_elem);

    for( ; ; ){
        li = scaner->current_lexem();
        t = lexem2terminal(li);
        current_state = parser_stack.top().st_num;
        auto it = action_table->find({current_state, t});
        Parser_action_info pai;
        if(it != action_table->end()){
            pai = it->second;
        }else{
            pai = error_hadling(current_state);
        }
        switch(static_cast<Parser_action_name>(pai.kind)){
            case Parser_action_name::Reduce:
                reduce(static_cast<Rule_type>(pai.arg));
                break;
            case Parser_action_name::Shift:
                shift(pai.arg, li);
                break;
            case Parser_action_name::Reduce_without_back:
                reduce_without_back(static_cast<Rule_type>(pai.arg));
                break;
            case Parser_action_name::OK:
                buf = buf_;
                scaner->back();
                return;
        }
    }
}
#endif