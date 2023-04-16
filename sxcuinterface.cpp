#include "sxcuinterface.h"
#include <QHttpPart>
#include <QtNetwork>
#include <QHashIterator>

const QString SXCUInterface::apiEndpoint                    = "https://sxcu.net/api";

const QString SXCUInterface::JSONParsable::type             = "JSONParsable_DONOTUSE";
const QString SXCUInterface::SXCUOpenGraphProperties::type  = "SXCUOpenGraphProperties";
const QString SXCUInterface::SXCUFile::type                 = "SXCUFile";
const QString SXCUInterface::SXCUSubdomain::type            = "SXCUSubdomain";
const QString SXCUInterface::SXCUCollection::type           = "SXCUCollection";
const QString SXCUInterface::SXCULink::type                 = "SXCULink";
const QString SXCUInterface::SXCUError::type                = "SXCUError";

QJsonObject SXCUInterface::JSONParsable::toJSON() {
    return QJsonObject();
}

SXCUInterface::SXCUOpenGraphProperties::SXCUOpenGraphProperties(const QJsonObject &object) {
    if(object.contains("color") && object.value("color").isString()) {
        this->color = object.value("color").toString();
    }
    if(object.contains("title") && object.value("title").isString()) {
        this->title = object.value("title").toString();
    }
    if(object.contains("description") && object.value("description").isString()) {
        this->description = object.value("description").toString();
    }
    if(object.contains("discord_hide_url") && object.value("discord_hide_url").isBool()) {
        this->discord_hide_url = object.value("discord_hide_url").toBool();
    }
}
QJsonObject SXCUInterface::SXCUOpenGraphProperties::toJSON() {
    QJsonObject object = QJsonObject();
    object["DATA_TYPE"] = SXCUInterface::SXCUOpenGraphProperties::type;
    object["color"] = color;
    object["title"] = title;
    object["description"] = description;
    object["discord_hide_url"] = discord_hide_url;
    return object;
}


SXCUInterface::SXCUFile::SXCUFile() {}
SXCUInterface::SXCUFile::SXCUFile(const QJsonObject &object) {
    if(object.contains("id") && object.value("id").isString()) {
        this->id = object.value("id").toString();
    }
    if(object.contains("url") && object.value("url").isString()) {
        this->url = QUrl(object.value("url").toString().replace("%F0%9F%92%A3", ""));
        // please do not ask where the bomb is coming from, it is literally a part
        // of the server response and does not affect the validity of the link
    }
    if(object.contains("del_url") && object.value("del_url").isString()) {
        this->del_url = QUrl(object.value("del_url").toString());
    }
    if(object.contains("thumb") && object.value("thumb").isString()) {
        this->thumb = QUrl(object.value("thumb").toString());
    }
    if(object.contains("views") && object.value("views").isDouble()) {
        this->views = object.value("views").toInt();
    }
    if(object.contains("viewable") && object.value("viewable").isBool()) {
        this->viewable = object.value("viewable").toBool();
    }
    if(object.contains("collection") && object.value("collection").isString()) {
        this->collection = new QString(object.value("collection").toString());
    }
    if(object.contains("size") && object.value("size").isDouble()) {
        this->size = object.value("size").toInt();
    }
    if(object.contains("creation_time") && object.value("creation_time").isDouble()) {
        this->creation_time = object.value("creation_time").toInt();
    }
    if(object.contains("og_properties") && !object.value("og_properties").isNull()) {
        this->og_properties = new SXCUOpenGraphProperties(object.value("og_properties").toObject());
    }
}
QJsonObject SXCUInterface::SXCUFile::toJSON() {
    QJsonObject object = QJsonObject();
    object["DATA_TYPE"] = SXCUInterface::SXCUFile::type;
    object["id"] = id;
    object["url"] = QJsonValue::fromVariant(url);
    object["del_url"] = QJsonValue::fromVariant(del_url);
    object["thumb"] = QJsonValue::fromVariant(thumb);
    object["views"] = views;
    object["viewable"] = viewable;
    object["collection"] = collection ? QJsonValue::fromVariant(*collection) : QJsonValue::Null;
    object["size"] = size;
    object["creation_time"] = creation_time;
    object["og_properties"] = og_properties ? QJsonValue(og_properties->toJSON()) : QJsonValue::Null;
    return object;
}
SXCUInterface::SXCUFile::~SXCUFile() {
    delete og_properties;
    delete collection;
}

