#include <QtCore/qglobal.h>
#include "setting.h"
#include "jsminndd.h"
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include <QtGlobal>

QSettings* Setting::s_setting = nullptr;
extern StruOptions g_struOptions;

Setting::Setting(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    if (!readSet())
    {
        return;
    }

    int lineEnd = s_setting->value("endline").toInt();
    if (lineEnd == 0)
    {
        ui.endAutoDetect->setChecked(true);
    }
    else if (lineEnd == 1)
    {
        ui.endWindow->setChecked(true);
    }
    else if (lineEnd == 2)
    {
        ui.endUnix->setChecked(true);
    }

    int keepcom = s_setting->value("keepcom").toInt();
    if (keepcom == 1)
    {
        ui.keepComment->setChecked(true);
    }

    int useIndent = s_setting->value("useindent").toInt();
    ui.useIndent->setChecked((useIndent == 1));

    int tablen = s_setting->value("tablen").toInt();
    ui.tabSpaceLen->setValue(tablen);

    int indentempline = s_setting->value("indentempline").toInt();
    ui.keepIndentEmptyLine->setChecked((indentempline == 1));

    int newlinebefore = s_setting->value("newlinebefore").toInt();
    ui.newlIineBefore->setChecked((newlinebefore == 1));
}

Setting::~Setting()
{
}

void Setting::updateOptions()
{
    g_struOptions.nPutCR = s_setting->value("endline").toInt();

    if (1 == s_setting->value("useindent").toInt())
    {
        g_struOptions.chIndent = ' ';
        g_struOptions.nChPerInd = s_setting->value("tablen").toInt();
    }
    else
    {
        g_struOptions.chIndent = '\t';
        g_struOptions.nChPerInd = 1;
    }

    g_struOptions.bNLBracket = s_setting->value("newlinebefore").toBool();
    g_struOptions.bKeepTopComt = s_setting->value("keepcom").toBool();
    g_struOptions.bIndentInEmpty = s_setting->value("indentempline").toBool();
}

bool Setting::readSet(bool staticLoad)
{
    // 20230808 §ª§Ù§ß§Ñ§é§Ñ§Ý§î§ß§à ini-§Ü§à§ß§æ§Ú§Ô §Ý§Ö§Ø§Ñ§Ý §Ó §Ü§Ñ§ä§Ñ§Ý§à§Ô§Ö plugin §â§ñ§Õ§à§Þ §ã §á§â§à§Ô§â§Ñ§Þ§Þ§à§Û,
    // §ß§à §Ó Windows §ä§Ñ§Þ §ß§Ö§ä §á§â§Ñ§Ó §ß§Ñ §Ù§Ñ§á§Ú§ã§î. §±§à§ï§ä§à§Þ§å §ç§â§Ñ§ß§Ö§ß§Ú§Ö §å§ß§Ú§æ§Ú§è§Ú§â§à§Ó§Ñ§ß§à ¡ª
    // §á§Ö§â§Ö§ß§Ö§ã§Ö§ß§à §Ó §á§à§Ý§î§Ù§à§Ó§Ñ§ä§Ö§Ý§î§ã§Ü§Ú§Û §Ü§Ñ§ä§Ñ§Ý§à§Ô.
#if 0
    QDir currDir = qApp->applicationDirPath();
    QString dirName = currDir.absolutePath();
    QString fileName = QString("%1/plugin/jstoolndd.ini").arg(dirName);
#endif

    QString settingDir = QString("notepad/jstoolndd");
    QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
    QString fileName = qs.fileName();
    QFileInfo fi(fileName);

    if (fi.exists())
    {
        if (s_setting == nullptr)
        {
            s_setting = new QSettings(fileName, QSettings::IniFormat);
            // §£ Qt6 QSettings::setIniCodec() §å§Õ§Ñ§Ý§×§ß ¡ª QSettings §Ó§ã§Ö§Ô§Õ§Ñ §â§Ñ§Ò§à§ä§Ñ§Ö§ä §Ó UTF-8.
            // §£ Qt5 §Ó§í§Ù§í§Ó§Ñ§Ö§Þ setIniCodec("UTF-8") §ñ§Ó§ß§à §Õ§Ý§ñ §Ô§Ñ§â§Ñ§ß§ä§Ú§Ú §Ü§à§Õ§Ú§â§à§Ó§Ü§Ú.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            s_setting->setIniCodec("UTF-8");
#endif
        }
        updateOptions();
    }
    else
    {
        if (!staticLoad)
        {
            // §¬§à§ß§æ§Ú§Ô§å§â§Ñ§è§Ú§à§ß§ß§í§Û §æ§Ñ§Û§Ý §ß§Ö §ß§Ñ§Û§Õ§Ö§ß.
            QMessageBox::warning(nullptr, tr("Error"), tr("JS Tool Setting file lost !"));
        }
        return false;
    }
    return true;
}

