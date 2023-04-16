#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_collectiondialog.h"
#include "QtCore/qmimedata.h"
#include "QtCore/qmimedatabase.h"
#include <QShortcut>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QStandardPaths>
#include <QClipboard>
#include <QDropEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    currentCollection = nullptr;
    currentFile = nullptr;
    currentLink = nullptr;

    sxcu_interface = new SXCUInterface();
    pman = new PersistenceManager();
    pman->setup(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    pman->setParent(this);

    ui->setupUi(this);
    ui->statusbar->showMessage("Welcome :D. Hover over confusing elements for help.");

    ui->copyToClipboardButton->setEnabled(false);
    ui->uploadButton->setEnabled(false);

    connect(ui->collectionBox, &QComboBox::currentTextChanged,this, &MainWindow::collectionSelectionChanged);
    connect(ui->copyToClipboardButton, &QPushButton::clicked, this, &MainWindow::copyToCBoardClicked);
    connect(ui->importButton, &QPushButton::clicked, this, &MainWindow::importButtonClicked);
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::uploadButtonClicked);

    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::entryDoubleClicked);

    this->setAcceptDrops(true);

    ui->tableView->setModel(pman);
    ui->tableView->show();
    ui->tableView->verticalHeader()->setVisible(false);

    QShortcut *delShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Backspace), this);
    //connect(delShortcut, &QShortcut::activatedAmbiguously, this, &MainWindow::removeEntry);
    connect(delShortcut, &QShortcut::activated, this, &MainWindow::removeEntry);

    ui->collectionBox->setInsertPolicy(QComboBox::InsertAtTop);
    ui->collectionBox->addItems(pman->collectionTitles());

    ui->imageView->setToolTip("Click the open button or simply drop a file here and a preview will be displayed.");
    ui->collectionBox->setToolTip("Choose a collection to upload to. WIP: Collections are not really considered yet; if you want to add one anyway, choose 'remove entry' and then 'add entry' to open the dialog for creation/import.");
    ui->sxcuSelfDestructBox->setToolTip("Choose whether the image should become unavailable after 24h.");
    ui->sxcuEmbeddableBox->setToolTip("Choose whether a direct link to file should be returned; a link to sxcu's interface is returned otherwise.");
    ui->copyToClipboardButton->setToolTip("Copy a link to a file after it has been uploaded.");
    ui->importButton->setToolTip("Open a file by navigating to it and opening it.");
    ui->uploadButton->setToolTip("Upload a file.");
    ui->importEdit->setToolTip("The URL of the file that shall be imported. For now, only SXCU URLs are valid, sorry :(.");
    ui->tableView->setToolTip("Double click an entry to copy it, press cmd or ctrl + backspace to delete it.");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete sxcu_interface;
    delete currentFile;
    delete currentLink;
    delete pman;
}

void MainWindow::showCollectionAdder() {
    QDialog *adderDialog = new QDialog();
    Ui::CollectionAdder *adder = new Ui::CollectionAdder();
    adder->setupUi(adderDialog);
    adderDialog->exec();

    if(adder->newRadioButton->isChecked()) {
        if(adder->newNameEdit->text().isEmpty()) {
            this->ui->statusbar->showMessage("Missing name for new collection.", 10000);
        } else {
            qDebug() << "Creating collection with title " + adder->newNameEdit->text();
            sxcu_interface->createCollection(adder->newNameEdit->text(), adder->newDescriptionEdit->text(), adder->newPrivateCheckbox, adder->newUnlistedCheckbox, [=](SXCUInterface::SXCUCollection *collection, SXCUInterface::SXCUError *error){
                if(collection) {
                    this->pman->addCollection(collection);
                    this->currentCollection = collection;
                }
                this->ui->statusbar->showMessage(error->code ? error->error : "Successfully added collection.", 10000);
            });
        }
    }

    delete adder;
}
void MainWindow::showCollectionRemover() {
    if(currentCollection) {
        this->ui->statusbar->showMessage("Deleting selected collection " + currentCollection->title + " locally.", 10000);
        this->pman->deleteCollection(currentCollection);
        this->ui->collectionBox->removeItem(this->ui->collectionBox->findText(currentCollection->title));
    }
}

void MainWindow::collectionSelectionChanged(const QString &content) {
    if(content == "+ add entry") {
        this->showCollectionAdder();
    } else if(content == "- remove entry") {
        if(currentCollection) this->showCollectionRemover();
    } else {
        this->currentCollection = pman->collectionForTitle(content);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}
void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasImage()) qDebug() << "Received drop event with image data";
    /* WIP: Implement multi file import, maybe even with PVSF */
    if(mimeData->hasUrls()) {
        qDebug() << "Drop event triggered using file(s)";
        QList<QUrl> urls = mimeData->urls();
        if(urls.isEmpty()) qWarning() << "Event without any files.";
        urls.count() == 1
            ? qDebug() << "Received file via drop"
            : qWarning() << "Received" << urls.count() << "but can only import one; using first...";
        QString filePath = urls.first().toLocalFile();
        const QStringList allowedTypes = {"image/png", "image/jpeg", "image/gif", "image/tiff"};
        if(allowedTypes.contains(QMimeDatabase().mimeTypeForFile(filePath).name())) {
            event->acceptProposedAction();
            import(filePath);
        } else {
            qWarning() << "Received yet unsupported non-image file. Aborting...";
        }
    }
    this->ui->imageView->setAcceptDrops(true);
}

