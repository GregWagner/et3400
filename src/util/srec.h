#ifndef SREC_H
#define SREC_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <vector>

struct srec_block {
    int bytecount;
    int address;
    uint8_t* data;
};

class SrecReader {
public:
    static bool Read(QString file, std::vector<srec_block>* blocks);
    static bool Write(QString file, std::vector<srec_block>* blocks);
};

#endif // SREC_H
