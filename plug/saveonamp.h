#ifndef SAVEONAMP_H
#define SAVEONAMP_H

#include <QDialog>

namespace Ui {
    class SaveOnAmp;
}

class SaveOnAmp : public QDialog
{
    Q_OBJECT

public:
    explicit SaveOnAmp(QWidget *parent = 0);
    ~SaveOnAmp();
    void load_names(char names[24][32]);

private:
    Ui::SaveOnAmp *ui;

private slots:
    void save(void);
};

#endif // SAVEONAMP_H
