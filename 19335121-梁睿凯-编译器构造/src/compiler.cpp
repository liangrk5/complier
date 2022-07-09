#include "global.h"


/****************************************************/
// 函数名   main
// 功  能   编译分析的入口
// 说  明   文件的处理，进入编译的过程
/****************************************************/
int main(int argv, char* argc[])
{
    char* filename;
    bool listIR, listObj, run, opt; 
    listIR = false;
    listObj = false;
    run = false;
    opt = false;
    std:: cout << "请输入文件名：" << std::endl;
    std::cin >> filename;
    fin = fopen(filename, "r");
    if(fin == NULL)
    {
        std::cout << "未成功打开文件，请检查文件名!" << std::endl;
        exit(0);
    }
    else
    {
        std::string tag;
        std::cout << "是否打印中间代码？(Y/N)" << std::endl;
        std::cin >> tag;
        if(tag == "Y" || tag == "y")
        {
            listIR = true;
        }
        std::cout << "是否打印目标代码？(Y/N)" << std::endl;
        std::cin >> tag;
        if(tag == "Y" || tag == "y")
        {
            listObj = true;
        }
        std::cout << "是否运行目标代码？(Y/N)" << std::endl;
        std::cin >> tag;
        if(tag == "Y" || tag == "y")
        {
            run = true;
        }
        init();
        gettoken();
        program();
        if(err != 0)
        {
            std::cout << "此代码中有" << err << "错误，请先修复错误!" << std::endl;
            exit(0);
        }
        if(listIR)
        {
            std::cout << "中间代码如下："<< std::endl;
            PrintIR();
        }
        if(opt)
        {
            std::cout << "优化后的中间代码如下：" << std::endl;
            IRoptimal(NextIR - 1);
            PrintIR();
        }
        objcode(NextIR - 1);
        if(listObj)
        {
            std::cout << "目标代码如下：" << std::endl;
            PrintObject();
        }
        if(run)
            vm();
        fclose(fin);
    }
    return 0;
}

/****************************************************/
// 函数名   init
// 功  能   初始化
// 说  明   
/****************************************************/
void init() {
    int i;

    //设置单字符符号
    for (i = 0; i <= 255; i++) {
        skind[i] = errtoken;                        //不正确的单字符符号为errtoken，先设置初值errtoken
    }
    skind['+'] = addtoken;
    skind['*'] = multoken;
    skind['-'] = subtoken;
    skind['/'] = divtoken;
    skind['>'] = gtctoken;
    skind['<'] = ltctoken;
    skind['('] = lparen;
    skind[')'] = rparen;
    skind['{'] = lbrace;
    skind['}'] = rbrace;
    //skind['\''] = quote;
    skind['='] = eqctoken;
    skind[','] = comma;
    skind[';'] = semicolon;


    //设置保留字名字
    strcpy(&(reservedword[0][0]), "bool");
    strcpy(&(reservedword[1][0]), "char");
    strcpy(&(reservedword[2][0]), "else");
    strcpy(&(reservedword[3][0]), "false");
    strcpy(&(reservedword[4][0]), "if");
    strcpy(&(reservedword[5][0]), "int");
    strcpy(&(reservedword[6][0]), "read");
    strcpy(&(reservedword[7][0]), "return");
    strcpy(&(reservedword[8][0]), "then");
    strcpy(&(reservedword[9][0]), "true");
    strcpy(&(reservedword[10][0]), "write");
    strcpy(&(reservedword[11][0]), "while");
    strcpy(&(reservedword[12][0]), "do");


    //设置保留字符号
    wkind[0] = booltoken;
    wkind[1] = chartoken;
    wkind[2] = elsetoken;
    wkind[3] = falsetoken;
    wkind[4] = iftoken;
    wkind[5] = inttoken;
    wkind[6] = readtoken;
    wkind[7] = rettoken;
    wkind[8] = thentoken;
    wkind[9] = truetoken;
    wkind[10] = writetoken;
    wkind[11] = whiletoken;
    wkind[12] = dotoken;


    tx = 0;                                     //表指针初值
    NextIR = 0;                                 //中间代码指令计数器
    //label = 0;                                //标号计数器
    err = 0;                                    //错误计数器
}


/****************************************************/
// 函数名   gettoken
// 功  能   词法分析，获取一个符号
// 说  明   读一个单词，并输出单词的类型和值
/****************************************************/
void gettoken() {
    int i, k;
    char c, ch;
    char a[al + 1];                             //临时符号，多出的一个字节用于存放0

    ch = fgetc(fin);

    if (ch == EOF) {
        token.kind = endfile;
        return;
    }           

    while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)        //忽略空格、换行、回车和TAB
        ch = fgetc(fin);

    if (isalpha(ch)) {                                          //名字或保留字以a...z开头
        k = 0;
        do {
            if (k < al)                                         //符号的最大长度为al，超长就截尾处理
            {
                a[k] = ch;
                k++;
            }
            ch = fgetc(fin);
        } while (isalpha(ch) || isdigit(ch));
        fseek(fin, -1, 1);

        a[k] = 0;
        strcpy(token.val.idname, a);

        i = lookup(a);
        if (i == -1)                                            //是识别标识符或保留关键字
        {
            token.kind = id;                                    //是标识符
        }
        else token.kind = wkind[i];                             //是关键字
    }
    else if (isdigit(ch))                                       //检测是否为数字：以0...9开头
    {
        k = 0;
        numval = 0;                                             //第一遍扫描，词法分条
        token.kind = num;
        do {
            numval = 10 * numval + ch - '0';
            k++;
            ch = fgetc(fin);
        } while (isdigit(ch));                                  // 获取数字的值
        token.val.numval = numval;
        //	k--;
        if (k > nmax) {
            char s[] = "lex:整数超过最大位数";
            error(s);
        }
        fseek(fin, -1, 1);
    }
    else if (ch == '=')                                         //检测赋值符号
    {
        ch = fgetc(fin);
        if (ch == '=') {
            token.kind = eqctoken;
        }
        else {
            token.kind = becomes;
            fseek(fin, -1, 1);
        }
    }
    else if (ch == '\'')                                        //检测字符常量
    {
        ch = fgetc(fin);
        c = ch;
        ch = fgetc(fin);
        if (ch == '\'') {
            if (c >= 32 && c <= 126) {
                token.kind = letter;
                token.val.ch = c;
            }
            else {
                char s[] = "lex:非法字符常量";
                error(s);    //fseek(fin,-1,1);
                token.kind = errtoken;
            }
        }
        else
        {
            char s[] = "lex:单引号没有匹配";
            error(s);
            token.kind = errtoken;
            fseek(fin, -1, 1);
            fseek(fin, -1, 1);
        }
    }
    else
        token.kind = skind[ch];     
}


/****************************************************/
// 函数名   lookup
// 功  能   判断字符串是否是关键字
// 说  明   
/****************************************************/
int lookup(char* idname) {
    int i = norw;
    for (i = norw; i >= 0; i--)
        if (strcmp(idname, reservedword[i]) == 0) return i;         //以idname字符串查保留符字表，若查到返回保留字类别码
    return -1;                                                      //idname不是保留字，返回-1
}


