@echo off
REM Batch script to test both FineGrained and CoarseGrained modes

echo ============================================
echo    BANK ACCOUNT SIMULATION - TEST SUITE
echo ============================================
echo.

REM Configuration
set THREADS=8
set OPS=100000
set ACCOUNTS=20

echo Testing with %THREADS% threads, %OPS% operations/thread, %ACCOUNTS% accounts
echo.

echo ============================================
echo TEST 1: FineGrained Mode
echo ============================================
"Bank Account PPD.exe" %THREADS% %OPS% %ACCOUNTS%
echo.
echo Press any key to continue to CoarseGrained test...
pause > nul
echo.

echo ============================================
echo TEST 2: CoarseGrained Mode
echo ============================================
"Bank Account PPD.exe" %THREADS% %OPS% %ACCOUNTS% coarse
echo.
echo.

echo ============================================
echo All tests completed!
echo ============================================
pause

