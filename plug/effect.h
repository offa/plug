#ifndef EFFECT_H
#define EFFECT_H

#include <QMainWindow>

namespace Ui {
    class Effect;
}

class Effect : public QMainWindow
{
    Q_OBJECT

public:
    explicit Effect(QWidget *parent = 0);
    ~Effect();

private:
    Ui::Effect *ui;
};

#endif // EFFECT_H
