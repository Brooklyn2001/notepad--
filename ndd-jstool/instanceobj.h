#pragma once
#include <QObject>
#include <QWidget>

class QMenu;

class InstanceObj :public QObject
{
public:
	//外面Ndd释放时，会自动释放该对象。
	InstanceObj(QWidget* pNotepad, QMenu* pMenu);
	~InstanceObj();

	void jsMin(bool isNewFile);
	void jsMinCurrent(bool isNewFile);
	void jsFormat();
	void optionsWin();

public:
	QWidget* m_pNotepad;
	QMenu* m_rootMenu;

private:
	InstanceObj(const InstanceObj& other) = delete;
	InstanceObj& operator=(const InstanceObj& other) = delete;
};