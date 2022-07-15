
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <winternl.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include "myprotect.h"



using namespace std;





DWORD getParentPID(DWORD pid);
int getProcessName(DWORD pid, LPSTR fname, DWORD sz);

template<typename T>
T* list_to_pointer(const initializer_list<T>& list)
{
	T* ptr = new T[list.size()];
	copy(list.begin(), list.end(), ptr);
	return ptr;
}
template long long* list_to_pointer(const initializer_list<long long>& list);

long long* to_pointer(const initializer_list<long long>& list)
{
	long long* ptr = new long long [list.size()];
	copy(list.begin(), list.end(), ptr);
	return ptr;
}




//用于vm内部抹去call，请使用list_to_pointer，作用相同
 template<typename T>
 __forceinline T* jmp_list_to_pointer(const initializer_list<T>& list)
 {
	 __asm
	 {
		 push list
		 push func_end
		 jmp to_pointer
		 func_end:
	 }
 }



namespace protect
{
	long long param[11] = {};
	long long params = 0;



	long long rand_n = 0;
	long long kernel_key = 123456;
	SECTION(.mizuki) void initialize(long long key)
	{
		last_error_code = success;
		if (kernel_key != 123456) {
			last_error_code = error_initial_twice;
			return;
		}
		srand((unsigned int)(time(nullptr)));
		kernel_key = key;
		//anti_debug();

	}
	// void push_param(const long long& param1, const long long& param2)
	// {
	// 	int_param1 = param1;
	// 	int_param2 = param2;
	// }




	/**
	 * \brief		打印不能直接被中文搜索引擎搜到的字符串
	 * \param func	自己的处理字符串的函数，可为空
	 * \param str	要加密的字符串
	 */

	SECTION(.mizuki) void print_string(void(*func)(char* str), char* str)
	{
		if (func != nullptr) func(str);
		if (str[0] >= 0)printf("{'\\x%x'", str[0]);
		else printf("{-0x%x", -str[0]);
		for (int i = 1; i < strlen(str); ++i)
		{
			if (str[i] >= 0) printf(",'\\x%x'", str[i]);
			else printf(",-0x%x", -str[i]);
		}
		printf(",0}\n");
	}





	SECTION(.mizuki) long long get_rand(const long long& l, const long long& r)
	{
		rand_n = l + rand() % (r - l + 1);
		return rand_n;
	}


	SECTION(.mizuki) long long nor_(const long long& n, const long long& m)
	{
		return ~(n | m);
	}
	SECTION(.mizuki) long long nand_(const long long& n, const long long& m)
	{
		return ~(n & m);
	}
	SECTION(.mizuki) long long not_(const long long& n, protect_type type)
	{
		const protect_type org = type;
		while (1)
		{
			switch (type)
			{
			case type_default:
				type = type_nor;
				continue;
			case type_rand:
				while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
				continue;
			case type_nor:
				return nor_(n, n);
			case type_nand:
				return nand_(n, n);
			default:
				last_error_code = error_invalid_protect_type;
				return -1;
			}

		}
	}
	SECTION(.mizuki) long long and_(const long long& n, const long long& m, protect_type type)
	{
		const protect_type org = type;
		while (1)
		{
			switch (type)
			{
			case type_default:
				type = type_nor;
				continue;
			case type_rand:
				while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
				continue;
			case type_nor:
				return nor_(not_(n), not_(m));
			case type_nand:
				return not_(nand_(n, m), org);
			default:
				last_error_code = error_invalid_protect_type;
				return -1;
			}

		}

		// if (type == type_default) type = type_nor;
		// const protect_type org = type;
		// if (type == type_rand)
		// {
		// 	while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
		// }
		// if (type == type_nor) return nor_(not_(n), not_(m));
		// else if (type == type_nand) return not_(nand_(n, m), org);
		// last_error_code = error_invalid_protect_type;
		// return -1;
	}
	SECTION(.mizuki) long long or_(const long long& n, const long long& m, protect_type type)
	{
		const protect_type org = type;
		while (1)
		{
			switch (type)
			{
			case type_default:
				type = type_nor;
				continue;
			case type_rand:
				while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
				continue;
			case type_nor:
				return not_(nor_(n, m), org);
			case type_nand:
				return nand_(not_(n), not_(m));
			default:
				last_error_code = error_invalid_protect_type;
				return -1;
			}

		}


		// if (type == type_default) type = type_nor;
		// const protect_type org = type;
		// if (type == type_rand)
		// {
		// 	while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
		// }
		// if (type == type_nor) return not_(nor_(n, m), org);
		// else if (type == type_nand) return nand_(not_(n), not_(m));
		// last_error_code = error_invalid_protect_type;
		// return -1;
	}
	SECTION(.mizuki) long long xor_(const long long& n, const long long& m, protect_type type)
	{
		const protect_type org = type;
		while (1)
		{
			switch (type)
			{
			case type_default:
				type = type_nor;
				continue;
			case type_rand:
				while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
				continue;
			case type_nor:
				return or_(nor_(not_(n), m), nor_(n, not_(m)), org);
			case type_nand:
				return nand_(not_(and_(n, not_(m, org), org)), not_(and_(not_(n, org), m, org)));
			default:
				last_error_code = error_invalid_protect_type;
				return -1;
			}

		}
		// if (type == type_default) type = type_nor;
		// const protect_type org = type;
		// if (type == type_rand)
		// {
		// 	while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
		// }
		// if (type == type_nor) return or_(nor_(not_(n), m), nor_(n, not_(m)), org);
		// else if (type == type_nand) return nand_(not_(and_(n, not_(m, org), org)), not_(and_(not_(n, org), m, org)));
		// last_error_code = error_invalid_protect_type;
		// return -1;
	}
	SECTION(.mizuki) long long xnor_(const long long& n, const long long& m, protect_type type)
	{
		const protect_type org = type;
		while (1)
		{
			switch (type)
			{
			case type_default:
				type = type_nor;
				continue;
			case type_rand:
				while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
				continue;
			case type_nor:
				return not_(xor_(n, m), org);
			case type_nand:
				return not_(xor_(n, m), org);
			default:
				last_error_code = error_invalid_protect_type;
				return -1;
			}

		}


		// if (type == type_default) type = type_nor;
		// const protect_type org = type;
		// if (type == type_rand)
		// {
		// 	while (type == type_rand || type == type_default) type = (protect_type)get_rand(0, protect_types - 1);
		// }
		// if (type == type_nor) return not_(xor_(n, m), org);
		// else if (type == type_nand) return not_(xor_(n, m), org);
		// last_error_code = error_invalid_protect_type;
		// return -1;
	}

