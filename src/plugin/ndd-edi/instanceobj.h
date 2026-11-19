#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include "edianalyzer.h"

class QWidget;
class QsciScintilla;
class QDockWidget;
class QMenu;
class EdiPanel;

// Главный объект плагина ndd-edi: держит редактор, анализатор и панель,
// реализует пункты меню (Format/Un-Format/Structure View) и анализ по клику.
class InstanceObj : public QObject
{
    Q_OBJECT
public:
    InstanceObj(QWidget *pNotepad, QWidget *menuParent, const QString &pluginFilePath);
    ~InstanceObj();

    QMenu *m_rootMenu = nullptr; // корень подменю плагина (из NDD_PROC_DATA), заполняется экспортным файлом

    // Пункты меню
    void formatEdifact();
    void unFormatEdifact();
    void showStructureView();

    // Анализ строки под указанной позицией (0-based line). Используется и меню,
    // и обработчиком двойного клика.
    void analyzeLine(int line);

private:
    QsciScintilla* curEdit() const;
    // Подключает обработчик двойного клика к текущему редактору (однократно на экземпляр).
    void trackCurrentEditor();

    QString m_dbPath;                 // путь к NppEdiAnalyzer.db (в каталоге плагина)
    QWidget *m_pNotepad = nullptr;    // главное окно CCNotePad
    QPointer<QDockWidget> m_dock;     // докируемая панель Structure View
    EdiPanel *m_panel = nullptr;      // содержимое панели
    EdiAnalyzer m_analyzer;           // парсер + SQLite-доступ
};
