#pragma once

#include "constants.h"
#include "location.h"
#include "source.h"
#include "token.h"
#include "types.h"
#include <exception>
#include <fstream>
#include <string>

class Lexer {
  public:
    Lexer(Source &source);
    Token GetToken();

  private:
    int NextChar();
    int CurChar();
    int PeekChar();
    int GetChar();

    Token NumberToken();
    Token StringToken();

    Location Where() const { return Location(source); }

  private:
    Source &source;
    int     curChar;
    int     nextChar;
    int     curValid;
};
