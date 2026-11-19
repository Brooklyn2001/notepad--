/* ndd-compareplus — адаптация диф-ядра ComparePlus (pnedev) под NDD.
 * Диф-ядро (diff.h/diff_types.h/histogram_diff.h/myers_diff.h) взято из
 * ComparePlus (C) Pavel Nedev <pg.nedev@gmail.com>, лицензия GPL-3.0.
 * NDD-обёртка: сравнение текущего документа с файлом по строкам,
 * маркеры + навигация prev/next через QsciScintilla.
 */

#include <QObject>
#include <QString>
#include <QMenu>
#include <functional>
#include <qsciscintilla.h>
#include <pluginGl.h>
#include "compareimpl.h"

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
#if defined(NDD_EXPORTDLL)
#define NDD_EXPORT __declspec(dllexport)
#else
#define NDD_EXPORT __declspec(dllimport)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData);
NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla*()> getCurEdit, std::function<bool(int, void*)> pluginCallBack, NDD_PROC_DATA* procData);

#ifdef __cplusplus
}
#endif

std::function<QsciScintilla*()> s_getCurEdit;
std::function<bool(int, void*)> s_invokeMainFun;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if (pProcData == nullptr)
		return false;

	pProcData->m_strPlugName = QObject::tr("ComparePlus");
	pProcData->m_strComment = QObject::tr("File compare (diff core from pnedev/ComparePlus)");
	pProcData->m_version = QString("v0.1-ndd");
	pProcData->m_auther = QString("comparePlus (pnedev) / ndd port");

	pProcData->m_menuType = 1;

	return true;
}

int NDD_PROC_MAIN(QWidget* pNotepad, const QString& /*strFileName*/,
		std::function<QsciScintilla*()> getCurEdit, std::function<bool(int, void*)> pluginCallBack,
		NDD_PROC_DATA* pProcData)
{
	if (pProcData == nullptr)
		return -1;

	s_getCurEdit = getCurEdit;
	s_invokeMainFun = pluginCallBack;

	CompareImpl* pImpl = new CompareImpl(pNotepad, pProcData->m_rootMenu);
	pImpl->setObjectName("nddplg");

	pProcData->m_rootMenu->addAction(QObject::tr("Compare with file..."), [=]() {
		pImpl->compareWithFile();
	});

	pProcData->m_rootMenu->addAction(QObject::tr("Compare against last reference"), [=]() {
		pImpl->compareWithLast();
	});

	pProcData->m_rootMenu->addSeparator();

	pProcData->m_rootMenu->addAction(QObject::tr("Next difference"), [=]() {
		pImpl->nextDiff();
	});

	pProcData->m_rootMenu->addAction(QObject::tr("Previous difference"), [=]() {
		pImpl->prevDiff();
	});

	pProcData->m_rootMenu->addSeparator();

	pProcData->m_rootMenu->addAction(QObject::tr("Clear compare markers"), [=]() {
		pImpl->clearMarkers();
	});

	return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE /*hInst*/, DWORD /*fdwReason*/, LPVOID /*lpvReserved*/)
{
	return TRUE;
}
#endif
