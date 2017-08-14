/*
    File:    act_expr_parser_rd.cpp
    Created: 14 August 2017 at 15:55 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include "../include/act_expr_parser_rd.h"

/*
 * Grammar rules for regexps with embedded actions can be written in the form
 *
 * S -> pTq
 * T -> E(bE)*
 * E -> (Ha?c?)+
 * H -> d | lTr
 *
 * Here the following notation is used:
 *
 *      a   $action_name
 *      b   operator |
 *      c   unary operators ? * +
 *      d   character, character class, or character class complement
 *      p   { (opening curly bracket)
 *      q   } (closing curly bracket)
 *      l   (
 *      r   )
 *
 * The compilation of the regexp will be performed by a recursive descent parser.
 */

enum class Terminal{
    End_of_text, Term_a,  Term_b,
    Term_c,      Term_d,  Term_p,
    Term_q,      Term_LP, Term_RP
};

static Terminal elexem2rerminal(const Expr_lexem_info& l)
{
    switch(l.code){
        case Expr_lexem_code::Nothing: case Expr_lexem_code::UnknownLexem:
            return Terminal::End_of_text;

        case Expr_lexem_code::Action:
            return Terminal::Term_a;

        case Expr_lexem_code::Or:
            return Terminal::Term_b;

        case Expr_lexem_code::Class_complement: case Expr_lexem_code::Character_class:
        case Expr_lexem_code::Character:
            return Terminal::Term_d;

        case Expr_lexem_code::Begin_expression:
            return Terminal::Term_p;

        case Expr_lexem_code::End_expression:
            return Terminal::Term_q;

        case Expr_lexem_code::Opened_round_brack:
            return Terminal::Term_LP;

        case Expr_lexem_code::Closed_round_brack:
            return Terminal::Term_RP;

        case Expr_lexem_code::Kleene_closure ... Expr_lexem_code::Optional_member:
            return Terminal::Term_c;
    }
}

static const char* expected_opening_curly_brace =
    "An opening curly brace is expected at line %zu.\n";
static const char* expected_closing_curly_brace =
    "An closing curly brace is expected at line %zu.\n";
static const char* unexpected_end_of_text =
    "Unexpected end of text at line %zu.\n";

void Act_expr_parser_rd::S_proc()
{
    enum class State{
        Begin, Body, End_body, End
    };
    State    state = State::Begin;
    Terminal t
    while((t = elexem2rerminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case State::Begin
                if(Terminal::Term_p == t){
                    state = State::Body;
                }else{
                    printf(expected_opening_curly_brace,
                           esc_->lexem_begin_line_number());
                }
                break;

            case State::Body
                esc_->back();
                T_proc();
                state = State::End_body;
                break;

            case State::End_body
                if(Terminal::Term_q == t){
                    state = State::End;
                }else{
                    printf(expected_closing_curly_brace,
                           esc_->lexem_begin_line_number());
                }
                break;

            case State::End:
                esc_->back();
                return;
        }
    }

    if(state != State::End){
        printf(unexpected_end_of_text, esc_->lexem_begin_line_number());
        esc_->back();
    }
}

void Act_expr_parser_rd::T_proc()
{}

void Act_expr_parser_rd::E_proc()
{}

/*
 * The function H_proc() implements a finite automaton with the following
 * transition table:
 *
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * |    state    |    d     |     l     |      T      |     r     |    Remark    |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * | Begin       | Leaf_arg | L_bracket |             |           | Start state. |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * | Leaf_arg    |          |           |             |           | Final state. |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * | L_bracket   |          |           | Br_contents |           |              |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * | Br_contents |          |           |             | R_bracket |              |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 * | R_bracket   |          |           |             |           | Final state. |
 * |-------------|----------|-----------|-------------|-----------|--------------|
 */
void Act_expr_parser_rd::H_proc()
{
    enum class State{
        Begin,       Leaf_arg,     L_bracket,
        Br_contents, Right_bracket
    };
    State    state = State::Begin;
    Terminal t
    while((t = elexem2rerminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case State::Begin:
                switch(t){
                    case Terminal::Term_LP:
                        break;
                    case Terminal::Term_d:
                        break;
                    default:
                        ;
                }
                break;
            case State::Leaf_arg:
                break;
            case State::L_bracket:
                break;
            case State::Br_contents:
                break;
            case State::R_bracket:
                break;
        }
    }

    if((state != State::Leaf_arg) && (state != State::R_bracket)){
        printf(unexpected_end_of_text, esc_->lexem_begin_line_number());
        esc_->back();
    }
}

void Act_expr_parser_rd::compile(Command_buffer& buf)
{
    buf_ = buf;
    S_proc(buf_);
    buf  = buf_;
}