#ifndef INBOXWINDOW_H
#define INBOXWINDOW_H

#include <QWidget>
#include "emaildao.h"
#include "list.h"
#include "windowdata.h"

namespace Ui {
class InboxWindow;
}

class InboxWindow : public QWidget {
  Q_OBJECT

 public:
  explicit InboxWindow(QWidget *parent = nullptr);
  ~InboxWindow();

 public:
  void writeEmail();

  // Class Methods
 private:
  void openEmail(long long id, const WindowData::Operation operation);

  void searchById(const long long id);
  void searchBySender(const char* sender);

  void clearEmailTable();
  void populateEmailTable(List<Email> emailList);

  // Pseudo Slots
 private:
  void onReadButtonClicked();
  void onEditButtonClicked();
  void onDeleteButtonClicked();
  long long getClickedId();

 private slots:
  void on_searchButton_clicked();

  void on_newEmailButton_clicked();

 signals:
  void showPage(const WindowData::Page page);

 private:
  Ui::InboxWindow* ui;
};

#endif // INBOXWINDOW_H
