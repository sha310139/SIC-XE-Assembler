#include<iostream>
#include<iomanip>
#include<algorithm>
#include<string>
#include<cctype>
#include<fstream>
#include<stdlib.h>
#include<cstring>
#include<vector>
#include<sstream>
#define HASH_SIZE 100
#define OPTABLE_SIZE 59
#define REGTABLE_SIZE 9

using namespace std;

typedef struct tokenT {
	string tokenStr;
	int tokenType;
	int tokenNo;
} tokenType ;

typedef struct lineT {
	vector<tokenType> token;
} lineType ;


typedef struct optableT {
	string instruction;
	string opcode;
	int format;
	bool isSICXE;
} optableType ;

typedef struct regT {
	string reg;
	string regNo;
} regType ;

typedef struct symbolT {
	string symbol;
	string loc;
} symbolType ;

typedef struct literalT {
	string literal;
	string loc;
	string objectCode;
	int tokenType;
} literalType ;

typedef struct instrT {
	string instrStr;
	string op1;
	string op2;
	int format;
	string loc;
	string opcode;
	bool error;
	string n,i,x,b,p,e;
	string objectCode;
} instrType ;

optableType gOptable[OPTABLE_SIZE] = {
						        "ADD", "18", 3, false,     "ADDF", "58", 3, true,     "ADDR", "90", 2, true,
                          	    "AND", "40", 3, false,    "CLEAR", "B4", 2, true,     "COMP", "28", 3, false,
                              "COMPF", "88", 3, true,     "COMPR", "A0", 2, true,      "DIV", "24", 3, false,
                          	   "DIVF", "64", 3, true,      "DIVR", "9C", 2, true,      "FIX", "C4", 1, true,
                              "FLOAT", "C0", 1, true,       "HIO", "F4", 1, true,        "J", "3C", 3, false,
                        	    "JEQ", "30", 3, false,      "JGT", "34", 3, false,     "JLT", "38", 3, false,
                        	   "JSUB", "48", 3, false,      "LDA", "00", 3, false,     "LDB", "68", 3, true,
                        	   "LDCH", "50", 3, false,      "LDF", "70", 3, true,      "LDL", "08", 3, false,
                     	        "LDS", "6C", 3, true,       "LDT", "74", 3, true,      "LDX", "04", 3, false,
                          	    "LPS", "D0", 3, true,       "MUL", "20", 3, false,    "MULF", "60", 3, true,
                         	   "MULR", "98", 2, true,      "NORM", "C8", 1, true,       "OR", "44", 3, false,
                           	     "RD", "D8", 3, false,      "RMO", "AC", 2, true,     "RSUB", "4C", 3, false,
                       	     "SHIFTL", "A4", 2, true,    "SHIFTR", "A8", 2, true,      "SIO", "F0", 1, true,
                          	    "SSK", "EC", 3, true,       "STA", "0C", 3, false,     "STB", "78", 3, true,
                         	   "STCH", "54", 3, false,      "STF", "80", 3, true,      "STI", "D4", 3, true,
                         	    "STL", "14", 3, false,      "STS", "7C", 3, true,     "STSW", "E8", 3, false,
                         	    "STT", "84", 3, true,       "STX", "10", 3, false,     "SUB", "1C", 3, false,
                         	   "SUBF", "5C", 3, true,      "SUBR", "94", 2, true,      "SVC", "B0", 2, true,
                           	     "TD", "E0", 3, false,      "TIO", "F8", 1, true,      "TIX", "2C", 3, false,
                         	   "TIXR", "B8", 2, true,        "WD", "DC", 3, false } ;


regType gRegTable[REGTABLE_SIZE] = { "A", "0", "X", "1", "L",  "2", "B",  "3", "S", "4",
                    	 			 "T", "5", "F", "6", "PC", "8", "SW", "9" } ;                         	   
                         	   
class ProcessToken {
	
public:
	void init();
	void lexicalAnalysis(string line, lineType &curLine);
	void output();
	void output_2();
	void open_file();
	void close_file();
	
	void SIC_pass1();
	void SIC_pass2();
	void SICXE_pass1();
	void SICXE_pass2();
	
private:
	vector<literalType> literal_table;
	vector<symbolType> symbol_table;
	vector<instrType> vInstr;
	vector<string> table1, table2, table3, table4;
	vector<string> table5, table6, table7;
	vector<string> vSource;
	vector<lineType> vLine;
	fstream fp;
	string fileName;
	
	void getToken(string& str, tokenType& temp, string type);
	bool isWhiteSpace(char ch);
	bool isDelimiter(char ch);
	bool isEnter(char ch);
	bool findTable(string str, int &num, vector<string> vTable);
	bool isAllNumber(string str);
	int stringToInt(string str);
	int hash(string str, vector<string>& vTable);
	int hash(string str, vector<symbolType>& vTable);
	int hash(string str, vector<literalType>& vTable);
	string decToHex(int num);
	bool loadTable1to4();
	bool readTable(vector<string> & vTable, int tableNo);
	void SIC_syntax(vector<tokenType> token, instrType &curInstr, string &pc);
	void SICXE_syntax(vector<tokenType> token, instrType &curInstr, string &pc);
	void SIC_checkFormatAndLoc(int index, vector<tokenType> token, instrType &curInstr, string &pc);
	void SICXE_checkFormatAndLoc(int index, vector<tokenType> token, instrType &curInstr, string &pc);
	void checkPseudo(int index, vector<tokenType> token, instrType &curInstr, string &pc);
	void SIC_ObjectCode(instrType &curInstr);
	void SICXE_ObjectCode(instrType &curInstr, string pc, string bReg);
	void findOptable(string str, string &opcode, int &format, bool &isSICXE);
	int hexToDec(string str);
	string hexToBinary(string str);
	string binaryToHex(string str);
	string findSymbolLoc(string str);
	string findLiteralLoc(string str);
	string add_zero(string str, int needSize);
	string getRegNo(string str);
};  // class ProcessToken

void ProcessToken::init() {
	symbol_table.clear();
	literal_table.clear();
	vSource.clear();
	vLine.clear();
	vInstr.clear();
	table1.clear(); table2.clear(); table3.clear(); table4.clear();
	table5.clear(); table6.clear(); table7.clear();
	symbol_table.resize(HASH_SIZE); table5.resize(HASH_SIZE); table6.resize(HASH_SIZE); table7.resize(HASH_SIZE);
	literal_table.resize(HASH_SIZE);
} // end init()


