cd build
ctest -j2 -C "Debug" -V -T Test
exit /b %ERRORLEVEL%
