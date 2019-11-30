#! /bin/sh
if [ -f "share_mem_size.conf" ]; then
  #sudo ipcs -m | grep "666" | awk '$2~/[0-9]+/{print $2}'|
  cat share_mem_size.conf |  awk -F ',' '{printf("0x%08x\n", $1)}' |
  while read s
  do
    smkeyid=$(sudo ipcs -m | grep $s | awk '$2~/[0-9]+/{print $2}')
	if [ ${smkeyid} ]; then
		sudo ipcrm -m ${smkeyid}
	fi
  done

  sudo rm -f share_mem_size.conf
fi
