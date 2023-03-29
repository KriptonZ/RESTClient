#include "UserInfoWidget.hpp"
#include "Data/UserInfo.hpp"
#include "Network/NetworkManager.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

UserInfoWidget::UserInfoWidget(NetworkManager *netManager, QWidget *parent)
	: QWidget(parent), mNetManager(netManager)
{
	initLayout();
}

void UserInfoWidget::setInfo(const UserInfo &info)
{
	mName->setText(info.name);
	mPosition->setText(info.position);
	mEmail->setText(info.email);
	mPhone->setText(info.phone);
	mPhotoUrl = QUrl(info.photoUrl);

	connect(mNetManager, &NetworkManager::imageReceived, this, &UserInfoWidget::onImageReceived);
	mNetManager->getImage(info.photoUrl);
}

void UserInfoWidget::onImageReceived(const QUrl &url, const QByteArray &array)
{
	if(mPhotoUrl != url)
	{
		return;
	}

	disconnect(mNetManager, &NetworkManager::imageReceived, this, &UserInfoWidget::onImageReceived);

	QPixmap img;
	img.loadFromData(array);
	mPhoto->setPixmap(img);
}

void UserInfoWidget::initLayout()
{
	mName = new QLabel(this);
	mPosition = new QLabel(this);
	mEmail = new QLabel(this);
	mPhone = new QLabel(this);

	auto vLayout = new QVBoxLayout();
	vLayout->addWidget(mName);
	vLayout->addWidget(mPosition);
	vLayout->addWidget(mEmail);
	vLayout->addWidget(mPhone);

	mPhoto = new QLabel(this);

	auto hLayout = new QHBoxLayout();
	hLayout->addWidget(mPhoto);
	hLayout->addLayout(vLayout, Qt::AlignLeft);

	setLayout(hLayout);

}
