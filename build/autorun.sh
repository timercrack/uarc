#!/bin/bash

finish()
{
	rm -rf $TMP_DIR >/dev/null 2>&1
	rm -rf $PKG_DIR >/dev/null 2>&1
	
	trap 2 3

	exit $result
}

extract()
{
	[ -d $TMP_DIR ] && rm -rf $TMP_DIR
	if [ -d $PKG_DIR ]; then
		cp -rf $PKG_DIR $TMP_DIR
	else
		echo "Error:$PKG_DIR 不存在"
		echo ""
		result=1
		return $result
	fi
	return $result
}

check_version()
{
	oldversion="0"
	newversion="0"
	[ -f $INSTALL_DIR/etc/uarc/version ] && oldversion=$(cat $INSTALL_DIR/etc/uarc/version | sed 's/ //g')
	[ -f $TMP_DIR/etc/uarc/version ] && newversion=$(cat $TMP_DIR/etc/uarc/version | sed 's/ //g')
	if [ $newversion = "0" ]; then
		result=1
		return $result
	fi
	backtitle="通用采集调控平台Ver${newversion}"
	common_option="--backtitle $backtitle"

	return $result
}

init()
{
	INSTALL_DIR=/home/uarc
	PKG_DIR=$(pwd)/../tmp_of_src
	TMP_DIR=/tmp/tmp_of_pkg
	DIALOG_DIR=$TMP_DIR/dialog
	DIALOG=$DIALOG_DIR/dialog

	STOP_UARC=$INSTALL_DIR/bin/uarc_stop
	START_UARC=$INSTALL_DIR/bin/uarc_start

	uarc_cfg_intfd=$INSTALL_DIR/etc/uarc/uarc-cfg/uarc_intfd.properties
	uarc_cfg_rmemd=$INSTALL_DIR/etc/uarc/uarc-cfg/uarc_rmemd.properties
	uarc_cfg_colld=$INSTALL_DIR/etc/uarc/uarc-cfg/uarc_colld.properties

	CONFIG_FILE=$INSTALL_DIR/etc/uarc/uarc_config

	trap "" 2 3
	
	if [ $INSTALL -ne 1 ]; then
		DIALOG_DIR=$INSTALL_DIR/dialog
		DIALOG=$DIALOG_DIR/dialog
	fi
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIALOG_DIR
	
	return $result
}

load_config()
{
        . $CONFIG_FILE
	return $result
}

check_user()
{
	if [ "$(id -nu)" != "root" ]; then
		if [ -x $DIALOG ]; then
			$DIALOG $common_option --title "无效的用户" --clear \
			--msgbox "该软件程序只能以root用户身份运行。" 0 0
		elif [ -x $DIALOG ]; then
			$DIALOG $common_option --title "无效的用户" --clear \
			--msgbox "该软件程序只能以root用户身份运行。" 0 0
		else
			echo "该软件程序只能以root用户身份运行。"
			echo ""
		fi
		result=1
	fi

	return $result
}

check_para()
{
	INSTALL=0
	CONFIG=0
	START=0
	STOP=0
	CLUSTER=0
	MASTER=0
	SLAVE=0

	for p in $@
	do
		if [ $p = "install" ]; then
			INSTALL=1
		elif [ $p = "config" ]; then
			CONFIG=1
		elif [ $p = "start" ]; then
			START=1
		elif [ $p = "stop" ]; then
			STOP=1
		elif [ $p = "cluster" ]; then
			CLUSTER=1
		elif [ $p = "master" ]; then
			MASTER=1
		elif [ $p = "slave" ]; then
			SLAVE=1
		else
			echo "无效参数:$p"
			result=1
			return $result
		fi
		shift
	done

	if [ ${INSTALL} -eq 0 -a ${CONFIG} -eq 0 -a ${START} -eq 0 -a ${STOP} -eq 0 ]; then
		INSTALL=1
		CONFIG=1
		START=1
	fi
	if [ ${MASTER} -eq 0 -a ${SLAVE} -eq 0 ]; then
		MASTER=1
	fi
	return $result
}

start_redis()
{
	cmd=`redis-server /etc/uarc/redis.conf`
	sleep 5
}

stop_redis()
{
	cmd=`killall redis-server`
}

start_keepalived()
{
	echo "start keepalived"
	echo 5 > /usr/local/etc/keepalived/.check_uarc_start
	cmd=`service keepalived start`
}

