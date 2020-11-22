#ifndef ADVANCEDSEARCH_H
#define ADVANCEDSEARCH_H

#include <QDialog>

namespace Ui {
class AdvancedSearch;
}

class AdvancedSearch : public QDialog
{
  Q_OBJECT

 public:
  explicit AdvancedSearch(QString& sender,
                          QString& receiver,
                          int& searchType,
                          QWidget* parent = nullptr);
  ~AdvancedSearch();

 private slots:
  void on_searchButton_clicked();

  void on_closeButton_clicked();

 private:
  Ui::AdvancedSearch* ui;

  QString& sender;
  QString& receiver;
  int& searchType;
};

#endif // ADVANCEDSEARCH_H
