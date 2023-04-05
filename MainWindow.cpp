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
	connect(mNetManager, &NetworkManager::tokenReceived, this, &MainWindow::onTokenReceived);
	mNetManager->getUsers(++mCurrentPage);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onTokenReceived(const QString &token)
{
	mToken = token;
	QVariantMap data;
	data.insert("name", mNameEditor->text());
	data.insert("email", mEmailEditor->text());
	data.insert("phone", mPhoneEditor->text());
	data.insert("position_id", mSelectedPosition->property("id").toInt());

	mNetManager->postUser(token, data, mPhotoPathEditor->text());
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

void MainWindow::onPositionsReceived(const QMap<int, QString> &positionsList)
{
	auto vLayout = new QVBoxLayout();
	for(const auto& pos : positionsList.keys())
	{
		auto radioButton = new QRadioButton(positionsList[pos], this);
		radioButton->setProperty("id", pos);
		vLayout->addWidget(radioButton);
		connect(radioButton, &QRadioButton::toggled, this, [this, radioButton](bool checked)
		{
			if(checked)
			{
				mSelectedPosition = radioButton;
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
												 "Images (*.jpeg *.jpg);;All files (*)");
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
	mEmailEditor->setValidator(new QRegExpValidator(QRegExp("^(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)"
															"*|\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")"
															"@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)"
															"+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
															"(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\\x01-\\x08\x0b\\x0c\\x0e-\\x1f\\x21-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])$"), this));

	auto phoneLabel = new QLabel("Phone", this);
	mPhoneEditor = new QLineEdit(this);
	mPhoneEditor->setPlaceholderText("+38 XXX XXXXXXX");
	mPhoneEditor->setValidator(new QRegExpValidator(QRegExp("^[\\+]{0,1}380([0-9]{9})$"), this));

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

	if(mNameEditor->text().length() < 2 || mNameEditor->text().length() > 60)
	{
		mNameEditor->setStyleSheet(InvalidInputStyle);
		errors << "Username should contain 2-60 characters";
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
	else
	{
		QFileInfo fi(mPhotoPathEditor->text());
		if(fi.size() > 5 * 1024 * 1024)
		{
			mPhotoPathEditor->setStyleSheet(InvalidInputStyle);
			errors << "The photo size must not be greater than 5 Mb";
		}
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