/****************************************************/
// 函数名   isalpha
// 功  能   判断是否是字母
// 说  明   
/****************************************************/
bool isalpha(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))return 1;
    else return 0;
}


/****************************************************/
// 函数名   isdigit
// 功  能   判断是否是数字
// 说  明   
/****************************************************/
bool isdigit(char c) {
    if (c >= '0' && c <= '9')return 1;
    else return 0;
}


/****************************************************/
// 函数名   printIR
// 功  能   打印中间代码序列
// 说  明   
/****************************************************/
void PrintIR() {
    int i = 0;
    while (i != NextIR) {
        printf("[%d]  ", i);
        PrintOneIR(i);
        printf("\n");
        i++;
    }
}


/****************************************************/
// 函数名   printOneIR
// 功  能   打印一条中间代码
// 说  明   
/****************************************************/
void PrintOneIR(int i) {
    printf("(");
    PrintIRName(IRcode[i].IRcodename);
    printf(",");
    if (IRcode[i].addr1 != NULL) {
        Printaddr(IRcode[i].addr1);
        printf(",");
    }
    else printf("-,");
    if (IRcode[i].addr2 != NULL) {
        Printaddr(IRcode[i].addr2);
        printf(",");
    }
    else printf("-,");// 

    if (IRcode[i].addr3 != NULL) {
        Printaddr(IRcode[i].addr3);
    }
    else printf("-");
    printf(")");
}


/****************************************************/
// 函数名   printIRName
// 功  能   打印代码的类别
// 说  明   由函数PrintOneIR调用
/****************************************************/
void PrintIRName(enum IRcode_name cname) {
    switch (cname) {
    case ADD:
        printf("ADD");
        break;
    case SUB:
        printf("SUB");
        break;
    case MUL:
        printf("MUL");
        break;
    case DIV:
        printf("DIV");
        break;
    case EQC:
        printf("EQC");
        break;
    case LTC:
        printf("LTC");
        break;
    case GTC:
        printf("GTC");
        break;
    case ASS:
        printf("ASS");
        break;
    case LAB:
        printf("LAB");
        break;
    case JPC:
        printf("JPC");
        break;
    case JUMP:
        printf("JUMP");
        break;
    case RET:
        printf("RET");
        break;
    case CALL:
        printf("CALL");
        break;
    case ENTRY:
        printf("ENTRY");
        break;
    case MENTRY:
        printf("MENTRY");
        break;
    case READ:
        printf("READ");
        break;
    case WRITE:
        printf("WRITE");
        break;
    default:
        break;
    }
}


/****************************************************/
// 函数名   Printaddr
// 功  能   打印ARG结构的内容
// 说  明   
/****************************************************/
void Printaddr(struct addrRecord* addr) {
    switch (addr->addrkind) {
    case labelkind:
        printf("L%d", addr->constant.value);
        break;
    case constkind: {
        if (addr->type == inttype) { printf("%d", addr->constant.value); }
        else if (addr->type == chartype) printf("\'%c\'", addr->constant.c);
        else if (addr->type == booltype) printf("%d", addr->constant.b);
        break;
    }
    case tempkind:
        printf("T%d", addr->constant.value);
        break;
    case varkind:
        printf("%s", addr->name);
        break;

    default:
        break;
    }
}


/****************************************************/
// 函数名   program
// 功  能   开始处理程序
// 说  明   
/****************************************************/
void program() {
    int i, t;
    int lev = 0;                                    //最外层为0
    int tx = 0;                                     //符号表开始
    int tx0;                                        //当前符号表中已经保存了的符号指针
    int off;
    int NIR1;
    struct addrRecord* mainlab, * temp1, * plab, * temp2;


    t = NewLabel();
    mainlab = addrLabel(t);                         //主程序入口标号
    genIR(JUMP, NULL, NULL, mainlab);               //跳转到主程序入口

    while (token.kind == id) {

        temp1 = addrconst(inttype);
        NIR1 = NextIR;

        if (strcmp(token.val.idname, "main") == 0)  //当前的单词是否是main
        {
            char msg[] = "main";
            i = found(msg, tx);                     //main是否已经登记过符号表
            if (i != 0) {
                char s[] = "sem:main函数重复!";
                error(s);
            }
            else {
                enter(proc, nul, lev, &tx, &off);   //将函数名填写到名字表中，地址之后需要修改
                table[tx].address = mainlab->constant.value;        //修改函数的入口标号
                genIR(MENTRY, NULL, temp1, mainlab);                //生成主程序的入口指令
            }
        }
        else   
        {
            char msg[] = "main";
            i = found(msg, tx);                     //main是否已经登记过符号表
            if (i != 0) {
                char s[] = "sem:main函数应该是最后一个!";
                error(s);
            }

            i = found(token.val.idname, tx);
            if (i != 0 && table[i].level == lev) {
                char s[] = "sem:ͬ同名标识符重复声明";
                error(s);
            }
            else {

                t = NewLabel();
                plab = addrLabel(t);
                enter(proc, nul, lev, &tx, &off);   //将函数名填写到名字表中，地址之后需要修改
                table[tx].address = t;
                genIR(ENTRY, NULL, temp1, plab);    //生成开辟空间的指令
            }

        }

        printtable(lev, tx, tx);                    //打印符号表
        gettoken();

        if (token.kind == lparen) {
            gettoken();
        }
        else {
            char s[] = "syn:缺左括号";
            error(s);
        }
        if (token.kind == rparen) {
            gettoken();
        }
        else {
            char s[] = "syn:右括号";
            error(s);
        }

        off = 0;                                    //函数内部的变量分配的起始地址
        tx0 = tx;                                   //记录即将进入的作用域在符号表的开始位置，为退出作用域做好准备
        tempnum = 1;                                //每个过程重新分配临时变量，可以优化为每个语句重新分配
        tempsize = 0;
        block(lev + 1, &tx, &off);                  //进入函数内部声明的处理，层加1
        tx = tx0;                                   //释放嵌套内存块符号在符号表中的空间

        IRcode[NIR1].addr2->constant.value = off;   //对ENTRY指令进行回填，将要开辟的空间大小填入指令

        //记录临时变量的空间
        temp2 = addrconst(inttype);
        IRcode[NIR1].addr1 = temp2;
        IRcode[NIR1].addr1->constant.value = tempsize;

    }
    char msg[] = "main";
    i = found(msg, tx);
    if (i == 0) {
        char s[] = "sem:没有main函数!";
        error(s);
    }
}


/****************************************************/
// 函数名   block
// 功  能   分析一个block
// 说  明   lev：当前块所在层
//          tx：名字表当前尾指针
/****************************************************/
int block(int lev, int* tx, int* off) {

    if (lev > levmax) {
        char s[] = "sem:嵌套层数太多";
        error(s);
    }

    if (token.kind == lbrace) {
        gettoken();
    }
    else {
        char s[] = "syn:缺少左大括号";
        error(s);
    }

    decls(lev, tx, off);                //处理声明
    stmts(lev, tx, off);                //处理语句

    if (lev == 1)
        RETstmt(lev, tx, off);          //return语句每个过程有且仅有一个

    if (token.kind != rbrace) {
        char s[] = "syn:缺右大括号";
        error(s);
    }
    else gettoken();
    return 0;
}


