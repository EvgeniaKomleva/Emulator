#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cstring>
#include <sstream>
#include <cmath>

using namespace std;
map<int, string > Mapa;// номер строки, название метки
vector<int> memory(1048581);
vector<int> registers(16);
int flags;
int mem_pos=2;

void PrintVector ( vector<int>& v) {
    for (auto s : v) {
        cout << s << " ";
    }
}

void PrintMap(const map<int, string>& m) {
    cout << "Size = " << m.size() << endl;
    for (auto item: m) {
        cout << item.first << ": " << item.second << endl;
    }

}

void clean_text_new (){ //удаляет метки и сохраняет указатели на следующую строку
    vector<string> v;
    ifstream input("input.fasm");
    ofstream output("outpu.txt");
    string line;
    int count_metka = 0;
    int count_strok = 0;
    while (getline(input, line)) {
        char str1[20], str2[20];
        int  k_m =  line.find(":");
        int k_r = sscanf(line.c_str(), "%s %s", str2, str1);
        if (k_m == -1  && k_r >1) {
            output << line << endl;
            count_strok = count_strok +1;
        } else {
            if (k_m != -1) {
                count_metka = count_metka + 1;
                line = line.substr(0,k_m);
                //cout << line << count_metka<< " " << count_strok<< endl;
                Mapa[count_strok ] = line;
            }
        }
    }
}


void clean_text () { //удаляет метки и сохраняет указатели на следующую строку
    vector<string> v;
    ifstream input("input.fasm");
    ofstream output("outpu.txt");
    string line;
    int count_metka = 0;
    int count_strok = 0;
    while (getline(input, line)) {
        char str1[20], str2[20];
        string l = line;
        int k_m = sscanf(line.c_str(), "%s %s", str2, str1);
        int k_r = line.find(":");
        if (k_m ==1 && k_r ==-1) {cout << line;}
        else {}
        //cout << k_m <<" "<< line<<endl;
        int r = line.find(";");
        line = line.substr(0,r);
        if (count_metka == 0) {
            int k = line.find(":");
            //line = line.substr(0,k);
            count_strok += 1;
            if (k == -1) {
                line = line.substr(0,k);
                if (strlen(line.c_str()) != 0) {
                    output << line << endl;
                }
            } else {
                count_metka += 1;
                string line1 = line.substr(0,k);
                line = line.substr(k+1,k+10);
                if (strlen(line.c_str()) != 0) {
                    output << line << endl;
                    count_metka = 0;
                }
                Mapa[count_strok - count_metka] = line1;
            }
        }
        else {
            v.push_back(line);
            char str[20], str1[20];
            string l = line;
            int k = sscanf(line.c_str(), "%s %s", str, str1);
            if (strlen(line.c_str()) != 0 || k>0) {
                output << line << endl;
            }
            count_metka = 0;
        }
    }
    //ОТЛАААААААААААААААААААААААДКА_______PrintMap(Mapa);
}


enum code { HALT = 0, SYSCALL = 1, ADD = 2, ADDI = 3, SUB = 4, SUBI = 5, MUL = 6, MULI = 7, DIV = 8, DIVI = 9, LC = 12, SHL = 13, SHLI = 14, SHR = 15,
    SHRI = 16, AND = 17, ANDI = 18, OR = 19,ORI = 20, XOR = 21, XORI = 22, NOT = 23,
    MOV = 24, ADDD = 32, SUBD = 33, MULD = 34, DIVD = 35, ITOD = 36,DTOI = 37, PUSH = 38,
    POP = 39, CALL = 40, CALLI = 41, RET = 42, CMP = 43, CMPI = 44, CMPD = 45, JMP = 46, JNE = 47,
    JEQ = 48, JLE = 49,JL = 50, JGE = 51, JG = 52, LOAD = 64, STORE = 65, LOAD2 = 66,
    STORE2 = 67, LOADR = 68, LOADR2 = 69, STORER = 70, STORER2 = 71 };

