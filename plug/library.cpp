#include "library.h"
#include "ui_library.h"

#include "mainwindow.h"

Library::Library(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Library)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value("Windows/libraryWindowGeometry").toByteArray());

    files = new QList<QFileInfo>();
    ui->spinBox->setValue(ui->listWidget->font().pointSize());
    ui->fontComboBox->setCurrentFont(ui->listWidget->font());

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

void Library::get_names(char names[24][32])
{
    for(int i = 0; i < 24; i++)
    {
        ui->listWidget->addItem(QString("%1").arg(names[i]));
    }
}

void Library::load_slot(int slot)
{
    if(slot < 0)
        return;
    ui->listWidget_2->setCurrentRow(-1);
    ((MainWindow*)parent())->load_from_amp(slot);
}

void Library::get_directory()
{
    QString directory = QFileDialog::getExistingDirectory(this, QString(tr("Choose directory")), QDir::homePath());
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
    ((MainWindow*)parent())->loadfile((*files)[row].canonicalFilePath());
}

void Library::resizeEvent(QResizeEvent *event)
{
    ui->label_3->setMaximumWidth((event->size().width()/2)-ui->pushButton->size().width());
}

void Library::change_font_size(int value)
{
    QFont font(ui->listWidget_2->font());
    font.setPointSize(value);
    ui->listWidget->setFont(font);
    ui->listWidget_2->setFont(font);
}

void Library::change_font_family(QFont font)
{
    font.setPointSize(ui->spinBox->value());
    ui->listWidget->setFont(font);
    ui->listWidget_2->setFont(font);
}
