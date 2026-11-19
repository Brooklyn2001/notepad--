#include "instanceobj.h"
#include <qsciscintilla.h>
#include <Scintilla.h>

#include <QWidget>
#include <QMenu>
#include <QMessageBox>

#include "ScintillaDoc.h"
#include "ToolsPrettyPrintFast.h"
#include "setting.h"

#include <functional>
#include <qsciscintilla.h>
#include <Scintilla.h>

XmltoolsOptions xmltoosOptions;

extern std::function<QsciScintilla* ()> s_getCurEdit;
extern std::function<bool(int, void*)> s_invokeMainFun;

InstanceObj::InstanceObj(QWidget* pNotepad, QMenu* pMenu) :QObject(pNotepad), m_isLoadOpt(false)
{
	m_pNotepad = pNotepad;
	m_rootMenu = pMenu;
}

InstanceObj::~InstanceObj()
{

}

//����һ�β�������
void InstanceObj::init()
{
    if (!m_isLoadOpt)
    {
        m_isLoadOpt = true;
        loadOption();
    }
}


void InstanceObj::nddLinearizeXmlFast()
{
    QsciScintilla* pEdit = s_getCurEdit();
    if (pEdit == nullptr)
    {
        return;
    }
    init();

    ScintillaDoc doc = ScintillaDoc(pEdit);

    if (xmltoosOptions.formatingEngine == 0) {
        sciDocLinearizeQuickXml(doc);
    }
    else if (xmltoosOptions.formatingEngine == 1) {
                #ifdef Q_OS_WIN
        sciDocLinearizeStringXml(doc);
#endif
    }
    else {
        sciDocLinearizeSimpleXml(doc);
    }

    doc.SetXOffset(0);
}


void InstanceObj::nddPrettyPrintXmlIndentOnlyFast()
{
    QsciScintilla* pEdit = s_getCurEdit();
    if (pEdit == nullptr)
    {
        return;
    }
    init();
    ScintillaDoc doc = ScintillaDoc(pEdit);

    if (xmltoosOptions.formatingEngine == 0) {
        sciDocPrettyPrintQuickXml_IndentOnly(doc);
    }
    else if (xmltoosOptions.formatingEngine == 1) {
                #ifdef Q_OS_WIN
        sciDocPrettyPrintStringXml_IndentOnly(doc);
#endif
    }
    else {
        sciDocPrettyPrintSimpleXml_IndentOnly(doc);
    }

    doc.SetXOffset(0);
}

void InstanceObj::nddPrettyPrintXmlFast()
{
    QsciScintilla* pEdit = s_getCurEdit();
    if (pEdit == nullptr)
    {
        return;
    }
    init();
    ScintillaDoc doc = ScintillaDoc(pEdit);

    if (xmltoosOptions.formatingEngine == 0) {
        sciDocPrettyPrintQuickXml(doc);
    }
    else if (xmltoosOptions.formatingEngine == 1) {
                #ifdef Q_OS_WIN
        sciDocPrettyPrintStringXml(doc);
#endif
    }
    else {
        sciDocPrettyPrintSimpleXml(doc);
    }

    doc.SetXOffset(0);
}

void InstanceObj::nddPrettyPrintXmlAttrFast()
{
    QsciScintilla* pEdit = s_getCurEdit();
    if (pEdit == nullptr)
    {
        return;
    }
    init();
    ScintillaDoc doc = ScintillaDoc(pEdit);

    if (xmltoosOptions.formatingEngine == 0) {
        sciDocPrettyPrintQuickXmlAttr(doc);
    }
    else if (xmltoosOptions.formatingEngine == 1) {
                #ifdef Q_OS_WIN
        sciDocPrettyPrintStringXmlAttr(doc);
#endif
    }
    else {
        sciDocPrettyPrintSimpleXmlAttr(doc);
    }

    doc.SetXOffset(0);
}

void InstanceObj::optionsWin()
{
    init();
    Setting* pWin = new Setting(m_pNotepad);
    pWin->setAttribute(Qt::WA_DeleteOnClose);
    pWin->setWindowFlag(Qt::Window);
    pWin->show();
}

//���ز��������ļ��ж�ȡ���ò�����
void InstanceObj::loadOption()
{
    Setting::readSet(true);
}
