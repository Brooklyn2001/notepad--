#include "setting.h"
#include "instanceobj.h"
#include <QDir>
#include <QSettings>
#include <QMessageBox>

QSettings* Setting::s_setting = nullptr;

extern XmltoolsOptions xmltoosOptions;

Setting::Setting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    #ifdef Q_OS_WIN
#else
ui.stringEngine->setEnabled(false);
#endif
	if (!readSet())
	{
		return;
	}

	xmltoosOptions.formatingEngine = s_setting->value("xmlengine").toInt();
	xmltoosOptions.maxIndentLevel = s_setting->value("maxInentlevel").toInt();
	xmltoosOptions.ensureConformity = s_setting->value("ensureconformity").toBool();
	xmltoosOptions.ppAutoclose = s_setting->value("ppautoclose").toBool();
	xmltoosOptions.applySpacePreserve = s_setting->value("applyspacepres").toBool();

	int engine = s_setting->value("xmlengine").toInt();
	if (engine == 0)
	{
		ui.quickEngine->setChecked(true);
	}
	else if (engine == 1)
	{
		ui.stringEngine->setChecked(true);
	}
	else if (engine == 2)
	{
		ui.simpleEngine->setChecked(true);
	}

	int value = s_setting->value("maxInentlevel").toInt();
	ui.maxIndentLevel->setValue(value);


	int ischeck = s_setting->value("ensureconformity").toInt();
	ui.cbConformity->setChecked((ischeck == 1));

	ischeck = s_setting->value("ppautoclose").toInt();
	ui.cbPPAutoClose->setChecked((ischeck == 1));

	ischeck = s_setting->value("applyspacepres").toInt();
	ui.cbApplySpacePreserve->setChecked((ischeck == 1));
}

Setting::~Setting()
{
}


void Setting::updateOptions()
{
	xmltoosOptions.formatingEngine = s_setting->value("xmlengine").toInt();
	xmltoosOptions.maxIndentLevel = s_setting->value("maxInentlevel").toInt();
	xmltoosOptions.ensureConformity = s_setting->value("ensureconformity").toBool();
	xmltoosOptions.ppAutoclose = s_setting->value("ppautoclose").toBool();
	xmltoosOptions.applySpacePreserve = s_setting->value("applyspacepres").toBool();
}

bool Setting::readSet(bool staticLoad)
{
#if 0
	QDir currDir = qApp->applicationDirPath();
	QString dirName = currDir.absolutePath();

	QString fileName = QString("%1/plugin/xmltoolndd.ini").arg(dirName);
#endif

	QString settingDir = QString("notepad/xmltoolndd");
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
#if 0
		QDir currDir = qApp->applicationDirPath();
		QString dirName = currDir.absolutePath();

		QString fileName = QString("%1/plugin/xmltoolndd.ini").arg(dirName);
#endif

		QString settingDir = QString("notepad/xmltoolndd");
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, settingDir);
		QString fileName = qs.fileName();

		s_setting = new QSettings(fileName, QSettings::IniFormat);
		s_setting->setIniCodec("UTF-8");
	}


	int v = 0;

	if (ui.quickEngine->isChecked())
	{
		v = 0;
	}
	else if (ui.stringEngine->isChecked())
	{
		v = 1;
	}
	else if (ui.simpleEngine->isChecked())
	{
		v = 2;
	}
	s_setting->setValue("xmlengine", v);
	s_setting->setValue("maxInentlevel", ui.maxIndentLevel->value());
	s_setting->setValue("ensureconformity", (ui.cbConformity->isChecked() ? 1 : 0));
	s_setting->setValue("ppautoclose", (ui.cbPPAutoClose->isChecked() ? 1 : 0));
	s_setting->setValue("applyspacepres", (ui.cbApplySpacePreserve->isChecked() ? 1 : 0));

	s_setting->sync();

	//同步更新一下g_struOptions,因为外面其实是用这个进行参数解析的。
	updateOptions();

	close();
}
