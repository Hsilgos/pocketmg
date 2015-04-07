echo off
echo "Processing valid..."
cd resources/valid
for /r %%i in (*.bmp) do python ../../../res/res2code.py -i %%i -d ../../Generated/bmp/valid
cd ..
cd ..

echo "Processing corrupt..."
cd resources/corrupt
for /r %%i in (*.bmp) do python ../../../res/res2code.py -i %%i -d ../../Generated/bmp/corrupt
cd ..
cd ..

echo "Processing questionable..."
cd resources/questionable
for /r %%i in (*.bmp) do python ../../../res/res2code.py -i %%i -d ../../Generated/bmp/questionable
cd ..
cd ..
echo on