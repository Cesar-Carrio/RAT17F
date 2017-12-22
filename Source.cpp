#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <assert.h>
#include <sstream>
#include <vector>
#include <stack>

using namespace std;

regex r_letter("[a-zA-Z]+");
regex r_digit("[0-9]+");

regex r_op("(@|=|/=|>|:=|<|=>|<=|\\+|\\*|/|-)"); // operator
regex r_sep("(\\(|\\)|,|;|:|%%|\\{|\\}|\\[|\\])"); // separator
regex r_key("(boolean|else|false|fi|floating|if|integer|read|return|true|while|write)"); // keyword

vector<string> tokenVector;
vector<string> lexemeVector;
vector<string> newLineVectors;

typedef int fsm_state;
typedef char fsm_input;

//to keep track of line numbers, it's used in lexer()
int lineCount = 1;
int vectorLineCount = 1;
int vectorMarker = 0;


struct instructionTable
{
    int instr_addr[500];
    string op[500];
    int opernd[500];
};
struct symbolTable
{
    string identifier[50];
    int memory_location[50];
    string type[50];
    int numberOfIdentifiers = 0;
};


stack<int> S;
instructionTable instructionT;
symbolTable symbolT;

int addr;
string save;
bool passThru = false;

int Memory_address = 9999; // will start at 10000 when id added to table
int instructionAddress = 1;

void gen_instr(string, int, instructionTable&, int&);
int get_address(string, symbolTable);
void back_patch(int, instructionTable&);

void insertIDSymbolTable(string, int&, string, symbolTable&);
bool isIDinSymbolTable(string, symbolTable);
void printSymbolTable(symbolTable);
bool IDusedWODeclaration(string, symbolTable);
bool IDdeclaredTwice(string, symbolTable);
bool typesMatch(string, string, symbolTable);

void printInstructionTable(instructionTable);
void printSymbolTable(symbolTable);


void gen_instr(string o, int oper, instructionTable &t, int &instr_address)
{
    
    t.instr_addr[instr_address] = instr_address;
    t.op[instr_address] = o;
    t.opernd[instr_address] = oper;
    instr_address++;
}

void back_patch(int jump_addr, instructionTable &t)
{
    addr = S.top();
    S.pop();
    t.opernd[addr] = jump_addr;
}

int get_address(string id, symbolTable sT)
{
    bool found = false;
    int i = 0, addr = 0;
    while (!found && i < sT.numberOfIdentifiers)
    {
        if (id == sT.identifier[i])
        {
            found = true;
            addr = sT.memory_location[i];
        }
        else
        {
            i++;
        }
    }
    return addr;
}

void insertIDSymbolTable(string id, int &mem, string ty, symbolTable &sT)
{
    
    
    mem++;
    sT.identifier[sT.numberOfIdentifiers] = id;
    sT.memory_location[sT.numberOfIdentifiers] = mem;
    sT.type[sT.numberOfIdentifiers] = ty;
    sT.numberOfIdentifiers++;
    
    
}

bool isIDinSymbolTable(string id, symbolTable sT)
{
    int i = 0;
    bool found = false;
    while (!found && i < sT.numberOfIdentifiers)
    {
        if (id == sT.identifier[i])
        {
            found = true;
        }
        else
        {
            i++;
        }
    }
    return found;
}

void printInstructionTable(instructionTable iT)
{
    ofstream ostream("instructionTable.txt");
    ostream << "Instr Address\t\tOp\t\toprnd\n";
    for (int i = 1; i < instructionAddress; i++)
    {
        ostream << iT.instr_addr[i] << "\t\t\t\t" << iT.op[i] << "\t\t\t" << iT.opernd[i] << endl;
    }
    
    
    ostream.close();
}

void printSymbolTable(symbolTable sT)
{
    ofstream ostream("symbolTable.txt");
    ostream << "Identifer\t\tMemory Location\t\tType\n";
    for (int i = 0; i < sT.numberOfIdentifiers; i++)
    {
        ostream << sT.identifier[i] << "\t\t\t\t" << sT.memory_location[i] << "\t\t\t\t" << sT.type[i] << endl;
    }
    
    ostream << "\nListing of Identifiers:\n";
    for (int i = 0; i < sT.numberOfIdentifiers; i++)
    {
        ostream << "->" << sT.identifier[i] << '\n';
    }
    ostream.close();
}


bool IDusedWODeclaration(string id, symbolTable sT)
{
    // check if ID is in symbolTable
    if (isIDinSymbolTable(id, sT))
        return false; // ID found in symbol table; ID declared
    else
        return true; // ID not found in symbol table; ID used without declaration
}

bool IDdeclaredTwice(string id, symbolTable sT)
{
    if (isIDinSymbolTable(id, sT))
        return true; // ID already in symbolTable; ID will be declared twice
    else
        return false; // ID is not in symbolTable; will not be declared twice
}

