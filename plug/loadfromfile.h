#ifndef LOADFROMFILE_H
#define LOADFROMFILE_H

#include <QTextEdit>
#include <QFile>
#include <QXmlStreamReader>
#include "data_structs.h"
#include "effects_enum.h"

class LoadFromFile
{
public:
    LoadFromFile(QFile*, QString*, struct amp_settings*, struct fx_pedal_settings[4]);
    ~LoadFromFile();
    void loadfile();

private:
    QString *name;
    struct amp_settings *amplifier_settings;
    struct fx_pedal_settings *fx_settings;
    QXmlStreamReader *xml;

    void parseAmp();
    void parseFX();
    void parseFUSE();
};

#endif // LOADFROMFILE_H
