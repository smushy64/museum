Set-PSDebug -Trace 1
.\build\debug\liquid-packager.exe create ./resources/test.manifest --enum-name ResourceID --header-output ./generated/resource.h --verbose --memory-usage
Set-PSDebug -Trace 0