bool ProcessToken::readTable(vector<string> & vTable, int tableNo) {
	fstream fp;
	string fileName;
	string temp;
	fileName = "Table" + to_string(tableNo) + ".table" ;
	fp.open(fileName, ios::in);
	if (fp.is_open()) {
		vTable.push_back("");  // the first element [0] is no item
		while(fp >> temp) {    // if use getline, it will read white space into string
			vTable.push_back(temp);
		} // end while
		fp.close();
		return true;
	} // if
	else return false;
} // end readTable()

bool ProcessToken::loadTable1to4() {
	if (readTable(table1, 1) && readTable(table2, 2) && 
		readTable(table3, 3) && readTable(table4, 4) )
		return true;
	return false;
} // end loadTable1to4()

void ProcessToken::open_file() {
	fileName = "";
	do {
		cout << "\nEnter file name :";
		cin >> fileName;
		fp.open(fileName+".txt", ios::in);
	} while (!fp.is_open()) ;
} // open_file()

void ProcessToken::close_file() {
	fp.close();
} // close_file()

void ProcessToken::SIC_pass1() {
	if (!loadTable1to4()) return ;
	string line = "", pc = "0000";
	lineType curLine;
	instrType curInstr;
	
	for (int i = 0 ; getline(fp, line, '\n') ; i++) {
		lexicalAnalysis(line, curLine);
		
		if (!curLine.token.empty()) {
			vSource.push_back(line);
			vLine.push_back(curLine);
			SIC_syntax(curLine.token, curInstr, pc);
			curLine.token.clear();
		} // if
	} // for
} // SIC_pass1()

void ProcessToken::SIC_pass2() {
	
	if (!loadTable1to4()) return ;
	
	string line = "", pc = "0000";
		
	for (int i = 0 ; i < vInstr.size() ; i++) {
		if (i+1 < vInstr.size())			pc = vInstr[i+1].loc;   // pc指向下一道指令
		if (!vInstr[i].error)				SIC_ObjectCode(vInstr[i]);
	} // for
	
} // SIC_pass2()

void ProcessToken::SICXE_pass1() {
	if (!loadTable1to4()) return ;
	
	string line = "", pc = "0000";
	lineType curLine;
	instrType curInstr;
	
	for (int i = 0 ; getline(fp, line, '\n') ; i++) {
		lexicalAnalysis(line, curLine);
		if (!curLine.token.empty()) {
			vSource.push_back(line);
			vLine.push_back(curLine);
			SICXE_syntax(curLine.token, curInstr, pc);
			curLine.token.clear();
			if (curInstr.error) cout << "error";
		} // if
	} // for
	
} // SICXE_pass1()

void ProcessToken::SICXE_pass2() {
	if (!loadTable1to4()) return ;
	string line = "", pc = "0000", bReg = "0000";
		
	
	for (int i = 0 ; i < vInstr.size() ; i++) {
		if (i+1 < vInstr.size())				pc = vInstr[i+1].loc;   // pc指向下一道指令
		if (vInstr[i].instrStr == "BASE")		bReg = findSymbolLoc(vInstr[i].op1);
		if (!vInstr[i].error)					SICXE_ObjectCode(vInstr[i], pc, bReg);
	} // for
	
	
} // SICXE_pass2()



void ProcessToken::lexicalAnalysis(string line, lineType &curLine) {
	char ch = '\0';
	string str = "", type = "";
	
	tokenType curToken;	// 用來存目前處理的token 

	for(int i = 0 ; i < line.size() ; i++) {
		ch = line[i];
		if(isWhiteSpace(ch)) {  	 // get token or no token
			if (str != "") {	 // get token
				getToken(str, curToken, "none");  // get string
				curLine.token.push_back(curToken);
			} // if
		} // if
		
		else if(isDelimiter(ch)) {   // get 1 or 2 token
			if(ch == '\'' && (str == "C" || str == "X" || str == "c" || str == "x")) {
				type = "";
				if (str == "C" || str == "c")		type = "string";
				else 								type = "hex";
					
				str = "";
				str = str + ch;
				getToken(str, curToken, "delimiter");   // put '\'' into token
				curLine.token.push_back(curToken);
					
				i++;
				ch = line[i];
				while(ch != '\'') {
					str = str + ch;
					i++;
					ch = line[i];
				} // while
				getToken(str, curToken, type);  // get string
				curLine.token.push_back(curToken);
			} // if
			
			else if (str != "") {	 // get 2 token
				getToken(str, curToken, "none");
				curLine.token.push_back(curToken);
			} // else if
			
			str = str + ch;
			getToken(str, curToken, "delimiter");
			curLine.token.push_back(curToken);
			
			if (ch == '.') {	// comment
				i++;
				while(i < line.size()) {
					ch = line[i];
					str = str + ch;
					i++;
				} // while
				
				getToken(str, curToken, "comment");
				curLine.token.push_back(curToken);
			} // if
		} // else if
		else {
			str = str + ch;
		} // else
	} // end for
	
	if (str != "") {	 // last token
		getToken(str, curToken, "none");  // get string
		curLine.token.push_back(curToken);
	} // if
} // end lexicalAnalysis()

