#include<bits/stdc++.h>
#include <sstream>
#include <Windows.h>
#include <conio.h>
using namespace std;

class SymbolTableEntry{
public:
     int sno;
     string category;
     string token;
     int lineno;
     string dataType;

     SymbolTableEntry(int a, string b, string c, int d, string e){
          sno = a;
          category = b;
          token = c;
          lineno = d;
          dataType = (e == "")? "NULL": e;
     }
};
class AST_Node{
public:
     string name;
     AST_Node *left;
     AST_Node *right;
     AST_Node(string s){
          name = s;
          left = nullptr;
          right = nullptr;
     }
};
class quadruple{
public:
     string op;
     string arg1;
     string arg2;
     string res;
     quadruple(){
          op = "";
          arg1 = "";
          arg2 = "";
          res = "";
     }
     quadruple(string a, string b, string c, string d){
          op = a;
          arg1 = b;
          arg2 = c;
          res = d;
     }
};

void C();
void C_1();
AST_Node* LOOPC();
AST_Node* LOOPC_1();

vector<char> acceptedNames = {'_'};
vector<string> keywords ={"auto","double","int" ,"struct",
                         "break","else","long","switch",
                         "if(currToken == ","enum","register","typedef",
                         "char","extern","return","union",
                         "const","float","short","unsigned",
                         "continue","for","signed","void",
                         "default","goto","sizeof","volatile",
                         "do","if","static","while","asm",
                         "bool","catch","class",
                         "const_cast","delete","dynamic_cast","explicit",
                         "export","false","friend","inline",
                         "mutable","namespace","new","operator",
                         "private","protected","public","reinterpret_cast",
                         "static_cast","template","this","throw",
                         "true","try","typeid","typename",
                         "using","virtual","wchar_t","main"};

vector<char> operators = {'+','-','*','/','%','<','>','!','^','&','|','~','='};
vector<SymbolTableEntry> SymbolTable;
vector<pair<string,int>> tokenList;
int lookAhead = 0;
vector<vector<string>> AST(100,vector<string>());
int AST_index = 0;
vector<string> ICG(100);
int ICG_index = -1;
vector<vector<string>> threeAddress(100,vector<string>());
int three_index = 0;
vector<quadruple> quad(100);
int quadlen = 0;

unordered_map<string, string> whatId;
unordered_map<string, string> whatName;
int idCount = 1;

unordered_map<string, string> whatNum;
unordered_map<string, string> whatNumber;
int numCount = 1;

string lastDataType = "";

string to_String(int value) {
    stringstream ss;
    ss << value;
    return ss.str();
}

