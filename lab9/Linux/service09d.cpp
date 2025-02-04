#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <csignal>
#include <ctime>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <cstring> // добавляем этот заголовочный файл

namespace fs = std::filesystem;

class ServiceDaemon {
public:
    ServiceDaemon(const std::string& configFilePath);
    void run();
    void handleSignal(int signal);
    static ServiceDaemon& instance() {
        static ServiceDaemon instance("/etc/service09d.conf");
        return instance;
    }

private:
    std::string logDirPath;
    std::string watchDirPath;
    std::ofstream srvLogFile;
    std::ofstream dirLogFile;
    bool running;
    std::string configFilePath;
    void loadConfig();
    void createLogFiles();
    void watchDirectory();
    void logSrvMessage(const std::string& message);
    void logDirMessage(const std::string& message);
    std::string getCurrentTime();
};

ServiceDaemon::ServiceDaemon(const std::string& configFilePath) : configFilePath(configFilePath) {
    loadConfig();
    if (!fs::exists(logDirPath)) {
        fs::create_directories(logDirPath);
        logSrvMessage("Успех! Демон Service09d создал каталог " + logDirPath);
    } else {
        logSrvMessage("Успех! Демон Service09d обнаружил каталог " + logDirPath);
    }

    if (!fs::exists(watchDirPath)) {
        logSrvMessage("Попытка запуска провалена! Демон Service09d не смог обнаружить каталог " + watchDirPath);
        exit(EXIT_FAILURE);
    }
}

void ServiceDaemon::run() {
    running = true;
    std::signal(SIGTERM, [](int sig){ instance().handleSignal(sig); });
    std::signal(SIGHUP, [](int sig){ instance().handleSignal(sig); });
    createLogFiles();
    logSrvMessage("Успех! Демон Service09d запущен с параметрами");

    while (running) {
        watchDirectory();
        sleep(1);
    }

    logSrvMessage("Демон Service09d завершил свою работу.");
}

void ServiceDaemon::handleSignal(int signal) {
    if (signal == SIGTERM) {
        logSrvMessage("Демон Service09d завершает свою работу.");
        running = false;
    } else if (signal == SIGHUP) {
        logSrvMessage("Демон Service09d перезагружается.");
        loadConfig();
        createLogFiles();
        logSrvMessage("Демон Service09d теперь работает с новыми параметрами.");
    }
}

void ServiceDaemon::loadConfig() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        logSrvMessage("Попытка выполнения операции сервисом провалена! Ошибка открытия файла конфигурации: " + std::string(strerror(errno)));
        exit(EXIT_FAILURE);
    }

    std::getline(configFile, logDirPath);
    std::getline(configFile, watchDirPath);
    configFile.close();

    if (logDirPath.empty()) {
        logDirPath = "/var/log/service09d";
    }
}

void ServiceDaemon::createLogFiles() {
    std::string dateTime = getCurrentTime();
    std::string srvLogFileName = logDirPath + "/" + dateTime + "-srv.log";
    std::string dirLogFileName = logDirPath + "/" + dateTime + "-dir.log";

    srvLogFile.open(srvLogFileName, std::ios::app);
    dirLogFile.open(dirLogFileName, std::ios::app);
}

void ServiceDaemon::watchDirectory() {
    int fd = inotify_init();
    if (fd == -1) {
        logSrvMessage("Попытка выполнения операции сервисом провалена! Ошибка инициализации inotify: " + std::string(strerror(errno)));
        return;
    }

    int wd = inotify_add_watch(fd, watchDirPath.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
    if (wd == -1) {
        logSrvMessage("Попытка выполнения операции сервисом провалена! Ошибка добавления каталога для наблюдения: " + std::string(strerror(errno)));
        close(fd);
        return;
    }

    char buffer[1024];
    ssize_t numRead;
    while ((numRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (char* p = buffer; p < buffer + numRead; ) {
            struct inotify_event* event = (struct inotify_event*) p;
            std::string eventDescription = (event->mask & IN_ISDIR) ? "Каталог: " : "Файл: ";

            if (event->mask & IN_CREATE)
                eventDescription += "Создано: ";
            else if (event->mask & IN_DELETE)
                eventDescription += "Удалено: ";
            else if (event->mask & IN_MODIFY)
                eventDescription += "Изменено: ";
            else if (event->mask & IN_MOVED_FROM)
                eventDescription += "Старое имя: ";
            else if (event->mask & IN_MOVED_TO)
                eventDescription += "Новое имя: ";

            eventDescription += event->name;
            logDirMessage(eventDescription);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

void ServiceDaemon::logSrvMessage(const std::string& message) {
    srvLogFile << getCurrentTime() << " " << message << std::endl;
}

void ServiceDaemon::logDirMessage(const std::string& message) {
    dirLogFile << getCurrentTime() << " " << message << std::endl;
}

std::string ServiceDaemon::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", t);
    return std::string(buf);
}

int main() {
    ServiceDaemon::instance().run();
    return 0;
}
