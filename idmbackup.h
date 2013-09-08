#ifndef IDMBACKUP_H
#define IDMBACKUP_H

#include <QMainWindow>
#include <windows.h>

namespace Ui {
class IDMBackup;
}

class IDMBackup : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit IDMBackup(QWidget *parent = 0);
    ~IDMBackup();
    
private:
    Ui::IDMBackup *ui;
    BOOL SaveRegKeyPath(QString &OutFile);
    BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
    BOOL IsKeyExist(HKEY hRoot, LPCSTR KeyPath);
    BOOL IsFileExist(LPCSTR FilePath);

private slots:
    void BackupIDM();

};

#endif // IDMBACKUP_H