/*FUNCTIONS FOR LEXICAL ANALYSER*/
void fillAccepted(){
     for(int i = 0; i < 9; i++) acceptedNames.push_back(i + '0');
     for(char x = 'a'; x <= 'z'; x++) acceptedNames.push_back(x);
     for(char x = 'A'; x <= 'Z'; x++) acceptedNames.push_back(x);
     return;
}
string tokenGenerator(string currToken){

     if(currToken == "for") return "FOR";
     if(currToken == "while") return "WHILE";
     if(currToken ==  "if") return "IF";
     if(currToken ==  "else") return "ELSE";
     if(currToken ==  "cout") return "COUT";
     if(currToken ==  "endl") return "ENDL";
     if(currToken ==  "break") return "BREAK";
     if(currToken ==  "continue") return "CONTINUE";
     if(currToken ==  "int") return "INT";
     if(currToken ==  "float") return "FLOAT";
     if(currToken ==  "char") return "CHAR";
     if(currToken ==  "void") return "VOID";
     if(currToken ==  "#include") return "INCLUDE";
     if(currToken ==  "main") return "MAINTOK";
     if(currToken ==  "<") return "T_lt";
     if(currToken ==  ">") return "T_gt";
     if(currToken ==  "=") return "T_eq";
     if(currToken ==  "<=") return "T_lteq";
     if(currToken ==  ">=") return "T_gteq";
     if(currToken ==  "==") return "T_eqeq";
     if(currToken ==  "!=") return "T_neq";
     if(currToken ==  "+") return "T_pl";
     if(currToken ==  "-") return "T_min";
     if(currToken ==  "*") return "T_mul";
     if(currToken ==  "/") return "T_div";
     if(currToken ==  "++") return "T_incr";
     if(currToken ==  "--") return "T_decr";
     if(currToken ==  "!") return "T_not";
     if(currToken ==  "||") return "T_or";
     if(currToken ==  "&&") return "T_and";


     bool allNum = true;                               //for numbers
     for(char c:currToken){
          if(!isdigit(c)) allNum = false;
     }
     if(allNum) return "NUM";

     bool allAccepted = true;                          // for identifier
     for(char c:currToken){
          bool present = false;
          for(char x:acceptedNames){
               if(x == c){
                    present = true;
                    break;
               }
          }
          if(!present){
               allAccepted = false;
               break;
          }
     }
     if(allAccepted) return "ID";

     if(currToken[0] == '<' && currToken[currToken.length() - 1] == '>') return "H";           // for .h files

     return currToken;
}
void addCurrentToken(string currToken, int lineNum){
     if(currToken == " " || currToken == "")return;
     string category;
     for(string s:keywords) if (s == currToken) category = "Keyword";
     for(char c:operators) if(c == currToken[0]) category = "Operator";

     bool allNum = true;
     for(char c:currToken){
          if(!isdigit(c)) allNum = false;
     }
     if(allNum) category = "Constant";
     if(currToken[0] == '#') category = "Pre-Processor Directive";
     if(currToken[0] == '<' && currToken[currToken.length()-1] == '>') category = "Header File";
     if(category == "") category = "Identifier";

     if(category == "Keyword"){
          if(currToken == "int") lastDataType = "INT";
          if(currToken == "float") lastDataType = "FLOAT";
          if(currToken == "char") lastDataType = "CHAR";
     }

     if(category == "Constant"){
          string newid = "NUM";
          newid += to_String(numCount);
          numCount++;
          whatNum[currToken] = newid;
          whatNumber[newid] = currToken;
          SymbolTableEntry ste ((int)SymbolTable.size() + 1, category, currToken, lineNum,lastDataType);
          SymbolTable.push_back(ste);
          tokenList.push_back({whatNum[currToken],lineNum});
          lastDataType = "";
     }

     else if(category == "Identifier"){
          if(whatId.find(currToken) == whatId.end()){
               string newid = "ID";
               newid += to_String(idCount);
               idCount++;
               whatId[currToken] = newid;
               whatName[newid] = currToken;
               SymbolTableEntry ste ((int)SymbolTable.size() + 1, category, currToken, lineNum,lastDataType);
               SymbolTable.push_back(ste);
          }
          else{
               for(int i = 0; i < SymbolTable.size(); i++){
                    if(SymbolTable[i].token == currToken && lastDataType != "") SymbolTable[i].dataType = "#";
                    if(SymbolTable[i].token == currToken && lastDataType == SymbolTable[i].dataType) SymbolTable[i].dataType = "%";
               }
          }
          tokenList.push_back({whatId[currToken],lineNum});
          lastDataType = "";
     }
     else{
          SymbolTableEntry ste ((int)SymbolTable.size() + 1, category, currToken, lineNum,"");
          SymbolTable.push_back(ste);
          string tokenToAdd = tokenGenerator(currToken);
          tokenList.push_back({tokenToAdd,lineNum});
     }


}
void printSymbolTable(){
     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n";
     for(int i = 0; i < 57; i++) cout<<" ";
     cout<<"SYMBOL TABLE\n\n";
     for(int i = 0; i < 130; i++) cout<<"-";
     cout<<"\n\n";

     cout<<"Serial No.";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Line No.";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Token";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Category";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Data Type\n\n";


     for(int i = 0; i < SymbolTable.size(); i++){
          cout<<SymbolTable[i].sno;

          int end = (SymbolTable[i].sno>9)?2:1;
          for(int j = 0; j < 30-end; j++) cout<<" ";

          cout<<SymbolTable[i].lineno;

          end = (SymbolTable[i].lineno>9)?2:1;
          for(int j = 0; j < 28-end; j++) cout<<" ";

          cout<<SymbolTable[i].token;

          end = (SymbolTable[i].token.length());
          for(int j = 0; j < 25-end; j++) cout<<" ";

          cout<<SymbolTable[i].category;

          end = (SymbolTable[i].category.length());
          for(int j = 0; j < 28-end; j++) cout<<" ";

          cout<<SymbolTable[i].dataType<<"\n";
     }

     for(int i = 0; i <130; i++) cout<<"-";
}
/*FUNCTIONS FOR LEXICAL ANALYSER*/

/*FUNCTIONS FOR ABSTRACT SYNTAX TREE*/
AST_Node* buildTree(string s, AST_Node* l, AST_Node* r){
     AST_Node* node = new AST_Node(s);
     node -> left = l;
     node -> right = r;
     return node;
}
void printTree(AST_Node* root){
     if(root -> left || root -> right) AST[AST_index].push_back("(");
     AST[AST_index].push_back(root -> name);
     AST[AST_index].push_back(" ");
     if(root -> left) printTree(root -> left);
     if(root -> right) printTree(root -> right);
     if(root -> left || root -> right) AST[AST_index].push_back(")");
}
/*FUNCTIONS FOR ABSTRACT SYNTAX TREE*/

/*FUNCTIONS FOR INTERMEDIATE CODE GENERATION*/
int temp_i = 0;
string temp;
int l_while = 0;
int l_num = 0;
int ltop = 0;
vector<int> label(20);
int l_for = 0;

