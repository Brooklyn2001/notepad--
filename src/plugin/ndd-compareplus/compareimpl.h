#ifndef COMPAREIMPL_H
#define COMPAREIMPL_H

#include <QObject>
#include <QString>

class QMenu;
class QWidget;

class CompareImpl : public QObject
{
	Q_OBJECT
public:
	CompareImpl(QWidget* pNotepad, QMenu* pMenu);
	~CompareImpl() override = default;

public slots:
	void compareWithFile();
	void compareWithLast();
	void nextDiff();
	void prevDiff();
	void clearMarkers();

private:
	bool compareTo(const QString& refPath);
	void markDiff(int lineFrom, int lineTo, bool isSelf);
	void navigate(bool forward);

	QWidget* m_pNotepad;
	QMenu*   m_rootMenu;
	QString  m_lastRef;
};

#endif // COMPAREIMPL_H