	SECTION(.mizuki) long long shl_(const long long& n, const long long& m)
	{
		return n << m;
	}
	SECTION(.mizuki) long long shr_(const long long& n, const long long& m)
	{
		//111000110   n<<1
		//011100011   n
		//异或得到 n<<1与n不同的位
		//mask/2得到n与n>>1不同的位
		//xor得到结果

		last_error_code = success;
		long long result = n;
		for (int i = 1; i <= m; ++i)
		{
			long long sh = shl_(result, 1);
			long long mask1 = xor_(sh, result);
			mask1 /= 2;
			result = xor_(result, mask1);
		}
		return result;
	}

	SECTION(.mizuki) long long add_(const long long& n, const long long& m, const protect_type& type)
	{
		last_error_code = success;
		const protect_type org = type;
		return xor_(n, m, org) + shl_(and_(n, m, org), 1);
	}
	SECTION(.mizuki) long long sub_(const long long& n, const long long& m, const protect_type& type)
	{
		last_error_code = success;
		return add_(n, -m, type);
	}
	SECTION(.mizuki) long long mul_(const long long& n, const long long& m, const protect_type& type)
	{
		//a*b=(a/2)*(b*2)	注意除不尽的情况
		last_error_code = success;
		const protect_type org = type;
		long long result = 0;
		long long a = n;
		long long b = m;

		while (1)
		{
			switch (b)
			{
			case 0:
				return 0;
			case 1:
				return add_(result, a, org);
			case -1:
				return add_(result, -a, org);
			default:
				//m为奇数
				if (and_(b, 1, org) == 1)result += a;
				a = shl_(a, 1);
				b = shr_(b, 1);
			}
		}

		// while (abs(b) >= 2)
		// {
		// 	//m为奇数
		// 	if (and_(b, 1, org) == 1) {
		// 		result += a;
		// 		a = shl_(a, 1);
		// 		b = shr_(b, 1);
		// 	}
		// 	else {
		// 		a = shl_(a, 1);
		// 		b = shr_(b, 1);
		// 	}
		// 	shr_(b, 1);
		// }
		// if (b == 0) return 0;
		// else if (b == 1) return add(result, a, org);
		// else if (b == -1) return add(result, -a, org);
		// last_error_code = error_impossible;
		// return -1;
	}

