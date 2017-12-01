#ifndef DEFAULTEFFECTS_H
#define DEFAULTEFFECTS_H

#include <memory>
#include <QDialog>
#include "data_structs.h"

namespace Ui {
    class DefaultEffects;
}

class DefaultEffects : public QDialog
{
    Q_OBJECT

public:
    explicit DefaultEffects(QWidget *parent = nullptr);

private:
    const std::unique_ptr<Ui::DefaultEffects> ui;

private slots:
    void choose_fx(int);
    void get_settings();
    void save_default_effects();
};

#endif // DEFAULTEFFECTS_H
