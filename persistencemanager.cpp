#include "persistencemanager.h"
#include <QJsonArray>
#include <QDir>

PersistenceManager::PersistenceManager() {
    this->entries = new std::vector<SXCUInterface::JSONParsable*>();
    this->subdomains = new std::vector<SXCUInterface::SXCUSubdomain*>();
    this->collections = new std::vector<SXCUInterface::SXCUCollection*>();

    this->setHeaderData(0, Qt::Horizontal, "title");
    this->setHeaderData(1, Qt::Horizontal, "link");
}

PersistenceManager::~PersistenceManager() {
    this->save();
    for(SXCUInterface::JSONParsable *entry : *entries) delete entry;
    for(SXCUInterface::JSONParsable *entry : *collections) delete entry;
    for(SXCUInterface::SXCUSubdomain *entry : *subdomains) delete entry;
    delete entries;
    delete collections;
    delete subdomains;
    delete path;
}

void PersistenceManager::addEntry(SXCUInterface::JSONParsable *entry) {
    if(entry) {
        this->entries->push_back(entry);
        this->save();
        qDebug() << "Stored and saved new entry.";
    } else {
        qWarning() << "Receivec entry was in fact a nullptr.";
    }
}

void PersistenceManager::deleteEntry(SXCUInterface::JSONParsable *entry) {
    for(int i = 0; i < this->entries->size(); i++) {
        if(this->entries->at(i) == entry) {
            this->entries->erase(entries->begin() + i);
        }
    }
    delete entry;
    this->save();
}
void PersistenceManager::deleteEntry(int index) {
    if(index < this->entries->size() - 1) entries->erase(entries->begin() + index);
}
SXCUInterface::SXCUFile *PersistenceManager::entryForTableindex(int index) {
    return index < entries->size() ? (SXCUInterface::SXCUFile*)entries->at(index) : nullptr;
}
SXCUInterface::SXCUCollection *PersistenceManager::collectionForIndex(int index) {
    return index < collections->size() ? collections->at(index) : nullptr;
}
SXCUInterface::SXCUCollection *PersistenceManager::collectionForTitle(const QString &title) {
    SXCUInterface::SXCUCollection *collection = nullptr;
    for(SXCUInterface::SXCUCollection *entry : *collections) if(entry->title == title) collection = entry;
    return collection;
}
void PersistenceManager::addCollection(SXCUInterface::SXCUCollection *collection) {
    if(collection) {
        this->deleteCollection(collection);
        this->collections->push_back(collection);
        this->save();
        qDebug() << "Saved new collection";
    } else { qWarning() << "New collection was actually a nullptr."; }
}
void PersistenceManager::deleteCollection(SXCUInterface::SXCUCollection *collection) {
    for(int i = 0; i < this->collections->size(); i++) {
        if(this->collections->at(i)->id == collection->id) {
            this->collections->erase(collections->begin() + i);
        }
    }
    delete collection;
    this->save();
}
void PersistenceManager::deleteCollection(int index) {
    if(index < this->collections->size() - 1) collections->erase(collections->begin() + index);
}
QStringList PersistenceManager::collectionTitles() {
    QStringList stringlist;
    for(SXCUInterface::SXCUCollection *entry : *collections) stringlist.append(entry->title);
    return stringlist;
}
void PersistenceManager::reset() {
    this->entries = new std::vector<SXCUInterface::JSONParsable*>();
    this->collections = new std::vector<SXCUInterface::SXCUCollection*>();
    this->subdomains = new std::vector<SXCUInterface::SXCUSubdomain*>();
}
void PersistenceManager::setup(const QString &filePath) {
    this->path = new QDir(filePath);
    if(!filePath.isEmpty()) {
        QFile *inputFile = new QFile(filePath + "/data.json");
        if(inputFile->exists()) {
            if(inputFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonParseError error;
                QByteArray jsonData = inputFile->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);
                qDebug() << "JSON error:" << error.errorString();
                this->readJSON(jsonDoc.object());
                inputFile->close();
                delete inputFile;
                return;
            } else {
                qWarning() << "Input file" << filePath + "/data.json" << "exists, but cannot be read! Cancelling...";
            }
        } else {
            qWarning() << "Input file" << filePath + "/data.json" << "does not exist! Setting up new entries...";
            // at least ensure that the file will be able to be created
            QDir().mkpath(QFileInfo(*inputFile).dir().path());
        }
    } else {
        qWarning() << "Missing file path! Will not be able to persist!";
    }
    entries = new std::vector<SXCUInterface::JSONParsable*>();
}