	/**
	 * \brief 小于返回-1，等于返回0，大于返回1
	 */
	SECTION(.mizuki) long long cmp_(const long long& n, const long long& m, const protect_type& type)
	{
		long long a = n;
		long long b = m;
		//if(a>=b) swap(a,b);

		//	not[(a-b)*3]<0	即a>=b
		long long tmp = not_(mul_(sub_(a, b, type), 3, type));
		if (tmp < 0 && tmp != -1)
		{
			return 1;
		}
		//[(a-b)*4]>>1 <0	即a<b
		if (shr_(mul_(sub_(a, b, type), 4, type), 1) < 0)
		{
			return -1;
		}
		return 0;
	}

	//请使用宏而不是直接调用if_
	SECTION(.mizuki) extern void if_(const long long& expression, void* address, const protect_type& type)
	{
		last_error_code = success;
		long long result = 0x11111111;
		result = cmp_(expression, 0, type);
		//如果下面的if被爆破，address会改变,程序会崩溃。只有cmp返回相等，result=0时地址才是正确的（与0异或不改变）
		address = (void*)xor_((long long)address, result, type);
		if (result) return;
		else if (result == 0)
		{
			//address=(void*)((long long)address^result);
			address = (void*)xor_((long long)address, result, type);
			__asm
			{
				mov eax, address;
				leave;
				push eax;
				mov eax, 0;
				retn 4;
			}
		}
		last_error_code = error_impossible;
	}

