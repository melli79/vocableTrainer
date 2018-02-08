/**
 * Created by Melchior on 31/10/17.
 */

#include <QMenuBar>
#include <QStatusBar>
#include <QtEvents>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QTableView>
#include <QToolBar>
#include <QTextStream>
#include <QFileDialog>
#include <cstdlib>
#include <fstream>
#include "VocableTrainer.h"
#include "VocableList.h"
#include "MyTableView.h"
#include "Lect2Parser.h"
#include "Trainer.h"

using namespace std;

const int VocableTrainer::maxRecentFiles;
const char* VocableTrainer::aboutText= "Vocable Trainer that helps you organize and train your vocables.";

VocableTrainer::VocableTrainer() :trainer(nullptr) {
    setWindowIcon(QIcon("myicons.icns"));
    tableModel= new VocableList();
    table= new MyTableView<Vocable>(this);
    table->setModel(tableModel);
    setCentralWidget(table);
    addToolBar(toolbar= new QToolBar());
    initTable();
    createActions();
    createStatusBar();
    this->setMinimumSize(350,300);
    readSettings();
    updateFileActions();
    setUnifiedTitleAndToolBarOnMac(true);
}

void VocableTrainer::createActions() {
    QMenu* fileMenu= menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&About"),this,&VocableTrainer::about);
    toolbar->addAction(fileMenu->addAction(tr("&New"),this,&VocableTrainer::newFile,QKeySequence("Ctrl+n")));
    toolbar->addAction(fileMenu->addAction(tr("&Open..."),this,&VocableTrainer::open,QKeySequence("Ctrl+o")));
    recentMenu= fileMenu->addMenu(tr("Open &recent..."));
    initRecent();
    fileMenu->addSeparator();
    toolbar->addAction(fileMenu->addAction("&Save",this,&VocableTrainer::save,QKeySequence("Ctrl+s")));
    toolbar->addAction(fileMenu->addAction("Save &as...",this,&VocableTrainer::saveAs,QKeySequence("Shift+Ctrl+S")));
    fileMenu->addAction("&Close",this,&VocableTrainer::newFile,QKeySequence("Ctrl+w"));
    fileMenu->addSeparator(); toolbar->addSeparator();
    fileMenu->addAction("Quit",this,&VocableTrainer::close);
    QMenu* trainingMenu= menuBar()->addMenu(tr("&Training"));
    connect(toolbar->addAction(tr("Training")),&QAction::triggered,trainingMenu,&QMenu::show);
    trainingMenu->addAction(Trainer::modeNames[Trainer::Zh2En],this, &VocableTrainer::startForeignTraining);
    trainingMenu->addAction(Trainer::modeNames[Trainer::Py2En],this, &VocableTrainer::startReadingTraining);
    trainingMenu->addAction(Trainer::modeNames[Trainer::En2Zh],this, &VocableTrainer::startNativeTraining);
    trainingMenu->addAction(Trainer::modeNames[Trainer::Py2Zh],this, &VocableTrainer::startWriteTraining);
    QAction* add10Lines = new QAction(tr("+10"));
    QObject::connect(add10Lines, &QAction::triggered, this, &VocableTrainer::add10Lines);
    toolbar->addAction(add10Lines);
    toolbar->addAction(new QAction(tr("Cut")));
    toolbar->addAction(new QAction(tr("Copy")));
    toolbar->addAction(new QAction(tr("Paste")));
    QAction* insertLine= new QAction(tr("+"));
    toolbar->addAction(insertLine);
    QObject::connect(insertLine,&QAction::triggered, this, &VocableTrainer::insertBefore);
    QAction* removeLine= new QAction(tr("-"));
    toolbar->addAction(removeLine);
    QObject::connect(removeLine,&QAction::triggered, this, &VocableTrainer::removeCurrentLine);
}

void VocableTrainer::initRecent() {
    for (auto& fileAction :fileActions) {
        fileAction= new QAction();
        fileAction->setVisible(false);
        recentMenu->addAction(fileAction);
    }
}

void VocableTrainer::openFile(const QString& fileName) {
    if(maybeSave()) {
        QFile file(fileName);
        if(!file.exists()) {
            QMessageBox::warning(this,tr("Read error"),tr("Error reading file %1: %2").arg(fileName,file.errorString()));
            //return;
        }
        std::ifstream in(fileName.toStdString());
        tableModel->load(in);
        statusBar()->showMessage(tr("%1 Vocables loaded.").arg(tableModel->rowCount()),2000);
        setCurrentFile(fileName);
    }
}

VocableTrainer::~VocableTrainer() {
    delete table;
    delete tableModel;
    for(auto recentFile :fileActions)
        delete recentFile;
    delete trainer;
}

void VocableTrainer::closeEvent(QCloseEvent* event) {
    if(maybeSave()) {
        writeSettings();
        event->accept();
        emit exit(0);
    }else {
        event->ignore();
    }
}

void VocableTrainer::about() {
    QMessageBox::about(this,QCoreApplication::applicationName(),tr(aboutText));
}

void VocableTrainer::createStatusBar() {
    statusBar()->showMessage(tr("Ready."),5000);
}

static QStringList deserialize(const QByteArray& bytes) {
    QDataStream in(bytes);
    QStringList result;
    in>>result;
    return result;
}

