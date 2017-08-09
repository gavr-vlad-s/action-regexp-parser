/*
    File:    act_expr_parser.h
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ACT_EXPR_PARSER_H
#define ACT_EXPR_PARSER_H

#include <memory>
// #include "../include/multipop_stack.h"
#include "../include/command.h"
#include "../include/expr_scaner.h"
#include "../include/expr_lexem_info.h"
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
// #include "../include/stack_elem.h"
#include "../include/slr_parser.h"
#include "../include/expr_traits.h"
#include "../include/expr_slr_tables.h"

using Concrete_SLR_parser =
    SLR_parser<Expr_r_traits, Expr_lex_traits, Expr_scaner, Command_buffer>;

enum class Number_or_string {
    Number_expr, String_expr
};

class Act_expr_parser : public Concrete_SLR_parser
{
public:
    Act_expr_parser()                            = default;
    ~Act_expr_parser()                           = default;
    Act_expr_parser(const Act_expr_parser& orig) = default;

    Act_expr_parser(const Expr_scaner_ptr&         esc,
                    const Errors_and_tries&        et,
                    const std::shared_ptr<Scope>&  scope,
                    const SLR_parser_tables<Expr_lex_traits>& tables) :
       Concrete_SLR_parser(esc, tables), scope_(scope),  et_(et) {};

    void compile(Command_buffer& buf, Number_or_string kind_of_expr);

private:
    std::shared_ptr<Scope>       scope_;
    Errors_and_tries             et_;

    using Error_handler = Parser_action_info (Act_expr_parser::*)();
    static Error_handler error_hadler[];

    using Attrib_calculator = Attributes<Lexem_type> (Act_expr_parser::*)();
    static Attrib_calculator attrib_calc[];

    Attributes<Lexem_type> attrib_by_S_is_pTq();
    Attributes<Lexem_type> attrib_by_T_is_TbE();
    Attributes<Lexem_type> attrib_by_T_is_E();
    Attributes<Lexem_type> attrib_by_E_is_EF();
    Attributes<Lexem_type> attrib_by_E_is_F();
    Attributes<Lexem_type> attrib_by_F_is_Gc();
    Attributes<Lexem_type> attrib_by_F_is_G();
    Attributes<Lexem_type> attrib_by_G_is_Ha();
    Attributes<Lexem_type> attrib_by_G_is_H();
    Attributes<Lexem_type> attrib_by_H_is_d();
    Attributes<Lexem_type> attrib_by_H_is_LP_T_RP();

    Parser_action_info state00_error_handler();
    Parser_action_info state01_error_handler();
    Parser_action_info state02_error_handler();
    Parser_action_info state03_error_handler();
    Parser_action_info state04_error_handler();
    Parser_action_info state06_error_handler();
    Parser_action_info state07_error_handler();
    Parser_action_info state11_error_handler();
    Parser_action_info state15_error_handler();

    using Checker = void (Act_expr_parser::*)(Expr_lexem_info);
    Checker checker;

    void checker_for_number_expr(Expr_lexem_info e);
    void checker_for_string_expr(Expr_lexem_info e);

protected:
    virtual void                   checker(Lexem_type l);
    virtual void                   generate_command(Rule_type r);
    virtual Attributes<Lexem_type> attrib_calc(Rule_type r);
    virtual Terminal_type          lexem2terminal(const Lexem_type& l);
    virtual Parser_action_info     error_hadling(size_t s);
};
// Terminal lexem2terminal(const Expr_lexem_info& l);
//
// enum Parser_action_name{
//     Act_OK, Act_shift, Act_reduce, Act_reduce_without_back
// };
//
// struct Parser_action_info{
//     uint8_t kind;
//     uint8_t arg;
// };
//
// enum Number_or_string {
//     Number_expr, String_expr
// };
//
// #define MAXIMAL_LENGTH_OF_RULE 3
// #define NUMBERS_OF_RULES (H_is_LP_T_RP + 1)
// class Act_expr_parser{
// public:
//     Act_expr_parser()                            = default;
//     ~Act_expr_parser()                           = default;
//     Act_expr_parser(const Act_expr_parser& orig) = default;
//
//     Act_expr_parser(const Expr_scaner_ptr&         esc,
//                     const Errors_and_tries&        et,
//                     const std::shared_ptr<Scope>&  scope) :
//                 parser_stack(Multipop_stack<Stack_elem>()),
//                 esc_(esc), scope_(scope),  et_(et) {};
//
//     void compile(Command_buffer& buf, Number_or_string kind_of_expr);
// private:
//     Command_buffer buf_;
//
//     Multipop_stack<Stack_elem> parser_stack;
//
//     Expr_lexem_info eli_;
//     Terminal        t;
//
//     std::shared_ptr<Expr_scaner> esc_;
//     std::shared_ptr<Scope>       scope_;
//     Errors_and_tries             et_;
//
//     size_t current_state;
//
//     void shift(size_t shifted_state, Expr_lexem_info e);
//     void reduce(Rule r);
//     void reduce_without_back(Rule r);
//
//     typedef Parser_action_info (Act_expr_parser::*Error_handler)();
//     static Error_handler error_hadler[];
//
//     struct Rule_info{
//         Non_terminal nt;  /* target for reducing */
//         uint8_t      len; /* rule length */
//     };
//     static Rule_info rules[];
//
//     Stack_elem rule_body[MAXIMAL_LENGTH_OF_RULE];
//
//     typedef Attributes (Act_expr_parser::*Attrib_calculator)();
//     static Attrib_calculator attrib_calc[];
//
//     Attributes attrib_by_S_is_pTq();
//     Attributes attrib_by_T_is_TbE();
//     Attributes attrib_by_T_is_E();
//     Attributes attrib_by_E_is_EF();
//     Attributes attrib_by_E_is_F();
//     Attributes attrib_by_F_is_Gc();
//     Attributes attrib_by_F_is_G();
//     Attributes attrib_by_G_is_Ha();
//     Attributes attrib_by_G_is_H();
//     Attributes attrib_by_H_is_d();
//     Attributes attrib_by_H_is_LP_T_RP();
//
//     void generate_command(Rule r);
//
//     Parser_action_info state00_error_handler();
//     Parser_action_info state01_error_handler();
//     Parser_action_info state02_error_handler();
//     Parser_action_info state03_error_handler();
//     Parser_action_info state04_error_handler();
//     Parser_action_info state06_error_handler();
//     Parser_action_info state07_error_handler();
//     Parser_action_info state11_error_handler();
//     Parser_action_info state15_error_handler();
//
//     typedef void (Act_expr_parser::*Checker)(Expr_lexem_info);
//     Checker checker;
//
//     void checker_for_number_expr(Expr_lexem_info e);
//     void checker_for_string_expr(Expr_lexem_info e);
// };
#endif