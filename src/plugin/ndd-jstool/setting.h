#pragma once

#include <QWidget>
#include <QSettings>
#include "ui_setting.h"

class Setting : public QWidget
{
	Q_OBJECT

public:
	Setting(QWidget *parent = nullptr);
	~Setting();

	static bool readSet(bool staticLoad = true);
	static void destorySet();
	static void updateOptions();

private slots:
	void on_save();

private:
	Ui::SettingClass ui;

	static QSettings* s_setting;
};