bool typesMatch(string t, string ty, symbolTable sT)
{
    bool found = false;
    int i = 0;
    
    if (sT.numberOfIdentifiers == 0)
    {
        return !found;
    }
    
    
    while (!found && i < sT.numberOfIdentifiers)
    {
        if (t == sT.identifier[i])
        {
            found = true;
            if (ty == sT.type[i])
                return found;
        }
        else
            i++;
    }
    return found;
}


bool printSwitchOn = true;
bool recognize(string str);
bool is_final_state(fsm_state state);
fsm_state get_start_state(void);
fsm_state move(fsm_state state, fsm_input input);

void lexer();
void printVector();

//production functions
bool Rat17F();
bool optFuncDef();
bool funcDef();
bool func();
bool optParamList();
bool paramList();
bool parameter();
bool qualifier();
bool body();
bool optDecList();
bool decList();
bool declaration();
bool IDS();
bool statementList();
bool statement();
bool compound();
bool assign();//
bool if_nonTerm();
bool return_nonTerm();
bool Write();
bool Read();
bool While();
bool condition();
bool relop();
bool expr();
bool expr_Prime();
bool term();
bool term_Prime();
bool factor();//
bool primary();//
bool empty();
bool funcDefPrime();
bool paramListPrime();
bool decListPrime();
bool IDS_Prime();
bool statementListPrime();
bool if_nonTerm_Prime();
bool return_nonTerm_prime();
bool primary_prime();



ofstream outputFile("output.txt");


int main()
{
    if (passThru == false) {
        lexer();
        printVector();
        Rat17F();
        
    }
    else
    {
        system("CLS");
        printInstructionTable(instructionT);
        printSymbolTable(symbolT);
        outputFile.close();
    }
    
    
    return 0;
    
    
}