/****************************************************/
// 函数名   printtable
// 功  能   输出table中从stx到endtx的符号
// 说  明   lev：当前块所在层
/****************************************************/
void printtable(int lev, int stx, int endtx) {
    int i;
    printf("the identifiers in block %d:\n", lev);
    for (i = stx; i <= (endtx); i++)       //输出符号表
    {
        printf("%s ", table[i].name);

        if (table[i].type == inttype)
            printf("type=int  ");
        else if (table[i].type == chartype)
            printf("type=char ");
        else if (table[i].type == booltype)
            printf("type=bool ");
        else if (table[i].type == nul)
            printf("type=    ");

        if (table[i].form == fun)
            printf("form=funtion ");
        else if (table[i].form == proc)
            printf("form=procedure ");
        else if (table[i].form == var)
            printf("form=variable ");

        printf(" lev=%d ", table[i].level);
        printf("address=%d \n", table[i].address);
    }
    printf("-------------- \n");
}


/****************************************************/
// 函数名   decls
// 功  能   声明的语法语义分析
// 说  明   lev：当前块所在层
//          tx：名字表当前尾指针
//          off：现在已经分配了的偏移量
/****************************************************/
void decls(int lev, int* tx, int* off) {

    enum datatype tk;                   //当前声明的数据类型
    int tx0;
    tx0 = (*tx) + 1;                    //这一层声明的符号在table中的起始点

    while (token.kind == inttoken || token.kind == booltoken || token.kind == chartoken) {

        if (token.kind == inttoken) tk = inttype;
        else if (token.kind == booltoken) tk = booltype;
        else if (token.kind == chartoken) tk = chartype;

        gettoken();
        vardef(tk, lev, tx, off);       //tx和off会被vardecls改变

        while (token.kind == comma) {
            gettoken();
            vardef(tk, lev, tx, off);
        }

        if (token.kind == semicolon) {
            gettoken();
        }
        else {
            char s[] = "syn:声明缺少分号!";
            error(s);
        }

    }
    printtable(lev, tx0, (*tx));
}


/****************************************************/
// 函数名   vardef
// 功  能   vardecl 变量声明的语法语义分析
// 说  明   lev：当前块所在层
//          tx：名字表当前尾指针
//          off：现在已经分配了的偏移量
/****************************************************/
void vardef(enum datatype tk, int lev, int* tx, int* off) {
    if (token.kind == id) {
        enter(var, tk, lev, tx, off);       //填写名字表
        gettoken();
    }
    else {
        char s[] = "syn:变量声明后面应该是标识符!";
        error(s);
    }
}


/****************************************************/
// 函数名   enter
// 功  能   登记符号表
// 说  明   查询当前符号是否存在符号表中，若没有则登记，
//          有则报错
/****************************************************/
void enter(enum idform f, enum datatype tk, int lev, int* tx, int* off) {
    int i;
    i = found(token.val.idname, *tx);
    if (i != 0 && table[i].level == lev) {
        char s[] = "sem:ͬ同名标识符重复声明";
        error(s);
    }
    (*tx)++;
    if ((*tx) > tmax) {
        char s[] = "sem:符号表越界";
        error(s);
        return;
    }
    strcpy(table[(*tx)].name, token.val.idname);        //全局变量id中已存有当前名字的名字
    table[(*tx)].form = f;                              //符号类型，如果划分变量和过程名
    table[(*tx)].type = tk;                             //符号类型
    table[(*tx)].level = lev;                           //所在块
    table[(*tx)].address = (*off);                      //变量的地址，整数2个空间
    if (tk == inttype)
        (*off) = (*off) + 2;
    else if (tk == booltype || tk == chartype)
        (*off) = (*off) + 1;
}



/****************************************************/
// 函数名   found
// 功  能   在符号表中查找的指定名字
// 说  明   tx是目前符号已经登记的位置
/****************************************************/
int found(char* idt, int tx) {
    int i;
    strcpy(table[0].name, idt);
    i = tx;
    while (strcmp(table[i].name, idt) != 0) {
        i--;
    }
    return i;
}


/****************************************************/
// 函数名   addrvar
// 功  能   对于给定的变量产生相应的ARG结构
// 说  明   变量的名字通过全局变量id传递
/****************************************************/
struct addrRecord* addrvar(int lev, int off, enum datatype tk) {
    struct addrRecord* addr = (addrRecord*)malloc(sizeof(addrRecord));
    /* 填写变量ARG结构的内容 */
    if (addr) {
        addr->level = lev;
        addr->addrkind = varkind;
        strcpy(addr->name, token.val.idname);
        addr->type = tk;
        addr->constant.value = off;
        return (addr);
    }
    else {
        printf("程序运行空间不足!\n");
        exit(0);
    }
}


/****************************************************/
// 函数名   NewLabel
// 功  能   产生一个新的标号值
// 说  明   通过全局变量Label加1，产生新的标号值
/****************************************************/
int NewLabel() {
    label++;
    return (label);
}


/****************************************************/
// 函数名   NewTemp
// 功  能   产生一个新的临时变量的ARG结构
// 说  明   临时变量的类型通过参数指定
/****************************************************/
struct addrRecord* NewTemp(enum datatype tk) {
    struct addrRecord* new_temp = (addrRecord*)malloc(sizeof(addrRecord));
    /* 填写临时变量的ARG内容 */
    if (new_temp) {
        new_temp->addrkind = tempkind;
        new_temp->constant.value = tempnum;      // ��ʱ�������
        new_temp->type = tk;
        tempnum++;

        //临时变量空间计算
        if (tk == inttype)
            tempsize = tempsize + 2;
        else if (tk == booltype || tk == chartype)
            tempsize = tempsize + 1;

        return (new_temp);
    }
    else {
        printf("程序运行空间不足!\n");
        exit(0);
    }
}


/****************************************************/
// 函数名   addrLabel
// 功  能   对于给定的标号产生相应的ARG结构
// 说  明   标号没有类型
/****************************************************/
struct addrRecord* addrLabel(int i) {
    struct addrRecord* addr = (addrRecord*)malloc(sizeof(addrRecord));
    if (addr) {
        addr->addrkind = labelkind;
        addr->constant.value = i;
        return (addr);
    }
    else {
        printf("程序运行空间不足!\n");
        exit(0);
    }
}


/****************************************************/
// 函数名   addrconst
// 功  能   对于给定的常量产生相应的ARG结构
// 说  明   常量没有偏移地址，但是有类型
/****************************************************/
struct addrRecord* addrconst(enum datatype tk) {
    addrRecord* addr = (addrRecord*)malloc(sizeof(addrRecord));
    if (addr) {
        addr->addrkind = constkind;
        addr->type = tk;

        return (addr);
    }
    else {
        printf("程序运行空间不足!\n");
        exit(0);
    }
}


/****************************************************/
// 函数名   genIR
// 功  能   生成一条中间代码
// 说  明   指定四元式的四个单元
/****************************************************/
void genIR(enum IRcode_name cname, struct addrRecord* ad1, struct addrRecord* ad2, struct addrRecord* ad3) {
    if (NextIR >= IRmax) {
        char s[] = "sem:程序太长";
        error(s); 
    }
    else {
        IRcode[NextIR].IRcodename = cname;
        IRcode[NextIR].addr1 = ad1;
        IRcode[NextIR].addr2 = ad2;
        IRcode[NextIR].addr3 = ad3;
        NextIR++;
    };
}


