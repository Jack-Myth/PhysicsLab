#ifndef ELECAPPLIANCESPANEL_H
#define ELECAPPLIANCESPANEL_H

#include <QDialog>

namespace Ui {
class ElecappliancesPanel;
}

class ElecappliancesPanel : public QDialog
{
    Q_OBJECT

public:
    explicit ElecappliancesPanel(QWidget *parent = 0);
    ~ElecappliancesPanel();

private:
    Ui::ElecappliancesPanel *ui;
};

#endif // ELECAPPLIANCESPANEL_H