void lexer() {
    
    ifstream fileStream;
    fileStream.open("Rat17F.txt");
    
    if (fileStream.is_open())
        cout << "open\n";
    else
        cout << "unable\n";
    
    cout << "token\t\tlexeme\n";
    cout << "------\t\t------\n";
    
    //for finding keywords, operators, separators.
    string kWord;
    string ops;
    string seps;
    string fsm_string;
    
    //Using count to double check correct amount of separators, operators, keywords, integers, floats, Indentifiers
    int count = 0;
    
    
    
    //Using c to get character by character from file
    char c;
    bool yesDec = false;
    
    while (fileStream.get(c))
    {
        if (c == '\n')
        {
            lineCount++;
            string newLine(1, c);
            newLineVectors.push_back(newLine);
            newLine.clear();
            vectorLineCount++;
        }
        
        //converting to string for regex_match
        string temp_OneChar_String(1, c);
        
        if (regex_match(temp_OneChar_String, r_letter)) {
            //appending chars to string to make word
            kWord.append(temp_OneChar_String);
        }
        else if (regex_match(temp_OneChar_String, r_letter) == false) {
            //if it isn't a letter then end of word
            //checking to see if kword is a keyword
            if (regex_match(kWord, r_key)) {
                count++;
                cout << "Keyword\t\t" << kWord << '\n';
                
                tokenVector.push_back("Keyword");
                lexemeVector.push_back(kWord);
                
                fsm_string.clear();
            }
            kWord.clear();
        }
        //=======================================================
        
        
        //================================================================
        //Fsm for Identifier
        if (regex_match(temp_OneChar_String, r_letter) == true && fsm_string.empty() == true)
        {
            fsm_string.append(temp_OneChar_String);
            continue;
        }
        else if ((regex_match(temp_OneChar_String, r_letter) || temp_OneChar_String == "#") && fsm_string.empty() == false)
        {
            fsm_string.append(temp_OneChar_String);
            continue;
        }
        else if (regex_match(temp_OneChar_String, r_letter) == false && temp_OneChar_String != "#" && regex_match(fsm_string, r_digit) == false && regex_match(temp_OneChar_String, r_digit) == false && yesDec == false)
        {
            
            if (regex_match(fsm_string, r_key) == false)
            {
                if (recognize(fsm_string) == true)            /*This inserts the string into the fsm to determine if its valid*/
                {
                    count++;
                    cout << "Identifier\t" << fsm_string << endl;
                    
                    tokenVector.push_back("Identifier");
                    lexemeVector.push_back(fsm_string);
                    
                    fsm_string.clear();
                }
                else if (recognize(fsm_string) == false && fsm_string != "")
                {
                    cout << "ERROR - Invalid Identifier: " << "(" << fsm_string << ")" << "\tLine Number: " << "(" << lineCount << ")" << '\n';
                    outputFile << "ERROR - Invalid floating Number: " << "(" << fsm_string << ")" << "\tLine Number: " << "(" << lineCount << ")" << '\n';
                    exit(EXIT_SUCCESS);
                    
                }
            }
        }
        
        //=======================================================
        
        
        //================================================================
        //Fsm for integers and reals
        if (regex_match(temp_OneChar_String, r_digit) == true && fsm_string.empty() == true)
        {
            fsm_string.clear();
            fsm_string.append(temp_OneChar_String);
        }
        else if (regex_match(temp_OneChar_String, r_digit) == true && fsm_string.empty() == false)
        {
            fsm_string.append(temp_OneChar_String);
            continue;
            
        }
        else if (temp_OneChar_String == ".")
        {
            yesDec = true;
            fsm_string.append(temp_OneChar_String);
            continue;
        }
        else if (regex_match(temp_OneChar_String, r_digit) == false && fsm_string.empty() == false)
        {
            int decimalCount = 0;
            bool decNum = false;
            for (unsigned i = 0; i < fsm_string.length(); ++i)
            {
                
                if (fsm_string.at(i) == '.')
                {
                    decNum = true;
                    decimalCount++;
                }
            }
            
            if (decNum == true)
            {
                if (recognize(fsm_string) == true)
                {
                    count++;
                    cout << "floating\t\t" << fsm_string << endl;
                    
                    tokenVector.push_back("floating");
                    lexemeVector.push_back(fsm_string);
                    
                    yesDec = false;
                    fsm_string.clear();
                }
                else if (decimalCount > 1)
                {
                    cout << "ERROR - Invalid floating Number: " << "(" << fsm_string << ")" << "\tLine Number: " << "(" << lineCount << ")" << '\n';
                    outputFile << "ERROR - Invalid floating Number: " << "(" << fsm_string << ")" << "\tLine Number: " << "(" << lineCount << ")" << '\n';
                    exit(EXIT_SUCCESS);
                }
                
            }
            else if (decNum == false)
            {
                if (recognize(fsm_string) == true)
                {
                    count++;
                    
                    cout << "Integer\t\t" << fsm_string << endl;
                    
                    tokenVector.push_back("Integer");
                    lexemeVector.push_back(fsm_string);
                    
                    fsm_string.clear();
                }
                
            }
            
            
            
        }
        
        //=======================================================
        //checking the colon to see if it's apart of separator or operator
        if (c == ':'  && seps.empty() == true) {
            seps.append(temp_OneChar_String);
            continue;
            
        }
        else if (c == '=' && seps.empty() == false) {
            seps.append(temp_OneChar_String);
            ops = seps;
            count++;
            cout << "Operator\t" << ops << '\n';
            
            tokenVector.push_back("Operator");
            lexemeVector.push_back(ops);
            
            
            ops.clear();
            seps.clear();
            continue;
        }
        else if (seps.length() == 1 && (c != '=' && c != '%') && regex_match(seps, r_op) == false)
        {    //outputting single colon
            count++;
            cout << "Separator\t" << seps << endl;
            
            tokenVector.push_back("Separator");
            lexemeVector.push_back(seps);
            
            seps.clear();
            ops.clear();
            continue;
        }
        //======================================================
        //prints double char operators
        if ((c == '<' || c == '>' || c == '/') && ops.empty() == true)
        {
            ops.append(temp_OneChar_String);
            continue;
        }
        else if (c == '=' && ops.empty() == false)
        {
            count++;
            ops.append(temp_OneChar_String);
            cout << "Operator\t" << ops << endl;
            
            tokenVector.push_back("Operator");
            lexemeVector.push_back(ops);
            
            ops.clear();
            continue;
        }
        //=======================================================
        //print out single operators
        if ((c != ':') && (regex_match(ops, r_op)) && (ops.length() == 1))
        {
            count++;
            cout << "Operator\t" << ops << endl;
            
            tokenVector.push_back("Operator");
            lexemeVector.push_back(ops);
            
            ops.clear();
            continue;
        }
        else if (regex_match(temp_OneChar_String, r_op))
        {
            count++;
            cout << "Operator\t" << temp_OneChar_String << " " << ops << endl;
            
            tokenVector.push_back("Operator");
            lexemeVector.push_back(temp_OneChar_String + ops);
            
            ops.clear();
            continue;
        }
        
        //=======================================================
        //prints out all other separators except :
        if (c != ':' && regex_match(temp_OneChar_String, r_sep))
        {
            count++;
            cout << "Separator\t" << temp_OneChar_String << endl;
            
            tokenVector.push_back("Separator");
            lexemeVector.push_back(temp_OneChar_String);
            
            temp_OneChar_String.clear();
            
        }
        
        
        
        //=========================================================
        //prints double %%
        if (c == '%' && seps.empty() == true)
        {
            seps.append(temp_OneChar_String);
            continue;
        }
        else if (c == '%' && seps.empty() == false)
        {
            count++;
            seps.append(temp_OneChar_String);
            cout << "Separator\t" << seps << endl;
            
            tokenVector.push_back("Separator");
            lexemeVector.push_back(seps);
            
            seps.clear();
            continue;
        }
        
        
    }
    
    
    fileStream.close();
};