/****************************************************/
// 函数名   stmts
// 功  能   stmts 语法语义分析
// 说  明   stmts --> {stmt}
/****************************************************/
int stmts(int lev, int* tx, int* off) {
    int tx0;
    int off0;
    int psize;

    tx0 = (*tx);                        //记录本层在符号表中开始的位置
    off0 = (*off);                      //记录本层在活动记录中开始的位置
    psize = off0;                       //平行block的空间大小

    while (token.kind == id || token.kind == whiletoken || token.kind == iftoken || token.kind == lbrace ||
        token.kind == readtoken || token.kind == writetoken) {
        if (token.kind == lbrace)       //block的特殊性
        {
            (*tx) = tx0;                //平行block在分配空间时，以及在存储符号表时共享空间，所以从相同的位置开始分配
            (*off) = off0;              //平行block在分配空间时，在活动记录中从相同的位置开始分配
            stmt(lev, tx, off);
            (*tx) = tx0;                //释放嵌套内层快符号在符号表中的空间

            if ((*off) > psize)         //记录平行的block中最大的空间
            {
                psize = (*off);
            }
        }
        else
            stmt(lev, tx, off);
    }

    (*off) = psize;                     //平行block的空间，利用最大的block计算

    return 0;
}



/****************************************************/
// 函数名   stmt
// 功  能   stmts 语法语义分析
// 说  明   stmt --> whilestm | ifstm | assstm
//         | callstm | block
/****************************************************/
int stmt(int lev, int* tx, int* off) {
    int i;

    if (token.kind == id) {
        i = found(token.val.idname, *tx);
        if (i == 0) {
            char s[] = "sem:标识符没有定义";
            error(s);                       //标识符没有定义的时候，会死循环
            gettoken();
        }
        else if (table[i].form == proc)
            CALLstmt(i);                    //准备按照过程调用处理

        else if (table[i].form == var) {

            ASSstmt(lev, tx, i);            //准备按照赋值语句处理
        }
    }
    else if (token.kind == iftoken)         //准备按照if语句处理
    {
        IFstmt(lev, tx, off);
    }
    else if (token.kind == whiletoken) {
        WHILEstmt(lev, tx, off);
    }
    else if (token.kind == lbrace)          //准备按照block处理
    {
        block(lev + 1, tx, off);

    }
    else if (token.kind == readtoken)       //准备按照read语句处理
    {
        READstmt(lev, tx, off);
    }
    else if (token.kind == writetoken)      //准备按照write语句处理
    {
        WRITEstmt(lev, tx, off);
    }
    else {
        char s[] = "sny:不是合法的语句格式";
        error(s);
    }
    return 0;
}



/****************************************************/
// 函数名   CALLstm
// 功  能   调用语句的语法语义分析
// 说  明   
/****************************************************/
int CALLstmt(int i) {
    struct addrRecord* lab;

    gettoken();
    lab = addrLabel(table[i].address);
    genIR(CALL, NULL, NULL, lab);

    if (token.kind == lparen) {
        gettoken();
        if (token.kind == rparen) {
            gettoken();
        }
        else {
            char s[] = "syn:缺少右括号";
            error(s);
        }
    }
    else {
        char s[] = "syn:缺少左括号";
        error(s);
    }

    if (token.kind == semicolon)
        gettoken();
    else ("sny:return后面缺少分号");
    return 0;
}


/****************************************************/
// 函数名   ASSstm
// 功  能   赋值语句的语法语义分析
// 说  明   
/****************************************************/
int ASSstmt(int lev, int* tx, int i) {
    struct addrRecord* bexprtemp, * temp;

    temp = addrvar(lev, table[i].address, table[i].type);   //创建赋值号左部的addr结构
    gettoken();

    if (token.kind == becomes) {
        gettoken();
    }
    else {
        char s[] = "sny:没有检测到赋值符号";
        error(s);
    }

    bexprtemp = bexpr(tx, lev);                             //处理赋值符号右侧表达式
    if (typematch(temp, bexprtemp)) {
        genIR(ASS, bexprtemp, NULL, temp);
    }
    else {
        char s[] = "sem:类型不匹配";
        error(s);
    };
    if (token.kind == semicolon) {
        gettoken();
    }
    else {
        char s[] = "sny:赋值语句后面缺分号";
        error(s);
    }

    return 0;
}


/****************************************************/
// 函数名   Ifstm
// 功  能   If语句语法语义分析
// 说  明   
/****************************************************/
int IFstmt(int lev, int* tx, int* off) {
    int i;
    struct addrRecord* bexprtemp, * temp, * elselab, * nextlab;

    gettoken();
    bexprtemp = bexpr(tx, lev);                     //调用布尔表达式函数
    if (bexprtemp->type != booltype) {
        char s[] = "sem:类型不匹配";
        error(s);
    }
    if (token.kind == thentoken) {
        gettoken();
    }
    else {
        char s[] = "sny:if语句缺少then";
        error(s);
    }

    elselab = addrLabel(NewLabel());                //准备好要使用的标号数据结构
    genIR(JPC, bexprtemp, NULL, elselab);           
    stmt(lev, tx, off);
    if (token.kind == elsetoken)                   
    {
        gettoken();
        nextlab = addrLabel(NewLabel());
        genIR(JUMP, NULL, NULL, nextlab); 
        genIR(LAB, NULL, NULL, elselab); 
        stmt(lev, tx, off); 
        genIR(LAB, NULL, NULL, nextlab); 
    }
    else {
        genIR(LAB, NULL, NULL, elselab);     
    }
    return 0;
}


/****************************************************/
// 函数名   WHILEstmt
// 功  能   while语句语法语义分析
// 说  明   
/****************************************************/
int WHILEstmt(int lev, int* tx, int* off) {
    int i;
    struct addrRecord* bexprtemp, * temp, * elselab, * nextlab;

    gettoken();

    elselab = addrLabel(NewLabel()); 
    nextlab = addrLabel(NewLabel());   
    genIR(LAB, NULL, NULL, elselab);
    bexprtemp = bexpr(tx, lev);     
    if (bexprtemp->type != booltype) {
        char s[] = "sem:类型不匹配";
        error(s);
    }
    if (token.kind == dotoken) {
        gettoken();
    }
    else {
        char s[] = "sny:while语句缺少do";
        error(s);
    }
    genIR(JPC, bexprtemp, NULL, nextlab);
    stmt(lev, tx, off);
    genIR(JUMP, NULL, NULL, elselab);
    genIR(LAB, NULL, NULL, nextlab);
    return 0;
}


/****************************************************/
// 函数名   RETstm
// 功  能   返回语句的语法语义分析
// 说  明   
/****************************************************/
int RETstmt(int lev, int* tx, int* off) {

    if (token.kind == rettoken)   
    {
        gettoken();
        genIR(RET, NULL, NULL, NULL);
        if (token.kind == semicolon)
            gettoken();
        else ("sny:return后面缺少;");
    }
    else {
        char s[] = "sem:过程缺少return";
        error(s);
    }
    return 0;
}



