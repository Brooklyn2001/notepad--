#include "TextEncoding.h"

#include <QString>

std::wstring utf82ws(std::string utf8) {
    return QString::fromUtf8(utf8.c_str()).toStdWString();
}

std::string ws2utf8(std::wstring ws) {
    return QString::fromStdWString(ws).toUtf8().constData();
}