	/**
	 * \brief 质数的case是有用的，合数的case会被反复执行是无用指令\n
	 * 2-nor 3-nand 5-not 7-and 11-or 13-xor 17-xnor 19-shl 23-shr 29-add 31-sub 37-mul 41-cmp
	 *
	 * \param key	操作密钥
	 * \param arg	参数数组
	 * \param n		参数数量(暂时不需要)
	 */
	SECTION(.mizuki) long long vm(const op_key key, long long* arg, const long long& n)
	{
		long long op = 0;
		long long org_op = 0;
		long long result = 0;
		IF_ELSE_BEGIN(!(cmp_(key, 0) == 0 || cmp_(key, 1) == 0), special_judge)
		{
			//不是0或1，解密操作码
			for (long long i = 2; i <= 100; ++i)
			{
				IF_ELSE_BEGIN(!(key % i == 0), get_org_op)
				{
					//混淆（垃圾指令）
					op = 1;
					org_op = i;
					params = 1;
					op = i;
					org_op *= i;
					params = op;
					op = i + 1;
					org_op = op;
					op = i;
					org_op = i;
					params = 0;
					//---------------
				}
				ELSE(get_org_op)
				{
					op = i;
					org_op = i;
					goto vm_circulation_start;
				}
				IF_ELSE_END()
			}
		
		}
		ELSE(special_judge)
		{
			op=key;
			org_op=0;
			goto vm_circulation_start;
		}
		IF_ELSE_END()

		//调试----------------------------------------------------
		// if(!(cmp_(key, 0) == 0 || cmp_(key, 1) == 0))
		// {
		// 	//不是0或1，解密操作码
		// 	for (long long i = 2; i <= 100; ++i)
		// 	{
		// 		if(!(key % i == 0))
		// 		{
		// 			//混淆（垃圾指令）
		// 			op = 1;
		// 			org_op = i;
		// 			params = 1;
		// 			op = i;
		// 			org_op *= i;
		// 			params = op;
		// 			op = i + 1;
		// 			org_op = op;
		// 			op = i;
		// 			org_op = i;
		// 			params = 0;
		// 			//---------------
		// 		}
		// 		else
		// 		{
		// 			op = i;
		// 			org_op = i;
		// 			goto vm_circulation_start;
		// 		}
		// 	}
		//
		// }
		// else
		// {
		// 	op=key;
		// 	org_op=0;
		// 	goto vm_circulation_start;
		// }
		//------------------------------------------------------


	vm_circulation_start:
		while (1)
		{
			switch (op)
			{
			case 0:
			{
				//虚拟机本身死循环+循环死循环
				while(1){}
				break;
			}
			case 1:
			{
				//虚拟机死循环+程序崩溃退出
				_asm {
					xor eax, eax
					xor ecx, ecx
					xor edx, edx
					xor ebx, ebx
					xor esp, esp
					xor ebp, ebp
					xor edi, edi
					xor esi, esi
				}
				break;
			}
			case 2:
			{
				//nor
				result = nor_(arg[0], arg[1]);
				insert_junk_code_1();
				break;
			}
			case 3:
			{
				//nand
				result = nand_(arg[0], arg[1]);
				break;
			}
			case 4:
			{
				break;
			}
			case 5:
			{
				//not
				result = nand_(arg[0], arg[0]);
				break;
			}
			case 6:
			{
				break;
			}
			case 7:
			{
				//and
				//not_(nand_(n, m));
				result = jmp_vm(op_not, jmp_list_to_pointer({ jmp_vm(op_nand, arg, 2) ,(long long)0}), 1);
				break;
			}
			case 8:
			{
				insert_junk_code_1();
				break;
			}
			case 9:
			{
				break;
			}
			case 10:
			{
				break;
			}
			case 11:
			{
				//or
				//return nand_(not_(n), not_(m));
				result = jmp_vm(op_nand, jmp_list_to_pointer({
					jmp_vm(op_not,jmp_list_to_pointer({arg[0] ,(long long)0}),1),
					jmp_vm(op_not,jmp_list_to_pointer({arg[1] ,(long long)0}),1)
				}), 2);
				break;
			}
			case 12:
			{
				insert_junk_code_1();
				break;
			}
			case 13:
				//xor_
				//nand_(not_(and_(n, not_(m))), not_(and_(not_(n), m)));
				result = jmp_vm(op_nand, jmp_list_to_pointer({
					jmp_vm(op_not,jmp_list_to_pointer({
						jmp_vm(op_and,jmp_list_to_pointer({
							arg[0],
							jmp_vm(op_not,jmp_list_to_pointer({arg[1]}),1)
						}),2),
					}),1),
					jmp_vm(op_not,jmp_list_to_pointer({
						jmp_vm(op_and,jmp_list_to_pointer({
							jmp_vm(op_not,jmp_list_to_pointer({arg[0]}),1),
							arg[1]
						}),2) ,
					}),1)
				}), 2);

			case 14:
			{
				do {
					__asm pushfd
					__asm pushad
					do {
						__asm {
							xor eax, eax
							sub eax, ecx
							xor ecx, ecx
							sub ecx, edx
							mov edi, 7
							sub edx, ebx
							mov esi, 8
							xor edx, edx
							sub ebx, esp
							xor ebx, ebx
							mov ebp, 6
							sub ebp, esi
							xor ebp, ebp
							mov edx, 3
							sub esi, edi
							mov ebx, 4
							xor edi, edi
							sub edi, eax
							xor esi, esi
							mov eax, 1
							mov ecx, 2
						}
						do {
							__asm popad
							__asm popfd
							break;
						} while (1);
						break;
					} while (1);
					break;
				} while (1);
				break;
			}
			case 15:
			{
				for (int i = 1; i <= 100; i += 3)
				{
					for (int j = 50; j >= 1; j -= 2)
					{
						for (int k = 1; k <= 10; k += 1)
						{
							__asm nop;
						}
					}
				}
				break;
			}
			case 16:
			{
				__asm
				{
					jmp label_16
					mov eax, 0xfedcba98;
					push eax;
					jmp eax;
					call eax;
					mov[eax], 0;
				label_16:
				}
				break;
			}
			case 17:
				//xnor
				//not_(xor_(n, m));
				result = jmp_vm(op_not, jmp_list_to_pointer({
					jmp_vm(op_xor,jmp_list_to_pointer({
						arg[0],
						arg[1]
					}),2) ,
					(long long)0
				}), 1);
				break;
			case 18:
			{
				__asm {
					jg vm_circulation_end;
					jng vm_circulation_end;
					xor eax, eax
						xor ecx, ecx
						xor edx, edx
						xor ebx, ebx
						xor esp, esp
						xor ebp, ebp
						xor edi, edi
						xor esi, esi
				}
				break;
			}
			case 19:
				//result = shl_(param1,param2);
				result = shl_(arg[0], arg[1]);
				break;
			case 20:
			{
				insert_junk_code_1();
				break;
			}
			case 21:
			{
				__asm {
					je vm_circulation_end;
					jne vm_circulation_end;
					__emit 0x01
						__emit 0x02
						__emit 0x03
						__emit 0x04
						__emit 0x05
						__emit 0x06
						__emit 0x07
						__emit 0x08
						__emit 0x09
						__emit 0x00
				}
				break;
			}
			case 22:
			{
				break;
				break;
				break;
				break;
				break;
				break;
			}
			case 23:
			{
				//result = shr_(param1,param2);
				long long temp_result = arg[0];
				long long sh =0;
				long long mask1=0;
				for (long long i = 1; i <= arg[1]; ++i)
				{
					sh = jmp_vm(op_shl, jmp_list_to_pointer({ temp_result,long long(1) }), 2);
					mask1 = jmp_vm(op_xor, jmp_list_to_pointer({ sh,temp_result }), 2);
					mask1 >>=1;
					temp_result=jmp_vm(op_xor, jmp_list_to_pointer({ temp_result,mask1 }), 2);
				}
				result = temp_result;
				break;
			}
			case 24:
			{
				break;
			}
			case 25:
			{
				break;
			}
			case 26:
			{
				insert_junk_code_1();
				break;
			}
			case 27:
			{
				break;
				break;
				break;
				break;
				break;
				break;
			}
			case 28:
			{
				__asm {
					pushad
					xor eax, eax
					xor ecx, ecx
					xor edx, edx
					xor ebx, ebx
					xor esi, esi
					xor edi, edi
					je case_28
					__emit 0x00
					case_28:
					popad
				}
				break;
			}
			case 29:
			{
				//result = add_(param1,param2,type_rand);
				//xor_(n, m) + shl_(and_(n, m), 1);

				result =jmp_vm(op_xor, arg, 2)  + jmp_vm(op_shl, jmp_list_to_pointer({
					jmp_vm(op_and,arg,2),
					(long long)1
				}), 2);
				break;
			}
			case 30:
			{
				__asm {
					push ebx
					xor ebx, ebx
					je case_30
					__emit 0xE8
					case_30:
					pop ebx
				}
				break;
			}
			case 31:
				result = jmp_vm(op_add, jmp_list_to_pointer({ arg[0],-arg[1] }), 2);
				break;
			case 32:
			{
				insert_junk_code_1();
				break;
			}
			case 33:
			{
				break;
				break;
				break;
				break;
				break;
				break;
			}
			case 34:
			{
				__asm {
					je vm_circulation_end;
					jne vm_circulation_end;
					__emit 0x55
					__emit 0x66
					__emit 0x77
					__emit 0x88
				}
				break;
			}
			case 35:
			{
				insert_junk_code_1();
				break;
			}
			case 36:
			{
				__asm {
					push eax
					xor eax, eax
					je case_36
					__emit 0xE8
					case_36:
					pop eax
				}
				break;
			}
			case 37:
			{
				//result = mul_(param1,param2,type_rand);
				result=0;
				long long a=arg[0];
				long long b=arg[1];
				while(1)
				{
					switch(b)
					{
					case 0:
						result=0;
						goto vm_circulation_end;
					case 1:
						result=jmp_vm(op_add, jmp_list_to_pointer({ result,a }), 2);
						goto vm_circulation_end;
					case -1:
						result=jmp_vm(op_sub, jmp_list_to_pointer({ result,a }), 2);
						goto vm_circulation_end;
					default:
						if(jmp_vm(op_and, jmp_list_to_pointer({ b,(long long)1 }), 2) == 1)
						{
							result+=a;
						}
						a=jmp_vm(op_shl, jmp_list_to_pointer({ a,(long long)1 }), 2);
						b=jmp_vm(op_shr, jmp_list_to_pointer({ b,(long long)1 }), 2);
					}
				}
				break;
			}
			case 38:
			{
				__asm {
					jz vm_circulation_end;
					jnz vm_circulation_end;
					__emit 0x11;
					__emit 0x22;
					__emit 0x33;
					__emit 0x44;
				}
				break;
			}
			case 39:
			{
				__asm jz vm_circulation_end;
				__asm jnz vm_circulation_end;
				break;
			}
			case 40:
			{
				insert_junk_code_1();
				break;
			}
			case 82:		//最大质数的两倍结束
			{
				//vm结束
				return result;
			}
			default:
				org_op+=1;
				op = org_op;
			}
		vm_circulation_end:
			op += org_op;
		}
	}



