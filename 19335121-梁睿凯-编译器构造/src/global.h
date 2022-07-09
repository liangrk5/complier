#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <vector>

#ifndef GLOBAL_H
#define GLOBAL_H

/* 预定义量 */
#define tmax 100        //符号表最大容量
#define al 10           //符号的最大长度
#define nmax 5          //整数的最大位数
#define tvmax 500       //最多能够申请到的临时变量
#define norw 13         //关键字个数
#define IRmax 500       //最多的中间代码数
#define levmax 10       //最深的嵌套程度

FILE* fin;
FILE* fout;
void init();


/* 词法分析 */
enum tokenkind {
	errtoken, endfile, id, num, letter, arrtoken,
	addtoken, subtoken, multoken, divtoken, ltctoken,
	gtctoken, eqctoken, comma, semicolon, lparen,
	rparen, lbrace, rbrace, quote, becomes,
	iftoken, elsetoken, thentoken, chartoken, booltoken,
	inttoken, falsetoken, truetoken, rettoken, writetoken,
	readtoken, whiletoken, dotoken,
};

struct tokenstruct
{
	enum tokenkind kind;
	union
	{
		char idname[al + 1];
		int numval;
		char ch;
	} val;
};


struct tokenstruct token;       //词法分析的当前token
int numval;                     //词法分析的当前常量值

int lookup(char* idname);       //确认一个标识符是否是关键字
bool isdigit(char c);           
bool isalpha(char c);
void gettoken();                //读一个单词
char reservedword[norw][al];    //保留字
enum tokenkind wkind[norw];     //保留字对应的符号
enum tokenkind skind[256];      //单字符的符号值，散列表，哈希映射



enum idform { var, proc, fun };     //标识符类型
enum datatype                       //程序中允许声明的符号类型
{
	nul,                            //用于proc的标识符，proc没有数据类型
	inttype,
	chartype,
	booltype
};

/* 符号表结构 */
struct tablestruct
{
	char name[al];                  //符号的名字
	enum idform form;               //标识符的类型，可以通过富豪的数据类型进行区分
	enum datatype type;             //符号的数据类型
	int level;                      //符号所在的层
	int address;                    //符号的地址
};
int tx;                             //符号表指针，取值范围[0,txmax-1]相当于一个标识符计数器
struct tablestruct table[tmax];     //符号表，采用静态数组存储

void enter(enum idform f, enum datatype tk, int lev, int* tx, int* off);    //登记符号表
void printtable(int lev, int stx, int endtx);                               
int found(char* idt, int tx);                                               //在符号表中查找一个标识符


/* 中间代码生成 */
int NextIR;                         //生成的中间代码计数器，取值范围[0,cxmax-1]
enum addr_kind { labelkind, constkind, varkind, tempkind }; //中间代码分量的种类，ARG结构分为：标号，数值，地址

//定义中间代码分量的结构体
struct addrRecord
{
	enum addr_kind  addrkind;
	int  level;                     //记录标识符的层
	union
	{
		int value;                  //记录整型常量值或者记录标号的值或者变量的偏移地址或者过程活动记录的大小
		char c;                     //记录字符常量值
		bool b;                     //记录布尔常量值
	} constant;
	char name[al];	                //变量名
	enum datatype type;             //符号的数据类型
};

enum IRcode_name
{
	ADD, SUB, MUL, DIV, LAB, EQC, LTC, GTC, ASS, JPC, JUMP, CALL, ENTRY, MENTRY, RET, READ, WRITE
}  CodeKind;

//中间代码的结构
struct IRCodeR
{
	IRcode_name IRcodename;
	addrRecord* addr1;
	addrRecord* addr2;
	addrRecord* addr3;
};

//存放中间代码的数组
struct IRCodeR IRcode[IRmax];


/* 临时变量 */
int tempsize;                       //中间代码生成的临时变量空间
int tempnum;                        //中间代码生成的临时变量
struct addrRecord* NewTemp(enum datatype tk);


/* 标号 */
int  NewLabel();
int label;                          //中间代码生成的标号
struct addrRecord* addrLabel(int  lable);


/* 其他语句处理 */
void program();
int block(int lev, int* tx, int* off);
struct addrRecord* addrconst(enum datatype tk);
struct addrRecord* addrvar(int lev, int off, enum datatype tk);
int typematch(struct addrRecord* addr1, struct addrRecord* addr2);
void vardef(enum datatype tk, int lev, int* tx, int* off);
void decls(int lev, int* tx, int* off);
struct addrRecord* bexpr(int* ptx, int lev);
struct addrRecord* expr(int* ptx, int lev);
struct addrRecord* term(int* ptx, int lev);
struct addrRecord* factor(int* ptx, int lev);
int stmts(int lev, int* tx, int* off);
int stmt(int lev, int* tx, int* off);
int CALLstmt(int i);
int ASSstmt(int lev, int* tx, int i);
int IFstmt(int lev, int* tx, int* off);
int WHILEstmt(int lev, int* tx, int* off);
int RETstmt(int lev, int* tx, int* off);
int READstmt(int lev, int* tx, int* off);
int WRITEstmt(int lev, int* tx, int* off);

