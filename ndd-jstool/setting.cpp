#include "setting.h"
#include "jsminndd.h"
#include <QDir>
#include <QSettings>
#include <QMessageBox>

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
	//20230808 原本ini配置是放在当前目录plugin下面的，发现在windows下该目录没有写权限。
	//统一修改到用户目录
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
			s_setting->setIniCodec("UTF-8");
		}
		updateOptions();
	}
	else
	{
		if (!staticLoad)
		{
			//不存在配置文件。
			QMessageBox::warning(nullptr, tr("Error"), tr("JS Tool Setting file lost !"));
		}
		return false;
	}

	return true;
}

//在模块卸载时可以调用一下。
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
	/*	QDir currDir = qApp->applicationDirPath();
		QString dirName = currDir.absolutePath();

		QString fileName = QString("%1/plugin/jstoolndd.ini").arg(dirName);*/

		QString settingDir = QString("notepad/jstoolndd");
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
		QString fileName = qs.fileName();

		s_setting = new QSettings(fileName, QSettings::IniFormat);
		s_setting->setIniCodec("UTF-8");
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

	//同步更新一下g_struOptions,因为外面其实是用这个进行参数解析的。
	updateOptions();

	close();
}