	/**
	 * \brief 此函数是vm内部衔接，抹去call指令用的，请使用函数vm而不是jmp_vm
	 * \param key
	 * \param arg
	 * \param n
	 * \return
	 */
	SECTION(.mizuki) __forceinline long long jmp_vm(const op_key key, long long* arg, const long long& n)
	{
		__asm						//传递参数
		{
			push n
			push arg
			push key
			push vm_end				//直接push返回地址再jmp，相当于call但是把返回地址换了
			jmp vm
			//-----垃圾指令---

			//----------------
		vm_end:
			//如果内联,不需要操作;如果不内联，需要平衡堆栈与返回
			//此处需要内联
			//leave
			//retn
		}
	}



	SECTION(.mizuki)  long long add(const long long& n, const long long& m)
	{
		param[0] = n;
		param[1] = m;
		params = 2;

		return vm(op_add, param, params);
	}
	SECTION(.mizuki)  long long sub(const long long& n, const long long& m)
	{
		param[0] = n;
		param[1] = m;
		params = 2;
		return vm(op_sub, param, params);
	}
	SECTION(.mizuki)  long long mul(const long long& n, const long long& m)
	{
		param[0] = n;
		param[1] = m;
		params = 2;
		return vm(op_mul, param, params);
	}


	SECTION(.mizuki) void print_last_error()
	{
		printf("last_error_code = %d\n", last_error_code);
	}
}







