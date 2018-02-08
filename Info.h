//
// Created by Melchior on 03/12/17.
//

#ifndef VOCABLETRAINER_INFO_H
#define VOCABLETRAINER_INFO_H

#include <QString>
#include <QTextStream>

struct Info {
    QString title, author, email, description;
};

QTextStream& operator<<(QTextStream& out, const Info& info);

#endif //VOCABLETRAINER_INFO_H
