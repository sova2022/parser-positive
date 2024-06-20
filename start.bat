@echo off

pushd "%~dp0" 
parser-positive.exe -l login -p password -S http://127.0.0.1:23
popd