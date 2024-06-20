#pragma once

#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QNetWorkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlRequestInterceptor>

class RequestInterceptor : public QWebEngineUrlRequestInterceptor {
	Q_OBJECT
public:
	RequestInterceptor(QObject* parent);

	void SetRegExp(const QString& regexp);

signals:
	void requestIntercepted(const QUrl& url);

protected:
	void interceptRequest(QWebEngineUrlRequestInfo& info) override;

private:
	QRegularExpression re_;
};


class SchemeHandler : public QWebEngineUrlSchemeHandler {
    Q_OBJECT
public:
    explicit SchemeHandler(QObject* parent);

	void SetRequestUrl(QUrl request_url);
 
signals:
    void responseReceived(const QString& response);

protected:
    void requestStarted(QWebEngineUrlRequestJob* job) override;

private:
    QNetworkAccessManager* network_manager_;
	QUrl request_url_;
};