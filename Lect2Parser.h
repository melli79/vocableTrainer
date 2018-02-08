//
// Created by Melchior on 30/11/17.
//

#ifndef LECT2PARSER_H
#define LECT2PARSER_H

#include "lect2-pskel.hxx"
#include "stringUtils.h"

class InfoParser :public Info_pskel {
public:
    InfoParser(xml_schema::string_pskel* title_parser_, xml_schema::string_pskel* author_parser_,
               xml_schema::string_pskel* mail_parser_, xml_schema::string_pskel* description_parser_) {
        title_parser(title_parser_);
        author_parser(author_parser_);
        mail_parser(mail_parser_);
        description_parser(description_parser_);
    }

    void pre() override {
        _info= Info();
    }

    void title(const QString &title_) override {
        _info.title= title_;
    }

    void author(const QString &author_) override {
        _info.author= author_;
    }

    void mail(const QString &mail_) override {
        _info.email= mail_;
    }

    void description(const QString &description_) override {
        _info.description= description_;
    }

    Info post_Info() override {
        return _info;
    }

protected:
    Info _info;
};

class VocableParser :public Vocable_pskel {
public:
    VocableParser(Zh_pskel* zh_parser_, xml_schema::string_pskel* t_parser_, xml_schema::string_pskel* en_parser_) {
        zh_parser(zh_parser_);
        t_parser(t_parser_);
        en_parser(en_parser_);
    }

    void pre () override {
        foreign= "";
        readings.clear();
        grammar= "";
        natives= Vocable::Translations();
    }

    void zh(const Zh& zh) override {
        foreign= zh.writing;
        readings= zh.readings;
    }

    void t(const QString& t) override {
        this->grammar= t;
    }

    void en(const QString& native) override {
        natives.insert(native);
    }

    Vocable post_Vocable() override {
        return Vocable(foreign,join(readings,";"), grammar, natives);
    }

protected:
    Vocable::Translation foreign;
    QStringList readings;
    QString grammar;
    Vocable::Translations natives;
};

class ZhParser :public Zh_pskel {
public:
    ZhParser(xml_schema::string_pskel* w_parser_, xml_schema::string_pskel* r_parser_) {
        w_parser(w_parser_);
        r_parser(r_parser_);
    }

    void pre() override {
        zh= Zh();
    }

    void w(const QString &w) override {
        zh.writing= w;
    }

    void r(const QString &r) override {
        zh.readings.push_back(r);
    }

    Zh post_Zh() override {
        return zh;
    }

protected:
    Zh zh;
};

#endif //LECT2PARSER_H
