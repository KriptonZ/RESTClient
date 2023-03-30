#include "MainWindow.hpp"
#include "Data/UserInfo.hpp"
#include "Network/NetworkManager.hpp"
#include "UI/UserInfoWidget.hpp"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QTabWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>

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

void MainWindow::onTokenReceived(const QString &token)
{
	mToken = token;
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

void MainWindow::onPositionsReceived(const QList<QString> &positionsList)
{
	auto vLayout = new QVBoxLayout();
	for(const auto& pos : positionsList)
	{
		auto radioButton = new QRadioButton(pos, this);
		vLayout->addWidget(radioButton);
		connect(radioButton, &QRadioButton::toggled, this, [this, radioButton](bool checked)
		{
			if(checked)
			{
				mSelectedPosition = radioButton->text();
			}
		});
	}
	dynamic_cast<QRadioButton*>(vLayout->itemAt(0)->widget())->setChecked(true);
	mPositionList->setLayout(vLayout);
}

void MainWindow::onShowMoreClicked()
{
	mNetManager->getUsers(++mCurrentPage);
}

void MainWindow::onUploadClicked()
{
	auto filePath = QFileDialog::getOpenFileName(this, "Load photo", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
												 "Images (*.png *.jpg);;All files (*)");
	if(filePath.isEmpty())
	{
		return;
	}
	mPhotoPathEditor->setText(filePath);
}

void MainWindow::onAddUserClicked()
{
	auto errors = validate();
	if(!errors.empty())
	{
		QMessageBox::warning(this, tr("Error"), errors.join("\n"));
		resetStyles();
		return;
	}
	mNetManager->getToken();
}

void MainWindow::initLayout()
{
	auto usersTitleText = new QLabel("Users list", this);
	mUsersList = new QListWidget(this);
	mShowMoreButton = new QPushButton("Show more", this);

	auto usersTabWidget = new QWidget(this);
	auto vLayout1 = new QVBoxLayout();
	vLayout1->addWidget(usersTitleText, Qt::AlignCenter);
	vLayout1->addWidget(mUsersList);
	vLayout1->addWidget(mShowMoreButton, Qt::AlignCenter);
	usersTabWidget->setLayout(vLayout1);

	auto addUserTitleText = new QLabel("Add user", this);

	auto nameLabel = new QLabel("Name", this);
	mNameEditor = new QLineEdit(this);
	mNameEditor->setPlaceholderText("John Smith");

	auto emailLabel = new QLabel("Email", this);
	mEmailEditor = new QLineEdit(this);
	mEmailEditor->setPlaceholderText("john.smith@maildomain.com");
	mEmailEditor->setValidator(new QRegExpValidator(QRegExp("^[\\w-\\.]+@([\\w-]+\\.)+[\\w-]{2,4}$"), this));

	auto phoneLabel = new QLabel("Phone", this);
	mPhoneEditor = new QLineEdit(this);
	mPhoneEditor->setPlaceholderText("+38 XXX XXXXXXX");
	mPhoneEditor->setValidator(new QRegExpValidator(QRegExp("^\\+38\\d{10}$"), this));

	auto positionLabel = new QLabel("Position", this);
	mPositionList = new QGroupBox(this);
	connect(mNetManager, &NetworkManager::positionsReceived, this, &MainWindow::onPositionsReceived);
	mNetManager->getPositions();

	auto photoLabel = new QLabel("Photo", this);
	mPhotoPathEditor = new QLineEdit(this);
	mPhotoPathEditor->setPlaceholderText("Upload photo");
	mPhotoPathEditor->setDisabled(true);
	auto uploadButton = new QPushButton("Upload", this);
	auto uploadLayout = new QHBoxLayout();
	uploadLayout->addWidget(mPhotoPathEditor);
	uploadLayout->addWidget(uploadButton);

	mAddUserButton = new QPushButton("Add user", this);

	auto addUserTab = new QWidget(this);
	auto vLayout2 = new QVBoxLayout();
	vLayout2->addWidget(addUserTitleText, Qt::AlignCenter);
	vLayout2->addWidget(nameLabel);
	vLayout2->addWidget(mNameEditor);
	vLayout2->addWidget(emailLabel);
	vLayout2->addWidget(mEmailEditor);
	vLayout2->addWidget(phoneLabel);
	vLayout2->addWidget(mPhoneEditor);
	vLayout2->addWidget(positionLabel);
	vLayout2->addWidget(mPositionList);
	vLayout2->addWidget(photoLabel);
	vLayout2->addLayout(uploadLayout);
	vLayout2->addWidget(mAddUserButton, Qt::AlignCenter);
	addUserTab->setLayout(vLayout2);

	auto tabs = new QTabWidget(this);
	tabs->addTab(usersTabWidget, "Users");
	tabs->addTab(addUserTab, "Add user");

	setCentralWidget(tabs);

	connect(mShowMoreButton, &QPushButton::clicked, this, &MainWindow::onShowMoreClicked);
	connect(mNetManager, &NetworkManager::noMoreContent, this, [this]()
	{
		mShowMoreButton->setVisible(false);
	});
	connect(uploadButton, &QPushButton::clicked, this, &MainWindow::onUploadClicked);
	connect(mAddUserButton, &QPushButton::clicked, this, &MainWindow::onAddUserClicked);
}

QStringList MainWindow::validate()
{
	QStringList errors;

	if(mNameEditor->text().isEmpty())
	{
		mNameEditor->setStyleSheet(InvalidInputStyle);
		errors << "Name can't be empty";
	}
	if(!mEmailEditor->hasAcceptableInput())
	{
		mEmailEditor->setStyleSheet(InvalidInputStyle);
		errors << "Email is wrong";
	}
	if(!mPhoneEditor->hasAcceptableInput())
	{
		mPhoneEditor->setStyleSheet(InvalidInputStyle);
		errors << "Phone is wrong";
	}
	if(mPhotoPathEditor->text().isEmpty())
	{
		mPhotoPathEditor->setStyleSheet(InvalidInputStyle);
		errors << "Choose the photo";
	}

	return errors;
}

void MainWindow::resetStyles()
{
	mNameEditor->setStyleSheet("");
	mEmailEditor->setStyleSheet("");
	mPhoneEditor->setStyleSheet("");
	mPhotoPathEditor->setStyleSheet("");
}

