#include "savetofile.h"
#include "ui_savetofile.h"
#include "mainwindow.h"

SaveToFile::SaveToFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveToFile)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(choose_destination()));
    connect(this, SIGNAL(destination_chosen(QString)), ui->lineEdit, SLOT(setText(QString)));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(savefile()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(close()));
}

SaveToFile::~SaveToFile()
{
    delete ui;
}

QString SaveToFile::choose_destination()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save..."), QDir::homePath(), tr("FUSE files (*.fuse)"));

    QFileInfo info(filename);
    if(info.suffix().isEmpty())
        filename.append(".fuse");

    emit destination_chosen(filename);
    return filename;
}

void SaveToFile::savefile()
{
    if(ui->lineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error!"), tr("No file given"));
        return;
    }

    QFile *file = new QFile(ui->lineEdit->text(), this);
    if (!file->open(QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error!"), tr("Could not create file"));
        return;
    }

    ((MainWindow*)parent())->change_title(ui->lineEdit_2->text());

    xml = new QXmlStreamWriter(file);
    struct amp_settings amplifier_settings;
    struct fx_pedal_settings fx_settings[4];
    ((MainWindow*)parent())->get_settings(&amplifier_settings, fx_settings);

    xml->setAutoFormatting(true);
    xml->writeStartDocument();
    xml->writeStartElement("Preset");
    xml->writeAttribute("amplifier", "Mustang I/II");
    xml->writeAttribute("ProductId", "1");

    writeAmp(amplifier_settings);
    manageWriteFX(fx_settings);
    writeFUSE();

    xml->writeEndElement();
    xml->writeEndDocument();

    file->close();
    delete file;
    delete xml;

    this->close();
}

void SaveToFile::writeAmp(struct amp_settings settings)
{
    int model=0, something=0, something2=0, something3 = 128;

    switch (settings.amp_num)
    {
    case FENDER_57_DELUXE:
        model = 0x67;
        something = 0x01;
        something2 = 0x53;
        break;

    case FENDER_59_BASSMAN:
        model = 0x64;
        something = 0x02;
        something2 = 0x67;
        break;

    case FENDER_57_CHAMP:
        model = 0x7c;
        something = 0x0c;
        something2 = 0x00;
        break;

    case FENDER_65_DELUXE_REVERB:
        model = 0x53;
        something = 0x03;
        something2 = 0x6a;
        something3 = 0x00;
        break;

    case FENDER_65_PRINCETON:
        model = 0x6a;
        something = 0x04;
        something2 = 0x61;
        break;

    case FENDER_65_TWIN_REVERB:
        model = 0x75;
        something = 0x05;
        something2 = 0x72;
        break;

    case FENDER_SUPER_SONIC:
        model = 0x72;
        something = 0x06;
        something2 = 0x79;
        break;

    case BRITISH_60S:
        model = 0x61;
        something = 0x07;
        something2 = 0x5e;
        break;

    case BRITISH_70S:
        model = 0x79;
        something = 0x0b;
        something2 = 0x7c;
        break;

    case BRITISH_80S:
        model = 0x5e;
        something = 0x09;
        something2 = 0x5d;
        break;

    case AMERICAN_90S:
        model = 0x5d;
        something = 0x0a;
        something2 = 0x6d;
        break;

    case METAL_2000:
        model = 0x6d;
        something = 0x08;
        something2 = 0x75;
        break;
    }

    xml->writeStartElement("Amplifier");
    xml->writeStartElement("Module");
    xml->writeAttribute("ID", QString("%1").arg(model));
    xml->writeAttribute("POS", "0");
    xml->writeAttribute("BypassState", "1");

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "0");
    xml->writeCharacters(QString("%1").arg((settings.volume<<8)|settings.volume));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "1");
    xml->writeCharacters(QString("%1").arg((settings.gain<<8)|settings.gain));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "2");
    xml->writeCharacters(QString("%1").arg((settings.gain2<<8)|settings.gain2));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "3");
    xml->writeCharacters(QString("%1").arg((settings.master_vol<<8)|settings.master_vol));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "4");
    xml->writeCharacters(QString("%1").arg((settings.treble<<8)|settings.treble));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "5");
    xml->writeCharacters(QString("%1").arg((settings.middle<<8)|settings.middle));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "6");
    xml->writeCharacters(QString("%1").arg((settings.bass<<8)|settings.bass));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "7");
    xml->writeCharacters(QString("%1").arg((settings.presence<<8)|settings.presence));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "8");
    xml->writeCharacters(QString("%1").arg((something3<<8)|something3));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "9");
    xml->writeCharacters(QString("%1").arg((settings.depth<<8)|settings.depth));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "10");
    xml->writeCharacters(QString("%1").arg((settings.bias<<8)|settings.bias));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "11");
    xml->writeCharacters(QString("%1").arg((something3<<8)|something3));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "12");
    xml->writeCharacters(QString("%1").arg(something));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "13");
    xml->writeCharacters(QString("%1").arg(something));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "14");
    xml->writeCharacters(QString("%1").arg(something));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "15");
    xml->writeCharacters(QString("%1").arg(settings.noise_gate));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "16");
    xml->writeCharacters(QString("%1").arg(settings.threshold));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "17");
    xml->writeCharacters(QString("%1").arg(settings.cabinet));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "18");
    xml->writeCharacters(QString("%1").arg(something));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "19");
    xml->writeCharacters(QString("%1").arg(settings.sag));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "20");
    xml->writeCharacters(QString("%1").arg(settings.brightness?"1":"0"));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "21");
    xml->writeCharacters("1");
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "22");
    xml->writeCharacters(QString("%1").arg((something2<<8)|something2));
    xml->writeEndElement();

    xml->writeEndElement();  // end Amplifier
    xml->writeEndElement();  // end Module
}

