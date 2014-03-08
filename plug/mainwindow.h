#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug>
#include <QShortcut>
#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include "effect.h"
#include "amplifier.h"
#include "mustang.h"
#include "data_structs.h"
#include "about.h"
#include "saveonamp.h"
#include "loadfromamp.h"
#include "save_effects.h"
#include "settings.h"
#include "loadfromfile.h"
#include "savetofile.h"
#include "library.h"
#include "defaulteffects.h"
#include "quickpresets.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void start_amp(void);    // initialize the communication
    void stop_amp(void);    // terminate communication
    int set_effect(struct fx_pedal_settings);
    int set_amplifier(struct amp_settings);
    int save_on_amp(char *, int);
    int load_from_amp(int);
    void enable_buttons(void);
    void change_name(int, QString *);
    void save_effects(int, char *, int, bool, bool, bool);
    void set_index(int);
    void loadfile(QString filename = QString());
    void get_settings(struct amp_settings*, struct fx_pedal_settings[4]);
    void change_title(QString);
    void update_firmware();
    void empty_other(int, Effect*);

private:
    Ui::MainWindow *ui;

    // pointers to objects
    QString current_name;
    char  names[100][32];
    bool manual_check, connected;
    Mustang *amp_ops;
    Amplifier *amp;
    Effect *effect1;
    Effect *effect2;
    Effect *effect3;
    Effect *effect4;
    About *about_window;
    SaveOnAmp *save;
    LoadFromAmp *load;
    SaveEffects *seffects;
    Settings *settings_win;
    SaveToFile *saver;
    Library *library;
    DefaultEffects *deffx;
    QuickPresets *quickpres;

    QNetworkReply *reply;

private slots:
    void about();
    void check_for_updates();
    void httpReadyRead();
    void show_fx1();
    void show_fx2();
    void show_fx3();
    void show_fx4();
    void show_amp();
    void show_library();
    void show_default_effects();
    int check_fx_family(int);
    void load_presets0();
    void load_presets1();
    void load_presets2();
    void load_presets3();
    void load_presets4();
    void load_presets5();
    void load_presets6();
    void load_presets7();
    void load_presets8();
    void load_presets9();


signals:
    void started();
};

#endif // MAINWINDOW_H
