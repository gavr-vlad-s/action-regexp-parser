/*
    File:    act_expr_parser.cpp
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <map>
#include <cstdio>
#include <cstdlib>
#include "../include/act_expr_parser.h"
#include "../include/belongs.h"
#include "../include/expr_traits.h"

// enum class Expr_lexem_code : uint16_t {
//     Nothing,             UnknownLexem,       Action,
//     Opened_round_brack,  Closed_round_brack, Or,
//     Kleene_closure,      Positive_closure,   Optional_member,
//     Character,           Begin_expression,   End_expression,
//     Class_complement,    Character_class
// };
// enum class Terminal{
//     End_of_text, Term_a,  Term_b,
//     Term_c,      Term_d,  Term_p,
//     Term_q,      Term_LP, Term_RP
// };

/* Grammar rules:
 *
 * -------------------------------------------
 * | Rule number | Rule       | Rule name    |
 * |------------------------------------------
 * | (0)         | S -> pTq   | S_is_pTq     |
 * | (1)         | T -> TbE   | T_is_TbE     |
 * | (2)         | T -> E     | T_is_E       |
 * | (3)         | E -> EF    | E_is_EF      |
 * | (4)         | E -> F     | E_is_F       |
 * | (5)         | F -> Gc    | F_is_Gc      |
 * | (6)         | F -> G     | F_is_G       |
 * | (7)         | G -> Ha    | G_is_Ha      |
 * | (8)         | G -> H     | G_is_H       |
 * | (9)         | H -> d     | H_is_d       |
 * | (10)        | H -> (T)   | H_is_LP_T_RP |
 * ---------------------------------------------
 *
 * In this grammar, a means $action_name, b means the operator |, c means unary
 * operators ? * +, d means a character or a character class, p means { (opening
 * curly bracket), q means } (closing curly bracket).
 */

Terminal Act_expr_parser::lexem2terminal(const Expr_lexem_info& l)
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

        default:
            return Terminal::Term_c;
    }
}

