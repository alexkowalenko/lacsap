#pragma once

#include <ostream>
#include <string>

class Location {
  public:
    Location(const std::string &file, unsigned int line, unsigned int col)
        : fname(file), lineNum(line), column(col) {}
    Location() : fname(""), lineNum(0), column(0) {}
    std::string  to_string() const;
    std::string  FileName() const { return fname; }
                 operator bool() const { return fname != "" || lineNum != 0; }
    unsigned int LineNumber() const { return lineNum; }
    unsigned int Column() const { return column; }

  private:
    std::string  fname;
    unsigned int lineNum;
    unsigned int column;
};

std::ostream &operator<<(std::ostream &os, const Location &loc);
