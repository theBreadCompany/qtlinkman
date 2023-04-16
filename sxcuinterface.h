#ifndef SXCUINTERFACE_H
#define SXCUINTERFACE_H


#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QJsonObject>
#include <QHash>

class SXCUInterface : QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager *qnam;
    static const QString apiEndpoint;

public:
    // -- Data classes --
    class JSONParsable {
    private:
        QHash<QString, std::type_info> classMap;
    public:
        virtual QJsonObject toJSON();
        static const QString type;
    };

    class SXCUOpenGraphProperties : JSONParsable {
    public:
        SXCUOpenGraphProperties(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;


        QString color;
        QString title;
        QString description;
        bool discord_hide_url;
    };

    class SXCUFile : public JSONParsable {
    public:
        SXCUFile();
        SXCUFile(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;
        ~SXCUFile();

        QString id;                             // upload, meta and SXCUCollection().files
        QUrl url;                               // upload, meta and SXCUCollection().files
        QUrl del_url;                           // upload
        QUrl thumb;                             // upload and SXCUCollection().files
        int views;                              // meta and SXCUCollection().files
        bool viewable;                          // meta
        QString* collection;                    // meta, optional
        int size;                               // meta
        int creation_time;                      // meta
        SXCUOpenGraphProperties *og_properties; // optional
    };

    class SXCUSubdomain : public JSONParsable {
    public:
        SXCUSubdomain(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;

        QString id;         // meta
        QString domain;     // list
        int upload_count;   // list; called "files" in meta, but upload_count sounds more descriptive
        int links;          // meta
        int file_views;     // list and meta
        bool needs_token;   // list and meta; aka public, which is a reserved keyword
        bool root;          // meta
        int last_activity;  // meta
    };

    class SXCUCollection : public JSONParsable {
    public:
        SXCUCollection(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;
        ~SXCUCollection();

        QString id;                     // meta; called collection_id in upload
        QString title;                  // meta and create
        QString desc;                   // upload; called desc in collection
        int views;                      // meta
        int creation_time;              // meta
        bool needs_token;               // aka public and private, which are reserved keywords; token only required for file uploads
        QString token;                  // upload
        bool unlisted;                  // meta
        int file_views;                 // meta
        std::vector<SXCUFile*> files;   // meta;
    };

    class SXCULink : public JSONParsable {
    public:
        SXCULink(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;
        QUrl url;
        QUrl del_url;
    };


    // -- Error classes --
    class SXCUError : public JSONParsable {
    public:
        SXCUError(int code, QString error);
        SXCUError(const QJsonObject &object);
        QJsonObject toJSON() override;
        static const QString type;
        int code;
        QString error;
    };

    SXCUInterface();
    ~SXCUInterface();

    void fetchFile(QString id, std::function<void(SXCUFile *file, SXCUError *error)> completionHandler);
    void createFile(QString filePath, QString collection, QString collection_token, bool no_embed, bool self_destruct, std::function<void(SXCUInterface::SXCUFile *file, SXCUError *error)>);
    //void deleteFile(QString id, QString deletion_token, void (*completionHandler)(SXCUError *error)); yes, but actually no; the server returns a URL that is at no point taken apart, so why do it now
    void deleteFile(SXCUFile *file, std::function<void(SXCUError *error)> completionHandler);

    void fetchSubdomainList(std::function<void(std::vector<SXCUSubdomain*>* subdomains, SXCUError *error)> completionHandler);
    void fetchSubdomain(QString name, std::function<void(SXCUSubdomain *subdomain, SXCUError *error)> completionHandler);
    void fetchSubdomainStatus(QString name, std::function<void(bool exists, SXCUError *error)> completionHandler);

    void fetchCollection(QString id, std::function<void(SXCUCollection *collection, SXCUError *error)> completionHandler);
    void createCollection(QString title, QString description, bool needs_token, bool unlisted, std::function<void(SXCUCollection *collection, SXCUError *error)> completionHandler);

    void createLinkRedirect(QUrl link, std::function<void(SXCULink *redirect, SXCUError *error)> completionHandler);
    void deleteLinkRedirect(QUrl deletionLink, std::function<void(bool successfull, SXCUError *error)> completionHandler = [](bool successfull, SXCUError *error){});

private:
    template <typename JSONParsableSubclass>
    static JSONParsableSubclass* parseJSON(QByteArray data);
    void request(QString method, QUrl url, std::function<void(QByteArray, QNetworkReply::NetworkError*)> completionHandler, QHash<QString, QVariant> *params = nullptr, QString postType = "");
};



#endif // SXCUINTERFACE_H
