#ifndef __GOLDEN_H__
#define __GOLDEN_H__

#ifdef WIN32
#  ifdef GOLDENAPI_EXPORTS
#    define GOLDENAPI extern "C" __declspec(dllexport)
#  else
#    define GOLDENAPI extern "C" __declspec(dllimport)
#  endif 
#  define GOLDENAPI_CALLRULE _stdcall
#else //WIN32
#  define GOLDENAPI 
#  define GOLDENAPI_CALLRULE 
#  define MAKELONG(l, h) ((long)((h) << 16) | (l))
#endif


#ifdef SOLARIS
#define GOLDENAPI extern "C" 
#define GOLDENAPI_CALLRULE 
#endif //SOLARIS

/// 二维坐标定义
typedef struct _GOLDEN_COORDINATE
{
  float x;
  float y;
} GOLDEN_COORDINATE;

typedef unsigned char       golden_byte;          /// 单字节数值
typedef char                golden_int8;          /// 8位整数
typedef unsigned char       golden_uint8;         /// 8位正整数
typedef short               golden_int16;         /// 16位整数
typedef unsigned short      golden_uint16;        /// 16位正整数
typedef int                 golden_int32;         /// 32位整数
typedef unsigned int        golden_uint32;        /// 32位正整数
typedef long long           golden_int64;         /// 64位整数
typedef short               golden_float16;       /// 16位浮点数
typedef float               golden_float32;       /// 32位浮点数
typedef double              golden_float64;       /// 64位浮点数
typedef GOLDEN_COORDINATE   golden_coordinate;    /// 二维坐标
typedef unsigned int        golden_error;         /// 错误数值

/// 0x0200 以内的系统定义质量码
enum GOLDEN_QUALITY
{
  GOLDEN_Q_GOOD     = 0,    // 正常
  GOLDEN_Q_NODATA   = 1,    // 无数据
  GOLDEN_Q_CREATED  = 2,    // 创建
  GOLDEN_Q_SHUTDOWN = 3,    // 停机
  GOLDEN_Q_CALCOFF  = 4,    // 计算停止
  GOLDEN_Q_BAD      = 5,    // 坏点
  GOLDEN_Q_DIVBYZERO= 6,    // 被零除
  GOLDEN_Q_REMOVED  = 7,    // 已被删除
  GOLDEN_Q_OPC      = 256,  // 从0x0100至0x01FF为OPC质量码
  GOLDEN_Q_USER     = 512   // 此质量码（含）之后为用户自定义
};

/// 为质量码增加或去除 UPDATE 标志，U 为 true 增加 UPDATE 标志，否则去除 UPDATE 标志
#define GOLDEN_QUALITY_MODIFY_UPDATE_FLAG(Q, U)   (U) ? (Q) |= 0x8000 : (Q) &= 0x7FFF;

/// 根据质量码判定事件是否被修改过
#define GOLDEN_QUALITY_IS_UPDATED(Q)              ( (Q) & 0x8000 )

/// 获得不包含 UPDATE 标志的质量码
#define GOLDEN_QUALITY_WITHOUT_FLAG(Q)            ( (Q) & 0x7FFF )

/// OPC协议中正常有效数据的质量
#define GOLDEN_QUALITY_OPC_GOOD                   0xC0

/// 从OPC协议获取的质量码，使用此宏转换为GOLDEN中对应的质量码
#define GOLDEN_QUALITY_FROM_OPC(OPC_Q)            ( GOLDEN_QUALITY_OPC_GOOD == (OPC_Q) ? GOLDEN_Q_GOOD : (OPC_Q) & 0xFF | GOLDEN_Q_OPC )

/// 判定GOLDEN中的质量码是否属于OPC协议范围
#define GOLDEN_QUALITY_IS_OPC(Q)                  ( GOLDEN_QUALITY_WITHOUT_FLAG(Q) >= GOLDEN_Q_OPC && GOLDEN_QUALITY_WITHOUT_FLAG(Q) < GOLDEN_Q_USER )

/// GOLDEN中属于OPC协议范围的质量码，使用此宏还原为OPC原始质量码
#define GOLDEN_QUALITY_TO_OPC(Q)                  ( GOLDEN_Q_GOOD == GOLDEN_QUALITY_WITHOUT_FLAG(Q) ? GOLDEN_QUALITY_OPC_GOOD : (Q) & 0xFF )

/// 依据质量判定对应的事件是否正常有效
#define GOLDEN_QUALITY_IS_VALID(Q)                ( GOLDEN_Q_GOOD == GOLDEN_QUALITY_WITHOUT_FLAG(Q) )

/// 系统常数定义
enum GOLDEN_CONST
{ 
  GOLDEN_TAG_SIZE = 80,                                 /// 标签点名称占用字节数。
  GOLDEN_DESC_SIZE = 100,                               /// 标签点描述占用字节数。
  GOLDEN_UNIT_SIZE = 20,                                /// 标签点单位占用字节数。
  GOLDEN_USER_SIZE = 20,                                /// 用户名占用字节数。
  GOLDEN_SOURCE_SIZE = 256,                             /// 标签点数据源占用字节数。
  GOLDEN_INSTRUMENT_SIZE = 50,                          /// 标签点所属设备占用字节数。
  GOLDEN_LOCATIONS_SIZE = 5,                            /// 采集标签点位址个数。
  GOLDEN_USERINT_SIZE = 2,                              /// 采集标签点用户自定义整数个数。
  GOLDEN_USERREAL_SIZE = 2,                             /// 采集标签点用户自定义浮点数个数。
  GOLDEN_EQUATION_SIZE = 2036,                          /// 计算标签点方程式占用字节数。
  GOLDEN_PACK_OF_SNAPSHOT = 7,                          /// 事件快照备用字节空间。
  //GOLDEN_PACK_OF_SNAPSHOT = 3,                          /// 事件快照备用字节空间。
  GOLDEN_PACK_OF_POINT = 11,                            /// 标签点备用字节空间。
  GOLDEN_PACK_OF_BASE_POINT = 76,                       /// 基本标签点备用字节空间。
  GOLDEN_PACK_OF_SCAN = 164,                            /// 采集标签点备用字节空间。
  GOLDEN_PACK_OF_CALC = 0,                              /// 计算标签点备用字节空间。
  GOLDEN_FILE_NAME_SIZE = 64,                           /// 文件名字符串字节长度
  GOLDEN_PATH_SIZE = 1024 - 4 - GOLDEN_FILE_NAME_SIZE,  /// 路径字符串字节长度
  GOLDEN_MAX_USER_COUNT  = 100 ,                        /// 最大用户个数
  GOLDEN_MAX_AUTH_COUNT = 100,                          /// 最大信任连接段个数
  GOLDEN_MAX_BLACKLIST_LEN = 100,                       /// 连接黑名单最大长度
  GOLDEN_MAX_SUBSCRIBE_SNAPSHOTS = 1000,                /// 单个连接最大订阅快照数量
};

