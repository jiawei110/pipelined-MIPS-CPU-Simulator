# pipelined-MIPS-CPU-Simulator
This is a pipelined-MIPS-CPU_Simulator written in C++

## basic data structure
###  linked list
  a linked list (running) used to saved all running instruction. dynamic string array in ChainNode is used to save details of running instruction.<br><br>
  Assume that name of the dynamic string array as "run_ins":
  <ul>
    <li>run_ins[0] : full instruction string
    <li>run_ins[1] : phase of instruction { "IF","ID","EX","MEM","WB" }
    <li>run_ins[2] : order of instruction { "lw","sw","add","sub","beq" }
    <li>run_ins[3] : total number of register, memory & constant in instruction {ex : add $4, $5, $3 , run_ins[3] = 3}
    <li>run_ins[4,5,6] : register/memory/constant name , {ex： $4 , 4($0), 1}
    <li>run_ins[7] : empty
    <li>run_ins[8] : result of instruction, use for forwarding
    <li>run_ins[9] : signal
  </ul>
<h3>others</h3>
<ul>
  <li>int registers[32] : to save result of register.
  <li>int words[32] : to save result in memory. 
  <li>bool branckOk : identify whether registers in beq instruction is ready
  <li>string txt : to save filename to be read.
  <li>int total_instruction : number of instruction
  <li>string* data : all instruction string
  <li>cur_addr : program counter
  <li>string* tobeforwarding : save all result produced in cycle and allow them to be used for forwarding after finishing a cycle
</ul>

## function
<ul>
<li>string replaceChar() : same as .replace in python , return a string
<li>string getRegNum() : get only number in register name {ex: 4 in $4}
<li>bool checkRegAccessable : check whether register is busy , return false when busy
<li>int getforwardingvalue() : return forwarding iteger when forwarding exists
<li>int* getValue() : return all value of registers/memory in instruction
<li>bool phaseIsAvailable() : check whether phase is available (instruction before it not in this phase)
<li>void signalCheck() : build each instruction signal
<li>runADD(),runSUB(),runBEQ(),runSW(),runLW() : function
<li>bool runBEQidcheck() : check whether registers in beq instruction are availabel
</ul>
  
  
  