stop_keepalived()
{
	echo "stop keepalived"
	cmd=`service keepalived stop`
}

copy_files()
{
	ret=0
	total_size=${1?"Please specify the total size."}
	files=${2?"No files specified!."}
	copied_size=0
	for f in $files
	do
		[ "$f" = "$INSTALL_DIR/" ] && continue
		if [ -d $f ]; then
			mkdir -p $INSTALL_DIR/${f#$TMP_DIR/}
			continue
		fi
		f_size=$(du $f | tail -n1 | awk '{print $1'})
		let copied_size+=$f_size
		\cp -df $f $INSTALL_DIR/${f#$TMP_DIR/}
		size_percent=$(expr $copied_size \* 100 / $total_size)
		usleep 100000
		echo "${f#$TMP_DIR/} 安装中……"
		echo "XXX"
		echo "$size_percent"
	done | \
	$DIALOG $common_option --title "安装中" --clear \
	--gauge "程序安装中，请稍后……" 10 60
	
	if [ $CLUSTER -eq 0 ]; then
		rm -rf $INSTALL_DIR/usr/
		rm -rf $INSTALL_DIR/bin/check_uarc
		rm -rf $INSTALL_DIR/etc/init.d/
	elif [ $CLUSTER -eq 1 ]; then 
		\cp -srf $INSTALL_DIR/etc/init.d/* /etc/init.d/ || ret=1
		\cp -srf $INSTALL_DIR/usr/local/etc/* /usr/local/etc/ || ret=1
		\cp -srf $INSTALL_DIR/usr/local/sbin/* /usr/local/sbin/ || ret=1
	fi

	\cp -srf $INSTALL_DIR/etc/uarc /etc/ || ret=1
	\cp -srf $INSTALL_DIR/bin/* /bin/ || ret=1
	\cp -srf $INSTALL_DIR/lib/* /lib/ || ret=1

	return $ret
}

stop_prog()
{
	if ps -ef | grep -v "grep" |  grep "uarc_.* --daemon" >/dev/null 2>&1; then
		$DIALOG $common_option --title "确认" --clear \
		--yesno "系统检测到程序已经在运行，确定要继续吗？" \
		7 40
		case $? in
			0)
				#关闭redis-server
				stop_redis
				if [ -x $STOP_UARC ]; then
					$STOP_UARC >/dev/null 2>&1
				else
					ps -ef | grep -v "grep" | grep "uarc_.* --daemon" | awk '{print $2}' | xargs kill >/dev/null 2>&1
				fi
				#关闭keepalived
				stop_keepalived
				;;
			*)
				echo "取消"
				return 1
				;;
		esac
	fi
	sleep 1

	return 0
}

install_config()
{
	sed -i -r '/^.*CLUSTER/s,(^.*CLUSTER=).*,\1'$CLUSTER',' $CONFIG_FILE || (result=1)
	return $result
}

fresh_install()
{
	stop_prog
	
	rm -rf $INSTALL_DIR >/dev/null 2>&1
	total_size=$(du $TMP_DIR | tail -n1 | awk '{print $1}')
	#files=$(find $TMP_DIR/ -name "*" ! -lname "*" -print)
	files=$(find $TMP_DIR/ -name "*" -print)
	copy_files "$total_size" "$files"
	mkdir -p /var/lib/uarc/

	install_config || finish


	return $?
}

upgrade()
{
	stop_prog

	total_size=0
	$DIALOG $common_option --title "请选择" \
	--yesno "是否保留日志、报文记录等文件？" 0 0
	case $? in
		0)
			files=$(find $TMP_DIR/ -name "*" ! -lname "*" -print | grep -Ev "$TMP_DIR/etc|$TMP_DIR/log|$TMP_DIR/msg")
			;;
		1)
			rm -rf $INSTALL_DIR/log/* $INSTALL_DIR/msg/* 1>/dev/null 2>&1
			files=$(find $TMP_DIR/ -name "*" ! -lname "*" -print | grep -v "$TMP_DIR/etc")
			;;
		*)
			;;
	esac
	for f in $files
	do
		let total_size+=$(du $f | awk '{print $1}')
	done
	copy_files "$total_size" "$files"
	ret=$?
	ret -eq 0 || finish
	install_config || finish

	return ret
}

uarc_config()
{
	proto_buffer_port_num=$(cat $uarc_cfg_intfd | grep "Tcp.listenPort" | awk -F'=' '{print $2}' | sed 's/ //')
	redis_db_ip_addr=$(cat $uarc_cfg_colld | grep "RedisDB.Base.IPAddr" | awk -F'=' '{print $2}' | sed 's/ //')
	redis_db_port_num=$(cat $uarc_cfg_colld | grep "RedisDB.Base.Port" | awk -F'=' '{print $2}' | sed 's/ //')
	smartreal_db_ip_addr=$(cat $uarc_cfg_rmemd | grep "SmartRealDB.Base.IPAddr" | awk -F'=' '{print $2}' | sed 's/ //')
	smartreal_db_port_num=$(cat $uarc_cfg_rmemd | grep "SmartRealDB.Base.Port" | awk -F'=' '{print $2}' | sed 's/ //')
	smartreal_db_username=$(cat $uarc_cfg_rmemd | grep "SmartRealDB.Base.UserName" | awk -F'=' '{print $2}' | sed 's/ //')
	smartreal_db_password=$(cat $uarc_cfg_rmemd | grep "SmartRealDB.Base.Password" | awk -F'=' '{print $2}' | sed 's/ //')
	mysql_db_ip_addr=$(cat $uarc_cfg_rmemd | grep "RelationDB.Base.IPAddr" | awk -F'=' '{print $2}' | sed 's/ //')
	mysql_db_port_num=$(cat $uarc_cfg_rmemd | grep "RelationDB.Base.Port" | awk -F'=' '{print $2}' | sed 's/ //')
	mysql_db_database=$(cat $uarc_cfg_rmemd | grep "RelationDB.Base.DataBaseName" | awk -F'=' '{print $2}' | sed 's/ //')
	mysql_db_username=$(cat $uarc_cfg_rmemd | grep "RelationDB.Base.UserName" | awk -F'=' '{print $2}' | sed 's/ //')
        mysql_db_password=$(cat $uarc_cfg_rmemd | grep "RelationDB.Base.Password" | awk -F'=' '{print $2}' | sed 's/ //')

	title="参数配置"

	while [ true ]; do
		exec 3>&1
		value=$( $DIALOG $common_option --clear --title $title \
		--ok-label "提交" \
                --cancel-label "退出" \
		--form "请根据实际工作环境配置以下各项连接参数" \
		0 0 0 \
		"ProtoBuf  Port Num:"    1 1 "$proto_buffer_port_num"     1 21 64 0 \
		"Redis DB  IP  Addr:"    3 1 "$redis_db_ip_addr"          3 21 64 0 \
		"Redis DB  Port Num:"    4 1 "$redis_db_port_num"         4 21 64 0 \
		"Smartreal IP  Addr:"    6 1 "$smartreal_db_ip_addr"        6 21 64 0 \
                "Smartreal Port Num:"    7 1 "$smartreal_db_port_num"       7 21 64 0 \
                "Smartreal Username:"    8 1 "$smartreal_db_username"       8 21 64 0 \
		"Smartreal Password:"    9 1 "$smartreal_db_password"       9 21 64 0 \
		"Mysql DB  IP  Addr:"   11 1 "$mysql_db_ip_addr"         11 21 64 0 \
                "Mysql DB  Port Num:"   12 1 "$mysql_db_port_num"        12 21 64 0 \
                "Mysql DB  DataBase:"   13 1 "$mysql_db_database"        13 21 64 0 \
		"Mysql DB  Username:"   14 1 "$mysql_db_username"        14 21 64 0 \
		"Mysql DB  Password:"   15 1 "$mysql_db_password"        15 21 64 0 \
		2>&1 1>&3 )
		ret=$?
		exec 3>&-
		case $ret in
			1)
				$DIALOG $common_option --clear \
				--title "确认" \
				--yesno "确定要放弃配置？" \
				0 0
				case $? in
					0)
						return $result
						;;
					*)
						;;
				esac
				;;
			0)
				proto_buffer_port_num=$(echo "$value" | sed -n '1p')
				redis_db_ip_addr=$(echo "$value" | sed -n '2p')
				redis_db_port_num=$(echo "$value" | sed -n '3p')
				smartreal_db_ip_addr=$(echo "$value" | sed -n '4p')
				smartreal_db_port_num=$(echo "$value"| sed -n '5p')
				smartreal_db_username=$(echo "$value" | sed -n '6p')
                                smartreal_db_password=$(echo "$value" | sed -n '7p')
				mysql_db_ip_addr=$(echo "$value" | sed -n '8p')
				mysql_db_port_num=$(echo "$value"| sed -n '9p')
				mysql_db_database=$(echo "$value"| sed -n '10p')
				mysql_db_username=$(echo "$value" | sed -n '11p')
                                mysql_db_password=$(echo "$value" | sed -n '12p')

	      if [[ ! $proto_buffer_port_num =~ ^([0-9]{1,4}|[1-5][0-9]{4}|6[0-5]{2}[0-3][0-5])$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "Protobuffer的Port端口号输入格式错误,请重新输入!" 0 0
					proto_buffer_port_num=
					continue
	      fi
	      if [[ ! $redis_db_ip_addr =~ ^(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "Redis数据库的IP地址输入格式错误,请重新输入!" 0 0
					redis_db_ip_addr=
					continue
	      fi
	      if [[ ! $redis_db_port_num =~ ^([0-9]{1,4}|[1-5][0-9]{4}|6[0-5]{2}[0-3][0-5])$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "Redis数据库的Port端口号输入格式错误,请重新输入!" 0 0
					redis_db_port_num=
					continue
	      fi
	      if [[ ! $smartreal_db_ip_addr =~ ^(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "实时数据库的IP地址输入格式错误,请重新输入!" 0 0
					smartreal_db_ip_addr=
					continue
	      fi
	      if [[ ! $smartreal_db_port_num =~ ^([0-9]{1,4}|[1-5][0-9]{4}|6[0-5]{2}[0-3][0-5])$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "实时数据库的Port端口号输入格式错误,请重新输入!" 0 0
					smartreal_db_port_num=
					continue
	      fi
	      if [[ ! $mysql_db_ip_addr =~ ^(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "MySQL数据库的IP地址输入格式错误,请重新输入!" 0 0
					mysql_db_ip_addr=
					continue
	      fi
	      if [[ ! $mysql_db_port_num =~ ^([0-9]{1,4}|[1-5][0-9]{4}|6[0-5]{2}[0-3][0-5])$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "MySQL数据库的Port端口号输入格式错误,请重新输入!" 0 0
					mysql_db_port_num=
					continue
	      fi
				$DIALOG $common_option --clear \
				--title "确认" \
				--yesno "您配置的参数如下: \nProtoBuf  Port Num: $proto_buffer_port_num\n\nRedis DB  IP  Addr: $redis_db_ip_addr\nRedis DB  Port Num: $redis_db_port_num\n\nSmartreal IP  Addr: $smartreal_db_ip_addr\nSmartreal Port Num: $smartreal_db_port_num\nSmartreal Username: $smartreal_db_username\nSmartreal Password: $smartreal_db_password\n\nMysql DB  IP  Addr: $mysql_db_ip_addr\nMysql DB  Port Num: $mysql_db_port_num\nMysql DB  DataBase: $mysql_db_database\nMysql DB  Username: $mysql_db_username\nMysql DB  Password: $mysql_db_password" \
				0 0
				case $? in
					0)

			        sed -i -r '/^.*Tcp.listenPort/s,(^.*Tcp.listenPort.*= ).*,\1'$proto_buffer_port_num',' $uarc_cfg_intfd || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.IPAddr/s,(^.*RedisDB.Base.IPAddr.*= ).*,\1'$redis_db_ip_addr',' $uarc_cfg_colld || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.Port/s,(^.*RedisDB.Base.Port.*= ).*,\1'$redis_db_port_num',' $uarc_cfg_colld || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.IPAddr/s,(^.*RedisDB.Base.IPAddr.*= ).*,\1'$redis_db_ip_addr',' $uarc_cfg_intfd || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.Port/s,(^.*RedisDB.Base.Port.*= ).*,\1'$redis_db_port_num',' $uarc_cfg_intfd || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.IPAddr/s,(^.*RedisDB.Base.IPAddr.*= ).*,\1'$redis_db_ip_addr',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*RedisDB.Base.Port/s,(^.*RedisDB.Base.Port.*= ).*,\1'$redis_db_port_num',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*SmartRealDB.Base.IPAddr/s,(^.*SmartRealDB.Base.IPAddr.*= ).*,\1'$smartreal_db_ip_addr',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*SmartRealDB.Base.Port/s,(^.*SmartRealDB.Base.Port.*= ).*,\1'$smartreal_db_port_num',' $uarc_cfg_rmemd || (result=1 && return 1)
                                sed -i -r '/^.*SmartRealDB.Base.UserName/s,(^.*SmartRealDB.Base.UserName.*= ).*,\1'$smartreal_db_username',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*SmartRealDB.Base.Password/s,(^.*SmartRealDB.Base.Password.*= ).*,\1'$smartreal_db_password',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*RelationDB.Base.IPAddr/s,(^.*RelationDB.Base.IPAddr.*= ).*,\1'$mysql_db_ip_addr',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*RelationDB.Base.Port/s,(^.*RelationDB.Base.Port.*= ).*,\1'$mysql_db_port_num',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*RelationDB.Base.DataBaseName/s,(^.*RelationDB.Base.DataBaseName.*= ).*,\1'$mysql_db_database',' $uarc_cfg_rmemd || (result=1 && return 1)
                                sed -i -r '/^.*RelationDB.Base.UserName/s,(^.*RelationDB.Base.UserName.*= ).*,\1'$mysql_db_username',' $uarc_cfg_rmemd || (result=1 && return 1)
		                sed -i -r '/^.*RelationDB.Base.Password/s,(^.*RelationDB.Base.Password.*= ).*,\1'$mysql_db_password',' $uarc_cfg_rmemd || (result=1 && return 1)

						return $result
						;;
					1)
						;;
					*)
						;;
				esac
				;;
			*)
				;;
		esac
	done

	result=1
	return $result
}

cluster_config()
{
	title="参数配置"
	while [ true ]; do
		exec 3>&1
		value=$( $DIALOG $common_option --clear --title $title \
		--ok-label "提交" \
                --cancel-label "退出" \
		--form "请根据实际工作环境配置以下各项连接参数" \
		0 0 0 \
		"Node  Name:"	 1 1 "$NODE_NAME"	1 21 64 0 \
		"Virtual IP:"    3 1 "$VIP"		3 21 64 0 \
		"Redis Port:"    5 1 "$REDIS_PORT"	5 21 64 0 \
		"Gate   Way:"    7 1 "$GATEWAY"         7 21 64 0 \
		2>&1 1>&3 ) 
		ret=$?
		exec 3>&-
		case $ret in
			1)
				$DIALOG $common_option --clear \
				--title "确认" \
				--yesno "确定要放弃配置？" \
				0 0
				case $? in
					0)
						return $result
						;;
					*)
						;;
				esac
				;;
			0)
				NODE_NAME=$(echo "$value" | sed -n '1p')
				VIP=$(echo "$value" | sed -n '2p')
				REDIS_PORT=$(echo "$value" | sed -n '3p')
				GATEWAY=$(echo "$value" | sed -n '4p')
	      if [[ ! $NODE_NAME =~ ^Node[A-Z]$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "Node Name输入格式错误,请重新输入!" 0 0
					NODE_NAME=
					continue
	      fi

	      if [[ ! $VIP =~ ^(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "虚拟IP地址输入格式错误,请重新输入!" 0 0
					VIP=
					continue
	      fi
	      if [[ ! $REDIS_PORT =~ ^([0-9]{1,4}|[1-5][0-9]{4}|6[0-5]{2}[0-3][0-5])$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "Redis的Port端口号输入格式错误,请重新输入!" 0 0
					REDIS_PORT=
					continue
	      fi
	      if [[ ! $GATEWAY =~ ^(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))$ ]]; then
					$DIALOG $common_option --clear \
					--title "错误" \
					--msgbox "网关输入格式错误,请重新输入!" 0 0
					GATEWAY=
					continue
	      fi
				$DIALOG $common_option --clear \
				--title "确认" \
				--yesno "您配置的参数如下: \n Node Name: $NODE_NAME\n\nVirtual IP: $VIP\n\nRedis Port: $REDIS_PORT\n\n Gate Way : $GATEWAY" \
				0 0
				case $? in
					0)

			        sed -i -r '/^.*NODE_NAME/s,(^.*NODE_NAME=).*,\1'$NODE_NAME',' $CONFIG_FILE || (result=1 && return 1)
			        sed -i -r '/^.*VIP/s,(^.*VIP=).*,\1'$VIP',' $CONFIG_FILE || (result=1 && return 1)
		                sed -i -r '/^.*REDIS_PORT/s,(^.*REDIS_PORT=).*,\1'$REDIS_PORT',' $CONFIG_FILE || (result=1 && return 1)
		                sed -i -r '/^.*GATEWAY/s,(^.*GATEWAY=).*,\1'$GATEWAY',' $CONFIG_FILE || (result=1 && return 1)

				#修改keepalived.conf
				KEEPALIVED_CONFIG=/usr/local/etc/keepalived/keepalived.conf
				sed -i -r '/.*(([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2([0-4][0-9]|5[0-5]))\/24.*/d' $KEEPALIVED_CONFIG || (result=1 && return 1)
				sed -i -r '/.*virtual_ipaddress.*\{$/a '\\\\t$VIP\/24'' $KEEPALIVED_CONFIG || (result=1 && return 1)
				sed -i -r '/^.*[^_]router_id Node[A-Z]/s,(^.*[^_]router_id )Node[A-Z],\1'$NODE_NAME',' $KEEPALIVED_CONFIG || (result=1 && return 1)
						return $result
						;;
					1)
						;;
					*)
						;;
				esac
				;;
			*)
				;;
		esac
	done
}

