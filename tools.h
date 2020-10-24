#ifndef TOOLS_H
#define TOOLS_H

#include <QLineEdit>

namespace Tools {

bool isValidEmail(const char* email);

bool isIdInUse(const long long& id);
bool equalEmails(const char* a, const char* b);

bool validateEmail(QLineEdit* lineEdit);
}  // namespace Tools
#endif // TOOLS_H
