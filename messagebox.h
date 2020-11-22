#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog {
  Q_OBJECT
 public:
  explicit MessageBox(QWidget* parent = nullptr,
                      const QString& msg = "",
                      bool question = false);
  ~MessageBox();
  static int display(const QString& msg);
  static int question(const QString& msg);

 private:
  Ui::MessageBox* ui;
};

#endif  // MESSAGEBOX_H
