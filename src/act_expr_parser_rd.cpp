/*
    File:    act_expr_parser_rd.cpp
    Created: 14 August 2017 at 15:55 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include <tuple>
#include <utility>
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

static Terminal elexem2terminal(const Expr_lexem_info& l)
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
static const char* expected_char_or_char_class =
    "A character, a character class, a character class complement, or an "
    "opening round bracket is expected at line %zu.\n";
static const char* expected_closing_round_bracket =
    "An closing round bracket is expected at line %zu.\n";;

void Act_expr_parser_rd::S_proc()
{
    enum class State{
        Begin, Body, End_body, End
    };
    State    state = State::Begin;
    Terminal t;
    while((t = elexem2terminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case State::Begin
                if(Terminal::Term_p == t){
                    state = State::Body;
                }else{
                    printf(expected_opening_curly_brace,
                           esc_->lexem_begin_line_number());
                    et_.ec -> increment_number_of_errors();
                }
                break;
            case State::Body
                esc_->back();
                T_proc();
                state = State::End_body;
                break;
            case State::End_body
                state = State::End;
                if(t != Terminal::Term_q){
                    printf(expected_closing_curly_brace,
                           esc_->lexem_begin_line_number());
                    et_.ec -> increment_number_of_errors();
                    esc_->back();
                }
                break;
            case State::End:
                esc_->back();
                return;
        }
    }
    if(state != State::End){
        printf(unexpected_end_of_text, esc_->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        esc_->back();
    }
}

/*
 * The function T_proc() implements a finite automaton with the following
 * transition table:
 *
 * |-------------|-------|-----|--------------|
 * |    state    |   b   |  E  |    Remark    |
 * |-------------|-------|-----|--------------|
 * | Begin       |       | End | Start state. |
 * |-------------|-------|-----|--------------|
 * | End         | Begin |     | Final state. |
 * |-------------|-------|-----|--------------|
 */
Act_expr_parser_rd::Args_info Act_expr_parser_rd::write_command(const Args_info& a)
{
    Args_info info = a;
    Command   com;
    info.num_of_ors++;
    if(1 == info.num_of_ors){
        info.arg1       = buf_.size() - 1;
    }else{
        info.arg2       = buf_.size() - 1;
        com.args.first  = info.arg1;
        com.args.second = info.arg2;
        com.name        = (info.op == Operation::Or_op) ?
                          Command_name::Or              :
                          Command_name::Concat;
        com.action_name = 0;
        buf.push_back(command);
        info.arg1 = buf.size() - 1;
    }
    return info;
}

void Act_expr_parser_rd::T_proc()
{
    enum class State{
        Begin, End
    };
    State     state = State::Begin;
    Terminal  t;
    Or_args_info a
    a.num_of_ors    = 0;
    a.arg1          = 0;
    a.arg2          = 0;
    a.op            = Operation::Or_op;
    while((t = elexem2terminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case State::Begin:
                esc_->back();
                E_proc();
                a = write_command(a);
                state = State::End;
                break;
            case State::End:
                switch(eli.code){
                    case Expr_lexem_code::Or:
                        state = State::Begin;
                        a.num_of_ors++;
                        break;
                    default:
                        esc_->back();
                        return;
                }
                break;
        }
    }
    if(state != State::End){
        printf(unexpected_end_of_text, esc_->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        esc_->back();
    }
}

/*
 * The function E_proc() implements a finite automaton with the following
 * transition table:
 *
 * |---------|----------|-----------|-------------|--------------|
 * |  state  |    a     |     c     |      H      |    Remark    |
 * |---------|----------|-----------|-------------|--------------|
 * | Begin   |          |           | H_st        | Start state. |
 * |---------|----------|-----------|-------------|--------------|
 * | H_st    | Act      | Unary     | H_st        | Final state. |
 * |---------|----------|-----------|-------------|--------------|
 * | Act     |          | Unary     | H_st        | Final state. |
 * |---------|----------|-----------|-------------|--------------|
 * | Unary   |          |           | H_st        | Final state. |
 * |---------|----------|-----------|-------------|--------------|
 */
void Act_expr_parser_rd::E_proc()
{
    enum class State{
        Begin, H_st, Act, Unary
    };
    State     state = State::Begin;
    Terminal  t;
    Or_args_info a
    a.num_of_ors    = 0;
    a.arg1          = 0;
    a.arg2          = 0;
    a.op            = Operation::Concat_op;
    while((t = elexem2terminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case Begin:
                esc_->back();
                H_proc();
                a = write_command(a);
                break;
            case H_st:
                break;
            case Act:
                break;
            case Unary:
                break;
//             case State::Begin:
//                 esc_->back();
//                 E_proc();
//                 a = write_command(a);
//                 state = State::End;
//                 break;
//             case State::End:
//                 switch(eli.code){
//                     case Expr_lexem_code::Or:
//                         state = State::Begin;
//                         a.num_of_ors++;
//                         break;
//                     default:
//                         esc_->back();
//                         return;
//                 }
//                 break;
        }
    }
    if(state == State::Begin){
        printf(unexpected_end_of_text, esc_->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        esc_->back();
    }
}

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
Act_expr_parser_rd::State_H Act_expr_parser_rd::H_proc_begin(const Expr_lexem_info& l)
{
    Command  com;
    State_H state;
    switch(l.code){
        case Expr_lexem_code::Opened_round_brack:
            state           = State::L_bracket;
            break;
        case Expr_lexem_code::Character:
            com.name        = Command_name::Char;
            com.c           = eli.c;
            state           = State::Leaf_arg;
            buf_.push_back(com);
            break;
        case Expr_lexem_code::Character_class:
            com.name        = Command_name::Char_class;
            com.idx_of_set  = eli.set_of_char_index;
            state           = State::Leaf_arg;
            buf_.push_back(com);
            break;
        case Expr_lexem_code::Class_complement:
            com.name        = Command_name::Char_class_complement;
            com.idx_of_set  = eli.set_of_char_index;
            state           = State::Leaf_arg;
            buf_.push_back(com);
            break;
        default:
            printf(expected_char_or_char_class, esc_->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            esc_->back();
            state           = State::Begin;
    }
    return state;
}

void Act_expr_parser_rd::H_proc()
{
    State    state = State::Begin;
    Terminal t;
    while((t = elexem2terminal(eli = esc_->current_lexem())) != Terminal::End_of_text){
        switch(state){
            case State::Begin:
                state = H_proc_begin(eli);
                break;

            case State::Leaf_arg:
                esc_->back();
                return;

            case State::L_bracket:
                esc_->back();
                T_proc();
                state = State::Br_contents;
                break;

            case State::Br_contents:
                state = State::R_bracket;
                if(t != Terminal::Term_RP){
                    printf(expected_closing_round_bracket,
                           esc_->lexem_begin_line_number());
                    et_.ec -> increment_number_of_errors();
                    esc_->back();
                }
                break;

            case State::R_bracket:
                esc_->back();
                return;
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