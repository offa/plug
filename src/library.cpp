#include "library.h"
#include "ui_library.h"

#include "mainwindow.h"

Library::Library(char names[100][32], QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Library)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value("Windows/libraryWindowGeometry").toByteArray());

    files = new QList<QFileInfo>();

    if(settings.contains("Library/lastDirectory"))
    {
        ui->label_3->setText(settings.value("Library/lastDirectory").toString());
        get_files(settings.value("Library/lastDirectory").toString());
    }

    QFont font(settings.value("Library/FontFamily", ui->listWidget->font().family()).toString(), settings.value("Library/FontSize", ui->listWidget->font().pointSize()).toInt());
    ui->listWidget->setFont(font);
    ui->listWidget_2->setFont(font);

    ui->spinBox->setValue(font.pointSize());
    ui->fontComboBox->setCurrentFont(font);

    for(int i = 0; i < 100; i++)
    {
        if(names[i][0] == 0x00)
            break;
        ui->listWidget->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    }

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(load_slot(int)));
    connect(ui->listWidget_2, SIGNAL(currentRowChanged(int)), this, SLOT(load_file(int)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(get_directory()));
    connect(this, SIGNAL(directory_changed(QString)), ui->label_3, SLOT(setText(QString)));
    connect(this, SIGNAL(directory_changed(QString)), this, SLOT(get_files(QString)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(change_font_size(int)));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(change_font_family(QFont)));
}

Library::~Library()
{
    QSettings settings;
    settings.setValue("Windows/libraryWindowGeometry", saveGeometry());
    delete files;
    delete ui;
}

void Library::load_slot(int slot)
{
    if(slot < 0)
        return;

    ui->listWidget_2->setCurrentRow(-1);
    dynamic_cast<MainWindow*>(parent())->load_from_amp(slot);
}

void Library::get_directory()
{
    QSettings settings;

    QString directory = QFileDialog::getExistingDirectory(this, QString(tr("Choose directory")), settings.value("Library/lastDirectory", QString(QDir::homePath())).toString());

    if(directory.isEmpty())
        return;

    settings.setValue("Library/lastDirectory", directory);
    emit directory_changed(directory);
}

void Library::get_files(QString path)
{
    QDir directory(path, "*.fuse", (QDir::Name | QDir::IgnoreCase), (QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));

    if(!files->isEmpty())
        files->clear();
    ui->listWidget_2->clear();
    *files = directory.entryInfoList(QDir::Files|QDir::NoDotAndDotDot|QDir::Readable);
    for(int i = 0; i < files->size(); i++)
        ui->listWidget_2->addItem((*files)[i].completeBaseName());
}

void Library::load_file(int row)
{
    if(row < 0)
        return;

    ui->listWidget->setCurrentRow(-1);
    dynamic_cast<MainWindow*>(parent())->loadfile((*files)[row].canonicalFilePath());
}

void Library::resizeEvent(QResizeEvent *event)
{
    ui->label_3->setMaximumWidth((event->size().width()/2)-ui->pushButton->size().width());
}

void Library::change_font_size(int value)
{
    QSettings settings;
    QFont font(ui->listWidget_2->font());

    font.setPointSize(value);
    ui->listWidget->setFont(font);
    ui->listWidget_2->setFont(font);

    settings.setValue("Library/FontSize", value);
}

void Library::change_font_family(QFont font)
{
    QSettings settings;

    font.setPointSize(ui->spinBox->value());
    ui->listWidget->setFont(font);
    ui->listWidget_2->setFont(font);

    settings.setValue("Library/FontFamily", font.family());
}
