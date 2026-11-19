#pragma once

#include <QString>
#include <QList>
#include <QSqlDatabase>

// Структуры EDI-анализа (EDIFACT), по образцу EDIAnalizer.cs оригинального
// NppEdiAnalyzer. Данные сопоставляются с SQLite-справочником NppEdiAnalyzer.db.

struct EdiComponent
{
    QString tag;          // идентификатор компонента
    QString description;  // описание
    int minLength = 0;
    int maxLength = 0;
    QString dataType;     // тип данных
    QString refTable;     // справочная таблица
    QString value;        // значение из разобранной строки
};

struct EdiElement
{
    QString tag;
    QString description;
    QString value;
    QString refTable;
    QList<EdiComponent> components;
};

struct EdiSegment
{
    QString tag;         // первые 3 символа строки сегмента
    QString description;
    QList<EdiElement> elements;
};

// Парсинг строки EDIFACT-сегмента + сопоставление со справочником SQLite.
class EdiAnalyzer
{
public:
    // Открывает базу по пути dbPath (пустая строка = не открыта драйвером заранее).
    // Возвращает true, если база открылась и есть драйвер QSQLITE.
    bool open(const QString &dbPath);

    void close();
    bool isOpen() const;

    // Разбирает текущую строку сегмента и наполняет её структурой из базы.
    // myString — строка сегмента БЕЗ завершающего терминатора (например "UNH+1+ORDERS:D:96A").
    // Returns: заполненный сегмент (пустой tag, если строка невалидна).
    EdiSegment analyzeSegment(const QString &myString);

    QSqlDatabase database() const { return m_db; }

private:
    QSqlDatabase m_db;
};
