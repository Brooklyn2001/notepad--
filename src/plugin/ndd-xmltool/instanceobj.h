#pragma once
#include <QObject>
#include <QWidget>

class QMenu;


struct XmltoolsOptions {
	int formatingEngine; //0 "QuickXml" 1 "StringXml") 2 (simplexml)"
	int maxIndentLevel;
	bool ensureConformity;
	bool ppAutoclose;
	bool applySpacePreserve;

	XmltoolsOptions() :formatingEngine(0), maxIndentLevel(0), ensureConformity(true), ppAutoclose(true), applySpacePreserve(false)
	{

	}
};

class InstanceObj :public QObject
{
public:
	//外面Ndd释放时，会自动释放该对象。
	InstanceObj(QWidget* pNotepad, QMenu* pMenu);
	~InstanceObj();

public slots:
	void nddPrettyPrintXmlFast();
	void nddPrettyPrintXmlAttrFast();
	void nddPrettyPrintXmlIndentOnlyFast();
	void nddLinearizeXmlFast();
	void optionsWin();
	void loadOption();

public:
	QWidget* m_pNotepad;
	QMenu* m_rootMenu;

private:
	InstanceObj(const InstanceObj& other) = delete;
	InstanceObj& operator=(const InstanceObj& other) = delete;
	void init();
	bool m_isLoadOpt;
};