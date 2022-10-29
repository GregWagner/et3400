#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../emu/et3400.h"
#include "../util/settings.h"
#include "../util/srec.h"
#include "../widgets/display.h"
#include "../widgets/keypad.h"
#include "about.h"
#include "debugger.h"
#include "file.h"
#include "settings.h"
#include "tips.h"

#include <fstream>
#include <iostream>

#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>

class MainWindow : public QMainWindow {
    uint16_t MONITOR_SIZE = 0x400;
    uint16_t FANTOMII_SIZE = 0x800;
    uint16_t TINYBASIC_SIZE = 0x800;

    const uint16_t MONITOR_ADDR = 0xFC00;
    const uint16_t FANTOMII_ADDR = 0x1400;
    const uint16_t TINYBASIC_ADDR = 0x1C00;

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    void execute_emu();

private:
    long long last_cycles;
    Settings settings;
    Display* display;
    Keypad* keypad;
    SettingsDialog* settings_dialog;
    DebuggerDialog* debugger_dialog;
    et3400emu* emu;

    void load_ram();
    void save_ram();
    void show_about();
    void show_settings();
    void show_debugger();
    void show_tips();
    void updatecps();
    void fps();
};

#endif // MAINWINDOW_H