/// 标签点数值类型，决定了标签点数值所占用的存储字节数。
enum GOLDEN_TYPE
{ 
  GOLDEN_BOOL,      /// 布尔类型，0值或1值。
  GOLDEN_UINT8,     /// 无符号8位整数，占用1字节。
  GOLDEN_INT8,      /// 有符号8位整数，占用1字节。
  GOLDEN_CHAR,      /// 单字节字符，占用1字节。
  GOLDEN_UINT16,    /// 无符号16位整数，占用2字节。
  GOLDEN_INT16,     /// 有符号16位整数，占用2字节。
  GOLDEN_UINT32,    /// 无符号32位整数，占用4字节。
  GOLDEN_INT32,     /// 有符号32位整数，占用4字节。
  GOLDEN_INT64,     /// 有符号64位整数，占用8字节。
  GOLDEN_REAL16,    /// 16位浮点数，占用2字节。
  GOLDEN_REAL32,    /// 32位单精度浮点数，占用4字节。
  GOLDEN_REAL64,    /// 64位双精度浮点数，占用8字节。
  GOLDEN_COOR,      /// 二维坐标，具有x、y两个维度的浮点数，占用8字节。
  GOLDEN_STRING,    /// 字符串，长度不超过存储页面大小。
  GOLDEN_BLOB,      /// 二进制数据块，占用字节不超过存储页面大小。
};

#define GOLDEN_TYPE_IS_NORMAL(TYPE)    ( (TYPE == GOLDEN_STRING || TYPE == GOLDEN_BLOB) ? false : true )

/// 标签点类别，决定了标签点具有哪些扩展属性。标签点可以同时具备多个类别，最多可以定义33个标签点类别。
enum GOLDEN_CLASS
{ 
  GOLDEN_BASE = 0,    /// 基本标签点，所有类别标签点均在基本标签点的属性集上扩展自己的属性集。
  GOLDEN_SCAN = 1,    /// 采集标签点。
  GOLDEN_CALC = 2,    /// 计算标签点。
  GOLDEN_ALARM = 4,   /// 报警标签点。
};

/// 判定标签点是否采集标签点
#define GOLDEN_CLASS_IS_SCAN(CLASSOF)    (CLASSOF & GOLDEN_SCAN)

/// 判定标签点是否计算标签点
#define GOLDEN_CLASS_IS_CALC(CLASSOF)    (CLASSOF & GOLDEN_CALC)

/// 判定标签点是否报警标签点
#define GOLDEN_CLASS_IS_ALARM(CLASSOF)    (CLASSOF & GOLDEN_ALARM)

/// 计算标签点触发机制。
enum GOLDEN_TRIGGER
{ 
  GOLDEN_NULL_TRIGGER,        /// 无触发
  GOLDEN_EVENT_TRIGGER,       /// 事件触发
  GOLDEN_TIMER_TRIGGER,       /// 周期触发
};

/// 计算结果时间戳参考。
enum GOLDEN_TIME_COPY
{ 
  GOLDEN_CALC_TIME,             ///采用计算时间
  GOLDEN_LATEST_TIME,           ///采用最晚标签点时间
  GOLDEN_EARLIEST_TIME,         ///采用最早标签点时间
};

/// 标签点搜索结果排序方式
enum GOLDEN_SEARCH_SORT
{ 
  GOLDEN_SORT_BY_TABLE,     /// 首先按所属表排序，同一个表内的标签点之间按标签点名称排序
  GOLDEN_SORT_BY_TAG,       /// 以标签点名称排序
  GOLDEN_SORT_BY_ID,        /// 以标签点ID排序
};

/// 历史数据搜索方式
enum GOLDEN_HIS_MODE 
{
  GOLDEN_NEXT,            ///寻找下一个最近的数据；
  GOLDEN_PREVIOUS,        ///寻找上一个最近的数据；
  GOLDEN_EXACT,           ///取指定时间的数据，如果没有则返回错误 GoE_DATA_NOT_FOUND；
  GOLDEN_INTER,           ///取指定时间的内插值数据。
};

enum GOLDEN_API_OPTION
{
  GOLDEN_API_AUTO_RECONN,     /// api 在连接中断后是否自动重连, 0 不重连；1 重连。默认为 0 不重连
}; 

enum GOLDEN_PROCESS_NAME
{
  GOLDEN_PROCESS_FIRST = 1, 
  GOLDEN_PROCESS_HISTORIAN = GOLDEN_PROCESS_FIRST, 
  GOLDEN_PROCESS_EQUATION, 
  GOLDEN_PROCESS_BASE, 
  GOLDEN_PROCESS_LAST, 
}; 

/// 大任务
enum GOLDEN_BIG_JOB_NAME
{
  /// 历史数据服务
  GOLDEN_MERGE    = 1,         ///合并附属文件到主文件；
  GOLDEN_ARRANGE  = 2,         ///整理存档文件，整理过程中会完成合并
  GOLDEN_REINDEX  = 3,         ///重建索引；
  GOLDEN_BACKUP   = 4,         ///备份。
  GOLDEN_REACTIVE = 5,         ///激活为活动存档。
  /// 方程式服务
  GOLDEN_COMPUTE  = 11,        ///历史计算
  /// 标签点信息服务
  GOLDEN_UPDATE_TABLE = 21,    ///修改表名称
  GOLDEN_REMOVE_TABLE = 22,    ///删除表
};

