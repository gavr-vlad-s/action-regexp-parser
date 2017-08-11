/*
    File:    expr_traits.h
    Created: 09 August 2017 at 14:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef EXPR_TRAITS_H
#define EXPR_TRAITS_H
#include "../include/slr_parser.h"
#include "../include/expr_lexem_info.h"

enum Rule : uint8_t{
    S_is_pTq, T_is_TbE,     T_is_E,
    E_is_EF,  E_is_F,       F_is_Gc,
    F_is_G,   G_is_Ha,      G_is_H,
    H_is_d,   H_is_LP_T_RP
};

enum class Non_terminal : uint8_t{
    Nt_S, Nt_T, Nt_E,
    Nt_F, Nt_G, Nt_H
};

enum class Terminal{
    End_of_text, Term_a,  Term_b,
    Term_c,      Term_d,  Term_p,
    Term_q,      Term_LP, Term_RP
};

using Expr_r_traits   = Rule_traits<Rule, 3>;

using Expr_lex_traits = Lexem_traits<Expr_lexem_info, Terminal, Non_terminal>;
#endif