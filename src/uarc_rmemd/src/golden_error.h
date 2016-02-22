#ifndef __GOLDEN_ERROR_H__
#define __GOLDEN_ERROR_H__

#define GoE_UNKNOWN_ERROR               0xFFFFFFFF  // 未知错误
#define GoE_OK                          0x00000000  // 操作成功完成
#define GoE_WINDOWS_ERROR               0xFFF00000  // Windows操作系统错误的起始值
#define GoE_WINDOWS_ERROR_MAX           0xFFFF0000  // Windows操作系统错误的结束值

#define GOLDEN_SOCKET_ADAPTER_FALSE(GOLDEN_ERROR) (GOLDEN_ERROR > 0xFFF02710 && GOLDEN_ERROR <= 0xFFF02AFC)
#define GOLDEN_WINDOWS_ERROR_TO_GOLDEN(WINDOWS_ERROR) (GoE_WINDOWS_ERROR + WINDOWS_ERROR)

#define GoE_INVALID_OPENMODE            0xFFFF0001  // 无效的文件打开方式
#define GoE_OPENFILE_FAILED             0xFFFF0002  // 打开文件失败
#define GoE_MOVETOEND_FAILED            0xFFFF0003  // 移动文件指针到文件尾失败
#define GoE_DIFFER_READBYTES            0xFFFF0004  // 读取内容长度与要求不符
#define GoE_GETFILEPOS_FAILED           0xFFFF0005  // 获取当前文件指针失败
#define GoE_FLUSHFILE_FAILED            0xFFFF0006  // 刷新文件缓冲区失败
#define GoE_SETSIZE_FAILED              0xFFFF0007  // 设置文件大小失败
#define GoE_FILE_NOT_CLOSED             0xFFFF0008  // 试图用未关闭的文件对象创建或打开文件
#define GoE_FILE_UNKNOWN                0xFFFF0009  // 创建或打开文件时必须指定文件名
#define GoE_INVALID_HEADER              0xFFFF000A  // 数据文件头信息错误
#define GoE_DISABLED_FILE               0xFFFF000B  // 数据文件无效，试图访问无效数据文件
#define GoE_FILE_NOT_OPENED             0xFFFF000C  // 试图访问尚未打开的数据文件
#define GoE_POINT_NOT_FOUND             0xFFFF000D  // 要求访问的标签点不存在或无效
#define GoE_READYBLOCK_NOT_FOUND        0xFFFF000E  // 数据文件中找不到从指定数据块以后的可用的空块
#define GoE_FILE_IS_INCULT              0xFFFF000F  // 文件未被使用过
#define GoE_FILE_IS_FULL                0xFFFF0010  // 数据文件已满
#define GoE_FILEEX_IS_FULL              0xFFFF0011  // 数据文件扩展区已满，无法继续装载数据
#define GoE_INVALID_DATA_TYPE           0xFFFF0012  // 无效的数据类型
#define GoE_DATABLOCK_NOT_FOUND         0xFFFF0013  // 找不到符合时间条件的数据块
#define GoE_DATA_BETWEEN_BLOCK          0xFFFF0014  // 数据时间位于找到的块和下一个数据块之间
#define GoE_CANT_MODIFY_EXIST_VALUE     0xFFFF0015  // 不允许修改已存在的数据
#define GoE_WRONGDATA_IN_BLOCK          0xFFFF0016  // 块中有错误数据导致数据块头信息不符
#define GoE_DATATIME_NOT_IN             0xFFFF0017  // 数据文件中没有该标签点指定时间的数据
#define GoE_NULL_ARCHIVE_PATH           0xFFFF0018  // 操作的数据文件路径为空
#define GoE_REG_ARCHIVE_PATH            0xFFFF0019  // 数据文件已被注册
#define GoE_UNREG_ARCHIVE_PATH          0xFFFF001A  // 未注册的数据文件
#define GoE_FILE_INEXISTENCE            0xFFFF001B  // 指定的文件不存在
#define GoE_DATA_TYPE_NOT_MATCH         0xFFFF001C  // 数据类型不匹配
#define GoE_FILE_IS_READONLY            0xFFFF001D  // 不允许修改只读数据文件中的数据
#define GoE_TOMANY_ARCHIVE_FILE         0xFFFF001E  // 过多的数据文件
#define GoE_NO_POINTS_LIST              0xFFFF001F  // 缺少标签点列表
#define GoE_NO_ACTIVED_ARCHIVE          0xFFFF0020  // 缺少活动文档
#define GoE_NO_ARCHIVE_FILE             0xFFFF0021  // 缺少数据文档
#define GoE_NEED_ACTIVED_ARCHIVE        0xFFFF0022  // 只能在活动文档上执行该操作
#define GoE_INVALID_TIMESTAMP           0xFFFF0023  // 无效的时间戳
#define GoE_NEED_MORE_WRITABLE          0xFFFF0024  // 非只读文档个数太少
#define GoE_NO_ARCHIVE_FOR_PUT          0xFFFF0025  // 找不到合适的追加历史数据的文档
#define GoE_INVALID_VALUE_MODE          0xFFFF0026  // 无效的取值模式
#define GoE_DATA_NOT_FOUND              0xFFFF0027  // 找不到需要的数据
#define GoE_INVALID_PARAMETER           0xFFFF0028  // 无效的参数
#define GoE_REDUPLICATE_TAG             0xFFFF0029  // 重复的标签点名
#define GoE_REDUPLICATE_TABNAME         0xFFFF002A  // 重复的表名称
#define GoE_REDUPLICATE_TABID           0xFFFF002B  // 重复的表ID
#define GoE_TABLE_NOT_FOUND             0xFFFF002C  // 指定的表不存在
#define GoE_UNSUPPORTED_CLASSOF         0xFFFF002D  // 不支持的标签点类别
#define GoE_WRONG_OR_DUPLIC_TAG         0xFFFF002E  // 错误的或重复的标签点名
#define GoE_REDUPLICATE_PT              0xFFFF002F  // 重复的标签点标识
#define GoE_POINT_LICENSE_FULL          0xFFFF0030  // 标签点数超出了许可证规定的最大数目
#define GoE_TABLE_LICENSE_FULL          0xFFFF0031  // 标签点表个数超出了许可证规定的最大数目
#define GoE_WRONG_OR_DUPLIC_TABNAME     0xFFFF0032  // 错误的或重复的表名称
#define GoE_INVALID_FILE_FORMAT         0xFFFF0033  // 无效的数据文件格式
#define GoE_WRONG_TABNAME               0xFFFF0034  // 错误的表名称
#define GoE_WRONG_TAG                   0xFFFF0035  // 错误的标签点名
#define GoE_NOT_IN_SCOPE                0xFFFF0036  // 数值超出了应属的范围
#define GoE_CANT_LOAD_BASE              0xFFFF0037  // 不能同标签点信息服务取得联系
#define GoE_CANT_LOAD_SNAPSHOT          0xFFFF0038  // 不能同快照数据服务取得联系
#define GoE_CANT_LOAD_HISTORY           0xFFFF0039  // 不能同历史数据服务取得联系
#define GoE_CANT_LOAD_EQUATION          0xFFFF003A  // 不能同实施方程式服务取得联系
#define GoE_ARRAY_SIZE_NOT_MATCH        0xFFFF003B  // 数组尺寸不匹配
#define GoE_INVALID_HOST_ADDRESS        0xFFFF003C  // 无效的主机地址
#define GoE_CONNECT_FALSE               0xFFFF003D  // 连接已断开
#define GoE_TOOMANY_BYTES_RECVED        0xFFFF003E  // 接收到的数据长度超出了指定字节长度
#define GoE_REQID_RESPID_NOT_MATCH      0xFFFF003F  // 应答与请求的ID不一致
#define GoE_LESS_BYTES_RECVED           0xFFFF0040  // 接收到的数据长度小于指定字节长度
#define GoE_UNSUPPORTED_CALC_MODE       0xFFFF0041  // 不支持的计算模式
#define GoE_UNSUPPORTED_DATA_TYPE       0xFFFF0042  // 不支持的标签点类型
#define GoE_INVALID_EXPRESSION          0xFFFF0043  // 无效的表达式
#define GoE_INCOND_DATA_NOT_FOUND       0xFFFF0044  // 找不到符合条件的数据
#define GoE_VALID_DATA_NOT_FOUND        0xFFFF0045  // 找不到需要的有效数据
#define GoE_VALUE_OR_STATE_IS_NAN       0xFFFF0046  // 数据或状态不正常，为NAN
#define GoE_CREATE_MUTEX_FAILED         0xFFFF0047  // 创建互斥对象失败
#define GoE_TLSALLOCFAIL                0xFFFF0048  // 处理TLS时调用系统函数LocalAlloc()失败，可能因为内存不足导致
#define GoE_TO_MANY_POINTS              0xFFFF0049  // 正在调用的API函数不支持过多的标签点数量，请参考函数声明和开发手册
#define GoE_LIC_INFO_ERROR              0xFFFF004A  // 获取授权许可协议信息时发生错误
#define GoE_ARCHIVE_BUFFER_FULL         0xFFFF004B  // 标签点的历史数据补写缓冲区已满，请稍后再补
#define GoE_USER_NOT_EXIST              0xFFFF004C  // 用户不存在
#define GoE_USER_IS_DISABLED            0xFFFF004D  // 帐户被禁用
#define GoE_WRONG_PASSWORD              0xFFFF004E  // 错误的口令
#define GoE_ACCESS_IS_DENIED            0xFFFF004F  // 访问被拒绝，请确定是否具有足够的权限
#define GoE_HAVE_NOT_LOGIN              0xFFFF0050  // 您尚未登录，请先登录
#define GoE_USER_IS_DELETED             0xFFFF0051  // 帐户已被删除
#define GoE_USER_ALREADY_EXIST          0xFFFF0052  // 帐户已存在
#define GoE_WRONG_CREATE_TABNAME        0xFFFF0053  // 创建删除表失败
#define GoE_WRONG_FIELD_VALUE           0xFFFF0054  // 标签点属性值有错误
#define GoE_INVALID_TAG_ID              0xFFFF0055  // 无效的标签点ID

