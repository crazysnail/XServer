#! /bin/sh
#http://www.linuxidc.com/Linux/2013-11/93253.htm
#mysqldump -uusername -hhostname -ppassword db_name |gzip -9 >/path/db_name.gz
#gunzip </path/db_name.gz | mysql -uusername -ppassword -hhostname db_name
 
echo "dump db....."
basepath=$(cd `dirname $0`; pwd)
cd ${basepath}
curday=`date +%Y_%m_%d_%H_%M_%S`
Serverid=`cat system_start.conf | grep "id=" | awk -F '=' '$2~/[0-9]+/{print $2}'`
echo ${Serverid}
sudo mysqldump -uroot -hlocalhost -pxQxdWax#NTQ3DGV --hex-blob diablo_game_${Serverid} |gzip -9 >./dbbak/db_${Serverid}_${curday}_bak.gz
echo "dump ok ....."

