#include <iostream>
#include <fstream>
#include <string>
#include<sstream>
#include<algorithm>
#include<vector>
#include<ctype.h>
#include"linked_list.h"
using namespace std;

bool upBottom = true;
string phase[5] = { "IF","ID","EX","MEM","WB" };
string instructionsSet[5] = { "lw","sw","add","sub","beq" };
int registers[32] = {};
int words[32] = {};


Chain<string*> running;
bool branchOK = NULL;



string replaceChar(string s,string c1)
{
    size_t found_c1 = s.find(c1);
    string s_noc1 = "";
    for (int x = 0; x < s.length(); x++)
    {
        if (found_c1 != string::npos)
        {
            if (x == found_c1) {
                found_c1 = s.find(c1, found_c1 + 1);
                continue;
            }
        }
        s_noc1 += s[x];
    }
    return s_noc1;
}
string getRegNum(string rawReg)
{
    size_t found_reg = rawReg.find("$");
    string regNum = "";
    //cout << valuelst[0] << valuelst[1] << valuelst[2];
    if (found_reg != string::npos) {
        for (int i = found_reg + 1; i < rawReg.length(); i++)
        {
            if (isdigit(rawReg[i]))
            {
                regNum += rawReg[i];
            }
        }
    }
    else
        regNum = rawReg;
    return regNum;
}

bool checkRegAccessable(int index, string reg) 
{
    for (int i = index - 1; i >= 0; i--) 
    {
        string* run_ins = running.getByIndex(i);
        if (run_ins[2].compare("beq")==0 || run_ins[2].compare("sw")==0) 
        {
            continue;
        }
        if (getRegNum(run_ins[4]).compare(getRegNum(reg)) == 0) 
        {
            if (run_ins[8] == "None")                             //reg is used as target in previous instruction and not ready to forward, inaccessible
            {
                return false;
            }
            else
                return true;
        }
    }
    return true;                    //not target of previous running instruction , accessible and read registers value
}
                

int getforwardingvalue(string reg) 
{
    for (int i = running.lencheck()-1; i >= 0; i--)
    {
        string* run_ins = running.getByIndex(i);
        if (getRegNum(run_ins[4]).compare(getRegNum(reg)) == 0) 
        {
            if (run_ins[8] == "None")
            {
                continue;           //最新結果還沒出來
            }
            else
            {
                return stoi(run_ins[8]);
            }
        }

    }
    return NULL;
}


int* getValue(string* ins_detail, int rmcount)
{
    string* lst = new string[rmcount];
    int* valuelst = new int[rmcount];
    for (int i = 0; i < rmcount; i++) 
    {
        string num = "";
        size_t found_neg = ins_detail[4 + i].find("-");
        size_t found_reg = ins_detail[4 + i].find("$");
        size_t found_p1 = ins_detail[4 + i].find("(");
        size_t found_p2 = ins_detail[4 + i].find(")");
        if (found_reg != string::npos) 
        {
            //memory
            if (found_p1 != string::npos && found_p2 != string::npos) 
            {
                //cout << "First occurrence is memory" << ins_detail[4 + i] << endl;
                if (found_neg != string::npos) 
                {
                    lst[i] += "-";
                }
                for (int j = 0; j < found_p1; j++) 
                {
                    if (isdigit(ins_detail[4 + i][j])) 
                    {
                        lst[i] += ins_detail[4 + i][j];
                    }
                }
                int offset = stoi(lst[i]);

                lst[i] =getRegNum(ins_detail[4+i]);
                for (int j = found_reg + 1; j < found_p2; j++) 
                {
                    if (isdigit(ins_detail[4 + i][j]))
                    {
                        lst[i] += ins_detail[4 + i][j];
                    }
                }

                int wordAim;
                if (getforwardingvalue(lst[i]) == NULL)
                {
                    wordAim = (offset + registers[stoi(lst[i])]) / 4;                     //no forwarding
                }
                else
                    wordAim = (offset + getforwardingvalue(lst[i]))/4;                     //forwarding
                valuelst[i] = words[wordAim];                 

            }

            //reg
            else {
                //cout << "First occurrence is register" <<ins_detail[4 + i] << endl;
                for (int j = found_reg + 1; j < ins_detail[4+i].length(); j++)
                {
                    if (isdigit(ins_detail[4 + i][j]))
                    {
                        lst[i] += ins_detail[4 + i][j];
                    }
                }
                
                if (getforwardingvalue(lst[i]) == NULL)
                {
                    valuelst[i] = registers[stoi(lst[i])];                      //no forwarding
                }                                                                           
                else
                    valuelst[i] = getforwardingvalue(lst[i]);                     //forwarding
            }
        }
        else 
        {
            //constant
            //cout << "First occurrence is constant" << ins_detail[4 + i] << endl;
            if (found_neg != string::npos)
            {
                lst[i] += "-";
            }
            for (int j = 0; j < ins_detail[4 + i].length(); j++)
            {
                if (isdigit(ins_detail[4 + i][j]))
                {
                    lst[i] += ins_detail[4 + i][j];
                }
            }
            valuelst[i] = stoi(lst[i]);                                     //constant no forwarding
        }
        //cout << valuelst[i] << endl;
    } 
    return valuelst;
}