SXCUInterface::SXCUSubdomain::SXCUSubdomain(const QJsonObject &object) {
    if(object.contains("id") && object.value("id").isString()) {
        this->id = object.value("id").toString();
    }
    if(object.contains("domain") && object.value("domain").isString()) {
        this->domain = object.value("domain").toString();
    }
    if(object.contains("files") && object.value("files").isDouble()) {
        this->upload_count = object.value("files").toInt();
    }
    if(object.contains("upload_count") && object.value("upload_count").isDouble()) {
        this->upload_count = object.value("upload_count").toInt();
    }
    if(object.contains("links") && object.value("links").isDouble()) {
        this->links = object.value("links").toInt();
    }
    if(object.contains("file_views") && object.value("file_views").isDouble()) {
        this->file_views = object.value("file_views").toInt();
    }
    if(object.contains("public") && object.value("public").isBool()) {
        this->needs_token = !object.value("public").toBool();
    }
    if(object.contains("root") && object.value("root").isBool()) {
        this->root = object.value("root").toInt();
    }
    if(object.contains("last_activity") && object.value("last_activity").isDouble()) {
        this->links = object.value("last_activity").toInt();
    }
}
QJsonObject SXCUInterface::SXCUSubdomain::toJSON() {
    QJsonObject object;
    object["DATA_TYPE"] = SXCUInterface::SXCUSubdomain::type;
    object["id"] = id;
    object["domain"] = domain;
    object["upload_count"] = upload_count;
    object["links"] = links;
    object["file_views"] = file_views;
    object["public"] = needs_token;
    object["root"] = root;
    object["last_activity"] = last_activity;
    return object;
}

