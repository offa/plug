#ifndef LOADFROMAMP_H
#define LOADFROMAMP_H

#include <memory>
#include <QMainWindow>

namespace Ui {
    class LoadFromAmp;
}

class LoadFromAmp : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoadFromAmp(QWidget *parent = nullptr);
    ~LoadFromAmp();
    void load_names(char names[][32]);
    void delete_items();
    void change_name(int, QString *);

private:
    const std::unique_ptr<Ui::LoadFromAmp> ui;

private slots:
    void load();
};

#endif // LOADFROMAMP_H