void ProcessToken::SIC_syntax(vector<tokenType> token, instrType &curInstr, string &pc) {
	// 判斷是否符合SIC的文法 
	symbolType temp;
	
	curInstr.instrStr = "";
	curInstr.loc = "";
	curInstr.op1 = "";
	curInstr.op2 = "";
	curInstr.opcode = "";
	curInstr.format = 0;
	curInstr.error = false;
	curInstr.n = "0", curInstr.i = "0", curInstr.x = "0", curInstr.b = "0", curInstr.p = "0", curInstr.e = "0";
	curInstr.objectCode = "";
	
	int hashValue = 0;
	
	if (token[0].tokenStr == ".") {
		curInstr.instrStr = ".";
		vInstr.push_back(curInstr);
		return;
	} // if

	int index = 0; // 紀錄opcode在哪個index 

    // 第一步 >> 看有沒有label, 順便放到symbol_table裡 + 找到指令的index
	if (token[0].tokenType == 5) {  			  // first >> label, second must be instruction or pseudo
		if (token.size() > 1 && (token[1].tokenType == 1 || token[1].tokenType == 2)) { 
			// label後面確定還有東西 && 是instruction 
			index = 1;  // 紀錄instruction的位置 
		} // if
		else {   // label後面沒有東西 or label後面第二個不是指令, 一定error 
			curInstr.error = true;
			return;
		} // else
	} // if
	else if (token[0].tokenType == 1 || token[0].tokenType == 2) {  // first >> instruction or pseudo
		index = 0;  // 紀錄instruction的位置 
	} // else if
	else {
		curInstr.error = true;
		return;
	} // else
	
	curInstr.instrStr = token[index].tokenStr;  // 將instrution指定 
	
	// 把pc初始化 
	if (token.size() == index+2 && token[index].tokenStr == "START") {  // {label} START hex_num
		pc = token[index+1].tokenStr;  // hex_num
	} // if
	else if (token.size() == index+2 && token[index].tokenStr == "EQU" && token[index+1].tokenType == 6) {
		pc = decToHex(stringToInt(token[index+1].tokenStr));
	} // else if
	else if (token.size() == index+2 && token[index].tokenStr == "EQU" && token[index+1].tokenType == 5) {
		pc = findSymbolLoc(token[index+1].tokenStr);
	} // else if
	
	curInstr.loc = pc;
	
	// pc初始化之後, symbol就可放進table 
	if (token[0].tokenType == 5) {  // 有symbol
		temp.loc = pc;
		temp.symbol = token[0].tokenStr;
		hashValue = hash(temp.symbol, symbol_table);
		symbol_table[hashValue] = temp; 
	} // if
	
	// 第二步 >> 確認是否為SIC指令 + 確認指令後面operand格式是否正確 + 更新pc 
	
	bool tempIsSICXE = false;
	
	if (token[index].tokenType == 1) {  // 是SICXE指令, 非SIC指令 
		findOptable(token[index].tokenStr, curInstr.opcode, curInstr.format, tempIsSICXE);
		if (tempIsSICXE) {
			curInstr.error = true;
			return;
		} // if
	} // if
	
	if (token[index].tokenType == 1) 	SIC_checkFormatAndLoc(index, token, curInstr, pc);
	else 								checkPseudo(index, token, curInstr, pc);
	
	vInstr.push_back(curInstr);
} // SIC_syntax()

void ProcessToken::SICXE_syntax(vector<tokenType> token, instrType &curInstr, string &pc) {
	// 判斷是否符合SICXE的文法 
	
	symbolType temp;
	instrType curLiteral;
	
	curInstr.instrStr = "", curInstr.loc = "", curInstr.op1 = "", curInstr.op2 = "";
	curInstr.opcode = "";
	curInstr.format = 0;
	curInstr.error = false;
	curInstr.n = "0", curInstr.i = "0", curInstr.x = "0", curInstr.b = "0", curInstr.p = "0", curInstr.e = "0";
	curInstr.objectCode = "";
	
	int hashValue = 0;
	if (token[0].tokenStr == ".") {
		curInstr.instrStr = ".";
		vInstr.push_back(curInstr);
		return;
	} // if

	int index = 0; // 紀錄opcode在哪個index 

    // 第一步 >> 看有沒有label, 順便放到symbol_table裡 + 找到指令的index

	if (token[0].tokenType == 5) {  			  // first >> label, second must be instruction or pseudo
		if (token.size() > 1 && (token[1].tokenType == 1 || token[1].tokenType == 2) ) { 
			// label後面確定還有東西 && 是instruction 
			index = 1;  // 紀錄instruction的位置 
		} // if
		else if (token.size() > 2 && token[1].tokenStr == "+" &&
				(token[2].tokenType == 1 || token[2].tokenType == 2) ) { 
			// label後面確定還有東西 && 是一個'+'和instruction 
			index = 2;  // 紀錄instruction的位置 
			curInstr.format = 4;
		} // else if
		else {   // label後面沒有東西 or label後面第二個不是指令, 一定error 
			curInstr.error = true;
			vInstr.push_back(curInstr);
			return;
		} // else
	} // if
	else if (token[0].tokenType == 1 || token[0].tokenType == 2) {  // first >> instruction or pseudo
		index = 0;  // 紀錄instruction的位置 
	} // else if
	else if (token.size() > 1 && 
			token[0].tokenStr == "+" && token[1].tokenType == 1) { // first >> '+' , second >> instruction
		index = 1;
		curInstr.format = 4;
	} // else if
	else {
		curInstr.error = true;
		vInstr.push_back(curInstr);
		return;
	} // else
	
	curInstr.instrStr = token[index].tokenStr;  // 將instrution指定 
	
	// 把pc初始化 
	if (token.size() == index+2 && token[index].tokenStr == "START") {  // {label} START hex_num
		pc = token[index+1].tokenStr;  // hex_num
		curInstr.loc = pc;
	} // if
	else if (token.size() == index+2 && token[index].tokenStr == "EQU" && token[index+1].tokenType == 6) {
		pc = decToHex(stringToInt(token[index+1].tokenStr));
		curInstr.loc = pc;
	} // else if
	else if (token.size() == index+2 && token[index].tokenStr == "EQU" && token[index+1].tokenType == 5) {
		pc = findSymbolLoc(token[index+1].tokenStr);
		curInstr.loc = pc;
	} // else if
	else if (token[index].tokenStr == "END") {
		curInstr.loc = "";
	} // else
	else {
		curInstr.loc = pc;
	} // else

	
	// pc初始化之後, symbol就可放進table 
	if (token[0].tokenType == 5) {  // 有symbol
		temp.loc = pc;
		temp.symbol = token[0].tokenStr;
		hashValue = hash(temp.symbol, symbol_table);
		symbol_table[hashValue] = temp; 
	} // if
	
	// 第二步 >> 確認指令後面operand格式是否正確 + 更新pc 
	
	if (token[index].tokenType == 1) {
		SICXE_checkFormatAndLoc(index, token, curInstr, pc);
	} // if
	else {
		checkPseudo(index, token, curInstr, pc);
	} // else
	
	vInstr.push_back(curInstr);
	
	vector<int> vTemp;
	
	if (curInstr.instrStr == "END" || curInstr.instrStr == "LTORG") {
			
		for (int i = 0 ; i < literal_table.size() ; i++) {
			if (literal_table[i].literal != "") {
		    	if (literal_table[i].tokenType == 7) {
		    		curLiteral.instrStr = "";
					curLiteral.loc = "";
					curLiteral.op1 = "";
					curLiteral.op2 = "";
					curLiteral.opcode = "";
					curLiteral.format = -1;
					curLiteral.error = false;
					curLiteral.n = "0", curLiteral.i = "0", curLiteral.x = "0";
					curLiteral.b = "0", curLiteral.p = "0", curLiteral.e = "0";
					curLiteral.objectCode = "";
		   	 		curLiteral.instrStr = literal_table[i].literal;
		    		curLiteral.objectCode = literal_table[i].objectCode;
		    		curLiteral.loc = pc;
		    		literal_table[i].loc = pc;
					vInstr.push_back(curLiteral);
					pc = decToHex(hexToDec(pc)+literal_table[i].objectCode.size()/2);
				} // if
		    	else {
					vTemp.push_back(i);
				} // else
			} // if
		} // for
		
		for (int index = 0; index < vTemp.size() ; index++) {
			curLiteral.instrStr = "";
			curLiteral.loc = "";
			curLiteral.op1 = "";
			curLiteral.op2 = "";
			curLiteral.opcode = "";
			curLiteral.format = -1;
			curLiteral.error = false;
			curLiteral.n = "0", curLiteral.i = "0", curLiteral.x = "0";
			curLiteral.b = "0", curLiteral.p = "0", curLiteral.e = "0";
			curLiteral.objectCode = "";
			curLiteral.instrStr = literal_table[vTemp[index]].literal;
			curLiteral.objectCode = literal_table[vTemp[index]].objectCode;
		    curLiteral.loc = pc;
		    literal_table[vTemp[index]].loc = pc;
			vInstr.push_back(curLiteral);
			pc = decToHex(hexToDec(pc)+literal_table[vTemp[index]].objectCode.size()/2);
		} // for
	} // if
} // SICXE_syntax()


