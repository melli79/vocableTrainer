//
// Created by Melchior on 11/11/17.
//

#ifndef VOCABLETRAINER_VOCABLE_H
#define VOCABLETRAINER_VOCABLE_H

#include <QString>
#include <QSet>
#include <utility>

class QTextStream;

class Vocable;
QTextStream& operator<<(QTextStream& out, const Vocable& v);


class Vocable {
public:
    typedef QString Translation;
    typedef QSet<Translation> Translations;
private:
    Translation foreign;
    Translation reading;
    QString grammar;
    Translations natives;
public:
    Vocable() = default;
    template <typename Container>
    Vocable(QString foreign_, QString reading_, QString grammar_, const Container natives_)
            :foreign(std::move(foreign_)), reading(std::move(reading_)), grammar(std::move(grammar_)),
             natives(natives_) {}

    bool isEmpty() const {
        return foreign.isEmpty() && reading.isEmpty() && natives.empty();
    }

    QString getForeign() const {
        return foreign;
    }

    bool setForeign(QString foreign_) {
        foreign= std::move(foreign_);
        return true;
    }

    QString getReading() const {
        return reading;
    }

    bool setReading(QString reading_) {
        reading= std::move(reading_);
        return true;
    }

    QString getGrammar() const {
        return grammar;
    }

    bool setGrammar(QString grammar_) {
        grammar= std::move(grammar_);
        return true;
    }

    const QSet<QString> getNatives() const {
        return natives;
    }

    template <class Container>  // something like a std::vector
    bool setNatives(const Container values) {
        natives.clear();
        foreach(QString s,values) natives.insert(s);
        return true;
    }

    bool addNative(const QString& native) {
        if(natives.contains(native)) return false;
        natives.insert(native);
        return true;
    }

    bool removeNative(const QString& native) {
        return natives.remove(native);
    }

};

#endif //VOCABLETRAINER_VOCABLE_H