// v2.1 increased errors
#define GoE_NO_FREE_TABLE_ID            0xFFFF1001  // 没有空闲的表ID可用
#define GoE_NO_FREE_TAG_POSITION        0xFFFF1002  // 没有空闲的标签点位址可用
#define GoE_NO_FREE_SCAN_TAG_POSITION   0xFFFF1003  // 没有空闲的采集标签点位址可用
#define GoE_NO_FREE_CALC_TAG_POSITION   0xFFFF1004  // 没有空闲的计算标签点位址可用
#define GoE_INVALID_IPC_POSITION        0xFFFF1005  // 无效的位址被用于进程间内存共享
#define GoE_WRONG_IPC_POSITION          0xFFFF1006  // 错误的位址被用于进程间内存共享
#define GoE_IPC_ACCESS_EXCEPTION        0xFFFF1007  // 共享内存访问异常
#define GoE_ARV_PAGE_NOT_READY          0xFFFF1008  // 没有空闲的历史数据缓存页
#define GoE_ARVEX_PAGE_NOT_READY        0xFFFF1009  // 没有空闲的补历史数据缓存页
#define GoE_INVALID_POSITION_FROM_ID    0xFFFF100A  // 依据标签点ID获得的位址无效
#define GoE_NO_ACTIVE_PAGE_ALLOCATOR    0xFFFF100B  // 新的活动存档无法加载页分配器
#define GoE_MAP_IS_NOT_READY            0xFFFF100C  // 内存映射尚未就绪
#define GoE_FILE_MAP_FAILED             0xFFFF100D  // 文件映射到内存失败
#define GoE_TIME_RANGE_NOT_ALLOWED      0xFFFF100E  // 不允许使用的时间区间
#define GoE_NO_DATA_FOR_SUMMARY         0xFFFF100F  // 找不到用于统计的源数据
#define GoE_CANT_OPERATE_ON_ACTIVED     0xFFFF1010  // 不允许操作活动存档文件
#define GoE_SCAN_POINT_LICENSE_FULL     0xFFFF1011  // 采集标签点数超出了许可证规定的最大数目
#define GoE_CALC_POINT_LICENSE_FULL     0xFFFF1012  // 计算标签点数超出了许可证规定的最大数目
#define GoE_HISTORIAN_IS_SHUTTINGDOWN   0xFFFF1013  // 历史数据服务正在停止
#define GoE_SNAPSHOT_IS_SHUTTINGDOWN    0xFFFF1014  // 实时数据服务正在停止
#define GoE_EQUATION_IS_SHUTTINGDOWN    0xFFFF1015  // 实时方程式服务正在停止
#define GoE_BASE_IS_SHUTTINGDOWN        0xFFFF1016  // 标签点信息服务正在停止
#define GoE_SERVER_IS_SHUTTINGDOWN      0xFFFF1017  // 网络通信服务正在停止
#define GoE_OUT_OF_MEMORY               0xFFFF1018  // 内存不足
#define GoE_INVALID_PAGE                0xFFFF1019  // 无效的数据页，有可能是未加载
#define GoE_PAGE_IS_EMPTY               0xFFFF101A  // 遇到空的数据页
#define GoE_STR_OR_BLOB_TOO_LONG        0xFFFF101B  // 字符串或BLOB数据长度超出限制
#define GoE_CREATED_OR_OVERDUE          0xFFFF101C  // 尚未产生任何快照或快照已过期
#define GoE_ARCHIVE_INFO_NOT_MATCHING   0xFFFF101D  // 历史存档文件头部信息与实际不符
#define GoE_TIME_RANGE_OVERLAPPING      0xFFFF101E  // 指定的时间范围与已有存档文件重叠
#define GoE_CANNOT_SHIFT_TO_ACTIVED     0xFFFF101F  // 找不到合适的存档文件用于切换成活动存档
#define GoE_INDEX_NOT_READY             0xFFFF1020  // 历史存档文件对应的索引尚未就绪
#define GoE_INDEX_NODE_NOT_MATCH        0xFFFF1021  // 索引节点与指向的内容不符
#define GoE_CAN_NOT_CREATE_INDEX        0xFFFF1022  // 无法创建索引节点
#define GoE_CAN_NOT_REMOVE_INDEX        0xFFFF1023  // 无法删除索引节点
#define GoE_INVALID_FILTER_EXPRESS      0xFFFF1024  // 无效的过滤器表达式
#define GoE_MORE_VAR_IN_FILTER_EXP      0xFFFF1025  // 过滤器表达式中的包含了过多的变量
#define GoE_INVALID_ARV_PAGE_ALLOCATE   0xFFFF1026  // 刚分配的历史数据缓存页ID与标签点事件对象ID不匹配
#define GoE_INVALID_ARVEX_PAGE_ALLOCATE 0xFFFF1027  // 刚分配的补历史数据缓存页ID与标签点事件对象ID不匹配
#define GoE_BIG_JOB_IS_NOT_DONE         0xFFFF1028  // 正在执行重要的任务，请稍后再试
#define GoE_DATABASE_NEED_RESTART       0xFFFF1029  // 数据库需要重新启动以便应用新的参数
#define GoE_INVALID_TIME_FORMAT         0xFFFF102A  // 无效的时间格式字符串
#define GoE_DATA_PLAYBACK_DONE          0xFFFF102B  // 历史数据回放过程已结束
#define GoE_BAD_EQUATION                0xFFFF102C  // 错误的方程式
#define GoE_NOT_ENOUGH_SAPCE            0xFFFF102D  // 剩余磁盘空间不足
#define GoE_ACTIVED_ARCHIVE_EXIST       0xFFFF102E  // 已存在活动存档
#define GoE_ARCHIVE_HAVE_EX_FILES       0xFFFF102F  // 指定的存档文件具有附属文件
#define GoE_ARCHIVE_IS_NOT_LATEST       0xFFFF1030  // 指定的存档文件不是最晚的
#define GoE_DB_SYSTEM_NOT_RUNNING       0xFFFF1031  // 数据库管理系统尚未完全启动
#define GoE_ARCHIVE_IS_ALTERED          0xFFFF1032  // 存档文件内容发生变更
#define GoE_ARCHIVE_IS_TOO_SMALL        0xFFFF1033  // 不允许创建太小的存档文件和附属文件
#define GoE_INVALID_INDEX_NODE          0xFFFF1034  // 遇到无效的索引节点
#define GoE_MODIFY_SNAPSHOT_NOT_ALLOWED 0xFFFF1035  // 不允许删除或修改快照事件
#define GoE_SEARCH_INTERRUPTED          0xFFFF1036  // 因目标正被删除或恢复，搜索被迫中断，请稍后再试
#define GoE_RECYCLE_SHUTDOWN            0xFFFF1037  // 回收站已失效，相关操作无法完成
#define GoE_NEED_TO_REINDEX             0xFFFF1038  // 索引文件缺失，或部分索引节点被损坏，需要重建索引
#define GoE_INVALID_QUALITY             0xFFFF1039  // 无效的质量码
#define GoE_EQUATION_NOT_READY          0xFFFF103A  // 实时方程式服务正在解析，请稍后再试
#define GoE_ARCHIVES_LICENSE_FULL       0xFFFF103B  // 存档文件数已达到许可证规定的最大数目
#define GoE_RECYCLED_LICENSE_FULL       0xFFFF103C  // 标签点回收站容量超出了许可证规定的最大数目
#define GoE_STR_BLOB_LICENSE_FULL       0xFFFF103D  // 字符串或BLOB类型标签点数量超出了许可证规定的最大数目
#define GoE_NOT_SUPPORT_WHEN_DEBUG      0xFFFF103E  // 此功能被某个调试选项禁用
#define GoE_MAPPING_ALREADY_LOADED      0xFFFF103F  // 映射已经被加载，不允许重复加载
#define GoE_ARCHIVE_IS_MODIFIED         0xFFFF1040  // 存档文件被修改，动作被中断
#define GoE_ACTIVE_ARCHIVE_FULL         0xFFFF1041  // 活动文档已满
#define GoE_SPLIT_NO_DATA               0xFFFF1042  // 拆分数据页后所给时间区间内没有数据
#define GoE_INVALID_DIRECTORY           0xFFFF1043  // 指定的路径不存在或无效
#define GoE_ARCHIVE_LACK_EX_FILES       0xFFFF1044  // 指定存档文件的部分附属文件缺失
#define GoE_BIG_JOB_IS_CANCELED         0xFFFF1045  // 后台任务被取消

