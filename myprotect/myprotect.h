
#pragma once

#pragma inline_depth(100)
#pragma inline_recursion(on)

#define SECTION(name) __declspec(code_seg(#name))


//将初始化列表转化为数组，返回数组的指针（便于直接传参）
template<typename T>
T* list_to_pointer(const std::initializer_list<T>& list);

template<typename T>
__forceinline T* jmp_list_to_pointer(const std::initializer_list<T>& list);


namespace protect
{
	extern long long param[11];
	extern long long params;


	SECTION(.mizuki) __forceinline extern long long get_rand(const long long& l, const long long& r);
	SECTION(.mizuki) extern void initialize(long long kernel_key = 987654);			//初始化函数

	//（Crackme）用initial函数设置key 。用于加密和解密，如果检测到程序被调试、更改，key会变为0。0与任何数异或为其本身，解密会失败
	extern long long kernel_key;


	SECTION(.mizuki) extern void print_string(void(*func)(char* str), char* str);


	const long long protect_types = 4;	//protect_type种类数目
	enum protect_type
	{
		type_default,	//默认（基于或非的运算）
		type_nor,		//基于或非运算
		type_nand,		//基于与非运算
		type_rand,		//随机选择，分支不同但结果相同
	};
	SECTION(.mizuki) __forceinline extern long long nor_(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long nand_(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long not_(const long long& n, protect_type type = type_default);
	SECTION(.mizuki) __forceinline extern long long and_(const long long& n, const long long& m, protect_type type = type_default);
	SECTION(.mizuki) __forceinline extern long long or_(const long long& n, const long long& m, protect_type type = type_default);
	SECTION(.mizuki) __forceinline extern long long xor_(const long long& n, const long long& m, protect_type type = type_default);
	SECTION(.mizuki) __forceinline extern long long xnor_(const long long& n, const long long& m, protect_type type = type_default);
	SECTION(.mizuki) __forceinline extern long long shl_(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long shr_(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long add_(const long long& n, const long long& m, const protect_type& type = type_default);		//混淆加法
	SECTION(.mizuki) __forceinline extern long long sub_(const long long& n, const long long& m, const protect_type& type = type_default);		//混淆减法
	SECTION(.mizuki) __forceinline extern long long mul_(const long long& n, const long long& m, const protect_type& type = type_default);		//混淆乘法
	SECTION(.mizuki) extern long long cmp_(const long long& n, const long long& m, const protect_type& type = type_default);		//混淆比较
	SECTION(.mizuki) extern void if_(const long long& expression, void* address, const protect_type& type = type_default);		//混淆if，push ret实现跳转

	SECTION(.mizuki) __forceinline extern long long add(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long sub(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long mul(const long long& n, const long long& m);


	// 2-nor 3-nand 5-not 7-and 11-or 13-xor 17-xnor 19-shl 23-shr 29-add 31-sub 37-mul 41-cmp
	enum op_key
	{
		op_endless_loop=0,		//死循环
		op_exit=1,				//崩溃退出
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
		op_cmp=41*41*43*43*71,
	};
	SECTION(.mizuki) extern long long vm(const op_key key,long long* arg ,const long long& n);
	SECTION(.mizuki) __forceinline extern long long jmp_vm(const op_key key,long long* arg,const long long& n);


	//还没完善
	enum last_error
	{
		success,						//成功
		success_possible,				//操作完成，但可能结果不符合预期
		error_invalid_protect_type,		//无效的protect_type
		error_impossible,				//不可能执行到的地方被执行了
		error_initial_twice				//初始化函数initial被调用了两次或以上(key不能更改)
	};
	static last_error last_error_code = success;
	SECTION(.mizuki) void print_last_error();


}



__forceinline extern void insert_junk_code_1(void(*func1)() = nullptr, void(*func2)() = nullptr);
inline extern void insert_junk_code_1_part2();


//为了防止函数直接被干掉，需要inline
//反调试（此时是否被调试）
__forceinline extern void anti_debug();


__forceinline extern void check_debugger_1();
__forceinline extern void check_debugger_2();
__forceinline extern void check_debugger_3();
__forceinline extern void check_debugger_4();	//第四个方法不直接退出，影响程序后续流程转向失败



//-------------判断是否被调试方法-----------
//
__forceinline extern bool check_debugger_method_1();
__forceinline extern bool check_debugger_method_2();
__forceinline extern bool check_debugger_method_3();
__forceinline extern bool check_debugger_method_4();
//-----------------------------------------

//-------------退出或崩溃-----------------
//
__forceinline extern void exit_1();
__forceinline extern void exit_2();
__forceinline extern void exit_3();
__forceinline extern void exit_4();
//----------------------------------------



//反斜杠后面不能有多余空格，asm块要包起来并且每一句前要加__asm
//使用旧式 C 注释 ( /* comment */) 而不是汇编式注释 ( ; comment) 或单行 C 注释 ( // comment)

//压栈再弹出，把标签地址传递给指针
#define GET_LABEL_ADDRESS(p,label) do{		\
	void* tmp_p=0;							\
	__asm{									\
	__asm push label						\
	__asm pop tmp_p							\
	}										\
	p=tmp_p;								\
}while(0)





/**
 * \brief 混淆if开始标志。如果表达式不成立，将会跳转到IF_END(mark)
 * 尽量使表达式不成立，即跳过if中间的内容，效果更好
 * \param expression	判断的表达式
 * \param mark			以字母开头的标签，与IF_END相对应，不能是已经声明的标签。
 */
#define IF_BEGIN(expression,mark) do{			\
	void* tmp_addr=nullptr;						\
	GET_LABEL_ADDRESS(tmp_addr,mark);			\
	if_(expression,tmp_addr,protect::type_rand);

 /**
  * \brief 混淆if结束标志
  * \param mark 以字母开头的标签，与IF_BEGIN相对应，不能是已经声明的标签。
  */
#define IF_END(mark)						\
	mark:									\
	break;									\
	}while(0);								\



  /**
   * \brief 混淆if_else开始标志。如果表达式不成立，将会跳转到ELSE(mark)
   * 尽量使表达式不成立，即跳过if中间的内容，效果更好
   * \param expression	判断的表达式
   * \param mark			以字母开头的标签，与ELSE相对应，不能是已经声明的标签。
   */
#define IF_ELSE_BEGIN(expression,mark) do{		\
	void* tmp_addr=nullptr;						\
	GET_LABEL_ADDRESS(tmp_addr,mark);			\
	if_(expression,tmp_addr,protect::type_rand);

   /**
	* \brief 混淆if_else的else开始标志
	* \param mark 以字母开头的标签，与IF_ELSE_BEGIN相对应，不能是已经声明的标签。
	*/
#define ELSE(mark)								\
	break;										\
	mark:										\
	__asm nop

	/**
	 * \brief 混淆if_else结束标志
	 */
#define IF_ELSE_END()							\
	}while(0);



#pragma code_seg(".text")