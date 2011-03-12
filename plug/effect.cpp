#include "effect.h"
#include "ui_effect.h"

#include "mainwindow.h"

Effect::Effect(QWidget *parent, int number) :
    QMainWindow(parent),
    ui(new Ui::Effect)
{
    ui->setupUi(this);

    // initialize variables
    effect_num = ui->comboBox->currentIndex();
    fx_slot = number;
    put_post_amp = false;
    knob1 = 0;
    knob2 = 0;
    knob3 = 0;
    knob4 = 0;
    knob5 = 0;
    knob6 = 0;
    changed = false;

    // load window size
    QSettings settings;
    restoreGeometry(settings.value(QString("Windows/Effect%1WindowGeometry").arg(fx_slot)).toByteArray());

    // set window title
    setWindowTitle(QString(tr("Effect %1")).arg(fx_slot+1));

    // connect elements to slots
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_fx(int)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(set_post_amp(bool)));
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_knob1(int)));
    connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_knob2(int)));
    connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_knob3(int)));
    connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_knob4(int)));
    connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_knob5(int)));
    connect(ui->dial_6, SIGNAL(valueChanged(int)), this, SLOT(set_knob6(int)));
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(send_fx()));
    connect(ui->pushButton, SIGNAL(toggled(bool)), this, SLOT(off_switch(bool)));

    QShortcut *close = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(close, SIGNAL(activated()), this, SLOT(close()));

    QShortcut *off = new QShortcut(QKeySequence(QString("F%1").arg(fx_slot+1)), this, 0, 0, Qt::ApplicationShortcut);
    connect(off, SIGNAL(activated()), ui->pushButton, SLOT(toggle()));
}

Effect::~Effect()
{
    QSettings settings;
    settings.setValue(QString("Windows/Effect%1WindowGeometry").arg(fx_slot), saveGeometry());
    delete ui;
}

// functions setting variables
void Effect::set_post_amp(bool value)
{
    put_post_amp = value;
    set_changed(true);
}

void Effect::set_knob1(int value)
{
    knob1 = value;
    set_changed(true);
}

void Effect::set_knob2(int value)
{
    knob2 = value;
    set_changed(true);
}

void Effect::set_knob3(int value)
{
    knob3 = value;
    set_changed(true);
}

void Effect::set_knob4(int value)
{
    knob4 = value;
    set_changed(true);
}

void Effect::set_knob5(int value)
{
    knob5 = value;
    set_changed(true);
}

void Effect::set_knob6(int value)
{
    knob6 = value;
    set_changed(true);
}

