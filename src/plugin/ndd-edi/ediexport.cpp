#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
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

std::function<QsciScintilla* (QWidget*)> s_getCurEdit;
std::function<bool (QWidget*, int, void*)> s_invokeMainFun;
InstanceObj* s_instanceObj = nullptr;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if (pProcData == NULL)
		return false;
		
	pProcData->m_strPlugName = QObject::tr("EDI Analyzer");
	pProcData->m_strComment = QObject::tr("Plugin for Notepad-- to analyze EDI standard formats (Edifact/X12) by brooklyn");
	pProcData->m_version = QString("v1.0");
	pProcData->m_auther = QString("brooklyn");

	// Создаем корневое меню
	QMenu* menu = new QMenu(pProcData->m_strPlugName);
	
	// Добавляем пункты в меню
	QAction* actFormat = menu->addAction(QObject::tr("Format EDIFACT"));
	actFormat->setShortcut(QKeySequence("Alt+Down"));
	actFormat->setObjectName("actionFormat");

	QAction* actUnFormat = menu->addAction(QObject::tr("Un-Format EDIFACT"));
	actUnFormat->setShortcut(QKeySequence("Alt+Up"));
	actUnFormat->setObjectName("actionUnFormat");

	menu->addSeparator();

	QAction* actStruct = menu->addAction(QObject::tr("Structure View"));
	actStruct->setObjectName("actionStructureView");

	// В NDD для подменю нужно передать menuAction() и установить m_menuType = 0
	pProcData->m_pAction = menu->menuAction();
	pProcData->m_menuType = 0; 

	return true;
}

int NDD_PROC_MAIN(QWidget* pNotepad, const QString &strFileName, std::function<QsciScintilla*(QWidget*)>getCurEdit, std::function<bool(QWidget*, int, void*)> pluginCallBack, NDD_PROC_DATA* pProcData)
{
	if (pProcData == nullptr)
		return -1;

	// Инициализируем InstanceObj
	if (s_instanceObj == nullptr) {
		s_instanceObj = new InstanceObj(pNotepad, pProcData->m_pAction->menu(), strFileName);
		s_instanceObj->setObjectName("nddplg");
		
		// Привязываем сигналы действий к методам InstanceObj
		if (pProcData->m_pAction && pProcData->m_pAction->menu()) {
			QMenu* menu = pProcData->m_pAction->menu();
			
			if (QAction* act = menu->findChild<QAction*>("actionFormat"))
				QObject::connect(act, &QAction::triggered, s_instanceObj, &InstanceObj::formatEdifact);
				
			if (QAction* act = menu->findChild<QAction*>("actionUnFormat"))
				QObject::connect(act, &QAction::triggered, s_instanceObj, &InstanceObj::unFormatEdifact);
				
			if (QAction* act = menu->findChild<QAction*>("actionStructureView"))
				QObject::connect(act, &QAction::triggered, s_instanceObj, &InstanceObj::showStructureView);
		}
	}

	s_getCurEdit = getCurEdit;
	s_invokeMainFun = pluginCallBack;

	return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (NULL == lpvReserved) {
			// Ничего не удаляем, чтобы избежать краша при выгрузке, если меню еще живо
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