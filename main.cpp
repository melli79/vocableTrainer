#include <QApplication>
#include <QCommandLineParser>

#include "VocableTrainer.h"

/*
 * Created by melli
 *
 * Main program of the Vocable trainer
 **/
int main(int nArgs, char** args) {
    QApplication app(nArgs,args);
    app.setApplicationName("Vocable Trainer");
    app.setOrganizationName("mellis");
    app.setApplicationVersion("0.2");
    app.setWindowIcon(QIcon("vocableTrainer.icns"));
    QCommandLineParser parser;
    parser.setApplicationDescription(VocableTrainer::aboutText);
    parser.addPositionalArgument("vocableList.l2","l2-file with vocables");
    parser.addVersionOption();
    parser.process(app);

    auto trainer= new VocableTrainer();
    QObject::connect(trainer,&VocableTrainer::exit,&app,&QApplication::exit);
    trainer->show();
    auto fileArgs= parser.positionalArguments();
    if(!fileArgs.empty()) trainer->openFile(fileArgs.at(0));

    return app.exec();
}
