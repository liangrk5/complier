#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

char Keywords[22][10]=
{//关键字表
"double", "int", "struct", "break", "static",
"long", "switch", "case", "char", "return", 
"const", "float", "short", "continue", "for", 
"signed", "void","default", "do", "while",
"if", "else"};
char Symbol[30][10] = 
{//符号表
"+", "-", "*", "/", "=", "==", "!=", ">", "<", ">=",
"<=", ",", ";", "{", "}", "(", ")", "!","&", "&&", 
"|", "||", "^", "#", "%", "[", "]", ".", ":", "$"};
static char word[20];//每次扫描提取到的词

bool IsLetter(char ch)
{//判断是否为字母，包括下划线 
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z'|| ch=='_')
		return true;
	else
		return false;

}
bool IsDigit(char ch)
{//判断是否为数字 
	if (ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}
bool IsSymbol(char ch)
{//判断是否为单字符号 
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ';' 
			|| ch == '(' || ch == ')' || ch == '^' || ch == ',' || ch == '#' 
			|| ch == '%' || ch == '[' || ch == ']' || ch == '{' || ch == '}' 
			|| ch == '.' || ch == ':'|| ch == '$')
		return true;
	else
		return false;
}
int IsKeyword(char word[])
{//判断是否为关键字 
	for(int i=0;i<32;i++)
		if (strcmp(word,Keywords[i])==0) 
			return i + 1;
	return -1;
}
int LexicalAnalyzer(char CurrentCode[], int &cc)
{//词法分析 
	int i, w = 0, cn;
	while (CurrentCode[cc] == ' ')
		cc++;
	for (i = 0; i<20; i++)
		word[i] = '\0';
	if (IsLetter(CurrentCode[cc]))
	{
		word[w] = CurrentCode[cc];
		w++;
		cc++;
		while (IsLetter(CurrentCode[cc]) || IsDigit(CurrentCode[cc]))
		{
			word[w] = CurrentCode[cc];
			w++;
			cc++;
		}
		word[w] = '\0';
		cn = IsKeyword(word);
		if(cn == -1)
		return 33;
		else 
		return cn;
	}
	else if (IsDigit(CurrentCode[cc]))
	{
		while (IsDigit(CurrentCode[cc]))
		{
			word[w] = CurrentCode[cc];
			w++;
			cc++;
		}
		word[w] = '\0';
		return 34; 
	}
	else if (IsSymbol(CurrentCode[cc]))
	{
		word[0] = CurrentCode[cc];
		word[1] = '\0';
		cc++;
		for (i = 0; i<30; i++)
			if (strcmp(word, Symbol[i]) == 0)
				return 35 + i;
	}
	else  if (CurrentCode[cc] == '<')
	{
		cc++;
		if (CurrentCode[cc] == '=')
		{
			cc++;
			return 45;
		}	
	    else
	        return 43;
	}
	else  if (CurrentCode[cc] == '>')
	{
		cc++;
        if (CurrentCode[cc] == '=')
		{
			cc++;
			return 44;
		}
	    
		else
	        return 42;
	}
	else  if (CurrentCode[cc] == '=')
	{
		cc++;
        if (CurrentCode[cc] == '=')
		{
			cc++;
			return 40;
		}
	    
		else
	        return 39;
	}
	else  if (CurrentCode[cc] == '!')
	{
		cc++;
        if (CurrentCode[cc] == '=')
		{
			cc++;
			return 41;
		}
	    
		else
	        return 52;
	}
	else  if (CurrentCode[cc] == '&')
	{
		cc++;
        if (CurrentCode[cc] == '&')
		{
			cc++;
			return 54;
		}
	    
		else
	        return 53;
	}
	else  if (CurrentCode[cc] == '|')
	{
		cc++;
        if (CurrentCode[cc] == '|')
		{
			cc++;
			return 56;
		}
	    
		else
	        return 55;
	}
	else  if (CurrentCode[cc] == '\0')
	{
		cc++; 
		return 0;
	}
	else
	{
		cc++;
		return -2;
	}
}

typedef struct
{//栈的结构体 
    char *base;
    char *top;
    int stacksize;
}Stack;
int InitStack(Stack &S)
{//创造一个空栈
	S.base=(char *)malloc(100*sizeof(char));
	if(!S.base)
    exit(-2);
	S.top=S.base;
	S.stacksize=100;
	return 1;
};
int DestroyStack(Stack &S)
{//销毁栈S
	while(S.top!=S.base)
		free(--S.top);
	return 1;
}
int StackEmpty(Stack S)
{//判断是否为空栈 
    if(S.top==S.base)
        return 1;
    else
        return 0;
}
int GetTop(Stack S,char &e)
{//若栈不为空，取栈顶元素e 
	if(S.base==S.top)
   		return 0;
	e=*(S.top-1);
	return 1;
}
int Push(Stack &S,char e)
{//将e压入栈顶 
	*S.top=e;
	S.top++;
	return 1;
}
int Pop(Stack &S,char &e)
{//若栈不为空，将栈顶元素e出栈 
    if(S.top==S.base)
        return 0;
    e=*--S.top;
    return 1;
}
int StackTraverse(Stack S)
{//遍历栈中的元素
    if(S.base==NULL)
        return -1;
    if(S.base==S.top)
        printf("栈中没有元素\n");
    char *p;
    p=S.top;
    while(p-S.base>0)
    {
        p--;
        printf("%c ",*p);
    }
    printf("\n");
    return 1;
}
int ReverseTraverse(Stack S)
{//遍历输出从栈底至栈顶的元素 
    if(S.base==NULL)
        return -1;
    if(S.base==S.top)
        printf("\t栈中没有元素\n");
    char *p,a[30];
    int i=0;
    p=S.base;
    while(p<S.top)
    {
        a[i]=*p;
        i++;
        p++;
    }
    a[i]='\0';
    printf("\t%-15s",a);
    return 1;
}


