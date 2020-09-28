#ifndef EMAILTOOLS_H
#define EMAILTOOLS_H

#include <string>
#include "email.h"

namespace EmailTools {

const static std::string filename = "emails.dat";

void write(Email& email);
Email read(const long long& id);

bool isIdInUse(const long long& id);
bool isValidEmail(const char* email);
}  // namespace EmailTools

#endif  // EMAILTOOLS_H
