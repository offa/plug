#ifndef SAVETOFILE_H
#define SAVETOFILE_H

#include <memory>
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
    explicit SaveToFile(QWidget *parent = nullptr);

private slots:
    QString choose_destination();
    void savefile();

signals:
    void destination_chosen(QString);

private:
    const std::unique_ptr<Ui::SaveToFile> ui;
    QXmlStreamWriter *xml;

    void writeAmp(struct amp_settings);
    void manageWriteFX(struct fx_pedal_settings[4]);
    void writeFX(struct fx_pedal_settings);
    void writeFUSE();
    void writeUSBGain(int);

};

#endif // SAVETOFILE_H
