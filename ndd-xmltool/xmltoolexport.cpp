
#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#ifdef WIN32
#include <Windows.h>
#endif

#include "instanceobj.h"
#include "setting.h"

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
	NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla* ()>getCurEdit, std::function<bool(int, void*)> pluginCallBack, NDD_PROC_DATA* procData);


#ifdef __cplusplus
	}
#endif

std::function<QsciScintilla* ()> s_getCurEdit;
std::function<bool (int, void*)> s_invokeMainFun;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if(pProcData == NULL)
	{
		return false;
	}
	pProcData->m_strPlugName = QObject::tr("XML Tool");
	pProcData->m_strComment = QObject::tr("XML Tool For Ndd");

	pProcData->m_version = QString("v1.0");
	pProcData->m_auther = QString("zuowei.yin");

	pProcData->m_menuType = 1;

	return true;
}

//则点击菜单栏按钮时，会自动调用到该插件的入口点函数。
//pNotepad:就是CCNotepad的主界面指针
//strFileName:当前插件DLL的全路径，如果不关心，则可以不使用
//getCurEdit:从NDD主程序传递过来的仿函数，通过该函数获取当前编辑框操作对象QsciScintilla
//pProcData:如果pProcData->m_menuType = 0 ,则该指针为空；如果pProcData->m_menuType = 1，则该指针有值。目前需要关心s_procData.m_rootMenu
//开发者可以在该菜单下面，自行创建二级菜单
int NDD_PROC_MAIN(QWidget* pNotepad, const QString &strFileName, std::function<QsciScintilla*()>getCurEdit, std::function<bool(int, void*)> pluginCallBack,NDD_PROC_DATA* pProcData)
{
	InstanceObj* pInstance = nullptr;

	if (pProcData != nullptr)
	{
		pInstance = new InstanceObj(pNotepad, pProcData->m_rootMenu);
	}
	else
	{
		return -1;
	}

	s_getCurEdit = getCurEdit;
	s_invokeMainFun = pluginCallBack;

	pProcData->m_rootMenu->addAction("Pretty print", pInstance, &InstanceObj::nddPrettyPrintXmlFast);
	pProcData->m_rootMenu->addAction("Pretty print - indent attributes", pInstance, &InstanceObj::nddPrettyPrintXmlAttrFast);
	pProcData->m_rootMenu->addAction("Pretty print - indent only", pInstance, &InstanceObj::nddPrettyPrintXmlIndentOnlyFast);
	pProcData->m_rootMenu->addAction("Linearize", pInstance, &InstanceObj::nddLinearizeXmlFast);
	pProcData->m_rootMenu->addSeparator();
	pProcData->m_rootMenu->addAction("Options...", pInstance, &InstanceObj::optionsWin);

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
			Setting::destorySet();
		}
		break;
	}
	return TRUE;
}
#else
void onDllUnload(void)
{
	Setting::destorySet();
}
#endif
