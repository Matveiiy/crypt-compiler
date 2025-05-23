//
// Created by motya on 14.04.2025.
//

#ifndef CRYPT_LEXER_H
#define CRYPT_LEXER_H

#include "parser.h"
namespace parser {
    struct TokenData {
        std::string identifier = "";
        int token = 0;
        int lines = 0;
        int cnt = 0;
    };
    extern TokenData cur;
    extern TokenData prv;


    void keep_newlines();
    void skip_newlines();

    void roll_back();

    int get_tok(int token_info = ANY_TOKEN_EXPECTED);
    void init_lexer(std::istream &in);


    std::string token_to_string(TokenInfo tok, std::string temp_data);
}

#endif //CRYPT_LEXER_H
