#pragma comment(lib, "Shlwapi.lib")
#include <windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <Shlwapi.h>
using namespace std;

#define SERVICE_NAME L"Service09"
string LOG_DIR = "D:\\lab9\\Win\\logs";
string WATCH_DIR = "D:\\lab9\\Win\\folder";

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
ofstream srvLog;
ofstream dirLog;

void LogServiceEvent(const string& message);
void LogDirectoryEvent(const string& message);
void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD CtrlCode);
void WatchDirectory(LPCSTR directoryPath);

void LogServiceEvent(const string& message) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    string timeStamp = "[" + to_string(st.wHour) + ":" + to_string(st.wMinute) + ":" + to_string(st.wSecond) + "] ";
    srvLog << timeStamp << message << endl;
    srvLog.flush();
    cout << timeStamp << message << endl;
}

void LogDirectoryEvent(const string& message) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    string timeStamp = "[" + to_string(st.wHour) + ":" + to_string(st.wMinute) + ":" + to_string(st.wSecond) + "] ";
    dirLog << timeStamp << message << endl;
    dirLog.flush();
    cout << timeStamp << message << endl;
}

void WINAPI ServiceMain(DWORD argc, LPWSTR* argv) {
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0) {
        return;
    }

    string srvLogPath = LOG_DIR + "\\" + to_string(time(nullptr)) + "-srv.log";
    string dirLogPath = LOG_DIR + "\\" + to_string(time(nullptr)) + "-dir.log";

    srvLog.open(srvLogPath, ios::out);
    dirLog.open(dirLogPath, ios::out);

    if (!srvLog.is_open()) {
        LogServiceEvent("Ошибка открытия файла для логирования (srv).");
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    }

    if (!dirLog.is_open()) {
        LogServiceEvent("Ошибка открытия файла для логирования (dir).");
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    }
    else {
        LogServiceEvent("Успех! Service09 обнаружил каталог " + LOG_DIR + " для логирования");
    }

    LogServiceEvent("Попытка запустить Service09 с параметрами: " + LOG_DIR + ", " + WATCH_DIR);

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    WatchDirectory(WATCH_DIR.c_str());
}

void WINAPI ServiceCtrlHandler(DWORD CtrlCode) {
    switch (CtrlCode) {
    case SERVICE_CONTROL_STOP:
        LogServiceEvent("Успех! Сервис Service09 изменил своё состояние с 'работает' на 'остановлен'");
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    case SERVICE_CONTROL_PAUSE:
        LogServiceEvent("Успех! Сервис Service09 изменил своё состояние с 'работает' to 'приостановлен'");
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    case SERVICE_CONTROL_CONTINUE:
        LogServiceEvent("Успех! Сервис Service09 изменил своё состояние с 'приостановлен' to 'работает'");
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    default:
        break;
    }
}

void WatchDirectory(LPCSTR directoryPath) {
    HANDLE hDir = CreateFileA(
        directoryPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        LogServiceEvent("Попытка запуска провалена! Service09 не смог обнаружить каталог " + string(directoryPath));
        return;
    }

    const int bufferSize = 4096;
    char buffer[bufferSize];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION* notifyInfo;

    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        LogServiceEvent("Ожидаем изменения в каталоге...");
        if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION, &bytesReturned, NULL, NULL)) {

            notifyInfo = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                wchar_t fileName[MAX_PATH];
                wcsncpy_s(fileName, notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
                fileName[notifyInfo->FileNameLength / sizeof(WCHAR)] = L'\0';

                char fileNameChar[MAX_PATH];
                WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameChar, MAX_PATH, NULL, NULL);

                switch (notifyInfo->Action) {
                case FILE_ACTION_ADDED:
                    LogDirectoryEvent(string("Добавлен: ") + fileNameChar);
                    break;
                case FILE_ACTION_REMOVED:
                    LogDirectoryEvent(string("Удалён: ") + fileNameChar);
                    break;
                case FILE_ACTION_MODIFIED:
                    LogDirectoryEvent(string("Изменён: ") + fileNameChar);
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    LogDirectoryEvent(string("Старое имя: ") + fileNameChar);
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    LogDirectoryEvent(string("Новое имя: ") + fileNameChar);
                    break;
                default:
                    LogDirectoryEvent(string("Неизвестное событие: ") + fileNameChar);
                    break;
                }

                notifyInfo = (FILE_NOTIFY_INFORMATION*)((char*)notifyInfo + notifyInfo->NextEntryOffset);
            } while (notifyInfo->NextEntryOffset != 0);
        }
    }

    CloseHandle(hDir);
}

int wmain(int argc, wchar_t* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    if (argc > 1 && wcslen(argv[1]) > 0) {
        wstring ws(argv[1]);
        WATCH_DIR = string(ws.begin(), ws.end());
        cout << "WATCH_DIR: " << WATCH_DIR << endl;
    }

    if (!PathFileExistsA(LOG_DIR.c_str())) {
        if (CreateDirectoryA(LOG_DIR.c_str(), NULL)) {
            LogServiceEvent("Успех! Service09 создал каталог " + LOG_DIR);
        }
        else {
            LogServiceEvent("Попытка запуска провалена! Service09 не смог создать каталог " + LOG_DIR);
            return 1;
        }
    }
    else {
        LogServiceEvent("Успех! Service09 обнаружил каталог  " + LOG_DIR);
    }

    if (!PathFileExistsA(WATCH_DIR.c_str())) {
        LogServiceEvent("Попытка запуска провалена! Service09 не смог обнаружить каталог " + WATCH_DIR);
        return 1;
    }

    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {const_cast<LPWSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(ServiceTable)) {
        return 1;
    }

    return 0;
}
