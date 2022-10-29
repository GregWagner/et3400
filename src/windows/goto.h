#ifndef GOTO_H
#define GOTO_H

#include "../common/common_defs.h"
#include "../util/label.h"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QVariant>
#include <vector>

class GotoDialog : public QDialog {
    Q_OBJECT

public:
    GotoDialog();
    void setLabels(std::vector<Label>* labels);
    offs_t getSelectedAddress();

private:
    offs_t _selectedAddress;
    QVBoxLayout* mainLayout;
    QLineEdit* address;
    QListView* listView;
    QDialogButtonBox* buttonBox;

    void setupUi(QDialog* Dialog);
    void addressEntered();
    void labelDoubleClicked(const QModelIndex& index);
    void labelSelected(const QModelIndex& index);
    void validate();
};

#endif // GOTO_H