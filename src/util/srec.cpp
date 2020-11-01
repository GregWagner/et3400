#include "srec.h"

srec_block make_block(int bytecount, int address, uint8_t *data)
{
    srec_block block = {bytecount, address, data};
    return block;
}

bool Srec::Read(QString path, std::vector<srec_block> *blocks)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.length() > 0)
        {
            bool success = false;
            int bytecount = line.mid(2, 2).toUInt(&success, 16);
            QString addr;
            QString data;
            QString checksum;
            QString type = line.mid(0, 2);

            if (type == "S1")
            {
                int dataLength = bytecount - 3;

                addr = line.mid(4, 4);
                data = line.mid(8, dataLength * 2);

                int addrVal = addr.toUInt(&success, 16);

                uint8_t *buffer = (uint8_t *)malloc(bytecount - 3);

                for (int ptr = 0; ptr < dataLength; ptr++)
                {
                    buffer[ptr] = data.mid(ptr * 2, 2).toUInt(&success, 16);
                }

                blocks->push_back(make_block(bytecount, addrVal, buffer));

                checksum = line.mid(bytecount * 2 + 2, 2);
                int checksumActual = checksum.toUInt(&success, 16);
            }
        }
    }

    return true;
}