bool phaseIsAvailable(string phase)
{
    for (int i = 0; i < running.lencheck(); i++)
    {
        string* run_ins = running.getByIndex(i);
        if (run_ins[1].compare(phase) == 0)
        {
            return false;
        }
    }
    return true;
}

void signalCheck(string* ins_detail, string order) 
{
    bool con = false;    
    ins_detail[9] = "";
    if (ins_detail[1] == "EX") 
    {
        con = true;
        //RegDST
        if (order == "add" || order == "sub")
        {
            ins_detail[9] += "1";
        }
        else if (order == "lw")
            ins_detail[9] += "0";
        else
            ins_detail[9] += "X";

        //ALUSRC
        if (order == "add" || order == "sub" || order == "beq")
        {
            ins_detail[9] += "0";
        }
        else if (order == "lw" || order == "sw")
            ins_detail[9] += "1";
        ins_detail[9] += " ";
    }
    if (ins_detail[1] == "MEM" || con)
    {
        con = true;

        //BRANCH
        if (order == "beq")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";

        //MEMREAD
        if (order == "lw")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";

        //MEMWRITE
        if (order == "sw")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";

        ins_detail[9] += " ";
    }
    if (ins_detail[1] == "WB" || con)
    {
        con = true;

        //REGWRITE
        if (order == "add" || order == "sub" || order == "lw")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";

        //MEMTOREG
        if (order == "add" || order == "sub")
        {
            ins_detail[9] += "0";
        }
        else if (order == "lw")
            ins_detail[9] += "1";
        else
            ins_detail[9] += "X";



        /*
        //ALUOP1
        if (order == "add" || order == "sub")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";

        //ALUOP0
        if (order == "beq")
        {
            ins_detail[9] += "1";
        }
        else
            ins_detail[9] += "0";
        */

    }
}


void runADD(string* ins_detail,int rmcount, string* tobeforward) 
{
    string tar_reg = getRegNum(ins_detail[4]);
    int* valuelst = getValue(ins_detail, rmcount);
    //ins_detail[8] = to_string(valuelst[1] + valuelst[2]);
    *tobeforward = to_string(valuelst[1] + valuelst[2]);
                                                                                                                  
    //cout<<"add result: $"<< tar_reg << " : " << *tobeforward << endl;
}

void runSUB(string* ins_detail, int rmcount, string* tobeforward)
{
    string tar_reg = getRegNum(ins_detail[4]);
    int* valuelst = getValue(ins_detail, rmcount);

    *tobeforward = to_string(valuelst[1] - valuelst[2]);

    //cout <<"sub result: $"<< tar_reg << " : " << *tobeforward << endl;
}

