#pragma once

#include <QString>
#include <QJsonObject>

enum class Position
{
	Unknown = -1,
	FrontendDev,
	BackendDev,
	Designer,
	QA
};

struct UserInfo
{
	int id;
	QString name;
	QString email;
	QString phone;
	QString position;
	int positionId;
	qint64 registrationTimestamp;
	QString photoUrl;

	UserInfo();
	UserInfo(const QJsonObject& json);
};
