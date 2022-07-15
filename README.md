蒟蒻写的简单辣鸡代码，玩玩而已，轻喷。不会写readme，显示有问题（逃..

使用方法：

1、将myprotect.cpp和myprotect.h加入项目

2、设置编译选项

​	1 项目->属性->c/c++->优化-> 内联函数扩展选择（任何适用项），启用内部函数（是），优选大小或速度（均不），全程序优化（是）。release和debug根据自己喜好配置

​	2（建议，非必须）发布release版本前，项目->属性->链接器->调试->生成调试信息（否），去除私有符号（是）。项目->属性->链接器->常规->启用增量链接（否）



可使用的功能

宏定义：

1	SECTION(name)

将接下来的一个函数放入指定的节中，例子如下

SECTION(.example) void function(){

}   //此函数将被放置在.example节中



2	IF_BEGIN(表达式,标签名)、IF_END(标签名)

混淆if，用cmp_混淆函数比较，push,ret实现跳转。与IF_END(标签名)同时使用，标签名不可重复。例子如下

IF(!(key==123456),label1)

{	puts("key不等于123456")	}

IF_END(label1)

注意事项：cmp_是内联展开成大量与非、或非计算进行比较。但是存在两个爆破点，一是直接修改表达式的值让其为真（如果看出此处的意图），二是猜测ret的正确地址修改push 的地址再ret。另外，根据实现原理，最好将错误等代码放在if成立的括号内，这样正确时ret的地址才能受到保护

3	IF_ELSE_BEGIN(表达式，标签名)、ELSE(标签名)、IF_ELSE_END()

如上述，最好把正确等代码放在else中，例子如下

IF_ELSE_BEGIN(key!=123456,protect_test)
{
		puts("不正确");
}
ELSE(protect_test)
{
		//正确
}
IF_ELSE_END()





//-------------判断是否被调试方法（反调试以后再好好写bushi）

__forceinline extern bool check_debugger_method_1();

__forceinline extern bool check_debugger_method_2();

__forceinline extern bool check_debugger_method_3();	检测父进程是否为explorer

__forceinline extern bool check_debugger_method_4();	CheckRemoteDebuggerPresent
//------------------------------------------------------------------------------

//退出或崩溃方法
__forceinline extern void exit_1();	清空寄存器

__forceinline extern void exit_2();	访问高地址

__forceinline extern void exit_3();	调用exit,ExitProcess,TerminateProcess

__forceinline extern void exit_4();	int 3



花指令（只写了一个稍微复杂点的，简单的直接塞伪vm那了）

__forceinline extern void insert_junk_code_1(void(*func1)() = nullptr, void(*func2)() = nullptr);
inline extern void insert_junk_code_1_part2();

可以提供至多两个函数指针让函数在花指令执行中执行，例子

int key=0;

void func1(){

​	while(key!=123456)

​	{	cin>>key;	}

}

void func2(){

​	if(key!=123456)

​	{	puts("while被爆破");	}

}

main(){

​	insert_junk_code_1();	//插入一段无用花指令

​	insert_junk_code_1(func1,func2);	//插入一段无用花指令，并且在其间执行func1，再执行func2

​	insert_junk_code_1();	//插入一段无用花指令

}

效果是这样的...反汇编引擎会出错，主函数流程会在花指令开始后被根本不执行的retn截断,最下面的call进不去，里面全是垃圾指令。直接把所有花指令干掉的话没准哪里传了1、2个函数进去执行，也被干掉，程序就不对了。

<img src="https://github.com/mizukiyyds/my_protect/blob/main/jpg/1.jpg" alt="1" style="zoom:50%;" />










template<typename T>
T* list_to_pointer(const std::initializer_list<T>& list);

将初始化列表转化为数组，返回数组的指针





混淆与虚拟代码均在命名空间protect中

void print_string(void(*func)(char* str), char* str)

输出一串不可被od中文搜索引擎搜索到的字符串，参数一func为自己的给字符串处理的函数（可以啥也不干）。不管是否加密，都不会被搜索到。例子如下

//字符串必须是局部变量才不会被搜索到

 void encrypt(char* str)
{

 	//处理你的字符串,或者跟我一样摆大烂啥也不干:)
}

int main(){

​		using namespace protect;

		char str[1001]="example";
		print_string(encrypt,str);
		system("pause");

}