void push(string s = ""){
     if(s == "") ICG[++ICG_index] = tokenList[lookAhead].first;
     else ICG[++ICG_index] = s;
}
void pusha(){
     ICG[++ICG_index] = "  ";
}
void pushx(){
     ICG[++ICG_index] = "x ";
}
void pushab(){
     ICG[++ICG_index] = " ";
     ICG[++ICG_index] = " ";
     ICG[++ICG_index] = " ";
}
void codegen(){
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back(ICG[ICG_index-2]);
     threeAddress[three_index].push_back(ICG[ICG_index-1]);
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = ICG[ICG_index-1];
     quad[quadlen].arg1 = ICG[ICG_index-2];
     quad[quadlen].arg2 = ICG[ICG_index];
     quad[quadlen].res = temp;

     quadlen++;
     ICG_index -= 2;
     ICG[ICG_index] = temp;
     temp_i++;
}
void codegen_assigna(){
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back(ICG[ICG_index-3]);
     threeAddress[three_index].push_back(ICG[ICG_index-2]);
     threeAddress[three_index].push_back(ICG[ICG_index-1]);
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     if(ICG[ICG_index].length() == 1){
          string t = "";
          t += ICG[ICG_index - 2];
          t += ICG[ICG_index - 1];

          quad[quadlen].op = t;
          quad[quadlen].arg1 = ICG[ICG_index-3];
          quad[quadlen].arg2 = ICG[ICG_index];
          quad[quadlen].res = temp;

          quadlen++;
     }
     else{
          quad[quadlen].op = ICG[ICG_index-2];
          quad[quadlen].arg1 = ICG[ICG_index-3];
          quad[quadlen].arg2 = ICG[ICG_index-1];
          quad[quadlen].res = temp;

          quadlen++;
     }

     ICG_index -= 4;
     temp_i++;
     ICG[++ICG_index] = temp;
}
void codegen_assign(){
     threeAddress[three_index].push_back(ICG[ICG_index-3]);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = "=";
     quad[quadlen].arg1 = ICG[ICG_index];
     quad[quadlen].arg2 = "NULL";
     quad[quadlen].res = ICG[ICG_index-3];

     quadlen++;
     ICG_index -= 2;
}
void if1(){
     l_num++;
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back("not");
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = "not";
     quad[quadlen].arg1 = ICG[ICG_index];
     quad[quadlen].res = temp;
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("if");
     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L"+ to_String(l_num));
     three_index++;

     quad[quadlen].op = "if";
     quad[quadlen].arg1 = ICG[ICG_index-2];
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     temp_i++;
     label[++ltop] = l_num;
}
void if3(){
     int y;
     y = label[ltop--];

     threeAddress[three_index].push_back("L" + to_String(y) + ':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(y);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
}
void ifelse1(){
     l_num++;
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back("not");
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = "not";
     quad[quadlen].arg1 = ICG[ICG_index];
     quad[quadlen].res = temp;
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("if");
     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L"+ to_String(l_num));
     three_index++;

     quad[quadlen].op = "if";
     quad[quadlen].arg1 = ICG[ICG_index-2];
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     temp_i++;
     label[++ltop] = l_num;
}
void ifelse2(){
     int x;
     l_num++;
     x = label[ltop--];

     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L" + to_String(l_num));
     three_index++;

     quad[quadlen].op = "goto";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L" + to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("L" + to_String(x) + ':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L" + to_String(x);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     label[++ltop] = l_num;
}
void ifelse3(){
     int y;
     y = label[ltop--];

     threeAddress[three_index].push_back("L" + to_String(y) + ':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L" + to_String(y);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     l_num++;
}
void while1(){
     l_while = l_num;
     temp = "L";
     string temp_l = to_String(l_num);
     temp += temp_l;
     l_num++;

     threeAddress[three_index].push_back(temp+':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].arg2 = "NULL";
     quad[quadlen].res = temp;
     quadlen++;
}
void while2(){
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back("not");
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = "not";
     quad[quadlen].arg1 = ICG[ICG_index];
     quad[quadlen].res = temp;
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("if");
     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L"+ to_String(l_num));
     three_index++;

     quad[quadlen].op = "if";
     quad[quadlen].arg1 = temp;
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     temp_i++;
}
void while3(){
     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L" + to_String(l_while));
     three_index++;

     quad[quadlen].op = "goto";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_while);
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("L" + to_String(l_num) + ':');
     three_index++;
     l_num++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_num-1);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
}
void for1(){
     l_for = l_num;

     threeAddress[three_index].push_back("L" + to_String(l_num) + ':');
     three_index++;
     l_num++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L" + to_String(l_num-1);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
}
void for2(){
     temp = "T";
     string tmp_id = to_String(temp_i);
     temp += tmp_id;

     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("=");
     threeAddress[three_index].push_back("not");
     threeAddress[three_index].push_back(ICG[ICG_index]);
     three_index++;

     quad[quadlen].op = "not";
     quad[quadlen].arg1 = ICG[ICG_index];
     quad[quadlen].res = temp;
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("if");
     threeAddress[three_index].push_back(temp);
     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L"+ to_String(l_num));
     three_index++;

     quad[quadlen].op = "if";
     quad[quadlen].arg1 = temp;
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     temp_i++;
     label[++ltop] = l_num;
     l_num++;

     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L" + to_String(l_num));
     three_index++;

     quad[quadlen].op = "goto";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
     label[++ltop] = l_num;
     l_num++;

     threeAddress[three_index].push_back("L" + to_String(l_num) + ':');
     three_index++;
     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
}
void for3(){
     int x;
     x = label[ltop--];

     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L" + to_String(l_for));
     three_index++;

     quad[quadlen].op = "goto";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_for);
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("L" + to_String(x) + ':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(x);
     quad[quadlen].arg2 = "NULL";

     quadlen++;

}
void for4(){
     int x;
     x = label[ltop--];

     threeAddress[three_index].push_back("goto");
     threeAddress[three_index].push_back("L" + to_String(l_num));
     three_index++;

     quad[quadlen].op = "goto";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(l_num);
     quad[quadlen].arg2 = "NULL";

     quadlen++;

     threeAddress[three_index].push_back("L" + to_String(x) + ':');
     three_index++;

     quad[quadlen].op = "Label";
     quad[quadlen].arg1 = "NULL";
     quad[quadlen].res = "L"+ to_String(x);
     quad[quadlen].arg2 = "NULL";

     quadlen++;
}
/*FUNCTIONS FOR INTERMEDIATE CODE GENERATION*/

/*FUNCTIONS FOR TOP DOWN RECURSIVE DESCENT PARSER*/
void error(){
     cout<<"Error in line no. "<<tokenList[lookAhead].second<<"\n";
     lookAhead++;
}
void match(string t){
     if(t == tokenList[lookAhead].first){
          lookAhead ++;
     }
     else error();
}

AST_Node* LIT(){
     if(tokenList[lookAhead].first.substr(0,2) == "ID") {
          cout<<"LIT -> id\n";
          push();
          AST_Node *ptr = new AST_Node(whatName[tokenList[lookAhead].first]);
          lookAhead++;
          return ptr;
     }
     else if(tokenList[lookAhead].first.substr(0,3) == "NUM"){
          cout<<"LIT -> num\n";
          push();
          AST_Node *ptr = new AST_Node(whatNumber[tokenList[lookAhead].first]);
          lookAhead++;
          return ptr;
     }
     else error();
     return nullptr;
}
void BIN_AROP(){
     if(tokenList[lookAhead].first == "T_pl") {push("+");cout<<"BIN_AROP -> +\n";match("T_pl");}
     else if(tokenList[lookAhead].first == "T_min") {push("-");cout<<"BIN_AROP -> -\n";match("T_min");}
     else if(tokenList[lookAhead].first == "T_mul") {push("*");cout<<"BIN_AROP -> *\n";match("T_mul");}
     else if(tokenList[lookAhead].first == "T_div") {push("/");cout<<"BIN_AROP -> /\n";match("T_div");}
}
void BIN_BOOLOP(){
     if(tokenList[lookAhead].first == "T_and") {push("&&");cout<<"BIN_BOOLOP -> &&\n";match("T_and");}
     else if(tokenList[lookAhead].first == "T_or") {push("||");cout<<"BIN_BOOLOP -> ||\n";match("T_or");}
}
void UN_AROP(){
     if(tokenList[lookAhead].first == "T_incr") {push("++");cout<<"UN_AROP -> ++\n";match("T_incr");}
     if(tokenList[lookAhead].first == "T_decr") {push("--");cout<<"UN_AROP -> --\n";match("T_decr");}
}
void UN_BOOLOP(){
     if(tokenList[lookAhead].first == "T_not") {push("!");cout<<"UN_BOOLOP -> !\n";match("T_not");}
}
void TYPE(){
     if(tokenList[lookAhead].first == "INT"){
          cout<<"TYPE -> int\n";
          match("INT");
     }
     else if(tokenList[lookAhead].first == "CHAR"){
          cout<<"TYPE -> char\n";
          match("CHAR");
     }
     else if(tokenList[lookAhead].first == "FLOAT"){
          cout<<"TYPE -> float\n";
          match("FLOAT");
     }
     else error();

}
AST_Node* ARITH_EXPR(){
     if(tokenList[lookAhead].first.substr(0,2) == "ID" || tokenList[lookAhead].first.substr(0,3) == "NUM"){
          cout<<"ARITH_EXPR -> LIT\t";
          AST_Node* ptr1 = LIT();
          if(tokenList[lookAhead].first == "T_pl" || tokenList[lookAhead].first == "T_min" || tokenList[lookAhead].first == "T_div" || tokenList[lookAhead].first == "T_mul"){
               string op = tokenList[lookAhead].first;
               cout<<"ARITH_EXPR -> BIN_AROP\tARITH_EXPR -> ARITH_EXPR\t";
               BIN_AROP();
               AST_Node* ptr2 = ARITH_EXPR();
               codegen();
               AST_Node* ptr = nullptr;
               if(op == "T_pl"){
                    ptr = buildTree("+",ptr1,ptr2);
               }
               if(op == "T_min"){
                    ptr = buildTree("-",ptr1,ptr2);
               }
               if(op == "T_div"){
                    ptr = buildTree("/",ptr1,ptr2);
               }
               if(op == "T_mul"){
                    ptr = buildTree("*",ptr1,ptr2);
               }
               cout<<"\n";
               return ptr;
          }
          else if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
               cout<<"ARITH_EXPR -> BIN_BOOLOP\tARITH_EXPR -> ARITH_EXPR\t";
               string op = tokenList[lookAhead].first;
               BIN_BOOLOP();
               AST_Node* ptr2 = ARITH_EXPR();
               AST_Node* ptr = nullptr;
               if(op == "T_and"){
                    ptr = buildTree("&&",ptr1,ptr2);
               }
               if(op == "T_or"){
                    ptr = buildTree("||",ptr1,ptr2);
               }
               cout<<"\n";
               return ptr;
          }
          else if(tokenList[lookAhead].first == "T_incr" || tokenList[lookAhead].first == "T_decr"){
               cout<<"ARITH_EXPR -> UN_AROP\n";
               UN_AROP();
          }
          else {
               return ptr1;
          }
     }

     else if(tokenList[lookAhead].first == "T_incr" || tokenList[lookAhead].first == "T_decr"){
          cout<<"ARITH_EXPR -> UN_AROP\tARITH_EXPR -> ARITH_EXPR\n";
          UN_AROP();
          ARITH_EXPR();
     }
     else if(tokenList[lookAhead].first == "T_not"){
          cout<<"ARITH_EXPR -> UN_BOOLOP\tARITH_EXPR -> ARITH_EXPR\n";
          UN_BOOLOP();
          ARITH_EXPR();
     }
     else error();
     return nullptr;
}
AST_Node* ASSIGN_EXPR(){
     if(tokenList[lookAhead].first.substr(0,2) == "ID"){
          cout<<"ASSIGN_EXPR -> LIT\tASSIGN_EXPR -> =\tASSIGN_EXPR -> ARITH_EXPR\n";
          push();
          AST_Node *ptr1 = LIT();
          push();
          match("T_eq");
          AST_Node *ptr2 = ARITH_EXPR();
          codegen_assign();
          AST_Node *ptr = buildTree("=",ptr1,ptr2);
          return ptr;
     }
     else {
          cout<<"ASSIGN_EXPR -> TYPE\tASSIGN_EXPR -> LIT\n";
          TYPE();
          push();
          AST_Node *ptr1 = LIT();
          if(tokenList[lookAhead].first == "T_eq"){
               cout<<"ASSIGN_EXPR -> =\tASSIGN_EXPR -> ARITH_EXPR\n";
               push();
               match("T_eq");
               AST_Node *ptr2 =  ARITH_EXPR();
               codegen_assign();
               AST_Node *ptr = buildTree("=",ptr1,ptr2);
               return ptr;
          }
          cout<<"\n";
          return ptr1;
     }
     return nullptr;
}
void RELOP(){
     if(tokenList[lookAhead].first == "T_lt") {push("<");cout<<"RELOP -> <\n";match("T_lt");}
     else if(tokenList[lookAhead].first == "T_gt") {push(">");cout<<"RELOP -> >\n";match("T_gt");}
     else if(tokenList[lookAhead].first == "T_lteq"){push("<=");cout<<"RELOP -> <=\n"; match("T_lteq");}
     else if(tokenList[lookAhead].first == "T_gteq") {push(">=");cout<<"RELOP -> >=\n";match("T_gteq");}
     else if(tokenList[lookAhead].first == "T_neq") {push("!=");cout<<"RELOP -> !=\n";match("T_neq");}
     else if(tokenList[lookAhead].first == "T_eqeq") {push("==");cout<<"RELOP -> ==\n";match("T_eqeq");}
     else error();
}
AST_Node* STATEMENT(){
     if(tokenList[lookAhead + 1].first == "T_pl" || tokenList[lookAhead + 1].first == "T_min" || tokenList[lookAhead + 1].first == "T_div" || tokenList[lookAhead + 1].first == "T_mul" ||
tokenList[lookAhead + 1].first == "T_and" || tokenList[lookAhead + 1].first == "T_or" || tokenList[lookAhead + 1].first == "T_incr" ||
tokenList[lookAhead + 1].first == "T_decr" || tokenList[lookAhead + 1].first == "T_not"){
          cout<<"STATEMENT -> ARITH_EXPR\n";
          return ARITH_EXPR();
     }
     else{
          cout<<"STATEMENT -> ASSIGN_EXPR\n";
          return ASSIGN_EXPR();
     }
     return nullptr;
}
AST_Node* COND(){
     if(tokenList[lookAhead].first.substr(0,2) == "ID" || tokenList[lookAhead].first.substr(0,3) == "NUM"){
          cout<<"COND -> LIT\t";
          AST_Node* ptr1 = LIT();
          if(tokenList[lookAhead].first == "T_lt" || tokenList[lookAhead].first == "T_gt" || tokenList[lookAhead].first == "T_lteq" || tokenList[lookAhead].first == "T_gteq" ||
     tokenList[lookAhead].first == "T_neq" || tokenList[lookAhead].first == "T_eqeq"){
               string op = tokenList[lookAhead].first;
               cout<<"COND -> RELOP\tCOND -> LIT\t";
               RELOP();
               AST_Node* ptr2 = LIT();
               AST_Node* ptr = nullptr;
               if(op == "T_lt") {pusha();ptr = buildTree("<",ptr1,ptr2);}
               if(op == "T_gt") {pusha();ptr = buildTree(">",ptr1,ptr2);}
               if(op == "T_lteq") {ptr = buildTree("<=",ptr1,ptr2);}
               if(op == "T_gteq") {ptr = buildTree(">=",ptr1,ptr2);}
               if(op == "T_neq") ptr = buildTree("!=",ptr1,ptr2);
               if(op == "T_eqeq") ptr = buildTree("==",ptr1,ptr2);

               codegen_assigna();

               if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
                    cout<<"COND -> BIN_BOOLOP\tCOND -> LIT\tCOND -> RELOP\tCOND -> LIT\n";
                    string op2 = tokenList[lookAhead].first;
                    BIN_BOOLOP();
                    AST_Node* ptr3 = LIT();
                    string op3 = tokenList[lookAhead].first;
                    RELOP();
                    AST_Node* ptr4 = LIT();
                    AST_Node* ptr5 = nullptr;
                    if(op3 == "T_lt") ptr5 = buildTree("<",ptr3,ptr4);
                    if(op3 == "T_gt") ptr5 = buildTree(">",ptr3,ptr4);
                    if(op3 == "T_lteq") ptr5 = buildTree("<=",ptr3,ptr4);
                    if(op3 == "T_gteq") ptr5 = buildTree(">=",ptr3,ptr4);
                    if(op3 == "T_neq") ptr5 = buildTree("!=",ptr3,ptr4);
                    if(op3 == "T_eqeq") ptr5 = buildTree("==",ptr3,ptr4);

                    AST_Node* ptr6 = nullptr;
                    if(op2 == "T_and") ptr6 = buildTree("&&",ptr,ptr5);
                    if(op2 == "T_or") ptr6 = buildTree("||",ptr,ptr5);
                    return ptr6;
               }
               else return ptr;
          }
          else if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
               cout<<"COND -> BIN_BOOLOP\tCOND -> LIT\t";
               BIN_BOOLOP();
               LIT();
          }
          cout<<"\n";
          return ptr1;
     }

     if(tokenList[lookAhead].first == "T_not"){
          cout<<"COND -> UN_BOOLOP\t";
          UN_BOOLOP();
          if(tokenList[lookAhead].first == "("){
               cout<<"COND -> (\tCOND -> LIT\t";
               match("(");
               LIT();
               if(tokenList[lookAhead].first == ")"){
                    cout<<"COND -> )\t";
                    match(")");
               }
               else{
                    cout<<"COND -> RELOP\tCOND -> LIT\t";
                    RELOP();
                    LIT();
                    if(tokenList[lookAhead].first == ")"){
                         cout<<"COND -> )\t";
                         match(")");
                    }
               }
          }
          else{
               cout<<"COND -> LIT\t";
               LIT();
               if(tokenList[lookAhead].first == "T_lt" || tokenList[lookAhead].first == "T_gt" || tokenList[lookAhead].first == "T_lteq" || tokenList[lookAhead].first == "T_gteq" ||
          tokenList[lookAhead].first == "T_neq" || tokenList[lookAhead].first == "T_eqeq"){
                    cout<<"COND -> RELOP\tCOND -> LIT\t";
                    RELOP();
                    LIT();
               }
          }
          cout<<"\n";
     }
     return nullptr;
}
AST_Node* LOOPBODY(){
     if(tokenList[lookAhead].first == "{"){
          cout<<"LOOPBODY -> {\tLOOPBODY -> LOOPC\tLOOPBODY -> }\n";
          match("{");
          AST_Node* ptr = LOOPC();
          match("}");
          return ptr;
     }

     else if(tokenList[lookAhead].first == ";"){
          cout<<"LOOPBODY -> ;\n";
          match(";");
     }

     else{
          cout<<"LOOPBODY -> STATEMENT\tLOOPBODY -> ;\n";
          AST_Node* ptr = STATEMENT();
          match(";");
          return ptr;
     }

     return nullptr;
}
AST_Node* LOOPS(){
     if(tokenList[lookAhead].first == "WHILE"){
          cout<<"LOOPS -> while\tLOOPS -> (\tLOOPS -> COND\tLOOPS -> )\tLOOPS -> LOOPBODY\n";
          while1();
          match("WHILE");
          match("(");
          AST_Node* ptr1 = COND();
          while2();
          match(")");
          AST_Node* ptr2 = LOOPBODY();
          while3();
          AST_Node* ptr = buildTree("WHILE", ptr1, ptr2);
          return ptr;
     }
     else if(tokenList[lookAhead].first == "FOR"){
          cout<<"LOOPS -> for\tLOOPS -> (\tLOOPS -> ASSIGN_EXPR\tLOOPS -> ;\tLOOPS -> COND\tLOOPS -> ;\tLOOPS -> STATEMENT\tLOOPS -> )\tLOOPS -> CONDITION\n";
          match("FOR");
          match("(");
          AST_Node* ptr1 = ASSIGN_EXPR();
          for1();
          match(";");
          AST_Node* ptr2 = COND();
          for2();
          match(";");
          AST_Node* ptr3 = STATEMENT();
          for3();
          match(")");
          AST_Node* ptr4 = LOOPBODY();
          for4();

          AST_Node* ptr5 = buildTree("SEQ", ptr1,ptr2);
          AST_Node* ptr6 = buildTree("BODY",ptr4,ptr3);
          AST_Node* ptr =  buildTree("FOR",ptr5,ptr6);

          return ptr;
     }
     else if(tokenList[lookAhead].first == "IF"){
          bool ifelsepresent = false;
          for(int i = lookAhead+1; i < tokenList.size(); i++){
               if(tokenList[i].first == "IF") break;
               if(tokenList[i].first == "ELSE"){ ifelsepresent = true; break;}
          }
          cout<<"LOOPS -> if\tLOOPS -> (\tLOOPS -> COND\tLOOPS -> )\tLOOPS -> LOOPBODY\t";
          match("IF");
          match("(");
          AST_Node *ptr1 = COND();
          match(")");

          if(ifelsepresent)ifelse1();
          else if1();

          AST_Node *ptr2 = LOOPBODY();

          if(ifelsepresent)ifelse2();
          else if3();

          AST_Node* ptr3 = nullptr;
          if(tokenList[lookAhead].first == "ELSE"){
               cout<<"LOOPS -> else\tLOOPS -> LOOPBODY";
               match("ELSE");
               ptr3 = LOOPBODY();
               ifelse3();
          }
          cout<<"\n";
          AST_Node *ptr = buildTree("IF",ptr1,ptr2);
          if(ifelsepresent) ptr = buildTree("ELSE", ptr3, ptr);
          return ptr;
     }
     return nullptr;
}
AST_Node* LOOPC(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" ||
     tokenList[lookAhead].first.substr(0,2) == "ID"){
          cout<<"LOOPC -> STATEMENT\tLOOPC -> LOOPC1\n";
          AST_Node* ptr1 = STATEMENT();
          match(";");
          AST_Node* ptr2 = LOOPC_1();
          if(!ptr2) return ptr1;
          AST_Node* ptr = buildTree("SEQ",ptr1,ptr2);
          return ptr;
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          cout<<"LOOPC -> LOOPS\tLOOPC -> LOOPC1\n";
          AST_Node* ptr1 = LOOPS();
          AST_Node* ptr2 = LOOPC_1();
          if(!ptr2) return ptr1;
          AST_Node* ptr = buildTree("SEQ",ptr1,ptr2);
          return ptr;
     }
     return nullptr;
}
AST_Node* LOOPC_1(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" ||
     tokenList[lookAhead].first.substr(0,2) == "ID"){
          cout<<"LOOPC1 -> STATEMENT\tLOOPC1 -> LOOPC1\n";
          AST_Node* ptr1 = STATEMENT();
          match(";");
          AST_Node* ptr2 = LOOPC_1();
          if(!ptr2) return ptr1;
          AST_Node* ptr = buildTree("SEQ",ptr1,ptr2);
          return ptr;
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          cout<<"LOOPC1 -> LOOPS\tLOOPC1 -> LOOPC1\n";
          AST_Node* ptr1 = LOOPS();
          AST_Node* ptr2 = LOOPC_1();
          if(!ptr2) return ptr1;
          AST_Node* ptr = buildTree("SEQ",ptr1,ptr2);
          return ptr;
     }
     else cout<<"LOOPC1 -> epsilon\n";
     return nullptr; //epsilon
}
void C_1(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" ||
     tokenList[lookAhead].first.substr(0,2) == "ID"){
          cout<<"C1 -> STATEMENT\tC1 -> C1\n";
          AST_Node* root = STATEMENT();
          match(";");
          printTree(root);
          AST_index++;
          C_1();
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          cout<<"C1 -> LOOPS\tC1 -> C1\n";
          AST_Node* root = LOOPS();
          printTree(root);
          AST_index++;
          C_1();
     }
     else {
          cout<<"C1 -> epsilon\n";
     } //epsilon
}
void C(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" ||
     tokenList[lookAhead].first.substr(0,2) == "ID"){
          cout<<"C -> STATEMENT\tC -> C1\n";
          AST_Node* root = STATEMENT();
          match(";");
          printTree(root);
          AST_index++;
          C_1();
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          cout<<"C -> LOOPS\tC -> C1\n";
          AST_Node* root = LOOPS();
          printTree(root);
          AST_index++;
          C_1();
     }
}
void BODY(){
     cout<<"BODY -> {\tBODY -> C\tBODY -> }\n";
     if(tokenList[lookAhead].first == "{"){
          match("{");
          C();
     }
     else error();

     if(tokenList[lookAhead].first == "}"){
          match("}");
     }
     else error();
}
void MAIN(){
     if(tokenList[lookAhead].first == "VOID"){
          cout<<"MAIN -> VOID\t";
          match("VOID");
     }
     else if(tokenList[lookAhead].first == "INT"){
          cout<<"MAIN -> INT\t";
          match("INT");
     }
     else error();

     if(tokenList[lookAhead].first == "MAINTOK"){
          match("MAINTOK");
          match("(");
          match(")");
          cout<<"MAIN -> MAINTOK\t\tMAIN -> (\tMAIN -> )\tMAIN -> BODY\n";
          BODY();
     }
     else error();
}
void START(){
     if(tokenList[lookAhead].first == "INCLUDE"){
          match("INCLUDE");
          match("H");
          cout<<"START -> #include\tSTART -> H\tSTART -> MAIN\n";
          MAIN();
     }
     else error();
}
void S(){
     cout<<"\nS -> START\n";
     START();
}
/*FUNCTIONS FOR TOP DOWN RECURSIVE DESCENT PARSER*/
int main(){
     fillAccepted();

     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     for(int i = 0; i < 99; i++) cout<<" ";
     cout<<"MINI C++ COMPILER\n\n";
     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";

     cout<<"Source Code given as input: \n\n";
     ifstream f;
     f.open("../Input.txt");
     string line;
     string currToken = "";
     int lineNum = 0;
     while(!f.eof()){
          getline(f,line);
          cout<<line<<"\n";
          lineNum++;
          if(line == "") continue;
          if(line[0] == '/' && line[1] == '/') continue;
          currToken = "";

          for(char c: line){
               if(c == '(' || c == ')' || c == '{' || c == '}'){
                    addCurrentToken(currToken,lineNum);
                    currToken = "";
                    currToken += c;
                    tokenList.push_back({currToken,lineNum});
                    currToken = "";
                    continue;
               }
               if(c == ' ' || c == ';'){
                    addCurrentToken(currToken,lineNum);
                    currToken = "";
                    if(c == ';'){
                         currToken += c;
                         tokenList.push_back({currToken,lineNum});
                         currToken = "";
                    }
                    continue;
               }
               // bool found = false;
               // for(char ch:acceptedNames){
               //      if(ch == c) {
               //           found = true;
               //           currToken += c;
               //      }
               // }
               //
               // if(!found){
               //      addCurrentToken(currToken,lineNum);
               //      currToken = c;
               // }
               currToken += c;

          }
          if(currToken != "") addCurrentToken(currToken,lineNum);

     }
     cout<<"\nPress Any Key to begin Lexical Analysis.\n";
     _getch();

     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     for(int i = 0; i < 95; i++) cout<<" ";
     cout<<"PHASE 1: LEXICAL ANALYSER\n\n";
     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     cout<<"* Takes the given source code in a high level language.\n";
     cout<<"* Removes whitespace and comments.\n";
     cout<<"* Converts input source code to tokens.\n";
     cout<<"* Generates a list of tokens to be used by Parser.\n";
     cout<<"* Fills up Symbol Table.\n\n";
     cout<<"* Token List to be sent to Parser: \n";
     for(pair<string,int> s:tokenList){
          //Sleep(125);
          cout<<"\to "<<s.first<<"\n";
     }
     cout<<"\n";
     cout<<"\nPress Any Key to begin Syntax Analysis/ Parser.\n";
     _getch();
     cout<<"\n\n";

     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     for(int i = 0; i < 92; i++) cout<<" ";
     cout<<"PHASE 2: SYNTAX ANALYSER / PARSER\n\n";
     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";

     for(int i = 0; i < 89; i++) cout<<" ";
     cout<<"|| TOP DOWN RECURSIVE DESCENT PARSER ||\n\n";

     cout<<"* Takes the list of tokens given by Lexical Analyser.\n";
     cout<<"* Generates a Parse Tree using the given Grammar.\n";
     cout<<"* Reports syntactical errors in the code.\n";
     cout<<"* Fills up Symbol Table.\n\n";


     tokenList.push_back({"$",lineNum+1});
     cout<<"\n* Parsing the given input\n";
     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n";
     for(int i = 0; i < 57; i++) cout<<" ";
     cout<<"PARSE TREE\n\n";
     for(int i = 0; i < 130; i++) cout<<"-";
     cout<<"\n\n";
     S();
     if(tokenList[lookAhead].first == "$"){
          cout<<"\n\n* Parsing Complete. String is Valid.\n\n";
          printSymbolTable();
     }
     else {
          cout<<"* Parsing cannot be completed. Invalid String.";
          return 0;
     }

     cout<<"\n\nPress Any Key to begin Semantic Analysis.\n";
     _getch();
     cout<<"\n\n";

     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     for(int i = 0; i < 95; i++) cout<<" ";
     cout<<"PHASE 3: SEMANTIC ANALYSER\n\n";
     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";

     cout<<"* Takes the parse tree generated by Syntax Analyser / Parser.\n";
     cout<<"* Generates an Annotated Parse Tree using the given Grammar.\n";
     cout<<"* Performs type checking, scope resolution, array bound checking.\n";
     cout<<"* Reports semantical errors in the code such as missing declaration or multiple declarations.\n\n";

     cout<<"* Performing Semantic Analysis.\n\n";
     bool error = false;
     for(int i = 0; i < SymbolTable.size(); i++){
          if(SymbolTable[i].dataType == "#"){
               error = true;
               cout<<"Error in line number "<<SymbolTable[i].lineno<<": Multiple declaration for "<<SymbolTable[i].token<<"\n";
          }
          if(SymbolTable[i].dataType == "%"){
               error = true;
               cout<<"Error in line number "<<SymbolTable[i].lineno<<": Re-declaration for "<<SymbolTable[i].token<<"\n";
          }
          if(SymbolTable[i].category == "Identifier" && SymbolTable[i].dataType == "NULL"){
               error = true;
               cout<<"Error in line number "<<SymbolTable[i].lineno<<": Missing declaration for "<<SymbolTable[i].token<<"\n";
          }
     }
     cout<<"\n";
     if(error) {
          cout<<"* Semantic errors found. Semantic Analysis failed\n";
          return 0;
     }
     cout<<"* Semantic Analysis complete. No semantic errors found.\n\n";

     cout<<"\n\nPress Any Key to begin Intermediate Code Generation.\n";
     _getch();
     cout<<"\n\n";

     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";
     for(int i = 0; i < 90; i++) cout<<" ";
     cout<<"PHASE 4: INTERMEDIATE CODE GENERATION\n\n";
     for(int i = 0; i < 220; i++) cout<<"=";
     cout<<"\n\n";

     cout<<"* Takes the annotated parse tree generated by Semantic Analyser.\n";
     cout<<"* Converts the source code to machine independent code.\n";
     cout<<"* Generates Abstract Syntax Tree / Three-Address Code/ Postfix Notation/ Quadruples.\n\n";

     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n";
     for(int i = 0; i < 53; i++) cout<<" ";
     cout<<"ABSTRACT SYNTAX TREE\n\n";
     for(int i = 0; i < 130; i++) cout<<"-";
     cout<<"\n\n";

     for(int i = 0; i < AST_index; i++){
          for(int j = 0; j < AST[i].size(); j++){
               cout<<AST[i][j]<<" ";
          }
          cout<<"\n";
          for(int j = 0; j < 130; j++) cout<<"~";
          cout<<"\n";
     }
     cout<<"\n";
     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n\n\n";

     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n";
     for(int i = 0; i < 53; i++) cout<<" ";
     cout<<"THREE ADDRESS CODE\n\n";
     for(int i = 0; i < 130; i++) cout<<"-";
     cout<<"\n\n";

     for(int i = 0; i < three_index; i++){
          for(int j = 0; j < threeAddress[i].size(); j++){
               if(threeAddress[i][j].substr(0,2) == "ID") cout<<whatName[threeAddress[i][j]]<<" ";
               else if(threeAddress[i][j].substr(0,3) == "NUM") cout<<whatNumber[threeAddress[i][j]]<<" ";
               else cout<<threeAddress[i][j]<<" ";
          }
          cout<<"\n";
     }

     cout<<"\n\n";

     for(int i = 0; i <130; i++) cout<<"-";
     cout<<"\n\n";
     for(int i = 0; i < 58; i++) cout<<" ";
     cout<<"QUADRUPLES\n\n";
     for(int i = 0; i < 130; i++) cout<<"-";
     cout<<"\n\n";

     for(int i = 0; i < 65; i++) cout<<"~";
     cout<<"\n";
     cout<<"OPERATOR\tARG1\t\tARG2\t\tRESULT\n";
     for(int i = 0; i < 65; i++) cout<<"~";
     cout<<"\n";
     for(int i = 0; i < quadlen; i++){
          cout<<"  "<<quad[i].op<<"\t\t";
          if(quad[i].arg1.substr(0,2) == "ID") cout<<whatName[quad[i].arg1];
          else if(quad[i].arg1.substr(0,3) == "NUM") cout<<whatNumber[quad[i].arg1];
          else cout<<quad[i].arg1;
          cout<<"\t\t";
          if(quad[i].arg2.substr(0,2) == "ID") cout<<whatName[quad[i].arg2];
          else if(quad[i].arg2.substr(0,3) == "NUM") cout<<whatNumber[quad[i].arg2];
          else cout<<quad[i].arg2;
          cout<<"\t\t";
          if(quad[i].res.substr(0,2) == "ID") cout<<whatName[quad[i].res];
          else if(quad[i].res.substr(0,3) == "NUM") cout<<whatNumber[quad[i].res];
          else cout<<quad[i].res;
          cout<<"\n";
     }
     /*
     cout<<"\n";
     for(auto i:whatId){
          cout<<i.first<<" "<<i.second<<"\n";
     }

     for(auto i:whatName){
          cout<<i.first<<" "<<i.second<<"\n";
     }
     */
}
