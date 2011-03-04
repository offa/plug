#ifndef SAVETOFILE_H
#define SAVETOFILE_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "data_structs.h"

namespace Ui {
    class SaveToFile;
}

class SaveToFile : public QDialog
{
    Q_OBJECT

public:
    explicit SaveToFile(QWidget *parent = 0);
    ~SaveToFile();

private:
    Ui::SaveToFile *ui;
    QXmlStreamWriter *xml;

    void writeAmp(struct amp_settings);
    void manageWriteFX(struct fx_pedal_settings[4]);
    void writeFX(struct fx_pedal_settings);
    void writeFUSE();

private slots:
    QString choose_destination();
    void savefile();

signals:
    void destination_chosen(QString);
};

#endif // SAVETOFILE_H