SXCUInterface::SXCUCollection::SXCUCollection(const QJsonObject &object) {
    if(object.contains("id") && object.value("id").isString()) {
        this->id = object.value("id").toString();
    }
    if(object.contains("collection_id") && object.value("collection_id").isString()) {
        this->id = object.value("collection_id").toString();
    }
    if(object.contains("title") && object.value("title").isString()) {
        this->title = object.value("title").toString();
    }
    if(object.contains("desc") && object.value("desc").isString()) {
        this->desc = object.value("desc").toString();
    }
    if(object.contains("views") && object.value("views").isDouble()) {
        this->views = object.value("views").toInt();
    }
    if(object.contains("creation_time") && object.value("creation_time").isDouble()) {
        this->creation_time = object.value("creation_time").toInt();
    }
    if(object.contains("public") && object.value("public").isBool()) {
        this->needs_token = !object.value("public").toBool();
    }
    if(object.contains("private") && object.value("private").isBool()) {
        this->needs_token = object.value("private").toBool();
    }
    if(object.contains("unlisted") && object.value("unlisted").toBool()) {
        this->unlisted = object.value("unlisted").toBool();
    }
    if(object.contains("file_views") && object.value("file_views").isDouble()) {
        this->file_views = object.value("file_views").toInt();
    }
    if(object.contains("files") && object.value("files").isArray()) {
        QJsonArray jsonFiles = object.value("files").toArray();
        std::vector<SXCUFile*> files;
        for(int i = 0; i < jsonFiles.count(); i++) {
            SXCUInterface::SXCUFile *file = new SXCUFile(jsonFiles[i].toObject());
            files.push_back(file);
        }
        this->files = files;
    }
}
QJsonObject SXCUInterface::SXCUCollection::toJSON() {
    QJsonObject object = QJsonObject();
    object["DATA_TYPE"] = SXCUInterface::SXCUCollection::type;
    object["id"] = id;
    object["title"] = title;
    object["desc"] = desc;
    object["views"] = views;
    object["creation_time"] = creation_time;
    object["public"] = !needs_token;
    object["token"] = token;
    object["unlisted"] = unlisted;
    object["file_views"] = file_views;
    QJsonArray jsonFiles;
    for(int i = 0; i < files.size(); i++) {
        jsonFiles.append(files[i]->toJSON());
    }
    object["files"] = QJsonValue(jsonFiles);
    return object;
}
SXCUInterface::SXCUCollection::~SXCUCollection() {
    for(SXCUInterface::JSONParsable *file : files) delete file;
}
SXCUInterface::SXCULink::SXCULink(const QJsonObject &object) {
    if(object.contains("url") && object.value("url").isString()) {
        this->url = QUrl(object.value("url").toString());
    }
    if(object.contains("del_url") && object.value("del_url").isString()) {
        this->del_url = QUrl(object.value("del_url").toString());
    }
}
QJsonObject SXCUInterface::SXCULink::toJSON() {
    QJsonObject object;
    object["DATA_TYPE"] = SXCUInterface::SXCULink::type;
    object["url"] = QJsonValue::fromVariant(url);
    object["del_url"] = QJsonValue::fromVariant(del_url);
    return object;
}
SXCUInterface::SXCUError::SXCUError(const QJsonObject &object) {
    if(object.contains("code") && object.value("code").toDouble()) {
        this->code = object.value("code").toInt();
    }
    if(object.contains("error") && object.value("error").isString()) {
        this->error = object.value("error").toString();
    }
}
SXCUInterface::SXCUError::SXCUError(int code, QString error) {
    this->code = code;
    this->error = error;
}
QJsonObject SXCUInterface::SXCUError::toJSON() {
    QJsonObject object;
    object["code"] = code;
    object["error"] = error;
    return object;
}

