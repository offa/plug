#ifndef QUICKPRESETS_H
#define QUICKPRESETS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class QuickPresets;
}

class QuickPresets : public QDialog
{
    Q_OBJECT

public:
    explicit QuickPresets(QWidget *parent = nullptr);
    ~QuickPresets();
    void load_names(char names[][32]);
    void delete_items();
    void change_name(int, QString *);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QuickPresets *ui;

private slots:
    void setDefaultPreset0(int);
    void setDefaultPreset1(int);
    void setDefaultPreset2(int);
    void setDefaultPreset3(int);
    void setDefaultPreset4(int);
    void setDefaultPreset5(int);
    void setDefaultPreset6(int);
    void setDefaultPreset7(int);
    void setDefaultPreset8(int);
    void setDefaultPreset9(int);
};

#endif // QUICKPRESETS_H