/****************************************************/
// 函数名   READstmt
// 功  能   read语句的语法语义分析
// 说  明   
/****************************************************/
int READstmt(int lev, int* tx, int* off) {
    struct addrRecord* temp;
    int i;

    gettoken();
    if (token.kind != lparen) {
        char s[] = "sny:缺少左括号";
        error(s);
    } 
    else {
        do {
            gettoken();
            if (token.kind == id) {
                i = found(token.val.idname, *tx); 
            }
            else {
                char s[] = "sny:缺少标识符";
                error(s);
            }

            gettoken();
            if (i == 0) {
                char s[] = "sem:标识符未声明";
                error(s);
            } 
            else if (table[i].type == inttype || table[i].type == chartype) {
                temp = addrvar(lev, table[i].address, table[i].type);
                genIR(READ, NULL, NULL, temp);
            }
            else {
                char s[] = "sny:read后面标识符的类型不对";
                error(s);
            }

        } while (token.kind == comma); 
    }
    if (token.kind != rparen) {
        char s[] = "sny:缺少右括号";
        error(s);  
    }
    else {
        gettoken();
    }
    if (token.kind == semicolon)
        gettoken();
    else ("sny:read后缺少分号");
    return 0;
}



/****************************************************/
// 函数名   WHILEstmt
// 功  能   循环语句的语法语义分析
// 说  明   
/****************************************************/
int WRITEstmt(int lev, int* tx, int* off) {
    struct addrRecord* temp;
    int i;


    gettoken();
    if (token.kind == lparen) {
        do {
            gettoken();
            temp = expr(tx, lev);
            genIR(WRITE, NULL, NULL, temp);
        } while (token.kind == comma);
        if (token.kind != rparen) {
            ("sny:write后面缺少右括号");
        }
        else {
            gettoken();
        }
    }
    else {
        char s[] = "sny:缺少左括号";
        error(s);
    }
    if (token.kind == semicolon)
        gettoken();
    else ("sny:write后缺少分号");
    return 0;
}


/****************************************************/
// 函数名   typematch
// 功  能   类型检查
// 说  明   进行二元运算的类型检查
/****************************************************/
int typematch(struct addrRecord* addr1, struct addrRecord* addr2) {
    if (addr1->type == addr2->type)
        return 1;
    return 0;
}


/****************************************************/
// 函数名   bexpr
// 功  能   bexpr语法语义分析
// 说  明   返回代码结果的addr
/****************************************************/
struct addrRecord* bexpr(int* tx, int lev) {
    struct addrRecord* expr1, * expr2, * temp = NULL;

    expr1 = expr(tx, lev);
    if (token.kind == ltctoken) {
        gettoken();
        expr2 = expr(tx, lev);
        if (typematch(expr1, expr2)) {
            temp = NewTemp(booltype);
            genIR(LTC, expr1, expr2, temp);
        }
        else {
            char s[] = "sem:类型不匹配";
            error(s);
        }
    }
    else if (token.kind == gtctoken) {
        gettoken();
        expr2 = expr(tx, lev);
        if (typematch(expr1, expr2)) {
            temp = NewTemp(booltype);
            genIR(GTC, expr1, expr2, temp);
        }
        else {
            char s[] = "sem:类型不匹配";
            error(s);
        }
    }
    else if (token.kind == eqctoken) {
        gettoken();
        expr2 = expr(tx, lev);
        if (typematch(expr1, expr2)) {
            temp = NewTemp(booltype);
            genIR(EQC, expr1, expr2, temp);
        }
        else {
            char s[] = "sem:类型不匹配";
            error(s);
        }
    }
    else temp = expr1;
    return temp;
}


/****************************************************/
// 函数名   expr
// 功  能   expr语法语义分析
// 说  明   
/****************************************************/
struct addrRecord* expr(int* ptx, int lev) {
    struct addrRecord* term1, * term2, * temp = NULL;
    struct tokenstruct tmp_token;

    term1 = term(ptx, lev);
    if (token.kind == addtoken || token.kind == subtoken) {
        do {
            tmp_token = token;
            gettoken();
            term2 = term(ptx, lev);
            if (typematch(term1, term2)) {
                temp = NewTemp(term1->type);
                if (tmp_token.kind == addtoken) {
                    genIR(ADD, term1, term2, temp);
                }
                else if (tmp_token.kind == subtoken) {
                    genIR(SUB, term1, term2, temp);
                }
                term1 = temp;
            }
            else {
                char s[] = "sem:类型不匹配";
                error(s);
            }
        } while (token.kind == addtoken || token.kind == subtoken);
    }
    else temp = term1;
    return temp;
}



/****************************************************/
// 函数名   term
// 功  能   term语法语义分析
// 说  明   
/****************************************************/
struct addrRecord* term(int* tx, int lev) {
    struct addrRecord* factor1, * factor2, * temp = NULL;
    struct tokenstruct tmp_token;
    factor1 = factor(tx, lev);

    if (token.kind == multoken || token.kind == divtoken) {
        do {
            tmp_token = token;
            gettoken();
            factor2 = factor(tx, lev);
            if (typematch(factor1, factor2)) {
                temp = NewTemp(factor1->type);
                if (tmp_token.kind == multoken) {
                    genIR(MUL, factor1, factor2, temp);
                }
                else if (tmp_token.kind == divtoken) {
                    genIR(DIV, factor1, factor2, temp);
                }
                factor1 = temp;
            }
            else {
                char s[] = "sem:类型不匹配";
                error(s);
            }
        } while (token.kind == multoken || token.kind == divtoken);
    }
    else temp = factor1;
    return temp;
}


/****************************************************/
// 函数名   factor
// 功  能   factor语法语义分析
// 说  明   
/****************************************************/
struct addrRecord* factor(int* tx, int lev) {
    int i = 0, off = 2;                     
    struct addrRecord* temp = {};

    if (token.kind == id)    
    {
        i = found(token.val.idname, *tx); 
        if (i == 0) {
            char s[] = "sem:标识符未声明";
            error(s);
        }
        else {
            temp = addrvar(lev, table[i].address, table[i].type);
            gettoken();
        }
    }
    else if (token.kind == num)   
    {
        temp = addrconst(inttype);
        temp->constant.value = token.val.numval;
        gettoken();
    }
    else if (token.kind == lparen) 
    {
        gettoken();
        temp = expr(tx, lev);
        if (token.kind == rparen) {
            gettoken();
        }
        else {
            char s[] = "sny:缺少右括号";
            error(s);
        }
    }
    else if (token.kind == letter) {
        temp = addrconst(chartype);
        temp->constant.c = token.val.ch;
        gettoken();
    }
    else if (token.kind == truetoken || token.kind == falsetoken) 
    {
        temp = addrconst(booltype);
        if (token.kind == truetoken) {
            temp->constant.b = 1;
            tempoff = IRcode[i].addr3->constant.value; 
        }
        else if (token.kind == falsetoken) {
            temp->constant.b = 0;
        }
        gettoken();
    }
    return temp;
}