SXCUInterface::SXCUInterface() {
    qnam = new QNetworkAccessManager(this);
}
SXCUInterface::~SXCUInterface() {
    delete qnam;
}
void SXCUInterface::fetchFile(QString id, std::function<void(SXCUFile *file, SXCUError *error)> completionHandler) {
    request("GET", QUrl(apiEndpoint + "/files" + id), [=](QByteArray result, QNetworkReply::NetworkError *error){
        if(error) qWarning() << "An error occured: " << *error << "(" << result << ")";
        completionHandler(SXCUInterface::parseJSON<SXCUFile>(result), SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::createFile(QString filePath, QString collection, QString collection_token, bool no_embed, bool self_destruct, std::function<void(SXCUInterface::SXCUFile *file, SXCUError *error)> completionHandler) {
    if(filePath.isEmpty()) {
        qWarning() << "file parameter for uploading missing!";
        return;
    }
    QHash<QString, QVariant> *params = new QHash<QString, QVariant>();
    params->insert("file", filePath);
    if(!collection.isEmpty()) params->insert("collection", collection);
    if(!collection_token.isEmpty()) params->insert("collection_token", collection_token);
    if(no_embed) params->insert("noembed", QVariant(no_embed));
    if(self_destruct) params->insert("self_destruct", QVariant(self_destruct));

    request("POST", QUrl(apiEndpoint + "/files/create"), [=](QByteArray result, QNetworkReply::NetworkError *error){
            qDebug() << "completionHandler received the results for the file POST, lets see...";
            if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
            completionHandler(SXCUInterface::parseJSON<SXCUFile>(result), nullptr); // FIXME: map NetworkError to custom errors
        }, params, "multipart-formdata");
}
/*
void SXCUInterface::deleteFile(QString id, QString deletion_token, void (*completionHandler)(QException *error)) {
    if(id.isEmpty() || deletion_token.isEmpty()) {
        if(id.isEmpty()) qWarning() << "";
        if(deletion_token.isEmpty()) qWarning() << "";
        return;
    }
    request("GET", QUrl(apiEndpoint + "/files/delete" + id + "/" + deletion_token), [=](QByteArray result, QNetworkReply::NetworkError *error){
        if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
        completionHandler(nullptr);
    });
}
*/
void SXCUInterface::deleteFile(SXCUFile *file, std::function<void(SXCUError *error)> completionHandler) {
    if(!file || file->del_url.isEmpty()) {
        qWarning() << "Tried to delete a file that is missing or missing its del_url!";
        return;
    }
    request("GET", file->del_url, [=](QByteArray result, QNetworkReply::NetworkError *error){
        completionHandler(SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::fetchSubdomainList(std::function<void(std::vector<SXCUSubdomain *>* subdomains, SXCUError *error)> completionHandler) {
    request("GET", QUrl(apiEndpoint + "/subdomains"), [=](QByteArray result, QNetworkReply::NetworkError *error){
            if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
            QJsonDocument document = QJsonDocument::fromJson(result);
            std::vector<SXCUSubdomain *> *subdomains = new std::vector<SXCUSubdomain *>();
            if(!document.isNull() && document.isArray()) {
                QJsonArray jsonDomains = document.array();
                for(int i = 0; i < jsonDomains.count(); i++) subdomains->push_back(new SXCUSubdomain(jsonDomains[i].toObject()));
            }
            completionHandler(subdomains, SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::fetchSubdomain(QString name, std::function<void(SXCUSubdomain *subdomain, SXCUError *error)> completionHandler) {
    request("GET", apiEndpoint + "/subdomains/" + name, [=](QByteArray result, QNetworkReply::NetworkError *error) {
        if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
        completionHandler(SXCUInterface::parseJSON<SXCUSubdomain>(result), SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::fetchSubdomainStatus(QString name, std::function<void(bool exists, SXCUError *error)> completionHandler) {
    request("GET", apiEndpoint + "/subdomains/check/" + name, [=](QByteArray result, QNetworkReply::NetworkError *error){
        if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
        QJsonDocument document = QJsonDocument::fromJson(result);
        bool exists = false;
        if(!document.isNull() && document.object().contains("exists")) exists = document.object().value("exists").toBool();
        completionHandler(exists, SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::fetchCollection(QString id, std::function<void(SXCUCollection *collection, SXCUError *error)> completionHandler) {
    request("GET", apiEndpoint + "/collections/" + id, [=](QByteArray result, QNetworkReply::NetworkError *error){
        if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
        completionHandler(SXCUInterface::parseJSON<SXCUCollection>(result), SXCUInterface::parseJSON<SXCUError>(result));
    });
}
void SXCUInterface::createCollection(QString title, QString description, bool needs_token, bool unlisted, std::function<void(SXCUCollection *collection, SXCUError *error)> completionHandler) {
    QHash<QString, QVariant> *params = new QHash<QString, QVariant>();
    params->insert("title", title);
    params->insert("desc", description);
    params->insert("private", needs_token);
    params->insert("unlisted", unlisted);

    request("POST", apiEndpoint + "/collections/create", [=](QByteArray result, QNetworkReply::NetworkError *error){
            if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
            completionHandler(SXCUInterface::parseJSON<SXCUCollection>(result), SXCUInterface::parseJSON<SXCUError>(result));
        }, params, "url-encoded");
}
void SXCUInterface::createLinkRedirect(QUrl link, std::function<void(SXCULink *redirect, SXCUError *error)> completionHandler) {
    QHash<QString, QVariant> *params = new QHash<QString, QVariant>();
    params->insert("link", link);

    request("POST", apiEndpoint + "links/create", [=](QByteArray result, QNetworkReply::NetworkError *error){
            if(*error) qWarning() << "An error occured:" << *error << "(" << result << ")";
            completionHandler(SXCUInterface::parseJSON<SXCULink>(result), SXCUInterface::parseJSON<SXCUError>(result));
        }, params, "url-encoded");
}
void SXCUInterface::deleteLinkRedirect(QUrl deletionLink, std::function<void(bool successfull, SXCUError *error)> completionHandler) {
    request("GET", deletionLink, [=](QByteArray result, QNetworkReply::NetworkError *error){
        completionHandler(!error, SXCUInterface::parseJSON<SXCUError>(result));
    });
}
template <typename JSONParsableSubclass>
JSONParsableSubclass* SXCUInterface::parseJSON(QByteArray data) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if(!jsonDoc.isNull()) {
        qDebug() << "Data contains valid JSON data, trying to build SXCUFile";
        JSONParsableSubclass *result = new JSONParsableSubclass(jsonDoc.object());
        result ? qDebug() << "constructed successfully!" : qWarning() << "construction failed, passing nullptr!";
        return result;
    } else {
        qWarning() << "Returned content is no valid JSON!";
        return nullptr;
    }
}
void SXCUInterface::request(QString method, QUrl url, std::function<void(QByteArray, QNetworkReply::NetworkError*)> completionHandler, QHash<QString, QVariant> *params, QString postType) {
    qDebug() << "Building request for" << url << "; using method" << method << (params ? "with" : "without") << "parameters";

    QNetworkRequest request(url);
    QNetworkReply *reply = nullptr;
    if(method == "GET") {
        if(params && !params->isEmpty()) {
            QUrlQuery *query = new QUrlQuery(url);
            QHashIterator<QString, QVariant> i (*params);
            while(i.hasNext()) {
                i.next();
                query->addQueryItem(i.key(), params->value(i.key()).toString());
            }
            url.setQuery(*query);
            QNetworkRequest urlrequest(url);
            request = urlrequest;
        }
        reply = qnam->get(request);
        qDebug() << "Request building done";
    } else if(method == "POST") {
        if(params && !params->isEmpty() && !postType.isEmpty()) {

            if(postType == "multipart-formdata") {
                //request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

                QHttpMultiPart *multipartData = new QHttpMultiPart(QHttpMultiPart::FormDataType);
                QHashIterator<QString, QVariant> i (*params);
                while(i.hasNext()) {
                    i.next();
                    QString key = i.key();

                    QHttpPart partData;
                    if(key == "file") {
                        QFile *file = new QFile(params->value(key).toString());
                        QString pathExt = (new QFileInfo(*file))->completeSuffix();
                        partData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/" + pathExt));
                        partData.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + file->fileName() + "\""));


                        file->open(QIODevice::ReadOnly);
                        partData.setBodyDevice(file);
                        file->setParent(multipartData);
                    } else {
                        partData.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + key + "\""));
                        partData.setBody(params->value(key).toByteArray());
                    }
                    multipartData->append(partData);
                }

                multipartData->setParent(reply);
                qDebug() << "Built multipart request with keys:" << params->keys();
                reply = qnam->post(request, multipartData);
            } else if(postType == "url-encoded") {
                QString data;
                QHashIterator<QString, QVariant> i (*params);
                while(i.hasNext()) {
                    i.next();
                    data.append(i.key() + "=" + i.value().toString() + "&");
                }
                data = data.chopped(1);
                QNetworkRequest request(url);
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                reply = qnam->post(request, data.toUtf8()); // FIXME: is there really no other way?!
            } else {
                qDebug() << "Missing POST type!";
            }

        } else {
            qDebug() << "Declared POST method, but missing data and/or type!";
        }
    } else if(method == "DELETE") {

    } else {
        qDebug() << "Using wrong HTTP Method! Only supports GET, POST, DELETE, got" << method;
    }

    if(reply != nullptr) {
        qDebug() << "Request constructed and fired successfully, binding to QNetworkReply::finished(...)";
        connect(reply, &QNetworkReply::finished, this, [=](){
            QByteArray data = reply->readAll();
            qDebug() << "QNetworkRequest finished; Calling completionhandler with data:" << data;
            QNetworkReply::NetworkError error = reply->error();
            completionHandler(data, &error);
        });
    }
}
