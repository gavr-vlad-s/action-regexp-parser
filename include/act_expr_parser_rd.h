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

private:
    Command_buffer               buf_;

    std::shared_ptr<Expr_scaner> esc_;
    std::shared_ptr<Scope>       scope_;
    Errors_and_tries             et_;

    Expr_lexem_info              eli;
    Expr_lexem_code              elc;

    void S_proc();
    void T_proc();
    void E_proc();
    void H_proc();

    enum class State_H{
        Begin,       Leaf_arg,     L_bracket,
        Br_contents, Right_bracket
    };

    State_H H_proc_begin(const Expr_lexem_info& e);
};
#endif