void Effect::choose_fx(int value)
{
    QSettings settings;
    effect_num = value;
    set_changed(true);

    // activate proper knobs and set their max values
    switch (value)
    {
    case EMPTY:
        ui->checkBox->setDisabled(true);
        if(sender() == ui->comboBox)
        {
            ui->dial->setValue(0);
            ui->dial_2->setValue(0);
            ui->dial_3->setValue(0);
            ui->dial_4->setValue(0);
            ui->dial_5->setValue(0);
            ui->dial_6->setValue(0);
        }
        ui->dial->setDisabled(true);
        ui->dial_2->setDisabled(true);
        ui->dial_3->setDisabled(true);
        ui->dial_4->setDisabled(true);
        ui->dial_5->setDisabled(true);
        ui->dial_6->setDisabled(true);
        ui->spinBox->setDisabled(true);
        ui->spinBox_2->setDisabled(true);
        ui->spinBox_3->setDisabled(true);
        ui->spinBox_4->setDisabled(true);
        ui->spinBox_5->setDisabled(true);
        ui->spinBox_6->setDisabled(true);
        break;

    case SIMPLE_COMP:
        ui->checkBox->setDisabled(false);
        ui->dial->setMaximum(3);
        ui->spinBox->setMaximum(3);
        ui->dial_2->setValue(0);
        ui->dial_3->setValue(0);
        ui->dial_4->setValue(0);
        ui->dial_5->setValue(0);
        ui->dial_6->setValue(0);
        ui->dial->setDisabled(false);
        ui->dial_2->setDisabled(true);
        ui->dial_3->setDisabled(true);
        ui->dial_4->setDisabled(true);
        ui->dial_5->setDisabled(true);
        ui->dial_6->setDisabled(true);
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(true);
        ui->spinBox_3->setDisabled(true);
        ui->spinBox_4->setDisabled(true);
        ui->spinBox_5->setDisabled(true);
        ui->spinBox_6->setDisabled(true);
        break;

    case RING_MODULATOR:
        ui->checkBox->setDisabled(false);
        ui->dial->setMaximum(255);
        ui->spinBox->setMaximum(255);
        ui->dial_4->setMaximum(1);
        ui->spinBox_4->setMaximum(1);
        ui->dial_5->setMaximum(255);
        ui->spinBox_5->setMaximum(255);
        ui->dial_6->setValue(0);
        ui->dial->setDisabled(false);
        ui->dial_2->setDisabled(false);
        ui->dial_3->setDisabled(false);
        ui->dial_4->setDisabled(false);
        ui->dial_5->setDisabled(false);
        ui->dial_6->setDisabled(true);
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->spinBox_6->setDisabled(true);
        break;

    case PHASER:
        ui->checkBox->setDisabled(false);
        ui->dial->setMaximum(255);
        ui->spinBox->setMaximum(255);
        ui->dial_4->setMaximum(255);
        ui->spinBox_4->setMaximum(255);
        ui->dial_5->setMaximum(1);
        ui->spinBox_5->setMaximum(1);
        ui->dial_6->setValue(0);
        ui->dial->setDisabled(false);
        ui->dial_2->setDisabled(false);
        ui->dial_3->setDisabled(false);
        ui->dial_4->setDisabled(false);
        ui->dial_5->setDisabled(false);
        ui->dial_6->setDisabled(true);
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->spinBox_6->setDisabled(true);
        break;

    case MONO_ECHO_FILTER:
    case STEREO_ECHO_FILTER:
    case TAPE_DELAY:
    case STEREO_TAPE_DELAY:
        ui->checkBox->setDisabled(false);
        ui->dial->setMaximum(255);
        ui->spinBox->setMaximum(255);
        ui->dial_4->setMaximum(255);
        ui->spinBox_4->setMaximum(255);
        ui->dial_5->setMaximum(255);
        ui->spinBox_5->setMaximum(255);
        ui->dial->setDisabled(false);
        ui->dial_2->setDisabled(false);
        ui->dial_3->setDisabled(false);
        ui->dial_4->setDisabled(false);
        ui->dial_5->setDisabled(false);
        ui->dial_6->setDisabled(false);
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->spinBox_6->setDisabled(false);
        break;

    default:
        ui->checkBox->setDisabled(false);
        ui->dial->setMaximum(255);
        ui->spinBox->setMaximum(255);
        ui->dial_4->setMaximum(255);
        ui->spinBox_4->setMaximum(255);
        ui->dial_5->setMaximum(255);
        ui->spinBox_5->setMaximum(255);
        ui->dial_6->setValue(0);
        ui->dial->setDisabled(false);
        ui->dial_2->setDisabled(false);
        ui->dial_3->setDisabled(false);
        ui->dial_4->setDisabled(false);
        ui->dial_5->setDisabled(false);
        ui->dial_6->setDisabled(true);
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->spinBox_6->setDisabled(true);
        break;
    }

    // set knobs labels
    switch (value)
    {
    case EMPTY:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case OVERDRIVE:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Gain"));
        ui->label_3->setText(tr("L&ow"));
        ui->label_4->setText(tr("&Medium"));
        ui->label_5->setText(tr("&High"));
        ui->label_6->setText(tr(""));
        break;

    case FIXED_WAH:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Frequency"));
        ui->label_3->setText(tr("Mi&n Freq"));
        ui->label_4->setText(tr("Ma&x Freq"));
        ui->label_5->setText(tr("&Q"));
        ui->label_6->setText(tr(""));
        break;

    case TOUCH_WAH:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Sensivity"));
        ui->label_3->setText(tr("Mi&n Freq"));
        ui->label_4->setText(tr("Ma&x Freq"));
        ui->label_5->setText(tr("&Q"));
        ui->label_6->setText(tr(""));
        break;

    case FUZZ:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Gain"));
        ui->label_3->setText(tr("&Octave"));
        ui->label_4->setText(tr("L&ow"));
        ui->label_5->setText(tr("&High"));
        ui->label_6->setText(tr(""));
        break;

    case FUZZ_TOUCH_WAH:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Gain"));
        ui->label_3->setText(tr("&Sensivity"));
        ui->label_4->setText(tr("&Octave"));
        ui->label_5->setText(tr("&Peak"));
        ui->label_6->setText(tr(""));
        break;

    case SIMPLE_COMP:
        ui->label->setText(tr("&Type"));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case COMPRESSOR:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Threshold"));
        ui->label_3->setText(tr("&Ratio"));
        ui->label_4->setText(tr("Atta&ck"));
        ui->label_5->setText(tr("&Release"));
        ui->label_6->setText(tr(""));
        break;

    case SINE_CHORUS:
    case TRIANGLE_CHORUS:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("&Depth"));
        ui->label_4->setText(tr("A&vr Delay"));
        ui->label_5->setText(tr("LR &Phase"));
        ui->label_6->setText(tr(""));
        break;

    case SINE_FLANGER:
    case TRIANGLE_FLANGER:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("&Depth"));
        ui->label_4->setText(tr("&Feedback"));
        ui->label_5->setText(tr("LR &Phase"));
        ui->label_6->setText(tr(""));
        break;

    case VIBRATONE:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rotor"));
        ui->label_3->setText(tr("&Depth"));
        ui->label_4->setText(tr("&Feedback"));
        ui->label_5->setText(tr("LR &Phase"));
        ui->label_6->setText(tr(""));
        break;

    case VINTAGE_TREMOLO:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("&Duty Cycle"));
        ui->label_4->setText(tr("Atta&ck"));
        ui->label_5->setText(tr("Relea&se"));
        ui->label_6->setText(tr(""));
        break;

    case SINE_TREMOLO:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("&Duty Cycle"));
        ui->label_4->setText(tr("LFO &Clipping"));
        ui->label_5->setText(tr("&Tri Shaping"));
        ui->label_6->setText(tr(""));
        break;

    case RING_MODULATOR:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Frequency"));
        ui->label_3->setText(tr("&Depth"));
        ui->label_4->setText(tr("LFO &Shape"));
        ui->label_5->setText(tr("LFO &Phase"));
        ui->label_6->setText(tr(""));
        break;

    case STEP_FILTER:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("Re&sonance"));
        ui->label_4->setText(tr("Mi&n Freq"));
        ui->label_5->setText(tr("Ma&x Freq"));
        ui->label_6->setText(tr(""));
        break;

    case PHASER:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Rate"));
        ui->label_3->setText(tr("&Depth"));
        ui->label_4->setText(tr("&Feedback"));
        ui->label_5->setText(tr("LFO &Shape"));
        ui->label_6->setText(tr(""));
        break;

    case PITCH_SHIFTER:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Pitch"));
        ui->label_3->setText(tr("&Detune"));
        ui->label_4->setText(tr("&Feedback"));
        ui->label_5->setText(tr("P&redelay"));
        ui->label_6->setText(tr(""));
        break;

    case MONO_ECHO_FILTER:
    case STEREO_ECHO_FILTER:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Delay"));
        ui->label_3->setText(tr("&Feedback"));
        ui->label_4->setText(tr("Fre&quency"));
        ui->label_5->setText(tr("&Ressonance"));
        ui->label_6->setText(tr("&In Level"));
        break;

    case MONO_DELAY:
    case MULTITAP_DELAY:
    case REVERSE_DELAY:
    case PING_PONG_DELAY:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Delay"));
        ui->label_3->setText(tr("&Feedback"));
        ui->label_4->setText(tr("&Brightness"));
        ui->label_5->setText(tr("A&ttenuation"));
        ui->label_6->setText(tr(""));
        break;

    case DUCKING_DELAY:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Delay"));
        ui->label_3->setText(tr("&Feedback"));
        ui->label_4->setText(tr("&Release"));
        ui->label_5->setText(tr("&Threshold"));
        ui->label_6->setText(tr(""));
        break;

    case TAPE_DELAY:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Delay"));
        ui->label_3->setText(tr("&Feedback"));
        ui->label_4->setText(tr("Fl&utter"));
        ui->label_5->setText(tr("&Brightness"));
        ui->label_6->setText(tr("&Stereo"));
        break;

    case STEREO_TAPE_DELAY:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Delay"));
        ui->label_3->setText(tr("&Feedback"));
        ui->label_4->setText(tr("Fl&utter"));
        ui->label_5->setText(tr("&Separation"));
        ui->label_6->setText(tr("&Brightness"));
        break;

    case SMALL_HALL_REVERB:
    case LARGE_HALL_REVERB:
    case SMALL_ROOM_REVERB:
    case LARGE_ROOM_REVERB:
    case SMALL_PLATE_REVERB:
    case LARGE_PLATE_REVERB:
    case AMBIENT_REVERB:
    case ARENA_REVERB:
    case FENDER_63_SPRING_REVERB:
    case FENDER_65_SPRING_REVERB:
        ui->label->setText(tr("&Level"));
        ui->label_2->setText(tr("&Decay"));
        ui->label_3->setText(tr("D&well"));
        ui->label_4->setText(tr("D&iffusion"));
        ui->label_5->setText(tr("&Tone"));
        ui->label_6->setText(tr(""));
        break;
    }

    if(settings.value("Settings/defaultEffectValues").toBool())
    {
        switch (value)
        {
        case EMPTY:
            break;

        case OVERDRIVE:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case FIXED_WAH:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case TOUCH_WAH:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case FUZZ:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case FUZZ_TOUCH_WAH:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x80);
            break;

        case SIMPLE_COMP:
            ui->dial->setValue(0x01);
            ui->dial_2->setValue(0x00);
            ui->dial_3->setValue(0x00);
            ui->dial_4->setValue(0x00);
            ui->dial_5->setValue(0x00);
            ui->dial_6->setValue(0x00);
            break;

        case COMPRESSOR:
            ui->dial->setValue(0x8d);
            ui->dial_2->setValue(0x0f);
            ui->dial_3->setValue(0x4f);
            ui->dial_4->setValue(0x7f);
            ui->dial_5->setValue(0x7f);
            ui->dial_6->setValue(0x00);
            break;


        case SINE_CHORUS:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x0e);
            ui->dial_3->setValue(0x19);
            ui->dial_4->setValue(0x19);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case TRIANGLE_CHORUS:
            ui->dial->setValue(0x5d);
            ui->dial_2->setValue(0x0e);
            ui->dial_3->setValue(0x19);
            ui->dial_4->setValue(0x19);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case SINE_FLANGER:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x0e);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case TRIANGLE_FLANGER:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x00);
            ui->dial_3->setValue(0xff);
            ui->dial_4->setValue(0x33);
            ui->dial_5->setValue(0x41);
            ui->dial_6->setValue(0x00);
            break;

        case VIBRATONE:
            ui->dial->setValue(0xf4);
            ui->dial_2->setValue(0xff);
            ui->dial_3->setValue(0x27);
            ui->dial_4->setValue(0xad);
            ui->dial_5->setValue(0x82);
            ui->dial_6->setValue(0x00);
            break;

        case VINTAGE_TREMOLO:
            ui->dial->setValue(0xdb);
            ui->dial_2->setValue(0xad);
            ui->dial_3->setValue(0x63);
            ui->dial_4->setValue(0xf4);
            ui->dial_5->setValue(0xf1);
            ui->dial_6->setValue(0x00);
            break;

        case SINE_TREMOLO:
            ui->dial->setValue(0xdb);
            ui->dial_2->setValue(0x99);
            ui->dial_3->setValue(0x7d);
            ui->dial_4->setValue(0x00);
            ui->dial_5->setValue(0x00);
            ui->dial_6->setValue(0x00);
            break;

        case RING_MODULATOR:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case STEP_FILTER:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case PHASER:
            ui->dial->setValue(0xfd);
            ui->dial_2->setValue(0x00);
            ui->dial_3->setValue(0xfd);
            ui->dial_4->setValue(0xb8);
            ui->dial_5->setValue(0x00);
            ui->dial_6->setValue(0x00);
            break;

        case PITCH_SHIFTER:
            ui->dial->setValue(0xc7);
            ui->dial_2->setValue(0x3e);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x00);
            ui->dial_5->setValue(0x00);
            ui->dial_6->setValue(0x00);
            break;


        case MONO_ECHO_FILTER:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x80);
            break;

        case STEREO_ECHO_FILTER:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0xb3);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x80);
            break;

        case MONO_DELAY:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case MULTITAP_DELAY:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x66);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case REVERSE_DELAY:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case PING_PONG_DELAY:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case TAPE_DELAY:
            ui->dial->setValue(0x7d);
            ui->dial_2->setValue(0x1c);
            ui->dial_3->setValue(0x00);
            ui->dial_4->setValue(0x63);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case STEREO_TAPE_DELAY:
            ui->dial->setValue(0x7d);
            ui->dial_2->setValue(0x88);
            ui->dial_3->setValue(0x1c);
            ui->dial_4->setValue(0x63);
            ui->dial_5->setValue(0xff);
            ui->dial_6->setValue(0x80);
            break;

        case DUCKING_DELAY:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;


        case SMALL_HALL_REVERB:
            ui->dial->setValue(0x6e);
            ui->dial_2->setValue(0x5d);
            ui->dial_3->setValue(0x6e);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x91);
            ui->dial_6->setValue(0x00);
            break;

        case LARGE_HALL_REVERB:
            ui->dial->setValue(0x4f);
            ui->dial_2->setValue(0x3e);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x05);
            ui->dial_5->setValue(0xb0);
            ui->dial_6->setValue(0x00);
            break;

        case SMALL_ROOM_REVERB:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case LARGE_ROOM_REVERB:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case SMALL_PLATE_REVERB:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case LARGE_PLATE_REVERB:
            ui->dial->setValue(0x38);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x91);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0xb6);
            ui->dial_6->setValue(0x00);
            break;

        case AMBIENT_REVERB:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case ARENA_REVERB:
            ui->dial->setValue(0xff);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;

        case FENDER_63_SPRING_REVERB:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x80);
            ui->dial_3->setValue(0x80);
            ui->dial_4->setValue(0x80);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0);
            break;

        case FENDER_65_SPRING_REVERB:
            ui->dial->setValue(0x80);
            ui->dial_2->setValue(0x8b);
            ui->dial_3->setValue(0x49);
            ui->dial_4->setValue(0xff);
            ui->dial_5->setValue(0x80);
            ui->dial_6->setValue(0x00);
            break;
        }
    }
}