/**
 * \brief 插入一段花指令，让两个函数在花指令中间执行
 *
 * 参数：两个void类型的函数指针，让函数在花指令中间执行
 * 参数为空则相当于直接插入一长段无用花指令
 * 注意：不是给函数里面的代码插入花指令
 * 流程：开始 -> insert_junk_code_1_part2 -> 返回到标签back -> 跳转到 to_end -> 跳转到 real_end ->next_user_code
 */
void insert_junk_code_1(void(*func1)(), void(*func2)())
{
	int ret_address = 0;
	__asm
	{
		pushad;						//保存当前寄存器
		mov edx, back;
		mov ret_address, 0x114514;	//垃圾指令
		mov ret_address, edx;		//取真正返回地址back
		push ret_address;			//将真正返回地址压栈
		mov ret_address, 0x114514;	//垃圾指令

		mov edx, func1;				//将func1传递给下面函数执行
		call  insert_junk_code_1_part2;

		//任意输入垃圾代码----------------
		pushad;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		lea eax, [edx];
		shr edx, 66;
		call back;
		ret;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		nop
			popad;
		ret 0x114514;

		//--------------------------------

	back:
		mov eax, 0xFFFFFFFF;		//垃圾指令
		xor eax, eax;
		je to_end;				//恒成立跳转，跳向花指令即将结束
		//任意输入垃圾代码----------------
		mov ecx, 0x68e872;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		in eax, dx;
		pushad;
		push func1;
		call eax;
		mov ecx, 0xCC5dff25;
		in eax, dx;
		mov edx, 0x5F5E;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		popad;
		int 3;
		mov ecx, 0xff255f5e;
		ret 0xfe;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0xE8; __emit 0x00;
		__emit 0x76; __emit 0x25; __emit 0x98; __emit 0x33; __emit 0x68; __emit 0x69;
		//--------------------------------
	to_end:
		push next_user_code;
		push func2;
		jmp ebx;

	next_user_code:
		popad;
		//花指令结束，这接着就是后面的代码了
	}

};

void insert_junk_code_1_part2()
{
	void(*func1)() = nullptr;
	void(*func2)() = [](){};	//用nullptr在pop func2时会覆盖上层函数的局部变量，原因未知，换为lambda没有问题
	//--自定义1 可以输入初始化代码或垃圾代码-----------------
	__asm
	{
		push edx;

		//添加你的花指令（不是任意指令）------
		push 0x114514
		mov ebx, 0x1919810;
		mov ecx, 0x5201314;
		mov edx, 0x654321;
		sub edx, ecx;
		sub ebx, eax;
		pop eax;
		//------------------------------------

		pop esi;				//esi保存edx传递的func1地址，从这里开始不要操作esi

		//添加你的花指令-----------------------
		mov eax, 0xffffffff;
		push eax;
		mov ebx, 0x1;
		mov ecx, 0x2;
		sub ebx, eax;

		cmp ecx, ebx;	//一定相等
		//jnz FFFFFFFF	必定不会实现
		//0F 85 AD B3 BF FF
		__emit 0x0F;
		__emit 0x85;
		__emit 0xFF;
		__emit 0xFF;
		__emit 0xFF;
		__emit 0xFF;
		//------------

		pop eax;
		//----------------------------------------

		mov func1, esi;
	}

	if (func1 != nullptr) {
		func1();
	}

	//-------------------------------------------------------


	__asm {
		leave;
		pop eax;			//堆栈平衡,因为不返回原来地址
		mov ebx, real_end;	//将结束地址放入ebx
		retn;
		//任意输入垃圾代码------------------
		pushad;
		ret;
		in eax, dx;
		pushad;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		lea eax, insert_junk_code_1_part2;
		call eax;
		mov ecx, 00011451;
		mov edx, 41919810;
		ret;
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		popad;
		ret 0x114514;
		popad;
		ret 114514;
		__emit 0x3f; __emit 0xa5; __emit 0x11; __emit 0x77; __emit 0x68; __emit 0x69;

		//-----------------------------------
	}


	//自定义2 可以输入初始化代码---------------------
	
	__asm {
	real_end:
		pop func2;
	}
	if (func2 != nullptr) func2();


	//-------------------------------------------------

	__asm {
		ret;

		//下面输入任意垃圾代码-----
		__emit 0xff; __emit 0x25; __emit 0xE8; __emit 0xE8; __emit 0x68; __emit 0x69;
		__emit 0xf5; __emit 0x20; __emit 0xE8; __emit 0xE9; __emit 0x66; __emit 0x69;
		//-------------------------
	}
}





