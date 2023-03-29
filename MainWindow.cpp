#include "MainWindow.hpp"
#include "Data/UserInfo.hpp"
#include "Network/NetworkManager.hpp"
#include "UI/UserInfoWidget.hpp"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	mNetManager = new NetworkManager(this);

	initLayout();	

	connect(mNetManager, &NetworkManager::usersReceived, this, &MainWindow::onUsersReceived);

	mNetManager->getUsers(++mCurrentPage);
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

void MainWindow::onShowMoreClicked()
{
	mNetManager->getUsers(++mCurrentPage);
}

void MainWindow::initLayout()
{
	auto titleText = new QLabel("Users list", this);
	mUsersList = new QListWidget(this);
	mShowMoreButton = new QPushButton("Show more", this);

	auto centralWidget = new QWidget(this);
	auto vLayout = new QVBoxLayout();
	vLayout->addWidget(titleText, Qt::AlignCenter);
	vLayout->addWidget(mUsersList);
	vLayout->addWidget(mShowMoreButton, Qt::AlignCenter);

	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	connect(mShowMoreButton, &QPushButton::clicked, this, &MainWindow::onShowMoreClicked);
	connect(mNetManager, &NetworkManager::noMoreContent, this, [this]()
	{
		mShowMoreButton->setVisible(false);
	});
}