void ProcessToken::SIC_checkFormatAndLoc(int index, vector<tokenType> token, instrType &curInstr, string &pc) {	
	int tempFormat = 0;
	bool tempIsSICXE = false; 
	
	findOptable(curInstr.instrStr, curInstr.opcode, tempFormat, tempIsSICXE);
	
	if (tempIsSICXE) {
		curInstr.error = true;
		return;
	} // if
	
	if (token.size() == index+1 && token[index].tokenStr == "RSUB") {  // RSUB後面不能有operand 
		pc = decToHex(hexToDec(pc)+hexToDec("3"));
	} // if
	else if (token.size() == index+2 && token[index+1].tokenType == 5) {  // {<lable>} <opcode> <symbol>
		curInstr.op1 = token[index+1].tokenStr;
		pc = decToHex(hexToDec(pc)+hexToDec("3"));
	} // else if
	else if (token.size() == index+4    	&& 	token[index+1].tokenType == 5 &&
			 token[index+2].tokenStr == "," && 	token[index+3].tokenStr == "X"  ) {  // {<lable>} <opcode> <symbol> <,> <X>
		curInstr.op1 = token[index+1].tokenStr;
		curInstr.op2 = token[index+3].tokenStr;
		curInstr.x = "1";
		pc = decToHex(hexToDec(pc)+hexToDec("3"));
	} // else if
	else 	curInstr.error = true;
} // SIC_checkFormatAndLoc()

void ProcessToken::SICXE_checkFormatAndLoc(int index, vector<tokenType> token, instrType &curInstr, string &pc) {
	int tempFormat = 0, hashValue = 0;
	bool tempIsSICXE = false; 
	
	findOptable(curInstr.instrStr, curInstr.opcode, tempFormat, tempIsSICXE);
	
	if (curInstr.format == 4 && tempFormat != 3) {
		curInstr.error = true;
		return;
	} // if
	
	if (curInstr.format != 4) {
		curInstr.format = tempFormat;
	} // if
	
	if (curInstr.format == 1) {
		if (token.size() != index+1) {
			curInstr.error = true;
			return;
		} // if
		pc = decToHex(hexToDec(pc)+hexToDec("1"));	
	} // if
	else if (curInstr.format == 2) {
		pc = decToHex(hexToDec(pc)+hexToDec("2"));
		if (token.size() == index+2 && (token[index+1].tokenType == 3 || token[index+1].tokenType == 6)) {  // {<lable>} <opcode> <r1>
			curInstr.op1 = token[index+1].tokenStr;
		} // else if
		else if (token.size() == index+4    	&& 	token[index+1].tokenType == 3 &&
			token[index+2].tokenStr == ","      && 	token[index+3].tokenType == 3  ) {  // {<lable>} <opcode> <r1> <,> <r2>
			curInstr.op1 = token[index+1].tokenStr;
			curInstr.op2 = token[index+3].tokenStr;
		} // else if
		else 	curInstr.error = true;
	} // else if
	else if (curInstr.format == 3 || curInstr.format == 4) {
		curInstr.i = "1";
		curInstr.n = "1";
		if (token.size() == index+2 && (token[index+1].tokenType == 5 || token[index+1].tokenType == 6)) {  // {<lable>} <opcode> <symbol>(address)
			curInstr.op1 = token[index+1].tokenStr;
		} // if
		else if (token.size() == index+4    	&& 	token[index+1].tokenType == 5 &&
			 token[index+2].tokenStr == "," 	&& 	token[index+3].tokenStr == "X") {  // {<lable>} <opcode> <symbol> <,> <X>
			curInstr.op1 = token[index+1].tokenStr;
			curInstr.op2 = token[index+3].tokenStr;
			curInstr.x = "1";
		} // else if
		else if (token.size() == index+3  && token[index+1].tokenStr == "#" && 
				 (token[index+2].tokenType == 5 || token[index+2].tokenType == 6) ) {  // {<lable>} <opcode> <#> <symbol>(decnum)
			curInstr.op1 = token[index+2].tokenStr;
			curInstr.i = "1";
			curInstr.n = "0";
		} // else if
		else if (token.size() == index+3   &&
			   	 token[index+1].tokenStr == "@" && token[index+2].tokenType == 5) {  // {<lable>} <opcode> <@> <symbol>
			curInstr.op1 = token[index+2].tokenStr;
			curInstr.n = "1";
			curInstr.i = "0";
		} // else if
		else if (token.size() == index+5 && token[index+1].tokenStr == "=" && token[index+2].tokenStr == "'" &&
		        (token[index+3].tokenType == 6 || token[index+3].tokenType == 7)) {  // {label} <opcode> = X''… | C''…
		    curInstr.op1 = token[index+3].tokenStr;
		    hashValue = hash(token[index+3].tokenStr, literal_table);
		    literal_table[hashValue].literal = token[index+3].tokenStr;
		    literal_table[hashValue].loc = "";
		    if (token[index+3].tokenType == 6)	{
		    	literal_table[hashValue].tokenType = 6;
				literal_table[hashValue].objectCode = token[index+3].tokenStr;
			} // if
		    else {
		    	literal_table[hashValue].tokenType = 7;
		    	literal_table[hashValue].objectCode = "";
		    	for (int i = 0 ; i < token[index+3].tokenStr.size() ; i++) {
					literal_table[hashValue].objectCode = literal_table[hashValue].objectCode + decToHex(token[index+3].tokenStr[i]);
				} // for
			} // else
		} // else if
		else if (token.size() == index+3 && token[index+1].tokenStr == "=" && token[index+2].tokenType == 6) {  // {label} <opcode> = 3277
		    curInstr.op1 = token[index+2].tokenStr;
		    hashValue = hash(token[index+2].tokenStr, literal_table);
		    literal_table[hashValue].tokenType = 6;
		    literal_table[hashValue].literal = token[index+2].tokenStr;
		    literal_table[hashValue].loc = "";
		    literal_table[hashValue].objectCode = token[index+2].tokenStr;
		} // else if
		else if ( token.size() == index+1 && token[index].tokenStr == "RSUB" ) {  // RSUB後面不能有operand 
		} // else if
		else {
			curInstr.error = true;
			return;
		} // else
		
		if (curInstr.format == 3) {
			pc = decToHex(hexToDec(pc)+hexToDec("3"));
			curInstr.e = "0";
		} // if
		else {
			pc = decToHex(hexToDec(pc)+hexToDec("4"));
			curInstr.e = "1";
		} // else
	} // else if
	else {
		curInstr.error = true;
		return;
	} // else
	
} // SIC_checkFormatAndLoc()

