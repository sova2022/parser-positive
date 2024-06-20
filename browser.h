#pragma once

#include <QtWidgets>

#include <QJsonArray>
#include <QJsonObject>
#include <QTextBlock>
#include <QTimer>
#include <QWebChannel>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>



#include "domain.h"
#include "interceptor.h"

class Browser : public QWebEngineView {	
public:
	Browser(QObject* parent);
	~Browser();

	RequestInterceptor* GetInterceptor();
	SchemeHandler* GetSchemeHandler();
	QNetworkAccessManager* GetNetWorkManager();

private:
	QWebEngineProfile* profile_;
	QWebEnginePage* page_;
	RequestInterceptor* interceptor_;
	QNetworkAccessManager* network_manager_;
	QWebChannel* channel_;
	SchemeHandler* handler_;
};