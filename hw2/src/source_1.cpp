#include <iostream>
#include <cstdio>
#include <string>


using namespace std;

enum SYM_KIND { // 符号类型
    SYM_NUM,    // 数字
    SYM_ADD,    // +
    SYM_SUB,    // -
    SYM_MUL,    // *
    SYM_DIV,    // /
    SYM_MOD,    // %
    SYM_POW,    // ^
    SYM_LBR,    // (
    SYM_RBR,    // )
    SYM_END,    // '\0'
    SYM_ERR     // 非法符号
};

enum ERR_KIND{  // 状态
    ERR_OK,
    ERR_INVALID_CHAR,
    ERR_NO_OPERATOR,
    ERR_BR_NOT_MATCH,
    ERR_NO_NUM,
    ERR_END
};

string expr;

class parser{
public:
    parser();
    SYM_KIND get_sym();
    void E();
    void E_();
    void T();
    void T_();
    void F();
    void F_();
    void N();
    void NUM();
    void remake();
    void output_error();

private:
    int pos;
    ERR_KIND err;
};

parser::parser()
{
    pos = 0;
    err = ERR_OK;
}


SYM_KIND parser::get_sym()
{
    char ch = expr[pos++];
    while(ch == ' ' || ch == '\t')
        ch = expr[pos++];
    if(ch <= '9' && ch >= '0')
    {
        pos--;
        NUM();
        return SYM_NUM;
    }
    else if(ch == '+') return SYM_ADD;
    else if(ch == '-') return SYM_SUB;
    else if(ch == '*') return SYM_MUL;
    else if(ch == '/') return SYM_DIV;
    else if(ch == '%') return SYM_MOD;
    else if(ch == '^') return SYM_POW;
    else if(ch == '(') return SYM_LBR;
    else if(ch == ')') return SYM_RBR;
    else if(ch == '\0')
    {
        err = ERR_END;
        return SYM_END;
    }
    err = ERR_INVALID_CHAR;
    return SYM_ERR;
}

void parser::E()
{
    if(err != ERR_OK)
    {
        return;
    }
    T();
    E_();
}

void parser::E_()
{
    if(err != ERR_OK)
    {
        return;
    }
    SYM_KIND kind = get_sym();
    if (kind == SYM_ADD)
    {
        T();
        E_();
    }
    else if(kind == SYM_SUB)
    {
        T();
        E_();
    }
    else if(kind == SYM_END || kind == SYM_RBR)
    {
        pos--;
    }
    else if(kind != SYM_ERR)
    {
        err = ERR_NO_OPERATOR;
    }
}

void parser::T()
{
    if (err != ERR_OK)
    {
        return;
    }
    F();
    T_();
}

void parser::T_()
{
    if (err != ERR_OK)
    {
        return;
    }
    SYM_KIND kind = get_sym();
    if (kind == SYM_MUL)
    {
        F();
        T_();
    }
    else if (kind == SYM_DIV)
    {
        F();
        T_();
    }
    else if (kind == SYM_MOD)
    {
        F();
        T_();
    }
    else if(kind == SYM_ADD || kind == SYM_SUB || kind == SYM_RBR || kind == SYM_END)
    {
        pos--;
    }
    else if (kind != SYM_ERR)
    {
        err = ERR_NO_OPERATOR;
    }   
}

void parser::F()
{
    if (err != ERR_OK)
    {
        return;
    }
    N();
    F_();  
}

void parser::F_()
{
    if (err != ERR_OK)
    {
        return;
    }
    SYM_KIND kind = get_sym();
    if (kind == SYM_POW)
    {
        F();
    }
    else if (kind == SYM_ADD || kind == SYM_SUB || kind == SYM_MUL || kind == SYM_DIV || kind == SYM_MOD || kind == SYM_END || kind == SYM_RBR)
    {
        pos--;
    }
    else if (kind != SYM_ERR)
    {
        err = ERR_NO_OPERATOR;
    } 
}

void parser::N()
{
    if (err != ERR_OK)
    {
        return;
    }
    SYM_KIND kind = get_sym();
    if (kind == SYM_LBR)
    {
        E();
        if (err == ERR_END)
        {
            err = ERR_BR_NOT_MATCH;
        }
        else if (err == ERR_OK)
        {
            kind = get_sym();
            if (kind != SYM_RBR)
            {
                err = ERR_BR_NOT_MATCH;
            }
        }
    }
    else if (kind == SYM_ADD)
    {
        kind = get_sym();
        if (kind != SYM_NUM)
        {
            err = ERR_NO_NUM;
        }
    }
    else if (kind == SYM_SUB)
    {
        kind = get_sym();
        if (kind != SYM_NUM)
        {
            err = ERR_NO_NUM;
        }
    }
    else if (kind != SYM_NUM)
    {
        err = ERR_NO_NUM;
    }   
}

void parser::NUM()
{
    while (expr[pos] <= '9' && expr[pos] >= '0')
    {
        pos++;
    }
    if (expr[pos] == '.')
    {
        pos++;
        while(expr[pos] <= '9' && expr[pos] >= '0')
        {
            pos++;
        }
    }  
}

void parser::remake()
{
    pos = 0;
    err = ERR_OK;
}

void parser::output_error()
{
    if (err == ERR_END)
    {
        cout << "算术表达式正确！" << endl;
    }
    else if (err == ERR_OK)
    {
        err = ERR_BR_NOT_MATCH;
    }
    if (err == ERR_BR_NOT_MATCH)
    {
        cout << "算术表达式错误，括号不匹配！" << endl;
    }
    else if (err == ERR_NO_NUM)
    {
        cout << "算术表达式错误，缺少数字！" << endl;
    }
    else if (err == ERR_NO_OPERATOR)
    {
        cout << "算术表达式错误，缺少运算符！" << endl;
    }
    else if (err == ERR_INVALID_CHAR)
    {
        cout << "算术表达式错误，非法字符输入！" << endl;
    }  
}

int main()
{
    parser myparser;
    cout << ">>";
    cin >> expr;
    while (expr.size() != 0)
    {
        if (expr == "quit")
        {
            break;
        }
        myparser.remake();
        myparser.E();
        myparser.output_error();
        cout << ">>";
        cin >> expr;
    }
    
    return 0;
}