void ProcessToken::checkPseudo(int index, vector<tokenType> token, instrType &curInstr, string &pc) {
	
	if (token.size() == index+2 && token[index].tokenStr == "START") {  // {label} START hex_num
		curInstr.op1 = token[index+1].tokenStr;
		pc = token[index+1].tokenStr;  // hex_num
	} // if
	else if (token[index].tokenStr == "END") {  // {label} END {label}
		if ( token.size() == index+1 || 
		    (token.size() == index+2 && token[index+1].tokenType == 5)) {  // END or label END or label END label
		    	pc = pc;
		} // if
		else 	curInstr.error = true;
	} // else if
	else if (index == 1 && token[index].tokenStr == "EQU" && token.size() == index+2) {  // label EQU label | dec_num | *
		if (token[index+1].tokenType == 5 || token[index+1].tokenType == 6 || token[index+1].tokenStr == "*") {
			curInstr.op1 = token[index+1].tokenStr;
			pc = decToHex(hexToDec(pc)+hexToDec("3"));
		} // if
		else 	curInstr.error = true;
	} // else if
	else if (token[index].tokenStr == "BYTE" || token[index].tokenStr == "WORD") {  // {label} BYTE(WORD) X''… | C''… | dec_num
		if (token.size() == index+2 && token[index+1].tokenType == 6) {  // {label} BYTE(WORD) dec_num
			curInstr.op1 = token[index+1].tokenStr;
			if (token[index].tokenStr == "BYTE") {
				int num = stringToInt(token[index+1].tokenStr);
				string str = decToHex(num);
				int size = str.size();
				if (size%2 != 0) size++;
				pc = decToHex(hexToDec(pc)+(size/2));
				curInstr.objectCode = add_zero(decToHex(stringToInt(curInstr.op1)), size);
			} // if
			else {
				pc = decToHex(hexToDec(pc)+hexToDec("3"));
				curInstr.objectCode = add_zero(decToHex(stringToInt(curInstr.op1)), 6);
			} // else
		} // if
		else if (token.size() == index+4 && token[index+1].tokenStr == "'" &&
		        (token[index+2].tokenType == 6 || token[index+2].tokenType == 7)) {  // {label} BYTE(WORD) X''… | C''…
		    curInstr.op1 = token[index+2].tokenStr;
		    if (token[index].tokenStr == "BYTE" && token[index+2].tokenType == 6) {  // hex_num (X'F1')
				pc = decToHex(hexToDec(pc)+token[index+2].tokenStr.size()/2);
				curInstr.objectCode = curInstr.op1;
			} // if
			else if (token[index].tokenStr == "BYTE" && token[index+2].tokenType == 7) {
				pc = decToHex(hexToDec(pc)+token[index+2].tokenStr.size());
				for (int i = 0 ; i < curInstr.op1.size() ; i++) {
					curInstr.objectCode = curInstr.objectCode + decToHex(curInstr.op1[i]);
				} // for
			} // else if
			else if (token[index].tokenStr == "WORD" && token[index+2].tokenType == 6){
				pc = decToHex(hexToDec(pc)+hexToDec("3"));
				curInstr.objectCode = curInstr.op1;
			} // else if
			else if (token[index].tokenStr == "WORD" && token[index+2].tokenType == 7){
				pc = decToHex(hexToDec(pc)+hexToDec("3"));
				for (int i = 0 ; i < curInstr.op1.size() ; i++) {
					curInstr.objectCode = decToHex(curInstr.op1[i]);
				} // for
			} // else if
		} // else if
		else 	curInstr.error = true;
	} // else if
	else if (token[index].tokenStr == "LTORG" && token.size() == index+1) {  // {label} LTORG
		pc = decToHex(hexToDec(pc)+hexToDec("3"));
	} // else if
	else if (token[index].tokenStr == "BASE" && token.size() == index+2 && 
			(token[index+1].tokenType == 5 || token[index+1].tokenType == 6)) {  // {label} BASE dec_num | symbol
		curInstr.op1 = token[index+1].tokenStr;
		// pc不變 
	} // else if
	else if ((token[index].tokenStr == "RESB" || token[index].tokenStr == "RESW") && 
			  token.size() == index+2 && token[index+1].tokenType == 6 ) {  // {label} RESB(RESW) dec_num
		curInstr.op1 = token[index+1].tokenStr;
		if (token[index].tokenStr == "RESB") {
			int num = stringToInt(token[index+1].tokenStr);
			pc = decToHex(hexToDec(pc)+num);
		} // if
		else {
			int num = stringToInt(token[index+1].tokenStr);
			pc = decToHex(hexToDec(pc)+num*3);
		} // else
	} // else if
	else {
		curInstr.error = true;
	} // else
} // SIC_checkPseudo()