bool runBEQ(string* ins_detail, int rmcount, string* data, int* pc)
{
    int* valuelst = getValue(ins_detail, rmcount);
    //check condition
    if (valuelst[0] == valuelst[1])
    {
        running.Pop();
        int jumpRange = valuelst[2];
        int jumpto = *pc + jumpRange;
        string* length = new string[10];
        length[0] = data[jumpto];
        length[1] = "IF";
        length[8] = "None";
        string order = "";
        size_t found_space = length[0].find(" ");
        for (int i = 0; i < found_space; i++)
        {
            order += length[0][i];
        }
        length[2] = order;
        running.Push_back(length);
        *pc = jumpto;
        //cout << "jump to: " << jumpto << endl;
        return true;
    }
    else
        return false;
}
void runSW(string* ins_detail , int rmcount)                                                                   
{
    int* valuelst = getValue(ins_detail, rmcount);
    size_t found_reg = ins_detail[5].find("$");
    size_t found_p1 = ins_detail[5].find("(");
    size_t found_p2 = ins_detail[5].find(")");
    string s = "";
    for (int j = 0; j < found_p1; j++)
    {
        if (isdigit(ins_detail[5][j]))
        {
            s += ins_detail[5][j];
        }
    }
    int offset = stoi(s);
    s = "";
    for (int j = found_reg + 1; j < found_p2; j++)
    {
        if (isdigit(ins_detail[5][j]))
        {
            s += ins_detail[5][j];
        }
    }
    int tar_word;
    if (getforwardingvalue(s) == NULL)
    {
        tar_word = (offset + registers[stoi(s)]) / 4;                      //no forwarding
    }
    else
        tar_word = (offset + getforwardingvalue(s)) / 4;                     //forwarding
    words[tar_word] = valuelst[0];
    //cout <<"sw result: mem"<<tar_word<<" changed : "<<words[tar_word] << endl;



}
void runLW(string* ins_detail, int rmcount, string* tobeforward) 
{
    int* valuelst = getValue(ins_detail, rmcount);
    string tar_reg = getRegNum(ins_detail[4]);

    *tobeforward = to_string(valuelst[1]);
    //cout <<"lw result: $"<<tar_reg<<" : "<< *tobeforward << endl;

}

bool runBEQidcheck(string* ins_detail, int rmcount, int index)
{
    int* valuelst = getValue(ins_detail, rmcount);
    string tar_reg1 = getRegNum(ins_detail[4]);
    string tar_reg2 = getRegNum(ins_detail[5]);
    if (checkRegAccessable(index,tar_reg1) && checkRegAccessable(index,tar_reg2))
    {
        return true;
    }
    else
    {
        return false;
    }


}








