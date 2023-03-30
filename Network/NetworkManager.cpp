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
	QNetworkRequest request(QUrl(QString(BaseUrl + "/token")));
	mNetworkAccessManager->get(request);
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
	QNetworkRequest request(QUrl(QString(BaseUrl + "/positions")));
	mNetworkAccessManager->get(request);
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
			QJsonObject replyJsonInfo = QJsonDocument::fromJson(reply->readAll()).object();
			if(replyJsonInfo.contains("token"))
			{
				emit tokenReceived(replyJsonInfo.value("token").toString());
			}
			else if(replyJsonInfo.contains("positions"))
			{
				QJsonArray positions = replyJsonInfo.value("positions").toArray();
				QList<QString> positionsList;
				for(const auto& position : positions)
				{
					positionsList.append(position.toObject().value("name").toString());
				}

				emit positionsReceived(positionsList);
			}
			else if(replyJsonInfo.contains("users"))
			{
				bool noContentFollowing = replyJsonInfo.value("links").toObject().value("next_url").isNull();
				if(noContentFollowing)
				{
					emit noMoreContent();
				}
				QJsonArray users = replyJsonInfo.value("users").toArray();
				QList<UserInfo> usersList;
				for(const auto& user : users)
				{
					usersList.append(UserInfo(user.toObject()));
				}

				emit usersReceived(usersList);
			}
		}
		else if(header == "image/jpeg")
		{
			emit imageReceived(reply->url(), reply->readAll());
		}
	}

	reply->deleteLater();
}
