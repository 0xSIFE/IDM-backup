#include "idmbackup.h"
#include "ui_idmbackup.h"
#include <accctrl.h>
#include <aclapi.h>
#include <QDir>
#include <QMessageBox>

IDMBackup::IDMBackup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IDMBackup)
{
    ui->setupUi(this);

    connect(ui->backupIDMButton, SIGNAL(clicked()), this, SLOT(BackupIDM()));
}

BOOL IDMBackup::SaveRegKeyPath(QString &OutFile)
{
    BOOL  ret = TRUE;
    HKEY  hKey = NULL;
    DWORD  d;
    HKEY  hRoot;
    QString SubKey = "Software\\DownloadManager";

    //  Set SE_BACKUP_NAME privilege
    SetPrivilege(SE_BACKUP_NAME, TRUE);
    //  Determine the hive
    hRoot = HKEY_CURRENT_USER;
    //  We have to save only existing key ! (KEY_READ parameter below)
    if (RegOpenKeyEx(hRoot, SubKey.toStdWString().c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        QMessageBox::critical(0, "", "");
        /*if (IsFileExist(OutFile.toStdString().c_str())) {
            //we must delete file before saving, otherwise it doesn't work !
            if (DeleteFile(OutFile.toStdWString().c_str()))
                d = RegSaveKey(hKey, OutFile.toStdWString().c_str(), NULL);
        }
        else*/
            d = RegSaveKey(hKey, OutFile.toStdWString().c_str(), NULL);

        if(d != ERROR_SUCCESS)
            ret = FALSE;

        RegCloseKey(hKey);
    }
    else {
        if (IsKeyExist(hRoot, SubKey.toLocal8Bit().data()) == FALSE)
            d = ERROR_FILE_NOT_FOUND;
        else
            d = 0;

        ret = FALSE;
    }

    SetPrivilege(SE_BACKUP_NAME, FALSE);
    return ret;
}

BOOL IDMBackup::SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    LUID luid;
    HANDLE hToken;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    if ( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) )
        return FALSE;

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;

    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES) NULL, 0);

    return ( (GetLastError()!=ERROR_SUCCESS)?FALSE:TRUE);
}

BOOL IDMBackup::IsKeyExist(HKEY hRoot, LPCSTR KeyPath)
{
    HKEY		hKey;
    BOOL		ret;

    if(hRoot == NULL) return FALSE;
    if(KeyPath == NULL || (!lstrlenA(KeyPath)) ) return FALSE;

    ret = RegOpenKeyExA(hRoot, KeyPath, 0, KEY_EXECUTE, &hKey);
    if ( (RegOpenKeyExA(hRoot, KeyPath, 0, KEY_EXECUTE, &hKey)==ERROR_FILE_NOT_FOUND) ||
          (RegOpenKeyExA(hRoot, KeyPath, 0, KEY_EXECUTE, &hKey)!=ERROR_SUCCESS) )
          ret = FALSE;
    else
        ret = TRUE;

    RegCloseKey(hKey);
    return ret;
}

//	Test if the file FilePath exist
//	Return TRUE if it exists, otherwise return FALSE
BOOL IDMBackup::IsFileExist(LPCSTR FilePath)
{
    HANDLE			hSearch;
    WIN32_FIND_DATA FileData;

    //hSearch = FindFirstFileA(FilePath, &FileData);
    return (hSearch != INVALID_HANDLE_VALUE);
}

void IDMBackup::BackupIDM()
{
    QString fileIDM = QDir::currentPath() + "/" + "idm.reg";
    bool ss = IDMBackup::SaveRegKeyPath(fileIDM);
    if(ss)
        QMessageBox::critical(0, "", fileIDM);
}

IDMBackup::~IDMBackup()
{
    delete ui;
}
