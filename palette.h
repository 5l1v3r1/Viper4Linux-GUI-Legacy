#ifndef PALETTE_H
#define PALETTE_H

#include <QDialog>

namespace Ui {
class palette;
}

class palette : public QDialog
{
    Q_OBJECT

public:
    explicit palette(QWidget *parent = nullptr);
    ~palette();

private:
    Ui::palette *ui;
private slots:
    int loadColor(int index,int rgb_index);
    void closeWin();
    void Reset();
    void updateBase();
    void updateBack();
    void updateFore();
    void updateIcons();
    void updateSelection();
    void saveColor(int index,QColor color);
};

#endif // PALETTE_H
