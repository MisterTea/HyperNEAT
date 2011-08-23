cd db
mkdir raw
builddb3
compressdb3
deltree /Y raw
del /q raw
rmdir raw
del buildlog.txt
del compress.txt
del compresslog.txt
del dbhist2.txt
del dbhist3.txt
del dbhist4.txt
del dbhist5.txt
del dbhist6.txt
del perflog.txt
del sizelog.txt
del winlog.txt
echo database generation successful! you can close this window now.
