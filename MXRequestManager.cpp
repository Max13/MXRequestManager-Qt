/**
 * @file		mxrequestmanager.cpp
 * @brief		MXRequestManager
 *
 * @details		REST Request Manager by Max13
 *
 * @version		0.1
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

MXRequestManager::MXRequestManager(QObject *parent) : QNetworkAccessManager(parent)
{
	this->m_netReply = NULL;
	this->m_netRequest = new QNetworkRequest;
	this->setUserAgent();

	connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
	connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(QUrl apiUrl, QString authUser,
								   QString authPass, QObject *parent)
	: QNetworkAccessManager(parent)
{
	this->m_netReply = NULL;
	this->m_netBaseApiUrl = apiUrl;
	if (!authUser.isEmpty() || !authPass.isEmpty())
	{
		this->m_netAuthUser = authUser;
		this->m_netAuthPass = authPass;
	}
	this->m_netRequest = new QNetworkRequest;
	this->setUserAgent();

	connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
	connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
}

MXRequestManager::MXRequestManager(MXRequestManager const& other)
	: QNetworkAccessManager(other.parent())
{
	this->m_netDataRaw = other.m_netDataRaw;
	this->m_netReply = other.m_netReply;
	this->m_netRequest = new QNetworkRequest(*(other.m_netRequest));
	this->m_netAuthUser = other.m_netAuthUser;
	this->m_netAuthPass = other.m_netAuthPass;
	this->m_netBaseApiUrl = other.m_netBaseApiUrl;

	connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
	connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(requestAuth(QNetworkReply*,QAuthenticator*)));
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

QByteArray	const&	MXRequestManager::rawData(void) const
{
	return (this->m_netDataRaw);
}

QVariantMap	const&	MXRequestManager::data(void) const
{
	return (this->m_netDataMap);
}

void	MXRequestManager::setAuthUser(const QString& authUser)
{
	this->m_netAuthUser = authUser;
}

void	MXRequestManager::setAuthPass(const QString& authPass)
{
	this->m_netAuthPass = authPass;
}

void	MXRequestManager::setApiUrl(const QUrl& apiUrl)
{
	this->m_netBaseApiUrl = apiUrl;
}

void	MXRequestManager::setUserAgent(const QString& userAgent)
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
// ---

// Treatments
bool	MXRequestManager::request(QString resource, QString method, MXMap const& data)
{
	MXMapIterator	i(data);
	MXPairList		params;

	while (i.hasNext())
	{
		i.next();
		params.append(MXPair(i.key(), i.value()));
	}

	return (this->request(resource, method, params));
}

bool	MXRequestManager::request(QString resource, QString method, MXPairList const& data)
{
	QUrl apiUrl(this->m_netBaseApiUrl);
	apiUrl.setPath(resource);
	if (method.toUpper() != "POST")
		apiUrl.setQueryItems(data);
	this->m_netRequest->setUrl(apiUrl);

	emit this->begin();

	if (method.toUpper() == "DELETE")
		this->m_netReply = this->deleteResource(*(this->m_netRequest));
	else if (method.toUpper() == "GET")
		this->m_netReply = this->get(*(this->m_netRequest));
	else if (method.toUpper() == "HEAD")
		this->m_netReply = this->head(*(this->m_netRequest));
	else if (method.toUpper() == "POST")
	{
		QUrl params;
		params.setQueryItems(data);

		this->m_netRequest->setHeader(QNetworkRequest::ContentTypeHeader,
									  "application/octet-stream");
		this->m_netReply = this->post(*(this->m_netRequest), params.encodedQuery());
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
	return (TRUE);
}

bool	MXRequestManager::request(QString resource, QString method, QIODevice *data)
{
	if (resource.isEmpty() || method.isEmpty())
		return (FALSE);

	QUrl apiUrl(this->m_netBaseApiUrl);
	apiUrl.setPath(resource);
	this->m_netRequest->setUrl(apiUrl);

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
	return (TRUE);
}

bool	MXRequestManager::request(QString resource, QString method, QByteArray const& data)
{
	if (resource.isEmpty() || method.isEmpty())
		return (FALSE);

	QUrl apiUrl(this->m_netBaseApiUrl);
	apiUrl.setPath(resource);
	this->m_netRequest->setUrl(apiUrl);

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
	return (TRUE);
}


bool	MXRequestManager::request(QString resource, QString method, QHttpMultiPart *data)
{
	if (resource.isEmpty() || method.isEmpty())
		return (FALSE);

	QUrl apiUrl(this->m_netBaseApiUrl);
	apiUrl.setPath(resource);
	this->m_netRequest->setUrl(apiUrl);

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
	return (TRUE);
}
// ---

// Signals / Slots
void	MXRequestManager::requestError(QNetworkReply::NetworkError code)
{
	if (code != QNetworkReply::NoError)
		qDebug() << "Error " << code << ": " << this->m_netReply->errorString();
	else
		qDebug() << "Error Emitted: No Error...";
}

void	MXRequestManager::requestFinished(QNetworkReply *reply)
{
	bool ok;

	qDebug() << "##### Request Finished #####";
	qDebug() << "----- Request Data -----";
	qDebug() << "Known headers: " << this->m_netRequest->rawHeaderList();
	qDebug() << "Raw Query list: " << this->m_netRequest->url().queryItems();
	qDebug() << "Encoded Query: " << this->m_netRequest->url().encodedQuery();
	qDebug() << "Ressource: " << this->m_netRequest->url().path();
	qDebug() << "Method: " << this->m_netRequest->attribute(QNetworkRequest::CustomVerbAttribute,
															QVariant("Unknown")).toString();
	qDebug() << "----- /Request Data -----";
	qDebug() << "- HTTP Error code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	qDebug() << "- Qt Network Error: " << reply->error() << " - " << reply->errorString();

	this->m_netDataRaw = reply->readAll();
	qDebug() << "--- Reply ---";
	qDebug() << this->m_netDataRaw;
	qDebug() << "--- /Reply ---";

	this->m_netDataMap = QtJson::Json::parse(this->m_netDataRaw, ok).toMap();
	if (!ok)
		qDebug() << "Error while parsing...";
	else if (this->m_netDataMap.contains("errors"))
		qDebug() << "Error on request: "
				 << this->m_netDataMap.value("errors").toList().at(0).toMap().value("message").toString();

	qDebug() << "##### /Request Finished #####";
	if (!ok)
		emit this->parsingError();
	emit this->finished();
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
	auth->setUser(this->m_netAuthUser);
	auth->setPassword(this->m_netAuthPass);
}
// ---
