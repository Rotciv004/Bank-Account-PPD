@echo off
REM Comprehensive comparison script for FineGrained vs CoarseGrained modes

echo ============================================
echo    MODE COMPARISON TEST SUITE
echo ============================================
echo.

echo This script will run multiple tests to compare FineGrained vs CoarseGrained locking
echo.
pause
echo.

REM Test 1: Small scale
echo ============================================
echo TEST 1: Small Scale (4 threads, 50000 ops, 10 accounts)
echo ============================================
echo.
echo --- FineGrained ---
"Bank Account PPD.exe" 4 50000 10
echo.
echo.
echo --- CoarseGrained ---
"Bank Account PPD.exe" 4 50000 10 coarse
echo.
pause
echo.

REM Test 2: Medium scale
echo ============================================
echo TEST 2: Medium Scale (8 threads, 100000 ops, 20 accounts)
echo ============================================
echo.
echo --- FineGrained ---
"Bank Account PPD.exe" 8 100000 20
echo.
echo.
echo --- CoarseGrained ---
"Bank Account PPD.exe" 8 100000 20 coarse
echo.
pause
echo.

REM Test 3: Large scale
echo ============================================
echo TEST 3: Large Scale (12 threads, 200000 ops, 30 accounts)
echo ============================================
echo.
echo --- FineGrained ---
"Bank Account PPD.exe" 12 200000 30
echo.
echo.
echo --- CoarseGrained ---
"Bank Account PPD.exe" 12 200000 30 coarse
echo.
echo.

echo ============================================
echo All comparison tests completed!
echo ============================================
echo.
echo Compare the execution times and throughput to see which mode performs better
echo for different workloads.
echo.
pause