/// 标签点索引表属性集。
typedef struct _GOLDEN_TABLE
{
  /**
  * 表的唯一标识。
  * 从1开始，到上限为止。
  */
  int  id;
  /**
  * 表类型。
  * 暂时保留用途。
  */
  int  type;
  /** 表名称。
  *  命名规则：
  *  1、第一个字符必须是26个字母之一或数字0-9之一；
  *  2、不允许使用控制字符，比如换行符或制表符；
  *  3、不允许使用以下字符（'*'、'''、'?'、';'、'{'、'}'、'['、']'、'|'、'\'、'`'、'''、'"'、'.'）；
  *  4、字节长度不要超出 GOLDEN_TAG_SIZE，如果超出，系统会自动将后面的字符截断。
  */
  char name[GOLDEN_TAG_SIZE];
  /**
  *  表描述。
  *  缺省值：空字符串。
  *  字节长度不要超出 GOLDEN_DESC_SIZE，多余的部分会被截断。
  */
  char desc[GOLDEN_DESC_SIZE];
} GOLDEN_TABLE;

/// 基本标签点属性集。
/**
 * 基本标签点属性集。8字节对齐的条件下占用512字节。
 */
typedef struct _GOLDEN_POINT
{
  /**
  *  标签点名称。
  *  用于在表中唯一标识一个标签点；
  *  该属性允许被修改；
  *  命名规则：
  *  1、第一个字符必须是26个字母之一或数字0-9之一或者"_"、"%"；
  *  2、不允许使用控制字符，比如换行符或制表符；
  *  3、不允许使用以下字符（'*'、'''、'?'、';'、'('、')'、'{'、'}'、'['、']'、'|'、'\'、'`'、'''、'"'、'.'）；
  *  4、字节长度不要超出79，如果超出，系统会自动将后面的字符截断。
  */
  char tag[GOLDEN_TAG_SIZE];
  /**
  *  全库唯一标识。
  *  只读属性，创建标签点时系统会自动为标签点分配的唯一标识，即标签点ID，标签点ID一经创建后永不更改。
  */
  int id;
  /**
  *  标签点的数值类型。
  *  只读属性，在创建标签点时指定。
  */
  int type;
  /**
  *  标签点所属表ID。
  */
  int table;
  /**
  *  有关标签点的描述性文字。
  *  字节长度不要超出99，多余的部分会被截断。
  */
  char desc[GOLDEN_DESC_SIZE];
  /**
  *  工程单位。
  *  字节长度不要超出19，多余的部分会被截断。
  */
  char unit[GOLDEN_UNIT_SIZE];
  /**
  *  是否存档。
  *  缺省值：ON，1；
  *  ON或1表示存档，OFF或0表示不存档。
  */
  golden_byte archive;
  /**
  *  数值位数。
  *  缺省值：-5；
  *  范围：>=-20、<=10；
  *  用来控制数值的显示格式；
  *  如果为0或正数，表示数值的小数位数，如果为负数，表示数值的有效位数。
  */
  short digits;
  /**
  *  停机状态字（Shutdown）
  *  缺省值：0；
  *  定义该点在停机状态下是否补写停机状态值。
  *  1 表示补写；0 表示不补写。
  */
  golden_byte shutdown;
  /**
  *  量程下限。
  *  缺省值：0；
  *  单位：标签点工程单位。
  */
  float lowlimit;
  /**
  *  量程上限。
  *  缺省值：100；
  *  单位：标签点工程单位。
  */
  float highlimit;
  /**
  *  是否阶跃。
  *  缺省值：OFF，0；
  *  该属性决定了中间值的计算是用阶梯还是连续的内插值替换；
  *  缺省情况下该属性为OFF，即中间值的计算是用内插值替换；
  *  如果被设置为ON，则中间值的数值同前一个有记录的数值相同。
  *  在历史数据检索中，本设置可能被外部输入的阶跃开关覆盖。
  */
  golden_byte step;
  /**
  *  典型值。
  *  缺省值：50；
  *  大于等于量程下限，小于等于量程上限。
  */
  float typical;
  /**
  *  是否压缩。
  *  缺省值：ON，1；
  *  如果该属性被关闭（OFF，0），任何到达数据存储服务器Server的数据都会被提交到历史数据库；否则（ON，1），只有满足压缩条件的数据才会被提交到历史数据库。
  *  需要手工录入的标签点应该将该属性设置为OFF，0。
  */
  golden_byte compress;
  /**
  *  压缩偏差。
  *  单位：标签点工程单位；
  *  缺省值：1；
  *  当有新的数值被提交到数据存储服务器Server，如果从上一个被提交到历史数据库的数值开始有数值超出了压缩偏差外，则上一个被提交到数据存储服务器Server的数值被视为关键数值；
  *  该属性与[压缩偏差百分比（the percent of compression deviation）]属性含义相同，该属性与量程（high
  *  limit - low limit）的百分比即[压缩偏差百分比（the percent of compression
  *  deviation）]属性的值；
  *  对该属性的修改将导致对[压缩偏差百分比（the percent of compression
  *  deviation）]的修改，同样，对[压缩偏差百分比（the percent of compression
  *  deviation）]的修改也将修改该属性，如果两个同时被修改，[压缩偏差百分比（the percent of compression
  *  deviation）]具有更高的优先权。
  */
  float compdev;
  /**
  *  压缩偏差百分比。
  *  单位：百分比；
  *  参考：[压缩偏差]。
  */
  float compdevpercent;
  /**
  *  最大压缩间隔。
  *  单位：秒；
  *  缺省值：28800；
  *  如果某个数值与上一个被提交到历史数据库的数值的时间间隔大于或等于最大压缩间隔，无论是否满足压缩条件，该数值都应该被视为关键数值从而被提交到历史数据库的数据队列；
  *  数据库中两个标签点间的时间间隔有可能超出该属性值，因为数据存储服务器Server可能长时间接收不到提交的数据，而且任何系统绝对不会自己创造数据。
  */
  int comptimemax;
  /**
  *  最短压缩间隔。
  *  单位：秒；
  *  缺省值：0；
  *  如果某个数值与上一个被提交到历史数据库的数值的时间间隔小于最短压缩间隔，该数值会被忽略；
  *  该属性有降噪（suppress noise）的作用。
  */
  int comptimemin;
  /**
  *  例外偏差。
  *  单位：标签点工程单位；
  *  缺省值：0.5；
  *  如果某个数值与上一个被提交到数据存储服务器Server的数值的偏差大于该例外偏差（以数值的工程单位为准），则该数值被视为例外数值，应该被提交到数据存储服务器Server；
  *  建议例外偏差应该小于等于压缩偏差的一半；
  *  该属性与[例外偏差百分比（The Percent of Exception Deviation）]属性含义相同，该属性与量程（high
  *  limit - low limit）的百分比即[例外偏差百分比（The Percent of Exception
  *  Deviation）]属性的值；
  *  对该属性的修改将导致对[例外偏差百分比（The Percent of Exception
  *  Deviation）]的修改，同样，对[例外偏差百分比（The Percent of Exception
  *  Deviation）]的修改也将修改该属性，如果两个同时被修改，[例外偏差百分比（The Percent of Exception
  *  Deviation）]具有更高的优先权。
  */
  float excdev;
  /**
  *  例外偏差百分比。
  *  单位：百分比；
  *  参考：[例外偏差]。
  */
  float excdevpercent;
  /**
  *  最大例外间隔。
  *  单位：秒；
  *  缺省值：600；
  *  如果某个数值与上一个被提交到数据存储服务器Server的数值的时间间隔大于或等于最大例外间隔，无论是否满足例外条件，该数值都应该被视为例外数值从而被提交到数据存储服务器Server；
  *  数据库中两个标签点间的时间间隔有可能超出该属性值，因为接口可能长时间采集不到数据，而且任何系统绝对不会自己创造数据；
  *  如果要关闭例外过滤，设置该属性为0即可。
  */
  int exctimemax;
  /**
  *  最短例外间隔。
  *  单位：秒；
  *  缺省值：0；
  *  如果某个数值与上一个被提交到数据存储服务器Server的数值的时间间隔小于最短例外间隔，无论是否满足例外条件，该数值都会被忽略；
  *  该属性有降噪（suppress noise）的作用。
  */
  int exctimemin;
  /**
  *  标签点类别。
  *  GOLDEN_CLASS类型的组合，最多可以扩展至32种类型的组合；
  *  所有类别标签点均继承自"基本"类型标签点。
  *  不同类别的标签点具有不同的属性集，"采集"类别的标签点具有"设备标签"、"位号"、"自定义整数"和"自定义浮点数"等扩展属性，"计算"类别的标签点具有"扩展描述"、"触发机制"等扩展属性。
  */
  unsigned int classof;
  /**
  *  标签点属性最后一次被修改的时间。
  */
  int changedate;
  /**
  *  标签点属性最后一次被修改的用户名。
  */
  char changer[GOLDEN_USER_SIZE];
  /**
  *  标签点被创建的时间。
  */
  int createdate;
  /**
  *  标签点创建者的用户名。
  */
  char creator[GOLDEN_USER_SIZE];
  /**
  *  允许镜像。
  *  默认值：OFF，0
  *  一旦启用该属性为ON，1，则表示该标签点为镜像同步点，如果处于镜像发送端，该标签点的所有更新会被同步发送至镜像接收端；如果处于镜像接收端，该标签点的实时值与发送端同步更新。
  */
  golden_byte mirror;
  /**
  *  时间戳精度。
  *  默认值：秒，0；
  *  用于设定标签点的历史值在存储中精确到"秒"（0）还是"毫秒"（1）。
  *  标签点一经创建就不允许修改该属性。
  */
  golden_byte millisecond;
  /**
  *  采集点扩展属性集存储地址索引。
  */
  unsigned int scanindex;
  /**
  *  计算点扩展属性集存储地址索引。
  */
  unsigned int calcindex;
  /**
  *  报警点扩展属性集存储地址索引。
  */
  unsigned int alarmindex;
  /**
  *  标签点全名，格式为“表名称.标签点名称”。
  */
  char table_dot_tag[GOLDEN_TAG_SIZE + GOLDEN_TAG_SIZE];
  /** 
  * 统计加速。
  * 默认值：关，0；
  * 用于设定是否生成标签点统计信息，从而加速历史数据统计过程。
  */ 
  golden_byte summary; 
  /**
  *  基本标签点备用字节。
  */
  golden_byte padding[GOLDEN_PACK_OF_POINT];
} GOLDEN_POINT;

