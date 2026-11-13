#pragma once

#include <QWidget>
#include "edianalyzer.h"

class QTableWidget;
class QCheckBox;
class QLabel;

// Панель "Structure View" — отображает структуру разобранного EDI-сегмента
// в виде таблицы Value / Element / Component (аналог frmGoToLine в оригинале).
class EdiPanel : public QWidget
{
    Q_OBJECT
public:
    explicit EdiPanel(QWidget *parent = nullptr);

    // Очистить и заполнить таблицу данными сегмента.
    void showSegment(const EdiSegment &seg);

    void clear();

private:
    QTableWidget *m_table = nullptr;
    QCheckBox *m_showEmpty = nullptr;
    QLabel *m_segmentName = nullptr;
};
