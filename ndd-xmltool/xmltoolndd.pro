TEMPLATE	= lib
LANGUAGE	= C++

#QMAKE_CXXFLAGS += /std:c++17

CONFIG	+= qt warn_on Release c++17
QT += core gui widgets

HEADERS	+= *.h \
		 ./QuickXmlLib/QuickXml/src/XmlParser.h \
		 ./QuickXmlLib/QuickXml/src/XmlFormater.h \
		 ./SimpleXmlLib/SimpleXml/src/SimpleXml.h \
		 ./StringXmlLib/StringXml/src/StringXml.h

SOURCES	+= *.cpp
FORMS	+= *.ui


INCLUDEPATH	+= ./QuickXmlLib/QuickXml/src
INCLUDEPATH	+= ./QuickXmlLib/QuickXml/src
INCLUDEPATH	+= ./SimpleXmlLib/SimpleXml/src
INCLUDEPATH	+= ./StringXmlLib/StringXml/src
INCLUDEPATH	+= ../../include
INCLUDEPATH	+= ../../qscint/src
INCLUDEPATH	+= ../../qscint/src/Qsci
INCLUDEPATH	+= ../../qscint/scintilla/include

win32 {
   if(contains(QMAKE_HOST.arch, x86_64)){
    CONFIG(Debug, Debug|Release){
        DESTDIR = ../../x64/Debug/plugin
		LIBS += -L../../x64/Debug -L./lib/debug
		LIBS += -lqmyedit_qt5d
		LIBS += -lStringXml -lSimpleXml -lQuickXml
    }else{
        DESTDIR = ../../x64/Release/plugin
		LIBS += -L../../x64/Release
		LIBS += -lqmyedit_qt5
    }
   }
}

unix {
   if(contains(QMAKE_HOST.arch, x86_64)){
    CONFIG(Debug, Debug|Release){
        DESTDIR = ../../x64/Debug/plugin
                LIBS += -L../../x64/Debug -L./lib/debug
                LIBS += -lqmyedit_qt5d
                LIBS +=  -lSimpleXml -lQuickXml
    }else{
        DESTDIR = ../../x64/Release/plugin
                LIBS += -L../../x64/Release
                LIBS += -L/Users/yinzuowei/work/CCNotePad/plugin/xmltools/SimpleXmlLib/x64/Release/plugin/xmltools
                LIBS += -L/Users/yinzuowei/work/CCNotePad/plugin/xmltools/QuickXmlLib/x64/Release/plugin/xmltools
                LIBS += -lqmyedit_qt5
                 LIBS +=  -lQuickXml -lSimpleXml
    }
   }
}

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