/// 采集标签点扩展属性集。
/**
 *  采集标签点扩展属性集。8字节对齐条件下占用512字节。
 */
typedef struct _GOLDEN_SCAN_POINT
{
  /**
  *  全库唯一标识。0表示无效。
  */
  int id;
  /**
  *  数据源。
  *  缺省值：空（NULL）；
  *  将标签点同某些接口或某些模块相关联；
  *  每个数据源字符串只允许由26个字母（大小写敏感）和数字（0-9）组成，字节长度不要超出255，多余的部分会被截断。
  */
  char source[GOLDEN_SOURCE_SIZE];
  /**
  *  是否采集。
  *  缺省值：ON，1；
  *  该属性可能会被某些接口用到，如果该属性被关闭（OFF，0），从接口传来的数据可能不会被报告到数据库。
  */
  golden_byte scan;
  /**
  *  设备标签。
  *  缺省值：空（NULL）；
  *  字节长度不要超出49，多余的部分会被截断。
  */
  char instrument[GOLDEN_INSTRUMENT_SIZE];
  /**
  *  共包含五个设备位址，缺省值全部为0。
  */
  int locations[GOLDEN_LOCATIONS_SIZE];
  /**
  *  共包含两个自定义整数，缺省值全部为0。
  */
  int userints[GOLDEN_USERINT_SIZE];
  /**
  *  共包含两个自定义单精度浮点数，缺省值全部为0。
  */
  float userreals[GOLDEN_USERREAL_SIZE];
  /**
  *  采集标签点备用字节。
  */
  golden_byte padding[GOLDEN_PACK_OF_SCAN];
} GOLDEN_SCAN_POINT;

