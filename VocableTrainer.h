//
// Created by melli on 31/10/17.
//

#ifndef VOCABLETRAINER_H
#define VOCABLETRAINER_H

#include <QMainWindow>
#include "Info.h"
#include "Trainer.h"

class Vocable;
template <typename RowModel>
class MyTableView;
class VocableList;
class Trainer;

class VocableTrainer :public QMainWindow {
    Q_OBJECT
private:
    static const int maxRecentFiles= 5;
    MyTableView<Vocable>* table;
    VocableList* tableModel;
    QString fileName;
    QToolBar* toolbar;
    QMenu* recentMenu;
    QStringList recentFileNames;
    QAction* fileActions[maxRecentFiles];
    Info fileInfo;
    Trainer* trainer;

public:
    static const char* aboutText;
    explicit VocableTrainer();
    ~VocableTrainer() override;
    void openFile(const QString& fileName);

protected slots:
    void about();
    void newFile();
    void open();
    void openRecentFile();
    bool save();
    bool saveAs();
    void insertBefore();
    void add10Lines();
    void removeCurrentLine();
    void startForeignTraining();
    void startReadingTraining();
    void startNativeTraining();
    void startWriteTraining();
    void showTable();

signals:
    void exit(int resultCode);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void createActions();
    void initRecent();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString& fileName);
    void setCurrentFile(const QString& string);
    void prependToRecentFiles(const QString& fileName);
    void updateFileActions();
    void initTable();

    int getSelectedRow() const; // -1 means nothing selected
    void startTraining(const Trainer::Mode &mode);
};


#endif //VOCABLETRAINER_H