#define GoE_ARVEX_BLOB_PAGE_NOT_READY   0xFFFF1046  // 没有空闲的blob补历史数据缓存页
#define GoE_INVALID_ARVEX_BLOB_PAGE_ALLOCATE  0xFFFF1047  // 刚分配的blob补历史数据缓存页ID与标签点事件对象ID不匹配

#define GoE_TIMESTAMP_EQUALTO_SNAPSHOT                      0xFFFF1048  // 当前时间戳与快照时间戳相同
#define GoE_TIMESTAMP_NOT_GREATER_THAN_SNAPSHOT             0xFFFF1049  // 写入的快照的时间戳小于等于当前快照的时间戳
#define GoE_TIMESTAMP_GREATER_THAN_ALLOW                    0xFFFF104A  // 写入的快照的时间戳超过了允许的时间
#define GoE_TIMESTAMP_BEGINTIME_GREAGER_THAN_ENDTIME        0xFFFF104B  // 开始时间大于结束时间
#define GoE_TIMESTAMP_BEGINTIME_EQUALTO_ENDTIME             0xFFFF104C  // 开始时间等于结束时间

#define GoE_INVALID_COUNT                                   0xFFFF104D  //无效的count
#define GoE_INVALID_CAPACITY                                0xFFFF104E  //无效的capacity
#define GoE_INVALID_PATH                                    0xFFFF104F  //无效的path
#define GoE_INVALID_POSITION                                0xFFFF1050  //无效的position
#define GoE_INVALID_ARV_PAGE                                0xFFFF1051  //无效的golden_arv_page<GOLDEN_T>,未加载，或者size小于等于0
#define GoE_INVALID_HISINFO_ITEM_STATE                      0xFFFF1052
#define GoE_INVALID_INTERVAL                                0xFFFF1053
#define GoE_INVALID_LENGTH                                  0xFFFF1054  //无效的字符串长度
#define GoE_INVALID_SERACH_MODE                             0xFFFF1055  //无效的search mode
#define GoE_INVALID_FILE_ID                                 0xFFFF1056  //无效的存档文件ID
#define GoE_INVALID_MILLISECOND                             0xFFFF1057  //无效的毫秒值
#define GoE_INVALID_DEADLINE                                0xFFFF1058  //无效的截止时间
#define GoE_INVALID_JOBNAME                                 0xFFFF1059  //
#define GoE_INVALID_JOBSTATE                                0xFFFF105A  //
#define GoE_INVALID_PROCESS_RATE                            0xFFFF105B  //




