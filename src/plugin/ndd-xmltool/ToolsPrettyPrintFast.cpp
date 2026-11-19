#include <sstream>
#include "ToolsPrettyPrintFast.h"
#include "XmlParser.h"
#include "XmlFormater.h"
#include "SimpleXml.h"
#include "ScintillaDoc.h"
#include "instanceobj.h"


#ifdef Q_OS_WIN
#include "StringXml.h"
#endif

#include <qsciscintilla.h>


extern XmltoolsOptions xmltoosOptions;

void sciDocPrettyPrintQuickXml_IndentOnly(ScintillaDoc& doc)
{
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoosOptions.maxIndentLevel;
    params.ensureConformity = xmltoosOptions.ensureConformity;
    params.autoCloseTags = xmltoosOptions.ppAutoclose;
    params.indentAttributes = true;
    params.indentOnly = true;
    params.applySpacePreserve = xmltoosOptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
       // dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXml_IndentOnly(ScintillaDoc& doc) {
    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoosOptions.ppAutoclose;
    if (xmltoosOptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoosOptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = true;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    std::stringstream* prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        //dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //outText.clear();
}

#ifdef Q_OS_WIN
void sciDocPrettyPrintStringXml_IndentOnly(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoosOptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrintIndent();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        //dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}
#endif

void sciDocLinearizeQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoosOptions.maxIndentLevel;
    params.ensureConformity = xmltoosOptions.ensureConformity;
    params.autoCloseTags = xmltoosOptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoosOptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.linearize();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";

    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocLinearizeSimpleXml(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    SimpleXml::PrettyPrintParms parms;
    parms.eol = "";
    parms.tab = "";
    parms.insertIndents = false;
    parms.insertNewLines = false;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoosOptions.ppAutoclose;
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    SimpleXml::ChunkedStream s(inText.text, inText.length);
    auto prettyPrinter = SimpleXml::PrettyPrinter(s, parms);
    prettyPrinter.Convert();
    inText.FreeMemory();
    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";

    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //outText.clear();
}

#ifdef Q_OS_WIN
void sciDocLinearizeStringXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoosOptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.linearize();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";

    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}
#endif

void sciDocPrettyPrintQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoosOptions.maxIndentLevel;
    params.ensureConformity = xmltoosOptions.ensureConformity;
    params.autoCloseTags = xmltoosOptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoosOptions.applySpacePreserve;

    //auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

   // auto docclock_end = clock();

    /*{
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
    }*/

    inText.FreeMemory();

    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoosOptions.ppAutoclose;
    if (xmltoosOptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoosOptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
       // dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //outText.clear();
    doc.SetScrollWidth(80); // 80 is arbitrary
}

#ifdef Q_OS_WIN
void sciDocPrettyPrintStringXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoosOptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
       // dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}
#endif

void sciDocPrettyPrintQuickXmlAttr(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoosOptions.maxIndentLevel;
    params.ensureConformity = xmltoosOptions.ensureConformity;
    params.autoCloseTags = xmltoosOptions.ppAutoclose;
    params.indentAttributes = true;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoosOptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        //dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXmlAttr(ScintillaDoc& doc) {
    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoosOptions.ppAutoclose;
    if (xmltoosOptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoosOptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = false;
    parms.indentAttributes = true;

    auto docclock_start = clock();
    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        //dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //outText.clear();
    doc.SetScrollWidth(80); // 80 is arbitrary
}

#ifdef Q_OS_WIN
void sciDocPrettyPrintStringXmlAttr(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoosOptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrintAttr();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        //dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}
#endif
