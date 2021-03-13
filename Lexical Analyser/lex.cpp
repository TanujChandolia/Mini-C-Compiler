#include<bits/stdc++.h>
using namespace std;

//Read user file.
//Convert to tokens; remove whitespaces and comments.
//Output the converted token file.
void C();
void C_1();


class SymbolTableEntry{
public:
     int sno;
     string category;
     string token;
     int lineno;

     SymbolTableEntry(int a, string b, string c, int d){
          sno = a;
          category = b;
          token = c;
          lineno = d;
     }
};


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
                         "using","virtual","wchar_t"};

vector<char> operators = {'+','-','*','/','%','<','>','!','^','&','|','~','='};
vector<SymbolTableEntry> SymbolTable;
vector<pair<string,int>> tokenList;
int lookAhead = 0;


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

     SymbolTableEntry ste ((int)SymbolTable.size() + 1, category, currToken, lineNum);
     SymbolTable.push_back(ste);

     string tokenToAdd = tokenGenerator(currToken);
     tokenList.push_back({tokenToAdd,lineNum});
}
void printSymbolTable(){
     cout<<"SYMBOL TABLE: \n\n";
     cout<<"Serial No.";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Line No.";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Token";
     for(int i=0;i<20;i++) cout<<" ";
     cout<<"Category\n\n";


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

          cout<<SymbolTable[i].category<<"\n";
     }
}

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
void LIT(){
     if(tokenList[lookAhead].first == "ID") match("ID");
     else if(tokenList[lookAhead].first == "NUM") match("NUM");
     else error();
}
void BIN_AROP(){
     if(tokenList[lookAhead].first == "T_pl") match("T_pl");
     else if(tokenList[lookAhead].first == "T_min") match("T_min");
     else if(tokenList[lookAhead].first == "T_mul") match("T_mul");
     else if(tokenList[lookAhead].first == "T_div") match("T_div");
}
void BIN_BOOLOP(){
     if(tokenList[lookAhead].first == "T_and") match("T_and");
     else if(tokenList[lookAhead].first == "T_or") match("T_or");
}
void UN_AROP(){
     if(tokenList[lookAhead].first == "T_incr") match("T_incr");
     if(tokenList[lookAhead].first == "T_decr") match("T_decr");
}
void UN_BOOLOP(){
     if(tokenList[lookAhead].first == "T_not") match("T_not");
}
void TYPE(){
     if(tokenList[lookAhead].first == "INT"){
          match("INT");
     }
     else if(tokenList[lookAhead].first == "CHAR"){
          match("CHAR");
     }
     else if(tokenList[lookAhead].first == "FLOAT"){
          match("FLOAT");
     }
     else error();

}
void ARITH_EXPR(){
     if(tokenList[lookAhead].first == "ID" || tokenList[lookAhead].first == "NUM"){
          LIT();
          if(tokenList[lookAhead].first == "T_pl" || tokenList[lookAhead].first == "T_min" || tokenList[lookAhead].first == "T_div" || tokenList[lookAhead].first == "T_mul"){
               BIN_AROP();
               ARITH_EXPR();
          }
          else if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
               BIN_BOOLOP();
               ARITH_EXPR();
          }
          else if(tokenList[lookAhead].first == "T_incr" || tokenList[lookAhead].first == "T_decr"){
               UN_AROP();
          }
          else return;
     }

     else if(tokenList[lookAhead].first == "T_incr" || tokenList[lookAhead].first == "T_decr"){
          UN_AROP();
          ARITH_EXPR();
     }
     else if(tokenList[lookAhead].first == "T_not"){
          UN_BOOLOP();
          ARITH_EXPR();
     }
     else error();
}
void ASSIGN_EXPR(){
     if(tokenList[lookAhead].first == "ID"){
          match("ID");
          match("T_eq");
          ARITH_EXPR();
     }
     else {
          TYPE();
          match("ID");
          if(tokenList[lookAhead].first == "T_eq"){
               match("T_eq");
               ARITH_EXPR();
          }
     }
}
void RELOP(){
     if(tokenList[lookAhead].first == "T_lt") match("T_lt");
     else if(tokenList[lookAhead].first == "T_gt") match("T_gt");
     else if(tokenList[lookAhead].first == "T_lteq") match("T_lteq");
     else if(tokenList[lookAhead].first == "T_gteq") match("T_gteq");
     else if(tokenList[lookAhead].first == "T_neq") match("T_neq");
     else if(tokenList[lookAhead].first == "T_eqeq") match("T_eqeq");
     else error();
}
void STATEMENT(){
     if(tokenList[lookAhead + 1].first == "+" || tokenList[lookAhead + 1].first == "-" || tokenList[lookAhead + 1].first == "/" || tokenList[lookAhead + 1].first == "*" ||
tokenList[lookAhead + 1].first == "&&" || tokenList[lookAhead + 1].first == "||"){
          ARITH_EXPR();
     }
     else{
          ASSIGN_EXPR();
     }
}
void COND(){
     if(tokenList[lookAhead].first == "ID" || tokenList[lookAhead].first == "NUM"){
          LIT();
          if(tokenList[lookAhead].first == "T_lt" || tokenList[lookAhead].first == "T_gt" || tokenList[lookAhead].first == "T_lteq" || tokenList[lookAhead].first == "T_gteq" ||
     tokenList[lookAhead].first == "T_neq" || tokenList[lookAhead].first == "T_eqeq"){
               RELOP();
               LIT();
               if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
                    BIN_BOOLOP();
                    LIT();
                    RELOP();
                    LIT();
               }
          }
          else if(tokenList[lookAhead].first == "T_and" || tokenList[lookAhead].first == "T_or"){
               BIN_BOOLOP();
               LIT();
          }
     }

     if(tokenList[lookAhead].first == "T_not"){
          UN_BOOLOP();
          if(tokenList[lookAhead].first == "("){
               match("(");
               LIT();
               if(tokenList[lookAhead].first == ")"){
                    match(")");
               }
               else{
                    RELOP();
                    LIT();
                    if(tokenList[lookAhead].first == ")"){
                         match(")");
                    }
               }
          }
          else{
               LIT();
               if(tokenList[lookAhead].first == "T_lt" || tokenList[lookAhead].first == "T_gt" || tokenList[lookAhead].first == "T_lteq" || tokenList[lookAhead].first == "T_gteq" ||
          tokenList[lookAhead].first == "T_neq" || tokenList[lookAhead].first == "T_eqeq"){
                    RELOP();
                    LIT();
               }
          }
     }
}
void LOOPBODY(){
     if(tokenList[lookAhead].first == "{"){
          match("{");
          C();
          match("}");
     }

     else if(tokenList[lookAhead].first == "}"){
          match("}");
     }

     else{
          STATEMENT();
          match(";");
     }

}
void LOOPS(){
     if(tokenList[lookAhead].first == "WHILE"){
          match("WHILE");
          match("(");
          COND();
          match(")");
          LOOPBODY();
     }
     else if(tokenList[lookAhead].first == "FOR"){
          match("FOR");
          match("(");
          ASSIGN_EXPR();
          match(";");
          COND();
          match(";");
          STATEMENT();
          match(")");
          LOOPBODY();
     }
     else if(tokenList[lookAhead].first == "IF"){
          match("IF");
          match("(");
          COND();
          match(")");
          LOOPBODY();
          if(tokenList[lookAhead].first == "ELSE"){
               match("ELSE");
               LOOPBODY();
          }
     }
}
void C_1(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" || 
     tokenList[lookAhead].first == "ID"){
          STATEMENT();
          match(";");
          C_1();
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          LOOPS();
          C_1();
     }
     else return; //epsilon
}
void C(){
     if(tokenList[lookAhead].first == "INT" || tokenList[lookAhead].first == "CHAR" || tokenList[lookAhead].first == "FLOAT" ||
     tokenList[lookAhead].first == "ID"){
          STATEMENT();
          match(";");
          C_1();
     }
     else if(tokenList[lookAhead].first == "FOR" || tokenList[lookAhead].first == "WHILE" || tokenList[lookAhead].first == "IF"){
          LOOPS();
          C_1();
     }
}
void BODY(){
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
          match("VOID");
     }
     else if(tokenList[lookAhead].first == "INT"){
          match("INT");
     }
     else error();

     if(tokenList[lookAhead].first == "MAINTOK"){
          match("MAINTOK");
          match("(");
          match(")");
          BODY();
     }
     else error();
}
void START(){
     if(tokenList[lookAhead].first == "INCLUDE"){
          match("INCLUDE");
          match("H");
          MAIN();
     }
     else error();
}
void S(){
     START();
}

/*FUNCTIONS FOR TOP DOWN RECURSIVE DESCENT PARSER*/
int main(){
     fillAccepted();

     ifstream f;
     f.open("../Input.txt");
     string line;
     string currToken = "";
     int lineNum = 0;
     while(!f.eof()){
          getline(f,line);
          lineNum++;
          if(line == "") continue;
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

     printSymbolTable();
     cout<<"\n";
     cout<<"Token List to be sent to Parser: \n";
     for(pair<string,int> s:tokenList) cout<<s.first<<" ";
     cout<<"\n";
     tokenList.push_back({"$",lineNum+1});
     cout<<"\nParsing the given input\n";
     S();
     if(tokenList[lookAhead].first == "$")
          cout<<"Parsing Complete\n";
     else cout<<"Parsing cannot be completed. Invalid String.";
     return 0;
}
