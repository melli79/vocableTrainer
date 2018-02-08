//
// Created by Melchior on 01/11/17.
//

#ifndef VOCABLETRAINER_VOCABLELIST_H
#define VOCABLETRAINER_VOCABLELIST_H

#include <QAbstractTableModel>
#include <QTextStream>
#include <utility>
#include "Vocable.h"
#include "Info.h"
#include "lect2-pskel.hxx"
#include "MyTableModel.h"

class GrammarColumn;

class VocableList :public MyTableModel<Vocable>, protected Lect2_pskel {
    Q_OBJECT
public:
    typedef ColumnType<Vocable>  ColumnType;
protected:
    Info info;
public:
    const static ColumnType foreignColumn, readingColumn, nativeColumn;
    const GrammarColumn* grammarColumn;

    explicit VocableList();
    ~VocableList() override;

    bool load(std::istream& in);
    bool save(QTextStream& out);

    QStringList getAllGrammars() const;
    const QList<Vocable>& getVocables() const { return vocables; }

protected:  // parser stuff
    ::xml_schema::document* parser;
    ::xsd::cxx::parser::xerces::properties<char> parserProperties;
    void pre() override;
    void setInfo(const Info &info_) override;
    void addVoc(const Vocable &vocable) override;
};


class GrammarColumn :public ColumnType<Vocable> {
private:
    const VocableList* vocableList;
public:
    explicit GrammarColumn(VocableList* model_, QString name_= QObject::tr("grammar","columnName"), SizePolicy width_= SizePolicy(30)) :ColumnType(
            std::move(name_), [](const Vocable& vocable) {return vocable.getGrammar();},
            [](Vocable& vocable, const QString& grammar_) { return vocable.setGrammar(grammar_);},
            ComboBox,width_),vocableList(model_) {}

    QStringList getComboBoxItems() const override { return vocableList->getAllGrammars(); }

};


#endif //VOCABLETRAINER_VOCABLELIST_H
