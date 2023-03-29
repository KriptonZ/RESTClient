#pragma once

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QUrl>

struct UserInfo;
class NetworkManager;

class UserInfoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit UserInfoWidget(NetworkManager *netManager, QWidget *parent = nullptr);
	void setInfo(const UserInfo& info);

signals:

private slots:
	void onImageReceived(const QUrl& url, const QByteArray& array);

private:
	void initLayout();

	NetworkManager* mNetManager = nullptr;

	QLabel* mName = nullptr;
	QLabel* mPosition = nullptr;
	QLabel* mEmail = nullptr;
	QLabel* mPhone = nullptr;

	QLabel *mPhoto = nullptr;
	QUrl mPhotoUrl;

};

