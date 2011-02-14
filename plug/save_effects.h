#ifndef SAVE_EFFECTS_H
#define SAVE_EFFECTS_H

#include <QDialog>

namespace Ui {
    class Save_effects;
}

class Save_effects : public QDialog
{
    Q_OBJECT

public:
    explicit Save_effects(QWidget *parent = 0);
    ~Save_effects();

private:
    Ui::Save_effects *ui;

private slots:
    void select_checkbox();
    void send();
};

#endif // SAVE_EFFECTS_H