// send settings to the amplifier
void Effect::send_fx()
{
    struct fx_pedal_settings pedal;

    if(!get_changed())
        return;
    set_changed(false);

    pedal.effect_num = effect_num;
    pedal.fx_slot = fx_slot;
    pedal.put_post_amp = put_post_amp;
    pedal.knob1 = knob1;
    pedal.knob2 = knob2;
    pedal.knob3 = knob3;
    pedal.knob4 = knob4;
    pedal.knob5 = knob5;
    pedal.knob6 = knob6;

    ((MainWindow*)parent())->set_effect(pedal);
}

void Effect::load(struct fx_pedal_settings settings)
{
    set_changed(true);

    ui->comboBox->setCurrentIndex(settings.effect_num);
    ui->dial->setValue(settings.knob1);
    ui->dial_2->setValue(settings.knob2);
    ui->dial_3->setValue(settings.knob3);
    ui->dial_4->setValue(settings.knob4);
    ui->dial_5->setValue(settings.knob5);
    ui->dial_6->setValue(settings.knob6);
    ui->checkBox->setChecked(settings.put_post_amp);
}

void Effect::get_settings(struct fx_pedal_settings &pedal)
{
    pedal.effect_num = effect_num;
    pedal.fx_slot = fx_slot;
    pedal.put_post_amp = put_post_amp;
    pedal.knob1 = knob1;
    pedal.knob2 = knob2;
    pedal.knob3 = knob3;
    pedal.knob4 = knob4;
    pedal.knob5 = knob5;
    pedal.knob6 = knob6;
}

void Effect::off_switch(bool value)
{
    if(value)
    {
        ui->pushButton->setText(tr("On"));
        ui->comboBox->setDisabled(true);
        ui->setButton->setDisabled(true);
        // all other are disabled by setting effect to EMPTY
        choose_fx(EMPTY);
    }
    else
    {
        ui->pushButton->setText(tr("Off"));
        ui->comboBox->setDisabled(false);
        ui->setButton->setDisabled(false);
        choose_fx(ui->comboBox->currentIndex());
        activateWindow();
    }
    set_changed(true);
    send_fx();
}

void Effect::set_changed(bool value)
{
    changed = value;
}

bool Effect::get_changed()
{
    return changed;
}

void Effect::enable_set_button(bool value)
{
    ui->setButton->setEnabled(value);
}
