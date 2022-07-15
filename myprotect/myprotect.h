
#pragma once

#pragma inline_depth(100)
#pragma inline_recursion(on)

#define SECTION(name) __declspec(code_seg(#name))


//����ʼ���б�ת��Ϊ���飬���������ָ�루����ֱ�Ӵ��Σ�
template<typename T>
T* list_to_pointer(const std::initializer_list<T>& list);

template<typename T>
__forceinline T* jmp_list_to_pointer(const std::initializer_list<T>& list);


namespace protect
{
	extern long long param[11];
	extern long long params;


	SECTION(.mizuki) __forceinline extern long long get_rand(const long long& l, const long long& r);
	SECTION(.mizuki) extern void initialize(long long kernel_key = 987654);			//��ʼ������

	//��Crackme����initial��������key �����ڼ��ܺͽ��ܣ������⵽���򱻵��ԡ����ģ�key���Ϊ0��0���κ������Ϊ�䱾�����ܻ�ʧ��
	extern long long kernel_key;


	SECTION(.mizuki) extern void print_string(void(*func)(char* str), char* str);


	const long long protect_types = 4;	//protect_type������Ŀ
	enum protect_type
	{
		type_default,	//Ĭ�ϣ����ڻ�ǵ����㣩
		type_nor,		//���ڻ������
		type_nand,		//�����������
		type_rand,		//���ѡ�񣬷�֧��ͬ�������ͬ
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
	SECTION(.mizuki) __forceinline extern long long add_(const long long& n, const long long& m, const protect_type& type = type_default);		//�����ӷ�
	SECTION(.mizuki) __forceinline extern long long sub_(const long long& n, const long long& m, const protect_type& type = type_default);		//��������
	SECTION(.mizuki) __forceinline extern long long mul_(const long long& n, const long long& m, const protect_type& type = type_default);		//�����˷�
	SECTION(.mizuki) extern long long cmp_(const long long& n, const long long& m, const protect_type& type = type_default);		//�����Ƚ�
	SECTION(.mizuki) extern void if_(const long long& expression, void* address, const protect_type& type = type_default);		//����if��push retʵ����ת

	SECTION(.mizuki) __forceinline extern long long add(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long sub(const long long& n, const long long& m);
	SECTION(.mizuki) __forceinline extern long long mul(const long long& n, const long long& m);


	// 2-nor 3-nand 5-not 7-and 11-or 13-xor 17-xnor 19-shl 23-shr 29-add 31-sub 37-mul 41-cmp
	enum op_key
	{
		op_endless_loop=0,		//��ѭ��
		op_exit=1,				//�����˳�
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


	//��û����
	enum last_error
	{
		success,						//�ɹ�
		success_possible,				//������ɣ������ܽ��������Ԥ��
		error_invalid_protect_type,		//��Ч��protect_type
		error_impossible,				//������ִ�е��ĵط���ִ����
		error_initial_twice				//��ʼ������initial�����������λ�����(key���ܸ���)
	};
	static last_error last_error_code = success;
	SECTION(.mizuki) void print_last_error();


}



__forceinline extern void insert_junk_code_1(void(*func1)() = nullptr, void(*func2)() = nullptr);
inline extern void insert_junk_code_1_part2();


//Ϊ�˷�ֹ����ֱ�ӱ��ɵ�����Ҫinline
//�����ԣ���ʱ�Ƿ񱻵��ԣ�
__forceinline extern void anti_debug();


__forceinline extern void check_debugger_1();
__forceinline extern void check_debugger_2();
__forceinline extern void check_debugger_3();
__forceinline extern void check_debugger_4();	//���ĸ�������ֱ���˳���Ӱ������������ת��ʧ��



//-------------�ж��Ƿ񱻵��Է���-----------
//
__forceinline extern bool check_debugger_method_1();
__forceinline extern bool check_debugger_method_2();
__forceinline extern bool check_debugger_method_3();
__forceinline extern bool check_debugger_method_4();
//-----------------------------------------

//-------------�˳������-----------------
//
__forceinline extern void exit_1();
__forceinline extern void exit_2();
__forceinline extern void exit_3();
__forceinline extern void exit_4();
//----------------------------------------



//��б�ܺ��治���ж���ո�asm��Ҫ����������ÿһ��ǰҪ��__asm
//ʹ�þ�ʽ C ע�� ( /* comment */) �����ǻ��ʽע�� ( ; comment) ���� C ע�� ( // comment)

//ѹջ�ٵ������ѱ�ǩ��ַ���ݸ�ָ��
#define GET_LABEL_ADDRESS(p,label) do{		\
	void* tmp_p=0;							\
	__asm{									\
	__asm push label						\
	__asm pop tmp_p							\
	}										\
	p=tmp_p;								\
}while(0)





/**
 * \brief ����if��ʼ��־��������ʽ��������������ת��IF_END(mark)
 * ����ʹ���ʽ��������������if�м�����ݣ�Ч������
 * \param expression	�жϵı��ʽ
 * \param mark			����ĸ��ͷ�ı�ǩ����IF_END���Ӧ���������Ѿ������ı�ǩ��
 */
#define IF_BEGIN(expression,mark) do{			\
	void* tmp_addr=nullptr;						\
	GET_LABEL_ADDRESS(tmp_addr,mark);			\
	if_(expression,tmp_addr,protect::type_rand);

 /**
  * \brief ����if������־
  * \param mark ����ĸ��ͷ�ı�ǩ����IF_BEGIN���Ӧ���������Ѿ������ı�ǩ��
  */
#define IF_END(mark)						\
	mark:									\
	break;									\
	}while(0);								\



  /**
   * \brief ����if_else��ʼ��־��������ʽ��������������ת��ELSE(mark)
   * ����ʹ���ʽ��������������if�м�����ݣ�Ч������
   * \param expression	�жϵı��ʽ
   * \param mark			����ĸ��ͷ�ı�ǩ����ELSE���Ӧ���������Ѿ������ı�ǩ��
   */
#define IF_ELSE_BEGIN(expression,mark) do{		\
	void* tmp_addr=nullptr;						\
	GET_LABEL_ADDRESS(tmp_addr,mark);			\
	if_(expression,tmp_addr,protect::type_rand);

   /**
	* \brief ����if_else��else��ʼ��־
	* \param mark ����ĸ��ͷ�ı�ǩ����IF_ELSE_BEGIN���Ӧ���������Ѿ������ı�ǩ��
	*/
#define ELSE(mark)								\
	break;										\
	mark:										\
	__asm nop

	/**
	 * \brief ����if_else������־
	 */
#define IF_ELSE_END()							\
	}while(0);



#pragma code_seg(".text")