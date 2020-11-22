#ifndef SECONDARYINDEXENTRY_H
#define SECONDARYINDEXENTRY_H

#include <string>
#include "list.h"

class SecondaryIndexEntry
{
 public:
  SecondaryIndexEntry();
  SecondaryIndexEntry(const std::string& email);
  SecondaryIndexEntry(const std::string& email, long long reference);

  std::string getEmail() const;
  void setEmail(const std::string& email);

  List<long long>& getReferenceList();
  void setReferenceList(const List<long long>& referenceList);

  bool operator==(SecondaryIndexEntry other) const;
  bool operator<(SecondaryIndexEntry other) const;

 private:
  std::string email;
  List<long long> referenceList;
};

#endif // SECONDARYINDEXENTRY_H
