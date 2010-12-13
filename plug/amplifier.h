#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <QMainWindow>

namespace Ui {
    class Amplifier;
}

class Amplifier : public QMainWindow
{
    Q_OBJECT

public:
    explicit Amplifier(QWidget *parent = 0);
    ~Amplifier();

private:
    Ui::Amplifier *ui;
};

#endif // AMPLIFIER_H