void ProcessToken::SIC_ObjectCode(instrType &curInstr) {
	string address = "";
	if (curInstr.opcode != "") {  // optable指令 
		if (curInstr.instrStr == "RSUB") {
			curInstr.objectCode = curInstr.opcode + "0000";
			
		} // if
		else {
			address = hexToBinary(findSymbolLoc(curInstr.op1));  // 16bits binary address
			address = add_zero(address, 16);   // 補0 
			address.assign(address, 1, address.size()-1);
			address = curInstr.x + address;
			curInstr.objectCode = curInstr.opcode + binaryToHex(address);
		} // else
	} // if
} // SIC_ObjectCode()

void ProcessToken::SICXE_ObjectCode(instrType &curInstr, string pc, string bReg) {
	string address = "", newOpcode = "", disp = "", r1 = "", r2 = "";
	string loc = "", literal = "";
	int disp_int = 0;
	if (curInstr.opcode != "") {  // optable指令 
		if (curInstr.format == 1) {
			curInstr.objectCode = curInstr.opcode;
		} // if
		else if (curInstr.format == 2) {
			r1 = getRegNo(curInstr.op1);
			if (r1 == "-1")	r1 = curInstr.op1;

			if (curInstr.op2 != "")	{
				r2 = getRegNo(curInstr.op2);
				curInstr.objectCode = curInstr.opcode + r1 + r2;
			} // if
			else {
				curInstr.objectCode = curInstr.opcode + r1 + "0";
			} // else

			
		} // else if
		else if (curInstr.format == 3) {
			if (curInstr.instrStr == "RSUB") {
				disp = "000";
			} // if
			else {
				loc = findSymbolLoc(curInstr.op1);
				literal = findLiteralLoc(curInstr.op1);
				if (loc != "-1") {
					curInstr.p = "1";
					curInstr.b = "0";
					
					disp_int = hexToDec(loc) - hexToDec(pc);
					if (disp_int > 4095 || disp_int < -4095) {  // 用base reg 
						curInstr.p = "0";
						curInstr.b = "1";
						disp_int = hexToDec(loc) - hexToDec(bReg);
					} // if
					
					disp = decToHex(disp_int);
					if (disp.size() > 3)	disp.assign(disp, disp.size() - 3, 3);
					disp = add_zero(disp, 3);  // 3nibbles disp
				} // if
				else if (literal != "-1") {
					curInstr.p = "1";
					curInstr.b = "0";
					
					disp_int = hexToDec(literal) - hexToDec(pc);
					if (disp_int > 4095 || disp_int < -4095) {  // 用base reg 
						curInstr.p = "0";
						curInstr.b = "1";
						disp_int = hexToDec(literal) - hexToDec(bReg);
					} // if
					
					disp = decToHex(disp_int);
					if (disp.size() > 3)	disp.assign(disp, disp.size() - 3, 3);
					disp = add_zero(disp, 3);  // 3nibbles disp
				} // else if
				else {
					curInstr.p = "0";
					curInstr.b = "0";
					disp = decToHex(stringToInt(curInstr.op1));
					if (disp.size() > 3)	disp.assign(disp, disp.size() - 3, 3);
					disp = add_zero(disp, 3);  // 3nibbles disp
				} // else
			} // else
	
			newOpcode = hexToBinary(curInstr.opcode);
			newOpcode = add_zero(newOpcode, 8);
			newOpcode.assign(newOpcode, 0, 6);
			curInstr.objectCode = binaryToHex(newOpcode + curInstr.n + curInstr.i + curInstr.x +
										      curInstr.b + curInstr.p + curInstr.e) + disp;
		} // else if
		else if (curInstr.format == 4) {
			if (curInstr.instrStr == "RSUB") {
				address = "00000";
			} // if
			else if (curInstr.instrStr == "COMP") {
				address = add_zero(curInstr.op1, 5);
			} // else if
			else if (curInstr.i == "1" && curInstr.n == "0") {
				loc = findSymbolLoc(curInstr.op1);
				literal = findLiteralLoc(curInstr.op1);
				if (loc != "-1") {
					curInstr.p = "1";
					curInstr.b = "0";
					address = loc;
				} // if
				else if (literal != "-1") {
					curInstr.p = "1";
					curInstr.b = "0";
					address = literal;
				} // else if
				else {
					curInstr.p = "0";
					curInstr.b = "0";
					address = decToHex(stringToInt(curInstr.op1));
					if (address.size() > 5)	 address.assign(address, address.size() - 5, 5);
				} // else
				address = add_zero(address, 5);  // 5nibbles address
			} // else if
			else {
				loc = findSymbolLoc(curInstr.op1);
				literal = findLiteralLoc(curInstr.op1);
				curInstr.p = "0";
				curInstr.b = "0";
				if (loc != "-1") {
					address = loc;
				} // if
				else if (literal != "-1") {
					address = literal;
				} // else if
				else {
					address = decToHex(stringToInt(curInstr.op1));
					if (address.size() > 5)	 address.assign(address, address.size() - 5, 5);
				} // else
				address = add_zero(address, 5);   // 補0 
			} // else
			
			newOpcode = hexToBinary(curInstr.opcode);
			newOpcode = add_zero(newOpcode, 8);
			newOpcode.assign(newOpcode, 0, 6);
			curInstr.objectCode = binaryToHex(newOpcode + curInstr.n + curInstr.i + curInstr.x + 
								  			  curInstr.b + curInstr.p + curInstr.e) + address;
		} // else if
		
	} // if
} // SICXE_ObjectCode()

string ProcessToken::findSymbolLoc(string str) {
	for(int i = 0 ; i < symbol_table.size() ; i++) {
		if (symbol_table[i].symbol == str) {
			return symbol_table[i].loc;
		} // if	
	} // for
	return "-1";
} // findSymbolLoc()

string ProcessToken::findLiteralLoc(string str) {
	for(int i = 0 ; i < literal_table.size() ; i++) {
		if (literal_table[i].literal == str) {
			return literal_table[i].loc;
		} // if	
	} // for
	return "-1";
} // findSymbolLoc()

void ProcessToken::findOptable(string str, string &opcode, int &format, bool &isSICXE) {
	for(int i = 0 ; i < OPTABLE_SIZE ; i++) {
		if (gOptable[i].instruction == str) {
			opcode = gOptable[i].opcode;
			format = gOptable[i].format;
			isSICXE = gOptable[i].isSICXE;
			return;
		} // if	
	} // for
} // findOptable()

