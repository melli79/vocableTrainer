//
// Created by Melchior on 29/12/17.
//

#include "Trainer.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

const QStringList Trainer::modeNames= {Trainer::tr("Foreign to Native"), Trainer::tr("Reading to Native"),
                                       Trainer::tr("Reading to Foreign"), Trainer::tr("Native to Foreign")};

Trainer::Trainer(QWidget* parent, const QList<Vocable>& vocables_, Trainer::Mode mode_) :QDialog(parent), mode(mode_) {
    for(const Vocable& vocable :vocables_) {
        if(!vocable.isEmpty()) vocables.push_back(new VocableItem{&vocable});
    }
    setWindowTitle(tr("Vocable Training – %1").arg(modeNames[mode]));
    auto* layout= new QGridLayout(this);
    layout->addWidget(new QLabel(tr("foreign:")),0,0);
    layout->addWidget(foreignLabel= new QLabel(),0,1);
    QFont font = foreignLabel->font();
    font.setPointSizeF(font.pointSizeF()*1.5f);
    foreignLabel->setFont(font);
    layout->addWidget(new QLabel(tr("reading:")),1,0);
    layout->addWidget(readingLabel= new QLabel(),1,1);
    layout->addWidget(new QLabel(tr("translations:")),2,0);
    layout->addWidget(translationsLabel= new QLabel(),2,1);
    layout->addWidget(new QLabel(tr("Your answer:")),3,0);
    layout->addWidget(inputLine= new QLineEdit(),3,1);
    layout->addWidget(nextButton= new QPushButton(tr("Next")),4,1,Qt::AlignRight);
    layout->addWidget(stopButton= new QPushButton(tr("Stop")),4,2);
    connect(nextButton,&QAbstractButton::clicked,this,&Trainer::next);
    connect(stopButton,&QAbstractButton::clicked,this,&Trainer::stop);
}

Trainer::~Trainer() {
    foreach(auto* item,vocables)  delete item;
    delete foreignLabel;
    delete readingLabel;
    delete translationsLabel;
    delete inputLine;
    delete nextButton;
    delete stopButton;
}

void Trainer::resume() {
    showNext();
}

void Trainer::showNext() {
    if(!vocables.empty()) {
        VocableItem& item= *vocables.first();
        foreignLabel->setText(item.vocable->getForeign());
        readingLabel->setText(item.vocable->getReading());
        translationsLabel->setText(item.vocable->getNatives().toList().join(";"));
        inputLine->clear();
        if(item.success>0) switch (mode) {
          case Zh2En:
            foreignLabel->setVisible(true);
            readingLabel->setVisible(false);
            translationsLabel->setVisible(false);
            break;
          case Py2Zh:
          case Py2En:
            foreignLabel->setVisible(false);
            readingLabel->setVisible(true);
            translationsLabel->setVisible(false);
            break;
          case En2Zh:
            foreignLabel->setVisible(false);
            readingLabel->setVisible(false);
            translationsLabel->setVisible(true);
            break;
        }else {
            foreignLabel->setVisible(true);
            readingLabel->setVisible(true);
            translationsLabel->setVisible(true);
        }
        setModal(true);
        setVisible(true);
    }
}

void Trainer::next(bool) {
    QString input= inputLine->text();
    VocableItem& item= *vocables.takeFirst();
    bool success= false;
    switch (mode) {
        case Zh2En:
        case Py2En:
            success= item.vocable->getNatives().contains(input);
            break;
        case En2Zh:
        case Py2Zh:
            success= item.vocable->getForeign()==input;
            break;
    }
    if(success) {
        if(++item.success>=3&&isFinished()) {
            stopButton->setText(tr("Finish"));
        }
        auto here= vocables.begin();
        for(unsigned i=0; here!=vocables.end()&& i<unsigned(item.success)*item.success+1; ++i)  ++here;
        vocables.insert(here,&item);
    }else {
        if(item.success>=3) stopButton->setText(tr("Abort"));
        item.success= 0;
        vocables.insert(vocables.begin(),&item);
    }
    showNext();
}

void Trainer::stop(bool) {
    setVisible(false);
    setModal(false);
    showStat();
    emit stopped();
}

void Trainer::showStat() {
    QString stats;
    if(isFinished())  stats= tr("Congratulations!  You have learned %1 vocables.").arg(vocables.size());
    else {
        unsigned few= countFew();
        if(few>0) stats = tr("You have succeeded with %1 of %2 vocables. (%3 in good progress.)")
                .arg(countSuccess()).arg(vocables.size()).arg(few);
        else  stats= tr("You have succeeded with %1 of %2 vocables.").arg(countSuccess()).arg(vocables.size());
    }
    QMessageBox::information(parentWidget(), tr("Vocable Trainer: Training Results"),stats);
}

unsigned Trainer::countSuccess() const {
    unsigned result= 0;
    for(auto item :vocables)
        if(item->success>=3) ++result;
    return result;
}

unsigned Trainer::countFew() const {
    unsigned result= 0;
    for(auto item :vocables)
        if(item->success>0&&item->success<3) ++result;
    return result;
}

bool Trainer::isFinished() const {
    for(auto* item :vocables)
        if(item->success<3)  return false;
    return true;
}
