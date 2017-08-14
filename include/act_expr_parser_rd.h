/*
    File:    act_expr_parser_rd.h
    Created: 14 August 2017 at 15:41 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef ACT_EXPR_PARSER_RD_H
#define ACT_EXPR_PARSER_RD_H

#include <memory>
#include "../include/command.h"
#include "../include/expr_scaner.h"
#include "../include/expr_lexem_info.h"
#include "../include/scope.h"
#include "../include/errors_and_tries.h"

class Act_expr_parser_rd{
public:
    Act_expr_parser_rd()                               = default;
    virtual ~Act_expr_parser_rd()                      = default;
    Act_expr_parser_rd(const Act_expr_parser_rd& orig) = default;
    Act_expr_parser_rd(Expr_scaner_ptr&        esc,
                       const Errors_and_tries& et,
                       std::shared_ptr<Scope>& scope) :
       esc_(esc), scope_(scope),  et_(et) {};

    void compile(Command_buffer& buf);

protected:
    virtual void checker(const Expr_lexem_info& e) = 0;
    Errors_and_tries             et_;

private:
    Command_buffer               buf_;

    std::shared_ptr<Expr_scaner> esc_;
    std::shared_ptr<Scope>       scope_;

    Expr_lexem_info              eli;
    Expr_lexem_code              elc;

    void S_proc();
    void T_proc();
    void E_proc();
    void H_proc();
};

class Num_act_expr_parser_rd : public Act_expr_parser_rd{
public:
    Num_act_expr_parser_rd()                                   = default;
    virtual ~Num_act_expr_parser_rd()                          = default;
    Num_act_expr_parser_rd(const Num_act_expr_parser_rd& orig) = default;
    Num_act_expr_parser_rd(Expr_scaner_ptr&        esc,
                           const Errors_and_tries& et,
                           std::shared_ptr<Scope>& scope) :
        Act_expr_parser_rd(esc, et, scope) {};

protected:
    virtual void checker(const Expr_lexem_info& e);
};

class Str_act_expr_parser_rd : public Act_expr_parser_rd{
public:
    Str_act_expr_parser_rd()                                   = default;
    virtual ~Str_act_expr_parser_rd()                          = default;
    Str_act_expr_parser_rd(const Str_act_expr_parser_rd& orig) = default;
    Str_act_expr_parser_rd(Expr_scaner_ptr&        esc,
                           const Errors_and_tries& et,
                           std::shared_ptr<Scope>& scope) :
        Act_expr_parser_rd(esc, et, scope) {};

protected:
    virtual void checker(const Expr_lexem_info& e);
};
#endif