bool PersistenceManager::save() {
    if(!path || path->isEmpty()) {
        qWarning() << "Missing path to save to!";
    } else {
        if(!path->exists()) path->mkdir(".");

        QString dataPath = path->absolutePath() + "/data.json";

        qDebug() << "Saving by overwriting " + dataPath;

        QJsonDocument jsonDoc = this->writeJSON();
        QFile *outputFile = new QFile(dataPath);
        if(outputFile->open(QIODevice::WriteOnly)) {
            qint64 outBytes = outputFile->write(jsonDoc.toJson());
            qDebug() << "Written" << outBytes << "to" << *path;
            outputFile->close();
            return true;
        } else {
            qWarning() << "Could not open save file!";
        }
    }

    qWarning() << "Cancelling save...";
    return false;
}
bool PersistenceManager::readJSON(const QJsonObject &object) {
    if(object.contains("entries") && object.value("entries").isArray()) {
        QJsonArray jsonEntries = object.value("entries").toArray();
        for(int i = 0; i < jsonEntries.count(); i++) {
            QJsonObject jsonObject = jsonEntries[i].toObject();
            if(jsonObject.contains("DATA_TYPE") && jsonObject.value("DATA_TYPE").isString()) {
                QString type = jsonObject.value("DATA_TYPE").toString();
                if(type == SXCUInterface::SXCUFile::type) {
                    entries->push_back(new SXCUInterface::SXCUFile(jsonObject));
                }
            }
        }
    }
    if(object.contains("collections") && object.value("collections").isArray()) {
        QJsonArray jsonCollections = object.value("collections").toArray();
        for(int i = 0; i < jsonCollections.count(); i++) {
            QJsonObject jsonObject = jsonCollections[i].toObject();
            if(jsonObject.contains("DATA_TYPE") && jsonObject.value("DATA_TYPE").isString()) {
                QString type = jsonObject.value("DATA_TYPE").toString();
                if(type == SXCUInterface::SXCUCollection::type) {
                    entries->push_back(new SXCUInterface::SXCUCollection(jsonObject));
                }
            }
        }
    }
    if(object.contains("subdomains") && object.value("subdomains").isArray()) {
        QJsonArray jsonSubdomains = object.value("subdomains").toArray();
        for(int i = 0; i < jsonSubdomains.count(); i++) {
            QJsonObject jsonObject = jsonSubdomains[i].toObject();
            if(jsonObject.contains("DATA_TYPE") && jsonObject.value("DATA_TYPE").isString()) {
                QString type = jsonObject.value("DATA_TYPE").toString();
                if(type == SXCUInterface::SXCUSubdomain::type) {
                    entries->push_back(new SXCUInterface::SXCUSubdomain(jsonObject));
                }
            }
        }
    }
    return true;
}
QJsonDocument PersistenceManager::writeJSON() {
    qDebug() << "Initialised saving entries to in-memory JSON";
    QJsonArray jsonEntries = QJsonArray();
    QJsonArray jsonCollections = QJsonArray();
    QJsonArray jsonSubdomains = QJsonArray();
    if(entries) for(int i = 0; i < entries->size(); i++) jsonEntries.append(entries->at(i)->toJSON());
    if(collections) for(int i = 0; i < collections->size(); i++) jsonCollections.append(collections->at(i)->toJSON());
    if(subdomains) for(int i = 0; i < subdomains->size(); i++) jsonSubdomains.append(subdomains->at(i)->toJSON());
    //jsonEntries.count() == entries->size() && jsonCollections.count() == collections->size() && jsonSubdomains.count() == subdomains->size()
    //    ? qDebug() << "Successfully saved entries, collections and subdomains to JSON"
    //    : qWarning() << "Failed saving to JSON!";
    QJsonObject object = {{"entries", jsonEntries}, {"collections", jsonCollections}, {"subdomains", jsonSubdomains}};
    return QJsonDocument(object);
}

int PersistenceManager::rowCount(const QModelIndex &parent) const {
    //eturn 2; // title, link
    qDebug() << "row count queried; currently" << entries->size() << "entries available.";
    return entries->size();
}
int PersistenceManager::columnCount(const QModelIndex &parent) const {
    qDebug() << "column count queried; currently 2 columns active";
    return 2;
}
QVariant PersistenceManager::data(const QModelIndex &index, int role) const {
    SXCUInterface::SXCUFile *field = (SXCUInterface::SXCUFile*)entries->at(index.row());
    qDebug() << "Entry at row" << index.row() << ":" << field->id;
    QVariant data;
    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0:
            data = field->og_properties ? QVariant(field->og_properties->title) : field->id;
            break;
        case 1:
            data = field->url;
            break;
        default:
            data = "N/A";
        }
    }
    return data;
}

QVariant PersistenceManager::headerData(int section, Qt::Orientation orientation, int role) const {
    qDebug() << "Fetching header data for section" << section;
    QVariant data;
    if(role == Qt::DisplayRole) {
        switch(section) {
        case 0:
            data = "title";
            break;
        case 1:
            data = "link";
            break;
        default:
            data = "N/A";
        }
    }
    return data;
}
/*
Qt::ItemFlags PersistenceManager::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags &= ~Qt::ItemIsUserCheckable;
    return flags;
}
*/