void MainWindow::importButtonClicked() {
    qDebug() << "Import initiated";
    QString input = QFileDialog::getOpenFileName(this, "Choose an image to upload", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.gif)");
    if(input == nullptr) {
        qDebug() << "File import canceled";
    } else {
        qDebug() << "Received file " << input;
        import(input);
    }
}

void MainWindow::import(QString& filePath) {
    QFile *inputFile = new QFile(filePath);
    if(!inputFile->exists()) {
        qDebug() << "Input file does somehow not exist! Cancelling...";
        return;
    }
    this->currentFile = new QString(filePath);

    QImage *inputImage = new QImage(filePath);
    QGraphicsPixmapItem *inputPixmap = new QGraphicsPixmapItem(QPixmap::fromImage(*inputImage));
    QGraphicsScene *iconScene = new QGraphicsScene();
    iconScene->addItem(inputPixmap);
    ui->imageView->setScene(iconScene);
    ui->imageView->fitInView(inputPixmap, Qt::KeepAspectRatio);
    ui->imageView->show();

    ui->uploadButton->setEnabled(true);

    delete inputFile;
    delete inputImage;
}

void MainWindow::uploadButtonClicked() {
    qDebug() << "Uploading to SXCU initiated";
    if(this->currentFile->isNull()) {
        qDebug() << "Missing input file, cancelling...";
        return;
    }
    qDebug() << "Input file" << *this->currentFile << "valid";
    this->ui->statusbar->showMessage("Uploading...");
    this->sxcu_interface->createFile(*currentFile, "", "", ui->sxcuEmbeddableBox->checkState(), ui->sxcuSelfDestructBox->checkState(), [&](SXCUInterface::SXCUFile *file, SXCUInterface::SXCUError *error) {
        if(file) {
            this->pman->addEntry(file);
            this->currentLink = new QString(file->url.toString());
            this->ui->copyToClipboardButton->setEnabled(true);
            this->ui->tableView->update();
        }
        this->ui->statusbar->showMessage(error ? error->error : "Uploading succeeded.", 10000);

    });

}
void MainWindow::copyToCBoardClicked() {
    qDebug() << "Copy to clipboard initiated";
    if(this->currentLink) {
        QGuiApplication::clipboard()->setText(*this->currentLink);
        qDebug() << "Copied" << *this->currentLink << "to clipboard";
        ui->statusbar->showMessage("Copied " + *this->currentLink + " to clipboard.");
    }
}

void MainWindow::importEditChanged(const QString &content) {
    qDebug() << "Importing of URL initialized.";
    if(!content.isEmpty()) {
        QUrl input = QUrl(content);
        if(input.isValid()) {
            if(input.host() == "sxcu.net") {
                ui->statusbar->showMessage("URL looks valid and has 'sxcu.net' as host.");
                QString ID = input.path().section("/", -1);
                sxcu_interface->fetchFile(ID, [=](SXCUInterface::SXCUFile *file, SXCUInterface::SXCUError *error){
                    if(error) ui->statusbar->showMessage(error->error, 10000);
                    if(file) {
                        pman->addEntry(file);
                        ui->statusbar->showMessage("Import successfull!", 10000);
                    }
                });
            } else {
                ui->statusbar->showMessage("Text is a valid URL, but its host is not 'sxcu.net'.");
            }
        } else {
            ui->statusbar->showMessage("Entered text is no valid URL.");
        }
    } else {
        ui->statusbar->showMessage("Don't just press enter xD.");
    }
}

void MainWindow::entryDoubleClicked(const QModelIndex &index) {
    qDebug() << "Copy from selected table entry to clipboard initiated.";
    QString data = ui->tableView->model()->data(index).toString();
    QGuiApplication::clipboard()->setText(data);
    qDebug() << "Copied" << data << "to clipboard";
    ui->statusbar->showMessage("Copied " + data + " to clipboard.");
}

void MainWindow::removeEntry() {
    qDebug() << "Entry removal triggered.";
    if(ui->tableView->currentIndex().column() == 1) {
        int index = ui->tableView->currentIndex().row();
        SXCUInterface::SXCUFile *entryToDelete = pman->entryForTableindex(index);
        if(entryToDelete) {
            qDebug() << "Deleting file" << entryToDelete->id << "at index" << index;
            this->ui->statusbar->showMessage("Deleting file " + entryToDelete->id);
            sxcu_interface->deleteFile(entryToDelete, [this](SXCUInterface::SXCUError *error) {
                this->ui->statusbar->showMessage(error->code ? error->error : "File successfully deleted", 10000);
            });
        }
    }
}