int main()
{
    //初始化
    string txt = "memory8.txt";
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 1;
        words[i] = 1;

    }
    registers[0] = 0;
    ifstream memoryTxt_pre(txt);
    
    string instruction;
    int total_instruction = 0;                                 //記錄總instruction數量
    while (getline(memoryTxt_pre, instruction)) 
    {
        total_instruction += 1;
    }    
    memoryTxt_pre.close();

    stringstream ss;                                             //用於字串切割，空格切割                            
    string* data = new string[total_instruction];              //主要資料存儲
    
    ifstream memoryTxt(txt);
    int cur_instruction_count = 0;
    while (getline(memoryTxt, instruction))
    {
        data[cur_instruction_count] = instruction;
        cur_instruction_count += 1;
        
    }
    int circle = 0;
    int cur_addr = -1;
    int succeed_count = 0;
    //cout <<"total_instruction: "<< total_instruction << endl;


    while (true)
    {
        int forwardinglength = running.lencheck();
        string* tobeforwarding = new string[forwardinglength];
        for (int i = 0; i < forwardinglength; i++) 
        {
            tobeforwarding[i] = "None";
        }
        

        for (int i = 0; i <running.lencheck(); i++) {
            string* run_ins = running.getByIndex(i);
            int j;
            string order;
            string reg_mem;
         
            for (j = 0; j <= 4; j++)                                                    //更新phase
            {
                if (run_ins[1].compare(phase[j]) == 0)
                    break;
            }

            if (run_ins[1] == "ID"  && run_ins[2]!="beq")                                                     //檢查要使用的reg是不是上面instruction的result且已經處理完畢，若還沒則保持在"ID"   :: stall
            {            
                int chk_begin = 0;
                int chk_end = 0;
                if (run_ins[2] == "add" || run_ins[2] == "sub") {
                    chk_begin = 1;
                    chk_end = stoi(run_ins[3]);
                }                             
                else if (run_ins[2] == "sw") 
                {
                    chk_begin = 0;
                    chk_end = 1;
                }
                bool regInUse = false;
                for (int rmc = chk_begin; rmc < chk_end; rmc++)
                {
                    string reg = "";
                    if (run_ins[4 + rmc].find("$") != string::npos) {
                        reg= getRegNum(run_ins[4 + rmc]);
                        //cout << reg << endl;
                        if (!checkRegAccessable(i,reg))                                        //checkRegAccessable(i,reg) = false :: 要使用的reg為上面instruction的result且還未處理完畢 ：：做stall
                        {
                            regInUse = true;
                            break;
                        }
                    }
                }
                if (regInUse)
                    continue;
            }
            if (run_ins[1] == "ID" && run_ins[2] == "beq" && branchOK) 
            {
                bool access = runBEQ(run_ins, stoi(run_ins[3]), data, &cur_addr);
                run_ins[1] = "EX";
                if (access) {             //beq成功跳躍，建設好跳躍ins的if后跳下一個cycle
                    break;
                }
                else
                    continue;
            }
            if (run_ins[1] == "ID" && run_ins[2] == "beq" && !branchOK) 
            {
                branchOK = runBEQidcheck(run_ins, stoi(run_ins[3]),i);
                continue;
            }
            if (j == 4)                                                                //instruction處理完畢
            {
                running.Delete(i);
                i-=1;
                continue;
            }

            if (phaseIsAvailable(phase[j + 1])) {
                run_ins[1] = phase[j + 1];                                                //更新phase
            }
            else
                continue;





            if (run_ins[1].compare("ID") == 0)
            {
                //ID phase
                ss << run_ins[0];
                ss >> order;
                run_ins[2] = order;
                //cout << run_ins[2] << endl;
                         

                int rmc = 0;                                        //計算instruction中reg_mem數量
                while (ss >> reg_mem)
                {
                    size_t found_comma_left = reg_mem.find(",");
                    string prereg = "";
                    if (found_comma_left != string::npos) 
                    {
                        if (found_comma_left == reg_mem.length() - 1)       //...,
                        {
                            run_ins[4 + rmc] = reg_mem;
                            cout << reg_mem << endl;
                            rmc+=1;
                            continue;
                        }
                        size_t found_comma_right = reg_mem.find(",",found_comma_left+1);
                        while (found_comma_right != string::npos)          //,...,...  / ,...
                        {
                            for (int xx = found_comma_left + 1; xx < found_comma_right; xx++)
                            {
                                prereg += reg_mem[xx];
                            }
                            run_ins[4 + rmc] = prereg;
                            cout << prereg << endl;
                            rmc+=1;
                            prereg = "";
                            found_comma_left = found_comma_right;
                            found_comma_right = reg_mem.find(",", found_comma_left + 1);
                        }
                        for (int xx = found_comma_left + 1; xx < reg_mem.length(); xx++) 
                        {
                            prereg += reg_mem[xx];
                        }
                        run_ins[4 + rmc] = prereg;
                        cout << prereg << endl;
                        rmc+=1;
                    }
                    else     // no comma
                    {
                        run_ins[4 + rmc] = reg_mem;
                        cout << reg_mem << endl;
                        rmc+=1;
                    }
                    //reg_mem seperate with space(can be updated)
                    //reg_mem = replaceChar(reg_mem, ",");

                    //run_ins[4+rmc] = reg_mem;                       //前面四個： 0:instruction , 1:phase , 2:order , 3:reg_mem_num
                    //rmc += 1;
                }

                ss.clear();                                        //clear stringBuilder
                run_ins[3] = to_string(rmc);                                   //記錄數量
                if (run_ins[2] == "beq")
                {
                    branchOK = runBEQidcheck(run_ins, stoi(run_ins[3]),i);
                }

            }
            else if (run_ins[1].compare("EX") == 0)
            {
                //check signal 
                order = run_ins[2];
                if (order.compare("add") == 0) {
                    runADD(run_ins, stoi(run_ins[3]), &tobeforwarding[i]);
                }
                if (order.compare("sub") == 0) {
                    runSUB(run_ins, stoi(run_ins[3]), &tobeforwarding[i]);
                }
                signalCheck(run_ins, order);


            }
            else if (run_ins[1].compare("MEM") == 0)
            {
                order = run_ins[2];
                if (order.compare("lw") == 0)
                    runLW(run_ins, stoi(run_ins[3]),&tobeforwarding[i]);
                if (order.compare("sw") == 0)
                    runSW(run_ins, stoi(run_ins[3]));
                signalCheck(run_ins, order);

            }
            else if (run_ins[1].compare("WB") == 0) 
            {
                order = run_ins[2];
                if (order.compare("add") == 0 || order.compare("sub") == 0 || order.compare("lw") == 0) 
                {
                    string tar_reg = getRegNum(run_ins[4]);
                    registers[stoi(tar_reg)] = stoi(run_ins[8]);
                    //cout << "register change: $" << tar_reg << " : " << registers[stoi(tar_reg)] << endl;
                }   
                signalCheck(run_ins, order);
            }
        }
        if (cur_addr+1 < total_instruction)
        {                                                                          
            if (phaseIsAvailable("IF")) {
                cur_addr += 1;
                string* length = new string[10];
                length[0] = data[cur_addr];
                length[1] = "IF";
                length[8] = "None";
                string order = "";
                size_t found_space = length[0].find(" ");
                for (int i = 0; i < found_space; i++)
                {
                   order += length[0][i];
                }
                length[2] = order;
                running.Push_back(length);
                
            }
        }
        if (running.IsEmpty())
        {
            break;
        }
        else 
        {
            circle += 1;
            cout << "Cycle " << circle << endl;
        }

        for (int i = 0; i < running.lencheck(); i++) {                            //print result
            string* run_ins = running.getByIndex(i);
            cout << run_ins[2] + ": " + run_ins[1]+" "+run_ins[9] << endl;
        }
        //cout <<"program counter: " << cur_addr << endl;
        //cout << "len: " << running.lencheck() << endl;
        cout << endl;


        for (int i = 0; i < forwardinglength; i++)
        {
            if (tobeforwarding[i] != "None")
            {
                string* run_ins = running.getByIndex(i);
                run_ins[8] = tobeforwarding[i];
            }
        }
        delete[] tobeforwarding;

    }
    //output final
    cout << "需要花費"<<circle<<"個cycles" << endl;
    for (int i = 0; i < 32; i++) 
    {
        cout << "$" << i << " ";
    }
    cout << endl;
    for (int i = 0; i < 32; i++)
    {
        cout << registers[i] << "  ";
        if (i >= 10)
            cout << " ";
    }
    cout << endl;
    for (int i = 0; i < 32; i++)
    {
        cout << "W" << i << " ";
    }
    cout << endl;
    for (int i = 0; i < 32; i++)
    {
        cout << words[i] << "  ";
        if (i >= 10)
            cout << " ";
    }
    cout << endl;





    memoryTxt.close();
}

