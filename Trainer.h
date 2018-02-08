//
// Created by Melchior on 29/12/17.
//

#ifndef VOCABLETRAINER_TRAINER_H
#define VOCABLETRAINER_TRAINER_H

#include <QDialog>
#include <QLinkedList>
#include "Vocable.h"

struct VocableItem;
class QLineEdit;
class QLabel;
class QPushButton;

class Trainer :public QDialog {
    Q_OBJECT
public:
    enum Mode {Zh2En, Py2En, Py2Zh, En2Zh};
    static const QStringList modeNames;
private:
    QLinkedList<VocableItem*> vocables;
    Mode mode;
    QLineEdit* inputLine;
    QLabel* foreignLabel;
    QLabel* readingLabel;
    QLabel* translationsLabel;
    QPushButton* nextButton;
    QPushButton* stopButton;

public:
    Trainer(QWidget* parent, const QList<Vocable>& vocables_, Mode mode_=Zh2En);
    ~Trainer() override;

    void resume();
    Mode getMode() const { return mode; }

signals:
    void stopped();

protected slots:
    void next(bool);
    void stop(bool);

protected:
    void showNext();
    void showStat();
    unsigned countSuccess() const ;
    unsigned countFew() const;
    bool isFinished() const;
};

struct VocableItem {
    const Vocable* vocable;
    unsigned short success= 0;
};

#endif //VOCABLETRAINER_TRAINER_H
