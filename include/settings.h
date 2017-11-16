#ifndef SETTINGS_H
#define SETTINGS_H

#include <memory>
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

private slots:
    void change_updates(bool);
    void change_connect(bool);
    void change_oneset(bool);
    void change_keepopen(bool);
    void change_popupwindows(bool);
    void change_effectvalues(bool);

private:
    const std::unique_ptr<Ui::Settings> ui;

};

#endif // SETTINGS_H
