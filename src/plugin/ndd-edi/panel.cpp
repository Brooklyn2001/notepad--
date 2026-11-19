#include "panel.h"

#include <QTableWidget>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>

EdiPanel::EdiPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    m_segmentName = new QLabel(tr("Segment"), this);
    layout->addWidget(m_segmentName);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({tr("Value"), tr("Element"), tr("Component")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    m_showEmpty = new QCheckBox(tr("Show empty values"), this);
    layout->addWidget(m_showEmpty);
}

void EdiPanel::clear()
{
    m_table->setRowCount(0);
    m_segmentName->setText(tr("Segment"));
}

void EdiPanel::showSegment(const EdiSegment &seg)
{
    clear();

    m_segmentName->setText(seg.tag + QLatin1Char(' ') + seg.description);

    int row = 0;
    const bool showEmpty = m_showEmpty->isChecked();

    // Колонки: 0=Value, 1=Element, 2=Component (как в оригинальном listview).
    for (const EdiElement &el : seg.elements)
    {
        if (!el.value.isNull() || showEmpty)
        {
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(el.value));
            m_table->setItem(row, 1, new QTableWidgetItem(el.description));
            m_table->setItem(row, 2, new QTableWidgetItem(QString()));
            ++row;
        }

        for (const EdiComponent &comp : el.components)
        {
            if (!comp.value.isEmpty() || showEmpty)
            {
                m_table->insertRow(row);
                m_table->setItem(row, 0, new QTableWidgetItem(comp.value));
                m_table->setItem(row, 1, new QTableWidgetItem(QString()));
                m_table->setItem(row, 2, new QTableWidgetItem(comp.description));
                ++row;
            }
        }
    }
}
