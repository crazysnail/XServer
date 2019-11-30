set tempdir=mt_%date:~0,4%_%date:~5,2%_%date:~8,2%
set db_script_dir=%tempdir%\db_script
set db_config_dir=%tempdir%\db_config
set lua_dir=%tempdir%\lua_script
set exe_dir=%tempdir%\windows\Debug
set log_dir=%tempdir%\windows\Debug\log
mkdir %tempdir%
mkdir %db_script_dir%
mkdir %db_config_dir%
mkdir %lua_dir%
mkdir %exe_dir%
mkdir %log_dir%
del server.zip

@rem system dependency
copy C:\Windows\SysWOW64\api-ms-win-core-file-l1-2-0.dll %exe_dir%
copy C:\Windows\SysWOW64\api-ms-win-core-file-l2-1-0.dll %exe_dir%
copy C:\Windows\SysWOW64\api-ms-win-core-processthreads-l1-1-1.dll %exe_dir%
copy C:\Windows\SysWOW64\api-ms-win-core-localization-l1-2-0.dll %exe_dir%
copy C:\Windows\SysWOW64\api-ms-win-core-synch-l1-2-0.dll %exe_dir%
copy C:\Windows\SysWOW64\api-ms-win-core-timezone-l1-1-0.dll %exe_dir%
copy C:\Windows\SysWOW64\msvcp140d.dll %exe_dir%
copy C:\Windows\SysWOW64\vcruntime140d.dll %exe_dir%
copy C:\Windows\SysWOW64\ucrtbased.dll %exe_dir%

@rem mysql dependency
copy bin\windows\Debug\libmysql.dll %exe_dir%
copy bin\windows\Debug\mysqlcppconn.dll %exe_dir%
copy bin\windows\Debug\libcurl.dll %exe_dir%
copy bin\windows\Debug\libeay32.dll %exe_dir%
copy bin\windows\Debug\ssleay32.dll %exe_dir%

@rem exe
copy bin\windows\Debug\GameServer.exe %exe_dir%

@rem server config dependency
copy bin\windows\Debug\server_config.conf %exe_dir%

xcopy /E bin\db_config %db_config_dir%
xcopy /E bin\db_script %db_script_dir%
xcopy /E bin\lua_script %lua_dir%


"c:\Program Files\WinRAR\WinRAR.exe" a server.zip %tempdir%
rmdir /Q /S %tempdir%
echo "got server.zip."

del e:\server.zip
copy server.zip e:\
del server.zip
pause