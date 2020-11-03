/* 客户端 
 * 需要用户输入 服务器的IP 和端口号
 * */
#include <arpa/inet.h>                          // 为了使用 inet_pton
#include <stdio.h>
#include <stdlib.h>                              // 为了使用 atoi
#include <sys/types.h>          
#include <sys/socket.h>                          
#include <string.h>


//定义一个登录信息结构体 包含登录的所有信息
typedef struct{
char tips[64];          // 假如用户想不起密码 可以通过 tips 获取提示
char name_account[20];  // 登录帐号的名字 
char passwd[20];        // 登录密码
char type;              // 帐号的 类型 分为 root用户和普通用户
}account;               //登录帐号信息结构体

// 定义一个结构体 包括员工的所有属性
typedef struct {
	char name_staff[20];    //员工姓名
	char sex[10]; 		    //性别
	int age; 			    //年龄
	char phone[11]; 	    //电话
	char addr[50]; 		    //住址
}staff;

//定义 2个信息结构体 一个是 客户端发,服务器收的结构体 一个是 客户端收 服务器发的结构体

//服务器发送的消息结构体
typedef struct {
char flag;              // flag 标志 服务器 回应的是什么请求
char isitsuccess;       // isitsuccess 标志 客户端的请求是否成功执行
char msg_success[256];  // 客户端的请求成功时候 服务器发送的消息
char msg_failed[256];   // 客户端的请求失败时 发送的消息
account user_account;   // 用户账户
staff user_staff;       // 用户的员工资料
}MSG_to_client;

//客户端发送的消息结构体
typedef struct {
char flag;              // flag 标志 客户端请求的种类 
account user_account;   // 用户账户
staff user_staff;       // 用户的员工资料
}MSG_to_server;


int main(int argc,const char* argv[])
{
	int fd_client=-1;             // 客户端文件描述符
	int ret=-1;                             // 用来接收函数返回值
	unsigned int  addr_server;            // 服务器的IP地址 网络字节序  
	short int       port_server;                // 服务器端口号  网络字节序 
	short int      _port_server;               // 服务器端口号 本地字节序
	struct sockaddr_in      ip_server;              //服务器地址信息结构体
	
	
	// 用户需要提供 IP 地址和端口号
	if(argc<3)
		goto err1;
	// 检查用户输入的 IP地址是否合法 使用 inet_pton 成功返回1
	ret=inet_pton(AF_INET,argv[1],&addr_server);
	if(ret!=1)
		goto err2;
	//检查用户输入的 端口号是否合法 端口号范围 1024-49151
	_port_server = atoi(argv[2]);
	// 判断该端口号 在不在范围内
	ret = !(_port_server>=1024 && _port_server<=49151);
	if(ret)
		goto err3;
	// 将端口号转换为网络字节序
	port_server=htons(_port_server);

	//  执行到这里说明 用户的输入没有问题
	//将用户输入的地址和端口 填入信息结构体
	//清零
	bzero(&ip_server,sizeof(ip_server));
	ip_server.sin_family=AF_INET;             // IPV4
	ip_server.sin_port=port_server;           // 填充端口号
	ip_server.sin_addr.s_addr = addr_server;  // 填充 IP地址

	// 创建套接字 对应 客户端
	fd_client = socket(AF_INET,SOCK_STREAM,0);
	if(fd_client<0)
		goto err4;
	// 直接连接 服务器
	ret=connect(fd_client,(struct sockaddr*)&ip_server,sizeof(ip_server));
	if(ret<0)
		goto err5;

	printf("连接服务器成功\n!");
	//申请空间 接收消息 和 发送消息的缓冲区
	MSG_to_server *buf_to_server=(MSG_to_server*)malloc(sizeof(MSG_to_server));
	MSG_to_client *buf_to_client=(MSG_to_client*)malloc(sizeof(MSG_to_client));
	bzero(buf_to_server,sizeof(MSG_to_server));  // 清零
	bzero(buf_to_client,sizeof(MSG_to_client));

	//发送消息
	buf_to_server->flag='h';
	send(fd_client,buf_to_server,sizeof(MSG_to_server),0);

	//接收消息
	recv(fd_client,buf_to_client,sizeof(MSG_to_client),0);
	printf("%s\n",buf_to_client->msg_success);



	while(1);



	return 0;


err5:
	perror("connect");
	return -1;
err4:
	perror("socket");                   // 根据 errno 打印出错信息
	return -1;
err3:
	printf("端口号建议范围:1024-49151");
	return -1;
err2:
	printf("不合适的地址\n");
	return -1;
err1:
	printf("你输入了%d个参数 需要2个参数,例如 ./a.out 127.0.0.1 8888 \n",argc-1);
	return -1;
}