char row[17]=
{//SLR分析表中的行头
'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g'
}; 
char column[12]=
{//SLR分析表中的列头 
'i','n','+','-','*','/','(',')','$','E','T','F'
}; 
char TableValue[10];//读取到的SLR分析表中的内容
char SLR1Table[17][12][10]=
{ //将SLR分析表输入进行存储
"s5","s6","e1","e1","e1","e1","s4","e2","e1","1","2","3",
"e3","e3","s7","s8","0","0","e3","e2","acc","0","0","0",
"e3","e3","r3","r3","s9","sa","e3","r3","r3","0","0","0",
"e3","e3","r6","r6","r6","r6","e3","r6","r6","0","0","0",
"s5","s6","e1","e1","e1","e1","s4","e1","e1","b","2","3",
"0","0","r8","r8","r8","r8","e3","r8","r8","0","0","0",
"e3","0","r9","r9","r9","r9","e3","r9","r9","0","0","0",
"s5","s6","e1","e1","e1","e1","s4","e1","e1","0","c","3",
"s5","s6","e1","e1","e1","e1","s4","e1","e1","0","d","3",
"s5","s6","e1","e1","e1","e1","s4","e1","e1","0","0","e",
"s5","s6","e1","e1","e1","e1","s4","e1","e1","0","0","f",
"e3","e3","s7","s8","0","0","e3","sg","e4","0","0","0",
"e3","e3","r1","r1","s9","sa","e3","r1","r1","0","0","0",
"e3","e3","r2","r2","s9","sa","e3","r2","r2","0","0","0",
"e3","e3","r4","r4","r4","r4","e3","r4","r4","0","0","0",
"e3","e3","r5","r5","r5","r5","e3","r5","r5","0","0","0",
"r7","r7","r7","r7","r7","r7","r7","r7","r7","0","0","0"};
char Products[10][10]=
{//按顺序存储表达式 
"S->E","E->E+T","E->E-T","E->T","T->T*F","T->T/F","T->F","F->(E)","F->i","F->n"
};

