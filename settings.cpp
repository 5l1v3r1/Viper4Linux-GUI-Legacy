#include "settings.h"
#include "ui_settings.h"
#include "main.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QDialog>
#include <unistd.h>
#include <pwd.h>
#include <QCloseEvent>
#include <cstdio>
#include <cstdlib>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>
#include <QStyleFactory>
#include "palette.h"

using namespace std;
static bool lockslot = false;
settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings){
    ui->setupUi(this);

    lockslot = true;
    connect(ui->styleSelect,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changeStyle(const QString&)));
    connect(ui->paletteSelect,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changePalette(const QString&)));
connect(ui->paletteConfig,SIGNAL(clicked()),this,SLOT(openPalConfig()));
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    char result[100];
    strcpy(result,homedir);
    strcat(result,"/.config/viper4linux/audio.conf");

    string path = mainwin->getPath();
    string style_sheet = mainwin->getStylesheet();
    int thememode = mainwin->getThememode();
    string palette = mainwin->getColorpalette();

    if(path.empty()) ui->path->setText(QString::fromUtf8(result));
    else ui->path->setText(QString::fromStdString(path));
    ui->autofx->setChecked(mainwin->getAutoFx());
    ui->muteonrestart->setChecked(mainwin->getMuteOnRestart());
    ui->glavafix->setChecked(mainwin->getGFix());
    connect(ui->save, SIGNAL(clicked()), this, SLOT(submit()));
    connect(ui->github, SIGNAL(clicked()), this, SLOT(github()));
    connect(ui->glavafix_help, SIGNAL(clicked()), this, SLOT(glava_help()));
    connect(ui->uimode_css, SIGNAL(clicked()), this, SLOT(changeThemeMode()));
    connect(ui->uimode_pal, SIGNAL(clicked()), this, SLOT(changeThemeMode()));

    ui->styleSelect->addItem("Default","default");
    ui->styleSelect->addItem("Black","amoled");
    ui->styleSelect->addItem("Blue","blue");
    ui->styleSelect->addItem("Breeze Light","breeze_light");
    ui->styleSelect->addItem("Breeze Dark","breeze_dark");
    ui->styleSelect->addItem("Gray","dark_orange");
    ui->styleSelect->addItem("MacOS","aqua");
    ui->styleSelect->addItem("Material Dark","materialdark");
    ui->styleSelect->addItem("Ubuntu","ubuntu");
    ui->styleSelect->addItem("Visual Studio Dark","vsdark");
    ui->styleSelect->addItem("Visual Studio Light","vslight");

    ui->paletteSelect->addItem("Default","default");
    ui->paletteSelect->addItem("Dark","dark");
    ui->paletteSelect->addItem("Blue","blue");
    ui->paletteSelect->addItem("Purple","purple");
    ui->paletteSelect->addItem("Gray","gray");
    ui->paletteSelect->addItem("White","white");
    ui->paletteSelect->addItem("Custom","custom");

    QVariant qvS(QString::fromStdString(style_sheet));
    int index = ui->styleSelect->findData(qvS);
    if ( index != -1 ) {
       ui->styleSelect->setCurrentIndex(index);
    }

    QVariant qvS2(QString::fromStdString(palette));
    int index2 = ui->paletteSelect->findData(qvS2);
    if ( index2 != -1 ) {
       ui->paletteSelect->setCurrentIndex(index2);
    }

    ui->styleSelect->setEnabled(!thememode);
    ui->paletteConfig->setEnabled(thememode && palette=="custom");
    ui->paletteSelect->setEnabled(thememode);

    ui->uimode_css->setChecked(!thememode);//If 0 set true, else false
    ui->uimode_pal->setChecked(thememode);//If 0 set false, else true

    lockslot = false;
}
settings::~settings(){
    delete ui;
}
void settings::submit(){
    mainwin->setGFix(ui->glavafix->isChecked());
    mainwin->setPath(ui->path->text().toUtf8().constData());
    mainwin->setAutoFx(ui->autofx->isChecked());
    mainwin->setMuteOnRestart(ui->muteonrestart->isChecked());
    this->close();
}
void settings::changeThemeMode(){
    if(lockslot)return;

    int mode = 0;
    if(ui->uimode_css->isChecked())mode=0;
    else if(ui->uimode_pal->isChecked())mode=1;

    ui->styleSelect->setEnabled(!mode);
    ui->paletteSelect->setEnabled(mode);
    ui->paletteConfig->setEnabled(mode && mainwin->getColorpalette()=="custom");
    mainwin->setThememode(mode);
}
void settings::changePalette(const QString&){
    if(lockslot)return;
    mainwin->setColorpalette(ui->paletteSelect->itemData(ui->paletteSelect->currentIndex()).toString().toUtf8().constData());
    ui->paletteConfig->setEnabled(mainwin->getColorpalette()=="custom");
}
void settings::openPalConfig(){
    auto c = new class palette(this);
    c->setFixedSize(c->geometry().width(),c->geometry().height());
    c->show();
}
void settings::changeStyle(const QString& style){
    mainwin->setStylesheet(ui->styleSelect->itemData(ui->styleSelect->currentIndex()).toString().toUtf8().constData());
}
void settings::github(){
    QDesktopServices::openUrl(QUrl("https://github.com/ThePBone/Viper4Linux-GUI"));
}
void settings::reject()
{
    mainwin->enableSetBtn(true);
    QDialog::reject();
}
void settings::glava_help(){
    QMessageBox *msgBox = new QMessageBox(this);
     msgBox->setText("This fix kills GLava (desktop visualizer) and restarts it after a new config has been applied.\nThis prevents GLava to switch to another audio sink, while V4L is restarting.");
     msgBox->setStandardButtons(QMessageBox::Ok);
     msgBox->setDefaultButton(QMessageBox::Ok);
     msgBox->exec();
}