/****************************************************/
// 函数名   error
// 功  能   错误信息输出
// 说  明   
/****************************************************/
void error(char* message) {
    printf("error : %s\n", message);
    err++;
}


/****************************************************/
// 函数名   IRoptimal
// 功  能   中间代码的优化
// 说  明   
/****************************************************/
void IRoptimal(int n) {
    int i = 0;
    while (i < n) {
        switch (IRcode[i].IRcodename) {
        case ADD:
            constMerge(i);
            break;
        case SUB:
            constMerge(i);
            break;
        case DIV:
            constMerge(i);
            break;
        case MUL:
            constMerge(i);
            break;
        default:
            break;
        }
        i++;
    }
}



/****************************************************/
// 函数名   ConstMerge
// 功  能   常量合并
// 说  明   
/****************************************************/
void constMerge(int i) {
    if (IRcode[i].addr1->addrkind == constkind && IRcode[i].addr2->addrkind == constkind)
        if (IRcode[i].addr1->type == inttype && IRcode[i].addr2->type == inttype) {

            switch (IRcode[i].IRcodename) {
            case ADD:
                IRcode[i].addr1->constant.value = IRcode[i].addr1->constant.value + IRcode[i].addr2->constant.value;
                break;
            case SUB:
                IRcode[i].addr1->constant.value = IRcode[i].addr1->constant.value - IRcode[i].addr2->constant.value;
                break;
            case MUL:
                IRcode[i].addr1->constant.value = IRcode[i].addr1->constant.value * IRcode[i].addr2->constant.value;
                break;
            case DIV:
                IRcode[i].addr1->constant.value = IRcode[i].addr1->constant.value / IRcode[i].addr2->constant.value;
                break;
            default:
                break;
            }
            IRcode[i].IRcodename = ASS;
            IRcode[i].addr2 = NULL;
        }
}



/****************************************************/
// 函数名   objcode
// 功  能   将中间代码转换成目标代码
// 说  明   参数n是中间代码数量
/****************************************************/
void objcode(int n) {
    int curIR = 0;

    curin = 0;          //目标指令写入代码段的地址初始化，第一条目标指令的地址为0
    jptx = 0;
    labtx = 0;
    temptx = 0;

    while (curIR <= n) {
        switch (IRcode[curIR].IRcodename) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case LTC:
        case GTC:
        case EQC:
            binaryopGen(curIR);
            break;
        case ASS:
            sourceOperandGen(IRcode[curIR].addr1, ax);          
            thirdOperandGen(curIR, ax);
            break;
        case JUMP:
            jumpGen(curIR);
            break;
        case JPC:
            jpcGen(curIR);
            break;
        case LAB:
            labGen(curIR);
            break;
        case CALL:
            callGen(curIR);
            break;    
        case ENTRY:
            entryGen(curIR);
            break; 
        case RET:
            retGen();
            break;   
        case WRITE:
            writeGen(curIR);
            break;
        case READ:
            readGen(curIR);
            break;
        case MENTRY:
            entryGen(curIR);
            break;
        default:
            char s[] = "中间代码名出错!\n";
            error(s);
            break;
        }
        curIR++;
    }
}


/****************************************************/
// 函数名   binaryopGen
// 功  能   生成二元运算的目标代码
// 说  明   参数curIR是正在分析的中间代码的编号
/****************************************************/
void binaryopGen(int curIR) {
    int t;
    enum datatype tk;
    enum IRcode_name irname;

    sourceOperandGen(IRcode[curIR].addr1, ax);                     //取左操作数值存在ax中
    sourceOperandGen(IRcode[curIR].addr2, bx);                     //取右操作数值存在bx中
    irname = IRcode[curIR].IRcodename;
    if (irname == ADD || irname == SUB || irname == MUL || irname == DIV) {
        switch (irname)        
        {
        case ADD:
            emit(opADD, ax, ax, bx);
            break;
        case SUB:
            emit(opSUB, ax, ax, bx);
            break;
        case MUL:
            emit(opMUL, ax, ax, bx);
            break;
        case DIV:
            emit(opDIV, ax, ax, bx);
            break;
        }
        thirdOperandGen(curIR, ax);
    }
    else if (irname == LTC || irname == GTC || irname == EQC) {
        switch (irname) {
        case LTC:
            emit(opSUB, flag, ax, bx);
            emit(opJNL, 0, 0, 0);                                          
            enterjp(curin - 1, IRcode[curIR].addr3->constant.value, tempk); 
            break;
        case GTC:
            emit(opSUB, flag, ax, bx);
            emit(opJNG, 0, 0, 0);
            enterjp(curin - 1, IRcode[curIR].addr3->constant.value, tempk); 
            break;
        case EQC:
            emit(opSUB, flag, ax, bx);
            emit(opJNE, 0, 0, 0);
            enterjp(curin - 1, IRcode[curIR].addr3->constant.value, tempk);
            break;
        default:
            break;
        }
    }
}


/****************************************************/
// 函数名   thirdOperandGen
// 功  能   生成算术运算的目标代码
// 说  明   参数curIR是正在分析的中间代码的编号
/****************************************************/
void thirdOperandGen(int curIR, int r) {
    int t, j;
    enum datatype tk;
    switch (IRcode[curIR].addr3->addrkind)                          //处理第三个操作数，根据目标操作数临时变量或者变量进行不同处理
    {
    case constkind: {
        char s[] = " IR:constants cannot be stored!\n";
        error(s); 
        break;
    }
    case varkind:
        emit(opST, r, IRcode[curIR].addr3->constant.value, bp);     //计算结果放入内存
        break;
    case tempkind:
        t = IRcode[curIR].addr3->constant.value;

        j = foundTemp(t);
        if (j != 0)                                                 //找到同名的临时变量
        {
            char s[] = " IR:Temp variable is duplicated!\n";
            error(s);   
        }
        else          
        {
            entertempV(IRcode[curIR].addr3);
            emit(opST, r, temptable[temptx].address, bp); 
        }
        break;
    default:
        break;
    }
}



/****************************************************/
// 函数名   entertempV
// 功  能   登记临时变量符号表
// 说  明  
/****************************************************/
void entertempV(struct addrRecord* addr) {
    enum datatype tk;
    if (temptx > tempmax) {
        char s[] = " obj:临时变量表越界 ";
        error(s);
    }
    else {

        temptx++;                      
        tk = addr->type;
        temptable[temptx].tnum = addr->constant.value;
        temptable[temptx].type = addr->type;
        temptable[temptx].address = tempoff;
        if (tk == inttype) {
            tempoff = tempoff + 2;
        }
        else if (tk == booltype || tk == chartype) {
            tempoff = tempoff + 1;
        }
    }
}



/****************************************************/
// 函数名   foundTemp
// 功  能   在临时变量符号表中查找的指定临时变量
// 说  明   参数tnum是待查询的临时变量编号
/****************************************************/
int foundTemp(int tnum) {
    int i;        
    i = temptx;
    temptable[0].tnum = tnum;
    while (temptable[i].tnum != tnum) {
        i--;
    }
    return i;
}



