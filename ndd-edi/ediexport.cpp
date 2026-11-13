#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#include "instanceobj.h"
#ifdef WIN32
#include <Windows.h>
#endif

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
	#if defined(NDD_EXPORTDLL)
		#define NDD_EXPORT __declspec(dllexport)
	#else
		#define NDD_EXPORT __declspec(dllimport)
	#endif
#else
	#define NDD_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
	extern "C" {
#endif

	NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData);
	NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla* (QWidget*)>getCurEdit, std::function<bool(QWidget*, int, void*)> pluginCallBack, NDD_PROC_DATA* procData);

#ifdef __cplusplus
	}
#endif

//static NDD_PROC_DATA s_procData;

std::function<QsciScintilla* (QWidget*)> s_getCurEdit;
std::function<bool (QWidget*, int, void*)> s_invokeMainFun;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if (pProcData == NULL)
	{
		return false;
	}
	pProcData->m_strPlugName = QObject::tr("EDI Analyzer");
	pProcData->m_strComment = QObject::tr("Plugin for Notepad-- to analyze EDI standard formats (Edifact/X12) by brooklyn");

	pProcData->m_version = QString("v1.0");
	pProcData->m_auther = QString("brooklyn");

	// Создаём подменю: три пункта (Format / Un-Format / Structure View).
	pProcData->m_menuType = 1;

	return true;
}

int NDD_PROC_MAIN(QWidget* pNotepad, const QString &strFileName, std::function<QsciScintilla*(QWidget*)>getCurEdit, std::function<bool(QWidget*, int, void*)> pluginCallBack, NDD_PROC_DATA* pProcData)
{
	InstanceObj* pInstanse = nullptr;

	// m_menuType = 1: pProcData->m_rootMenu — корень подменю, где создаём свои пункты.
	if (pProcData != nullptr)
	{
		pInstanse = new InstanceObj(pNotepad, pProcData->m_rootMenu, strFileName);
		pInstanse->setObjectName("nddplg");
	}
	else
	{
		return -1;
	}

	s_getCurEdit = getCurEdit;
	s_invokeMainFun = pluginCallBack;

	pInstanse->m_rootMenu->addAction(QObject::tr("Format EDIFACT"), [=]() {
		pInstanse->formatEdifact();
	}, QKeySequence("Alt+Down"));

	pInstanse->m_rootMenu->addAction(QObject::tr("Un-Format EDIFACT"), [=]() {
		pInstanse->unFormatEdifact();
	}, QKeySequence("Alt+Up"));

	pInstanse->m_rootMenu->addSeparator();

	pInstanse->m_rootMenu->addAction(QObject::tr("Structure View"), [=]() {
		pInstanse->showStructureView();
	});

	return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (NULL == lpvReserved) {
		}
		break;
	}
	return TRUE;
}
#else
void onDllUnload(void)
{
}
#endif
