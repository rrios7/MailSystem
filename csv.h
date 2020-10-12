#ifndef CSV_H
#define CSV_H

#include <fstream>
#include <sstream>
#include <string>

namespace CSV {
class Writer {
 public:
  Writer(const std::string& filename,
         const int columns,
         const char delimiter = ',')
      : filename{filename},
        columns{columns},
        delimiter{delimiter},
        fieldCounter{0} {}

  ~Writer() {
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
      data << std::endl;
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

class Reader {};

}  // namespace CSV

#endif  // CSV_H
