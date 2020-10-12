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
  return this->id == other.id;
}