void anti_debug()
{
	insert_junk_code_1(check_debugger_1, check_debugger_2);
	insert_junk_code_1(check_debugger_3, check_debugger_4);
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)check_debugger_1, NULL, NULL, NULL);
	// CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)check_debugger_2,NULL,NULL,NULL);
	// CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)check_debugger_3,NULL,NULL,NULL);
	// CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)check_debugger_4,NULL,NULL,NULL);

}

void check_debugger_1()
{
	if (check_debugger_method_1())
	{
		insert_junk_code_1(exit_1, exit_2);
		exit_1();
		exit_2();
		insert_junk_code_1(exit_3, exit_4);
		exit_3();
		exit_4();
	}
}

void check_debugger_2()
{
	if (check_debugger_method_2())
	{
		insert_junk_code_1(exit_1, exit_2);
		exit_1();
		exit_2();
		insert_junk_code_1(exit_3, exit_4);
		exit_3();
		exit_4();
	}
}
void check_debugger_3()
{
	if (check_debugger_method_3())
	{
		// exit_1();
		// exit_2();
		// exit_3();
		// exit_4();

		//假装没有发现被调试，修改了kernel_key，让后续解密失败
	}
}
void check_debugger_4()
{
	if (check_debugger_method_4())
	{
		insert_junk_code_1(exit_1, exit_2);
		exit_1();
		exit_2();
		insert_junk_code_1(exit_3, exit_4);
		exit_3();
		exit_4();
	}
}
bool check_debugger_method_1()
{
	__try
	{
		__asm int 3;
	}
	__except (1)
	{
		return false;
	}
	//运行环境异常
	char text[] = { '\xD4' ^ 0x55,'\xCB' ^ 0x55,'\xD0' ^ 0x55,'\xD0' ^ 0x55,'\xBB' ^ 0x55,'\xB7' ^ 0x55,'\xBE' ^ 0x55,'\xB3' ^ 0x55,'\xD2' ^ 0x55,'\xEC' ^ 0x55,'\xB3' ^ 0x55,'\xA3' ^ 0x55,'\x0' };
	//错误
	char title[] = { '\xB4' ^ 0x55,'\xED' ^ 0x55,'\xCE' ^ 0x55,'\xF3' ^ 0x55,'\x0' };
	for (int i = 0; i < strlen(text); ++i) text[i] ^= 0x55;
	for (int i = 0; i < strlen(title); ++i) title[i] ^= 0x55;
	MessageBoxA(0, text, title, MB_ICONERROR | MB_TOPMOST | MB_OK);
	return true;
}
bool check_debugger_method_2()
{
	__try
	{
		RaiseException(DBG_PRINTEXCEPTION_C, 0, 0, 0);
	}
	__except (GetExceptionCode() == DBG_PRINTEXCEPTION_C)
	{
		return false;
	}
	//运行环境异常
	char text[] = { '\xD4' ^ 0x55,'\xCB' ^ 0x55,'\xD0' ^ 0x55,'\xD0' ^ 0x55,'\xBB' ^ 0x55,'\xB7' ^ 0x55,'\xBE' ^ 0x55,'\xB3' ^ 0x55,'\xD2' ^ 0x55,'\xEC' ^ 0x55,'\xB3' ^ 0x55,'\xA3' ^ 0x55,'\x0' };
	//错误
	char title[] = { '\xB4' ^ 0x55,'\xED' ^ 0x55,'\xCE' ^ 0x55,'\xF3' ^ 0x55,'\x0' };
	for (int i = 0; i < strlen(text); ++i) text[i] ^= 0x55;
	for (int i = 0; i < strlen(title); ++i) title[i] ^= 0x55;
	MessageBoxA(0, text, title, MB_ICONERROR | MB_TOPMOST | MB_OK);
	return true;
}



