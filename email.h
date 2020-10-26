#ifndef EMAIL_H
#define EMAIL_H

#include <fstream>
#include <string>

class Email {
 public:
  Email();
  Email(const long long id);
  Email(const Email& other);

  long long getId() const;
  void setId(const long long id);

  const char* getDate() const;
  void setDate(const char* const date);

  const char* getTime() const;
  void setTime(const char* const time);

  const char* getSubject() const;
  void setSubject(const char* const subject);

  const char* getSender() const;
  void setSender(const char* const sender);

  const char* getReceiver() const;
  void setReceiver(const char* const receiver);

  const char* getCC() const;
  void setCC(const char* const cc);

  const char* getBCC() const;
  void setBCC(const char* const bcc);

  const char* getContent() const;
  void setContent(const char* const content);

  void copyData(const Email& other);

  Email& operator=(const Email& ohter);
  bool operator==(const Email& other);

  friend std::ifstream& operator>>(std::ifstream& is, Email& c);
  friend std::ofstream& operator<<(std::ofstream& os, Email& c);

 private:
  const static int dateLength = 10;
  const static int timeLength = 5;
  const static int subjectLength = 100;
  const static int emailLength = 50;
  const static int emailContent = 1024;
  const static int maximumCopies = 20;

  long long id;
  char date[dateLength + 1];
  char time[timeLength + 1];
  char subject[subjectLength + 1];
  char sender[emailLength + 1];
  char receiver[emailLength + 1];
  char cc[maximumCopies * (emailLength + 2)];
  char bcc[maximumCopies * (emailLength + 2)];
  char content[emailContent + 1];
};

#endif  // EMAIL_H