int ElementSearch(char a[],char e,int length)
{//查找表头，返回其对应的下标
    int i,result=-1;
    for(i=0;i<length;i++)
    {
        if(a[i]==e)
        {
           result=i;
           break;
        }
    }
    return result;
}
int TableSearch(char SLR1Table[17][12][10],char e,char element,int ROW,int COLUMN)
{//从SLR分析表中查找
    int RowIndex=ElementSearch(row,e,ROW);
    int ColIndex=ElementSearch(column,element,COLUMN);
    if(RowIndex==-1 || ColIndex==-1)
        return -1;//异常 
    else
    {
        if(strcmp(SLR1Table[RowIndex][ColIndex],"0")==0)//空，出错 
            return 0;
        else if (SLR1Table[RowIndex][ColIndex][0]=='s')//移进 
        {
        	strcpy(TableValue,SLR1Table[RowIndex][ColIndex]);
        	return 1;
		} 
        	
        else if (SLR1Table[RowIndex][ColIndex][0]=='r')//归约 
        {
        	strcpy(TableValue,SLR1Table[RowIndex][ColIndex]);
        	return 2;
		} 
		else if(strcmp(SLR1Table[RowIndex][ColIndex],"acc")==0)//接受 
            return 4;
		else if (SLR1Table[RowIndex][ColIndex][0]=='e')//错误
        {
        	strcpy(TableValue,SLR1Table[RowIndex][ColIndex]);
        	return 5;
		} 
        else 
        {
            strcpy(TableValue,SLR1Table[RowIndex][ColIndex]);
            return 3;
        }
    }
}
int Parser(Stack s,char str[],char SLR1Table[17][12][10],int ROW,int COLUMN)
{//语法分析总控程序 
    Push(s,'0');
    int i=0,j,isTrue=1,result;
    char e='0';
    //printf("\tSLR(1)分析法分析过程：\n\n");
    //printf("\t分析栈         输入串        动作\n");
    while(1)
    {
		//ReverseTraverse(s);
        //printf("\t%-14s",str+i);
        GetTop(s,e);
        result=TableSearch(SLR1Table,e,str[i],ROW,COLUMN);
        switch(result)
        {
        	case -1:
					//printf("找不到表头\n");
		        	return 0;					
        	case 0: 
					//printf("表内为空\n\n"); 
					return 0;				
			case 1: 
					Push(s,str[i]); //移进 
					Push(s,TableValue[1]);
					//printf("%s\n",TableValue);
					i++; 
					break;				
			case 2: 
					for(j=0;j<2*(strlen(Products[TableValue[1]-'0'])-3);j++) //归约 
						Pop(s,e);
					GetTop(s,e);
					Push(s,Products[TableValue[1]-'0'][0]);
					//printf("用%s归约\n",Products[TableValue[1]-'0']);
					TableSearch(SLR1Table,e,Products[TableValue[1]-'0'][0],ROW,COLUMN);
					Push(s,TableValue[0]);
					break;						
			case 4: 
					//printf("结束\n\n");
					return isTrue;				
			case 5:  
					if(TableValue[1] == '1')//e1：期望输入符号为运算对象，将分析中的产生式归约成 T 
					{
						printf("error：缺少运算对象\n");
						while(str[i]!='$'&&str[i]!='+'&&str[i]!='-'&&str[i]!='*'&&str[i]!='/'&&str[i]!=')')
							i++;
						GetTop(s,e);
						while(e!='0'&&e!='4'&&e!='7'&&e!='8')
						{
							Pop(s,e);
							Pop(s,e);
							GetTop(s,e);
						}
						if(e == '0' ||e == '4')
						{
							Push(s,'T');
							Push(s,'2');
						}
						else if(e == '7')
						{
							Push(s,'T');
							Push(s,'c');
						}
						else if(e == '8')
						{
							Push(s,'T');
							Push(s,'d');
						}
					}
					else if(TableValue[1] == '2')//e2：遇到不配对的右括号，将括号内容归约成 T 
					{
						printf("error：不配对的右括号\n");
						GetTop(s,e);
						while(e!='0'&&e!='4'&&e!='7'&&e!='8')
						{
							Pop(s,e);
							Pop(s,e);
							GetTop(s,e);
						}
						if(e == '0' ||e == '4')
						{
							Push(s,'T');
							Push(s,'2');
						}
						else if(e == '7')
						{
							Push(s,'T');
							Push(s,'c');
						}
						else if(e == '8')
						{
							Push(s,'T');
							Push(s,'d');
						}
						i++;
					}
					else if(TableValue[1] == '3')//e3：期望运算符，将分析中的产生式归约成 T
					{
						printf("error：缺少运算符\n");
						while(str[i]!='$'&&str[i]!='+'&&str[i]!='-'&&str[i]!='*'&&str[i]!='/'&&str[i]!=')')
							i++;
						GetTop(s,e);
						while(e!='0'&&e!='4'&&e!='7'&&e!='8')
						{
							Pop(s,e);
							Pop(s,e);
							GetTop(s,e);
						}
						if(e == '0' ||e == '4')
						{
							Push(s,'T');
							Push(s,'2');
						}
						else if(e == '7')
						{
							Push(s,'T');
							Push(s,'c');
						}
						else if(e == '8')
						{
							Push(s,'T');
							Push(s,'d');
						}
					}
					else if(TableValue[1] == '4')//e4：根据情况归约成T或F 
					{ 
						printf("error：缺少右括号\n");
						GetTop(s,e);
						while(e!='0'&&e!='4'&&e!='7'&&e!='8'&&e!='9'&&e!='a')
						{
							Pop(s,e);
							Pop(s,e);
							GetTop(s,e);
						}
						if(e == '0'||e == '7'||e == '8')
						{
							Push(s,'F');
							Push(s,'3');
						}
						else if(e == '4') 
						{
							Pop(s,e);
							Pop(s,e);
							GetTop(s,e);
							if(e == '9'||e == 'a')
							{
								Pop(s,e);
								Pop(s,e);
							}
							else 
							{
								Push(s,'F');
								Push(s,'3');
							} 
						}
						else if(e == '9')
						{
							Push(s,'F');
							Push(s,'e');
						}
						else if(e == 'a')
						{
							Push(s,'F');
							Push(s,'f');
						}
					}
					isTrue = 0;		
        }
    }
}

int main()
{
    int ROW=17,COLUMN=12; 
	int i=0,si=0,lex,result;
    char str[30],newstr[30];
	printf(">>");
    fgets(str,sizeof(str),stdin);
    printf("\n"); 
    while(str[si]!='\0')
    {
    	lex = LexicalAnalyzer(str, si);
    	if(lex == 33)
    		newstr[i]='i';
    	else if(lex == 34)
    		newstr[i]='n';
    	else if(lex == 64)
    		newstr[i]='$';
    	else
			newstr[i]=str[i]; 
    	i++;
    }
    newstr[i]='\0';
    Stack s;
    InitStack(s);
    result=Parser(s,newstr,SLR1Table,ROW,COLUMN);
    if(result==0)
		printf("错误！\n",str);
    else
		printf("正确！\n",str);
    return 0;
}


