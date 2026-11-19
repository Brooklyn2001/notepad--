#include "instanceobj.h"
#include "panel.h"
#include "edianalyzer.h"

#include <QWidget>
#include <QMenu>
#include <QFileInfo>
#include <QDir>
#include <QMainWindow>
#include <QDockWidget>
#include <QMessageBox>
#include <functional>

#include <qsciscintilla.h>
#include <qsciscintillabase.h>

// Редактор-хелпер, передаваемый из главного приложения через NDD_PROC_MAIN.
extern std::function<QsciScintilla*(QWidget*)> s_getCurEdit;

InstanceObj::InstanceObj(QWidget *pNotepad, QWidget *menuParent, const QString &pluginFilePath)
    : QObject(menuParent), m_pNotepad(pNotepad)
{
    m_rootMenu = qobject_cast<QMenu*>(menuParent);

    // Справочник EDI — рядом с DLL плагина (как в оригинале рядом с NppEdiAnalyzer.db).
    m_dbPath = QFileInfo(pluginFilePath).absolutePath() + QDir::separator() + QStringLiteral("NppEdiAnalyzer.db");

    trackCurrentEditor(); // подключаем двойной клик к текущему редактору
}

InstanceObj::~InstanceObj()
{
}

QsciScintilla* InstanceObj::curEdit() const
{
    if (!m_pNotepad)
        return nullptr;
    return s_getCurEdit(m_pNotepad);
}

// Локальная замена всех вхождений str->to по всему документу (Util.ReplaceAll).
static void replaceAll(QsciScintilla *edit, const QString &from, const QString &to)
{
    if (!edit || from.isEmpty())
        return;
    bool ok = edit->findFirst(from, false, false, false, true, true);
    while (ok)
    {
        edit->replace(to);
        ok = edit->findNext();
    }
}

void InstanceObj::formatEdifact()
{
    // Как в оригинале: разбить EDIFACT-сегменты, добавляя перевод строки после терминатора "'".
    QsciScintilla *edit = curEdit();
    if (!edit)
        return;
    edit->beginUndoAction();
    replaceAll(edit, QStringLiteral("'"), QStringLiteral("'\r\n"));
    edit->endUndoAction();
}

void InstanceObj::unFormatEdifact()
{
    QsciScintilla *edit = curEdit();
    if (!edit)
        return;
    edit->beginUndoAction();
    replaceAll(edit, QStringLiteral("'\r\n"), QStringLiteral("'"));
    replaceAll(edit, QStringLiteral("'\n"), QStringLiteral("'"));
    edit->endUndoAction();
}

// Если базы нет — показываем хотя бы найденные сегмент и элементы без описаний.
static EdiSegment segmentPlaceholder(const QString &segStr)
{
    EdiSegment seg;
    seg.tag = segStr.left(3);
    QStringList parts = segStr.split(QLatin1Char('+'));
    for (int i = 1; i < parts.size(); ++i)
    {
        EdiElement el;
        el.value = parts.at(i);
        seg.elements.append(el);
    }
    return seg;
}

void InstanceObj::analyzeLine(int line)
{
    QsciScintilla *edit = curEdit();
    if (!edit)
        return;

    QString mySegment = edit->text(line);
    if (mySegment.isEmpty())
        return;

    // Убрать терминатор строки.
    mySegment.replace(QLatin1Char('\r'), QString());
    mySegment.replace(QLatin1Char('\n'), QString());
    // Убрать EDIFACT-терминатор сегмента "'" в конце.
    if (mySegment.endsWith(QLatin1Char('\'')))
        mySegment.chop(1);
    if (mySegment.isEmpty())
        return;

    if (!m_analyzer.isOpen())
        m_analyzer.open(m_dbPath);

    EdiSegment seg = m_analyzer.isOpen() ? m_analyzer.analyzeSegment(mySegment)
                                         : segmentPlaceholder(mySegment);

    if (m_panel)
        m_panel->showSegment(seg);
}

void InstanceObj::trackCurrentEditor()
{
    QsciScintilla *edit = curEdit();
    if (!edit)
        return;
    // Низкоуровневый сигнал Scintilla: срабатывает на двойной клик.
    // Подключаемся один раз на экземпляр (Qt::UniqueConnection).
    QObject::connect(edit, &QsciScintillaBase::SCN_DOUBLECLICK, this,
                     [this](int /*position*/, int line, int /*modifiers*/) {
                         analyzeLine(line);
                     }, Qt::UniqueConnection);
}

void InstanceObj::showStructureView()
{
    if (!m_dock)
    {
        m_dock = new QDockWidget(tr("EDI Structure View"), m_pNotepad);
        m_dock->setObjectName("nddEdiDock");
        m_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        m_panel = new EdiPanel(m_dock);
        m_dock->setWidget(m_panel);

        // Докируем в главное окно (CCNotePad наследует QMainWindow), либо плавающее окно.
        if (QMainWindow *main = qobject_cast<QMainWindow*>(m_pNotepad))
            main->addDockWidget(Qt::RightDockWidgetArea, m_dock);
        else
            m_dock->setFloating(true);

        m_dock->show();
    }
    else
    {
        m_dock->show();
        m_dock->raise();
    }
}
