#include "emailtools.h"
#include <QDebug>
#include <cstring>
#include <fstream>
#include <regex>
#include "csv.h"

void EmailTools::writeEmail(Email& email) {
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

Email EmailTools::readEmail(const long long& id) {
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
  Email email = readEmail(id);
  return isValidEmail(email.getSender());
}

bool EmailTools::isValidEmail(const char* email) {
  std::regex exp{R"([a-zA-Z0-9.-_]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+)"};
  return std::regex_match(email, exp);
}

void EmailTools::readInbox(List<Email>& inbox) {
  std::ifstream file{EmailTools::filename, std::ios::binary};

  if (!file.good())
    file.close();

  int id = 0;
  Email email;
  while (!file.eof()) {
    file.seekg(id * sizeof(Email));
    file.read(reinterpret_cast<char*>(&email), sizeof(email));

    if (email.getId() == id && isValidEmail(email.getSender())) {
      inbox.pushBack(email);
    }
    ++id;
  }

  file.close();
}

bool EmailTools::equalEmails(const char* a, const char* b) {
  return std::strcmp(a, b) == 0;
}

void EmailTools::createBackup() {
  std::ifstream file{EmailTools::filename, std::ios::binary};

  if (!file.good())
    file.close();

  int id = 0;
  Email email;
  CSV::Writer writer("emails.csv", 9);

  // Headers
  writer << "ID"
         << "Date"
         << "Time"
         << "Subject"
         << "Sender"
         << "Receiver"
         << "CC"
         << "BCC"
         << "Content";

  while (!file.eof()) {
    file.seekg(id * sizeof(Email));
    file.read(reinterpret_cast<char*>(&email), sizeof(email));

    if (email.getId() == id && isValidEmail(email.getSender())) {
      writer << email.getId() << email.getDate() << email.getTime()
             << email.getSubject() << email.getSender() << email.getReceiver()
             << email.getCC() << email.getBCC() << email.getContent();
    }
    ++id;
  }

  file.close();
}
