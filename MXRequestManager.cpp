/**
 * @file		mxrequestmanager.cpp
 * @brief		MXRequestManager
 *
 * @details		REST Request Manager by Max13
 *
 * @version		0.5
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

MXRequestManager	*MXRequestManager::m_self = NULL;

MXRequestManager::MXRequestManager(QObject *parent) : QNetworkAccessManager(parent)
{
    this->m_valid = false;
    this->m_responseType = RAW;
    this->m_netReply = NULL;
    this->m_netRequest = new QNetworkRequest;
    this->setUserAgent();

    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished()));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(QUrl apiUrl, QString authUser,
                                   QString authPass, QObject *parent)
    : QNetworkAccessManager(parent)
{
    this->m_valid = false;
    this->m_responseType = RAW;
    this->m_netReply = NULL;
    this->m_netBaseApiUrl = apiUrl;
    if (!authUser.isEmpty() || !authPass.isEmpty())
    {
        this->m_netAuthUser = authUser;
        this->m_netAuthPass = authPass;
    }
    this->m_netRequest = new QNetworkRequest;
    this->setUserAgent();

    connect(this, SIGNAL(requestDone()), SLOT(requestFinished()));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(MXRequestManager const& other)
    : QNetworkAccessManager(other.parent())
{
    this->m_valid = other.m_valid;
    this->m_responseType = other.m_responseType;
    this->m_netDataRaw = other.m_netDataRaw;
    this->m_netReply = other.m_netReply;
    this->m_netRequest = new QNetworkRequest(*(other.m_netRequest));
    this->m_netAuthUser = other.m_netAuthUser;
    this->m_netAuthPass = other.m_netAuthPass;
    this->m_netBaseApiUrl = other.m_netBaseApiUrl;

    connect(this, SIGNAL(requestDone()), SLOT(requestFinished()));
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
    this->m_valid = other.m_valid;
    this->m_netDataRaw = other.m_netDataRaw;
    this->m_netReply = other.m_netReply;
    this->m_netRequest = other.m_netRequest;
    this->m_netAuthUser = other.m_netAuthUser;
    this->m_netAuthPass = other.m_netAuthPass;
    this->m_netBaseApiUrl = other.m_netBaseApiUrl;

    return (*this);
}
// ---

// Statics
//bool	MXRequestManager::isAccessible(QUrl const& apiUrl)
//{
//    return (MXRequestManager::isAccessible(apiUrl.host()));
//}

//bool	MXRequestManager::isAccessible(QString const& apiHost)
//{
//    if (apiHost.isEmpty())
//        return (false);

//    QHostInfo	hostInfo = QHostInfo::fromName(apiHost);
//    qDebug() << "HostInfo: Received - " << apiHost;
//    qDebug() << "HostInfo: Error " << hostInfo.errorString();
//    qDebug() << "HostInfo: Addresses.isEmpty() : " << hostInfo.addresses().isEmpty();

//    if (hostInfo.error() != QHostInfo::NoError || hostInfo.addresses().isEmpty())
//        return (false);
//    return (true);
//}
// ---

// Getters / Setters
MXRequestManager	*MXRequestManager::getInstance(void)
{
    if (!MXRequestManager::m_self)
    {
        MXRequestManager::m_self = new MXRequestManager;
    }

    return (MXRequestManager::m_self);
}

MXRequestManager	*MXRequestManager::inst(void)
{
    return (MXRequestManager::getInstance());
}

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

QByteArray	const&		MXRequestManager::rawData(void) const
{
    return (this->m_netDataRaw);
}

QVariant const&			MXRequestManager::data(void) const
{
    return (this->m_netDataVariant);
}

QNetworkReply const&	MXRequestManager::netReply(void) const
{
    return (*(this->m_netReply));
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

bool	MXRequestManager::isValid(void)
{
    return (this->m_valid);
}

// ---

// Treatments
bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXMap const& data)
{
    MXMapIterator		i(data);
    MXEncodedPairList	params;

    while (i.hasNext())
    {
        i.next();
        params.append(MXEncodedPair(QUrl::toPercentEncoding(i.key()),
                             QUrl::toPercentEncoding(i.value())));
    }

    return (this->request(resource, method, params));
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXEncodedMap const& data)
{
    MXEncodedMapIterator	i(data);
    MXEncodedPairList		params;

    while (i.hasNext())
    {
        i.next();
        params.append(MXEncodedPair(i.key(), i.value()));
    }

    return (this->request(resource, method, params));
}


bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXPairList const& data)
{
    int					i = -1;
    MXEncodedPairList	params;

    while (++i < data.size())
        params.append(MXEncodedPair(QUrl::toPercentEncoding(data.at(i).first),
                                    QUrl::toPercentEncoding(data.at(i).second)));

    return (this->request(resource, method, params));
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  MXEncodedPairList const& data)
{
    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    if (method != "POST")
        apiUrl.setEncodedQueryItems(data);
    this->m_netRequest->setUrl(apiUrl);

    this->m_valid = false;
    emit this->begin();

    if (method.toUpper() == "DELETE")
        this->m_netReply = this->deleteResource(*(this->m_netRequest));
    else if (method.toUpper() == "GET")
        this->m_netReply = this->get(*(this->m_netRequest));
    else if (method.toUpper() == "HEAD")
        this->m_netReply = this->head(*(this->m_netRequest));
    else if (method.toUpper() == "POST")
    {
        QUrl tmpApiUrl;
        tmpApiUrl.setEncodedQueryItems(data);

        this->m_netRequest->setHeader(QNetworkRequest::ContentTypeHeader,
                                      "application/octet-stream");
        this->m_netReply = this->post(*(this->m_netRequest), tmpApiUrl.encodedQuery());
    }
    else if (method.toUpper() == "PUT")
    {
        this->m_netRequest->setHeader(QNetworkRequest::ContentLengthHeader, 0);
        this->m_netReply = this->put(*(this->m_netRequest), (QIODevice*)NULL);
    }
    else
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toAscii());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QIODevice *data)
{
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    this->m_valid = false;
    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
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
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toAscii());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QByteArray const& data)
{
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    this->m_valid = false;
    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
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
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toAscii());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}


bool	MXRequestManager::request(QString const& resource, QString const& method,
                                  QHttpMultiPart *data)
{
    if (resource.isEmpty() || method.isEmpty())
        return (false);

    QUrl apiUrl(this->m_netBaseApiUrl);
    apiUrl.setPath(resource);
    this->m_netRequest->setUrl(apiUrl);

    this->m_valid = false;
    emit this->begin();

    this->m_netRequest->setUrl(this->m_netBaseApiUrl.toString()+resource);
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
        this->m_netReply = this->sendCustomRequest(*(this->m_netRequest), method.toAscii());

    connect(this->m_netReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(requestDownloadProgress(qint64,qint64)));
    connect(this->m_netReply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(requestUploadProgress(qint64,qint64)));
    return (true);
}

bool	MXRequestManager::parseResponse(void)
{
    bool parseOk;

    if (this->m_responseType == RAW)
    {
        this->m_netDataVariant.clear();
        return (true);
    }
    else if (this->m_responseType == JSON &&
        this->m_netReply->header(QNetworkRequest::ContentTypeHeader).toString().
        left(16).compare("application/json") == 0) // Expect JSON, got JSON
    {

        this->m_netDataVariant = QtJson::Json::parse(this->m_netDataRaw, parseOk);
        if (parseOk)
            return (true);
    }

    qDebug() << "Error while parsing...";
    emit this->parsingError();
    return (false);
}
// ---

// Signals / Slots

void	MXRequestManager::requestNetworkError(void)
{
    qDebug() << "Network Error " << this->m_netReply->error()
             << ": " << this->m_netReply->errorString();

    emit this->networkError();
}

void	MXRequestManager::requestFinished(void)
{
    bool	requestError = NoError;
    int		httpCode = this->netReply().
                       attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    this->m_netDataRaw = this->m_netReply->readAll();
    qDebug() << "##### Request Finished #####";
    qDebug() << "----- Request Data -----";
    qDebug() << "Known headers:" << this->m_netRequest->rawHeaderList();
    qDebug() << "Raw Query list:" << this->m_netRequest->url().queryItems();
    qDebug() << "Encoded Query:" << this->m_netRequest->url().encodedQuery();
    qDebug() << "Ressource:" << this->m_netRequest->url().path();
    qDebug() << "----- /Request Data -----";
    qDebug() << "- Qt Network Error:" << this->m_netReply->error()
             << " - " << this->m_netReply->errorString();
    qDebug() << "- HTTP Error code:" << httpCode;
    if (this->m_netReply->error() != QNetworkReply::NoError && httpCode == 0)
    {
        requestError |= NetworkError;
        this->requestNetworkError();
    }

    qDebug() << "--- Reply ---";
    qDebug() << "- Headers:" << this->m_netReply->rawHeaderPairs();
    qDebug() << "- Body:" << this->m_netDataRaw;
    qDebug() << "--- /Reply ---";

    qDebug() << "##### /Request Finished #####";

    if (!this->parseResponse())
        requestError |= ParsingError;

    if (requestError == NoError)
    {
        this->m_valid = true;
        emit this->finishedSuccessfully();
    }
    else
    {
        emit this->requestError();
        emit this->finishedWithError(httpCode, this->m_netReply->error());
    }
    emit this->requestDone();
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

void	MXRequestManager::requestAuth(QNetworkReply __attribute__((unused))*reply,
                                      QAuthenticator *auth)
{
    qDebug() << "Requested HTTP AUTH !";
    auth->setUser(this->m_netAuthUser);
    auth->setPassword(this->m_netAuthPass);
}
// ---
