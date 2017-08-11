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
#include "../include/command.h"
#include "../include/expr_scaner.h"
#include "../include/expr_lexem_info.h"
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/slr_parser.h"
#include "../include/expr_traits.h"
#include "../include/expr_slr_tables.h"

using Concrete_SLR_parser =
    SLR_parser<Expr_r_traits, Expr_lex_traits, Expr_scaner, Command_buffer>;

class Act_expr_parser : public Concrete_SLR_parser
{
public:
    Act_expr_parser()                            = default;
    virtual ~Act_expr_parser()                   = default;
    Act_expr_parser(const Act_expr_parser& orig) = default;

    Act_expr_parser(const Expr_scaner_ptr&         esc,
                    const Errors_and_tries&        et,
                    const std::shared_ptr<Scope>&  scope,
                    const SLR_parser_tables<Expr_lex_traits>& tables) :
       Concrete_SLR_parser(esc, tables), scope_(scope),  et_(et) {};

    void compile(Command_buffer& buf);

private:
    std::shared_ptr<Scope>       scope_;

    using Error_handler = Parser_action_info (Act_expr_parser::*)();
    static Error_handler error_hadler[];

    using Attrib_calculator = Attributes<Lexem_type> (Act_expr_parser::*)();
    static Attrib_calculator attrib_calculator[];

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

protected:
    virtual void                   checker(Lexem_type l) = 0;
    virtual void                   generate_command(Rule_type r);
    virtual Attributes<Lexem_type> attrib_calc(Rule_type r);
    virtual Terminal_type          lexem2terminal(const Lexem_type& l);
    virtual Parser_action_info     error_hadling(size_t s);

    Errors_and_tries             et_;
};

class Num_regexp_parser : public Act_expr_parser{
public:
    Num_regexp_parser()                              = default;
    virtual ~Num_regexp_parser()                     = default;
    Num_regexp_parser(const Num_regexp_parser& orig) = default;
    Num_regexp_parser(const Expr_scaner_ptr&         esc,
                      const Errors_and_tries&        et,
                      const std::shared_ptr<Scope>&  scope,
                      const SLR_parser_tables<Expr_lex_traits>& tables) :
        Act_expr_parser(esc, et, scope, tables) {}

protected:
    virtual void checker(Expr_lexem_info l);
};

class Str_regexp_parser : public Act_expr_parser{
public:
    Str_regexp_parser()                              = default;
    virtual ~Str_regexp_parser()                     = default;
    Str_regexp_parser(const Str_regexp_parser& orig) = default;
    Str_regexp_parser(const Expr_scaner_ptr&         esc,
                      const Errors_and_tries&        et,
                      const std::shared_ptr<Scope>&  scope,
                      const SLR_parser_tables<Expr_lex_traits>& tables) :
        Act_expr_parser(esc, et, scope, tables) {}

protected:
    virtual void checker(Expr_lexem_info l);
};
#endif