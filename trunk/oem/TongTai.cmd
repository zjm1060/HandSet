
@set path=%path%;D:\Python27

python fix_oem_info.py ..\setup\deploy_release\FlashDisk2\DSTester\info.xml .\TongTai\info.xml

copy .\TongTai\splash.bmp ..\setup\deploy_release\FlashDisk2\DSTester\bmp\

python ..\build\install_package.py ..\setup\deploy_release  ..\setup\deploy_release\flashdisk2\DSTester\info.xml

pause