//================================================================
bool is_final_state(fsm_state state)
{
    if (state == 2 || state == 3 || state == 4 || state == 5 || state == 7 || state == 8)
    {
        return true;
    }
    else {
        return false;
    }
};


//================================================================
fsm_state get_start_state(void)
{
    return 1;
};

//================================================================

fsm_state move(fsm_state state, fsm_input input)
{
    
    string temp_string;
    temp_string = input;
    
    
    
    if (temp_string != "#" && temp_string != "." && regex_match(temp_string, r_letter) == true && regex_match(temp_string, r_digit) == true)
    {
        assert(1);
    }
    
    
    
    switch (state)
    {
        case 1:
            if (regex_match(temp_string, r_letter))
            {
                return 2;
            }
            else if (regex_match(temp_string, r_digit))
            {
                return 3;
            }
            break;
        case 2:
            if (regex_match(temp_string, r_letter))
            {
                return 4;
            }
            else if (temp_string == "#")
            {
                return 5;
            }
            break;
        case 3:
            if (regex_match(temp_string, r_digit))
            {
                return 3;
            }
            else if (temp_string == ".")
            {
                return 6;
            }
            break;
        case 4:
            if (regex_match(temp_string, r_letter))
            {
                return 4;
            }
            else if (temp_string == "#")
            {
                return 5;
            }
            break;
        case 5:
            if (regex_match(temp_string, r_letter))
            {
                return 7;
            }
            break;
        case 6:
            if (regex_match(temp_string, r_digit))
            {
                return 8;
            }
            break;
        case 7:
            if (regex_match(temp_string, r_letter))
            {
                return 4;
            }
            else if (temp_string == "#")
            {
                return 5;
            }
            break;
        case 8:
            if (regex_match(temp_string, r_digit))
            {
                return 8;
            }
            break;
        default:
            assert(1);
    }
    
};



bool recognize(string str)
{
    if (str == "")
    {
        return false;
    }
    fsm_state state = get_start_state();
    string::const_iterator i = str.begin();
    fsm_input input = *i;
    while (i != str.end())
    {
        state = move(state, *i);
        ++i;
    }
    
    if (is_final_state(state))
    {
        return true;
    }
    else {
        return false;
    }
    
};


void printVector() {
    
    cout << "\n\n\n\n";
    
    
    vector<string>::iterator lexemeit = lexemeVector.begin();
    
    for (vector<string>::iterator it = tokenVector.begin(); it != tokenVector.end(); ++it)
    {
        if (*lexemeit == "\n")
        {
            ++lexemeit;
            continue;
        }
        cout << *it;
        cout << "\t\t";
        cout << *lexemeit << '\n';
        ++lexemeit;
    }
    
    cout << "Line count in vector: " << vectorLineCount << "\n\n";
}


//New productions.=========================================================
//Need to add the error messages now
//
bool Rat17F()
{
    
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<RAT17F>-><OFD>%%<ODL><SL>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<RAT17F>-><OFD>%%<ODL><SL>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool result = true;
    
    if (optFuncDef())
    {
        if (lexemeVector[vectorMarker] == "%%")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (optDecList())
            {
                if (statementList())
                {
                    
                }
                else
                {
                    result = false;
                }
            }
            else
            {
                result = false;
            }
        }
        else
        {
            puts("Error - Expected a %%\n");
            outputFile << "Error - Expected a %%\n";
            result = false;
            exit(EXIT_SUCCESS);
        }
    }
    
    return result;
}

bool optFuncDef()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<OFD>-><FD>|<Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<OFD>-><FD>|<Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool result = true;
    
    if (funcDef())
    {
        
    }
    else
    {
        empty();
    }
    
    return result;
}

bool funcDef()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<FD>-><F><FDPrime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<FD>-><F><FDPrime>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (func())
    {
        if (funcDefPrime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool funcDefPrime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<FDPrime>-><F>|<Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<FDPrime>-><F>|<Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (func())
    {
        
    }
    else
    {
        empty();
    }
    
    return res;
}

bool func()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<F> -> @ <Identifier> (<OPL>) <ODL> <body>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<F> -> @ <Identifier> (<OPL>) <ODL> <body>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "@")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (tokenVector[vectorMarker] == "Identifier")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            
            
            insertIDSymbolTable(lexemeVector[vectorMarker], Memory_address, tokenVector[vectorMarker], symbolT);
            
            vectorMarker++;
            if (lexemeVector[vectorMarker] == "(")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
                if (optParamList())
                {
                    if (lexemeVector[vectorMarker] == ")")
                    {
                        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        vectorMarker++;
                        if (optDecList())
                        {
                            if (body())
                            {
                                
                            }
                            else
                            {
                                res = false;
                                return res;
                            }
                        }
                        else
                        {
                            res = true;
                        }
                    }
                    else
                    {
                        puts("Error - Expected an )\n");
                        outputFile << "Error - Expected an )\n";
                        res = false;
                        exit(EXIT_SUCCESS);
                    }
                }
                else
                {
                    res = true;
                }
            }
            else
            {
                puts("Error - Expected a (\n");
                outputFile << "Error - Expected a (\n";
                res = false;
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            puts("Error - Expected an Identifier\n");
            outputFile << "Error - Expected an Identifier\n";
            res = false;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool optParamList()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<OPL> -> <PL> | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<OPL> -> <PL> | <Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (paramList())
    {
        
    }
    else {
        empty();
    }
    
    return res;
}