/// 计算点扩展属性集。
/**
*  计算点扩展属性集。8字节对齐条件下占用2048字节。
*/
typedef struct _GOLDEN_CALC_POINT
{
  /**
  *  全库唯一标识。0表示无效。
  */
  int id;
  /**
  *  实时方程式。
  *  缺省值：空（NULL）；
  *  字节长度不要超出2047，长度超长的方程式将被拒绝设置入库，返回一个错误，避免错误的方程式进入系统，引发不安全因素。
  */
  char equation[GOLDEN_EQUATION_SIZE];
  /**
  *  计算触发机制。枚举值参见 GOLDEN_TRIGGER。
  *  仅对"计算"类别标签点起作用，用于设置实时方程式服务对单个计算点的计算触发采用"事件触发"还是"周期触发"，
  *  对于"周期触发"以"事件触发"作为其先决判断条件，如果"事件触发"不满足，则不进行"周期触发"。
  */
  golden_byte trigger;
  /**
  *  计算结果时间戳参考, 枚举值参见 GOLDEN_TIME_COPY
  *  0: 表示采用计算时间作为计算结果时间戳；
  *  1: 表示采用输入标签点中的最晚时间作为计算结果时间戳；
  *  2: 表示采用输入标签点中的最早时间作为计算结果时间戳。
  */
  golden_byte timecopy;
  /**
  *  对于“周期触发”的计算点，设定其计算周期，单位：秒。
  */
  int period;
  // 计算标签点备用字节。
  // golden_byte padding[GOLDEN_PACK_OF_CALC];
} GOLDEN_CALC_POINT;

/// 标签点派别结构
typedef union _GOLDEN_TAG_FACTION
{
  unsigned int faction;
  struct
  {
    golden_byte type;
    golden_byte usems;
    golden_byte reserved1;
    golden_byte reserved2;
  } factions;
} GOLDEN_TAG_FACTION;

/**
*  用户权限
*/
enum GOLDEN_PRIV_GROUP
{
  GOLDEN_RO,      /// 只读
  GOLDEN_DW,      /// 数据记录
  GOLDEN_TA,      /// 标签点表管理员
  GOLDEN_SA,      /// 数据库管理员
};

/**
*  标签点变更原因
*/
enum GOLDEN_TAG_CHANGE_REASON
{
  GOLDEN_TAG_CREATED = 1,  /// 标签点被创建
  GOLDEN_TAG_UPDATED,      /// 标签点属性被更新
  GOLDEN_TAG_REMOVED,      /// 标签点被放入回收站
  GOLDEN_TAG_RECOVERD,     /// 标签点被恢复
  GOLDEN_TAG_PURGED,       /// 标签点被清除
  GOLDEN_TAB_UPDATED,      /// 标签点表被重命名
  GOLDEN_TAB_REMOVED,      /// 标签点表被删除
};

/// 每个历史数据存档文件最多允许有99个附属文件
#define GOLDEN_ARVEX_MAX          99

/// 历史数据存档文件头部页
typedef struct _GOLDEN_HEADER_PAGE
{
  int db_ver;                             // 所属数据库版本
  int data_ver;                           // 数据格式变更版本
  int begin;                              // 数据起始时间
  int end;                                // 数据结束时间
  int begin_ms;                           // 保留，暂不使用。
  int end_ms;                             // 保留，暂不使用。
  int create_time;                        // 创建时间
  int modify_time;                        // 修改时间
  int merge_time;                         // 上次合并时间
  int arrange_time;                       // 上次整理时间
  int reindex_time;                       // 上次重建索引时间
  int backup_time;                        // 上次备份时间
  long long rated_capacity;               // 创建时容量（额定容量）
  long long capacity;                     // 当前容量，文件内包含的总数据页数，不包括头页。
  long long size;                         // 实际使用量，已被占用的数据页数，不包括头页。
  long long ex_capacity;                  // 附属文件容量
  golden_byte is_main;                    // 是主文件还是附属文件
  golden_byte page_size;                  // 单个页的字节尺寸，单位为KB。
  golden_byte id_or_count;                // 主文件在这里存放附属文件数量，附属文件在这里存放自身的ID。
  golden_byte auto_merge;                 // 启用自动合并
  golden_byte auto_arrange;               // 启用自动整理
  golden_byte merged;                     // 1:已合并，0:尚未合并或合并后又产生了新的附属文件。
  golden_byte arranged;                   // 1：已整理，0：尚未整理过或整理后内容发生变更。
  golden_byte reserved_1;                 // 保留字节
  char file_name[GOLDEN_FILE_NAME_SIZE];  // 在这里存放自己的文件名。
  unsigned int crc32;                     // 以上内容的CRC32校验码，暂不启用。
  golden_byte ids[GOLDEN_ARVEX_MAX];      // 仅对主文件有效，依次存放某个附属文件是否启用。1表示启用，0表示未创建或未启用。
} GOLDEN_HEADER_PAGE;

/// 系统支持的最大路径长度
#define GOLDEN_MAX_PATH                 2048

/// 系统支持的最大主机名长度
#define GOLDEN_MAX_HOSTNAME_SIZE        1024

/// 连接到GOLDEN数据库服务器的连接信息
typedef struct _GOLDEN_HOST_CONNECT_INFO
{
  golden_int32 ipaddr ;                                           /// 连接的客户端IP地址
  golden_uint16 port ;                                            /// 连接端口
  golden_int32 job;                                               /// 连接最近处理的任务
  golden_int32 job_time;                                          /// 最近处理任务的时间
  golden_int32 connect_time;                                      /// 客户端连接时间
  char client[GOLDEN_MAX_HOSTNAME_SIZE];                          /// 连接的客户端主机名称
  char process[GOLDEN_PATH_SIZE+GOLDEN_FILE_NAME_SIZE];           /// 连接的客户端程序名
  char user[GOLDEN_USER_SIZE];                                    /// 登录的用户
} GOLDEN_HOST_CONNECT_INFO, *PGOLDEN_HOST_CONNECT_INFO;

/// 历史存档文件状态
enum GOLDEN_ARCHIVE_STATE
{
  GOLDEN_INVALID_ARCHIVE, // 0:无效；
  GOLDEN_ACTIVED_ARCHIVE, // 1:活动；
  GOLDEN_NORMAL_ARCHIVE,  // 2:普通；
  GOLDEN_READONLY_ARCHIVE // 3:只读
};

