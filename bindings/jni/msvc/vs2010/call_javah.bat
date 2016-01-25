@ECHO OFF
ECHO Generating native JNI headers...
IF EXIST ..\..\..\src\native\include\org_zeromq_zebra_XrapMsg.h GOTO HAVE_XrapMsg
"%JAVA_HOME%\bin\javah.exe" -d ..\..\..\src\native\include -classpath ..\..\..\src\main\java org.zeromq.zebra.XrapMsg
:HAVE_XrapMsg
IF EXIST ..\..\..\src\native\include\org_zeromq_zebra_ZebHandler.h GOTO HAVE_ZebHandler
"%JAVA_HOME%\bin\javah.exe" -d ..\..\..\src\native\include -classpath ..\..\..\src\main\java org.zeromq.zebra.ZebHandler
:HAVE_ZebHandler