bool paramList()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<PL> -> <P><PLPrime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<PL> -> <P><PLPrime>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (parameter())
    {
        if (paramListPrime())
        {
            
        }
        else
        {
            res = false;
            return res;
        }
    }
    else
    {
        res = false;
        return res;
    }
    
    return res;
}

bool paramListPrime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<PLPrime> -> ,<PL> | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<PLPrime> -> ,<PL> | <Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == ",")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (paramList())
        {
            
        }
        else
        {
            res = false;
            return res;
        }
    }
    else
    {
        empty();
        return res;
    }
    
    return res;
}

bool parameter()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<P> -> <IDS>:<Q>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<P> -> <IDS>:<Q>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    bool res = true;
    
    if (IDS())
    {
        if (lexemeVector[vectorMarker] == ":")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (qualifier())
            {
                
            }
            else
            {
                res = false;
                return res;
            }
        }
        else
        {
            puts("Error - Expected an :\n");
            outputFile << "Error - Expected an :\n";
            res = false;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
        return res;
    }
    
    return res;
}

bool qualifier()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Q> -> integer | boolean | floating\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Q> -> integer | boolean | floating\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "integer")
    {
        save = lexemeVector[vectorMarker];
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        
        if (isIDinSymbolTable(lexemeVector[vectorMarker], symbolT))
        {
            ofstream ostream("symbolTable.txt");
            ostream << "\nERROR - Cannot declare an Identifier (" << lexemeVector[vectorMarker] << ") twice!\n";
            ostream.close();
            exit(EXIT_FAILURE);
        }
    }
    else if (lexemeVector[vectorMarker] == "boolean")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (isIDinSymbolTable(lexemeVector[vectorMarker], symbolT))
        {
            ofstream ostream("symbolTable.txt");
            ostream << "\nERROR - Cannot declare an Identifier (" << lexemeVector[vectorMarker] << ") twice!\n";
            ostream.close();
            exit(EXIT_FAILURE);
        }
    }
    else if (tokenVector[vectorMarker] == "floating")
    {
        save = lexemeVector[vectorMarker];
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool body()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Body> -> { <SL> }\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Body> -> { <SL> }\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "{")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (statementList())
        {
            if (lexemeVector[vectorMarker] == "}")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
            }
            else
            {
                puts("Error - Expected an }\n");
                outputFile << "Error - Expected an }\n";
                res = false;
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            res = false;
            return res;
        }
    }
    else
    {
        puts("Error - Expected an {\n");
        outputFile << "Error - Expected an {\n";
        res = false;
        exit(EXIT_SUCCESS);
    }
    
    return res;
}

bool optDecList()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<ODL>-> <DL> | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<ODL>-> <DL> | <Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (decList())
    {
        
    }
    else
    {
        empty();
    }
    
    return res;
    
}

bool decList()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<DL> -> <D> ; <DLPrime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<DL> -> <D> ; <DLPrime>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (declaration())
    {
        if (lexemeVector[vectorMarker] == ";")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (decListPrime())
            {
                
            }
            else
            {
                res = false;
                return res;
            }
        }
        else
        {
            puts("Error - Expected an ;\n");
            outputFile << "Error - Expected an ;\n";
            res = false;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
        return res;
    }
    return res;
}

bool decListPrime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<DLP> -> <DL> | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<DLP> -> <DL> | <Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (decList())
    {
        
    }
    else
    {
        empty();
    }
    return res;
}

bool declaration()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<D> -> <Q><IDS>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<D> -> <Q><IDS>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (qualifier())
    {
        if (IDS())
        {
            
        }
        else
        {
            res = false;
            return false;
        }
    }
    else
    {
        res = false;
        return res;
    }
    
    return res;
}

bool IDS()
{
    string strType = save;
    
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<IDS> -> <Identifier><IDSPrime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<IDS> -> <Identifier><IDSPrime>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (tokenVector[vectorMarker] == "Identifier")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        if ("integer" == strType)
        {
            
            insertIDSymbolTable(lexemeVector[vectorMarker], Memory_address, strType, symbolT);
            strType.clear();
        }
        else if ("floating" == strType)
        {
            cout << "No floating numbers allowed!";
        }
        
        
        
        vectorMarker++;
        if (IDS_Prime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        puts("Error - Expected an Identifier\n");
        outputFile << "Error - Expected an Indentifier\n";
        res = false;
        exit(EXIT_SUCCESS);
    }
    
    return res;
}

