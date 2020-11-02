#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget {
  Q_OBJECT

 public:
  explicit SettingsWindow(QWidget* parent = nullptr);
  ~SettingsWindow();

 private slots:

  void on_memorySearchCheckBox_clicked();

  void on_primaryIndexAVLButton_clicked();

 private:
  Ui::SettingsWindow* ui;
};

#endif // SETTINGSWINDOW_H
