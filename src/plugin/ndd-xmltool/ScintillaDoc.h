#pragma once
#include <qsciscintilla.h>
#include <Scintilla.h>
#include <string>

struct ScintillaDoc {
	QsciScintilla* hCurrentEditView;

	bool inSelection;

	struct sciWorkText {
		char* text;
		intptr_t length;
		intptr_t selstart;

		operator bool() {
			return text != NULL;
		}

		void FreeMemory() {
			if (text) {
				delete[] text;
				text = NULL;
			}
		}
	};

	ScintillaDoc(QsciScintilla* scHandle) {
		hCurrentEditView = scHandle;
		inSelection = false;
	}

	bool IsReadOnly() {
		return 0 != (int)hCurrentEditView->SendScintilla(SCI_GETREADONLY, 0, (long)0);
	}

	int GetTabWidth() {
		int tabwidth = (int)hCurrentEditView->SendScintilla(SCI_GETTABWIDTH, 0, (long)0);
		if (tabwidth <= 0) tabwidth = 4;
		
		return tabwidth;
	}

	bool UseTabs() {
		return (bool)hCurrentEditView->SendScintilla(SCI_GETUSETABS, 0, (long)0);
	}

	std::string Tab() {
		if (UseTabs()) {
			return "\t";
		}
		else {
			std::string tab;
			int tabwidth = GetTabWidth();
			for (int i = 0; i < tabwidth; i++) {
				tab.append(" ");
			}

			return tab;
		}
	}
	
	std::string EOL() {
		int eol = (int)hCurrentEditView->SendScintilla(SCI_GETEOLMODE, 0, (long)0);
		switch (eol) {
			case SC_EOL_CR:		return "\r";
			case SC_EOL_LF:		return "\n";
			case SC_EOL_CRLF:
			default:			return "\r\n";
		}
	}

	Sci_PositionCR SelectionStart() {
		return (Sci_PositionCR)hCurrentEditView->SendScintilla(SCI_GETSELECTIONSTART, 0, (long)0);
	}

	Sci_PositionCR SelectionEnd() {
		return (Sci_PositionCR)hCurrentEditView->SendScintilla(SCI_GETSELECTIONEND, 0, (long)0);
	}

	void SetCurrentPosition(Sci_PositionCR selstart) {
		hCurrentEditView->SendScintilla(SCI_SETCURRENTPOS, selstart, (long)0);
	}

	void SetAnchor(Sci_PositionCR selend) {
		hCurrentEditView->SendScintilla(SCI_SETANCHOR, selend, (long)0);
	}

	void ClearAll() {
		hCurrentEditView->SendScintilla(SCI_CLEARALL, 0, (long)0);
	}

	void SetText(const char* text) {
		hCurrentEditView->SendScintilla(SCI_SETTEXT, (uintptr_t)0, text);
	}

	void ReplaceSelection(const char* text) {
		hCurrentEditView->SendScintilla(SCI_REPLACESEL, (uintptr_t)0, text);
	}

	Sci_PositionCR GetTextLength() {
		return (Sci_PositionCR)hCurrentEditView->SendScintilla(SCI_GETLENGTH, 0, (long)0);
	}

	char *GetSelectedText(size_t length) {
		char *text = new char[length + sizeof(char)];
		if (text == NULL)
		{
			return nullptr;  // allocation error, abort check;
		}
		text[length] = 0;
		hCurrentEditView->SendScintilla(SCI_GETSELTEXT, (uintptr_t)0, text);
		return text;
	}

	sciWorkText GetSelectedText() {
		auto selstart = SelectionStart();
		auto selend = SelectionEnd();
		if (selend > selstart) {
			auto len = selend - selstart;
			return { GetSelectedText(len), (intptr_t) len, (intptr_t)selstart };
		}
		return { NULL,0,0 };
	}

	char* GetText(Sci_PositionCR length) {
		char *text = new char[length + sizeof(char)];
		if (text == NULL) return nullptr;  // allocation error, abort check
		text[length] = 0;
		hCurrentEditView->SendScintilla(SCI_GETTEXT, length + sizeof(char), text);
		return text;
	}

	size_t GetText(Sci_PositionCR offset, char *target, Sci_PositionCR length) {
		if (target == NULL) return 0;  // allocation error, abort check
		long max = GetTextLength();

		if (offset >= max) return 0;

		Sci_TextRange range;
		range.chrg.cpMin = offset;
		range.chrg.cpMax = offset+length;
		range.lpstrText = target;

		if (range.chrg.cpMax > (Sci_PositionCR)max) {
			range.chrg.cpMax = (Sci_PositionCR)max;
		}
		
		return hCurrentEditView->SendScintilla(SCI_GETTEXTRANGE, range.chrg.cpMin, range.chrg.cpMax, range.lpstrText);
	}

	sciWorkText GetWorkText() {
		auto selstart = this->SelectionStart();
		auto selend = this->SelectionEnd();

		sciWorkText ret;

		if (selend > selstart) { // selection
			ret.length = selend - selstart;
			inSelection = true;
			ret.text = this->GetSelectedText(ret.length);
			ret.selstart = selstart;
		}
		else {
			ret.length = this->GetTextLength();
			inSelection = false;
			ret.text = this->GetText(ret.length);
			ret.selstart = -1;
		}

		return ret;
	}

	void SetWorkText(const char* text) {
		if (inSelection) {
			this->ReplaceSelection(text);
		}
		else {
			this->SetText(text);
		}
	}

	void SetXOffset(int offset = 0) {
		hCurrentEditView->SendScintilla(SCI_SETXOFFSET, offset, (long)0);
	}

	void SetScrollWidth(int width) {
		hCurrentEditView->SendScintilla(SCI_SETSCROLLWIDTH, width, (long)0);
	}

	void EmptyUndoBuffer() {
		hCurrentEditView->SendScintilla(SCI_EMPTYUNDOBUFFER, 0, (long)0);
	}
};