Act_expr_parser::Attrib_calculator Act_expr_parser::attrib_calculator[] = {
    &Act_expr_parser::attrib_by_S_is_pTq,
    &Act_expr_parser::attrib_by_T_is_TbE,
    &Act_expr_parser::attrib_by_T_is_E,
    &Act_expr_parser::attrib_by_E_is_EF,
    &Act_expr_parser::attrib_by_E_is_F,
    &Act_expr_parser::attrib_by_F_is_Gc,
    &Act_expr_parser::attrib_by_F_is_G,
    &Act_expr_parser::attrib_by_G_is_Ha,
    &Act_expr_parser::attrib_by_G_is_H,
    &Act_expr_parser::attrib_by_H_is_d,
    &Act_expr_parser::attrib_by_H_is_LP_T_RP
};

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_S_is_pTq()
{
    return rule_body[1].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_T_is_TbE()
{
    Attributes<Expr_lexem_info> s = rule_body[0].attr;
    s.indeces.end_index = buf_.size() - 1;
    return s;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_T_is_E()
{
    return rule_body[0].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_E_is_EF()
{
    Attributes<Expr_lexem_info> s = rule_body[0].attr;
    s.indeces.end_index = buf_.size() - 1;
    return s;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_E_is_F()
{
    return rule_body[0].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_F_is_Gc()
{
    Attributes<Expr_lexem_info> s = rule_body[0].attr;
    s.indeces.end_index = buf_.size() - 1;
    return s;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_F_is_G()
{
    return rule_body[0].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_G_is_Ha()
{
    return rule_body[0].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_G_is_H()
{
    return rule_body[0].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_H_is_d()
{
    Attributes<Expr_lexem_info> s;
    s.indeces.begin_index = s.indeces.end_index = buf_.size() - 1;
    return s;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_by_H_is_LP_T_RP()
{
    return rule_body[1].attr;
}

Attributes<Expr_lexem_info> Act_expr_parser::attrib_calc(Rule r)
{
    return (this->*attrib_calculator[r])();
}


Act_expr_parser::Error_handler Act_expr_parser::error_hadler[] = {
    &Act_expr_parser::state00_error_handler, // 0
    &Act_expr_parser::state01_error_handler, // 1
    &Act_expr_parser::state02_error_handler, // 2
    &Act_expr_parser::state03_error_handler, // 3
    &Act_expr_parser::state04_error_handler, // 4
    &Act_expr_parser::state04_error_handler, // 5
    &Act_expr_parser::state06_error_handler, // 6
    &Act_expr_parser::state07_error_handler, // 7
    &Act_expr_parser::state07_error_handler, // 8
    &Act_expr_parser::state02_error_handler, // 9
    &Act_expr_parser::state02_error_handler, // 10
    &Act_expr_parser::state11_error_handler, // 11
    &Act_expr_parser::state04_error_handler, // 12
    &Act_expr_parser::state04_error_handler, // 13
    &Act_expr_parser::state06_error_handler, // 14
    &Act_expr_parser::state15_error_handler, // 15
    &Act_expr_parser::state04_error_handler, // 16
    &Act_expr_parser::state07_error_handler  // 17
};

/* In this array, the rules are collected for which reduce is performed in
 * error-handling functions. The number of the array element is the number of the
 * current state of the parser. If a state in the corresponding error-handling
 * function is not reduced, then the element of this array with the corresponding
 * index is (-1). */
char reduce_rules[] = {
    -1,       -1,          -1,      -1,
    T_is_E,   E_is_F,      F_is_G,  G_is_H,
    H_is_d,   -1,          -1,      S_is_pTq,
    E_is_EF,  F_is_Gc,     G_is_Ha, -1,
    T_is_TbE, H_is_LP_T_RP
};

static const char* expected_opening_curly_brace =
    "An opening curly brace is expected at line %zu.\n";

static const char* expected_char_or_char_class =
    "A character, a character class, a character class complement, or an "
    "opening parenthesis is expected at line %zu.\n";

static const char* expected_or_operator =
    "An operator | or closing brace is expected at line %zu.\n";

static const char* unexpected_action = "Unexpected action at line %zu.\n";

static const char* unexpected_postfix_operator =
    "Unexpected postfix operator at line %zu.\n";

static const char* unexpected_end_of_text = "Unexpected end of text at line %zu.\n";

static const char* unexpected_opening_brace = "Unexpected opening brace at line %zu.\n";

static const char* expected_or_operator_or_parenthesis =
    "An operator | or closing parenthesis is expected at line %zu.\n";

static const char* character_class_is_not_allowed =
    "Error at line %zu: in the regular expression for numbers, a "
    "character class complement is not allowed.\n";

Parser_action_info Act_expr_parser::state00_error_handler()
{
    printf(expected_opening_curly_brace, scaner->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
    if(li.code != Expr_lexem_code::Closed_round_brack){
        scaner->back();
    }
    li.code = Expr_lexem_code::Begin_expression;
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::Shift); pa.arg = 2;
    return pa;
}

Parser_action_info Act_expr_parser::state01_error_handler()
{
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::OK); pa.arg = 0;
    return pa;
}

Parser_action_info Act_expr_parser::state02_error_handler()
{
    printf(expected_char_or_char_class, scaner->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
    scaner->back();
    li.code = Expr_lexem_code::Character;
    li.c    = 'a';
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::Shift); pa.arg = 8;
    return pa;
}

Parser_action_info Act_expr_parser::state03_error_handler()
{
    printf(expected_or_operator, scaner->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
    if(t != Terminal::Term_p){
        scaner->back();
    }
    li.code = Expr_lexem_code::Or;
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::Shift); pa.arg = 10;
    return pa;
}

Parser_action_info Act_expr_parser::state04_error_handler()
{
    Rule r = static_cast<Rule>(reduce_rules[current_state]);
    Parser_action_info pa;
    switch(t){
        case Terminal::Term_a:
            printf(unexpected_action, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce); pa.arg = r;
            break;

        case Terminal::Term_c:
            printf(unexpected_postfix_operator, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce); pa.arg = r;
            break;

        case Terminal::End_of_text:
            printf(unexpected_end_of_text, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce); pa.arg = r;
            break;

        case Terminal::Term_p:
            printf(unexpected_opening_brace, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
            break;

        default:
            ;
    }
    return pa;
}

Parser_action_info Act_expr_parser::state06_error_handler()
{
    Rule r = static_cast<Rule>(reduce_rules[current_state]);
    Parser_action_info pa;
    switch(t){
        case Terminal::Term_a:
            printf(unexpected_action, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
            break;

        case Terminal::Term_p:
            printf(unexpected_opening_brace, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
            break;

        case Terminal::End_of_text:
            printf(unexpected_end_of_text, scaner->lexem_begin_line_number());
            pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
            break;

        default:
            ;
    }
    return pa;
}

Parser_action_info Act_expr_parser::state07_error_handler()
{
    Rule r = static_cast<Rule>(reduce_rules[current_state]);
    Parser_action_info pa;
    if(Terminal::Term_p == t){
        printf(unexpected_opening_brace, scaner->lexem_begin_line_number());
        pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
    }else{
        printf(unexpected_end_of_text, scaner->lexem_begin_line_number());
        pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce_without_back); pa.arg = r;
    }
    et_.ec->increment_number_of_errors();
    return pa;
}

Parser_action_info Act_expr_parser::state11_error_handler()
{
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::Reduce); pa.arg = S_is_pTq;
    return pa;
}

Parser_action_info Act_expr_parser::state15_error_handler()
{
    printf(expected_or_operator_or_parenthesis, scaner->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
    if(t != Terminal::Term_p){
        scaner->back();
    }
    li.code = Expr_lexem_code::Or;
    Parser_action_info pa;
    pa.kind = static_cast<uint16_t>(Parser_action_name::Shift); pa.arg = 10;
    return pa;
}

Parser_action_info Act_expr_parser::error_hadling(size_t s)
{
    return (this->*error_hadler[s])();
}

void Act_expr_parser::generate_command(Rule r){
    Command            com;
    Id_scope::iterator it;
    size_t             act_index;
    size_t             min_index;
    size_t             max_index;
    switch(r){
        case T_is_TbE:
            com.name        = Command_name::Or;
            com.args.first  = rule_body[0].attr.indeces.end_index;
            com.args.second = rule_body[2].attr.indeces.end_index;
            com.action_name = 0;
            buf_.push_back(com);
            break;

        case E_is_EF:
            com.name        = Command_name::Concat;
            com.args.first  = rule_body[0].attr.indeces.end_index;
            com.args.second = rule_body[1].attr.indeces.end_index;
            com.action_name = 0;
            buf_.push_back(com);
            break;

        case F_is_Gc:
            switch(rule_body[1].attr.li.code){
                case Expr_lexem_code::Kleene_closure:
                    com.name = Command_name::Kleene;
                    break;
                case Expr_lexem_code::Positive_closure:
                    com.name = Command_name::Positive;
                    break;
                case Expr_lexem_code::Optional_member:
                    com.name = Command_name::Optional;
                    break;
                default:
                    ;
            }
//             com.name =
//                 static_cast<Command_name>(rule_body[1].attr.li.code -
//                                           Kleene_closure + Cmd_Kleene);
            com.args.first  = rule_body[0].attr.indeces.end_index;
            com.args.second = 0;
            com.action_name = 0;
            buf_.push_back(com);
            break;

        case H_is_d:
            switch(rule_body[0].attr.li.code){
                case Expr_lexem_code::Class_complement:
                    com.name        = Command_name::Char_class_complement;
                    com.idx_of_set  = rule_body[0].attr.li.set_of_char_index;
                    break;
                case Expr_lexem_code::Character_class:
                    com.name        = Command_name::Char_class;
                    com.idx_of_set  = rule_body[0].attr.li.set_of_char_index;
                    break;
                case Expr_lexem_code::Character:
                    com.name        = Command_name::Char;
                    com.c           = rule_body[0].attr.li.c;
                    break;
                default:
                    ;
            }
//             if(Expr_lexem_code::Character == rule_body[0].attr.li.code){
//                 com.name        = Cmd_char_def;
//                 com.c           = rule_body[0].attr.eli.c;
//             }else{
//                 com.name = Cmd_char_class_def;
//                 com.cls  = static_cast<Char_class>(
//                     rule_body[0].attr.li.code - Class_Latin);
//             }
            com.action_name = 0;
            buf_.push_back(com);
            break;

        case G_is_Ha:
            /* If the action a is not yet defined, then we display an error message and
             * assume that no action is specified. Otherwise, write down the index of
             * the action name. */
            act_index = rule_body[1].attr.li.action_name_index;
            it        = scope_->idsc.find(act_index);
            if(it == scope_->idsc.end()){
                printf("The action ");
                et_.ids_trie->print(act_index);
                printf(" is not defined at line %zu.\n",
                       scaner->lexem_begin_line_number());
                et_.ec -> increment_number_of_errors();
                return;
            } else if(it->second.kind != Action_name){
                printf("The identifier ");
                et_.ids_trie->print(act_index);
                printf(" is not action name at line %zu.\n",
                       scaner->lexem_begin_line_number());
                et_.ec -> increment_number_of_errors();
                return;
            };
            min_index = rule_body[0].attr.indeces.begin_index;
            max_index = rule_body[0].attr.indeces.end_index + 1;
            for(size_t i = min_index; i < max_index; i++){
                buf_[i].action_name = act_index;
            }
            break;

        default:
            ;
    }
}

void Num_regexp_parser::checker(Expr_lexem_info e){
    if(Expr_lexem_code::Class_complement == e.code){
        printf(character_class_is_not_allowed, scaner->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
    }
}

void Str_regexp_parser::checker(Expr_lexem_info e){
}