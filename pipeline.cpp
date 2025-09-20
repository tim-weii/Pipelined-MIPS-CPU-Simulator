#include <iostream>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

int reg[32][3]={0};	   // 32 registers
int pc=0;
int pc_front=pc;
int pc_pre=pc;
int mem[32]={0};	// 32 memory size
ofstream ofs; 

//////////////////////////////////////
//initialization memory and register
//////////////////////////////////////

// 記憶體中的每個word都是1
// $0暫存器的值為0，其他都是1
void init(){
	for(int i=0;i<32;i++){
		mem[i]=1;
	}
	for(int i=1;i<32;i++){
		reg[i][0]=1;
	}
	reg[0][2]=1;
}

///////////////////////////////////////
//Process class
///////////////////////////////////////

// 指令 
class Process{
	public:
		char code[20];		//instruction
		/////////////////////////////////
		//code type lw rd, rs(rt)
		//code type sw rd, rs(rt)
		//code type add rd, rs, rt
		//code type sub rd, rs, rt
		//code type beq rd, rs, rt
		/////////////////////////////////
		int ALU_value;		//ALU_work output	
		int rd;				//register destination
		int rs;				//register source
		int rt;				//register target 
		int dead;			//print mem and wb if beq success 
		int opcode;			//opcode code(lw=1,sw=2,add=3,sub=4)
		char control[8];	//control signal
		int step;			//record process current state 
		int flag;			//to detect data hazard 
		int ID;				//process address
		void work();
		void set_code(char*,int,int);
		void IF();
		void lw(int);
		void sw(int);
		void add(int);
		void sub(int);
		void beq(int);
		void cal(int);
		void ls(int);
		void beqC(int);
		int ALU_work();
		
};

///////////////////////////////////////
//write code into process and initialization 
/////////////////////////////////////// 

void Process::set_code(char* buffer,int len,int ID_code){
	for(int i=0;i<len;i++){
		char *tmp=buffer+i;
		code[i]=*tmp;
	}
	step=0;
	flag=0;
	ID=ID_code; 
	dead=0;
}

///////////////////////////////////////
//simulation process work in ALU 
///////////////////////////////////////

int Process::ALU_work(){
	switch(opcode){
		case 1:
			return rt+rs/4;
			break;
		case 2:
			return rt+rs/4;
			break;
		case 3:
			return rs+rt;
			break;
		case 4:
			return rs-rt;
			break;
		case 5:
			if(rs==rd){
				rd=32;
				pc=ID+1+rt;
				pc_front=pc;
			}
			break;
		default:
			cout<<"error opcode\n";
			break;
	}
	return 0;
}
///////////////////////////////////////
//process work main function
///////////////////////////////////////

//五個階段  IF, ID,EX, MEM, WB 
void Process::work(){
	switch(step){
		case 0:
			//IF
			step=1;
			IF();
			break;
		case 1:
			//ID
			step=1;
			if(opcode==5){
				if(!reg[rd][1]&&!reg[rs][1]){
					rd=reg[rd][0];
					rs=reg[rs][0];
					step++;
				}
				else{
					flag=1;
				}
				
			}
			else if(opcode==4||opcode==3||opcode==1){
				if(!reg[rs][1]&&!reg[rt][1]){
					rs=reg[rs][0];
					rt=reg[rt][0];
					reg[rd][1]=1;
					step++;
				}
				else{
					flag=1;
				}	
					
			}
			else if(opcode==2){
				if(!reg[rd][1]){
					rd=reg[rd][0];
					step++;
				}
				else{
					flag=1;
				}
			}
			
			break;
		case 2:
			//EX
			flag=0;
			step=3;
			ALU_value=ALU_work();
			break;
		case 3:
			//MEM
			if(opcode==1){
				ALU_value=mem[ALU_value];
			}
			else if(opcode==2){
				mem[ALU_value]=rd;
			}
			step=4;
			break;
		case 4:
			//WB
			if((opcode==1)||(opcode==3) || (opcode==4)){
				if(!rd){
					cout<<"error write back code.\n";
					ofs<<"error write back code.\n";
					exit(1);
				}
				reg[rd][0]=ALU_value;
				reg[rd][1]=0;
			}	
			step=5;
			break;
		default:
			cout<<"process error\n";
			break; 
	}
}

///////////////////////////////////////
//classification by opcode 
///////////////////////////////////////