bool IDS_Prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<IDSPrime> -> ,<IDS>|<Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<IDSPrime> -> ,<IDS>|<Empty>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == ",")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (IDS())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = empty();
    }
    
    return res;
}

bool statementList()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<SL> -> <S><SL_Prime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<SL> -> <S><SL_Prime>\n";
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (statement())
    {
        printInstructionTable(instructionT);
        if (statementListPrime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool statementListPrime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<SLP> -> <SL> | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<SLP> -> <SL> | <Empty>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (statementList())
    {
        
    }
    else
    {
        res = empty();
    }
    
    return res;
}


bool statement()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<S> -> <Comp> | <Assign> | <If> | <Return> | <Write> | <Read> | <While>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<S> -> <Comp> | <Assign> | <If> | <Return> | <Write> | <Read> | <While>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (compound())
    {
        
    }
    else if (assign())
    {
        
    }
    else if (if_nonTerm())
    {
        
    }
    else if (return_nonTerm())
    {
        
    }
    else if (Write())
    {
        
    }
    else if (Read())
    {
    }
    else if (While())
    {
        
    }
    else {
        res = false;
    }
    
    return res;
}

bool compound()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<compound> -> {<SL>}\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<compound> -> {<SL>}\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "{")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (statementList())
        {
            if (lexemeVector[vectorMarker] == "}")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
            }
            else
            {
                puts("Error - Expected a }\n");
                outputFile << "Error - Expected a }\n";
                res = false;
                passThru = true;
                return main();
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool assign()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Assign> -> <Identifier> := <Expr>;\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Assign> -> <Identifier> := <Expr>;\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (tokenVector[vectorMarker] == "Identifier")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        
        save = lexemeVector[vectorMarker];
        
        
        vectorMarker++;
        if (lexemeVector[vectorMarker] == ":=")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            
            
            string str = lexemeVector[vectorMarker];
            if (isdigit(str[0]) != 0)
            {
                int intVal;
                stringstream(lexemeVector[vectorMarker]) >> intVal;
                
                gen_instr("PUSHI", intVal, instructionT, instructionAddress);
            }
            else {
                
            }
            
            if (expr())
            {
                
                addr = get_address(save, symbolT);
                
                gen_instr("POPM", addr, instructionT, instructionAddress);
                
                
                
                if (lexemeVector[vectorMarker] == ";")
                {
                    cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    vectorMarker++;
                }
                else
                {
                    puts("Error - Expected a ;\n");
                    outputFile << "Error - Expected a ;\n";
                    res = false;
                    passThru = true;
                    return main();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            puts("Error - Expected a :=\n");
            outputFile << "Error - Expected a :=\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}


bool if_nonTerm()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<if> -> if (<Condition>)<S><IfPrime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<if> -> if (<Condition>)<S><IfPrime>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "if")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        
        
        addr = instructionAddress;
        
        
        vectorMarker++;
        if (lexemeVector[vectorMarker] == "(")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (condition())
            {
                if (lexemeVector[vectorMarker] == ")")
                {
                    cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    vectorMarker++;
                    if (statement())
                    {
                        
                        
                        back_patch(addr, instructionT);
                        
                        
                        
                        
                        if (if_nonTerm_Prime())
                        {
                            
                        }
                    }
                }
                else
                {
                    puts("Error - Expected a )\n");
                    outputFile << "Error - Expected a )\n";
                    res = false;
                    passThru = true;
                    return main();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            puts("Error - Expected a (\n");
            outputFile << "Error - Expected a (\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool if_nonTerm_Prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<IFPrime> -> fi | else <S> fi\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<IFPrime> -> fi | else <S> fi\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "fi")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "else")
    {
        
        addr = instructionAddress;
        
        
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (statement())
        {
            back_patch(addr, instructionT);
            
            if (lexemeVector[vectorMarker] == "fi")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
            }
            else
            {
                puts("Error - Expected a fi\n");
                outputFile << "Error - Expected a fi\n";
                res = false;
                passThru = true;
                return main();
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        puts("Error - Expected either an fi/else\n");
        outputFile << "Error - Expected either an fi/else\n";
        res = false;
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    return res;
}

bool return_nonTerm()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<return_nonTerm> -> return<return_nonTerm_prime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<return_nonTerm> -> return<return_nonTerm_prime>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "return")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (return_nonTerm_prime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

bool return_nonTerm_prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<return_nonTerm_prime> -> ;|<expr>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<return_nonTerm_prime> -> ;| <expr>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == ";")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (expr())
    {
        if (lexemeVector[vectorMarker] == ";")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
        }
        else
        {
            puts("Error - Expected a ;\n");
            outputFile << "Error - Expected a ;\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        puts("Error - Expected either a ';' or an expression\n");
        outputFile << "Error - Expected either a ';' or an expression\n";
        res = false;
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    return res;
}

bool Write()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Write> -> write(<expr>);\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Write> -> write(<expr>);\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "write")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        
        
        vectorMarker++;
        if (lexemeVector[vectorMarker] == "(")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (expr())
            {
                if (lexemeVector[vectorMarker] == ")")
                {
                    cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    vectorMarker++;
                    if (lexemeVector[vectorMarker] == ";")
                    {
                        addr = instructionAddress;
                        
                        gen_instr("STDOUT", 0, instructionT, instructionAddress);
                        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        vectorMarker++;
                    }
                    else
                    {
                        puts("Error - Expected a ;\n");
                        outputFile << "Error - Expected a ;\n";
                        res = false;
                        passThru = true;
                        return main();
                        exit(EXIT_SUCCESS);
                    }
                }
                else
                {
                    puts("Error - Expected a )\n");
                    outputFile << "Error - Expected a )\n";
                    res = false;
                    passThru = true;
                    return main();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            puts("Error - Expected a (\n");
            outputFile << "Error - Expected a (\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool Read()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Read> -> read(<IDS>);\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Read> -> read(<IDS>);\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "read")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        
        gen_instr("STDIN", 0, instructionT, instructionAddress);
        
        
        
        vectorMarker++;
        if (lexemeVector[vectorMarker] == "(")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            
            vectorMarker++;
            if (isIDinSymbolTable(lexemeVector[vectorMarker], symbolT))
            {
                int foundADDR = get_address(lexemeVector[vectorMarker],symbolT);
                gen_instr("POPM", foundADDR, instructionT, instructionAddress);
            }
            
            if (IDS())
            {
                
                if (lexemeVector[vectorMarker] == ")")
                {
                    cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    vectorMarker++;
                    if (lexemeVector[vectorMarker] == ";")
                    {
                        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                        vectorMarker++;
                    }
                    else
                    {
                        puts("Error - Expected a ;\n");
                        outputFile << "Error - Expected a ;\n";
                        res = false;
                        passThru = true;
                        return main();
                        exit(EXIT_SUCCESS);
                    }
                }
                else
                {
                    puts("Error - Expected a )\n");
                    outputFile << "Error - Expected a )\n";
                    res = false;
                    passThru = true;
                    return main();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            puts("Error - Expected a (\n");
            outputFile << "Error - Expected a (\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool While()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<While> -> while(<Condition>)<S>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<While> -> while(<Condition>)<S>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "while")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        
        addr = instructionAddress;
        static int intTemp = addr ;
        gen_instr("LABEL", 0, instructionT, instructionAddress);
        
        
        
        vectorMarker++;
        if (lexemeVector[vectorMarker] == "(")
        {
            cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
            vectorMarker++;
            if (condition())
            {
                if (lexemeVector[vectorMarker] == ")")
                {
                    cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                    vectorMarker++;
                    if (statement())
                    {
                        
                        
                        
                        gen_instr("JUMP", intTemp, instructionT, instructionAddress);
                        back_patch(instructionAddress, instructionT);
                        
                        
                        
                    }
                    else
                    {
                        res = false;
                    }
                }
                else
                {
                    puts("Error - Expected a )\n");
                    outputFile << "Error - Expected a )\n";
                    res = false;
                    passThru = true;
                    return main();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            puts("Error - Expected a (\n");
            outputFile << "Error - Expected a (\n";
            res = false;
            passThru = true;
            return main();
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool condition()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<condition> -> <expr><relop><expr>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<condition> -> <expr><relop><expr>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (expr())
    {
        string Op = lexemeVector[vectorMarker];
        if (relop())
        {
            if (expr())
            {
                
                if (Op == "<")
                {
                    gen_instr("LES\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
                else if (Op == ">")
                {
                    gen_instr("GRT\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
                else if (Op == "=:")
                {
                    gen_instr("EQU\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
                else if (Op == "/=")
                {
                    gen_instr("NEQ\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
                else if (Op == "<")
                {
                    gen_instr("LEQ\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
                else if (Op == ">")
                {
                    gen_instr("GEQ\t", 0, instructionT, instructionAddress);
                    S.push(instructionAddress);
                    gen_instr("JUMPZ", 0, instructionT, instructionAddress);
                }
                
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool relop()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<relop> -> =|/=|>|<|=>|<=\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<relop> -> =|/=|>|<|=>|<=\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "=")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "/=")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == ">")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "<")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "<=")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "=>")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else
    {
        puts("Error - Expected a relational operator\n");
        outputFile << "Error - Expected a relational operator\n";
        res = false;
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    return res;
}

bool expr()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<expr> -> <T><expr_Prime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<expr> -> <T><expr_Prime>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (term())
    {
        if (expr_Prime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool expr_Prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<expr_Prime> -> +<T><expr_Prime>|-<T><expr_Prime>|<Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<expr_Prime> -> +<T><expr_Prime>|-<T><expr_Prime>|<Empty>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "+" || lexemeVector[vectorMarker] == "-")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (term())
        {
            string testInt = lexemeVector[vectorMarker -1];
            if (isdigit(testInt[0]) != 0)
            {
                
                int intVal;
                stringstream(lexemeVector[vectorMarker-1]) >> intVal;
                
                gen_instr("PUSHI", intVal, instructionT, instructionAddress);
            }
            gen_instr("ADD\t", 0, instructionT, instructionAddress);
            
            if (expr_Prime())
            {
                
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        empty();
    }
    
    return res;
}

bool term()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<term> -> <factor><term_Prime>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<term> -> <factor><term_Prime>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (factor())
    {
        if (term_Prime())
        {
            
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    
    return res;
}

bool term_Prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<term_Prime> -> *<F><term_Prime> |/<F><term_Prime>|<Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<term_Prime> -> *<F><term_Prime> |/<F><term_Prime>|<Empty>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "*" || lexemeVector[vectorMarker] == "/")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (factor())
        {
            gen_instr("MUL", 0, instructionT, instructionAddress);
            
            if (term_Prime())
            {
                
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = empty();
    }
    
    return res;
}

bool factor()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<factor> -> - <primary> |<primary>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<factor> -> - <primary> |<primary>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    
    
    
    
    
    if (lexemeVector[vectorMarker] == "-")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (primary())
        {
        }
        else
        {
            res = false;
        }
    }
    else
    {
        if (primary())
        {
        }
        else
        {
            res = false;
        }
    }
    
    return res;
}

bool primary()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<primary> -> <Identifier><primary_prime> | <integer> | (<expr>) | <floating> | true | false\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<primary> -> <Identifier><primary_prime> | <integer> | (<expr>) | <floating> | true | false\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (tokenVector[vectorMarker] == "Identifier")
    {
        int i = 0;
        while (i < lexemeVector.size() && symbolT.numberOfIdentifiers > 0 )
        {
            if (isIDinSymbolTable(lexemeVector[i], symbolT) == false && tokenVector[i] == "Identifier")
            {
                ofstream ostream("symbolTable.txt");
                ostream << "\nERROR - Cannot use an Identifier (" << lexemeVector[i] << ") without declaring first!\n";
                ostream.close();
                exit(EXIT_FAILURE);
            }
            else
            {
                i++;
            }
        }
        
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        
        addr = get_address(lexemeVector[vectorMarker], symbolT);
        gen_instr("PUSHM", addr, instructionT, instructionAddress);
        
        vectorMarker++;
        if (primary_prime())
        {
            
        }
    }
    else if (tokenVector[vectorMarker] == "Integer")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (tokenVector[vectorMarker] == "floating")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else if (lexemeVector[vectorMarker] == "(")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (expr())
        {
            if (lexemeVector[vectorMarker] == ")")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
            }
            else
            {
                puts("Error - Expected a )\n");
                outputFile << "Error - Expected a )\n";
                res = false;
                passThru = true;
                return main();
                exit(EXIT_SUCCESS);
            }
        }
    }
    else if (lexemeVector[vectorMarker] == "true" || lexemeVector[vectorMarker] == "false")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
    }
    else
    {
        puts("Error - Invalid Primary\n");
        outputFile << "Error - Invalid Primary\n";
        res = false;
    }
    
    return res;
}

bool primary_prime()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<primary_prime> -> [<IDS>] | <Empty>\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<primary_prime> -> [<IDS>] | <Empty>\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    
    bool res = true;
    
    if (lexemeVector[vectorMarker] == "[")
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << "\n\n";
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        vectorMarker++;
        if (IDS())
        {
            if (lexemeVector[vectorMarker] == "]")
            {
                cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << "\n\n";
                outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
                vectorMarker++;
            }
            else
            {
                puts("Error - Expected a ]\n");
                outputFile << "Error - Expected a ]\n";
                res = false;
                passThru = true;
                return main();
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = empty();
    }
    
    return res;
}

bool empty()
{
    if (printSwitchOn && vectorMarker < tokenVector.size())
    {
        cout << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        puts("<Empty> -> epsilon\n");
        
        outputFile << "Token: " << tokenVector[vectorMarker] << "\t\t" << "Lexeme: " << lexemeVector[vectorMarker] << endl;
        outputFile << "<Empty> -> epsilon\n";
    }
    else
    {
        passThru = true;
        return main();
        exit(EXIT_SUCCESS);
    }
    return true;
}