int RR (string line, int c){
    /*8бит код команды,4бит код регистра-приёмника,4бит код регистра-источника,16бит модификатор источника, число со знаком*/
    int n, m, l;
    char str[20];
    sscanf(line.c_str(), "%s r%d r%d %d", str, &n, &m, &l);
    c = c << 24;
    n = n << 20;
    m = m << 16;
    int res = c + n + l +m;
    //cout << res<<endl;
    return res;
}

int RM (string line, int c ) {
    /*8старших бит код команды,4следующих бита  код регистра(приёмника или источника),20младших бит адрес в памяти в виде
беззнакового числа*/
    int n, m;
    char str[20];
    sscanf(line.c_str(), "%s r%d %d", str, &n, &m);
    int res =( c << 24) + (n << 20) + m;
    //cout << res<<endl;
    return res;
}

int RI (string line, int c) {
    /*8бит код команды,4бит код регистра-приёмника,20бит непо-средственный операнд, число со знаком */
    int n, m;
    char str[20];
    sscanf(line.c_str(), "%s r%d %d", str, &n, &m);
    n = n << 20;
    c = c << 24;
    int res = n + c + m;
    //cout << res << endl;
    return res;
}
int ret_cod (string line, int c) {
    int n;
    char str[20];
    sscanf(line.c_str(), "%s %d", str, &n);
    c = c << 24;
    int res = n + c;
    return res;
}

int calli_cod(string line, int c) {//хотелось бы чтобы возращала номер строки на которую прыгать
    /*8бит код команды,*/
    int n;
    stringstream x;//Создание потоковой переменной
    x << line;//Перенос строки в поток
    char word[20] = {};
    while (x >> word)
        for (auto item: Mapa) {
            if (item.second == word)
            {
                n = item.first;
                //c = c << 24;
                int res = (c << 24)+n;
                //cout << res << "EEE"<< endl;

                return res;
            }
        }
    return 0;
}
int jmp_metki (string line) {
    int n;
    char word[10];
    char str[10];
    sscanf(line.c_str(), "%s %s", str, word);
    string word_str = word;
    for (auto item: Mapa) {
        if (item.second == word_str)
        {
            n = item.first;
            return n-1;
        }
    }
    return 0;
}

int lc_metki (string line){ //подается на вход строка с меткой, ищет ее в маре и выдает
    int n,m;
    /*stringstream x;//Создание потоковой переменной
    x << line;//Перенос строки в поток
    char word[20] = {};
    while (x >> word)*/
    char word[10];
    char str[10];
    sscanf(line.c_str(), "%s r%d %s", str, &m, word);
    string word_str = word;
    for (auto item: Mapa) {
        if (item.second == word_str)
        {
            n = item.first;
            return n+(m << 20);
        }
    }
    return 0;
    //PrintMap(Mapa);
}
int jg_cod(string line, int c) {
    int n;
    char word[10];
    char str[10];
    sscanf(line.c_str(), "%s %s", str, word);
    string word_str = word;
    for (auto item: Mapa) {
        if (item.second == word_str)
        {
            n = item.first;
            return n+(c << 24);
        }
    }
    return 0;
}

