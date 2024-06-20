#pragma once
#include <QTextDocument>

#include "browser.h"

class Positive : public QObject {
    Q_OBJECT
public:
    Positive();

    ~Positive();

    bool CheckOnLogged();
    void GetBlocks(const QString& id, const QString& html, QJsonArray& json);
    void Login();
    void ParseHtml(const QString& html);
    void SendPostRequest(const QString& url, const QJsonArray& data);
    void SetSettings(Settings settings);
    void SetUrl(QUrl url);
    void Show();
    void Start(); 

private slots: 
    void CheckForChanges(const QString& html);
    void OnLoadFinished();      
       
private:
    Browser* browser_;  
    Settings settings_;   
    QUrl url_;
};

    