/* 中间代码生成和显示 */
void genIR(enum IRcode_name, struct addrRecord* ad1, struct addrRecord* ad2, struct addrRecord* ad3);
void PrintIRName(enum IRcode_name cname);
void Printarg(struct addrRecord* addr);
void PrintOneIR(int i);
void PrintIR();
void Printaddr(struct addrRecord* addr);

/* 中间代码优化 */
void IRoptimal(int n);
void constMerge(int i);

/* 错误处理 */
int err;
void error(char* message);


/* 处理临时变量 */
#define   tempmax 100                   //临时变量最大数量
struct temptablestruct
{
	int tnum;                           //符号的编号
	enum datatype type;                 //符号的数据类型
	int address;                        //符号的地址
};
int temptx;                             //临时变量符号表指针
struct temptablestruct temptable[tempmax];
int tempoff;


/* 跳转指令 */
#define   jpmax 100                     //跳转指令最大数量
enum jpkind { labk, tempk };
struct jptablestruct
{
	int pending;                        //不完整的目标指令
	int lab;                            //跳转的目标四元式编号或者临时变量编号
	enum jpkind kind;
};
int jptx;
struct jptablestruct jptable[jpmax];


#define   labmax 100                    //标号最大数量
struct labtablestruct
{
	int lab;                            //label编号
	int taddr;                          //label编号对应的目标代码编号
};

int labtx;                              //标号符号表指针
struct labtablestruct labtable[labmax];


/* 寄存器 */
#define   ISIZE  1024                   //指令存储区大小，定义为1024
#define   DSIZE  1024                   //数据存储区大小，定义为1024
#define   Rnum 8                        //寄存器数量，定义为8
#define   ax 0
#define   bx 1
#define   cx 2
#define   dx 3
#define   flag 4
#define   top 5
#define   bp 6
#define   pc 7
int reg[Rnum];	                        //reg用于寄存器存储，为8长的整数类型数组



/* 目标指令 */
int curin;                              //即将生成的目标指令地址


/* 操作指令标识符类型op r,d,s */
enum objop
{
	opIN,	                            //输入指令：将外部变量读入寄存器
	opOUT,	                            //输出指令：将寄存器的值输出
	opADD,                              //加法指令：将寄存器d的值+寄存器s的值，结果存在寄存器r
	opSUB,                              //减法指令：将寄存器d的值-寄存器s的值，结果存在寄存器r
	opMUL,                              //乘法指令：将寄存器d的值*寄存器s的值，结果存在寄存器r
	opDIV,                              //除法指令：将寄存器d的值/寄存器s的值，结果存在寄存器r
	opPUSH,                             //进栈指令：寄存器r的值压入栈顶，top指针+1
	opPOP,                              //出栈指令：栈顶值存入寄存器r，top指针-1
	opLD,	                            //读内存指令：将地址为d+reg(s)的内存单元的值，存入r
	opST,                               //写内存指令：将寄存器r的值存入d+reg(s)的内存单元
	opLIT,                              //字符常量放入内存
	opLDA,	                            //将立即数d+reg(s)的值放入寄存器r
	opLDC,                              //将立即数d放入寄存器r
	opJNL,                              //如果寄存器flag不小于0，则r
	opJNG,                              //如果寄存器flag不大于0，则r
	opJNE,                              //如果寄存器flag的值不等于0，则r
	opJUMP,                             //跳转到r
	opMOV,                              //寄存器d到寄存器r
};


/* 存储器 */

/* 指令结构类型：操作码，操作数1，操作数2，操作数3 */
typedef struct {
	enum objop iop;
	int iarg1;
	int iarg2;
	int iarg3;
} INSTRUCTION;
INSTRUCTION iMem[ISIZE];		        //iMem用于指令存储
int dMem[DSIZE];	                    //dMem用于数据存储



/* 目标代码主要函数 */
void objcode(int n);                    //目标代码生成
void emit(enum objop iop, int r, int s, int t);
void callGen(int i);
void entryGen(int i);
void retGen();
void binaryopGen(int i);
void sourceOperandGen(struct addrRecord* addr, int reg);
void thirdOperandGen(int i, int r);
void jumpGen(int i);
void jpcGen(int curIR);
void labGen(int i);
void writeGen(int i);
void readGen(int i);

/* 跳转指令函数 */
void enterjp(int taddr, int lab, enum jpkind k);
void enterlab(int lab, int curin);
int foundlab_labtable(int lab);
int foundlab_jptable(int lab, int tx, enum jpkind k);
void backpatchobj(int curin, int lab);

/* 临时变量函数 */
void entertempV(struct addrRecord* addr);
int foundTemp(int tnum);

/* 目标代码显示函数 */
void Printobjcode();
void PrintObject();
void PrintOneObj(int i);
void PrintREGname(int r);

/* 虚拟机函数 */
void vm();                 

#endif