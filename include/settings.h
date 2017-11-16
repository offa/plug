#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    Ui::Settings *ui;

private slots:
    void change_updates(bool);
    void change_connect(bool);
    void change_oneset(bool);
    void change_keepopen(bool);
    void change_popupwindows(bool);
    void change_effectvalues(bool);
};

#endif // SETTINGS_H
