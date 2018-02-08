//
// Created by Melchior on 01/11/17.
//

#include <iostream>
#include "VocableList.h"
#include "Lect2Parser.h"
#include "stringUtils.h"
#include "Finally.h"

typedef xsd::cxx::parser::xerces::properties<char> Properties;

const ColumnType<Vocable> VocableList::foreignColumn= VocableList::ColumnType(QObject::tr("中文"),
      [](const Vocable& vocable) {return vocable.getForeign();},
      [](Vocable& vocable, const QString& foreign_) { return vocable.setForeign(foreign_);},
      ColumnType::Default,
      ColumnType::SizePolicy(40,-1)
);
const ColumnType<Vocable> VocableList::readingColumn= VocableList::ColumnType(QObject::tr("pinyin"),
      [](const Vocable& vocable) {return vocable.getReading();},
      [](Vocable& vocable, const QString& reading_) { return vocable.setReading(reading_);},
      ColumnType::Default,
      ColumnType::SizePolicy(50,-1)
);
const ColumnType<Vocable> VocableList::nativeColumn= VocableList::ColumnType(QObject::tr("English"),
     [](const Vocable& vocable) {return join(vocable.getNatives(),";");},
     [](Vocable& vocable, const QString& native_) { return vocable.setNatives(
             QString(native_).split(";", QString::SkipEmptyParts));},
     ColumnType::Default,
     ColumnType::SizePolicy(50,-4)
);

VocableList::VocableList() :MyTableModel<Vocable>(std::initializer_list<const ColumnType*>{&foreignColumn,&readingColumn,
       new GrammarColumn(this,QObject::tr("Grammar"),ColumnType::SizePolicy(80,120)),&nativeColumn}),
       grammarColumn(static_cast<const GrammarColumn*>(&MyTableModel::getColumnType(2))) {
    auto* stringParser= new xml_schema::string_pimpl();
    info_parser(new InfoParser(stringParser,stringParser,stringParser,stringParser));
    voc_parser(new VocableParser(new ZhParser(stringParser,stringParser),stringParser,stringParser));
    parser= new xml_schema::document(*this,"lect2");
    this->parserProperties.no_namespace_schema_location("lect2.xsd");
}

VocableList::~VocableList() {
    if(parser!= nullptr) {
        auto* infoParser= getInfoParser();
        auto* strParser= infoParser!= nullptr? infoParser->getTitleParser() : nullptr;
        delete infoParser;
        auto* vocParser= getVocParser();
        if (vocParser!= nullptr) delete vocParser->getZhParser();
        delete vocParser;
        delete strParser;
        delete parser;
    }
    delete grammarColumn;
}

void VocableList::pre() {
    clear();
}

void VocableList::setInfo(const Info& info_) {
    this->info= info_;
}

void VocableList::addVoc(const Vocable& vocable) {
    this->vocables.push_back(vocable);
}


bool VocableList::load(std::istream& in) {
    layoutAboutToBeChanged();
    try {
        auto finalizer= finally([this]() {
            setModified(false);
            layoutChanged();
        });
        this->pre();
        parser->parse(in,0, parserProperties);
        this->post_Lect2();
        return true;
    }catch (const xml_schema::exception& e) {
        std::cerr<<e<<std::endl;
        return false;
    }

}

QStringList VocableList::getAllGrammars() const {
    QStringList result;
    QSet<QString> filter;
    for(const QString& gr :{tr("名词"),tr("专有名词"),tr("动词"),tr("及物动词"),tr("助动词"),tr("形容词"),tr("数词"),tr("量词"),tr("副词"),tr("介词"),tr("连词"),tr("助词"),tr("成语"),tr("叹词")}) {
        filter.insert(gr);  result.push_back(gr);
    }
    //QStringList newItems;
    for(const Vocable& vocable :vocables) {
        QString grammar = vocable.getGrammar();
        if(!filter.contains(grammar)) {
            result.push_back(grammar);
            filter.insert(grammar);
        }
    }
    //qStableSort(newItems);
    //result.append(newItems);
    return result;
}

bool VocableList::save(QTextStream &out) {
    out<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<!DOCTYPE lect2 SYSTEM \"lect2.dtd\">\n"
            "<lect2>\n";
    out<<info;
    for(const Vocable& vocable :vocables) {
        if(!vocable.isEmpty()) out<<vocable;
    }
    out<<"</lect2>\n";
    setModified(false);
    return true;
}


QTextStream& operator<<(QTextStream& out, const Info& info) {
    out<<"<info><title>"<<info.title<<"</title>\n";
    out<<"  <author>"<<info.author<<"</author>\n";
    if(!info.email.isEmpty()) out<<"  <mail>"<<info.email<<"</mail>\n";
    if(!info.description.isEmpty())  out<<"  <description>"<<info.description<<"</description>\n";
    return out<<"</info>\n";
}

QTextStream &operator<<(QTextStream&out, const Vocable& v) {
    out<<"<voc><cn>";
    if(!v.getForeign().isEmpty())  out<<"<w>"<<v.getForeign()<<"</w>";
    if(!v.getReading().isEmpty())  out<<"<r>"<<v.getReading()<<"</r>";
    out<<"</cn>\n";
    if(!v.getGrammar().isEmpty())  out<<"  <t>"<<v.getGrammar()<<"</t>\n";
    if(!v.getNatives().isEmpty())  out<<"  ";
    for(const Vocable::Translation& translation :v.getNatives()) {
        out<<"<en>"<<translation<<"</en>";
    }
    return out<<"</voc>\n";
}
