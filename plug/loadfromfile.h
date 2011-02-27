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
    LoadFromFile();
    ~LoadFromFile();
    void parseFile(QFile*, QString*, struct amp_settings*, struct fx_pedal_settings*);

private:
    void parseAmp(QXmlStreamReader*, struct amp_settings*);
    void parseFX(QXmlStreamReader*, struct fx_pedal_settings[]);
    void parseFUSE(QXmlStreamReader*, QString*);
};

#endif // LOADFROMFILE_H