// §®§à§Ø§ß§à §Ó§í§Ù§í§Ó§Ñ§ä§î §á§â§Ú §Ó§í§Ô§â§å§Ù§Ü§Ö §Þ§à§Õ§å§Ý§ñ.
void Setting::destorySet()
{
    if (s_setting != nullptr)
    {
        s_setting->deleteLater();
        s_setting = nullptr;
    }
}

void Setting::on_save()
{
    if (s_setting == nullptr)
    {
        /*  QDir currDir = qApp->applicationDirPath();
            QString dirName = currDir.absolutePath();
            QString fileName = QString("%1/plugin/jstoolndd.ini").arg(dirName);*/
        QString settingDir = QString("notepad/jstoolndd");
        QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
        QString fileName = qs.fileName();
        s_setting = new QSettings(fileName, QSettings::IniFormat);
        // §£ Qt6 QSettings::setIniCodec() §å§Õ§Ñ§Ý§×§ß ¡ª QSettings §Ó§ã§Ö§Ô§Õ§Ñ §â§Ñ§Ò§à§ä§Ñ§Ö§ä §Ó UTF-8.
        // §£ Qt5 §Ó§í§Ù§í§Ó§Ñ§Ö§Þ setIniCodec("UTF-8") §ñ§Ó§ß§à §Õ§Ý§ñ §Ô§Ñ§â§Ñ§ß§ä§Ú§Ú §Ü§à§Õ§Ú§â§à§Ó§Ü§Ú.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        s_setting->setIniCodec("UTF-8");
#endif
    }

    int v = 0;
    if (ui.endAutoDetect->isChecked())
    {
        v = 0;
    }
    else if (ui.endWindow->isChecked())
    {
        v = 1;
    }
    else if (ui.endUnix->isChecked())
    {
        v = 2;
    }

    s_setting->setValue("endline", v);
    s_setting->setValue("keepcom", (ui.keepComment->isChecked() ? 1 : 0));
    s_setting->setValue("useindent", (ui.useIndent->isChecked() ? 1 : 0));
    s_setting->setValue("tablen", ui.tabSpaceLen->value());
    s_setting->setValue("indentempline", (ui.keepIndentEmptyLine->isChecked() ? 1 : 0));
    s_setting->setValue("newlinebefore", (ui.newlIineBefore->isChecked() ? 1 : 0));
    s_setting->sync();

    // §³§Ú§ß§ç§â§à§ß§ß§à §à§Ò§ß§à§Ó§Ý§ñ§Ö§Þ g_struOptions, §ä.§Ü. §Ú§Þ§Ö§ß§ß§à §à§ß §Ú§ã§á§à§Ý§î§Ù§å§Ö§ä§ã§ñ
    // §Ó§ß§Ö§ê§ß§Ú§Þ §Ü§à§Õ§à§Þ §Õ§Ý§ñ §â§Ñ§Ù§Ò§à§â§Ñ §á§Ñ§â§Ñ§Þ§Ö§ä§â§à§Ó.
    updateOptions();
    close();
}