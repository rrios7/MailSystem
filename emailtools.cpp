#include "emailtools.h"
#include <fstream>
#include <regex>

void EmailTools::write(Email& email) {
  std::fstream file{EmailTools::filename, std::ios::in | std::ios::out |
                                              std::ios::ate | std::ios::binary};

  if (!file.is_open()) {
    file.open(EmailTools::filename, std::ios::out);
  }

  if (!file.good()) {
    file.close();
    return;
  }

  long id = email.getId();
  file.seekp(id * sizeof(email));
  file.write(reinterpret_cast<char*>(&email), sizeof(email));
  file.close();
}

Email EmailTools::read(const long long& id) {
  std::ifstream file{EmailTools::filename, std::ios::binary};

  if (!file.good())
    file.close();

  Email email;
  file.seekg(id * sizeof(Email));
  file.read(reinterpret_cast<char*>(&email), sizeof(email));
  file.close();

  return email;
}

bool EmailTools::isIdInUse(const long long& id) {
  Email email = read(id);
  return isValidEmail(email.getSender());
}

bool EmailTools::isValidEmail(const char* email) {
  std::regex exp{R"([a-zA-Z0-9.-_]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+)"};
  return std::regex_match(email, exp);
}
