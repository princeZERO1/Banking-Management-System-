@echo off
echo Compiling C Backend to CGI...

cd backend

gcc register.c -o ../cgi-bin/register.cgi
gcc login.c -o ../cgi-bin/login.cgi
gcc dashboard.c -o ../cgi-bin/dashboard.cgi
gcc deposit.c -o ../cgi-bin/deposit.cgi
gcc withdraw.c -o ../cgi-bin/withdraw.cgi
gcc transfer.c -o ../cgi-bin/transfer.cgi
gcc balance.c -o ../cgi-bin/balance.cgi
gcc history.c -o ../cgi-bin/history.cgi
gcc profile.c -o ../cgi-bin/profile.cgi
gcc admin.c -o ../cgi-bin/admin.cgi

echo Compilation Complete! Look in the cgi-bin directory.
cd ..
pause
