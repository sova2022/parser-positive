#include "positive.h"

static QString Trim(const QString& input) {
    int comma = input.indexOf(',');

    if (comma != -1) {
        return input.left(comma).trimmed();
    }
    return input.trimmed();
}

// Positive

Positive::Positive()
    : browser_(new Browser(this)) {

    browser_->GetInterceptor()->SetRegExp("gridBets");
    browser_->GetSchemeHandler()->SetRequestUrl(QUrl("https://positivebet.com/en/bets/index?ajax=gridBets&au=1&crid=&fl%5B%5D=43069&isAjax=true&markNewEvent=false&perPage=20"));
    
    connect(browser_, &QWebEngineView::loadFinished, this, &Positive::OnLoadFinished);
    connect(browser_->GetSchemeHandler(), &SchemeHandler::responseReceived, this, &Positive::CheckForChanges);
}

Positive::~Positive() {
    delete browser_;
}

bool Positive::CheckOnLogged() {
    browser_->page()->toHtml([this](const QString& html) {
        if (!html.contains("Logout")) {
            qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Account has been unlogged";
            settings_.is_logged = false;
            Login();
        }
    });
    return true;
}

void Positive::GetBlocks(const QString& id, const QString& html, QJsonArray& array) {
   if (id == "0") {
        return;
   }
    QTextDocument doc;
    doc.setHtml(html.toUtf8());     
 
    QJsonObject json_obj;
    json_obj["id_fork"] = id;
    json_obj["profit"] = doc.findBlockByNumber(3).text();

    QJsonObject details_1st_booker;
    details_1st_booker["event"] = Trim(doc.findBlockByNumber(7).text());
    details_1st_booker["koef"] = doc.findBlockByNumber(11).text();
    details_1st_booker["bet"] = doc.findBlockByNumber(15).text();
    json_obj[doc.findBlockByNumber(4).text()] = details_1st_booker;

    QJsonObject details_2nd_booker;
    details_2nd_booker["event"] = Trim(doc.findBlockByNumber(8).text());
    details_2nd_booker["koef"] = doc.findBlockByNumber(12).text();
    details_2nd_booker["bet"] = doc.findBlockByNumber(16).text();
    json_obj[doc.findBlockByNumber(5).text()] = details_2nd_booker;
    array.push_back(json_obj);    
}

void Positive::Login() {
    qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Trying to login positivebet";
    disconnect(browser_, &QWebEngineView::loadFinished, this, &Positive::OnLoadFinished);
    browser_->page()->load(QUrl("https://positivebet.com/en/user/login"));
    QString login_js = QString(R"(
                (function() {
                    document.getElementById('UserLogin_username').value = '%1';
                    document.getElementById('UserLogin_password').value = '%2';
                    document.querySelector('button.btn.btn-primary[type="submit"]').click();
                })();
            )").arg(QString::fromStdString(settings_.login)).arg(QString::fromStdString(settings_.password));
    QTimer::singleShot(2000, [this, login_js]() {
        browser_->page()->runJavaScript(login_js);
        });

    QTimer::singleShot(5000, [this]() {
        connect(browser_, &QWebEngineView::loadFinished, this, &Positive::OnLoadFinished);
        browser_->page()->load(url_);
        settings_.is_logged = true;
        qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Success";
        });
}

void Positive::SendPostRequest(const QString& url, const QJsonArray& data) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "parserpositive/json");
    QJsonDocument json_doc(data);
    QByteArray json_data = json_doc.toJson();
    browser_->GetNetWorkManager()->post(request, json_data);
    QString address_server = QString::fromStdString(settings_.server_url.substr(7));
    qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "response sent to server:" << address_server.toLocal8Bit().constData();
}

void Positive::SetSettings(Settings settings) {
    settings_ = std::move(settings);
}

void Positive::SetUrl(const QUrl url) {
    url_ = std::move(url);
}

void Positive::Start() {
    qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Parser positivebet.com launched";    
    browser_->page()->load(url_);

}

void Positive::Show() {
    browser_->resize(settings_.width, settings_.height);
    browser_->show();
}

void Positive::CheckForChanges(const QString& response) {

    if (settings_.is_logged) {
        browser_->page()->toHtml([this, response](const QString& html) {
            CheckOnLogged();
            QRegularExpression re("<table class=.*?>(.*?)</table>", QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch match = re.match(html);
            if (match.hasMatch()) {
                ParseHtml(response);
            }
        });
    }
    else {
        Login();
    }
}

void Positive::OnLoadFinished() {
    if (settings_.is_logged) {
        QString js_code = R"(document.getElementById('btnAutoRefresh').click();)";
        QTimer::singleShot(1000, [this, js_code]() {
            browser_->page()->runJavaScript(js_code);

            QTimer::singleShot(1000, [this]() {
                browser_->page()->toHtml([this](const QString& html) {
                    
                    QTimer::singleShot(1000, [this, html]() {
                        
                        qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit().constData() << "Auto refresh has been pressed";
                        if (!html.contains("btn active")) {
                            OnLoadFinished();
                        }
                     }); 

                });

            });

        });
    }
    else {
        QTimer::singleShot(1000, [this]() {
            Login();
        });
    }    
}

void Positive::ParseHtml(const QString& html) {
    QRegularExpression re("<tr id=\"(.*?)\">(.*?)</tr>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);
    QJsonArray array;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            GetBlocks(match.captured(1), match.captured(2), array);
        }
    }
    
    QJsonDocument json_doc(array);
    QString json_string = json_doc.toJson(QJsonDocument::Indented);
    if (settings_.out) {
        qDebug().noquote() << json_string;
    }    
    SendPostRequest(QString::fromStdString(settings_.server_url), array);
}

