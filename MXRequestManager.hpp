/**
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

#ifndef		MXREQUESTMANAGER_HPP
# define	MXREQUESTMANAGER_HPP

# include	<QByteArray>
# include	<QDebug>
# include	<QIODevice>
# include	<QJsonDocument>
# include	<QJsonParseError>
# include	<QList>
# include	<QMessageBox>
# include	<QPair>
# include	<QString>
// QtNetwork
# include	<QtNetwork/QAuthenticator>
# include	<QtNetwork/QHostInfo>
# include	<QtNetwork/QHttpMultiPart>
# include	<QtNetwork/QNetworkAccessManager>
# include   <QtNetwork/QNetworkProxy> // Hack about infinite keychain access
# include	<QtNetwork/QNetworkReply>
# include	<QtNetwork/QNetworkRequest>
// ---
# include	<QUrl>
# include	<QUrlQuery>
# include	<QVariantMap>

# define	MXREQUESTMANAGER_NAME		"MXRequestManager"
# define	MXREQUESTMANAGER_VERSION	"1.3"

# if		defined(__MACH__)
#	define	MXREQUESTMANAGER_PLATEFORM	"MacOS"
# elif		defined(WIN32)
#	define	MXREQUESTMANAGER_PLATEFORM	"Win32"
# elif		defined(UNIX)
#	define	MXREQUESTMANAGER_PLATEFORM	"Unix"
# else
#	define	MXREQUESTMANAGER_PLATEFORM	"Unknown"
# endif

/**
 * @class	MXRequestManager
 * @brief	Handles the requests
 * @extends	QNetworkAccessManager
 */

class MXRequestManager : public QNetworkAccessManager
{
    Q_OBJECT

    public:
        /**
        * @typedef
        */
        typedef	QMap<QString, QString>					MXMap;
        typedef	QMap<QByteArray, QByteArray>			MXEncodedMap;

        typedef	QMapIterator<QString, QString>			MXMapIterator;
        typedef	QMapIterator<QByteArray, QByteArray>	MXEncodedMapIterator;

        typedef	QPair<QString, QString>					MXPair;
        typedef	QPair<QByteArray, QByteArray>			MXEncodedPair;

        typedef	QList<MXPair>							MXPairList;
        typedef	QList<MXEncodedPair>					MXEncodedPairList;

        /**
        * @enum
        */
        enum SupportedContentTypes
        {
            JSON = 0 // Default
        };

    private:
        int                     m_httpAuthCount;
        int                     m_lastHttpCode;
        SupportedContentTypes	m_responseType;
        QByteArray				m_netDataRaw;
        QNetworkProxy           m_netProxy; // Hack: Keychain access
        QNetworkReply			*m_netReply;
        QNetworkRequest			*m_netRequest;
        QString					m_netAuthUser;
        QString					m_netAuthPass;
        QUrl					m_netBaseApiUrl;
        QVariantMap				m_netDataMap;

    public:
        // Contructors //
        /**
         * Constructs the Request Manager with empty HTTP Auth credentials,
         * empty request and default User-Agent.
         */
        MXRequestManager(QObject *parent = 0);

        /**
         * Constructs the Request Manager with a given API URL,
         * HTTP Auth User/Pass (if given).
         *
         * @param[in]	apiUrl		Base API URL, without the resource.
         * @param[in]	authUser	HTTP Auth User (Optional)
         * @param[in]	authPass	HTTP Auth Password (Optional)
         */
        MXRequestManager(QUrl apiUrl,
                         QString authUser = QString(),
                         QString authPass = QString(), QObject *parent = 0);

        /**
         * Constructs the Request Manager from another one
         * (will try to copy the values, not pointers)
         *
         * @param[in]	other	An other MXRequestManager instance
         */
        MXRequestManager(MXRequestManager const& other);

        /**
         * Destructs the internal attributes.
         */
        ~MXRequestManager();
        // --- //

        // Copy constructor //
        /**
         * Assigns a constructed Request Manager from another one
         * (will copy the pointers).
         *
         * @param[in]	other	An other MXRequestManager instance
         */
        MXRequestManager&	operator=(MXRequestManager const& other);
        // --- //

        // Static methods
        /**
         * Check the network accessibility. Usefull to check if the APIs
         * are accessible. From QUrl (With HTTP).
         *
         * @param[in]   QUrl	URL of the API (http[s]?://URL/)
         * @return		Bool	State of the accessibility
         */
        // static bool			isAccessible(QUrl const& apiUrl);

        /**
         * Check the network accessibility. Usefull to check if the APIs
         * are accessible. From String (Hostname).
         *
         * @param[in]   QUrl	URL of the API (api.example.com)
         * @return		Bool	State of the accessibility
         */
        // static bool			isAccessible(QString const& apiHost);
        // --- //

        /**
         * Get internal HTTP Auth User
         *
         * @param[in]	void
         * @return		QString	HTTP Auth User
         */
        QString			authUser(void) const;

        /**
         * Get internal HTTP Auth Password
         *
         * @param[in]	void
         * @return		QString	HTTP Auth Password
         */
        QString			authPass(void) const;

        /**
         * Get internal base API URL without resource
         *
         * @param[in]	void
         * @return		QUrl	Base API URL
         */
        QUrl			apiUrl(void) const;

