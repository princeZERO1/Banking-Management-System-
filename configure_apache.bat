@echo off
echo Configuring Apache for Banking Management System...

REM Paths
set PROJECT_PATH=C:/Users/Prince/Documents/antigravity/lucid-faraday
set CONF_FILE=C:\xampp\apache\conf\httpd.conf
set BACKUP_FILE=C:\xampp\apache\conf\httpd.conf.backup

REM Backup the original httpd.conf
echo Backing up original httpd.conf...
copy "%CONF_FILE%" "%BACKUP_FILE%"

REM Use PowerShell to apply the replacements cleanly
powershell -Command ^
    "(Get-Content '%CONF_FILE%') ^
    -replace 'DocumentRoot \"C:/xampp/htdocs\"', 'DocumentRoot \"%PROJECT_PATH%/frontend\"' ^
    -replace '<Directory \"C:/xampp/htdocs\">', '<Directory \"%PROJECT_PATH%/frontend\">' ^
    -replace 'ScriptAlias /cgi-bin/ \"C:/xampp/cgi-bin/\"', 'ScriptAlias /cgi-bin/ \"%PROJECT_PATH%/cgi-bin/\"' ^
    -replace '<Directory \"C:/xampp/cgi-bin\">', '<Directory \"%PROJECT_PATH%/cgi-bin\">' ^
    | Set-Content '%CONF_FILE%'"

REM Also enable CGI module if not already enabled
powershell -Command ^
    "(Get-Content '%CONF_FILE%') ^
    -replace '#LoadModule cgi_module modules/mod_cgi.so', 'LoadModule cgi_module modules/mod_cgi.so' ^
    | Set-Content '%CONF_FILE%'"

echo Configuration Complete!
echo Now restart Apache from the XAMPP Control Panel.
pause