#define GoE_INVALID_TABLE_ID                            0xFFFF105C  // 无效的表ID
#define GoE_INVALID_DATA_SOURCE                         0xFFFF105D  // 无效的数据源格式
#define GoE_INVALID_TRIGGER_METHOD                      0xFFFF105E  // 无效的触发方式
#define GoE_INVALID_CALC_TIME_RES                       0xFFFF105F  // 无效的计算结果时间戳参考方式
#define GoE_INVALID_TRIGGER_TIMER                       0xFFFF1060  // 无效的定时触发触发周期,不能小于1秒
#define GoE_INVALID_LIMIT                               0xFFFF1061  // 工程上限不得低于工程下限
#define GoE_INVALID_COMP_TIME                           0xFFFF1062  // 无效的研所间隔，最长压缩间隔不得小于最短压缩间隔
#define GoE_INVALID_EXT_TIME                            0xFFFF1063  // 无效的例外间隔，最长例外间隔不得小于最短例外间隔
#define GoE_INVALID_DIGITS                              0xFFFF1064  // 无效的数值位数，数值位数超出了范围,-20~10
#define GoE_INVALID_FULL_TAG_NAME                       0xFFFF1065  // 标签点全名有误，找不到表名与点名的分隔符“.”
#define GoE_INVALID_TABLE_DESC                          0xFFFF1066  // 表描述信息过有误
#define GoE_INVALID_USER_COUNT                          0xFFFF1067  // 非法的用户个数，小于0
#define GoE_INVALID_BLACKLIST_COUNT                     0xFFFF1068  // 非法的黑名单个数，小于0
#define GoE_INVALID_AUTHORIZATION_COUNT                 0xFFFF1069  // 非法的信任连接个数，小于0
#define GoE_INVALID_BIG_JOB_TYPE                        0xFFFF106A  // 非法的大任务类型
#define GoE_INVALID_SYS_PARAM                           0xFFFF106B  // 无效的系统参数，调用db_set_db_info2时，参数有误
#define GoE_INVALID_FILE_PARAM                          0xFFFF106C  // 无效的文件路径参数，调用db_set_db_info1时，参数有误
#define GOE_INVALID_FILE_SIZE                           0xFFFF106D  // 文件长度有误  < 1 baserecycle.dat、scanrecycle.dat、calcrecycle.dat、snaprecycle.dat
#define GoE_INVALID_TAG_TYPE                            0xFFFF106E  // 标签点类型有误，合法（ golden_bool ~ golden_blob)，但是不属于相应函数的处理范围
#define GOE_INVALID_RECY_STRUCT_POS                     0xFFFF106F  // 回收站对象最后一个结构体位置非法
#define GoE_INVALID_RECYCLE_FILE                        0xFFFF1070  // scanrecycle.dat、baserecycle.dat  snaprecycle.dat文件不存在或失效
#define GoE_INVALID_SUFFIX_NAME                         0xFFFF1071  // 无效的存档文件后缀名