void VocableTrainer::readSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    auto geometry= settings.value("geometry", QByteArray()).toByteArray();
    if(geometry.isEmpty()) {
        auto available= QApplication::desktop()->availableGeometry(this);
        resize(available.width()/3, available.height()/2);
        move((available.width()-width())/2,(available.height()-height())/2);
    }else {
        restoreGeometry(geometry);
    }
    recentFileNames= deserialize(settings.value("recentFiles",QByteArray()).toByteArray());
}

static QByteArray serialize(const QStringList& list) {
    QByteArray result;
    QDataStream out(result);
    out<<list;
    return result;
}

void VocableTrainer::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", serialize(recentFileNames));
}

bool VocableTrainer::maybeSave() {
    if(!tableModel->isModified()) return true;
    const auto choice= QMessageBox::warning(this,QCoreApplication::applicationName(),tr("The document has been modified.\n"
             "Do you want to save the changes?"),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
    switch (choice) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}

bool VocableTrainer::save() {
    if(fileName.isEmpty()) return saveAs();
    return saveFile(fileName);
}

bool VocableTrainer::saveFile(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this,tr("Write error"),tr("Cannot write to file %1: %2").arg(QDir::toNativeSeparators(fileName),
                                                                                 file.errorString()));
        return false;
    }
    QTextStream out(&file);
    bool result= tableModel->save(out);
    out.flush();  file.close();
    if(result) {
        statusBar()->showMessage(tr("File saved."),2000);
        setCurrentFile(fileName);
    }
    else  statusBar()->showMessage(tr("Error writing file."),5000);
    return result;
}

void VocableTrainer::setCurrentFile(const QString &fileName) {
    static int number= 1;
    QString currentFile;
    bool nameless = fileName.isEmpty();
    if(nameless) {
        currentFile= QString("vocables%1.l2").arg(number);
    }else {
        currentFile= fileName;
    }
    tableModel->setModified(false);
    setWindowModified(false);
    if(!nameless&&!(currentFile==this->fileName)) {
        prependToRecentFiles(currentFile);
        this->fileName= currentFile;
    }else {
        this->fileName= QString();
    }
    setWindowFilePath(currentFile);
}

void VocableTrainer::newFile() {
    if(maybeSave()) {
        tableModel->clear();
        setCurrentFile(QString());
    }
}

void VocableTrainer::open() {
    if(!maybeSave())  return;
    QString fileName= QFileDialog::getOpenFileName(this,tr("Open file"),QString(),"*.l2");
    if(!fileName.isEmpty()) {
        openFile(fileName);
    }
}

bool VocableTrainer::saveAs() {
    QString fileName= QFileDialog::getSaveFileName(this,tr("Save as"),QString(),"*.l2");
    if(!fileName.isEmpty()) {
        return saveFile(fileName);
    }
    return false;
}

void VocableTrainer::prependToRecentFiles(const QString &fileName) {
    recentFileNames.removeAll(fileName);
    recentFileNames.prepend(fileName);
    updateFileActions();
}

void VocableTrainer::updateFileActions() {
    const int count= qMin(maxRecentFiles,recentFileNames.size());
    int i=0;
    for(; i<count; ++i) {
        QString fileName= recentFileNames.at(i);
        QAction& a= *fileActions[i];
        a.setText(tr("&%1 %2").arg(i+1).arg(fileName));
        a.setData(fileName);
        a.setVisible(true);
    }
    for(; i<maxRecentFiles; ++i) {
        fileActions[i]->setVisible(false);
    }
}

void VocableTrainer::openRecentFile() {
    if(!save())  return;
    if(const auto* action= qobject_cast<const QAction*>(sender())) {
        openFile(action->data().toString());
    }
}

void VocableTrainer::initTable() {
    table->setColumnWidth(tableModel->getIndex(*tableModel->grammarColumn),50);
    table->setAlternatingRowColors(true);
}

void VocableTrainer::insertBefore() {
    int row= getSelectedRow();
    if(row<0)  row=0;
    tableModel->insertRows(row,1);
}

int VocableTrainer::getSelectedRow() const {
    int lowestRow= -1;
    QItemSelectionModel* selection= table->selectionModel();
    foreach(auto index, selection->selectedIndexes()) {
        if(lowestRow==-1)  lowestRow= index.row();
        else  lowestRow= min(lowestRow,index.row());
    }
    return lowestRow;
}

void VocableTrainer::removeCurrentLine() {
    int row= getSelectedRow();
    if(row>=0) tableModel->removeRows(row,1);
}

void VocableTrainer::add10Lines() {
    int row= getSelectedRow();
    if(row<0) row= tableModel->rowCount();
    tableModel->insertRows(row,10);
}

void VocableTrainer::startForeignTraining() {
    startTraining(Trainer::Zh2En);
}

void VocableTrainer::startTraining(const Trainer::Mode &mode) {
    table->setVisible(false);
    if(trainer != nullptr && mode != trainer->getMode()) {
        delete trainer;  trainer= nullptr;
    }
    if(trainer == nullptr) {
        trainer = new Trainer(this, tableModel->getVocables(), mode);
        connect(trainer, &Trainer::stopped, this, &VocableTrainer::showTable);
    }
    trainer->resume();
}

void VocableTrainer::showTable() {
    table->setVisible(true);
}

void VocableTrainer::startReadingTraining() {
    startTraining(Trainer::Py2En);
}

void VocableTrainer::startNativeTraining() {
    startTraining(Trainer::En2Zh);
}

void VocableTrainer::startWriteTraining() {
    startTraining(Trainer::Py2Zh);
}