enum GOLDEN_DB_PARAM_INDEX
{
  /// string parameter.
  GOLDEN_PARAM_STR_FIRST = 0x0, 
  GOLDEN_PARAM_TABLE_FILE = GOLDEN_PARAM_STR_FIRST,   // 标签点表文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_BASE_FILE,                             // 基本标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_SCAN_FILE,                             // 采集标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_CALC_FILE,                             // 计算标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_SNAP_FILE,                             // 标签点快照文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_LIC_FILE,                              // 协议文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_HIS_FILE,                              // 历史信息文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_LOG_DIR,                               // 服务器端日志文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_USER_FILE,                             // 用户权限信息文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_SERVER_FILE,                           // 网络服务进程与其它进程交互所用的共享内存文件，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_EQAUTION_FILE,                         // 方程式服务进程与其它进程交互所用的共享内存文件，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_ARV_PAGES_FILE,                        // 历史数据缓存文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_ARVEX_PAGES_FILE,                      // 补历史数据缓存文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_ARVEX_PAGES_BLOB_FILE,                 // 补历史数据blob、str缓存文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_AUTH_FILE,                             // 信任连接段信息文件路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_RECYCLED_BASE_FILE,                    // 可回收基本标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_RECYCLED_SCAN_FILE,                    // 可回收采集标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_RECYCLED_CALC_FILE,                    // 可回收计算标签点文件全路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_AUTO_BACKUP_PATH,                      // 自动备份目的地全路径，必须以“\”或“/”结束，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_SERVER_SENDER_IP,                      // 镜像发送地址，字符串最大长度为 GOLDEN_MAX_HOSTNAME_SIZE
  GOLDEN_PARAM_BLACKLIST_FILE,                        // 连接黑名单文件路径，字符串最大长度为 GOLDEN_MAX_PATH
  GOLDEN_PARAM_DB_VERSION,                            // 数据库版本
  GOLDEN_PARAM_LIC_USER,                              // 授权用户
  GOLDEN_PARAM_LIC_TYPE,                              // 授权方式
  GOLDEN_PARAM_INDEX_DIR,                             // 索引文件存放目录
  GOLDEN_PARAM_MIRROR_BUFFER_PATH,                    // 镜像缓存本地文件所在路径
  GOLDEN_PARAM_STR_LAST, 

  // int parameter.
  GOLDEN_PARAM_INT_FIRST = 0x1000, 
  GOLDEN_PARAM_LIC_TABLES_COUNT = GOLDEN_PARAM_INT_FIRST, // 协议中限定的标签点表数量
  GOLDEN_PARAM_LIC_TAGS_COUNT,                        // 协议中限定的所有标签点数量
  GOLDEN_PARAM_LIC_SCAN_COUNT,                        // 协议中限定的采集标签点数量
  GOLDEN_PARAM_LIC_CALC_COUNT,                        // 协议中限定的计算标签点数量
  GOLDEN_PARAM_LIC_ARCHICVE_COUNT,                    // 协议中限定的历史存档文件数量
  GOLDEN_PARAM_SERVER_IPC_SIZE,                       // 网络服务进程与其它进程进行交互所使用的共享内存池的字节尺寸
  GOLDEN_PARAM_EQUATION_IPC_SIZE,                     // 网络服务进程与其它进程进行交互所使用的共享内存池的字节尺寸
  GOLDEN_PARAM_HASH_TABLE_SIZE,                       // 标签点求余哈希表的尺寸
  GOLDEN_PARAM_TAG_DELETE_TIMES/*obsolete*/,          // 可整库删除标签点的次数
  GOLDEN_PARAM_SERVER_PORT,                           // 网络服务独立服务器端口
  GOLDEN_PARAM_SERVER_SENDER_PORT,                    // 网络服务镜像接收端口
  GOLDEN_PARAM_SERVER_RECEIVER_PORT,                  // 网络服务镜像发送端口
  GOLDEN_PARAM_SERVER_MODE,                           // 网络服务启动模式
  GOLDEN_PARAM_SERVER_CONNECTION_COUNT,               // 协议中限定网络服务连接并发数量
  GOLDEN_PARAM_ARV_PAGES_NUMBER,                      // 历史数据缓存中，页数量
  GOLDEN_PARAM_ARVEX_PAGES_NUMBER,                    // 补历史数据缓存中，页数量
  GOLDEN_PARAM_EXCEPTION_AT_SERVER,                   // 是否由服务器进行例外判定
  GOLDEN_PARAM_ARV_PAGE_RECYCLE_DELAY/*obsolete*/,    // 历史数据缓存页回收延时（毫秒）
  GOLDEN_PARAM_EX_ARCHIVE_SIZE,                       // 历史数据存档文件附属文件字节大小（单位：MB）
  GOLDEN_PARAM_ARCHIVE_BATCH_SIZE,                    // 分批返回存储值的每段大小
  GOLDEN_PARAM_DATAFILE_PAGESIZE,                     // 系统数据文件页大小
  GOLDEN_PARAM_ARV_ASYNC_QUEUE_SLOWER_DOOR,           // 历史数据缓存队列慢速归档区（单位：百分比）
  GOLDEN_PARAM_ARV_ASYNC_QUEUE_NORMAL_DOOR,           // 历史数据缓存队列中速归档区（单位：百分比）
  GOLDEN_PARAM_INDEX_ALWAYS_IN_MEMORY,                // 历史数据索引常驻内存的字节大小（单位：MB）
  GOLDEN_PARAM_DISK_MIN_REST_SIZE,                    // 最低可用磁盘空间（单位：MB）
  GOLDEN_PARAM_MIN_SIZE_OF_ARCHIVE,                   // 历史存档文件和附属文件的最小尺寸（单位：KB）
  GOLDEN_PARAM_DELAY_OF_AUTO_MERGE_OR_ARRANGE,        // 自动合并/整理最小延迟（单位：小时）
  GOLDEN_PARAM_START_OF_AUTO_MERGE_OR_ARRANGE,        // 自动合并/整理开始时间（单位：点钟）
  GOLDEN_PARAM_STOP_OF_AUTO_MERGE_OR_ARRANGE,         // 自动合并/整理停止时间（单位：点钟）
  GOLDEN_PARAM_START_OF_AUTO_BACKUP,                  // 自动备份开始时间（单位：点钟）
  GOLDEN_PARAM_STOP_OF_AUTO_BACKUP,                   // 自动备份停止时间（单位：点钟）
  GOLDEN_PARAM_MAX_LATENCY_OF_SNAPSHOT,               // 允许服务器时间之后多少小时内的数据进入快照（单位：小时）
  GOLDEN_PARAM_PAGE_ALLOCATOR_RESERVE_SIZE,           // 活动页分配器预留大小（单位：KB）， 0 表示使用操作系统视图大小
  GOLDEN_PARAM_INCLUDE_SNAPSHOT_IN_QUERY,             // 决定取样本值和统计值时，快照是否应该出现在查询结果中
  GOLDEN_PARAM_LIC_BLOB_COUNT,                        // 协议中限定的字符串或BLOB类型标签点数量
  GOLDEN_PARAM_MIRROR_BUFFER_SIZE,                    // 镜像文件大小（单位：GB）
  GOLDEN_PARAM_BLOB_ARVEX_PAGES_NUMBER,               // blob、str补历史的默认缓存页数量，默认为协议数2倍（blob、str点）
  GOLDEN_PARAM_MIRROR_EVENT_QUEUE_CAPACITY,           // 镜像缓存文件页大小，默认为协议数5倍
  GOLDEN_PARAM_QUICK_OPERATE_TAG,                     // 一旦启用，对标签点的“增、删、改”操作将不会要求强制写入磁盘
  GOLDEN_PARAM_NOTIFY_NOT_ENOUGH_SPACE,               // 一旦启用，设置为ON，则通过API返回大错误码，否则只记录日志

