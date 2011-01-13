#ifndef LOADFROMAMP_H
#define LOADFROMAMP_H

#include <QDialog>

namespace Ui {
    class LoadFromAmp;
}

class LoadFromAmp : public QDialog
{
    Q_OBJECT

public:
    explicit LoadFromAmp(QWidget *parent = 0);
    ~LoadFromAmp();

private:
    Ui::LoadFromAmp *ui;

private slots:
    void load(void);
};

#endif // LOADFROMAMP_H
