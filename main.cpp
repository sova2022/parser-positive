#include <QtCore/QCoreApplication>
#include <iostream>

#include "positive.h"
#include "signal.h"


void PrintHelp();
bool ParseArguments(int argc, char* argv[], Settings& settings);
QCoreApplication* app_ptr = nullptr;
void SignalHandler(int signal);


int main(int argc, char* argv[]) {

    QWebEngineUrlScheme scheme("myscheme");
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::HostAndPort);
    scheme.setDefaultPort(24);
    scheme.setFlags(QWebEngineUrlScheme::CorsEnabled);
    QWebEngineUrlScheme::registerScheme(scheme);

    QApplication app(argc, argv);
    app_ptr = &app;
    Settings settings;
    
    if (!ParseArguments(argc, argv, settings)) {
        return 1;
    }
    /*
    settings.login = "sova2022@mail.ru";
    settings.password = "Beeline1@";
    settings.hidden = false;
    settings.server_url = "http://127.0.0.1:23";
    */

    Positive positive;
    positive.SetUrl(QUrl("https://positivebet.com/en/bets/index"));
    positive.SetSettings(settings);
    positive.Start();
    if (settings.hidden) {
        std::cout << "Running in hidden mode" << std::endl;
    }
    else {
        positive.Show();
    }

    signal(SIGINT, SignalHandler);
    return app.exec();
}

void PrintHelp() {
    std::cout << "Usage: parser [OPTIONS]\n"
        << "Options:\n"
        << "  --help,         -h               Show this help message\n"
        << "  --login,        -l <login>       Specify the login username\n"
        << "  --password,     -p <password>    Specify the login password\n"
        << "  --hidden,       -H               Run in hidden mode\n"
        << "  --server,       -S               Specify the address server\n"
        << "  --resolution,   -r <1920@1080>   Specify the resolution browser page\n"
        << "  --output_forks, -f               Output the received forks to the console\n";
}


void SplitResolution(const std::string& resolution, Settings& settings) {
    size_t pos = resolution.find('@');
    if (pos != std::string::npos) {
        settings.width = std::stoi(resolution.substr(0, pos));
        settings.height = std::stoi(resolution.substr(pos + 1));
    }
}

bool ParseArguments(int argc, char* argv[], Settings& settings) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            PrintHelp();
            return false;
        }
        else if (strcmp(argv[i], "--login") == 0 || strcmp(argv[i], "-l") == 0) {
            if (i + 1 < argc) {
                settings.login = argv[++i];
            }
            else {
                std::cerr << "Error: --login requires a value" << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "--password") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                settings.password = argv[++i];
            }
            else {
                std::cerr << "Error: --password requires a value" << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-S") == 0) {
            if (i + 1 < argc) {
                settings.server_url = argv[++i];
            }
            else {
                std::cerr << "Error: --server requires a value" << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "--hidden") == 0 || strcmp(argv[i], "-H") == 0) {
            settings.hidden = true;
        }

        else if (strcmp(argv[i], "--resolution") == 0 || strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                SplitResolution(argv[++i], settings);
            }
        }
        else if (strcmp(argv[i], "--output_forks") == 0 || strcmp(argv[i], "-f") == 0) {
                    settings.out = false;                                  
        }
    }

    if (settings.login.empty() || settings.password.empty()) {
        std::cerr << "Error: --login and --password are required" << std::endl;
        return false;
    }

    return true;
}

void SignalHandler(int signal) {
    if (app_ptr) {
        std::cout << "Terminating the application..." << std::endl;
        app_ptr->quit();
    }
}

