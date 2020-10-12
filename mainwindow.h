#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>
#include "email.h"
#include "list.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private slots:
  // Sidebar slots
  void on_inboxButton_clicked();
  void on_backupButton_clicked();

  // Inbox slots
  void on_searchButton_clicked();

  void on_newEmailButton_clicked();

  // Email slots

  void on_receiverLineEdit_editingFinished();

  void on_senderLineEdit_editingFinished();

  void on_writeButton_clicked();

  void on_ccLineEdit_editingFinished();

  void on_bccLineEdit_editingFinished();

  void on_idSpinBox_editingFinished();

  // Backup Slots

  void on_createBackupButton_clicked();

 private:
  // Pseudo slots
  void onTimeout();
  void onReadButtonClicked();
  void onEditButtonClicked();
  void onDeleteButtonClicked();

  // Class Methods
  long long getClickedId();
  void populateEmailTable(List<Email> emailList);
  void clearEmailTable();

  void connectTimer();
  void disconnectTimer();

  void searchById(const long long id);
  void searchBySender(const char* sender);

  void openInbox();
  void openEmail(const long long id, const bool readOnly);

  void setEmailControls(const bool readOnly);
  void resetEmailControls();

  void loadEmailData(const Email& email);
  void clearEmailData();

  void displayEmailData();

  bool validateEmailId();
  bool validateEmail(QLineEdit* lineEdit);
  bool validateEmailList(QLineEdit* lineEdit);
  bool validateFields();

 private:
  Ui::MainWindow* ui;
  QTimer* timer;
  Email* emailData;
};

#endif  // MAINWINDOW_H
