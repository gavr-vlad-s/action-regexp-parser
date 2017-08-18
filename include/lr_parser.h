/*
    File:    lr_parser.h
    Created: 18 August 2017 at 17:42 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef LR_PARSER_H
#define LR_PARSER_H

#include <memory>
#include <map>
#include <utility>
#include <cstddef>
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

template<Integral T, Integral NT, typename R, size_t N>
struct Grammar_traits{
    using Terminal_t                 = T;
    using Non_terminal_t             = NT;
    using Rule_t                     = R;
    static const size_t rule_max_len = N;
};

template<typename S, typename Lexem_type>
struct Scaner_traits{
    using Lexem_t                    = Lexem_type;
    using Scaner_t                   = S;
};

template<typename G_tr, typename S_tr, typename Container>
class LR_parser{
public:
    using Lexem_type        = typename S_tr::Lexem_t;
    using Non_terminal_type = typename G_tr::Non_terminal_t;
    using Terminal_type     = typename G_tr::Terminal_t;
    using Scaner_type       = typename S_tr::Scaner_t;

    LR_parser<G, S, Container>()                                       = default;
    LR_parser<G, S, Container>(const LR_parser<G, S, Container>& orig) = default;

    LR_parser<G, S, Container>(const std::shared_ptr<Scaner_type>& scaner_,
                               const LR_parser_tables<G_tr>&       tables) : scaner(scaner_)
    {
        rules        = tables.rules;
        goto_table   = tables.goto_table;
        action_table = tables.action_table;
    }

    virtual ~LR_parser<G, S, Container>()                              = default;

    void compile(Container& buf);

protected:
    using Rule_type         = typename G_tr::Rule_t;
    using SE                = Stack_elem<Lexem_type>;

    size_t                              current_state;
    Lexem_type                          li;
    Terminal_type                       t;
    Multipop_stack<SE>                  parser_stack;
    std::shared_ptr<Scaner_type>        scaner;
    SE                                  rule_body[R_traits::max_len];
    Container                           buf_;

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
#endif