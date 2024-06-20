#include "interceptor.h"

// RequestInterceptor

RequestInterceptor::RequestInterceptor(QObject* parent)
    : QWebEngineUrlRequestInterceptor(parent) {
}

void RequestInterceptor::SetRegExp(const QString& regexp) {
    re_.setPattern(regexp);
    re_.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
}

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info) {
    QString request = info.requestUrl().toString();

    QRegularExpressionMatch match = re_.match(request);
    if (match.hasMatch()) {
        info.redirect(QUrl("myscheme://localhost:24"));
    }
}

// SchemeHandler

SchemeHandler::SchemeHandler(QObject* parent) 
	: QWebEngineUrlSchemeHandler(parent)
    , network_manager_(new QNetworkAccessManager(this)) {
}

void SchemeHandler::SetRequestUrl(QUrl request_url) {
    request_url_ = std::move(request_url);
}

void SchemeHandler::requestStarted(QWebEngineUrlRequestJob* job) {	
    QNetworkRequest request(request_url_);

    // Set headers from the intercepted request
    QMap<QByteArray, QByteArray> headers = job->requestHeaders();
    for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
        request.setRawHeader(it.key(), it.value());
    }

    // Set cookies from the intercepted request
    QList<QNetworkCookie> cookies;
    if (headers.contains("Cookie")) {
        QByteArray cookie_header = headers.value("Cookie");
        cookies = QNetworkCookie::parseCookies(cookie_header);
    }

    QNetworkCookieJar* cookie_jar = network_manager_->cookieJar();
    cookie_jar->setCookiesFromUrl(cookies, job->requestUrl());

    QNetworkReply* reply = network_manager_->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply, job]() {
        QByteArray response = reply->readAll();
        if (response.isEmpty()) {
            qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Response is empty.";
            job->fail(QWebEngineUrlRequestJob::RequestFailed);
        }
        else {
            QBuffer* buffer = new QBuffer();
            buffer->setData(response);
            buffer->open(QIODevice::ReadOnly);
            job->reply("response", buffer);
        }
        emit responseReceived(QString::fromUtf8(response));

        reply->deleteLater();
    });
}