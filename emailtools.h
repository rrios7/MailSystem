#ifndef EMAILTOOLS_H
#define EMAILTOOLS_H

#include <string>
#include "email.h"
#include "list.h"

namespace EmailTools {

const static std::string filename = "emails.dat";

void writeEmail(Email& email);
Email readEmail(const long long& id);
void readInbox(List<Email>& inbox);
void readEmailList(List<Email>& emailList);

bool isIdInUse(const long long& id);
bool isValidEmail(const char* email);
bool equalEmails(const char* a, const char* b);
}  // namespace EmailTools

#endif  // EMAILTOOLS_H
