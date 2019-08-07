#include "convolver.h"
#include "ui_convolver.h"
#include "main.h"
#include "irs/inspect.h"
#include <QDir>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <QString>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
static bool lockupdate = false;
static QString configpath = "";
using namespace std;
Convolver::Convolver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Convolver)
{
    ui->setupUi(this);

    QDir d = QFileInfo(QString::fromStdString(mainwin->getPath())).absoluteDir();
    QString absolute=d.absolutePath();
    configpath = QDir::cleanPath(absolute + QDir::separator());

    ui->path->setText(QString::fromStdString(mainwin->getIrsPath()));

    QDir irs_fav(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites"));
    if (!irs_fav.exists())
        irs_fav.mkpath(".");

    reload();
    reloadFav();

    connect(ui->reload, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->fav_add, SIGNAL(clicked()), this, SLOT(addFav()));
    connect(ui->fav_remove, SIGNAL(clicked()), this, SLOT(removeFav()));
    connect(ui->fav_rename, SIGNAL(clicked()), this, SLOT(renameFav()));
    connect(ui->files, SIGNAL(itemSelectionChanged()), this, SLOT(updateIR()));
    connect(ui->favorites, SIGNAL(itemSelectionChanged()), this, SLOT(updateIR_Fav()));
    connect(ui->fileSelect, SIGNAL(clicked()), this, SLOT(selectFolder()));
    connect(ui->inspectIrs, SIGNAL(clicked()), this, SLOT(inspectIrs()));
    connect(ui->inspectFav, SIGNAL(clicked()), this, SLOT(inspectFav()));

    ui->tabWidget->setCurrentIndex(mainwin->getConv_DefTab());
}

Convolver::~Convolver()
{
    delete ui;
}
void Convolver::closeWindow(){
    mainwin->enableConvBtn(true);
    this->close();
}
void Convolver::inspectIrs(){
    if(ui->files->selectedItems().count()<1)return;
    const QString selection = QDir(ui->path->text()).filePath(ui->files->selectedItems().first()->text());

    inspect *i = new inspect(nullptr,selection);
    i->setModal(true);
    i->show();
}
void Convolver::inspectFav(){
    if(ui->favorites->selectedItems().count()<1)return;
    QString selection = QDir(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites")).filePath(ui->favorites->selectedItems().first()->text());

    inspect *i = new inspect(nullptr,selection);
    i->setModal(true);
    i->show();
}
void Convolver::reload(){
    lockupdate=true;
    QDir path(ui->path->text());
    QStringList nameFilter("*.wav");
    nameFilter.append("*.irs");
    QStringList files = path.entryList(nameFilter);
    ui->files->clear();
    if(files.empty()){
        QFont font;
        font.setItalic(true);
        font.setPointSize(11);

        QListWidgetItem* placeholder = new QListWidgetItem;
        placeholder->setFont(font);
        placeholder->setText("No IRS files found");
        placeholder->setFlags(placeholder->flags() & ~Qt::ItemIsEnabled);
        ui->files->addItem(placeholder);
    }
    else ui->files->addItems(files);
    lockupdate=false;
}
void Convolver::reloadFav(){
    lockupdate=true;
    QDir path(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites"));
    QStringList nameFilter("*.wav");
    nameFilter.append("*.irs");
    QStringList files = path.entryList(nameFilter);
    ui->favorites->clear();
    if(files.empty()){
        QFont font;
        font.setItalic(true);
        font.setPointSize(11);

        QListWidgetItem* placeholder = new QListWidgetItem;
        placeholder->setFont(font);
        placeholder->setText("Nothing here yet...");
        placeholder->setFlags(placeholder->flags() & ~Qt::ItemIsEnabled);
        ui->favorites->addItem(placeholder);
        QListWidgetItem* placeholder2 = new QListWidgetItem;
        //placeholder2->setFont(font);
        placeholder2->setText("Add some IRS files in the 'filesystem' tab");
        placeholder2->setFlags(placeholder2->flags() & ~Qt::ItemIsEnabled);
        ui->favorites->addItem(placeholder2);
    }
    else ui->favorites->addItems(files);
    lockupdate=false;
}
void Convolver::renameFav(){
    if(ui->favorites->selectedItems().count()<1)return;
    bool ok;
    QString text = QInputDialog::getText(this, "Rename",
                                         "New Name", QLineEdit::Normal,
                                         ui->favorites->selectedItems().first()->text(), &ok);
    QString fullpath = QDir(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites")).filePath(ui->favorites->selectedItems().first()->text());;
    QString dest = QDir::cleanPath(configpath + QDir::separator() + "irs_favorites");
    if (ok && !text.isEmpty())QFile::rename(fullpath,QDir(dest).filePath(text));
    reloadFav();
}
void Convolver::removeFav(){
    if(ui->favorites->selectedItems().count()<1)return;
    QString fullpath = QDir(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites")).filePath(ui->favorites->selectedItems().first()->text());;
    if(!QFile::exists(fullpath)){
        QMessageBox::warning(this, "Error", "Selected File doesn't exist",QMessageBox::Ok);
        reloadFav();
        return;
    }
    QFile file (fullpath);
    file.remove();
    mainwin->writeLog("Removed "+fullpath+" from favorites (convolver/remove)");
    reloadFav();
}
void Convolver::addFav(){
    if(ui->files->selectedItems().count()<1)return; //Clearing Selection by code != User Interaction

    const QString src = QDir(ui->path->text()).filePath(ui->files->selectedItems().first()->text());
    const QString& dest = QDir(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites")).filePath(ui->files->selectedItems().first()->text());

    if (QFile::exists(dest))QFile::remove(dest);

    QFile::copy(src,dest);
    mainwin->writeLog("Adding " + src + " to favorites (convolver/add)");
    reloadFav();
}

void Convolver::updateIR(){
    if(lockupdate || ui->files->selectedItems().count()<1)return; //Clearing Selection by code != User Interaction
    QString path = QDir(ui->path->text()).filePath(ui->files->selectedItems().first()->text());
    if(QFileInfo::exists(path) && QFileInfo(path).isFile())mainwin->setIRS(path.toUtf8().constData());
}
void Convolver::updateIR_Fav(){
    if(lockupdate || ui->favorites->selectedItems().count()<1)return; //Clearing Selection by code != User Interaction
    QString path = QDir(QDir::cleanPath(configpath + QDir::separator() + "irs_favorites")).filePath(ui->favorites->selectedItems().first()->text());
    if(QFileInfo::exists(path) && QFileInfo(path).isFile())mainwin->setIRS(path.toUtf8().constData());
}
void Convolver::reject()
{
    mainwin->enableConvBtn(true);
    QDialog::reject();
}
void Convolver::selectFolder(){

    QFileDialog *fd = new QFileDialog;
    fd->setFileMode(QFileDialog::Directory);
    fd->setOption(QFileDialog::ShowDirsOnly);
    fd->setViewMode(QFileDialog::Detail);
    QString result = fd->getExistingDirectory();
    if (result!="")
    {
        ui->path->setText(result);
        reload();
    }
}
