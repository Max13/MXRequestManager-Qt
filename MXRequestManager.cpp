/**
 * @file		mxrequestmanager.cpp
 * @brief		MXRequestManager
 *
 * @details		REST Request Manager by Max13
 *
 * @version		1.0
 * @author		Adnan "Max13" RIHAN <adnan@rihan.fr>
 * @link		http://rihan.fr/
 * @copyright	http://creativecommons.org/licenses/by-sa/3.0/	CC-by-sa 3.0
 *
 * LICENSE: This source file is subject to the "Attribution-ShareAlike 3.0 Unported"
 * of the Creative Commons license, that is available through the world-wide-web
 * at the following URI: http://creativecommons.org/licenses/by-sa/3.0/.
 * If you did not receive a copy of this Creative Commons License and are unable
 * to obtain it through the web, please send a note to:
 * "Creative Commons, 171 Second Street, Suite 300,
 * San Francisco, California 94105, USA" so we can mail you a copy immediately.
 */

#include "MXRequestManager.hpp"

MXRequestManager::MXRequestManager(QObject *parent) : QNetworkAccessManager(parent), m_lastHttpCode(0)
{
    this->m_responseType = JSON;
    this->m_netReply = NULL;
    this->m_netRequest = new QNetworkRequest;
    this->setUserAgent();

    // Hack: Keychain access
    this->m_netProxy = this->proxy();
    this->m_netProxy.setHostName(" ");
    this->setProxy(this->m_netProxy);
    // ---

    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(QUrl apiUrl, QString authUser,
                                   QString authPass, QObject *parent)
    : QNetworkAccessManager(parent), m_lastHttpCode(0)
{
    this->m_responseType = JSON;
    this->m_netReply = NULL;
    this->m_netBaseApiUrl = apiUrl;
    if (!authUser.isEmpty() || !authPass.isEmpty())
    {
        this->m_netAuthUser = authUser;
        this->m_netAuthPass = authPass;
    }
    this->m_netRequest = new QNetworkRequest;
    this->setUserAgent();

    // Hack: Keychain access
    this->m_netProxy = this->proxy();
    this->m_netProxy.setHostName(" ");
    this->setProxy(this->m_netProxy);
    // ---

    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(MXRequestManager const& other)
    : QNetworkAccessManager(other.parent()), m_lastHttpCode(0)
{
    this->m_responseType = other.m_responseType;
    this->m_netDataRaw = other.m_netDataRaw;
    this->m_netReply = other.m_netReply;
    this->m_netRequest = new QNetworkRequest(*(other.m_netRequest));
    this->m_netAuthUser = other.m_netAuthUser;
    this->m_netAuthPass = other.m_netAuthPass;
    this->m_netBaseApiUrl = other.m_netBaseApiUrl;

    // Hack: Keychain access
    this->setProxy(other.m_netProxy);
    // ---

    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::~MXRequestManager()
{
    delete this->m_netReply;
    this->m_netReply = NULL;
    delete this->m_netRequest;
    this->m_netRequest = NULL;
}
// ---

// Operators Overloads
MXRequestManager&	MXRequestManager::operator=(MXRequestManager const& other)
{
    this->setParent(other.parent());
    this->m_netDataRaw = other.m_netDataRaw;
    this->m_netReply = other.m_netReply;
    this->m_netRequest = other.m_netRequest;
    this->m_netAuthUser = other.m_netAuthUser;
    this->m_netAuthPass = other.m_netAuthPass;
    this->m_netBaseApiUrl = other.m_netBaseApiUrl;

    return (*this);
}
// ---

// Getters / Setters
QString	MXRequestManager::authUser(void) const
{
    return (this->m_netAuthUser);
}

QString	MXRequestManager::authPass(void) const
{
    return (this->m_netAuthPass);
}

QUrl	MXRequestManager::apiUrl(void) const
{
    return (this->m_netBaseApiUrl);
}

QString	MXRequestManager::userAgent(void) const
{
    return (this->m_netRequest->rawHeader("User-Agent"));
}

int     MXRequestManager::lastHttpCode(void) const
{
    return (this->m_lastHttpCode);
}

QNetworkRequest const&  MXRequestManager::networkRequest(void)
{
    return (*(this->m_netRequest));
}

QNetworkReply const&  MXRequestManager::networkReply(void)
{
    return (*(this->m_netReply));
}

QByteArray	const&	MXRequestManager::rawData(void) const
{
    return (this->m_netDataRaw);
}

QVariantMap	const&	MXRequestManager::data(void) const
{
    return (this->m_netDataMap);
}

void	MXRequestManager::setAuthUser(QString const& authUser)
{
    this->m_netAuthUser = authUser;
}

void	MXRequestManager::setAuthPass(QString const& authPass)
{
    this->m_netAuthPass = authPass;
}

void	MXRequestManager::setApiUrl(QUrl const& apiUrl)
{
    this->m_netBaseApiUrl = apiUrl;
}

void	MXRequestManager::setUserAgent(QString const& userAgent)
{
    QByteArray	ua;

    if (userAgent.isEmpty())
        ua.append(MXREQUESTMANAGER_NAME)
                .append('/').append(MXREQUESTMANAGER_VERSION)
                .append(' ').append("Qt/").append(QT_VERSION_STR)
                .append('/').append(MXREQUESTMANAGER_PLATEFORM);
    else
        ua.append(userAgent);
    this->m_netRequest->setRawHeader("User-Agent", ua);
}

void	MXRequestManager::setResponseType(SupportedContentTypes const& responseType)
{
    this->m_responseType = responseType;
}

// ---

// Treatments
bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXMap const& data)
{
    MXMapIterator	i(data);
    MXPairList		params;

    while (i.hasNext())
    {
        i.next();
        params.append(MXPair(QUrl::toPercentEncoding(i.key()),
                             QUrl::toPercentEncoding(i.value())));

    }

    return (this->request(resource, method, params));
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXEncodedMap const& data)
{
    MXEncodedMapIterator	i(data);
    MXPairList				params;

    while (i.hasNext())
    {
        i.next();
        params.append(MXPair(i.key(), i.value()));
    }

    return (this->request(resource, method, params));
}

//bool	MXRequestManager::request(QString const& resource, QString const& method,
//								  MXPairList const& data)
//{
//	int					i = -1;
//	MXEncodedPairList	params;

//	while (++i < data.size())
//		params.append(MXEncodedPair(QUrl::toPercentEncoding(data.at(i).first),
//									QUrl::toPercentEncoding(data.at(i).second)));

//	return (this->request(resource, method, params));
//}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXPairList const& data) // Will be called
{
    this->m_httpAuthCount = 0;

    QUrlQuery	urlQuery;
    QUrl        apiUrl(this->m_netBaseApiUrl);

    apiUrl.setPath(resource);
    urlQuery.setQueryItems(data);
    if (method != "POST")
        apiUrl.setQuery(urlQuery);
    this->m_netRequest->setUrl(apiUrl);

    emit this->begin();

//    if (this->m_responseType == JSON)
//        this->m_netRequest->setRawHeader("Accept", "application/json,application/xml;q=0.9,*/*;q=0.8");
    if (method.toUpper() == "DELETE")
        this->m_netReply = this->deleteResource(*(this->m_netRequest));
    else if (method.toUpper() == "GET")
        this->m_netReply = this->get(*(this->m_netRequest));
    else if (method.toUpper() == "HEAD")
        this->m_netReply = this->head(*(this->m_netRequest));
    else if (method.toUpper() == "POST")
    {
//        QUrl tmpUrl;
//        tmpUrl.setQuery(urlQuery);

//        qDebug() << "Api URL:" << apiUrl;
//        qDebug() << "Method:" << method;
//        qDebug() << "Url Query:" << urlQuery.toString();
//        qDebug() << "tmpUrl:" << tmpUrl;
//        qDebug() << "tmpUrl Query:" << tmpUrl.toEncoded();
//        return (false);

        this->m_netRequest->setHeader(QNetworkRequest::ContentTypeHeader,
                                      "application/x-www-form-urlencoded; charset=utf-8");
        this->m_netReply = this->post(*(this->m_netRequest), urlQuery.toString().toUtf8());
    }
    else if (method.toUpper() == "PUT")
    {
        this->m_netRequest->setHeader(QNetworkRequest::ContentLengthHeader, 0);
        this->m_netReply = this->put(*(this->m_netRequest), (QIODevice*)NULL);
    }
    else
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toLatin1());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXEncodedPairList const& data)
{
    int			i = -1;
    MXPairList	params;

    while (++i < data.size())
        params.append(MXPair(QUrl::toPercentEncoding(data.at(i).first),
                             QUrl::toPercentEncoding(data.at(i).second)));

    return (this->request(resource, method, params));
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QIODevice *data)
{
    this->m_httpAuthCount = 0;
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
//    if (this->m_responseType == JSON)
//        this->m_netRequest->setRawHeader("Accept", "application/json,application/xml;q=0.9,*/*;q=0.8");
    if (method.toUpper() == "DELETE")
        this->m_netReply = this->deleteResource(*(this->m_netRequest));
    else if (method.toUpper() == "GET")
        this->m_netReply = this->get(*(this->m_netRequest));
    else if (method.toUpper() == "HEAD")
        this->m_netReply = this->head(*(this->m_netRequest));
    else if (method.toUpper() == "POST")
        this->m_netReply = this->post(*(this->m_netRequest), data);
    else if (method.toUpper() == "PUT")
        this->m_netReply = this->put(*(this->m_netRequest), data);
    else
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toLatin1());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QByteArray const& data)
{
    this->m_httpAuthCount = 0;
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
//    if (this->m_responseType == JSON)
//        this->m_netRequest->setRawHeader("Accept", "application/json,application/xml;q=0.9,*/*;q=0.8");
    if (method.toUpper() == "DELETE")
        this->m_netReply = this->deleteResource(*(this->m_netRequest));
    else if (method.toUpper() == "GET")
        this->m_netReply = this->get(*(this->m_netRequest));
    else if (method.toUpper() == "HEAD")
        this->m_netReply = this->head(*(this->m_netRequest));
    else if (method.toUpper() == "POST")
        this->m_netReply = this->post(*(this->m_netRequest), data);
    else if (method.toUpper() == "PUT")
        this->m_netReply = this->put(*(this->m_netRequest), data);
    else
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toLatin1());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QHttpMultiPart *data)
{
    this->m_httpAuthCount = 0;
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
//    if (this->m_responseType == JSON)
//        this->m_netRequest->setRawHeader("Accept", "application/json,application/xml;q=0.9,*/*;q=0.8");
    if (method.toUpper() == "DELETE")
        this->m_netReply = this->deleteResource(*(this->m_netRequest));
    else if (method.toUpper() == "GET")
        this->m_netReply = this->get(*(this->m_netRequest));
    else if (method.toUpper() == "HEAD")
        this->m_netReply = this->head(*(this->m_netRequest));
    else if (method.toUpper() == "POST")
        this->m_netReply = this->post(*(this->m_netRequest), data);
    else if (method.toUpper() == "PUT")
        this->m_netReply = this->put(*(this->m_netRequest), data);
    else
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toLatin1());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::parseResponse(QString const& contentType,
                                        QByteArray const& response)
{
    QString	parsingErrorString;

    if (this->m_responseType == JSON)
    {
        if (contentType.left(16).compare("application/json",
                                         Qt::CaseInsensitive) == 0) // JSON
        {
            QJsonParseError	jsonErr;

            this->m_netDataMap = QJsonDocument::fromJson(response, &jsonErr)
                                 .toVariant().toMap();
            if (jsonErr.error == QJsonParseError::NoError)
                return (true);
            parsingErrorString = jsonErr.errorString();
        }
    }

    qDebug() << "Error while parsing...";
    emit this->parsingError();
    return (false);
}
// ---

