#include "instanceobj.h"
#include <qsciscintilla.h>
#include <Scintilla.h>

#include <QWidget>
#include <QMenu>
#include <QMessageBox>
#include "jsminndd.h"
#include "setting.h"
#include "jsminCharArray.h"

#include "version.h"
#include "jsformatString.h"


extern std::function<QsciScintilla* ()> s_getCurEdit;
extern std::function<bool(int, void*)> s_invokeMainFun;

struct StruOptions g_struOptions;

InstanceObj::InstanceObj(QWidget* pNotepad, QMenu* pMenu) :QObject(pNotepad)
{
	m_pNotepad = pNotepad;
	m_rootMenu = pMenu;
}

InstanceObj::~InstanceObj()
{

}

void InstanceObj::optionsWin()
{
	Setting* pWin = new Setting(m_pNotepad);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->setWindowFlag(Qt::Window);
	pWin->show();
}


void InstanceObj::jsMin(bool isNewFile)
{
	
	QsciScintilla* hCurrScintilla = s_getCurEdit();
	if (hCurrScintilla == nullptr)
	{
		return;
	}
	size_t jsLen = hCurrScintilla->length();
	if (jsLen == 0)
	{
		return;
	}

	QString text = hCurrScintilla->text();

	QByteArray bytes = text.toUtf8();
	jsLen = bytes.size();
	char* pJS = bytes.data();

	size_t jsMinLen = jsLen + 10; // seem to be something wrong, so add some empty places

	unsigned char* pJSMin = new unsigned char[jsMinLen];
	memset(pJSMin, 0, jsMinLen);

	try
	{
		bool _bPutCR = true;
		int _nPutCR = g_struOptions.nPutCR;
		switch (_nPutCR)
		{
		case EOL_AUTO:
			if (getScintillaEolCR(hCurrScintilla))
				_bPutCR = true;
			else
				_bPutCR = false;
			break;
		case EOL_CRLF:
			_bPutCR = true;
			break;
		case EOL_LF:
			_bPutCR = false;
			break;
		}

		JSMinCharArray jsmin((const unsigned char*)pJS, pJSMin, _bPutCR, g_struOptions.bKeepTopComt);
		jsmin.go();

		QString result((char*)pJSMin);
		result = result.trimmed();

		if (isNewFile)
		{
			// Open a new document
			s_invokeMainFun(1, nullptr);

			// ReGet the current scintilla
			hCurrScintilla = s_getCurEdit();
		}

		QByteArray newText = result.toUtf8();
		newText.append('\0');
		const char* pText = newText.data();

		//hCurrScintilla->setText(result);
		hCurrScintilla->SendScintilla(SCI_SETTEXT, (uintptr_t)0, (const char*)pText);

		// Set file's language
		int newIdmLang = 0; //0 js
		if (guessJson(string((char*)pJSMin)))
		{
			newIdmLang = 1; //1 json
		}

		//���ߵ�ǰ�༭���ļ����﷨�ļ���
		s_invokeMainFun(2, &newIdmLang);
	}
	catch (runtime_error ex)
	{
		QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("JS Min Failed !"));
	}

	delete[] pJSMin;
}

void InstanceObj::jsMinCurrent(bool isNewFile)
{
	jsMin(isNewFile);
}

void makeFormatOption(QsciScintilla* hCurrScintilla, FormatterOption* pFormatterOption)
{
	int _nChPerInd = g_struOptions.nChPerInd;
	if (g_struOptions.chIndent == '\t')
	{
		_nChPerInd = 1;
	}

	CR_PUT _eCRPut = PUT_CR;
	int _nPutCR = g_struOptions.nPutCR;
	switch (_nPutCR)
	{
	case EOL_AUTO:
		if (getScintillaEolCR(hCurrScintilla))
			_eCRPut = PUT_CR;
		else
			_eCRPut = NOT_PUT_CR;
		break;
	case EOL_CRLF:
		_eCRPut = PUT_CR;
		break;
	case EOL_LF:
		_eCRPut = NOT_PUT_CR;
		break;
	}

	pFormatterOption->chIndent = g_struOptions.chIndent;
	pFormatterOption->nChPerInd = _nChPerInd;
	pFormatterOption->eCRPut = _eCRPut;
	pFormatterOption->eBracNL = g_struOptions.bNLBracket ? NEWLINE_BRAC : NO_NEWLINE_BRAC;
	pFormatterOption->eEmpytIndent = g_struOptions.bIndentInEmpty ? INDENT_IN_EMPTYLINE : NO_INDENT_IN_EMPTYLINE;
}

