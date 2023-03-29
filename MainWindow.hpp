#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>

class NetworkManager;
struct UserInfo;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void onUsersReceived(const QList<UserInfo>& usersList);

	void onShowMoreClicked();

private:
	void initLayout();

	int mCurrentPage = 0;
	QListWidget* mUsersList = nullptr;
	QPushButton* mShowMoreButton = nullptr;

	NetworkManager* mNetManager = nullptr;
};
