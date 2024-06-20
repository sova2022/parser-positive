#include "browser.h"

// Browser

Browser::Browser(QObject* parent)
	: profile_(new QWebEngineProfile(this))
	, page_(new QWebEnginePage(profile_))
	, network_manager_(new QNetworkAccessManager(this))
	, interceptor_(new RequestInterceptor(this))
	, channel_(new QWebChannel(this)) 
	, handler_(new SchemeHandler(this)) {
	
	profile_->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
	profile_->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
	profile_->setHttpUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36");
	profile_->setUrlRequestInterceptor(interceptor_);
	profile_->installUrlSchemeHandler("myscheme", handler_);

	channel_->registerObject(QStringLiteral("interceptor"), interceptor_);
	page_->setWebChannel(channel_);
	setPage(page_);
}

Browser::~Browser() {	
	delete network_manager_;
	delete interceptor_;
	delete channel_;
	delete handler_;
	delete page_;
	delete profile_;
}

RequestInterceptor* Browser::GetInterceptor() {
	return interceptor_;
}

SchemeHandler* Browser::GetSchemeHandler() {
	return handler_;
}

QNetworkAccessManager* Browser::GetNetWorkManager() {
	return network_manager_;
}

