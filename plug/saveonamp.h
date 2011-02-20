#ifndef SAVEONAMP_H
#define SAVEONAMP_H

#include <QMainWindow>

namespace Ui {
    class SaveOnAmp;
}

class SaveOnAmp : public QMainWindow
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
