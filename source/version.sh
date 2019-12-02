#!/bin/sh
#author san by 2016-5-11

Version_Dir=../version/
dirnm=$(dirname "$0")
echo $dirnm
filepath=$(cd "$(dirname "$0")"; pwd)
echo $pwd
echo $0

upname=$(basename $(dirname "$PWD"))
echo $upname

echo ${filepath}
execname=$(basename $0)
echo ${execname}
curtime=$(date +%Y%m%d_%H%M%S)
echo ${curtime}

mkdir ${Version_Dir}${upname}_${curtime}
mkdir ${Version_Dir}${upname}_${curtime}/db_config
mkdir ${Version_Dir}${upname}_${curtime}/json
mkdir ${Version_Dir}${upname}_${curtime}/linux
mkdir ${Version_Dir}${upname}_${curtime}/linux/debug
mkdir ${Version_Dir}${upname}_${curtime}/lua_script

revision=$(grep 'revision' lastinfo.log| grep -o -E '[0-9]*')
echo "vs: "${revision}

#git pull >lastinfo.log
##svn log -r ${revision}:HEAD --xml >update.log
#echo "cmake.."
#cmake ../temp
cd ../temp
#echo "begin make.."
#make -j4 >../makeinfo.log
cd ../source
#if grep -q "100%] Built target GameServer" makeinfo.log;
#then
	echo "make begin!"
	cp ../temp/bin/GameServer ${Version_Dir}${upname}_${curtime}/linux/debug/
	cp bin/db_config/*.csv ${Version_Dir}${upname}_${curtime}/db_config/
	cp -r bin/lua_script/* ${Version_Dir}${upname}_${curtime}/lua_script/
	cp -r bin/json/* ${Version_Dir}${upname}_${curtime}/json/
	cp ../makeinfo.log ${Version_Dir}${upname}_${curtime}
	cp lastinfo.log ${Version_Dir}${upname}_${curtime}
	#cp update.log ${Version_Dir}${upname}_${curtime}
	cp ../openserver/* ${Version_Dir}${upname}_${cur-time}/linux/debug/
	mkdir ${Version_Dir}${upname}_${curtime}/sys_command
	#exit
#else
	echo "cp done!";
#fi