

rm -rf ./log/*.log


#! /bin/sh
prestart(){
	Serverid=`cat server_config.conf | grep "server_id=" | awk -F '=' '$2~/[0-9]+/{print $2}'`
	echo ${Serverid}
	GameName="GameServer"${Serverid}
	echo ${GameName}
	ProcNumber=`ps -ef |grep ${GameName} |grep -v grep|wc -l`  
	if [ $ProcNumber -le 0 ];then  
	   result=0  
	else  
		result=1 
		echo "have a GameServer_,please shut it down!!!!"
		exit
	fi  

	sudo mv "GameServer" ${GameName}

	if grep -q "gm_mode=1" server_config.conf;
	then
		echo "gm_mode open !!!!"
		#exit
	fi

	if [ -f "../sys_command/sys_shutdown" ]; then 
		sudo rm -f ../sys_command/sys_shutdown
	fi

	if [ -f "../sys_command/kill_server" ]; then 
		sudo rm -f ../sys_command/kill_server
	fi

	if [ -f "start_server_ok" ]; then 
		sudo rm -f start_server_ok
	fi

	sudo chmod +x ./${GameName}
	#sudo chmod +x ./deletesm.sh
	#sudo chmod +x ./log/bi_py/*.py
	#sudo chmod +x ./log/bi_py/*.sh

	curday=`date +%Y_%m_%d_%H_%M_%S`
	#sudo mysqldump -uroot -hlocalhost -pxQxdWax#NTQ3DGV --hex-blob mt_game_${Serverid} |gzip -9 >./dbbak/db_${curday}_bak.gz
	
	#python updatedb_linux.py
}
case $1 in
	db)
	prestart
	echo "db start ok~~~"
	#source ./deletesm.sh
	source ./deleteallsm.sh
	nohup ./${GameName} &
	;;
	sm)
	prestart
	echo "sm start ok~~~"
	nohup ./${GameName} &
	;;
	*)
	echo "Please input start mode"
	;;
esac

ps -ax | grep ${GameName}



