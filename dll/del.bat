del tfw10fcu.ncb
attrib -h tfw10fcu.suo
del tfw10fcu.suo
del /q tfw10fcu\tfw10fcu.vcproj.*.user
del /q Debug\tfw10fcu*.*
del /q tfw10fcu\Debug\*
rmdir tfw10fcu\Debug
del /q Release\*
rmdir Release
del /q tfw10fcu\Release\*
rmdir tfw10fcu\Release
del /q /s x64\Release\*
rmdir x64\Release
del /q /s x64\Debug\*
rmdir x64\Debug
rmdir x64
del /q /s tfw10fcu\x64\*
rmdir tfw10fcu\x64\Release
rmdir tfw10fcu\x64\Debug
rmdir tfw10fcu\x64
del /q /s *.sdf
rmdir /q /s ipch