gui_config()
{
	uarc_config || finish
	if [ $CLUSTER -eq 1 ]; then
		cluster_config || finish
	fi
}

gui_install()
{
	if [ -d $INSTALL_DIR/bin -a $oldversion != "0" ]; then
		#upgrade or fresh install
		$DIALOG $common_option --title "选择安装方式" --clear \
		--extra-button --extra-label "升级安装" \
		--ok-label "重新安装" \
		--cancel-label "退出" \
		--yesno "系统检测到已经安装了版本$oldversion，您想？" 0 0
		case $? in
			0)
				fresh_install
				;;	
			3)
				if [ "$oldversion" != "NULL" -a "$(echo $oldversion | sed 's/\.//g')" -gt "$(echo $newversion | sed 's/\.//g')" ]; then
					$DIALOG $common_option --title "版本过低" --clear \
					--msgbox "提供的软件版本（版本号：$newversion）太低，不允许进行升级安装。\n已经安装的版本号为：$oldversion" 0 0
					finish
				fi
				upgrade
				;;
			*)
				echo "安装程序被终止。"
				finish
				;;
		esac
	else
		fresh_install

	fi

	[ $? -ne 0 ] && {
		echo "复制文件出现致命错误。"
		finish
	}

	ldconfig
	echo "$newversion" > $INSTALL_DIR/etc/uarc/version

	return 0
}

