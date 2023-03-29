#include "UserInfo.hpp"
#include <QString>
#include <QMap>

UserInfo::UserInfo()
{
	id = -1;
	name = "";
	email = "";
	phone = "";
	position = "";
	positionId = -1;
	registrationTimestamp = 0;
	photoUrl = "";
}

UserInfo::UserInfo(const QJsonObject &json)
{
	id = json.value("id").toInt();
	name = json.value("name").toString();
	email = json.value("email").toString();
	phone = json.value("phone").toString();
	position = json.value("position").toString();
	positionId = json.value("position_id").toInt();
	registrationTimestamp = json.value("registration_timestamp").toInt();
	photoUrl = json.value("photo").toString();;
}
