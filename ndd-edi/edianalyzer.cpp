#include "edianalyzer.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>

// Порядок колонок и 0-based индексы взяты из оригинального C#-кода
// (NppEdiAnalyzer.EDIAnalizeSelectedLine), чтобы совпадать с реальной структурой
// NppEdiAnalyzer.db:
//   VW_Segment_Elements:    [1]=element_tag, [6]=description,      [9]=element_ref
//   VW_Elements_Components: [1]=tag,         [6]=descri, [7]=min,
//                           [8]=max,         [9]=dataType,         [10]=refTable

static QString col(const QSqlRecord &rec, int idx)
{
    QVariant v = idx < rec.count() ? rec.value(idx) : QVariant();
    return v.isNull() ? QString() : v.toString();
}

static int colInt(const QSqlRecord &rec, int idx)
{
    QVariant v = idx < rec.count() ? rec.value(idx) : QVariant();
    bool ok = false;
    int n = v.toInt(&ok);
    return ok ? n : 0;
}

bool EdiAnalyzer::open(const QString &dbPath)
{
    if (m_db.isValid())
        m_db = QSqlDatabase(); // сброс

    if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
        qWarning() << "[ndd-edi] QSQLITE driver not available";
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", "ndd_edi_conn");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open())
    {
        qWarning() << "[ndd-edi] cannot open DB:" << m_db.lastError().text();
        return false;
    }
    return true;
}

void EdiAnalyzer::close()
{
    if (m_db.isValid())
    {
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase("ndd_edi_conn");
    }
}

bool EdiAnalyzer::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

EdiSegment EdiAnalyzer::analyzeSegment(const QString &myString)
{
    EdiSegment seg;
    if (myString.length() < 3)
        return seg;

    seg.tag = myString.left(3);

    // Элементы разделяются '+', компоненты внутри элемента — ':'.
    QStringList elementParts = myString.split(QLatin1Char('+'));
    if (elementParts.isEmpty())
        return seg;

    // Индекс элемента в базе сдвинут на 1 относительно parts (parts[0] — тег сегмента).
    auto value4Element = [&elementParts](int i) -> QString {
        int idx = i + 1;
        return (idx < elementParts.size()) ? elementParts.at(idx) : QString();
    };

    // Сегмент: описание из справочника.
    QSqlQuery q(m_db);
    if (seg.tag.size() >= 3)
    {
        q.prepare("SELECT * FROM VW_Segment_Elements "
                  "WHERE Segment_TAG = ? ORDER BY Element_Position");
        q.addBindValue(seg.tag);
        q.exec();
    }

    int iElement = 0;
    while (q.next())
    {
        QSqlRecord rec = q.record();

        QString elementTag = col(rec, 1);
        QString elementDesc = col(rec, 6);
        QString elementRef = col(rec, 9);

        EdiElement el;
        el.tag = elementTag;
        el.description = elementDesc;
        el.refTable = elementRef;
        // Фактическое значение элемента (из разобранной строки).
        el.value = value4Element(iElement);

        // Компоненты внутри элемента разделяются ':'. Разбиваем значение элемента.
        QStringList compParts = el.value.split(QLatin1Char(':'));

        // Компоненты этого элемента.
        QSqlQuery qc(m_db);
        qc.prepare("SELECT * FROM VW_Elements_Components "
                   "WHERE element_tag = ? AND component_name LIKE ? "
                   "ORDER BY Component_Position");
        qc.addBindValue(elementTag);
        qc.addBindValue(seg.tag + QStringLiteral("%"));
        qc.exec();

        int iComponent = 0;
        while (qc.next())
        {
            QSqlRecord cr = qc.record();
            EdiComponent comp;
            comp.tag = col(cr, 1);
            comp.description = col(cr, 6);
            comp.minLength = colInt(cr, 7);
            comp.maxLength = colInt(cr, 8);
            comp.dataType = col(cr, 9);
            comp.refTable = col(cr, 10);
            comp.value = (iComponent < compParts.size()) ? compParts.at(iComponent) : QString();

            el.components.append(comp);
            ++iComponent;
        }

        // Если компонентов в иерархии нет — значение относится к самому элементу.
        if (iComponent == 0)
            el.value = value4Element(iElement);

        seg.elements.append(el);
        ++iElement;
    }

    return seg;
}
