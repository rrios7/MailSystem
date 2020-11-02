#include "email.h"
#include <cstring>

Email::Email() {}

Email::Email(const long long id) : id{id} {}

Email::Email(const Email& other) {
  copyData(other);
}

long long Email::getId() const {
  return this->id;
}

void Email::setId(const long long id) {
  this->id = id;
}

const char* Email::getDate() const {
  return this->date;
}

void Email::setDate(const char* const date) {
  strcpy(this->date, date);
}

const char* Email::getTime() const {
  return this->time;
}

void Email::setTime(const char* const time) {
  strcpy(this->time, time);
}

const char* Email::getSubject() const {
  return this->subject;
}

void Email::setSubject(const char* const subject) {
  strcpy(this->subject, subject);
}

const char* Email::getSender() const {
  return this->sender;
}

void Email::setSender(const char* const sender) {
  strcpy(this->sender, sender);
}

const char* Email::getReceiver() const {
  return this->receiver;
}

void Email::setReceiver(const char* const receiver) {
  strcpy(this->receiver, receiver);
}

const char* Email::getCC() const {
  return this->cc;
}

void Email::setCC(const char* const cc) {
  strcpy(this->cc, cc);
}

const char* Email::getBCC() const {
  return this->bcc;
}

void Email::setBCC(const char* const bcc) {
  strcpy(this->bcc, bcc);
}

const char* Email::getContent() const {
  return this->content;
}

void Email::setContent(const char* const content) {
  strcpy(this->content, content);
}

void Email::copyData(const Email& other) {
  setId(other.id);
  setDate(other.date);
  setTime(other.time);
  setSubject(other.subject);
  setSender(other.sender);
  setReceiver(other.receiver);
  setCC(other.cc);
  setBCC(other.bcc);
  setContent(other.content);
}

Email& Email::operator=(const Email& other) {
  copyData(other);
  return *this;
}

bool Email::operator==(const Email& other) {
  return strcmp(this->sender, other.sender) == 0;
}

bool Email::operator<(const Email& other) {
  return strcmp(this->sender, other.getSender()) < 0;
}

std::ifstream& operator>>(std::ifstream& file, Email& email) {
  int field = 1;
  char* buffer = new char[1];

  while (field < 10) {
    long long length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));

    if (file.fail()) {
      email.setId(-1);
      break;
    }

    if (field != 1) {
      delete[] buffer;
      buffer = new char[length];
      file.read(buffer, length);
    }

    switch (field) {
      case 1:
        email.id = length;
        break;
      case 2:
        strcpy(email.date, buffer);
        break;
      case 3:
        strcpy(email.time, buffer);
        break;
      case 4:
        strcpy(email.subject, buffer);
        break;
      case 5:
        strcpy(email.sender, buffer);
        break;
      case 6:
        strcpy(email.receiver, buffer);
        break;
      case 7:
        strcpy(email.cc, buffer);
        break;
      case 8:
        strcpy(email.bcc, buffer);
        break;
      case 9:
        strcpy(email.content, buffer);
        break;
    }

    ++field;
  }
  return file;
}

std::ofstream& operator<<(std::ofstream& file, Email& email) {
  long long length;
  file.write(reinterpret_cast<char*>(&email.id), sizeof(email.id));
  length = std::strlen(email.date) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.date), length);
  length = std::strlen(email.time) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.time), length);
  length = std::strlen(email.subject) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.subject), length);
  length = std::strlen(email.sender) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.sender), length);
  length = std::strlen(email.receiver) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.receiver), length);
  length = std::strlen(email.cc) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.cc), length);
  length = std::strlen(email.bcc) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.bcc), length);
  length = std::strlen(email.content) + 1;
  file.write(reinterpret_cast<char*>(&length), sizeof(length));
  file.write(reinterpret_cast<char*>(&email.content), length);
  return file;
}