void Process::IF(){
	if (code[0] == 'a'){
		add(2);
	}
	else if (code[0] == 'l'){
		lw(3);
	}
	else if (code[0] == 'b'){
		beq(2);
	}
	else if (code[0] == 's'){
		if (code[1] == 'u'){
			sub(2);
		}
		else if (code[1] == 'w'){
			sw(3);
		}
	}
}
///////////////////////////////////////
//set lw opcode number and control signal 
///////////////////////////////////////
void Process::lw(int n){
	opcode = 1;
	control[0] ='0';
	control[1] ='1';
	control[2] ='0';
	control[3] ='1';
	control[4] ='0';
	control[5] ='1';
	control[6] ='1';
	ls(n);
}
///////////////////////////////////////
//set sw opcode number and control signal 
///////////////////////////////////////
void Process::sw(int n){
	opcode = 2;
	control[0] ='X';
	control[1] ='1';
	control[2] ='0';
	control[3] ='0';
	control[4] ='1';
	control[5] ='0';
	control[6] ='X';
	//"X10010X";
	ls(n);
}
///////////////////////////////////////
//set add opcode number and control signal 
///////////////////////////////////////
void Process::add(int n){
	opcode = 3;
	control[0] ='1';
	control[1] ='0';
	control[2] ='0';
	control[3] ='0';
	control[4] ='0';
	control[5] ='1';
	control[6] ='0';
	//"1000010";
	cal(n);
}
///////////////////////////////////////
//set sub opcode number and control signal 
///////////////////////////////////////
void Process::sub(int n){
	opcode = 4;
	control[0] ='1';
	control[1] ='0';
	control[2] ='0';
	control[3] ='0';
	control[4] ='0';
	control[5] ='1';
	control[6] ='0';
	//"1000010";
	cal(n);
}
///////////////////////////////////////
//set beq opcode number and control signal 
///////////////////////////////////////
void Process::beq(int n){
	opcode = 5;
	control[0] ='X';
	control[1] ='0';
	control[2] ='1';
	control[3] ='0';
	control[4] ='0';
	control[5] ='0';
	control[6] ='X';
	//"X01000X";
	beqC(n);
}
///////////////////////////////////////
//decode R type instruction
///////////////////////////////////////
void Process::cal(int n){
	int Addr[4]={0};
	for (int i = 0; i < 3; i++){
		char tempS[3];
		n += 3;
		int t = 0;
		while (n<=20){
			if (code[n] == ','||code[n]=='\0'){
				tempS[t]= '\0';
				break;
			}
			tempS[t] = code[n];
			t++;
			n++;
		}
		Addr[i] = atoi(tempS);
	}
	rd = Addr[0];
	rs = Addr[1];
	rt = Addr[2];
}
///////////////////////////////////////
//decode lw and sw instruction
///////////////////////////////////////
void Process::ls(int n){
	char tempS[3];
	n += 1;
	int t = 0;
	while (n<=20){
		if (code[n] == ','){
			tempS[t]= '\0';
			break;
		}
		tempS[t] = code[n];
		n++;
		t++;
	} 
	rd = atoi(tempS);
	char tempT[3];
	n+=2;
	t = 0;
	while (n<=20){
		if (code[n] == '('){
			tempT[t]= '\0';
			break;
		}
		tempT[t] = code[n];
		n++;
		t++;
	}
	rs = atoi(tempT);
	char tempU[3];
	n += 2;
	t = 0;
	while (n<=20){
		if (code[n] == ')'){
			tempS[t]= '\0';
			break;
		}
		tempU[t] = code[n];
		n++;
		t++;
	}
	rt = atoi(tempU);
}
///////////////////////////////////////
//decode beq instruction
///////////////////////////////////////
void Process::beqC(int n){
	int Addr[4]={0};
	for (int i = 0; i < 3; i++){
		char tempS[3];
		n += 3;
		if (i == 2)
			n--;
		int t = 0;
		while (n<20){
			if (code[n] == ','||code[n] == '\0'){
				tempS[t]='\0';
				break;
			}
			tempS[t] = code[n];
			t++;
			n++;
		}
		Addr[i] = atoi(tempS);
	}
	rd = Addr[0];
	rs = Addr[1];
	rt = Addr[2];
}

