#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include "tokens.h"

string ACTION_TABLE[182][89];
string RULES_RIGHT_LEFT[87][2];
t_token currentToken;

// enum TOKEN_ACTION_TABLE {
// // palavras reservadas
// ARRAY, BOOLEAN, BREAK, CHAR, CONTINUE, DO, ELSE, TOKEN_FALSE, FUNCTION,
// IF, INTEGER, OF, STRING, STRUCT, TOKEN_TRUE, TYPE, VAR, WHILE,
// // simbolos
// COLON, SEMI_COLON, COMMA, EQUALS, LEFT_SQUARE, RIGHT_SQUARE,
// LEFT_BRACES, RIGHT_BRACES, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, AND,
// OR, LESS_THAN, GREATER_THAN, LESS_OR_EQUAL, GREATER_OR_EQUAL,
// NOT_EQUAL, EQUAL_EQUAL, PLUS, PLUS_PLUS, MINUS, MINUS_MINUS, TIMES,
// DIVIDE, DOT, NOT,
// // tokens regulares
// CHARACTER, NUMERAL, STRINGVAL, TOKEN_ID,
// // token fim de arquivo
// END_OF_FILE,
// // token deconhecido
// UNKNOWN,
// // tokens não terminais
// P_PRIME,P,LDE,DE,T,DT,DC,DF,LP,B,LDV,LS,DV,LI,S,U,M,E,L,R,Y,F,LE,LV,IDD,IDU,ID,TRUE,FALSE,CHR,STR,NUM,NB,MF,MC,NF,MT,ME,MW
// };


// Número de elementos do lado direito de cada regra
//int RIGHT_OF_RULE[] = {1,		2,		1,		1,		1,		1,		1,		1,		1,		1,		9,		8,		4,		5,		3,		10,		5,		3,		4,		2,		1,		2,		1,		5,		3,		1,		1,		1,		6,		9,		9,		7,		8,		2,		4,		2,		2,		3,		3,		1,		3,		3,		3,		3,		3,		3,		1,		3,		3,		1,		3,		3,		1,		1,		2,		2,		2,		2,		3,		5,		2,		2,		1,		1,		1,		1,		1,		3,		1,		3,		4,		1,		1,		1,		1,		1,			1,			1,		1,		1,		0,		0,		0,		0,		0,		0,		0};

enum RULES_TYPE {
    P_PRIME=49,P,LDE,DE,T,DT,DC,DF,LP,B,LDV,LS,DV,LI,S,U,M,E,L,R,Y,F,LE,LV,IDD,IDU,ID,TRUE,FALSE,CHR,STR,NUM,NB,MF,MC,NF,MT,ME,MW
};

unordered_map<string, RULES_TYPE> TOKEN_RULES = {
    {"P_PRIME", P_PRIME},
    {"P", P},
    {"LDE", LDE},
    {"DE", DE},
    {"T", T},
    {"DT", DT},
    {"DC", DC},
    {"DF", DF},
    {"LP", LP},
    {"B", B},
    {"LDV", LDV},
    {"LS", LS},
    {"DV", DV},
    {"LI", LI},
    {"S", S},
    {"U", U},
    {"M", M},
    {"E", E},
    {"L", L},
    {"R", R},
    {"Y", Y},
    {"F", F},
    {"LE", LE},
    {"LV", LV},
    {"IDD", IDD},
    {"IDU", IDU},
    {"ID", ID},
    {"TRUE", TRUE},
    {"FALSE", FALSE},
    {"CHR", CHR},
    {"STR", STR},
    {"NUM", NUM},
    {"NB", NB},
    {"MF", MF},
    {"MC", MC},
    {"NF", NF},
    {"MT", MT},
    {"ME", ME},
    {"MW", MW}
};


// Elementos a esquerda das regras
// RULES_TYPE LEFT_OF_RULE[] = {
//     P,     LDE,	LDE,	DE,	    DE,	    T,	    T,	    T,      T,  	T,  	DT, 	DT, 	DT, 	DC, 	DC, 	DF, 	LP, 	LP, 	B,  	LDV,	LDV,	LS, 	LS, 	DV, 	LI, 	LI, 	S,  	S,  	U,  	U,  	M,	    M,  	M,  	M,  	M,  	M,  	M,  	E,  	E,  	E,  	L,  	L,  	L,  	L,	    L,  	L,  	L,  	R,  	R,	    R,  	Y,  	Y,  	Y,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,	    LE, 	LE, 	LV,	    LV, 	LV,	    IDD,	IDU,	ID, 	TRUE,   	FALSE,  	CHR,    STR,	NUM,     NB,   	MF,	    MC,	    NF,	    MT, 	ME,	    MW};

bool syntaticalError = false;

#define IS_SHIFT(act) (act[0] == 's')
#define IS_REDUCTION(act) (act[0] == 'r')