bool check_debugger_method_3()
{
	DWORD pid = GetCurrentProcessId();
	DWORD  ppid = getParentPID(pid);
	int e = 0;
	char fname[MAX_PATH] = { 0 };
	//char explorer1[]="explorer";
	char explorer1[] = { '\x65' ,'\x78' ,'\x70' ,'\x6C' ,'\x6F' ,'\x72' ,'\x65' ,'\x72','\x0' };
	//char explorer2[]="EXPLORER";
	char explorer2[] = { '\x45' ,'\x58' ,'\x50' ,'\x4C' ,'\x4F' ,'\x52' ,'\x45' ,'\x52','\x0' };
	e = getProcessName(ppid, fname, MAX_PATH);

	// if(strstr(fname,explorer1)!=nullptr||strstr(fname,explorer2)!=nullptr) return false;	
	// else    //父进程不是explorer，可能被调试
	// {
	// 	protect::kernel_key=0;
	// }

	{
		using namespace protect;
		void* p = nullptr;
		GET_LABEL_ADDRESS(p, label);
		if_(!(strstr(fname, explorer1) != nullptr || strstr(fname, explorer2) != nullptr), p, type_rand);
		{
			protect::kernel_key = 0;
			return true;
		}
	label:
		return false;
	}
}



bool check_debugger_method_4()
{
	BOOL bDebug = false;
	HANDLE hProcess = GetCurrentProcess();
	CheckRemoteDebuggerPresent(hProcess, &bDebug);
	if (bDebug)
	{
		//char text[]="应用程序正常初始化(0xc00000005)失败，请单击\"确定\",终止应用程序";
		char text[] = { '\xD3','\xA6' ,'\xD3' ,'\xC3' ,'\xB3' ,'\xCC' ,'\xD0' ,'\xF2' ,'\xD5' ,'\xFD' ,'\xB3' ,'\xA3' ,'\xB3' ,'\xF5' ,'\xCA' ,
			'\xBC' ,'\xBB' ,'\xAF' ,'\x28' ,'\x30' ,'\x78' ,'\x43' ,'\x30' ,'\x30' ,'\x30' ,'\x30' ,'\x30' ,'\x30' ,'\x30' ,'\x35' ,'\x29' ,'\xCA',
			'\xA7' ,'\xB0' ,'\xDC' ,'\xA3' ,'\xAC' ,'\xC7' ,'\xEB' ,'\xB5' ,'\xA5' ,'\xBB' ,'\xF7' ,'\x22' ,'\xC8' ,'\xB7' ,'\xB6' ,
			'\xA8'  ,'\x22' ,'\xA3','\xAC' ,'\xD6' ,'\xD5' ,'\xD6' ,'\xB9' ,'\xD3' ,'\xA6' ,'\xD3' ,'\xC3' ,'\xB3' ,'\xCC' ,'\xD0','\xF2','\x0'
		};
		//char title[]="应用程序错误";
		char title[] = { '\xD3' ,'\xA6' ,'\xD3' ,'\xC3' ,'\xB3' ,'\xCC' ,'\xD0' ,'\xF2' ,'\xB4' ,'\xED' ,'\xCE' ,'\xF3','\x0' };
		MessageBoxA(0, text, title, MB_ICONERROR | MB_TOPMOST | MB_OK);
		return true;
	}
	return false;
}









void exit_1()
{
	__asm
	{
		xor eax, eax;
		xor ecx, ecx;
		xor edx, edx;
		xor ebx, ebx;
		xor ebp, ebp;
		xor esp, esp;
		xor esi, esi;
		xor edi, edi;
	}

}

void exit_2()
{
	__asm
	{
		mov eax, 0xfedcba98;
		push eax;
		jmp eax;
		call eax;
		mov[eax], 0;
	}
}

void exit_3()
{
	insert_junk_code_1([]() {exit(0); ExitProcess(0); }, []() {TerminateProcess(GetCurrentProcess(), 0); });
}

void exit_4()
{
	__asm
	{
		int 3;
		int 3;
	}
}












DWORD getParentPID(DWORD pid)
{
	HANDLE h = NULL;
	PROCESSENTRY32 pe = { 0 };
	DWORD ppid = 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(h, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
			{
				ppid = pe.th32ParentProcessID;
				break;
			}
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);
	return (ppid);
}

int getProcessName(DWORD pid, LPSTR fname, DWORD sz)
{
	HANDLE h = NULL;
	int e = 0;
	h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (h)
	{
		if (GetModuleFileNameEx(h, NULL, fname, sz) == 0) e = GetLastError();
		CloseHandle(h);
	}
	else e = GetLastError();
	return (e);
}