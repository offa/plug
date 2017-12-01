#ifndef SAVEONAMP_H
#define SAVEONAMP_H

#include <memory>
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
    const std::unique_ptr<Ui::SaveOnAmp> ui;

private slots:
    void save();
};

#endif // SAVEONAMP_H
