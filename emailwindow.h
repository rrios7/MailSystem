#ifndef EMAILWINDOW_H
#define EMAILWINDOW_H

#include <QLineEdit>
#include <QTimer>
#include <QWidget>
#include "email.h"

namespace Ui {
class EmailWindow;
}

class EmailWindow : public QWidget
{
  Q_OBJECT

 public:
  explicit EmailWindow(QWidget *parent = nullptr);
  ~EmailWindow();

  // Class Methods
 public:
  void startTimer();
  void stopTimer();

  void setControls();
  void setFields();

 private:
  bool validateId();
  bool validateEmail(QLineEdit* lineEdit);
  bool validateEmailList(QLineEdit* lineEdit);
  bool validateFields();

  // Pseudo slots
 private:
  void onTimeout();

 private slots:
  void on_idSpinBox_editingFinished();

  void on_senderLineEdit_editingFinished();

  void on_receiverLineEdit_editingFinished();

  void on_ccLineEdit_editingFinished();

  void on_bccLineEdit_editingFinished();

  void on_writeButton_clicked();

 signals:
  void writeEmail();
  void writeBackupEmail();

 private:
  Ui::EmailWindow* ui;
  QTimer* timer;
};

#endif // EMAILWINDOW_H