int nachalo () {
    ifstream input("outpu.txt");
    string line;
    while (getline(input, line)) {
        int nac;
        char word[10];
        char str[10];
        sscanf(line.c_str(), "%s %s", str, word);
        string str_s = str;
        string word_str = word;
        if (str_s == "end") {
            for (auto item: Mapa) {
                if (item.second == word_str) {
                    nac = item.first;
                    return nac;
                    //registers[15] = nac;
                } //else { cout << line << endl; }
            }
        }
    }
    return 0;
}
void comanda() {
    ifstream input("outpu.txt");
    ofstream output("out.txt");
    string line;
    while (getline(input, line)) {
        int n, m, l;
        char str[20];
        sscanf(line.c_str(), "%s r%d r%d %d", str, &n, &m, &l);
        string line1;
        int c;
        line1 = str;
        if (line1 == "halt") {
            c = HALT;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "syscall") {
            c = SYSCALL;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "storer2") {
            c = STORER2;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "add") {
            c = ADD;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "addi") {
            c = ADDI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "sub") {
            c = SUB;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "subi") {
            c = SUBI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "mul") {
            c = MUL;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "muli") {
            c = MULI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "div") {
            c = DIV;
            int chislo = RR(line, c);
            memory.push_back(chislo);
            //PrintVector(registers);
            // cout <<endl;
        }
        else if (line1 == "divi") {
            c = DIVI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "lc") {
            c = LC;int chislo;
            if (sscanf(line.c_str(), "%s r%d %d", str, &n, &m)==3) {
                chislo = RI(line, c);
            } else {chislo = lc_metki(line)+ (c << 24);}
            memory.push_back(chislo);
        }
        else if (line1 == "shl") {
            c = SHL;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "shli") {
            c = SHLI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "shr") {
            c = SHR;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "shri") {
            c = SHRI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "and") {
            c = AND;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "andi") {
            c = ANDI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "or") {
            c = OR;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "ori") {
            c = ORI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "xor") {
            c = XOR;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "xori") {
            c = XORI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "not") {
            c = NOT;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "mov") {
            c = MOV;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "addd") {
            c = ADDD;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "subd") {
            c = SUBD;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "muld") {
            c = MULD;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "divd") {
            c = DIVD;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "itod") {
            c = ITOD;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "dtoi") {
            c = DTOI;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "push") {
            c = PUSH;int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "pop") {
            c = POP;int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "call") {
            c = CALL;int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "calli") {
            c = CALLI;
            int chislo = calli_cod(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "ret") {
            c = RET;
            int chislo = ret_cod(line, c);
            memory.push_back(chislo);
        }
        if (line1 == "cmp") {
            c = CMP;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        if (line1 == "cmpi") {
            c = CMPI;
            int chislo = RI(line, c);
            memory.push_back(chislo);
        }
        if (line1 == "cmpd") {
            c = CMPD;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        if (line1 == "jmp") {
            string l = line;
            int n;
            c = JMP;
            int chislo = jg_cod(line, c)-1;
            memory.push_back(chislo);
        }/*
            if (line1 == "jeq") {
                c = JEQ;
                RI(line, c);
            }
            if (line1 == "jle") {
                c = JLE;
                RI(line, c);
            }
            if (line1 == "jl") {
                c = JL;
                RI(line, c);
            }*/

        else if (line1 == "jne") {
            c = JNE;
            int chislo = jg_cod(line, c);
            memory.push_back(chislo);
        }
        if (line1 == "jge") {
            c = JGE;
            int chislo = jg_cod(line, c);
            memory.push_back(chislo);
            //cout << chislo<<" JJJJJJJJJJJJJJJJGEEE";
        }
        else if (line1 == "jg") {
            c = JG;
            int chislo = jg_cod(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "load") {
            c = LOAD;
            int chislo = RM(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "store") {
            c = STORE;
            int chislo = RM(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "load2") {
            c = LOAD2;
            int chislo = RM(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "store2") {
            c = STORE2;
            int chislo = RM(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "loadr") {
            c = LOADR;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "loadr2") {
            c = LOADR2;
            int chislo = RM(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "storer") {
            c = STORER;
            int chislo = RR(line, c);
            memory.push_back(chislo);
        }
        else if (line1 == "end") {output<< 100000<<" ";}
        //else { memory.push_back(44444444);}
    }
}

void sub(int regist1, int regist2 , int chislo){
    registers[regist1] = registers[regist1] -(registers[regist2] +chislo);
}

void  subi(int registr , int chislo) {
    registers[registr] = registers[registr] - chislo;
}

void  addi (int registr , int chislo ) {
    registers[registr] = registers[registr] + chislo;
}

void add (int regist1, int regist2, int chislo) {
    registers[regist1] = registers[regist1] +registers[regist2] +chislo;
}

void mov (int registr1, int registr2 , int chislo) {
    registers[registr1] = registers[registr2] + chislo;
}

void mul(int registr1, int registr2, int chislo) {
    long long int vtoroi_mnog =chislo+registers[registr2];
    long long int res_64= vtoroi_mnog*registers[registr1];
    long long  reg1 = res_64 & 4611686014132420608;
    int reg2 = res_64 &4294967295;
    reg1 = reg1 >> 32;
    int reg_n =(int) reg1;
    registers[registr1+1] = reg_n;
    registers[registr1] = reg2;
}

void lc (int registr, int chislo) {
    registers[registr] = chislo;
}

void muli(int regist, int chislo) { // ДОПИСААААААААААААААААААААТЬ______
    long long int vtoroi_mnog =chislo;
    long long int res_64= vtoroi_mnog*registers[regist];
    long long  reg1 = res_64 & 4611686014132420608;
    //long long  reg1 = res_64 & 18446744069414584320;
    int reg2 = res_64 &4294967295;
    reg1 = reg1 >> 32;
    int reg_n =(int) reg1;
    registers[regist+1] = reg_n;
    registers[regist] = reg2;
}

void div(int registr1, int registr2, int chislo) {
    long long reg_1 = registers[registr1+1] << 30;
    long long first_operand = reg_1+registers[registr1];
    registers[registr1] = (int)first_operand /registers[registr2];
    registers[registr1+1] =(int) first_operand % registers[registr2];
}

void divi(int registr , int chislo ) {

}
void shli(int registr , int chislo ) {
    registers[registr] = registers[registr] << chislo;
}
void shri(int registr , int chislo ) {
    registers[registr] = registers[registr] >> chislo;
}
void andi(int registr , int chislo ) {
    registers[registr] = registers[registr] & chislo;
}
void ori(int registr , int chislo ) {
    registers[registr] = registers[registr] | chislo;
}
void xori(int registr , int chislo ) {
    registers[registr] = registers[registr] ^ chislo;
}
void push(int registr , int chislo ) {
    memory[mem_pos] = registers[registr] + chislo;
    mem_pos +=1;
}
void pop(int registr , int chislo ) {
    //cout << mem_pos << "POOOOOOOOOOOOP";
    //PrintVector(memory);

    if (mem_pos > 1){
        registers[registr] = memory[mem_pos-1]+chislo;
        mem_pos=mem_pos-2;
    }
    else { mem_pos = mem_pos - 1;return;}
}
int ret(int cod , int i ) {
    int chislo = cod & 16777215;
    return chislo;
}
void cmpi(int registr , int chislo ) {
    /*если > dthytn +1, = - 0, < -1*/
    if (registers[registr] == chislo) { flags =  0;}
    else if (registers[registr] < chislo) { flags =  -1;}
    else { flags = 1;}
}
void not_f(int registr , int chislo ) {
    registers[registr] = ~(registers[registr]);
}

void shl(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr1] << registers[registr2];
}
void shr(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr1] >> registers[registr2];
}
void and_f(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr1] & registers[registr2];
}
void or_f(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr1] | registers[registr2] ;
}
void xor_f(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr1] ^ registers[registr2];
}
void addd(int registr1, int registr2, int chislo) {
    registers[registr1] =registers[registr1] + registers[registr2];
    registers[registr1+1] =registers[registr1+1] + registers[registr2+1];
}
void subd(int registr1, int registr2, int chislo) {
    registers[registr1] =registers[registr1] - registers[registr2];
    registers[registr1+1] =registers[registr1+1] - registers[registr2+1];
}
void muld(int registr1, int registr2, int chislo) {
    //double mul = registers[registr1] *registers[registr2];
    registers[registr1] =registers[registr1] *registers[registr2];
    registers[registr1+1] =registers[registr1+1] * registers[registr2+1];
}
void divd(int registr1, int registr2, int chislo) {
    registers[registr1] =registers[registr1] / registers[registr2];
    //registers[registr1+1] =registers[registr1+1] / registers[registr2+1];
}
void itod(int registr1, int registr2, int chislo) {
    registers[registr1] = registers[registr2];

}
void dtoi(int registr1, int registr2, int chislo) {

}
void call(int registr1, int registr2, int chislo ) {
    memory[1]= registers[15];
    registers[15] = registers[registr1];
    registers[registr2] = registers[15]+1;


}
void cmp(int registr1, int registr2, int chislo) {
    if (registers[registr1] > registers[registr2] +chislo) {
        flags =1;
    } else if (registers[registr1] > registers[registr2] +chislo) {flags = 0;}
    else{flags = -1;}
}
void cmpd(int registr1, int registr2, int chislo) {

}
void loadr(int registr1, int registr2, int chislo) {
    //int h = registers[registr2];
    //int m = memory[h + chislo];
    registers[registr1] = memory[mem_pos-1];
}
void storer(int registr1, int registr2, int chislo) {

}
void storer2(int registr1, int registr2, int chislo) {

}

void syscall (int regist, int chislo) {
    /*if (chislo == 0) {
        return 0;
    }*/
    if (chislo == 1) {//open

    }
    else if (chislo == 2) {//read

    }
    else if (chislo == 3) {//write

    }
    else if (chislo == 4) {//close

    }
    else if (chislo == 5) {//alloc

    }
    else if (chislo == 6) {//free

    }
    else if (chislo == 100) {//scan int!!
        cin >> registers[regist];
    }
    else if (chislo == 101) {//scan double
        // cin >> registers[regist];
    }
    else if (chislo == 102) {//print int!!
        int a = registers[regist];
        cout << a;
        //output1 << a;
        //ОТЛАДКАААА__PrintVector(registers);
    }
    else if (chislo == 103) {//print double
        double d =  registers[regist];
        cout <<d ;
    }
    else if (chislo == 104) {//getchar
        //getchar();
    }
    else if (chislo == 105) {//putchar
        //output1 << "HHHHHHHHHH";
        char a = registers[regist];
        //output1 <<  a;
        putchar(a);
    }// else {output1 << "GFFFFFFFFFFTY"<< endl; output1 << chislo<< endl;}
    //output1.close();
    //return 10000;
}
void emul_RI (int cod, int i) {
    int cod_comand = cod >> 24;
    int chislo = cod & 1048575;
    int regist = cod & 15728640;
    regist = regist >> 20;
    //ОТЛАДКАААА__cout << cod << " " << cod_comand<< " "<<regist << " "<< chislo << " " << i << endl;
    if (cod_comand == 1) {syscall(regist, chislo);}
    else if (cod_comand == 12) {lc(regist, chislo);}
    else if (cod_comand == 3) {addi(regist, chislo);}
    else if (cod_comand == 5) {subi(regist, chislo);}
    else if (cod_comand == 7) {muli(regist, chislo);}
    else if (cod_comand == 9) {divi(regist, chislo);}
    else if (cod_comand == 14) {shli(regist, chislo);}
    else if (cod_comand == 16) {shri(regist, chislo);}
    else if (cod_comand == 18) {andi(regist, chislo);}
    else if (cod_comand == 20) {ori(regist, chislo);}
    else if (cod_comand == 22) {xori(regist, chislo);}
    else if (cod_comand == 23) {not_f(regist, chislo);}
    else if (cod_comand == 38) {push(regist, chislo);}
    else if (cod_comand == 39) {pop(regist, chislo);}
        //else if (cod_comand == 42) {ret(regist, chislo);}
    else if (cod_comand == 44) {cmpi(regist, chislo);}
}


int calli( int registr, int chislo , int cod ) {
//    sscanf(line.c_str(), "%s %s", str, str1);
    return chislo;
}
void jmp(int cod , int i ) {
    registers[15] = cod & 255;
}
void jg(int cod , int i ) {
    if (flags == 1) {registers[15] = cod & 255;}
    else {registers[15]++;}
}
void jne(int cod , int i ) {
    if (flags != 0) {
        int r = cod & 255;
        registers[15] = r-1;}
    else {registers[15]++;}
}
void jl(int registr , int chislo ) {

}
void jle(int registr , int chislo ) {

}
void load(int registr , int chislo ) {

}
void store(int registr , int chislo ) {

}
void jge(int cod , int i) {
    if (flags == 1 || flags == 0) {registers[15] = (cod & 255)-1;}
    else {registers[15]++;}
}
void jeq(int registr , int chislo ) {

}
void loadr2(int registr , int chislo ) {

}
void store2(int registr , int chislo ) {

}
void load2(int registr , int chislo ) {

}

void emul_RM (int cod, int i) {
    int cod_comand = cod >> 24;
    int chislo = cod & 1048575;
    int regist = cod & 15728640;
    regist = regist >> 20;
    //ОТЛАДКАААА__cout << cod << " " << cod_comand<< " "<<regist << " "<< chislo << " " << i << endl;
    if (cod_comand == 41) {calli(regist, chislo, cod);}
    else if (cod_comand == 46) {jmp(regist, chislo);}
    else if (cod_comand == 47) {jne(regist, chislo);}
    else if (cod_comand == 48) {jeq(regist, chislo);}
    else if (cod_comand == 49) {jle(regist, chislo);}
    else if (cod_comand == 50) {jl(regist, chislo);}
    else if (cod_comand == 51) {jge(regist, chislo);}
        //else if (cod_comand == 52) {jg(regist, chislo);}

    else if (cod_comand == 64) {load(regist, chislo);}
    else if (cod_comand == 65) {store(regist, chislo);}
    else if (cod_comand == 66) {load2(regist, chislo);}
    else if (cod_comand == 67) {store2(regist, chislo);}
    else if (cod_comand == 70) {loadr2(regist, chislo);}
}


void emul_RR (int cod, int i) {
    int cod_comand = cod >> 24;
    int chislo = cod & 65535;
    int regist_1 = cod & 15728640;
    regist_1 = regist_1 >> 20;
    int regist_2 = cod & 983040;
    regist_2 = regist_2 >> 16;
    //ОТЛАДКАААА__cout << cod << " " << cod_comand<< " "<< regist_1 <<" "<< regist_2 << " " <<chislo << " " << i << endl;
    if (cod_comand == 6) { mul(regist_1, regist_2,chislo);}
    if (cod_comand == 24) { mov(regist_1, regist_2,chislo);}
    if (cod_comand == 2) {add(regist_1, regist_2,chislo);}
    if (cod_comand == 4) {sub(regist_1, regist_2,chislo);}
    if (cod_comand == 8) {div(regist_1, regist_2,chislo);}
    if (cod_comand == 13) {shl(regist_1, regist_2,chislo);}
    if (cod_comand == 15) {shr(regist_1, regist_2,chislo);}
    if (cod_comand == 17) {and_f(regist_1, regist_2,chislo);}
    if (cod_comand == 19) {or_f(regist_1, regist_2,chislo);}
    if (cod_comand == 21) {xor_f(regist_1, regist_2,chislo);}
    if (cod_comand == 32) {addd(regist_1, regist_2,chislo);}
    if (cod_comand == 33) {subd(regist_1, regist_2,chislo);}
    if (cod_comand == 34) {muld(regist_1, regist_2,chislo);}
    if (cod_comand == 35) {divd(regist_1, regist_2,chislo);}
    if (cod_comand == 36) {itod(regist_1, regist_2,chislo);}
    if (cod_comand == 37) {dtoi(regist_1, regist_2,chislo);}
    if (cod_comand == 40) {call(regist_1, regist_2,chislo);}
    if (cod_comand == 43) {cmp(regist_1, regist_2,chislo);}
    if (cod_comand == 45) {cmpd(regist_1, regist_2,chislo);}
    if (cod_comand == 68) {loadr(regist_1, regist_2,chislo);}
    if (cod_comand == 69) {storer(regist_1, regist_2,chislo);}
    if (cod_comand == 71) {storer2(regist_1, regist_2,chislo);}

}

unsigned int calli_RM (int cod) {//почему 41?!
    unsigned int cod1 = cod &16777215;
    return cod1;
}
void opredelenie_tipa_coda(int memory_size) {
    int cod;
    int i = 1;
    //PrintVector(memory);
    //unsigned int pos  = (unsigned int)registers[15];
    while(registers[15] <= memory_size) {
        cod = memory[1048581+registers[15]];
        //cout << " "<<cod<<"CCCCCCCCCCCOD"<< endl;
        int cod1 = cod >> 24;//получаем код команды
        //PrintVector(registers);
        /*cout <<"_________________________________" <<endl;*/
        //cout << " "<<cod1 << " ";
        //cout << endl;

        if ( cod1 == 0 ) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 1)
        {
            int chislo = cod & 1048575;
            if (chislo == 0) { return; }
            else {emul_RI(cod, i);registers[15]++;}
        }
        else if (cod1 == 6) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 24) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 12) {
            emul_RI(cod, i);
            registers[15]++;}
        else if (cod1 == 41)
        {
            memory[1] = registers[15];
            int k = calli_RM(cod);
            registers[15] = k;
        }//Проблема, дает только ссылку на прыжок
        else if (cod1 == 3) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 2) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 4) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 5) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 7) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 8) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 9) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 13) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 14) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 15) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 16) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 17) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 18) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 19) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 20) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 21) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 22) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 23) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 32) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 33) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 34) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 35) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 36) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 37) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 38) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 39) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 40) {emul_RR(cod, i);}
        else if (cod1 == 42) {
            int chislo = ret(cod, i)-1;
            if (chislo == 0) {chislo = chislo-1;}
            registers[15]= memory[1]+2+chislo;
            if (mem_pos == 1)
            {
                registers[15] = 15;}
        }
        else if (cod1 == 43) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 44) {emul_RI(cod, i);registers[15]++;}
        else if (cod1 == 45) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 46) {jmp(cod, i);}
        else if (cod1 == 47) {
            jne(cod, i);}
        else if (cod1 == 48) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 49) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 50) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 51) {jge(cod, i);}
        else if (cod1 == 52) {jg(cod, i);}//как определить в jg слово передается или чиселки?!;
            // от чего зависит аргумент ret
            // Почему какте то тесты выдают в тестирующей системе другой ствет чем в IDE
        else if (cod1 == 64) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 65) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 66) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 67) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 68) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 69) {emul_RR(cod, i);registers[15]++;}
        else if (cod1 == 70) {emul_RM(cod, i);registers[15]++;}
        else if (cod1 == 71) {emul_RR(cod, i);registers[15]++;}
        //else { return;}
        i = i +1;//р14 показывает на то что исполнять
        //сделать 4 функцию у которых вторым аргументом слова
        // массив структур сделать список обозначений
    }
    //ОТЛАААААААААААААААААААААААДКА_______PrintVector(registers);
}


int main() {
    registers[14] = 1048575;
    clean_text_new();
    registers[15] = nachalo();
    comanda();
    //cout << registers[15]<< " " <<memory.size()-1048581;
    ifstream input("outpu.txt");
    string line;
    int i = 0;
    while (getline(input, line)) {
        i++;
    }
    //cout << i;
    //PrintMap(Mapa);
    int memory_size = i;
    opredelenie_tipa_coda(memory_size);
    //ОТЛАААААААААААААААААААААААДКА_______cout << endl;
    //ОТЛАААААААААААААААААААААААДКА_______PrintVector(memory);
    //ОТЛАААААААААААААААААААААААДКА_______cout << endl;
    //PrintVector(registers);
    //ОТЛАААААААААААААААААААААААДКА_______PrintMap(Mapa);
    return 0;
}//перенести команды в массив памяти 37 0 999 0
