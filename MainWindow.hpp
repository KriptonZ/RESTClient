#pragma once

#include <QMainWindow>

class NetworkManager;
struct UserInfo;
class QListWidget;
class QPushButton;
class QLineEdit;
class QGroupBox;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void onTokenReceived(const QString& token);
	void onUsersReceived(const QList<UserInfo>& usersList);
	void onPositionsReceived(const QList<QString>& positionsList);

	void onShowMoreClicked();
	void onUploadClicked();
	void onAddUserClicked();

private:
	void initLayout();
	QStringList validate();
	void resetStyles();

	NetworkManager* mNetManager = nullptr;

	int mCurrentPage = 0;
	QString mToken;
	QListWidget* mUsersList = nullptr;
	QPushButton* mShowMoreButton = nullptr;

	QLineEdit* mNameEditor = nullptr;
	QLineEdit* mEmailEditor = nullptr;
	QLineEdit* mPhoneEditor = nullptr;
	QGroupBox* mPositionList = nullptr;
	QString mSelectedPosition;
	QLineEdit* mPhotoPathEditor = nullptr;
	QPushButton* mAddUserButton = nullptr;

	const QString InvalidInputStyle = "background-color: rgb(246,152,157)";


};