gui_start()
{
	stop_prog || finish
	$DIALOG $common_option --title "启动程序" --clear \
	--yesno "现在要启动程序吗？" 0 0
	if [ $? -eq 0 ]; then
		#启动redis
		start_redis
		if [ -x $START_UARC ]; then
			if [ $CLUSTER -eq 1 ]; then
				if [ $MASTER -eq 1 ]; then
					$START_UARC 2>/dev/null | \
					$DIALOG $common_option --title "程序启动中..." --clear \
					--progressbox 10 100
					$DIALOG $common_option --title "成功" \
					--sleep 1 --infobox "程序启动成功，结束本安装程序。" 0 0
				fi
			elif [ $CLUSTER -eq 0 ]; then
				$START_UARC 2>/dev/null | \
				$DIALOG $common_option --title "程序启动中..." --clear \
				--progressbox 10 100
				$DIALOG $common_option --title "成功" \
				--sleep 1 --infobox "程序启动成功，结束本安装程序。" 0 0
			fi
		else
			$DIALOG $common_option --title "错误" --clear \
			--msgbox "未知错误，启动脚本不存在。" 0 0
		fi
		if [ $CLUSTER -eq 1 ]; then
			#启动keepalived
			start_keepalived
		fi
	fi
}

gui_stop()
{
	stop_prog || finish
}

main()
{
	result=0
	check_para $1 || finish
	check_user || finish
	init || finish
	if [ $INSTALL -eq 1 ]; then
		extract || finish
		check_version || finish
	fi

	if [ $INSTALL -eq 1 ]; then
		gui_install
	fi
	if [ $CONFIG -eq 1 ]; then
		load_config || finish
		gui_config
	fi
	if [ $START -eq 1 ]; then
		load_config || finish
		gui_start
	fi
	if [ $STOP -eq 1 ]; then
		load_config || finish
		gui_stop
	fi
	finish
}

main "$*"