void SaveToFile::manageWriteFX(fx_pedal_settings settings[4])
{
    struct fx_pedal_settings empty;

    empty.effect_num = empty.fx_slot = 0;
    empty.put_post_amp = false;

    xml->writeStartElement("FX");

    xml->writeStartElement("Stompbox");
    xml->writeAttribute("ID", "1");
    if((settings[0].effect_num > EMPTY) && (settings[0].effect_num <= COMPRESSOR))
        writeFX(settings[0]);
    else if((settings[1].effect_num > EMPTY) && (settings[1].effect_num <= COMPRESSOR))
        writeFX(settings[1]);
    else if((settings[2].effect_num > EMPTY) && (settings[2].effect_num <= COMPRESSOR))
        writeFX(settings[2]);
    else if((settings[3].effect_num > EMPTY) && (settings[3].effect_num <= COMPRESSOR))
        writeFX(settings[3]);
    else
        writeFX(empty);
    xml->writeEndElement();  // end Stompbox

    xml->writeStartElement("Modulation");
    xml->writeAttribute("ID", "2");
    if((settings[0].effect_num >= SINE_CHORUS) && (settings[0].effect_num <= PITCH_SHIFTER))
        writeFX(settings[0]);
    else if((settings[1].effect_num >= SINE_CHORUS) && (settings[1].effect_num <= PITCH_SHIFTER))
        writeFX(settings[1]);
    else if((settings[2].effect_num >= SINE_CHORUS) && (settings[2].effect_num <= PITCH_SHIFTER))
        writeFX(settings[2]);
    else if((settings[3].effect_num >= SINE_CHORUS) && (settings[3].effect_num <= PITCH_SHIFTER))
        writeFX(settings[3]);
    else
        writeFX(empty);
    xml->writeEndElement();  // end Modulation

    xml->writeStartElement("Delay");
    xml->writeAttribute("ID", "3");
    if((settings[0].effect_num >= MONO_DELAY) && (settings[0].effect_num <= STEREO_TAPE_DELAY))
        writeFX(settings[0]);
    else if((settings[1].effect_num >= MONO_DELAY) && (settings[1].effect_num <= STEREO_TAPE_DELAY))
        writeFX(settings[1]);
    else if((settings[2].effect_num >= MONO_DELAY) && (settings[2].effect_num <= STEREO_TAPE_DELAY))
        writeFX(settings[2]);
    else if((settings[3].effect_num >= MONO_DELAY) && (settings[3].effect_num <= STEREO_TAPE_DELAY))
        writeFX(settings[3]);
    else
        writeFX(empty);
    xml->writeEndElement();  // end Delay

    xml->writeStartElement("Reverb");
    xml->writeAttribute("ID", "4");
    if((settings[0].effect_num >= SMALL_HALL_REVERB) && (settings[0].effect_num <= FENDER_65_SPRING_REVERB))
        writeFX(settings[0]);
    else if((settings[1].effect_num >= SMALL_HALL_REVERB) && (settings[1].effect_num <= FENDER_65_SPRING_REVERB))
        writeFX(settings[1]);
    else if((settings[2].effect_num >= SMALL_HALL_REVERB) && (settings[2].effect_num <= FENDER_65_SPRING_REVERB))
        writeFX(settings[2]);
    else if((settings[3].effect_num >= SMALL_HALL_REVERB) && (settings[3].effect_num <= FENDER_65_SPRING_REVERB))
        writeFX(settings[3]);
    else
        writeFX(empty);
    xml->writeEndElement();  // end Reverb

    xml->writeEndElement();  // end FX
}

