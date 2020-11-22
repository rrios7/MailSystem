#include "primaryindexentry.h"

PrimaryIndexEntry::PrimaryIndexEntry() {}

PrimaryIndexEntry::PrimaryIndexEntry(const long long id) : id{id} {}

PrimaryIndexEntry::PrimaryIndexEntry(const long long id, const long long pos)
    : id{id}, position{pos} {}

long long PrimaryIndexEntry::getId() const {
  return id;
}

void PrimaryIndexEntry::setId(const long long id) {
  this->id = id;
}

long long PrimaryIndexEntry::getPosition() const {
  return position;
}

void PrimaryIndexEntry::setPosition(const long long pos) {
  position = pos;
}

bool PrimaryIndexEntry::operator==(PrimaryIndexEntry other) const {
  return id == other.id;
}

bool PrimaryIndexEntry::operator<(PrimaryIndexEntry other) const {
  return id < other.id;
}
