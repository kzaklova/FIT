#! /bin/bash


if [ ! -e "xzaklo00" ]; then
    echo "Build the project first!"
    exit
fi

### Test for download

wget http://ivs.fit.vutbr.cz/ivs-projekt-2.html 2>/dev/null
./xzaklo00 http://ivs.fit.vutbr.cz/ivs-projekt-2.html zadani.html >/dev/null 2>&1

download=$(md5sum ivs-projekt-2.html | awk '{print $1}')
my_download=$(md5sum zadani.html | awk '{print $1}')

if [ "$download" == "$my_download" ]; then
	echo "Download OK"
else
	echo "Download FAILED"
fi

### Test for parity calculating

echo "ahoj" > even1
echo "lol" > even2
echo "c" > even3

echo "hello" > odd1
echo "a" > odd2
echo "bbb" > odd3

for i in {1..3}
do
	./xzaklo00 even$i >/dev/null 2>&1
	par=$?

	if [ "$par" != "0" ]; then
		echo "Parity FAILED"
		exit
	fi
done

for i in {1..3}
do
	./xzaklo00 odd$i >/dev/null 2>&1
	par=$?

	if [ "$par" != "1" ]; then
		echo "Parity FAILED"
		exit
	fi
done

echo "Parity OK"
