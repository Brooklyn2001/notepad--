#pragma once
#include "ScintillaDoc.h"

void sciDocPrettyPrintQuickXml_IndentOnly(ScintillaDoc& doc);

void sciDocPrettyPrintSimpleXml_IndentOnly(ScintillaDoc& doc);

void sciDocLinearizeQuickXml(ScintillaDoc& doc);
void sciDocLinearizeSimpleXml(ScintillaDoc& doc);

void nddPrettyPrintXmlFast();

void sciDocPrettyPrintQuickXml(ScintillaDoc& doc);
void sciDocPrettyPrintSimpleXml(ScintillaDoc& doc);


void sciDocPrettyPrintQuickXmlAttr(ScintillaDoc& doc);
void sciDocPrettyPrintSimpleXmlAttr(ScintillaDoc& doc);


#ifdef Q_OS_WIN

void sciDocPrettyPrintStringXml_IndentOnly(ScintillaDoc& doc);
void sciDocLinearizeStringXml(ScintillaDoc& doc);
void sciDocPrettyPrintStringXml(ScintillaDoc& doc);
void sciDocPrettyPrintStringXmlAttr(ScintillaDoc& doc);
#endif