#define GoE_INSERT_STRING_FALSE                         0xFFFF1072  // 向数据页中插入字符串数据失败
#define GoE_BLOB_PAGE_FULL                              0xFFFF1073  // blob数据页已满



#define GoE_PROTOCOLNOTIMPL             0xFFFF2001  // 用户请求的报文未实现
#define GoE_CRCERROR                    0xFFFF2002  // 报文CRC校验错误
#define GoE_WRONG_USERPW                0xFFFF2003  // 验证用户名密码失败
#define GoE_CHANGE_USERPW               0xFFFF2004  // 修改用户名密码失败
#define GoE_INVALID_HANDLE              0xFFFF2005  // 无效的句柄
#define GoE_INVALID_SOCKET_HANDLE       0xFFFF2006  // 无效的套接字句柄
#define GoE_FALSE                       0xFFFF2007  // 操作未成功完成，具体原因查看小错误码。
#define GoE_SCAN_POINT_NOT_FOUND        0xFFFF2008  // 要求访问的采集标签点不存在或无效
#define GoE_CALC_POINT_NOT_FOUND        0xFFFF2009  // 要求访问的计算标签点不存在或无效
#define GoE_REDUPLICATE_ID              0xFFFF200A  // 重复的标签点标识
#define GoE_HANDLE_SUBSCRIBED           0xFFFF200B  // 句柄已经被订阅
#define GoE_OTHER_SDK_DOING             0xFFFF200C  // 另一个API正在执行
#define GoE_BATCH_END                   0xFFFF200D  // 分段数据返回结束
#define GoE_AUTH_NOT_FOUND              0xFFFF200E  // 信任连接段不存在
#define GoE_AUTH_EXIST                  0xFFFF200F  // 连接地址段已经位于信任列表中
#define GoE_AUTH_FULL                   0xFFFF2010  // 信任连接段已满
#define GoE_USER_FULL                   0xFFFF2011  // 用户已满
#define GoE_VERSION_UNMATCH             0xFFFF2012  // 报文或数据版本不匹配
#define GoE_INVALID_PRIV                0xFFFF2013  // 无效的权限
#define GoE_INVALID_MASK                0xFFFF2014  // 无效的子网掩码
#define GoE_INVALID_USERNAME            0xFFFF2015  // 无效的用户名
#define GoE_INVALID_MARK                0xFFFF2016  // 无法识别的报文头标记
#define GoE_UNEXPECTED_METHOD           0xFFFF2017  // 意外的消息 ID
#define GoE_INVALID_PARAM_INDEX         0xFFFF2018  // 无效的系统参数索引值
#define GoE_DECODE_PACKET_ERROR         0xFFFF2019  // 解包错误
#define GoE_ENCODE_PACKET_ERROR         0xFFFF201A  // 编包错误
#define GoE_BLACKLIST_FULL              0xFFFF201B  // 阻止连接段已满
#define GoE_BLACKLIST_EXIST             0xFFFF201C  // 连接地址段已经位于黑名单中
#define GoE_BLACKLIST_NOT_FOUND         0xFFFF201D  // 阻止连接段不存在
#define GoE_IN_BLACKLIST                0xFFFF201E  // 连接地址位于黑名单中，被主动拒绝
#define GoE_INCREASE_FILE_FAILED        0xFFFF201F  // 试图增大文件失败
#define GoE_RPC_INTERFACE_FAILED        0xFFFF2020  // 远程过程接口调用失败

