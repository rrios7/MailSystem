#include "primaryindex.h"

PrimaryIndex::PrimaryIndex() {}

PrimaryIndex::PrimaryIndex(const long long id) : id{id} {}

PrimaryIndex::PrimaryIndex(const long long id, const long long pos)
    : id{id}, position{pos} {}

long long PrimaryIndex::getId() const {
  return id;
}

void PrimaryIndex::setId(const long long id) {
  this->id = id;
}

long long PrimaryIndex::getPosition() const {
  return position;
}

void PrimaryIndex::setPosition(const long long pos) {
  position = pos;
}

bool PrimaryIndex::operator==(PrimaryIndex other) const {
  return id == other.id;
}

bool PrimaryIndex::operator<(PrimaryIndex other) const {
  return id < other.id;
}