void InstanceObj::jsFormat()
{
	QsciScintilla* hCurrScintilla = s_getCurEdit();

	if (hCurrScintilla == nullptr)
	{
		return;
	}
	size_t jsLen = hCurrScintilla->length();
	if (jsLen == 0)
	{
		return;
	}

	bool bFormatSel = hCurrScintilla->hasSelectedText();

	size_t jsLenSel;
	char* pJS = nullptr;
	string initIndent("");

	int currentPos;
	int line;

	size_t selStart = 0;
	size_t selEnd = 0;

	if (!bFormatSel)
	{
		// ��ʽ��ȫ��
		pJS = new char[jsLen + 1];
		hCurrScintilla->SendScintilla(SCI_GETTEXT, (uintptr_t)(jsLen + 1), (const char*)pJS);

		currentPos = (int)hCurrScintilla->SendScintilla(SCI_GETCURRENTPOS, (long)0, (long)0);
		line = (int)hCurrScintilla->SendScintilla(SCI_LINEFROMPOSITION, (long)currentPos, (long)0);

	}
	else
	{
		selStart = hCurrScintilla->SendScintilla(SCI_GETSELECTIONSTART, (long)0, (long)0);
		selEnd = hCurrScintilla->SendScintilla(SCI_GETSELECTIONEND, (long)0, (long)0);

		// ��ʽ��ѡ�в���
		char testChar;
		// ����ͷ
		while (selStart > 0)
		{
			testChar = (char)hCurrScintilla->SendScintilla(SCI_GETCHARAT, (unsigned long)selStart - 1, (long)0);
			if (testChar == '\r' || testChar == '\n')
			{
				break;
			}

			--selStart;
		}
		// ����β
		while (selEnd < jsLen)
		{
			testChar = (char)hCurrScintilla->SendScintilla(SCI_GETCHARAT, (unsigned long)selEnd, (long)0);
			if (testChar == '\r' || testChar == '\n')
			{
				break;
			}

			++selEnd;
		}

		// ����ѡ������
		hCurrScintilla->SendScintilla(SCI_SETSELECTIONSTART, (unsigned long)selStart, (long)0);
		hCurrScintilla->SendScintilla(SCI_SETSELECTIONEND, (unsigned long)selEnd, (long)0);

		// ��� Sel
		jsLenSel = hCurrScintilla->SendScintilla(SCI_GETSELTEXT, (long)0, (long)0);
		pJS = new char[jsLenSel];
		hCurrScintilla->SendScintilla(SCI_GETSELTEXT, jsLen, (char*)pJS);

		// �õ� Initial Indent
		for (size_t i = 0; i < jsLenSel; ++i)
		{
			testChar = pJS[i];
			if (testChar != ' ' && testChar != '\t')
			{
				break;
			}
			initIndent += testChar;
		}
	}

	try
	{
		FormatterOption formatterOption;
		makeFormatOption(hCurrScintilla, &formatterOption);

		string strJSFormat;
		JSFormatString jsformat(pJS, &strJSFormat, formatterOption);
		if (bFormatSel)
		{
			jsformat.SetInitIndent(initIndent);
		}
		jsformat.Go();

		if (!bFormatSel)
		{
			hCurrScintilla->SendScintilla(SCI_SETTEXT, (uintptr_t)0, (const char*)(strJSFormat.c_str()));
			int newIdmLang = 0;
			if (guessJson(strJSFormat))
			{
				newIdmLang = 1;
			}
			//���ߵ�ǰ�༭���ļ����﷨�ļ���
			s_invokeMainFun(2, &newIdmLang);

			// line starts from 0, lineFixed starts from 1
			int lineJSF = line + 1;
			// formattedLine starts from 0, formattedLineJSF starts from 1
			int formattedLineJSF = jsformat.GetFormattedLine(lineJSF);
			int formattedLine = formattedLineJSF - 1;

			const int bigLineJump = 10; // for better page scrolling position
			hCurrScintilla->SendScintilla(SCI_GOTOLINE, formattedLine + bigLineJump, (long)0);
			hCurrScintilla->SendScintilla(SCI_GOTOLINE, formattedLine, (long)0);
		}
		else
		{
			// ����������Ļ���
			if ((strJSFormat.length() >= 2) && strJSFormat[strJSFormat.length() - 2] == '\r')
			{
				strJSFormat = strJSFormat.substr(0, strJSFormat.length() - 2);
			}
			else
			{
				strJSFormat = strJSFormat.substr(0, strJSFormat.length() - 1);
			}

			hCurrScintilla->SendScintilla(SCI_REPLACESEL, (uintptr_t)0, (const char*)strJSFormat.c_str());
			hCurrScintilla->SendScintilla(SCI_SETCURRENTPOS, (unsigned long)selStart, (long)0);
		}
	}
	catch (exception ex)
	{
		QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("JSFormat Failed !"));
	}

	if (pJS != nullptr)
	{
		delete[] pJS;
	}
}
