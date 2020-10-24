#include "tools.h"

#include <cstring>
#include <regex>
#include "emaildao.h"

bool Tools::isValidEmail(const char* email) {
  std::regex exp{R"([a-zA-Z0-9.-_]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+)"};
  return std::regex_match(email, exp);
}

bool Tools::isIdInUse(const long long& id) {
  Email email = EmailDAO::getInstance().read(id);
  return isValidEmail(email.getSender());
}

bool Tools::equalEmails(const char* a, const char* b) {
  return std::strcmp(a, b) == 0;
}

bool Tools::validateEmail(QLineEdit* lineEdit) {
  std::string email = lineEdit->text().toStdString();

  if (!isValidEmail(email.c_str())) {
    lineEdit->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
    return false;
  }

  lineEdit->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}