// Signals / Slots
void	MXRequestManager::requestError(QNetworkReply::NetworkError code)
{
    if (code != QNetworkReply::NoError)
        qDebug() << "Network Error " << code << ": " << this->m_netReply->errorString();
    else
        qDebug() << "Error Emitted: No Error...";

    emit this->finishedWithError();
    emit this->finished(false);
}

void	MXRequestManager::requestFinished(QNetworkReply *reply)
{
    bool requestOk = true;

    this->m_lastHttpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "##### Request Finished #####";
    qDebug() << "----- Request Data -----";
    qDebug() << "API:" << this->m_netRequest->url().toDisplayString();
    qDebug() << "Ressource:" << this->m_netRequest->url().path();
    qDebug() << "Known headers:" << this->m_netRequest->rawHeaderList();
    qDebug() << "Encoded Query:" << this->m_netRequest->url().query(QUrl::FullyEncoded);
    qDebug() << "Method:" <<
                this->m_netRequest->attribute(QNetworkRequest::CustomVerbAttribute,
                            QVariant("Unknown")).toString();
    qDebug() << "----- /Request Data -----";
    qDebug() << "- HTTP Error code:" << this->m_lastHttpCode;
    qDebug() << "- Qt Network Error:" << reply->error() << " - " << reply->errorString();
    if (reply->error() != QNetworkReply::NoError && this->m_lastHttpCode == 0)
        requestOk = false;

    this->m_netDataRaw = reply->readAll();
    qDebug() << "--- Reply ---";
    qDebug() << "- Headers:" << reply->rawHeaderPairs();
    qDebug() << "- Body:" << this->m_netDataRaw;
    qDebug() << "--- /Reply ---";

    qDebug() << "##### /Request Finished #####";

    if (!requestOk)
        this->requestError(reply->error());
    else
        emit this->finished(requestOk &&
                                        this->parseResponse(reply->
                                        header(QNetworkRequest::ContentTypeHeader)
                                        .toString(), this->m_netDataRaw));
}

void	MXRequestManager::requestDownloadProgress(qint64 bytesReceived,
                                                  qint64 bytesTotal)
{
    qDebug() << "Download Request BytesReceived/BytesAvailable: "
             << bytesReceived << '/' << bytesTotal;
    emit this->downloadProgress(bytesReceived, bytesTotal);
}

void	MXRequestManager::requestUploadProgress(qint64 bytesReceived,
                                                qint64 bytesTotal)
{
    qDebug() << "Upload Request BytesReceived/BytesAvailable: "
             << bytesReceived << '/' << bytesTotal;
    emit this->uploadProgress(bytesReceived, bytesTotal);
}

void	MXRequestManager::requestAuth(QNetworkReply /*__attribute__((unused))*/*reply,
                                      QAuthenticator *auth)
{
    if (++this->m_httpAuthCount == 2) {
        qDebug() << "Wrong HTTP Auth credentials, abording.";
        this->m_netReply->abort();
    }

    qDebug() << "HTTP Auth Required (" << reply->size() << "):" << reply->readAll();
    auth->setUser(this->m_netAuthUser);
    auth->setPassword(this->m_netAuthPass);
    qDebug() << "Auth available:" << auth->user() << auth->password();
}
// ---
