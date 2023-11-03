cd /d "C:\Users\ljg\Desktop\locust\server" &msbuild "server.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform="x64" /p:SolutionDir="C:\Users\ljg\Desktop\locust\locust" 
exit %errorlevel% 