        /**
         * Get internal User-Agent
         *
         * @param[in]	void
         * @return		QString	User-Agent
         */
        QString			userAgent(void) const;

        /**
         * Get last HTTP status code
         *
         * @param       void
         * @return      int Last HTTP status code
         */
        int     lastHttpCode(void) const;

        /**
         * Get internal QNetworkRequest
         *
         * @param       void
         * @return      QNetworkRequest Constant reference to the internal QNetworkRequest
         */
        QNetworkRequest const&  networkRequest(void);

        /**
         * Get internal QNetworkReply
         *
         * @param       void
         * @return      QNetworkReply Constant reference to the internal QNetworkReply
         */
        QNetworkReply const&  networkReply(void);

        /**
         * Get internal received data
         *
         * @param		void
         * @return		QByteArray	Constant reference to the received data
         */
        QByteArray	const&	rawData(void) const;

        /**
         * Get internal parsed received data
         *
         * @param	void
         * @return	QVariantMap	Constant reference to a parsed version of received data
         */
        QVariantMap	const&	data(void) const;

        /**
         * Set internal HTTP Auth User
         *
         * @param[in]	QString	HTTP Auth User
         * @return		void
         */
        void			setAuthUser(QString const& authUser);

        /**
         * Set internal HTTP Auth Password
         *
         * @param[in]	QString	HTTP Auth Password
         * @return		void
         */
        void			setAuthPass(QString const& authPass);

        /**
         * Set internal base API URL
         *
         * @param[in]	QUrl	Base API URL
         * @return		void
         */
        void			setApiUrl(QUrl const& apiUrl);

        /**
         * Set internal User-Agent
         *
         * @param[in]	QString	User-Agent (Empty for default)
         * @return		void
         */
        void			setUserAgent(QString const& userAgent = QString());

        /**
         * Set the accepted content type.
         * It means if the Content-Type of the replies isn't the same,
         * the manager will consider a server-side script error.
         */
        void			setResponseType(SupportedContentTypes const& responseType);
        // --- //

        // Requests with MX TypeDefs
        /**
         * Default request.
         * Prepares and processes a well formed/encoded request from given MXMap
         * Then calls the overloaded this->request taking an MXEncodedPairList
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Unencoded parameters as MXMap
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        MXMap const& data = MXMap());

        /**
         * @overload
         * Prepares and processes a well formed/encoded request from given MXEncodedMap
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Already encoded parameters as MXEncodedMap
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        MXEncodedMap const& data);

        /**
         * @overload
         * Prepares and processes a well formed/encoded request from given MXPairList
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Already encoded parameters as MXEncodedPairList
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        MXPairList const& data);

        /**
         * @overload
         * Prepares and processes a well formed/encoded request from given MXEncodedPairList
         * Will set arbitrary Content-Length header to 0 for PUT.
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Unencoded QList<QPair<QString, QString> > as MXPairList
         *							Will be appended to resource as query.
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        MXEncodedPairList const& data);
        // ---

        // Requests with QNetworkRequest overloads
        /**
         * Process the request, with given resource, method and data.
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method.
         * @param[in]	data		Pointer to the data to send. Default is empty.
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method, QIODevice *data);

        /**
         * Process the request, with given resource, method and data.
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Reference to the data to send.
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        QByteArray const& data);

        /**
         * Process the request, with given resource, method and data.
         *
         * @param[in]	resource	Name of resource, will be appended to the API URL.
         * @param[in]	method		Name of the HTTP method. Default is GET.
         * @param[in]	data		Reference to the data to send.
         * @return		bool		Returns the status of request. FALSE == no signal.
         */
        bool	request(QString const& resource, QString const& method,
                        QHttpMultiPart *data);

        /**
         * Parse the response depending on the responseType set.
         *
         * @param[in]	reponse	QByteArray of the response body.
         * @return		bool	Status of the parsing.
         */
        bool	parseResponse(QString const& contentType, QByteArray const& response);
        // ---

    signals:
        /**
         * Emitted when a request begins
         */
        void	begin(void);

        /**
         * Emitted when a request is finished
         * finishedWithNoError tell if there was a network error
         */
        void	finished(bool finishedWithNoError);

        /**
         * Emitted when a request is finished with a request error
         */
        void	finishedWithError(void);

        /**
         * Emitted when there is an error while treating the reply
         */
        void	parsingError(void);

        /**
         * Emitted when downloadProgess signal from the QNetworkReply
         */
        void	downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        /**
         * Emitted when uploadProgess signal from the QNetworkReply
         */
        void	uploadProgress(qint64 bytesReceived, qint64 bytesTotal);

    public slots:
        /**
         * Called when there is an error with the request
         */
        void	requestError(QNetworkReply::NetworkError code);

        /**
         * Called when a request is finished.
         * Copies the received data to the internal QByteArray then emits
         * finished();
         */
        void	requestFinished(QNetworkReply *reply);

        /**
         * Called when downloadProgess signal from the QNetworkReply
         */
        void	requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        /**
         * Called when uploadProgess signal from the QNetworkReply
         */
        void	requestUploadProgress(qint64 bytesReceived, qint64 bytesTotal);

        /**
         * Called when an HTTP Auth Basic is required.
         * Will fill the QAuthenticator object with internal authUser and authPass.
         */
        void	requestAuth(QNetworkReply *reply, QAuthenticator *auth);
};

#endif // MXREQUESTMANAGER_HPP