// Windows Socket Errors
#define GoE_NET_ERROR                   0xFFFF7000  // 网络错误的起始值
#define GoE_SOCK_WSAEINTR               0xFFFF7004  // （阻塞）调用被 WSACancelBlockingCall() 函数取消
#define GoE_SOCK_WSAEACCES              0xFFFF700D  // 请求地址是广播地址，但是相应的 flags 没设置
#define GoE_SOCK_WSAEFAULT              0xFFFF700E  // 非法内存访问
#define GoE_SOCK_WSAEMFILE              0xFFFF7018  // 无多余的描述符可用
#define GoE_SOCK_WSAEWOULDBLOCK         0xFFFF7023  // 套接字被标识为非阻塞，但操作将被阻塞
#define GoE_SOCK_WSAEINPROGRESS         0xFFFF7024  // 一个阻塞的 Windows Sockets 操作正在进行
#define GoE_SOCK_WSAEALREADY            0xFFFF7025  // 一个非阻塞的 connect() 调用已经在指定的套接字上进行
#define GoE_SOCK_WSAENOTSOCK            0xFFFF7026  // 描述符不是套接字描述符
#define GoE_SOCK_WSAEDESTADDRREQ        0xFFFF7027  // 要求（未指定）目的地址
#define GoE_SOCK_WSAEMSGSIZE            0xFFFF7028  // 套接字为基于消息的，消息太大（大于底层传输支持的最大值）
#define GoE_SOCK_WSAEPROTOTYPE          0xFFFF7029  // 对此套接字来说，指定协议是错误的类型
#define GoE_SOCK_WSAEPROTONOSUPPORT     0xFFFF702B  // 不支持指定协议
#define GoE_SOCK_WSAESOCKTNOSUPPORT     0xFFFF702C  // 在此地址族中不支持指定套接字类型
#define GoE_SOCK_WSAEOPNOTSUPP          0xFFFF702D  // MSG_OOB 被指定，但是套接字不是流风格的
#define GoE_SOCK_WSAEAFNOSUPPORT        0xFFFF702F  // 不支持指定的地址族
#define GoE_SOCK_WSAEADDRINUSE          0xFFFF7030  // 套接字的本地地址已被使用
#define GoE_SOCK_WSAEADDRNOTAVAIL       0xFFFF7031  // 远程地址非法
#define GoE_SOCK_WSAENETDOWN            0xFFFF7032  // Windows Sockets 检测到网络系统已经失效
#define GoE_SOCK_WSAENETUNREACH         0xFFFF7033  // 网络无法到达主机
#define GoE_SOCK_WSAENETRESET           0xFFFF7034  // 在操作进行时 keep-alive 活动检测到一个失败，连接被中断
#define GoE_SOCK_WSAECONNABORTED        0xFFFF7035  // 连接因超时或其他失败而中断
#define GoE_SOCK_WSAECONNRESET          0xFFFF7036  // 连接被复位
#define GoE_SOCK_WSAENOBUFS             0xFFFF7037  // 无缓冲区空间可用
#define GoE_SOCK_WSAEISCONN             0xFFFF7038  // 连接已建立
#define GoE_SOCK_WSAENOTCONN            0xFFFF7039  // 套接字未建立连接
#define GoE_SOCK_WSAESHUTDOWN           0xFFFF703A  // 套接字已 shutdown，连接已断开
#define GoE_SOCK_WSAETIMEDOUT           0xFFFF703C  // 连接请求超时，未能建立连接
#define GoE_SOCK_WSAECONNREFUSED        0xFFFF703D  // 连接被拒绝
#define GoE_SOCK_WSANOTINITIALISED      0xFFFF705D  // Windows Sockets DLL 未初始化


#endif /* __GOLDEN_ERROR_H__ */
