//
// Created by Melchior on 12/11/17.
//

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>
#include <QSet>

template <typename Container>
QString join(const Container& c, const QString& delim) {
    QString result;
    bool first= true;
    foreach(const QString& s,c) {
        if(first) {
            first= false;
        }else result.append(delim);
        result.append(s);
    }
    return result;
}

#endif //STRINGUTILS_H