string ProcessToken::getRegNo(string str) {
	for(int i = 0 ; i < REGTABLE_SIZE ; i++) {
		if (gRegTable[i].reg == str) {
			return gRegTable[i].regNo;
		} // if	
	} // for
	return "-1";
} // getRegNo()


void ProcessToken::output() {
	vector<string> :: iterator iter_vSource = vSource.begin();
	vector<lineType> :: iterator iter_vLine = vLine.begin();
	fstream fp;
	fp.open(fileName+"_output.txt", ios::out);

	for (; fp.is_open() && iter_vLine != vLine.end() && iter_vSource != vSource.end() ; iter_vLine++, iter_vSource++) {
		cout << *iter_vSource << "\n";
		fp << *iter_vSource << "\n";
		for (int i = 0; i < iter_vLine->token.size() ; i++) {
			if (iter_vLine->token[i].tokenType == 8) {  // comment
				//cout << " " << iter_vLine->token[i].tokenStr;
				//fp << " " << iter_vLine->token[i].tokenStr;
			} // if
			else {
				cout << "(" << iter_vLine->token[i].tokenType << "," << iter_vLine->token[i].tokenNo << ")";
				fp << "(" << iter_vLine->token[i].tokenType << "," << iter_vLine->token[i].tokenNo << ")";
			} // else
		} // for
		cout << "\n";
		fp << "\n";
	} // for
} // end output()

void ProcessToken::output_2() {
	vector<string> :: iterator iter_vSource = vSource.begin();
	vector<instrType> :: iterator iter_vInstr = vInstr.begin();
	fstream fpOut;
	fpOut.open(fileName+"_output_2.txt", ios::out);
	cout << "Line" << "\t" << "Loc" << "\t" << "Source" << "\t" << "statement" << "\t" << "Object code" << endl << endl;
	fpOut << "Line" << "\t" << "Loc" << "\t" << "Source" << "\t" << "statement" << "\t" << "Object code" << endl << endl;
	for ( int i = 1; fpOut.is_open() && iter_vInstr != vInstr.end(); iter_vInstr++, i++) {
		  	
		fpOut << i*5 << "\t";
		cout << i*5 << "\t";
		if (iter_vInstr->instrStr != "END" && iter_vInstr->instrStr != "BASE" && iter_vInstr->instrStr != ".") {
			cout << setfill('0') << setw(4) << iter_vInstr->loc;
			fpOut << setfill('0') << setw(4) << iter_vInstr->loc;
		} // if
		
		cout << "\t";
		fpOut << "\t";

		if (iter_vInstr->format != -1 && iter_vSource < vSource.end()) {
			cout << *iter_vSource;
			fpOut <<  *iter_vSource;
			iter_vSource++;
		} // if
		
		cout << "\t";
		fpOut << "\t";
		
		if 	(iter_vInstr->error) {
			cout << "error";
			fpOut << "error";
		} // if
		else if (iter_vInstr->objectCode != "")	{
			cout << iter_vInstr->objectCode;
			fpOut << iter_vInstr->objectCode;
		} // else if
		cout << endl;
		fpOut << endl;
	} // for
	fpOut.close();
} // end output()

void ProcessToken::getToken(string& str, tokenType& temp, string type) {
	int num = 0;
	string lowerStr = str, upperStr = str;
	
	transform(str.begin(), str.end(), lowerStr.begin(), ::tolower); 
	transform(str.begin(), str.end(), upperStr.begin(), ::toupper);
	temp.tokenStr = upperStr;  // 一律用大寫存字串 
	
	if (type == "comment") {
		temp.tokenType = 8;
		temp.tokenNo = -1;
	} // if
	else if (type == "string") {  // string (C'EOF')
		temp.tokenType = 7;
		num = hash(str, table7);
	} // else if
	else if (type == "hex" || isAllNumber(str)) {  // integer
		temp.tokenType = 6;
		num = hash(str, table6);
	} // else if
	else if (type == "delimiter" && findTable(str, num, table4)) {
		temp.tokenType = 4;
	} // else if
	else if (findTable(lowerStr, num, table1)) {
		temp.tokenType = 1;
	} // else if
	else if (findTable(upperStr, num, table2)) {
		temp.tokenType = 2;
	} // else if
	else if (findTable(upperStr, num, table3)) {
		temp.tokenType = 3;
	} // else if
	else {  // 5 or trash
		temp.tokenType = 5;
		num = hash(str, table5);
	} // else
	temp.tokenNo = num ;
	str = "";
} // end getToken()

string ProcessToken::add_zero(string str, int needSize) {
	// 補前面的0, 差幾個就補幾個, hex和binary都行 
	string newStr = "";
	for (int i = str.size() ; i < needSize ; i++) {
		newStr = newStr + "0";
	} // for
	newStr = newStr + str;  // 串接上去 
	return newStr;
} // add_zero()

int ProcessToken::hash(string str, vector<string>& vTable) {
	int num = 0, value = 0;    
	for (int i = 0 ; i < str.size() ; i++) {
		num = num + (int)str.at(i);
	} // end for 
	value = num % 100;			// 取得hashValue

	do {
		if (vTable[value].size() == 0) {  // empty space
			vTable[value] = str;
			break;
		} // end if
		else if (vTable[value] == str) {  // find the same item
			break;
		} // else if
		value = (value+1) % 100;
	} while (true);
	
	return value;
} // end hash()

int ProcessToken::hash(string str, vector<symbolType>& vTable) {
	int num = 0, value = 0;    
	for (int i = 0 ; i < str.size() ; i++) {
		num = num + (int)str.at(i);
	} // end for 
	value = num % 100;			// 取得hashValue

	do {
		if (vTable[value].symbol.size() == 0) {  // empty space
			vTable[value].symbol = str;
			break;
		} // end if
		else if (vTable[value].symbol == str) {  // find the same item
			break;
		} // else if
		value = (value+1) % 100;
	} while (true);
	
	return value;
} // end hash()

int ProcessToken::hash(string str, vector<literalType>& vTable) {
	int num = 0, value = 0;    
	for (int i = 0 ; i < str.size() ; i++) {
		num = num + (int)str.at(i);
	} // end for 
	value = num % 100;			// 取得hashValue

	do {
		if (vTable[value].literal.size() == 0) {  // empty space
			vTable[value].literal = str;
			break;
		} // end if
		else if (vTable[value].literal == str) {  // find the same item
			break;
		} // else if
		value = (value+1) % 100;
	} while (true);
	
	return value;
} // end hash()

