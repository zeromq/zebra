@ECHO OFF
SET solution=zebra.sln
SET version=11
SET log=build.log
SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat
SET environment="%programfiles(x86)%\%tools%"
IF NOT EXIST %environment% SET environment="%programfiles%\%tools%"
IF NOT EXIST %environment% GOTO no_tools

SET packages=
IF EXIST "..\..\..\..\libzmq\builds/msvc/vs2012\libzmq.import.props" (
    COPY /Y "..\..\..\..\libzmq\builds/msvc/vs2012\libzmq.import.props" . > %log%
    IF errorlevel 1 GOTO error
) ELSE (
    ECHO Did not find libzmq, aborting.
    ECHO Please clone from https://github.com/zeromq/libzmq, and then build.
    GOTO error
)
IF EXIST "..\..\..\..\uuid\builds/msvc/vs2012\uuid.import.props" (
    COPY /Y "..\..\..\..\uuid\builds/msvc/vs2012\uuid.import.props" . > %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_UUID=1
    ECHO Building with uuid
) ELSE (
    ECHO Building without uuid
)
IF EXIST "..\..\..\..\czmq\builds/msvc/vs2012\czmq.import.props" (
    COPY /Y "..\..\..\..\czmq\builds/msvc/vs2012\czmq.import.props" . > %log%
    IF errorlevel 1 GOTO error
) ELSE (
    ECHO Did not find czmq, aborting.
    ECHO Please clone from https://github.com/zeromq/czmq, and then build.
    GOTO error
)
IF EXIST "..\..\..\..\libmicrohttpd\builds/msvc/vs2012\libmicrohttpd.import.props" (
    COPY /Y "..\..\..\..\libmicrohttpd\builds/msvc/vs2012\libmicrohttpd.import.props" . > %log%
    IF errorlevel 1 GOTO error
) ELSE (
    ECHO Did not find libmicrohttpd, aborting.
    ECHO Please clone from http://ftp.gnu.org/gnu/libmicrohttpd/, and then build.
    GOTO error
)
IF EXIST "..\..\..\..\libcurl\builds/msvc/vs2012\libcurl.import.props" (
    COPY /Y "..\..\..\..\libcurl\builds/msvc/vs2012\libcurl.import.props" . > %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_LIBCURL=1
    ECHO Building with libcurl
) ELSE (
    ECHO Building without libcurl
)
IF EXIST "..\..\..\..\libsodium\builds/msvc/vs2012\libsodium.import.props" (
    COPY /Y "..\..\..\..\libsodium\builds/msvc/vs2012\libsodium.import.props" . > %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_LIBSODIUM=1
    ECHO Building with libsodium
) ELSE (
    ECHO Building without libsodium
)

ECHO Building zebra... (%packages%)

CALL %environment% x86 > nul
ECHO Platform=x86

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=Win32 %packages% %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error

CALL %environment% x86_amd64 > nul
ECHO Platform=x64

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=x64 %packages% %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error

ECHO Complete: %packages% %solution%
GOTO end

:error
ECHO *** ERROR, build terminated early: see %log%
GOTO end

:no_tools
ECHO *** ERROR, build tools not found: %tools%

:end
