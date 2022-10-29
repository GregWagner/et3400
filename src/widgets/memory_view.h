#ifndef MEMORY_VIEW_H
#define MEMORY_VIEW_H

//#include <thread>
#include <QAction>
#include <QBrush>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <QWidget>

#include "../common/common_defs.h"
#include "../dev/memory_map.h"
#include "../emu/et3400.h"

class MemoryView : public QFrame {
    Q_OBJECT

public:
    MemoryView();
    MemoryView(QWidget* parent);
    ~MemoryView();
    void update_display();
    void scroll(int steps);
    void scrollTo(int value);
    void set_emulator(et3400emu* emu);
    void set_range(offs_t start, offs_t end, uint8_t* memory);

signals:
    void on_scroll(int steps);
    void on_size(int max);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QScrollBar* scrollbar;
    QAction* action;
    QPixmap* buffer;
    QTimer* m_paintTimer;

    et3400emu* emu_ptr;

    bool running;
    offs_t start;
    offs_t end;
    uint8_t* memory;

    bool is_memory_set;
    int offset;
    int visible_items;
    int item_height = 20;
    int max_vscroll;

    void bufferDraw();
};

#endif // MEMORY_VIEW_H