/****************************************************/
// 函数名   sourceOperandGen
// 功  能   取源操作数到指定寄存器
// 说  明   分操作数为常数、变量处理
/****************************************************/
void sourceOperandGen(struct addrRecord* addr, int reg) {
    int t = 0;
    int j;
    switch (addr->addrkind) {
    case constkind:                                        //常量处理
    {
        if (addr->type == inttype)
            emit(opLDC, reg, addr->constant.value, 0);     //整型常量放reg
        else if (addr->type = chartype)
            emit(opLDC, reg, addr->constant.c, 0);         //字符常量放reg
        else if (addr->type = booltype)
            emit(opLDC, reg, addr->constant.b, 0);         //bool常量放reg
    }
    break;
    case varkind:
        emit(opLD, reg, addr->constant.value, bp);        //变量放reg寄存器
        break;
    case tempkind:
        t = addr->constant.value;

        j = foundTemp(t);
        if (j == 0)                
        {
            char s[] = " IR:Temporary variables can not be referenced before they are generated !\n";
            error(s);   
        }
        else                      
        {
            emit(opLD, reg, temptable[j].address, bp);     
        }
        break;
    default:
        break;
    }
}


/****************************************************/
// 函数名   emit
// 功  能   生成目标指令函数
// 说  明   
/****************************************************/
void emit(enum objop iop, int r, int d, int s) {
    if (curin >= ISIZE) {
        char s[] = "obj:目标程序太长";
        error(s); 
    }
    else {
        iMem[curin].iop = iop;
        iMem[curin].iarg1 = r;
        iMem[curin].iarg2 = d;
        iMem[curin].iarg3 = s;
        curin++;                           
    };
}


/****************************************************/
// 函数名   jumpGen
// 功  能   处理跳转指令
// 说  明   curIR表示正在处理的中间代码编号
/****************************************************/
void jumpGen(int curIR) {
    int j, k = 1, f, lab;

    emit(opJUMP, 0, 0, 0);
    lab = IRcode[curIR].addr3->constant.value;  


    j = foundlab_labtable(lab);

    if (j != 0) {
        iMem[curin - 1].iarg1 = labtable[j].taddr;  
    }
    else 
    {

        enterjp(curin - 1, lab, labk);
    }
}



/****************************************************/
// 函数名   jpcGen
// 功  能   处理跳转指令
// 说  明   curIR表示正在处理的中间代码编号
/****************************************************/
void jpcGen(int curIR) {
    int j, k, i, t, lab;

    t = IRcode[curIR].addr1->constant.value;  
    lab = IRcode[curIR].addr3->constant.value;  

    k = foundlab_jptable(t, jptx, tempk);   

    if (k != 0)  
    {
        jptable[k].lab = lab;
        jptable[k].kind = labk;
        i = jptable[k].pending;
    }
    else {
        char s[] = "JPC refers the error temp!";
        error(s);
    }

    j = foundlab_labtable(lab);

    if (j != 0) {
        backpatchobj(lab, labtable[j].taddr);
    }
}



/****************************************************/
// 函数名   enterjp
// 功  能   登记跳转指令符号表
// 说  明  
/****************************************************/
void enterjp(int taddr, int lab, enum jpkind k) {
    int i;

    if (jptx > jpmax) {
        char s[] = " obj:跳转指令表越界 ";
        error(s);
    }
    else {
        jptx++;
        jptable[jptx].pending = taddr; 
        jptable[jptx].lab = lab;      
        jptable[jptx].kind = k;
    }
}



/****************************************************/
// 函数名   enterlab
// 功  能   登记标号符号表
// 说  明   查询当前标号lab是否登记过，若没有则登记
//          有则报错
/****************************************************/
void enterlab(int lab, int in) {
    int j;


    j = foundlab_labtable(lab);


    if (j != 0) {
        char s[] = "IR:ͬ同名标号重复声明";
        error(s);
    }
    else if (labtx > labmax) {
        char s[] = " obj:标号表越界";
        error(s);
    }
    else {
        labtx++;
        labtable[labtx].lab = lab;               
        labtable[labtx].taddr = in;             
    }
}


/****************************************************/
// 函数名   foundlab_labtable
// 功  能   在标号表中查找的指定标号lab
// 说  明   
/****************************************************/
int foundlab_labtable(int lab) {
    int i;       
    i = labtx;

    labtable[0].lab = lab;
    while (labtable[i].lab != lab) {
        i--;
    }
    return i;
}



/****************************************************/
// 函数名   foundlab_jptable
// 功  能   在跳转指令表中从jtx开始查找指定类型k的编号lab
// 说  明   
/****************************************************/
int foundlab_jptable(int lab, int jtx, enum jpkind k) {
    int i;     
    i = jtx;

    jptable[0].lab = lab;
    while (!(jptable[i].lab == lab && jptable[i].kind == k)) {
        i--;
    }
    return i;
}



/****************************************************/
// 函数名   labelGen
// 功  能   处理LAB中间代码
// 说  明   curIR是当前正在处理的中间代码编号
/****************************************************/
void labGen(int curIR) {
    int lab;
    lab = IRcode[curIR].addr3->constant.value; 

    enterlab(lab, curin); 
    backpatchobj(lab, curin); 
}


/****************************************************/
// 函数名   callGen
// 功  能   处理call的中间代码
// 说  明   curIR是当前正在处理的中间代码编号
/****************************************************/
void callGen(int curIR) {
    int lab;
    int j, k = 0, f;
    emit(opLDA, ax, 2, pc);
    emit(opPUSH, ax, 0, 0);                      
    lab = IRcode[curIR].addr3->constant.value;  

    j = foundlab_labtable(lab);

    if (j != 0)   
    {
        emit(opJUMP, labtable[j].taddr, 0, 0); 
    }
    else 
    {
        enterjp(curin - 1, lab, labk);      
    }
}



/****************************************************/
// 函数名   backpatchobj
// 功  能   当定义性标号lab和对应的目标in出现，回填目标
//          指令中跳转指令的目标
// 说  明   参数为跳转的目标和标号
/****************************************************/
void backpatchobj(int lab, int in) {
    int i, j, k, f, t;

    i = 1;
    while (i <= jptx)   
    {                   
        j = foundlab_jptable(lab, i, labk);

        if (j != 0)         
        {
            k = jptable[j].pending;
            iMem[k].iarg1 = in;      
            t = j;

            while (t < jptx)    
            {
                jptable[t].pending = jptable[t + 1].pending;
                jptable[t].lab = jptable[t + 1].lab;
                jptable[t].kind = jptable[t + 1].kind;
                t++;
            }
            jptx--;
        }
        i++;
    }
}



/****************************************************/
// 函数名   entryGen
// 功  能   开始一个新过程的中间代码处理
// 说  明 
/****************************************************/
void entryGen(int curIR) {
    int lab;
    int psize;

    enum IRcode_name i;

    lab = IRcode[curIR].addr3->constant.value;


    enterlab(lab, curin);  

    backpatchobj(lab, curin);              

    emit(opPUSH, bp, 0, 0);            
    emit(opMOV, bp, top, 0);           

    psize = IRcode[curIR].addr2->constant.value + IRcode[curIR].addr1->constant.value; 
    emit(opLDA, top, psize, bp);
    temptx = 0;
    tempoff = IRcode[curIR].addr2->constant.value; 
}



