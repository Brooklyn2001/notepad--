//this file a header file of JSMinNpp
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

#ifndef NPP_TOOLS_H
#define NPP_TOOLS_H

#pragma warning(disable:4005)
class QsciScintilla;

struct StruOptions
{
	int nPutCR; // 是否输出 \r\n
	char chIndent; // 缩进用的字符
	int nChPerInd; // 每个缩进使用几个字符
	bool bNLBracket; // { 之前是否换行
	bool bKeepTopComt; // 是否保留头部的注释
	bool bIndentInEmpty; // 是否保留空行中的缩进
	//bool bDisableVersionCheck; // 是否关闭自动版本检查

	StruOptions():nPutCR(0), bKeepTopComt(false), bIndentInEmpty(false), bNLBracket(false), chIndent(' '), nChPerInd(4)
	{

	}
};

static bool s_updateThreadRunning = false;
static time_t s_lastUpdateCheckTime = 0;
static bool s_foundNewVersion = false;
static int s_newVersionItemIdx = 0;


const int EOL_AUTO = 0;
const int EOL_CRLF = 1; // \r\n
const int EOL_LF = 2; // \n


void loadOption();
bool getScintillaEolCR(QsciScintilla* hScintilla);
bool guessJson(const std::string& jsCode);

#endif //NPP_TOOLS_H
