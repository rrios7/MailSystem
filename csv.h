#ifndef CSV_H
#define CSV_H

#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include "list.h"

namespace CSV {
class Reader {
 private:
  enum State { NoQuote, Quote, QuotedQuote, Accepted };

 public:
  Reader() {}

  Reader(const std::string& filename) : file{filename} { skipBOM(); }

  void open(const std::string& filename) {
    if (file.is_open())
      file.close();

    file.open(filename);
    skipBOM();
  }

  void close() { file.close(); }

  void reset() {
    file.clear();
    file.seekg(0);
  }

  void skipHeaders() {
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\r');
  }

  bool next() {
    if (data.empty())
      std::getline(file, data, '\r');

    return !data.empty();
  }

  List<std::string> getNextRow() {
    List<std::string> tokens;

    if (data.empty())
      std::getline(file, data, '\r');

    while (data.size() > 0)
      tokens.pushBack(tokenize());

    return tokens;
  }

  std::string getNextField() {
    if (data.empty())
      std::getline(file, data, '\r');

    return tokenize();
  }

 private:
  void skipBOM() {
    char b = file.get();
    char o = file.get();
    char m = file.get();
    if (b != (char)0xEF || o != (char)0xBB || m != (char)0xBF)
      file.seekg(0);
  }

  std::string tokenize() {
    std::string token;
    State state = State::NoQuote;
    std::string::iterator it;

    for (it = data.begin(); it != data.end(); ++it) {
      char c = *it;

      if (state == State::Accepted)
        break;

      switch (state) {
        case State::NoQuote:
          switch (c) {
            case ',':
              state = State::Accepted;
              break;
            case '"':
              state = State::Quote;
              break;
            default:
              token.push_back(c);
              break;
          }
          break;
        case State::Quote:
          switch (c) {
            case '"':
              state = State::QuotedQuote;
              break;
            default:
              token.push_back(c);
              break;
          }
          break;
        case State::QuotedQuote:
          switch (c) {
            case ',':
              state = State::Accepted;
              break;
            case '"':
              state = State::Quote;
              token.push_back('"');
              break;
          }
          break;
        default:
          break;
      }
    }

    data = std::string(it, data.end());
    return token;
  }

 private:
  std::ifstream file;
  std::string data;
};

class Writer {
 public:
  Writer(const std::string& filename,
         const int columns,
         const char delimiter = ',')
      : filename{filename},
        columns{columns},
        delimiter{delimiter},
        fieldCounter{0} {}

  void flush() {
    std::ofstream file{filename};

    if (!file.good())
      file.close();

    // Byte Order Mark (UTF-8 Support)
    file << "\xef\xbb\xbf";

    file << toString();

    file.close();
  }

  template <typename T>
  Writer& operator<<(const T& field) {
    return append(field);
  }

  std::string toString() { return data.str(); }

 private:
  template <typename T>
  Writer& append(T field) {
    data << field;

    if (++fieldCounter == columns) {
      fieldCounter = 0;
      data << "\r";
    } else {
      data << delimiter;
    }

    return *this;
  }

  Writer& append(const char* field) { return append(std::string{field}); }
  Writer& append(char* field) { return append(std::string{field}); }

  Writer& append(std::string field) {
    size_t position = field.find("\"");
    bool specialCharacter = position != std::string::npos;

    while (position != std::string::npos) {
      field.insert(position, "\"");
      position = field.find("\"", position + 2);
    }

    specialCharacter |= field.find(delimiter) != std::string::npos;
    specialCharacter |= field.find('\n') != std::string::npos;

    if (specialCharacter) {
      field = "\"" + field + "\"";
    }

    return append<>(field);
  }

 private:
  std::string filename;
  int columns;
  char delimiter;
  int fieldCounter;
  std::stringstream data;
};
}  // namespace CSV

#endif  // CSV_H
