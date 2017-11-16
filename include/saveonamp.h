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
    explicit SaveOnAmp(QWidget *parent = nullptr);
    ~SaveOnAmp();
    void load_names(char names[][32]);
    void delete_items();

public slots:
    void change_index(int, QString);

private:
    Ui::SaveOnAmp *ui;

private slots:
    void save(void);
};

#endif // SAVEONAMP_H
