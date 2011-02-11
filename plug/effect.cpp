#include "effect.h"
#include "ui_effect.h"

#include "mainwindow.h"

Effect::Effect(QWidget *parent, int number) :
    QMainWindow(parent),
    ui(new Ui::Effect)
{
    ui->setupUi(this);

    // load window size
    QSettings settings;
    restoreGeometry(settings.value(QString("Effect %1 WindowGeometry").arg(fx_slot)).toByteArray());

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
}

Effect::~Effect()
{
    QSettings settings;
    settings.setValue(QString("Effect %1 WindowGeometry").arg(fx_slot), saveGeometry());
    delete ui;
}

// functions setting variables
void Effect::set_post_amp(bool value)
{
    put_post_amp = value;
}

void Effect::set_knob1(int value)
{
    knob1 = value;
}

void Effect::set_knob2(int value)
{
    knob2 = value;
}

void Effect::set_knob3(int value)
{
    knob3 = value;
}

void Effect::set_knob4(int value)
{
    knob4 = value;
}

void Effect::set_knob5(int value)
{
    knob5 = value;
}

void Effect::set_knob6(int value)
{
    knob6 = value;
}

void Effect::choose_fx(int value)
{
    effect_num = value;

    // activate proper knobs and set their max values
    switch (value)
    {
    case EMPTY:
        ui->checkBox->setDisabled(true);
        ui->dial->setValue(0);
        ui->dial_2->setValue(0);
        ui->dial_3->setValue(0);
        ui->dial_4->setValue(0);
        ui->dial_5->setValue(0);
        ui->dial_6->setValue(0);
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
}

// send settings to the amplifier
void Effect::send_fx()
{
    struct fx_pedal_settings pedal;

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
    ui->comboBox->setCurrentIndex(settings.effect_num);
    ui->dial->setValue(settings.knob1);
    ui->dial_2->setValue(settings.knob2);
    ui->dial_3->setValue(settings.knob3);
    ui->dial_4->setValue(settings.knob4);
    ui->dial_5->setValue(settings.knob5);
    ui->dial_6->setValue(settings.knob6);
    ui->checkBox->setChecked(settings.put_post_amp);
}
