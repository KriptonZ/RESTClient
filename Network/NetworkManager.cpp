#include "NetworkManager.hpp"
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

NetworkManager::NetworkManager(QObject *parent)
	: QObject(parent)
{
	mNetworkAccessManager = new QNetworkAccessManager(parent);
	connect(mNetworkAccessManager, &QNetworkAccessManager::finished,this, &NetworkManager::onFinishedReading);
}

void NetworkManager::getToken()
{

}

void NetworkManager::getUsers(const int page, const int count)
{
	QNetworkRequest request(QUrl(QString(BaseUrl + "/users?page=%1&count=%2").arg(page).arg(count)));
	mNetworkAccessManager->get(request);
}

void NetworkManager::postUsers()
{

}

void NetworkManager::getUser(int id)
{

}

void NetworkManager::getPositions()
{

}

void NetworkManager::getImage(const QString &url)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	mNetworkAccessManager->get(request);
}

void NetworkManager::onFinishedReading(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qDebug() << "Error: " << reply->errorString();
	}
	else
	{
		auto header = reply->header(QNetworkRequest::ContentTypeHeader).toString();
		if(header == "application/json")
		{
			QList<UserInfo> usersList;
			QJsonObject userJsonInfo = QJsonDocument::fromJson(reply->readAll()).object();
			QJsonArray users = userJsonInfo.value("users").toArray();
			for(const auto& user : users)
			{
				usersList.append(UserInfo(user.toObject()));
			}

			emit usersReceived(usersList);
		}
		else if(header == "image/jpeg")
		{
			emit imageReceived(reply->url(), reply->readAll());
		}
	}

	reply->deleteLater();
}
