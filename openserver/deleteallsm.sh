#! /bin/sh

sudo ipcs -m | grep "666" | awk '$2~/[0-9]+/{print $2}'|
while read s
do
sudo ipcrm -m ${s}
done

sudo rm -f share_mem_size.conf