Process P[1000];	
////////////////////////////////////////
//print process current state
////////////////////////////////////////
void print_data(int ID){
	switch(P[ID].step){
		case 5:
			cout<<"WB";
			ofs <<"WB";
			break;
		case 4:
			cout<<"MEM";
			ofs <<"MEM";
			break;
		case 3:
			cout<<"EXE"; 
			ofs<<"EXE";
			break;
		case 2:
			cout<<"ID";
			ofs <<"ID";
			break;
		case 1:
			if(P[ID].flag){
				cout<<"ID";
				ofs<<"ID";
			}
			else{
				cout<<"IF";
				ofs<<"IF";
			}
			break;
	}
}
////////////////////////////////////////
// print_opcode
//////////////////////////////////////// 
void print_code(int ID){
	switch(P[ID].opcode){
		case 1:
			cout<<"lw: ";
			ofs<<"lw: ";
			break;
		case 2:
			cout<<"sw: ";
			ofs<<"sw: ";
			break;
		case 3:
			cout<<"add: ";
			ofs<<"add: ";
			break;
		case 4:
			cout<<"sub: ";
			ofs<<"sub: ";
			break;
		case 5:
			cout<<"beq: ";
			ofs<<"beq: ";
			break;
		default:
			break;
	}
}
////////////////////////////////////////
//print control signal
////////////////////////////////////////
void print_control(int ID){
	int start=0;
	switch(P[ID].step){
		case 3:
			start=0;
			break;
		case 4:
			start=2;
			break;
		case 5:
			start=5;
			break;
		default:
			start=8;
			break;
	}
	for(int i=start;i<8;i++){
		if(i==0||i==2||i==5||i==8){
			cout<<" ";
			ofs<<" ";
		}
		cout<<P[ID].control[i];
		ofs <<P[ID].control[i];
	}
	cout<<"\n";
	ofs <<"\n";
}
////////////////////////////////////////
// integrate print function
////////////////////////////////////////
void print_all(int ID){
	print_code(ID);
	print_data(ID);
	print_control(ID); 
}
int main(){
	int cycle=1;
	int pc_lim=0;
	init();
	ifstream ifs;
////////////////////////////////////////
//read input data
////////////////////////////////////////
	char buffer[256]={0};
	ifs.open("input.txt");
	if(!ifs.is_open()){
		cout<<"open file error!!\n";
	}
	else{
		int line=0;
		
		while (!ifs.eof()) {
            ifs.getline(buffer, sizeof(buffer));
            P[line].set_code(buffer,sizeof(buffer),line);
			line++;
			pc_lim++;
        }
		ifs.close();
	}
////////////////////////////////////////
//start exeucte 
////////////////////////////////////////
	ofs.open("output.txt");
	int subcycle = 0;
	while(pc_front<=pc_lim-1){
		if(!subcycle){
			cout<<"============\n";
			cout<<"cycle:"<<cycle<<"\n";
			ofs<<"\ncycle:"<<cycle<<"\n";
		}
		subcycle=0;
/////////////////////////////////////////
//if beq success, print beq MEM and WB in next two cycle 
/////////////////////////////////////////
		for(int dead=0;dead<=pc_lim;dead++){
			switch(P[dead].dead){
				case 0:
					break;
				case 1:
					P[dead].dead+=1;
					break;
				case 2:
					P[dead].dead+=1;
					cout<<"beq: MEM 100 0X\n";
					ofs<<"beq: MEM 100 0X\n";
					break;
				case 3:
					P[dead].dead+=1;
					cout<<"beq: WB 0X\n";
					ofs<<"beq: WB 0X\n";
					break;
				case 4:
					P[dead].dead=0;
					break;
			} 
		}
		for(int pc_now=pc_front;pc_now<=pc;pc_now++){
			if(pc_now!=pc_front&&P[pc_now-1].step<=P[pc_now].step){
				print_all(pc_now);
				continue;				//stall	to avoid P[i].step <= P[i].step
			}
			else if(P[pc_now-1].flag&&P[pc_now-1].step==2){
				print_all(pc_now);
				continue;				//stall2 to avoid P[i].step =ID and P[i+1].step=ID if data hazard happend
			}
			else{
				P[pc_now].work();
				print_all(pc_now);
			}
			if(P[pc_now].rd==32){		//beq success
				P[pc_now].step=0;		
				P[pc_now].dead=1;		//set beq dead =1 to print beq mem and wb in next two cycle
				cycle--;				
				subcycle=1;
				for(int i=0;i<32;i++){	//register read lock reset
					reg[i][1]=0;
				}
				if(pc_now<pc){			//reset process step between new pc and beq ID
					for(int i=pc_now;i<=pc;i++){
						P[i].step=0;
						P[i].flag=0;
					}
				}	
				else{					//reset process step between new pc and beq ID,and branch predict instruction
					for(int i=pc;i<=pc_now+3;i++){		
						P[i].step=0;
						P[i].flag=0;
					}
				}
				break;
			}	
			if(!subcycle&&(P[pc].step>=2||(P[pc].step==1&&P[pc].flag==1))){	//create new pipeline 
				pc++;
			}
			if(pc>=pc_lim){			//end line 
				pc=pc_lim-1;
			}
		}			
		if(P[pc_front].step==5){	
			pc_front+=1;			//WB end delete pipeline 
		}
		cycle++;
	}
	ofs<<"total need "<<cycle-1<<" cycles\n";
	ofs.close();
	ofs.open("register.txt");
	for(int i=0;i<32;i++){
		ofs<<"$"<<i<<":"<<reg[i][0]<<"\n";
	}
	ofs.close();
	ofs.open("memory.txt");
	for(int i=0;i<32;i++){
		ofs<<"W"<<i<<":"<<mem[i]<<"\n";
	}
	ofs.close();
	system("pause");
	return 0;
}

