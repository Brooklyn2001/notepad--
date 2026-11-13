//this file a source file of JSMinNpp
//Copyright (C) 2007 Don HO <donho@altern.org>
//Copyright (C) 2010-2010 Sun Junwen
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include <time.h>
#include <stdexcept>
#include <string>

//#define DEBUG_MEM_LEAK
#undef DEBUG_MEM_LEAK

#ifdef DEBUG_MEM_LEAK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#include <qsciscintilla.h>
#include <Scintilla.h>
#include <QMessageBox>

#include "jsminndd.h"
#include "version.h"

#include "jsminCharArray.h"
#include "jsformatString.h"

#include "instanceobj.h"
#include "setting.h"



//加载参数，从文件中读取配置参数。
void loadOption()
{
	Setting::readSet(true);
}


bool guessJson(const string& jsCode)
{
	bool maybeJson = false;
	char charJson = 0;
	size_t jsCodeLen = jsCode.length();
	for (size_t i = 0; i < jsCodeLen; ++i)
	{
		char ch = jsCode[i];
		if (ch == ' ' || ch == '\t' ||
			ch == '\r' || ch == '\n')
		{
			continue; // Skip over whitespaces at beginning
		}
		if (ch == '{' || ch == '[')
		{
			maybeJson = true;
			charJson = ch;
		}
		break;
	}

	if (!maybeJson)
	{
		return false;
	}

	long long llCodeLen = jsCodeLen;
	for (long long i = llCodeLen - 1; i >= 0; --i)
	{
		char ch = jsCode[i];
		if (ch == ' ' || ch == '\t' ||
			ch == '\r' || ch == '\n')
		{
			continue; // Skip over whitespaces at the end
		}
		if ((charJson == '{' && ch == '}') ||
			(charJson == '[' && ch == ']'))
		{
			return true;
		}

		break;
	}

	return false;
}

bool getScintillaEolCR(QsciScintilla* hScintilla)
{
	int eolMode = hScintilla->SendScintilla(SCI_GETEOLMODE);
	switch (eolMode)
	{
	case SC_EOL_CRLF:
		return true;
	case SC_EOL_CR:
	case SC_EOL_LF:
		return false;
	}
	return true;
}




 
