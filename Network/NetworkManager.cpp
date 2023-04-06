#include "NetworkManager.hpp"
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHttpPart>
#include <QFile>
#include <QFileInfo>

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
	QNetworkRequest request(QUrl(QString(BaseUrl + UsersSection + "?page=%1&count=%2").arg(page).arg(count)));
	mNetworkAccessManager->get(request);
}

void NetworkManager::postUser(const QString &token, const QVariantMap &data, const QString &imageFilePath)
{
	QNetworkRequest request(QUrl(QString(BaseUrl + UsersSection)));
	request.setRawHeader(QString("Token").toUtf8(), token.toUtf8());

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	for(const auto& key : data.keys())
	{
		QHttpPart textPart;
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));
		textPart.setBody(data[key].toByteArray());
		multiPart->append(textPart);
	}

	QHttpPart imagePart;
	QFile *file = new QFile(imageFilePath);
	QFileInfo fi(imageFilePath);
	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"photo\"; filename=\"%1\"").arg(fi.fileName())));

	file->open(QIODevice::ReadOnly);
	imagePart.setBodyDevice(file);
	file->setParent(multiPart);

	multiPart->append(imagePart);

	auto reply = mNetworkAccessManager->post(request, multiPart);
	multiPart->setParent(reply);
}

void NetworkManager::getUser(int id)
{
	QNetworkRequest request(QUrl(QString(BaseUrl + UsersSection + "/%1").arg(id)));
	mNetworkAccessManager->get(request);
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
		QString errorDetails;
		QJsonObject replyJsonInfo = QJsonDocument::fromJson(reply->readAll()).object();
		if(replyJsonInfo.contains("message"))
		{
			errorDetails += replyJsonInfo.value("message").toString();
		}
		if(replyJsonInfo.contains("fails"))
		{
			errorDetails += ":\n";
			QJsonObject fails = replyJsonInfo.value("fails").toObject();
			for(const auto& key : fails.keys())
			{
				errorDetails += "- " + key + " (";
				QJsonArray failDetails = fails.value(key).toArray();
				for(const auto& failDetail : failDetails)
				{
					errorDetails += failDetail.toString() + ", ";
				}
				errorDetails.chop(2);
				errorDetails += ")\n";
			}
		}

		emit requestError(errorDetails);
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
			else if(replyJsonInfo.contains("user_id"))
			{
				emit userAdded(replyJsonInfo.value("user_id").toInt());
			}
			else if(replyJsonInfo.contains("user"))
			{
				emit userReceived(UserInfo(replyJsonInfo.value("user").toObject()));
			}
			else if(replyJsonInfo.contains("positions"))
			{
				QJsonArray positions = replyJsonInfo.value("positions").toArray();
				QMap<int, QString> positionsList;
				for(const auto& position : positions)
				{
					auto id = position.toObject().value("id").toInt();
					auto name = position.toObject().value("name").toString();
					positionsList.insert(id, name);
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