void GET_TABLE() {
    ifstream file;
    file.open("action_table.csv", ios::in); 
  
    if(!file.is_open()) cout << "Error opening action_table file\n";

    string cel;
    string garbage;
    getline(file, garbage, '\n');

    for (int line=0; line<181; line++){
        getline(file, garbage, ',');

        for (int col=0; col<87; col++){
            getline(file, cel, ',');
            ACTION_TABLE[line][col] = cel;
        }

        if (line == 180) {
            ACTION_TABLE[line][87] = "";
        }
        else{
            getline(file, cel, '\n');
            if (cel == "\n") ACTION_TABLE[line][87] = "";
            else  ACTION_TABLE[line][87] = cel; 
        }
        
    }
    
    file.close();
};

void GET_RULES() {
    ifstream file;
    file.open("rules_right_left.csv", ios::in); 
  
    if(!file.is_open()) cout << "Error opening 'RULES_RIGHT_LEFT' file\n";

    for (int line=0; line<86; line++){
        getline(file, RULES_RIGHT_LEFT[line][0], ',');
        getline(file, RULES_RIGHT_LEFT[line][1], '\n');
    }

    // for (int line=0; line<86; line++){
    //     cout << "(" << RULES_RIGHT_LEFT[line][0] << "," << RULES_RIGHT_LEFT[line][1] << ")" << endl;
    // }

    file.close();
};

void syntaxError(const std::string& errMsg) {
    std::cerr << "Erro sintatico: " << errMsg << std::endl;
    // exit(EXIT_FAILURE);
}

int action_to_int(string action) {
    string num;
    for (int i=0; i<action.length()-1; i++) {
        num[i] = action[i+1];
    }
    return stoi(num);
}


int main() {

    // Lê a tabela action_table.csv e copia para ACTION_TABLE (matriz de strings)
    GET_TABLE();
    GET_RULES();

    try {
        initializeLexer("input.txt");

        currentToken = nextToken();  // Inicializa o currentToken com o primeiro token do arquivo
        // cout << endl << "Current = " << currentToken << endl;

        std::stack<int> stack;
        int state = 0;

        stack.push(state);

        string action = ACTION_TABLE[stack.top()][currentToken];
        
        int cont = 0;

        // CONTROL + ; para descomentar múltiplas linhas

        while (action != "acc") {

            if (IS_SHIFT(action)) {
                cout << "IS_SHIFT - " << action << endl;

                state = action_to_int(action);
                stack.push(state);
                currentToken = nextToken();
                action = ACTION_TABLE[state][currentToken];
                cont += 1;

            } else if (IS_REDUCTION(action)) {
                cout << "IS_REDUCTION - " << action << endl;

                int rule = action_to_int(action);
                
                // Dando pop o número de elementos q a regra reduz
                for (int i=0; i<stoi(RULES_RIGHT_LEFT[rule-1][0]); i++)
                    stack.pop();

                try {
                    // cout << "try - STATE = " << state << endl;
                    cout << stack.top() << " - " << RULES_RIGHT_LEFT[rule-1][1] << endl;

                    for (int i=0; i<89; i++){
                        if (ACTION_TABLE[stack.top()][i] != "")
                            cout << "(" << i << ":" << ACTION_TABLE[stack.top()][i] << ") --- ";
                    }
                    state = stoi(ACTION_TABLE[stack.top()][TOKEN_RULES[RULES_RIGHT_LEFT[rule-1][1]]]);
                    // cout << endl << "TOKEN RULES =" << TOKEN_RULES[RULES_RIGHT_LEFT[rule-1][1]] << endl;
                    // cout << endl << "action_table =" << ACTION_TABLE[stack.top()][TOKEN_RULES[RULES_RIGHT_LEFT[rule-1][1]]] << "." << endl;
                    // cout << "STATE = " << state << endl;

                } catch(string err) {
                    syntaticalError = true;
                    syntaxError(err);
                    break;
                };

                stack.push(state);
                action = ACTION_TABLE[state][currentToken];
                cont += 1;
                // Semantic_Analysis(self.lexical, rule)
                
            } else{
                syntaticalError = true;
                syntaxError("Unexpected token");
                break;
            }

            if (lexicalError) {
                std::cerr << endl << " ---------- Houve erro Lexico ---------- " << endl;
                for (pair<char,int> x :vecLexError)
                    std::cerr << "Erro no caractere = " << x.first << " - Order = " << x.second << endl;
                    // exit(EXIT_FAILURE);
            }
        }
        
        finalizeLexer();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }


    return 0;
};


int oldd_main() {
    try {
        initializeLexer("input.txt");

        currentToken = nextToken();  // Inicializa o currentToken com o primeiro token do arquivo
        // cout << currentToken;
        
        if (lexicalError) {
            std::cerr << endl << " ---------- Houve erro Lexico ---------- " << endl;
            for (pair<char,int> x :vecLexError)
                std::cerr << "Erro no caractere = " << x.first << " - Order = " << x.second << endl;
                // exit(EXIT_FAILURE);
        }
        
        finalizeLexer();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
