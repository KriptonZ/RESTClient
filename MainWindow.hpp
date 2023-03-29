#pragma once

#include <QMainWindow>

class QListWidget;
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

private:
	void initLayout();

	QListWidget* mUsersList = nullptr;

	NetworkManager* mNetManager = nullptr;
};
