#ifndef WINDOWDATA_H
#define WINDOWDATA_H

#include "email.h"

class WindowData {
 public:
  static WindowData& getInstance() {
    static WindowData instance;
    return instance;
  }

 private:
  WindowData() {}

 public:
  WindowData(WindowData const&) = delete;
  void operator=(WindowData const&) = delete;

 public:
  enum Page { InboxPage = 0, EmailPage, BackupPage, SettingsPage };
  enum Operation { None = 0, Read, Write, Update, Delete };

 public:
  Email& getEmail() { return email; }
  void setEmail(Email& email) { this->email = email; }

  Operation getOperation() const { return operation; }
  void setOperation(Operation operation) { this->operation = operation; }

  Page getPreviousPage() const { return previousPage; }
  void setPreviousPage(const Page& page) { previousPage = page; }

  bool isMemorySearchEnabled() const { return memorySearch; }

  void setMemorySearch(bool value) { memorySearch = value; }

 private:
  Email email;
  Operation operation;
  Page previousPage;
  bool memorySearch;
};

#endif  // WINDOWDATA_H
