#ifndef PRIMARYINDEX_H
#define PRIMARYINDEX_H


class PrimaryIndex
{
 public:
  PrimaryIndex();
  PrimaryIndex(const long long id);
  PrimaryIndex(const long long id, const long long pos);

  long long getId() const;
  void setId(const long long id);

  long long getPosition() const;
  void setPosition(const long long pos);

  bool operator==(PrimaryIndex other) const;
  bool operator<(PrimaryIndex other) const;

 private:
  long long id;
  long long position;
};

#endif // PRIMARYINDEX_H
