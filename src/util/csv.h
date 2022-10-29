#ifndef CSV_H
#define CSV_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <vector>

bool readCSVRow(QTextStream& in, QStringList* row);

#endif // CSV_H