//运行后会输出一串字符串，然后复制，替换掉明文“example”即可





以下为通过强制内联展开进行混淆的函数（会增大程序体积）

long long nor_(const long long& n, const long long& m);										 /或非	~(n|m)

long long nand_(const long long& n, const long long& m)									   //与非   ~(n&m)

long long not_(const long long& n, protect_type type)											  //非	~n

long long and_(const long long& n, const long long& m, protect_type type)		//与	n&m

long long or_(const long long& n, const long long& m, protect_type type)			//或	n|m

long long xor_(const long long& n, const long long& m, protect_type type)			//异或	n^m

long long xnor_(const long long& n, const long long& m, protect_type type)		//同或	~(n^m)

long long shl_(const long long& n, const long long& m)											//左移	n<<m

 long long shr_(const long long& n, const long long& m)											//右移   n>>m

long long add_(const long long& n, const long long& m, const protect_type& type)	//加法	n+m

long long sub_(const long long& n, const long long& m, const protect_type& type)	//减法	n-m

long long mul_(const long long& n, const long long& m, const protect_type& type)	//乘法	n*m

//比较函数	n小于m返回-1，等于返回0，大于返回1
long long cmp_(const long long& n, const long long& m, const protect_type& type)

protect_type可不填，默认是用或非门进行计算，还有与非门与随机

效果大概是这样...这是流程图

<img src="https://github.com/mizukiyyds/my_protect/blob/main/jpg/2.jpg" alt="2" style="zoom:50%;" />









以下为自己瞎搞的伪vm（这个辣鸡代码也配？）

long long vm(const op_key key, long long* arg, const long long& n)

op_key为操作码，arg为参数数组，n为参数个数（暂时没用）。解码的时候（在混淆if_else中）求最小的质因数，依次对应。也就是说key传2和传2x2x2x3x3x3x4x4x5都是nor操作。

// 2-nor 3-nand 5-not 7-and 11-or 13-xor 17-xnor 19-shl 23-shr 29-add 31-sub 37-mul 41-cmp
	enum op_key
	{
		op_endless_loop=0,		//死循环
		op_exit=1,					//崩溃退出
		op_nor=2*2*3*5*7*11*13*17*19*23,
		op_nand=3*3*5*7*11*13*17*19*23,
		op_not=5*5*7*17*29*31*37,
		op_and=7*7*7*7*13*13*13*13,
		op_or=11*13*17*19*23*29,
		op_xor=13*13*13*13*17*17*37,
		op_xnor=17*19*29*31*37*41,
		op_shl=19*43*47*53*59,
		op_shr=23*23*23*23*29,
		op_add=29*29*31*41*41,
		op_sub=31*37*37*37*37,
		op_mul=37*41*41*67*97,
		op_cmp=41*41*43*43*71,	//cmp还没写，开摆
	};

例子如下

#include <iostream>

#include "myprotect.h"

int main()
{
	long long a=123;
	long long b=456;
	long long c=a+b;
	std::cout<<c<<endl;


	{
		using namespace protect;
		long long arg[2]={a,b};
		c=vm(op_add,arg,2);
		std::cout<<c<<endl;
		c=vm(op_add,list_to_pointer({a,b}),2);
		std::cout<<c<<endl;
	}


}



效果大概是这样，每次从很多指令中选择一个执行（插入了花指令，部分流程显示是错误的）

![3](https://github.com/mizukiyyds/my_protect/blob/main/jpg/3.jpg)







如果你闲的蛋疼浪费时间，想要用这个辣鸡头文件的同时自由发挥修改vm代码内部实现等等，可以看看以下几个函数的具体实现

long long vm(const op_key key, long long* arg, const long long& n)

__forceinline extern long long jmp_vm(const op_key key,long long* arg,const long long& n);

质数case有效，合数case是垃圾指令

假设vm函数传入了参数op_key=2x2x3x3x5x5x7x7	(nor操作)，解码后得到op=2,org_op=2，那么将执行

case2，case 2+2, case 2+2+2, case 2+2+2+...	直到default

然后org_op+1（目前变为3）,op变为3+3等于6，然后执行	case 6，case 6+3 ,case 6+3+3,...全是无用的合数case指令。只有第一个操作是有用的。

假设vm函数传入了参数op_add，就会一会执行一大堆合数的无用case,然后执行一个质数有用case,交替。



仅供参考娱乐
