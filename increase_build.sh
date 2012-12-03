#!/bin/sh

if [ $# != 1 ]; then
	echo "Usage: $0 <filename.pro>"
	exit 1
fi

if [ ! -f $1 ]; then
	echo "This file doesn't exist."
	exit 2
fi

echo "Copie du fichier... \c"
cp $1 $1.tmp
echo "OK!"

echo "Modif de la version... \c"
entireLine=`grep "VERSION" $1.tmp`
numberZone=`echo $entireLine | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+"`
version_maj=`echo $numberZone | cut -d. -f1`
version_min=`echo $numberZone | cut -d. -f2`
version_build=`echo $numberZone | cut -d. -f3`
echo "OK!"

echo "Ecriture du nouveau fichier... \c"
sed "s/VERSION.*/VERSION		= $version_maj.$version_min.`expr $version_build + 1`/" $1.tmp > $1
echo "OK!"
echo
echo "$0 OK!"
exit 0