  GOLDEN_PARAM_INT_LAST, 

  //exp int param
  GOLDEN_PARAM_EXP_INT_FIRST = 0x2000,
  GOLDEN_PARAM_MAX_BLOB_SIZE = GOLDEN_PARAM_EXP_INT_FIRST,   // blob、str类型数据在数据库中允许的最大长度
  GOLDEN_PARAM_EXP_INT_LAST,
}; 

enum GOLDEN_TAG_FIELD_INDEX
{
  GOLDEN_TAG_INDEX_BASE_FIRST = 0x0,
  GOLDEN_TAG_INDEX_TAG = GOLDEN_TAG_INDEX_BASE_FIRST, // tag
  GOLDEN_TAG_INDEX_ID,                                // id
  GOLDEN_TAG_INDEX_TYPE,                              // type
  GOLDEN_TAG_INDEX_TABLE,                             // table
  GOLDEN_TAG_INDEX_DESC,                              // desc
  GOLDEN_TAG_INDEX_UNIT,                              // unit
  GOLDEN_TAG_INDEX_ARCHIVE,                           // archive
  GOLDEN_TAG_INDEX_DIGITS,                            // digits
  GOLDEN_TAG_INDEX_SHUTDOWN,                          // shutdown
  GOLDEN_TAG_INDEX_LOWLIMIT,                          // lowlimit
  GOLDEN_TAG_INDEX_HIGHLIMIT,                         // highlimit
  GOLDEN_TAG_INDEX_STEP,                              // step
  GOLDEN_TAG_INDEX_TYPICAL,                           // typical
  GOLDEN_TAG_INDEX_COMPRESS,                          // compress
  GOLDEN_TAG_INDEX_COMPDEV,                           // compdev
  GOLDEN_TAG_INDEX_COMPDEVPERCENT,                    // compdevpercent
  GOLDEN_TAG_INDEX_COMPTIMEMAX,                       // comptimemax
  GOLDEN_TAG_INDEX_COMPTIMEMIN,                       // comptimemin
  GOLDEN_TAG_INDEX_EXCDEV,                            // excdev
  GOLDEN_TAG_INDEX_EXCDEVPERCENT,                     // excdevpercent
  GOLDEN_TAG_INDEX_EXCTIMEMAX,                        // exctimemax
  GOLDEN_TAG_INDEX_EXCTIMEMIN,                        // exctimemin
  GOLDEN_TAG_INDEX_CLASSOF,                           // classof
  GOLDEN_TAG_INDEX_CHANGEDATE,                        // changedate
  GOLDEN_TAG_INDEX_CHANGER,                           // changer
  GOLDEN_TAG_INDEX_CREATEDATE,                        // createdate
  GOLDEN_TAG_INDEX_CREATOR,                           // creator
  GOLDEN_TAG_INDEX_MIRROR,                            // mirror
  GOLDEN_TAG_INDEX_MS,                                // millisecond
  GOLDEN_TAG_INDEX_FULLNAME,                          // table_dot_tag
  GOLDEN_TAG_INDEX_SUMMARY,                           // summary
  GOLDEN_TAG_INDEX_BASE_LAST, 
  
  GOLDEN_TAG_INDEX_SCAN_FIRST = 0x1000, 
  GOLDEN_TAG_INDEX_SOURCE = GOLDEN_TAG_INDEX_SCAN_FIRST, // source
  GOLDEN_TAG_INDEX_SCAN,                              // scan
  GOLDEN_TAG_INDEX_INSTRUMENT,                        // instrument
  GOLDEN_TAG_INDEX_LOCATION1,                         // locations[0]
  GOLDEN_TAG_INDEX_LOCATION2,                         // locations[2]
  GOLDEN_TAG_INDEX_LOCATION3,                         // locations[3]
  GOLDEN_TAG_INDEX_LOCATION4,                         // locations[4]
  GOLDEN_TAG_INDEX_LOCATION5,                         // locations[5]
  GOLDEN_TAG_INDEX_USERINT1,                          // userints[0]
  GOLDEN_TAG_INDEX_USERINT2,                          // userints[1]
  GOLDEN_TAG_INDEX_USERREAL1,                         // userreals[0]
  GOLDEN_TAG_INDEX_USERREAL2,                         // userreals[1]
  GOLDEN_TAG_INDEX_SCAN_LAST, 

  GOLDEN_TAG_INDEX_CALC_FIRST = 0x2000,
  GOLDEN_TAG_INDEX_EQUATION = GOLDEN_TAG_INDEX_CALC_FIRST, // equation
  GOLDEN_TAG_INDEX_TRIGGER,                           // trigger
  GOLDEN_TAG_INDEX_TIMECOPY,                          // timecopy
  GOLDEN_TAG_INDEX_PERIOD,                            // period
  GOLDEN_TAG_INDEX_CALC_LAST, 

  GOLDEN_TAG_INDEX_SNAPSHOT_FIRST = 0x3000, 
  GOLDEN_TAG_INDEX_TIMESTAMP = GOLDEN_TAG_INDEX_SNAPSHOT_FIRST, // snapshot stamp (ms)
  GOLDEN_TAG_INDEX_VALUE,                             // snapshot value
  GOLDEN_TAG_INDEX_QUALITY,                           // snapshot quality
  GOLDEN_TAG_INDEX_SNAPSHOT_LAST, 
}; 

