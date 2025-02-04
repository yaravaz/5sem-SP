#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void PrintError(const char* message) {
    DWORD error = GetLastError();
    LPSTR errorText = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&errorText,
        0,
        NULL
    );
    cout << message << ": " << errorText << " (Error code: " << error << ")" << endl;
    LocalFree(errorText);
}

void MyCreateService(const wstring& serviceName, const wstring& binaryPath) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = CreateService(
        schSCManager,
        serviceName.c_str(),
        serviceName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        binaryPath.c_str(),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (schService == NULL) {
        PrintError("CreateService failed");
    }
    else {
        cout << "Service created successfully" << endl;
        CloseServiceHandle(schService);
    }

    CloseServiceHandle(schSCManager);
}

void MyStartService(const wstring& serviceName, const wstring& watchDir, const wstring& logDir) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!StartService(schService, 0, NULL)) {
        PrintError("StartService failed");
    }
    else {
        cout << "Service started successfully" << endl;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void StopService(const wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS ssStatus;
    ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus);
    cout << "Service stopped successfully" << endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void DeleteService(const wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), DELETE);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!DeleteService(schService)) {
        PrintError("DeleteService failed");
    }
    else {
        cout << "Service deleted successfully" << endl;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void PauseService(const wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS ssStatus;
    ControlService(schService, SERVICE_CONTROL_PAUSE, &ssStatus);
    cout << "Service paused successfully" << endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void ContinueService(const wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS ssStatus;
    ControlService(schService, SERVICE_CONTROL_CONTINUE, &ssStatus);
    cout << "Service continued successfully" << endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void InfoService(const wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        PrintError("OpenSCManager failed");
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        PrintError("OpenService failed");
        CloseServiceHandle(schSCManager);
        return;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        PrintError("QueryServiceStatusEx failed");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    cout << "Service status: " << ssp.dwCurrentState << endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 3) {
        cout << "Usage: ServiceCtl <ServiceName> <Operation> [AdditionalParameters]" << endl;
        return 1;
    }

    wstring serviceName = wstring(argv[1], argv[1] + strlen(argv[1]));
    string operation = argv[2];

    if (operation == "Create" && argc == 4) {
        wstring binaryPath = wstring(argv[3], argv[3] + strlen(argv[3]));
        MyCreateService(serviceName, binaryPath);
    }
    else if (operation == "Start" && argc == 5) {
        wstring watchDir = wstring(argv[3], argv[3] + strlen(argv[3]));
        wstring logDir = wstring(argv[4], argv[4] + strlen(argv[4]));
        MyStartService(serviceName, watchDir, logDir);
    }
    else if (operation == "Stop") {
        StopService(serviceName);
    }
    else if (operation == "Delete") {
        DeleteService(serviceName);
    }
    else if (operation == "Pause") {
        PauseService(serviceName);
    }
    else if (operation == "Continue") {
        ContinueService(serviceName);
    }
    else if (operation == "Info") {
        InfoService(serviceName);
    }
    else {
        cout << "Invalid operation or parameters" << endl;
    }

    return 0;
}
