#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
  Q_OBJECT

 public:
  explicit MessageBox(QWidget* parent = nullptr, const QString& msg = "");
  ~MessageBox();
  static void display(const QString& msg);

 private slots:
  void on_okButton_clicked();

 private:
  Ui::MessageBox *ui;
};

#endif // MESSAGEBOX_H
