/*
     File:    test-simple-parser.cpp
     Created: 03 August 2017 at 15:33 Moscow time.
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <string>
// #include "../include/fsize.h"
// #include "../include/error_count.h"
#include "../include/location.h"
// #include "../include/trie.h"
// #include "../include/aux_expr_scaner.h"
#include "../include/char_conv.h"
// #include "../include/test_aux_expr_scaner.h"
#include "../include/errors_and_tries.h"
#include "../include/file_contents.h"
#include "../include/char_trie.h"
#include "../include/expr_scaner.h"
// #include "../include/simple_regex_parser.h"
#include "../include/command.h"
#include "../include/print_commands.h"
#include "../include/scope.h"
// #include "../include/expr_slr_tables.h"
#include "../include/act_expr_parser.h"

// #include "../include/test_expr_scaner.h"

/* This function opens a file with test text, and it returns string containing text,
 * if file is successfully opened and the file size is not equal to zero, and the
 * empty string otherwise. */
std::u32string init_testing(const char* name)
{
    auto contents = get_contents(name);
    auto& str      = contents.second;
    switch(contents.first){
        case Get_contents_return_code::Normal:
            if(!str.length()){
                puts("File length is zero.");
                return U"";
            }else{
                return utf8_to_u32string(str.c_str());
            }
            break;

        case Get_contents_return_code::Impossible_open:
            puts("It is impossible to open the file.");
            return U"";

        case Get_contents_return_code::Read_error:
            puts("File reading failed.");
            return U"";
    }
    return U"";
}

int main(int argc, char** argv)
{
    if(1 == argc){
        puts("File is not given.");
    }else{
        std::u32string t = init_testing(argv[1]);
        if(t.length()){
            char32_t* p        = const_cast<char32_t*>(t.c_str());
            auto      loc      = std::make_shared<Location>(p);

            Errors_and_tries etr;
            etr.ec             = std::make_shared<Error_count>();
            etr.ids_trie       = std::make_shared<Char_trie>();
            etr.strs_trie      = std::make_shared<Char_trie>();
//             auto aesc          = std::make_shared<Aux_expr_scaner>(loc,  etr);
            auto trie_for_sets = std::make_shared<Trie_for_set_of_char32>();
            auto esc           = std::make_shared<Expr_scaner>(loc, etr, trie_for_sets);
            auto scope         = std::make_shared<Scope>();

//             std::shared_ptr<Act_expr_parser> arp;
// // #define NUM_TEST
// #ifdef NUM_TEST
//             arp = std::make_shared<Num_regexp_parser>(esc, etr, scope, expr_slr_tables);
// #else
//             arp = std::make_shared<Str_regexp_parser>(esc, etr, scope, expr_slr_tables);
// #endif
// //             auto expr_parser   = std::make_unique<Simple_regex_parser>(esc, etr);

            Command_buffer commands;
            arp->compile(commands);
            print_commands(commands, trie_for_sets);
        }
    }
    return 0;
}