bool ProcessToken::findTable(string str, int &num, vector<string> vTable) {
	for (int i = 1 ; i < vTable.size() ; i++) {
		if (str == vTable.at(i)) {
			num = i;
			return true;
		} // end if
	} // end for
	return false;
} // end findTable()

bool ProcessToken::isWhiteSpace(char ch) {
	if(ch == ' ' || ch == '\t' || ch == '\n')	return true;
	else										return false;
} // end isWhiteSpace()


bool ProcessToken::isEnter(char ch) {
	if(ch == '\n')	return true;
	else			return false;
} // end isEnter()

bool ProcessToken::isAllNumber(string str) {
	if (stringToInt(str) != -1)		return true;
	else							return false;
} // end isAllNumber()

int ProcessToken::stringToInt(string str) {  //轉型 
  	stringstream ss ;
  	int num = 0 ;
  	ss.str(str) ;
  	ss >> num ;
  	if ( ss.fail() ) return -1 ;  // 轉型失敗 
  	else 			 return num ;
} // end stringToInt()

string ProcessToken::decToHex(int num) {
	stringstream ss;
	string str="", upperStr="";
	ss << std::uppercase << hex << num;
	ss >> str;
	//transform(str.begin(), str.end(), upperStr.begin(), ::toupper);
	return str;
} // decToHex()

int ProcessToken::hexToDec(string str) {
	int num = 0;
	for(int i = 0 ; i < str.size()-1 ; i++) {
		if 		(str[i] == 'A')		num = (num + 10)*16;
		else if (str[i] == 'B')		num = (num + 11)*16;
		else if (str[i] == 'C')		num = (num + 12)*16;
		else if (str[i] == 'D')		num = (num + 13)*16;
		else if (str[i] == 'E')		num = (num + 14)*16;
		else if (str[i] == 'F')		num = (num + 15)*16;
		else 						num = (num + str[i] - '0')*16;
	} // for
	
	if (str.size()-1 >= 0) {
		if 		(str[str.size()-1] == 'A')		num = num + 10;
		else if (str[str.size()-1] == 'B')		num = num + 11;
		else if (str[str.size()-1] == 'C')		num = num + 12;
		else if (str[str.size()-1] == 'D')		num = num + 13;
		else if (str[str.size()-1] == 'E')		num = num + 14;
		else if (str[str.size()-1] == 'F')		num = num + 15;
		else 									num = num + str[str.size()-1] - '0';
	} // if
	
	return num;
} // hexToDec()

string ProcessToken::hexToBinary(string str) {
	string binary = "";
	for(int i = 0 ; i < str.size() ; i++) {
		if 		(str[i] == '0')		binary = binary + "0000";
		else if (str[i] == '1')		binary = binary + "0001";
		else if (str[i] == '2')		binary = binary + "0010";
		else if (str[i] == '3')		binary = binary + "0011";
		else if (str[i] == '4')		binary = binary + "0100";
		else if (str[i] == '5')		binary = binary + "0101";
		else if (str[i] == '6')		binary = binary + "0110";
		else if (str[i] == '7')		binary = binary + "0111";
		else if (str[i] == '8')		binary = binary + "1000";
		else if (str[i] == '9')		binary = binary + "1001";
		else if (str[i] == 'A')		binary = binary + "1010";
		else if (str[i] == 'B')		binary = binary + "1011";
		else if (str[i] == 'C')		binary = binary + "1100";
		else if (str[i] == 'D')		binary = binary + "1101";
		else if (str[i] == 'E')		binary = binary + "1110";
		else if (str[i] == 'F')		binary = binary + "1111";
		else 						binary = binary + "0000";
	} // for

	return binary;
} // hexToBinary()

string ProcessToken::binaryToHex(string str) {
	string hex = "";
	string binary = "";
	//cout <<str<<endl;
	for(int i = 0 ; i < str.size() ; i=i+4) {
		binary.assign(str, i, 4);
		if 		(binary == "0000")		hex = hex+"0";
		else if (binary == "0001")		hex = hex+"1";
		else if (binary == "0010")		hex = hex+"2";
		else if (binary == "0011")		hex = hex+"3";
		else if (binary == "0100")		hex = hex+"4";
		else if (binary == "0101")		hex = hex+"5";
		else if (binary == "0110")		hex = hex+"6";
		else if (binary == "0111")		hex = hex+"7";
		else if (binary == "1000")		hex = hex+"8";
		else if (binary == "1001")		hex = hex+"9";
		else if (binary == "1010")		hex = hex+"A";
		else if (binary == "1011")		hex = hex+"B";
		else if (binary == "1100")		hex = hex+"C";
		else if (binary == "1101")		hex = hex+"D";
		else if (binary == "1110")		hex = hex+"E";
		else if (binary == "1111")		hex = hex+"F";
		else 							hex = hex+"0";
	} // for

	return hex;
} // binaryToHex()

bool ProcessToken::isDelimiter(char ch){
	if(ch == ',' || ch == '+' || ch == '-' || ch == '*' ||  
	   ch == '/' || ch == ':' || ch == '?' || ch == '=' ||
	   ch == '#' || ch == '@' || ch == ';' || ch == '.' || ch == '\'')		return true;
	else																	return false;
} // end isDelimiter()

int main() {
	ProcessToken pToken;
	string str = "";
	
	
	
	while(true) {
		string str;
		cout << endl << "******** SIC & SIC/XE assembler *********" << endl;
		cout << "*****************************************" << endl;
		cout << "***     0. Quit                       ***" << endl;
		cout << "***     1. SIC assembler              ***" << endl;
		cout << "***     2. SIC/XE assembler           ***" << endl;
		cout << "*****************************************" << endl;
		cout << "Input number : ";

		getline(cin, str, '\n');
		
		if (str == "0")	break;
		else if (str == "1") {
			pToken.init();
			pToken.open_file();
			pToken.SIC_pass1();
			pToken.SIC_pass2();
			pToken.close_file();
			pToken.output_2();
		} // else if
		else if (str == "2") {
			pToken.init();
			pToken.open_file();
			pToken.SICXE_pass1();
			pToken.SICXE_pass2();
			pToken.close_file();
			pToken.output_2();
		} // else if
		
		cin.ignore();

	} // while
} // main
