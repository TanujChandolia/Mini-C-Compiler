#include<bits/stdc++.h>
using namespace std;

//Read user file.
//Convert to tokens; remove whitespaces and comments.
//Output the converted token file.

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
                         "case","enum","register","typedef",
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

void fillAccepted(){
     for(int i = 0; i < 9; i++) acceptedNames.push_back(i + '0');
     for(char x = 'a'; x <= 'z'; x++) acceptedNames.push_back(x);
     for(char x = 'A'; x <= 'Z'; x++) acceptedNames.push_back(x);
     return;
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
     if(category == "") category = "Identifier";

     SymbolTableEntry ste ((int)SymbolTable.size() + 1, category, currToken, lineNum);
     SymbolTable.push_back(ste);
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

          if(line[0] == '#'){
               continue;
          }

          for(char c: line){
               if(c == '(' || c == ')' || c == '{' || c == '}'){
                    currToken = "";
                    continue;
               }
               if(c == ' ' || c == ';'){
                    addCurrentToken(currToken,lineNum);
                    currToken = "";
                    continue;
               }
               bool found = false;
               for(char ch:acceptedNames){
                    if(ch == c) {
                         found = true;
                         currToken += c;
                    }
               }

               if(!found){
                    addCurrentToken(currToken,lineNum);
                    currToken = c;
               }

          }

     }

     printSymbolTable();
     return 0;
}
