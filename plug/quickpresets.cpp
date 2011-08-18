#include "quickpresets.h"
#include "ui_quickpresets.h"

QuickPresets::QuickPresets(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QuickPresets)
{
  ui->setupUi(this);

  connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset0(int)));
  connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset1(int)));
  connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset2(int)));
  connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset3(int)));
  connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset4(int)));
  connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset5(int)));
  connect(ui->comboBox_7, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset6(int)));
  connect(ui->comboBox_8, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset7(int)));
  connect(ui->comboBox_9, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset8(int)));
  connect(ui->comboBox_10, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultPreset9(int)));
}

QuickPresets::~QuickPresets()
{
  delete ui;
}

void QuickPresets::load_names(char names[][32])
{
  for(int i = 0; i < 100; i++)
  {
    if(names[i][0] == 0x00)
      break;
    ui->comboBox->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_2->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_3->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_4->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_5->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_6->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_7->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_8->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_9->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    ui->comboBox_10->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
  }
}

void QuickPresets::delete_items()
{
  int j = ui->comboBox->count();
  for(int i = 0; i < j; i++)
  {
    ui->comboBox->removeItem(0);
    ui->comboBox_2->removeItem(0);
    ui->comboBox_3->removeItem(0);
    ui->comboBox_4->removeItem(0);
    ui->comboBox_5->removeItem(0);
    ui->comboBox_6->removeItem(0);
    ui->comboBox_7->removeItem(0);
    ui->comboBox_8->removeItem(0);
    ui->comboBox_9->removeItem(0);
    ui->comboBox_10->removeItem(0);
  }
}

void QuickPresets::change_name(int slot, QString *name)
{
  ui->comboBox->setItemText(slot, *name);
  ui->comboBox->setCurrentIndex(slot);

  ui->comboBox_2->setItemText(slot, *name);
  ui->comboBox_2->setCurrentIndex(slot);

  ui->comboBox_3->setItemText(slot, *name);
  ui->comboBox_3->setCurrentIndex(slot);

  ui->comboBox_4->setItemText(slot, *name);
  ui->comboBox_4->setCurrentIndex(slot);

  ui->comboBox_5->setItemText(slot, *name);
  ui->comboBox_5->setCurrentIndex(slot);

  ui->comboBox_6->setItemText(slot, *name);
  ui->comboBox_6->setCurrentIndex(slot);

  ui->comboBox_7->setItemText(slot, *name);
  ui->comboBox_7->setCurrentIndex(slot);

  ui->comboBox_8->setItemText(slot, *name);
  ui->comboBox_8->setCurrentIndex(slot);

  ui->comboBox_9->setItemText(slot, *name);
  ui->comboBox_9->setCurrentIndex(slot);

  ui->comboBox_10->setItemText(slot, *name);
  ui->comboBox_10->setCurrentIndex(slot);
}

void QuickPresets::setDefaultPreset0(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset0", slot);
}

void QuickPresets::setDefaultPreset1(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset1", slot);
}

void QuickPresets::setDefaultPreset2(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset2", slot);
}

void QuickPresets::setDefaultPreset3(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset3", slot);
}

void QuickPresets::setDefaultPreset4(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset4", slot);
}

void QuickPresets::setDefaultPreset5(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset5", slot);
}

void QuickPresets::setDefaultPreset6(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset6", slot);
}

void QuickPresets::setDefaultPreset7(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset7", slot);
}

void QuickPresets::setDefaultPreset8(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset8", slot);
}

void QuickPresets::setDefaultPreset9(int slot)
{
  QSettings settings;
  settings.setValue("DefaultPresets/Preset9", slot);
}

void QuickPresets::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}
