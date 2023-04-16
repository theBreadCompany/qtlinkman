#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include "sxcuinterface.h"
#include <QJsonDocument>
#include <QFile>
#include <QAbstractTableModel>

class PersistenceManager : public QAbstractTableModel
{
    Q_OBJECT
public:
    QDir *path;

    PersistenceManager();
    ~PersistenceManager();

    void addEntry(SXCUInterface::JSONParsable *entry);
    void deleteEntry(SXCUInterface::JSONParsable *entry);
    void deleteEntry(int index);

    void addCollection(SXCUInterface::SXCUCollection *collection);
    void deleteCollection(SXCUInterface::SXCUCollection *collection);
    void deleteCollection(int index);

    SXCUInterface::SXCUFile *entryForTableindex(int index);
    SXCUInterface::SXCUCollection *collectionForIndex(int index);
    SXCUInterface::SXCUCollection *collectionForTitle(const QString &title);
    QStringList collectionTitles();
    void reset();
    void setup(const QString &filePath);


    // QAbstractTableModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<SXCUInterface::JSONParsable*> *entries;
    std::vector<SXCUInterface::SXCUCollection*> *collections;
    std::vector<SXCUInterface::SXCUSubdomain*>*subdomains;


    bool save();
    bool readJSON(const QJsonObject &object);
    QJsonDocument writeJSON();
};

#endif // PERSISTENCEMANAGER_H