enum GOLDEN_TAG_SORT_FLAG
{
  GOLDEN_SORT_FLAG_DESCEND = 0x0001, 
  GOLDEN_SORT_FLAG_CASE_SENSITIVE = 0x0002, 
  GOLDEN_SORT_FLAG_RECYCLED = 0x0004, 
}; 


/**
* 命名：gob_tags_change
* 功能：标签点属性更改通知订阅回调接口
* 参数：
*        [count]     整型，输入，标签点个数，ids 的长度，
*        [ids]       整型数组，输入，标签点被订阅且属性发生更改的标识列表
*        [what]      整型，参考枚举 GOLDEN_TAG_CHANGE_REASON,
*                    表示引起变更的源类型。
*/
typedef 
golden_error 
(*gob_tags_change)(
                   golden_int32 count, 
                   const golden_int32 *ids,
                   golden_int32 what
                   );

/**
* 命名：gos_data_change
* 功能：标签点快照改变通知订阅回调接口
* 参数：
*        [count]     整型，输入，
*                    表示 ids、datetimes、ms、values、states、qualities、errors的长度
*        [ids]       整型数组，输入，标签点被订阅且快照发生改变的标识列表
*        [datatimes] 整型数组，输入，实时数值时间列表，
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，实时数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值；否则为 0
*        [values]    双精度浮点型数组，输入，实时浮点型数值列表
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的快照值；否则为 0
*        [states]    64 位整型数组，输入，实时整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的快照值；否则为 0
*        [qualities] 短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY
*        [errors]    无符号整型数组，输出，写入实时数据的返回值列表，参考golden_error.h
* 备注：本接口对数据类型为 GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 的标签点无效。
*/
typedef 
golden_error 
(*gos_data_change)(
                   golden_int32 count,
                   const golden_int32 *ids, 
                   const golden_int32 *datetimes, 
                   const golden_int16 *ms, 
                   const golden_float64 *values,
                   const golden_int64 *states,
                   const golden_int16 *qualities,
                   const golden_error *errors
                   );


/**
* 命名：goh_data_playback
* 功能：标签点历史数据回放回调接口
* 参数：
*        [count]     整型，输入，
*                    表示 ids、datetimes、ms、values、states、qualities、errors的长度
*        [ids]       整型数组，输入，到达数据的标识列表
*        [datatimes] 整型数组，输入，到达数值时间列表，
*                    表示距离1970年1月1日08:00:00的秒数
*        [ms]        短整型数组，输入，到达数值时间列表，
*                    对于时间精度为毫秒的标签点，存放相应的毫秒值。
*        [values]    双精度浮点型数组，输入，到达的浮点型数值列表
*                    对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应数据。
*        [states]    64 位整型数组，输入，到达的整型数值列表，
*                    对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
*                    GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应数据。
*        [x]         32 位浮点数，输入，二维坐标的x值，对于数据类型为 GOLDEN_COOR 的标签点，存放相应数据。
*        [y]         32 位浮点数，输入，二维坐标的y值，对于数据类型为 GOLDEN_COOR 的标签点，存放相应数据。
*        [qualities] 短整型数组，输入，到达的数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY
*        [error]     无符号整型，输出，如返回GoE_DATA_PLAYBACK_DONE则表明是最后一次回放，否则只会返回GoE_OK。
* 备注：本接口对数据类型为 GOLDEN_STRING、GOLDEN_BLOB 的标签点无效。
*/
typedef 
golden_error 
(*goh_data_playback)(
                     golden_int32 count,
                     const golden_int32 *ids, 
                     const golden_int32 *datetimes, 
                     const golden_int16 *ms, 
                     const golden_float64 *values,
                     const golden_int64 *states,
                     const golden_float32 *x,
                     const golden_float32 *y,
                     const golden_int16 *qualities,
                     const golden_error *error
                     );

#ifdef WIN32
double inline __declspec (naked) __fastcall _golden_sqrt(double)
{
  _asm fld qword ptr [esp+4]
  _asm fsqrt
  _asm ret 8
}
#endif

#define GOLDEN_ABS(V1, V2)                          ( (V1) > (V2) ? ((V1) - (V2)) : ((V2) - (V1)) )
#define GOLDEN_TIME_LESS_THAN(S1, MS1, S2, MS2)     ( S1 < S2 ? true : ( S1 == S2 ? ( MS1 < MS2 ? true : false ) : false ) )
#define GOLDEN_TIME_EQUAL_TO(S1, MS1, S2, MS2)      ( S1 == S2 ? ( MS1 == MS2 ? true : false ) : false )
#define GOLDEN_TIME_GREATER_THAN(S1, MS1, S2, MS2)  ( S1 > S2 ? true : ( S1 == S2 ? ( MS1 > MS2 ? true : false ) : false ) )
#define GOLDEN_TIME_EQUAL_OR_LESS_THAN(S1, MS1, S2, MS2)     ( S1 < S2 ? true : ( S1 == S2 ? ( MS1 <= MS2 ? true : false ) : false ) )
#define GOLDEN_TIME_EQUAL_OR_GREATER_THAN(S1, MS1, S2, MS2)  ( S1 > S2 ? true : ( S1 == S2 ? ( MS1 >= MS2 ? true : false ) : false ) )
#define GOLDEN_MS_DELAY_BETWEEN(S1, MS1, S2, MS2)   ( (unsigned long long)(S1 - S2) * 1000 + (unsigned long long)(MS1 - MS2) )
#define GOLDEN_MS_ADD_TIME(S1, MS1, MSES, S2, MS2)  { \
                                                      unsigned long long mses__ = MSES + MS1; \
                                                      S2 = S1 + static_cast<int>(mses__ / 1000); \
                                                      MS2 = static_cast<short>(mses__ % 1000); }
#define GOLDEN_DISTANCE(X1, Y1, X2, Y2)             _golden_sqrt( GOLDEN_ABS(X1, X2) * GOLDEN_ABS(X1, X2) + GOLDEN_ABS(Y1, Y2) * GOLDEN_ABS(Y1, Y2) )


// 2.1.2525
#define GOLDEN_API_MAJOR_VERSION      2
#define GOLDEN_API_MINOR_VERSION      1
#define GOLDEN_API_BETA_VERSION       2525

#endif /* __GOLDEN_H__ */
