#ifndef FILE_H
#define FILE_H

#include "../emu/et3400.h"
#include <QFileDialog>
#include <QObject>
#include <QString>
#include <vector>

class File {
public:
    static void load_ram(QWidget* parent, et3400emu* emu_ptr);
    static void save_ram(QWidget* parent, et3400emu* emu_ptr);
};

#endif // FILE_H