void SaveToFile::writeFX(struct fx_pedal_settings settings)
{
    int model=0, position;

    switch (settings.effect_num) {
    case EMPTY:
        model = 0x00;
        break;

    case OVERDRIVE:
        model = 0x3c;
        break;

    case FIXED_WAH:
        model = 0x49;
        break;

    case TOUCH_WAH:
        model = 0x4a;
        break;

    case FUZZ:
        model = 0x1a;
        break;

    case FUZZ_TOUCH_WAH:
        model = 0x1c;
        break;

    case SIMPLE_COMP:
        model = 0x88;
        break;

    case COMPRESSOR:
        model = 0x07;
        break;

    case SINE_CHORUS:
        model = 0x12;
        break;

    case TRIANGLE_CHORUS:
        model = 0x13;
        break;

    case SINE_FLANGER:
        model = 0x18;
        break;

    case TRIANGLE_FLANGER:
        model = 0x19;
        break;

    case VIBRATONE:
        model = 0x2d;
        break;

    case VINTAGE_TREMOLO:
        model = 0x40;
        break;

    case SINE_TREMOLO:
        model = 0x41;
        break;

    case RING_MODULATOR:
        model = 0x22;
        break;

    case STEP_FILTER:
        model = 0x29;
        break;

    case PHASER:
        model = 0x4f;
        break;

    case PITCH_SHIFTER:
        model = 0x1f;
        break;

    case MONO_DELAY:
        model = 0x16;
        break;

    case MONO_ECHO_FILTER:
        model = 0x43;
        break;

    case STEREO_ECHO_FILTER:
        model = 0x48;
        break;

    case MULTITAP_DELAY:
        model = 0x44;
        break;

    case PING_PONG_DELAY:
        model = 0x45;
        break;

    case DUCKING_DELAY:
        model = 0x15;
        break;

    case REVERSE_DELAY:
        model = 0x46;
        break;

    case TAPE_DELAY:
        model = 0x2b;
        break;

    case STEREO_TAPE_DELAY:
        model = 0x2a;
        break;

    case SMALL_HALL_REVERB:
        model = 0x24;
        break;

    case LARGE_HALL_REVERB:
        model = 0x3a;
        break;

    case SMALL_ROOM_REVERB:
        model = 0x26;
        break;

    case LARGE_ROOM_REVERB:
        model = 0x3b;
        break;

    case SMALL_PLATE_REVERB:
        model = 0x4e;
        break;

    case LARGE_PLATE_REVERB:
        model = 0x4b;
        break;

    case AMBIENT_REVERB:
        model = 0x4c;
        break;

    case ARENA_REVERB:
        model = 0x4d;
        break;

    case FENDER_63_SPRING_REVERB:
        model = 0x21;
        break;

    case FENDER_65_SPRING_REVERB:
        model = 0x0b;
        break;
    }

    position = settings.put_post_amp ? (settings.fx_slot+4) : (settings.fx_slot);

    xml->writeStartElement("Module");
    xml->writeAttribute("ID", QString("%1").arg(model));
    xml->writeAttribute("POS", QString("%1").arg(position));
    xml->writeAttribute("BypassState", "1");

    if(model == EMPTY)
    {
        xml->writeCharacters("");
        xml->writeEndElement();  // end Module
        return;
    }

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "0");
    xml->writeCharacters(QString("%1").arg((settings.knob1<<8)|settings.knob1));
    xml->writeEndElement();

    if(settings.effect_num == SIMPLE_COMP)
    {
        xml->writeEndElement();  // end Module
        return;
    }

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "1");
    xml->writeCharacters(QString("%1").arg((settings.knob2<<8)|settings.knob2));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "2");
    xml->writeCharacters(QString("%1").arg((settings.knob3<<8)|settings.knob3));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "3");
    xml->writeCharacters(QString("%1").arg((settings.knob4<<8)|settings.knob4));
    xml->writeEndElement();

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "4");
    xml->writeCharacters(QString("%1").arg((settings.knob5<<8)|settings.knob5));
    xml->writeEndElement();

    if(settings.effect_num != MONO_ECHO_FILTER &&
            settings.effect_num != STEREO_ECHO_FILTER &&
            settings.effect_num != TAPE_DELAY &&
            settings.effect_num != STEREO_TAPE_DELAY)
    {
        xml->writeEndElement();  // end Module
        return;
    }

    xml->writeStartElement("Param");
    xml->writeAttribute("ControlIndex", "5");
    xml->writeCharacters(QString("%1").arg((settings.knob6<<8)|settings.knob6));
    xml->writeEndElement();

    xml->writeEndElement();  // end Module
}

void SaveToFile::writeFUSE()
{
    xml->writeStartElement("FUSE");
    xml->writeStartElement("Info");
    xml->writeAttribute("name", ui->lineEdit_2->text());
    xml->writeAttribute("author", ui->lineEdit_3->text());
    xml->writeAttribute("rating", "0");
    xml->writeAttribute("genre1", "-1");
    xml->writeAttribute("genre2", "-1");
    xml->writeAttribute("genre3", "-1");
    xml->writeAttribute("tags", "");
    xml->writeAttribute("fenderid", "0");
    xml->writeCharacters("");
    xml->writeEndElement();  // end Info
    xml->writeEndElement();  // end FUSE
}