/****************************************************/
// 函数名   retGen
// 功  能   退出过程
// 说  明 
/****************************************************/
void retGen() {
    emit(opMOV, top, bp, 0);
    emit(opPOP, bp, 0, 0); 
    emit(opPOP, pc, 0, 0); 
}


/****************************************************/
// 函数名   readGen
// 功  能   输入语句
// 说  明 
/****************************************************/
void readGen(int curIR) {

    if (IRcode[curIR].addr3->type == inttype) {
        emit(opIN, ax, 0, 0);
    }
    else if (IRcode[curIR].addr3->type == chartype) {
        emit(opIN, ax, 1, 0);
    }
    else if (IRcode[curIR].addr3->type == booltype) {
        emit(opIN, ax, 2, 0);
    }
    thirdOperandGen(curIR, ax);
}


/****************************************************/
// 函数名   writeGen
// 功  能   输出语句
// 说  明 
/****************************************************/
void writeGen(int curIR) {
    sourceOperandGen(IRcode[curIR].addr3, ax);
    if (IRcode[curIR].addr3->type == inttype) {
        emit(opOUT, ax, 0, 0);
    }
    else if (IRcode[curIR].addr3->type == chartype) {
        emit(opOUT, ax, 1, 0);
    }
    else if (IRcode[curIR].addr3->type == booltype) {
        emit(opOUT, ax, 2, 0);
    }
}



/****************************************************/
// 函数名   pritobject
// 功  能   打印目标代码序列
// 说  明 
/****************************************************/
void PrintObject() {
    int i = 0;
    while (i < curin) {
        printf("[%d]  ", i);
        PrintOneObj(i);
        printf("\n");
        i++;
    }
}


/****************************************************/
// 函数名   PritOneobj
// 功  能   打印一条目标代码
// 说  明 
/****************************************************/
void PrintOneObj(int i) {
    enum objop objname;
    objname = iMem[i].iop;

    if (objname == opADD || objname == opSUB || objname == opMUL || objname == opDIV) {
        switch (objname) {
        case opADD:
            printf("ADD ");
            break;
        case opSUB:
            printf("SUB ");
            break;
        case opMUL:
            printf("MUL ");
            break;
        case opDIV:
            printf("DIV ");
            break;
        default:
            break;
        }
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        PrintREGname(iMem[i].iarg2);
        printf(" ");
        PrintREGname(iMem[i].iarg3);
    }
    else if (objname == opJNL) {
        printf("JNL ");
        printf("%d", iMem[i].iarg1);
    }
    else if (objname == opJNG) {
        printf("JNG ");
        printf("%d", iMem[i].iarg1);
    }
    else if (objname == opJNE) {
        printf("JNE");
        printf("%d", iMem[i].iarg1);
    }
    else if (objname == opJUMP) {
        printf("JUMP ");
        printf("%d", iMem[i].iarg1);
    }
    else if (objname == opPUSH) {
        printf("PUSH ");
        PrintREGname(iMem[i].iarg1);
    }
    else if (objname == opPOP) {
        printf("POP ");
        PrintREGname(iMem[i].iarg1);
    }
    else if (objname == opLD) {
        printf("LD ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);
        printf(" ");
        PrintREGname(iMem[i].iarg3);
    }
    else if (objname == opST) {
        printf("ST ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);    
        printf(" ");
        PrintREGname(iMem[i].iarg3);
    }
    else if (objname == opLDA) {
        printf("LDA ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);     
        printf(" ");
        PrintREGname(iMem[i].iarg3);
    }
    else if (objname == opLIT) {
        printf("LIT ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%c", iMem[i].iarg2);     
    }
    else if (objname == opLDC) {
        printf("LDC ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);     
    }
    else if (objname == opMOV) {
        printf("MOV ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        PrintREGname(iMem[i].iarg2);
    }
    else if (objname == opIN) {
        printf("IN ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);
    }
    else if (objname == opOUT) {
        printf("OUT ");
        PrintREGname(iMem[i].iarg1);
        printf(" ");
        printf("%d", iMem[i].iarg2);
    }
}


/****************************************************/
// 函数名   PrintREGname
// 功  能   打印寄存器名字
// 说  明 
/****************************************************/
void PrintREGname(int r) {
    switch (r) {
    case ax:
        printf("ax");
        break;
    case bx:
        printf("bx");
        break;
    case cx:
        printf("cx");
        break;
    case dx:
        printf("dx");
        break;
    case flag:
        printf("flag");
        break;
    case top:
        printf("top");
        break;
    case bp:
        printf("bp");
        break;
    case pc:
        printf("pc");
        break;
    default:
        break;
    }
}



/****************************************************/
// 函数名   vm
// 功  能   解释执行目标代码
// 说  明 
/****************************************************/
void vm() 
{
    int r, d, s;
    int m;

    INSTRUCTION ip;  

    printf("start run!\n");
    reg[top] = 0;    
    reg[bp] = 0;
    reg[pc] = 0;
    do {

        ip = iMem[reg[pc]];  
        r = ip.iarg1;
        d = ip.iarg2;
        s = ip.iarg3;
        reg[pc] = reg[pc] + 1;
        switch (ip.iop) {
        case opIN:
            switch (d) {
            case 0:
                printf("Input an int value: ");
                scanf("%d", &(reg[r]));
                break;
            case 1:
                printf("Input a char :");
                scanf(" %c", &reg[r]);    
                break;
            case 2:
                printf("Input an bool value:");
                scanf("%d", &(reg[r]));
                break;
            }
            break;
        case opOUT:
            switch (d) {
            case 0:
                printf("output an int value:  %d\n", reg[r]);
                break;
            case 1:
                printf("output a char:  %c\n", reg[r]);
                break;
            case 2:
                printf("output:  %d\n", reg[r]);
                break;
            }
            break;
        case opADD:
            reg[r] = reg[d] + reg[s];
            break;
        case opSUB:
            reg[r] = reg[d] - reg[s];
            break;
        case opDIV:
            if (reg[s] != 0)
                reg[r] = reg[d] / reg[s];
            else
                printf("Zero divisor!!!");
            break;
        case opMUL:
            reg[r] = reg[d] * reg[s];
            break;
        case opPUSH:
            dMem[reg[top]] = reg[r];
            reg[top] = reg[top] + 1;
            break;
        case opPOP:
            reg[top] = reg[top] - 1;
            reg[r] = dMem[reg[top]];
            break;
        case opLD:
            m = d + reg[s];
            reg[r] = dMem[m];
            break;
        case opST:
            m = d + reg[s];
            dMem[m] = reg[r];
            break;
        case opLDA:
            reg[r] = d + reg[s];
            break;
        case opLDC:
            reg[r] = d;
            break;
        case opMOV:
            reg[r] = reg[d];
            break;
        case opJNL:
            if (reg[flag] >= 0)
                reg[pc] = r;
            break;
        case opJNG:
            if (reg[flag] <= 0)
                reg[pc] = r;
            break;
        case opJNE:
            if (reg[flag] != 0)
                reg[pc] = r;
            break;
        case opJUMP:
            reg[pc] = r;
            break;
        }
    } while (reg[pc] != 0);
}