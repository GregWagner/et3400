#ifndef ADD_LABEL_H
#define ADD_LABEL_H

#include "../common/common_defs.h"
#include "../util/label.h"

#include <QVariant>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

struct LabelInfo
{
    QString text;
    LabelType type;
    offs_t start;
    offs_t end;
};

class AddLabelDialog : public QDialog
{
    Q_OBJECT

public:
    AddLabelDialog();

    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *comment_radio;
    QRadioButton *data_radio;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *text_label;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *text_edit;
    QLineEdit *start_edit;
    QLineEdit *end_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setLabel(LabelInfo label);
    LabelInfo getLabel();

private:
    void setupUi(QDialog *Dialog);
    void set_comment(bool checked = false);
    void set_data(bool checked = false);
    void retranslateUi(QDialog *Dialog);
};

#endif // ADD_LABEL_H