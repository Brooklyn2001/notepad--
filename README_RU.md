# notepad--

[English](README.md) | [中文](README_CN.md) | [Русский](README_RU.md)

## О проекте

Notepad-- (сокращённо **ndd**) — лёгкий кроссплатформенный текстовый редактор,
написанный на **C++ / Qt Widgets**. Поддерживает Windows, macOS и Linux.

Проект вырос из китайского upstream [cxasm/notepad--](https://gitee.com/cxasm/notepad--)
и развивается как мультиплатформенный аналог Notepad++ с акцентом наWindows, Linux и macOS.

Цель — создание мультиплатформенной альтернативы Notepad++, с фокусом
на разработке под Windows и различные дистрибутивы Linux.

Главное преимущество `ndd` по сравнению с конкурентами класса Notepad —
**кроссплатформенность**: полноценная работа не только под Windows, но и под
Linux и macOS.

Сообщения об ошибках и отзывы принимаются в этом репозитории.

> **Примечание.** Данный репозиторий (`Brooklyn2001/notepad--`) — **новый форк**
> активного рабочего проекта. От upstream он отличается:
> - полной **русской локализацией** (UI + QScintilla);
> - **CMake-системой сборки** (Qt5);
> - набором плагинов, включая **ndd-edi** (EDI-анализатор EDIFACT/X12).
>
> Исходный upstream: [https://github.com/cxasm/notepad--](https://github.com/cxasm/notepad--)
> (зеркало Gitee: [https://gitee.com/cxasm/notepad--](https://gitee.com/cxasm/notepad--)).

## Возможности форка

- **Русская локализация** интерфейса и редактора QScintilla
  (`realcompare_ru.qm`, `qscintilla_ru.qm`); переключение языка —
  авто / Chinese / English / Russian.
- **CMake-сборка под Qt5** (MSVC 2022 + Qt 5.15.14 на Windows).
- **Плагины** (DLL, каталог `build/bin/plugin/`):
  - `helloworld` — пример плагина;
  - `ndd-jstool` — инструменты для JavaScript;
  - `ndd-xmltool` — инструменты для XML;
  - `ndd-compareplus` — сравнение файлов;
  - `ndd-edi` — анализатор EDI-стандартов (**EDIFACT / X12**), нативный C++/Qt,
    справочник на SQLite (`QSQLITE`, без внешних зависимостей).
	
## Плагины

Плагины — это DLL на **нативном C++/Qt**, которые Notepad-- сканирует в каталоге
`<appDir>/plugin` (при сборке — `build/bin/plugin/`). Загружаются через ABI
`NDD_PROC_MAIN` (`src/include/pluginGl.h`). Метаданные (автор, описание, пункты меню)
задаются в `NDD_PROC_IDENTIFY`/`NDD_PROC_DATA`; `m_menuType`: `0` — один пункт меню,
`1` — подменю.

### helloworld
Минимальный пример плагина-«заглушки» (исходники в `src/plugin/`). Показывает
структуру плагина NDD: экспорт `NDD_PROC_MAIN`, регистрацию действия и обработку
вызова. Используется как шаблон при разработке новых плагинов.

### ndd-jstool
Плагин с инструментами для работы с **JavaScript**-кодом в редакторе
(каталог `ndd-jstool/`, собирается как DLL). Предназначен для типовых операций
над JS-текстом прямо из меню плагина.

### ndd-xmltool
Плагин с инструментами для работы с **XML** (каталог `ndd-xmltool/`, DLL).
Предназначен для типовых операций над XML-документами (форматирование/обработка)
из меню плагина.

### ndd-compareplus
Плагин **сравнения файлов** (каталог `ndd-compareplus/`, DLL) — расширение
функциональности сравнения в духе Compare Plus для Notepad++.

### ndd-edi
Адаптация плагина **NppEdiAnalyzer** (для Notepad++) под Notepad--. Написан на
**нативном C++/Qt** (NDD загружает только C++/Qt DLL через `pluginGl.h`).
Анализирует EDI-стандарты **EDIFACT / X12**.

## Сборка (CMake, Qt5)

QScintilla собирается **из исходников** (`add_subdirectory(src/qscint)`) как
статическая библиотека `qmyedit_qt5.lib`. Переход на Qt6 отложен до обновления
QScintilla на версию с поддержкой Qt6.

```powershell
# Windows x64 (MSVC 2022 + Qt 5.15.14); пресет задаёт CMAKE_PREFIX_PATH
cmake --preset windows-x64
cmake --build --preset windows-x64
# или напрямую:
cmake --build build --config Release --target NotePad--