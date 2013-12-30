#include <QEventLoop>
#include <QSignalSpy>
#include <QString>
#include <QtTest>

#include "../src/MXRequestManager.hpp"

class MXRequestManagerTest : public QObject
{
    Q_OBJECT
    private:
        QString m_baseUrl;
        QString m_xmlRessource;
        QString m_jsonRessource;

    public:
        MXRequestManagerTest();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();
        void testInternalVariables();
        void testAPIWithParseError();
        void testAPIParsingOK();
};

MXRequestManagerTest::MXRequestManagerTest()
{
}

void MXRequestManagerTest::initTestCase()
{
    this->m_baseUrl = "http://api.virtual-info.info";
    this->m_xmlRessource = "/self.xml";
    this->m_jsonRessource = "/self.json";
}

void MXRequestManagerTest::cleanupTestCase()
{
}

void MXRequestManagerTest::testInternalVariables()
{
    MXRequestManager    req(this);

    req.setApiUrl(QUrl(this->m_baseUrl));
    req.setAuthUser("Username");
    req.setAuthPass("Password");
    req.setUserAgent("HELLO WORLD UA");

    QCOMPARE(req.apiUrl(), QUrl(this->m_baseUrl));
    QCOMPARE(req.authUser(), QString("Username"));
    QCOMPARE(req.authPass(), QString("Password"));
    QCOMPARE(req.userAgent(), QString("HELLO WORLD UA"));
}

void MXRequestManagerTest::testAPIWithParseError()
{
    MXRequestManager    req(this->m_baseUrl);
    QEventLoop          eventLoop(this);
    QSignalSpy          finishedWithErrorSpy(&req, SIGNAL(finishedWithError()));
    QSignalSpy          parsingErrorSpy(&req, SIGNAL(parsingError()));

    QVERIFY(finishedWithErrorSpy.isValid());
    QVERIFY(parsingErrorSpy.isValid());

    connect(&req, SIGNAL(finished(bool)), &eventLoop, SLOT(quit()));

    QVERIFY(req.request(this->m_xmlRessource, "GET"));
    eventLoop.exec();
    QVERIFY(finishedWithErrorSpy.count() == 1);
    QVERIFY(parsingErrorSpy.count() == 1);
    QCOMPARE(req.lastHttpCode(), 400);
    QVERIFY(req.data().isEmpty());
}

void MXRequestManagerTest::testAPIParsingOK()
{
    MXRequestManager    req(this->m_baseUrl);
    QEventLoop          eventLoop(this);

    connect(&req, SIGNAL(finished(bool)), &eventLoop, SLOT(quit()));

    QVERIFY(req.request(this->m_jsonRessource, "GET"));
    eventLoop.exec();
    QCOMPARE(req.lastHttpCode(), 200);
    QVERIFY(!req.data().isEmpty());
    QCOMPARE(req.data().value("self").toMap().value("HEADERS").toMap().value("User-Agent").toString(),
             req.userAgent());
}

QTEST_GUILESS_MAIN(MXRequestManagerTest)

#include "tst_MXRequestManager.moc"
