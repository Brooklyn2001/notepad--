#include "compareimpl.h"
#include <functional>
#include <vector>
#include <string>
#include <exception>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include <QMenu>
#include <QByteArray>
#include <qsciscintilla.h>
#include <Scintilla.h>

#include "diff/diff.h"

using namespace std;

extern std::function<QsciScintilla*()> s_getCurEdit;

// NDD использует margin 0 (номера строк) и 1 (символы/bookmarks).
// Для маркеров сравнения выделяем margin 2 (SC_MARK_BACKGROUND — подсветка строки).
static const int CMP_MARGIN  = 2;
static const int CMP_MARKER  = 1;
static const unsigned int CMP_MASK = (1u << CMP_MARKER);
static const int CMP_BG = 0x1E26CE; // тёмно-синий фон-подсветка


// Элемент строки для диф-ядра: хешируем строку по её UTF-8 байтам.
struct LineElem : public hash_type<uint64_t>
{
	LineElem(uint64_t h) : hash_type<uint64_t>(h) {}
};

static inline uint64_t hashLine(const QString& line)
{
	const QByteArray bytes = line.toUtf8();
	return std::hash<std::string>{}(std::string(bytes.constData(), bytes.size()));
}


CompareImpl::CompareImpl(QWidget* pNotepad, QMenu* pMenu)
	: QObject(pNotepad), m_pNotepad(pNotepad), m_rootMenu(pMenu)
{
}

void CompareImpl::clearMarkers()
{
	QsciScintilla* ed = s_getCurEdit();
	if (ed == nullptr)
		return;

	ed->markerDeleteAll(CMP_MARKER);
}

void CompareImpl::markDiff(int lineFrom, int lineTo, bool /*isSelf*/)
{
	QsciScintilla* ed = s_getCurEdit();
	if (ed == nullptr)
		return;

	const int lineCount = (int)ed->SendScintilla(SCI_GETLINECOUNT, 0L, 0L);

	if (lineFrom < 0)
		lineFrom = 0;
	if (lineTo >= lineCount)
		lineTo = lineCount - 1;

	for (int l = lineFrom; l <= lineTo; ++l)
		ed->markerAdd(l, CMP_MARKER);
}

// Сравнивает текущий документ с файлом refPath; маркирует отличающиеся строки текущего редактора.
bool CompareImpl::compareTo(const QString& refPath)
{
	QsciScintilla* ed = s_getCurEdit();
	if (ed == nullptr)
		return false;

	QFile f(refPath);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::warning(m_pNotepad, tr("ComparePlus"), tr("Cannot open reference file:\n%1").arg(refPath));
		return false;
	}

	const QString docText = ed->text();
	QString refText;
	{
		QByteArray raw = f.readAll();
		refText = QString::fromUtf8(raw);
	}
	f.close();

	// Строим векторы строк (без финального пустого элемента, порождаемого trailing newline)
	vector<LineElem> a, b;

	{
		const QStringList docLines = docText.split(QLatin1Char('\n'));
		a.reserve(docLines.size());
		for (const QString& ln : docLines)
		{
			QString s = ln;
			if (s.endsWith(QLatin1Char('\r')))
				s.chop(1);
			a.emplace_back(hashLine(s));
		}
		if (!a.empty() && a.back().hash == 0)
			a.pop_back();
	}

	{
		const QStringList refLines = refText.split(QLatin1Char('\n'));
		b.reserve(refLines.size());
		for (const QString& ln : refLines)
		{
			QString s = ln;
			if (s.endsWith(QLatin1Char('\r')))
				s.chop(1);
			b.emplace_back(hashLine(s));
		}
		if (!b.empty() && b.back().hash == 0)
			b.pop_back();
	}

	DiffCalc<LineElem> dc(a, b);

	diff_results diffs;
	try
	{
		diffs = dc(DiffAlg::MIXED, false, false);
	}
	catch (const std::exception&)
	{
		QMessageBox::warning(m_pNotepad, tr("ComparePlus"), tr("Compare failed."));
		return false;
	}

	// Маркеры только в текущем (левом) документе по его a-диапазонам.
	ed->markerDeleteAll(CMP_MARKER);

	// Гарантируем, что margin 2 сконфигурирован для показа наших маркеров.
	ed->SendScintilla(SCI_SETMARGINTYPEN, (unsigned long)CMP_MARGIN, (long)SC_MARGIN_SYMBOL);
	ed->SendScintilla(SCI_MARKERDEFINE, (unsigned long)CMP_MARKER, (long)SC_MARK_BACKGROUND);
	ed->SendScintilla(SCI_MARKERSETBACK, (unsigned long)CMP_MARKER, (long)CMP_BG);
	const int curMask = (int)ed->SendScintilla(SCI_GETMARGINMASKN, (unsigned long)CMP_MARGIN, 0L);
	ed->SendScintilla(SCI_SETMARGINMASKN, (unsigned long)CMP_MARGIN, (long)(curMask | (int)CMP_MASK));
	ed->SendScintilla(SCI_SETMARGINWIDTHN, (unsigned long)CMP_MARGIN, (long)16);

	for (const auto& d : diffs)
	{
		if (d.a.len() > 0)
			markDiff((int)d.a.s, (int)(d.a.e - 1), true);
	}

	QMessageBox::information(m_pNotepad, tr("ComparePlus"),
		tr("Differences: %1").arg(diffs.size()));

	return true;
}

void CompareImpl::compareWithFile()
{
	QsciScintilla* ed = s_getCurEdit();
	if (ed == nullptr)
		return;

	const QString ref = QFileDialog::getOpenFileName(m_pNotepad, tr("Select reference file"));
	if (ref.isEmpty())
		return;

	m_lastRef = ref;
	compareTo(ref);
}

void CompareImpl::compareWithLast()
{
	if (m_lastRef.isEmpty())
	{
		QMessageBox::information(m_pNotepad, tr("ComparePlus"), tr("No reference file yet. Use \"Compare with file...\" first."));
		return;
	}

	compareTo(m_lastRef);
}

void CompareImpl::navigate(bool forward)
{
	QsciScintilla* ed = s_getCurEdit();
	if (ed == nullptr)
		return;

	const long curPos = ed->SendScintilla(SCI_GETCURRENTPOS, 0L, 0L);
	const int curLine = (int)ed->SendScintilla(SCI_LINEFROMPOSITION, (unsigned long)curPos, 0L);
	int target;

	if (forward)
		target = ed->markerFindNext(curLine + 1, CMP_MASK);
	else
		target = ed->markerFindPrevious(curLine - 1, CMP_MASK);

	if (target < 0)
		target = forward ? ed->markerFindNext(0, CMP_MASK) : ed->markerFindPrevious(INT_MAX, CMP_MASK);

	if (target >= 0)
		ed->SendScintilla(SCI_GOTOLINE, (unsigned long)target, 0L);
}

void CompareImpl::nextDiff()
{
	navigate(true);
}

void CompareImpl::prevDiff()
{
	navigate(false);
}
