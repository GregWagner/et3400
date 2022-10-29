#ifndef KEYPAD_H
#define KEYPAD_H

#include "../dev/keypad_dev.h"
#include <QGridLayout>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QWidget>
#include <thread>

class Keypad : public QWidget {
    Q_OBJECT

public:
    explicit Keypad(QWidget* parent = nullptr);
    ~Keypad();
    keypad_io* device;

public slots:
    void press_key(keypad_io::Keys key);
    void release_key(keypad_io::Keys key);

private:
    QPushButton* create_button(QString name, QString icon, keypad_io::Keys key);
};

#endif // KEYPAD_H
