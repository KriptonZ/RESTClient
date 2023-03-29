#include "MainWindow.hpp"
#include "Data/UserInfo.hpp"
#include "Network/NetworkManager.hpp"
#include "UI/UserInfoWidget.hpp"
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	initLayout();

	mNetManager = new NetworkManager(this);

	connect(mNetManager, &NetworkManager::usersReceived, this, &MainWindow::onUsersReceived);

	mNetManager->getUsers();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onUsersReceived(const QList<UserInfo> &usersList)
{
	for(const auto& user : usersList)
	{
		auto userInfoWidget = new UserInfoWidget(mNetManager, this);
		userInfoWidget->setInfo(user);

		auto item = new QListWidgetItem(mUsersList);
		mUsersList->addItem(item);
		mUsersList->setItemWidget(item, userInfoWidget);
		item->setSizeHint(userInfoWidget->minimumSizeHint());
	}
}

void MainWindow::initLayout()
{
	auto titleText = new QLabel("Users list", this);

	mUsersList = new QListWidget(this);

	auto centralWidget = new QWidget(this);
	auto vLayout = new QVBoxLayout();
	vLayout->addWidget(titleText, Qt::AlignCenter);
	vLayout->addWidget(mUsersList);

	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);
}

