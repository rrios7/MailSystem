#ifndef PRIMARYINDEXENTRY_H
#define PRIMARYINDEXENTRY_H

class PrimaryIndexEntry {
 public:
  PrimaryIndexEntry();
  PrimaryIndexEntry(const long long id);
  PrimaryIndexEntry(const long long id, const long long pos);

  long long getId() const;
  void setId(const long long id);

  long long getPosition() const;
  void setPosition(const long long pos);

  bool operator==(PrimaryIndexEntry other) const;
  bool operator<(PrimaryIndexEntry other) const;

 private:
  long long id;
  long long position;
};

#endif  // PRIMARYINDEXENTRY_H
