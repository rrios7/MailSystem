#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "backupwindow.h"
#include "emailwindow.h"
#include "inboxwindow.h"
#include "settingswindow.h"
#include "windowdata.h"

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

 public:
  void showPage(const WindowData::Page page);

 private slots:

  void on_inboxButton_clicked();

  void on_backupButton_clicked();

  void on_settingsButton_clicked();

 private:
  Ui::MainWindow* ui;

  InboxWindow* inboxWindow;
  EmailWindow* emailWindow;
  BackupWindow* backupWindow;
  SettingsWindow* settingsWindow;
};

#endif  // MAINWINDOW_H
