#ifndef EMAILTOOLS_H
#define EMAILTOOLS_H

#include <fstream>
#include <string>
#include "email.h"

class EmailDAO {
 public:
  static EmailDAO& getInstance() {
    static EmailDAO instance;
    return instance;
  }

 private:
  EmailDAO() {
    file.open(fileName,
              std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      file.open(fileName, std::ios::out);
      file.close();
      file.open(fileName, std::ios::in | std::ios::out | std::ios::ate |
                              std::ios::binary);
    }
  }

  ~EmailDAO() { file.close(); }

 public:
  EmailDAO(EmailDAO const&) = delete;
  void operator=(EmailDAO const&) = delete;

 public:
  void write(Email& email) {
    if (file.fail())
      file.clear();

    long long id = email.getId();
    file.seekp(id * sizeof(email));
    file.write(reinterpret_cast<char*>(&email), sizeof(email));
  }

  Email read(const long long& id) {
    if (file.fail())
      file.clear();

    Email email;
    file.seekg(id * sizeof(Email));
    file.read(reinterpret_cast<char*>(&email), sizeof(email));
    return email;
  }

  bool next() { return !file.eof(); }

 private:
  const std::string fileName = "emails.dat";
  std::fstream file;
};

#endif  // EMAILTOOLS_H
