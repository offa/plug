#ifndef SAVE_EFFECTS_H
#define SAVE_EFFECTS_H

#include <QDialog>

namespace Ui {
    class Save_effects;
}

class SaveEffects : public QDialog
{
    Q_OBJECT

public:
    explicit SaveEffects(QWidget *parent = 0);
    ~SaveEffects();

private:
    Ui::Save_effects *ui;

private slots:
    void select_checkbox();
    void send();
};

#endif // SAVE_EFFECTS_H
