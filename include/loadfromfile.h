#ifndef LOADFROMFILE_H
#define LOADFROMFILE_H

#include <QTextEdit>
#include <QFile>
#include <QXmlStreamReader>
#include <memory>
#include "data_structs.h"
#include "effects_enum.h"

class LoadFromFile
{
public:
    LoadFromFile(QFile* file, QString* name, amp_settings* amp_settings, fx_pedal_settings fx_settings[4]);

    void loadfile();

private:

    QString* m_name;
    amp_settings* m_amp_settings;
    fx_pedal_settings* m_fx_settings;
    std::unique_ptr<QXmlStreamReader> m_xml;

    void parseAmp();
    void parseFX();
    void parseFUSE();
};

#endif // LOADFROMFILE_H
