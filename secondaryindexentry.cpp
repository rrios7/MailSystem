#include "secondaryindexentry.h"

SecondaryIndexEntry::SecondaryIndexEntry() {}

SecondaryIndexEntry::SecondaryIndexEntry(const std::string& email)
    : email{email} {}

SecondaryIndexEntry::SecondaryIndexEntry(const std::string& email,
                                         long long reference)
    : email{email} {
  referenceList.pushOrdered(reference);
}

std::string SecondaryIndexEntry::getEmail() const {
  return email;
}

void SecondaryIndexEntry::setEmail(const std::string& value) {
  email = value;
}

List<long long>& SecondaryIndexEntry::getReferenceList() {
  return referenceList;
}

void SecondaryIndexEntry::setReferenceList(
    const List<long long>& referenceList) {
  this->referenceList = referenceList;
}

bool SecondaryIndexEntry::operator==(SecondaryIndexEntry other) const {
  return email == other.email;
}

bool SecondaryIndexEntry::operator<(SecondaryIndexEntry other) const {
  return email < other.email;
}
