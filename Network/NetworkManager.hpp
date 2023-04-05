#pragma once

#include "Data/UserInfo.hpp"
#include <QObject>
#include <QNetworkAccessManager>

class NetworkManager : public QObject
{
	Q_OBJECT
public:
	explicit NetworkManager(QObject *parent = nullptr);

	void getToken();
	void getUsers(const int page = 1, const int count = 6);
	void postUser(const QString& token, const QVariantMap& data, const QString& imageFilePath);
	void getUser(int id);
	void getPositions();
	void getImage(const QString& url);

signals:
	void tokenReceived(const QString& token);
	void usersReceived(const QList<UserInfo>& users);
	void userReceived();
	void positionsReceived(const QMap<int, QString>& positions);
	void imageReceived(const QUrl& url, const QByteArray& byteArray);
	void noMoreContent();

private slots:
	void onFinishedReading(QNetworkReply *reply);

private:
	QNetworkAccessManager* mNetworkAccessManager = nullptr;
	const QString BaseUrl = "https://frontend-test-assignment-api.abz.agency/api/v1";

};

