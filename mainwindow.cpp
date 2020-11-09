#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent},
      ui{new Ui::MainWindow},
      inboxWindow{new InboxWindow},
      emailWindow{new EmailWindow},
      backupWindow{new BackupWindow},
      settingsWindow{new SettingsWindow} {
  ui->setupUi(this);

  ui->content->addWidget(inboxWindow);
  ui->content->addWidget(emailWindow);
  ui->content->addWidget(backupWindow);
  ui->content->addWidget(settingsWindow);
  setWindowFlag(Qt::FramelessWindowHint);

  connect(inboxWindow, &InboxWindow::showPage, this, &MainWindow::showPage);
  connect(emailWindow, &EmailWindow::writeEmail, this,
          [&]() { inboxWindow->writeEmail(); });
  connect(emailWindow, &EmailWindow::writeBackupEmail, this,
          [&]() { backupWindow->writeEmail(); });
  connect(backupWindow, &BackupWindow::showPage, this, &MainWindow::showPage);
  WindowData::getInstance();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::showPage(const WindowData::Page page) {
  WindowData& data = WindowData::getInstance();
  data.setPreviousPage(
      static_cast<WindowData::Page>(ui->content->currentIndex()));

  switch (page) {
    case WindowData::InboxPage:
      data.setOperation(WindowData::None);
      emailWindow->stopTimer();
      break;
    case WindowData::EmailPage:
      emailWindow->setControls();
      emailWindow->setFields();
      break;
    case WindowData::BackupPage:
      data.setOperation(WindowData::None);
      emailWindow->stopTimer();
      break;
    case WindowData::SettingsPage:
      data.setOperation(WindowData::None);
      break;
  }

  ui->content->setCurrentIndex(page);
}

void MainWindow::on_inboxButton_clicked() {
  showPage(WindowData::InboxPage);
}

void MainWindow::on_backupButton_clicked() {
  showPage(WindowData::BackupPage);
}

void MainWindow::on_settingsButton_clicked() {
  showPage(WindowData::SettingsPage);
}
