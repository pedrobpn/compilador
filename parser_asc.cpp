#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include "tokens.h"


string ACTION_TABLE[182][89];
t_token currentToken;



enum ASTNodeType {
    NODE_P, NODE_LDE, NODE_DE, NODE_T, NODE_DT, NODE_DC, NODE_DF, 
    NODE_LP, NODE_B, NODE_LDV, NODE_LS, NODE_DV, NODE_LI, NODE_S, 
    NODE_U, NODE_M, NODE_E, NODE_L, NODE_R, NODE_Y, NODE_F, NODE_LE, 
    NODE_LV, NODE_IDD, NODE_IDU, NODE_ID, NODE_TRUE, NODE_FALSE, NODE_CHR, 
    NODE_STR, NODE_NUM, NODE_NB, NODE_MF, NODE_MC, NODE_NF, NODE_MT, 
    NODE_ME, NODE_MW
};

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
int RIGHT_OF_RULE[] = {1,		2,		1,		1,		1,		1,		1,		1,		1,		1,		9,		8,		4,		5,		3,		10,		5,		3,		4,		2,		1,		2,		1,		5,		3,		1,		1,		1,		6,		9,		9,		7,		8,		2,		4,		2,		2,		3,		3,		1,		3,		3,		3,		3,		3,		3,		1,		3,		3,		1,		3,		3,		1,		1,		2,		2,		2,		2,		3,		5,		2,		2,		1,		1,		1,		1,		1,		3,		1,		3,		4,		1,		1,		1,		1,		1,			1,			1,		1,		1,		0,		0,		0,		0,		0,		0,		0};

enum RULES_TYPE {
    P,LDE,DE,DF,DT,T,DC,LI,LP,B,LDV,LS,DV,S,E,LV,L,R,Y,F,LE,ID,TRUE,FALSE,CHR,STR,NUM,P_PRIME,M,U,IDD,IDU,NB,MF,MC,NF,MT,ME,MW
};


// Elementos a esquerda das regras
RULES_TYPE LEFT_OF_RULE[] = {
    P,     LDE,	LDE,	DE,	    DE,	    T,	    T,	    T,      T,  	T,  	DT, 	DT, 	DT, 	DC, 	DC, 	DF, 	LP, 	LP, 	B,  	LDV,	LDV,	LS, 	LS, 	DV, 	LI, 	LI, 	S,  	S,  	U,  	U,  	M,	    M,  	M,  	M,  	M,  	M,  	M,  	E,  	E,  	E,  	L,  	L,  	L,  	L,	    L,  	L,  	L,  	R,  	R,	    R,  	Y,  	Y,  	Y,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,  	F,	    LE, 	LE, 	LV,	    LV, 	LV,	    IDD,	IDU,	ID, 	TRUE,   	FALSE,  	CHR,    STR,	NUM,     NB,   	MF,	    MC,	    NF,	    MT, 	ME,	    MW};

bool syntaticalError = false;

bool GET_TABLE() {
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
    return true;
};

void syntaxError(const std::string& errMsg) {
    std::cerr << "Erro sintatico: " << errMsg << std::endl;
    // exit(EXIT_FAILURE);
}

void match(t_token expected) {
    if (currentToken == expected) {
        currentToken = nextToken();
    } else {
        syntaxError("Token inesperado!");
    }
}

bool isIdToken(t_token token) {
    return token == TOKEN_ID;
}


bool isStatementStartToken(t_token token) {
    // Esta função verifica se o token atual é o início de uma sentença/comando. 
    return token == IF || token == WHILE || token == DO || isIdToken(token) || token == BREAK || token == CONTINUE;
}


bool IS_SHIFT(string cell) {
    return (cell[0] == 's');
}

bool IS_REDUCTION(string cell) {
    return (cell[0] == 'r');
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


    try {
        initializeLexer("input.txt");

        currentToken = nextToken();  // Inicializa o currentToken com o primeiro token do arquivo
        // cout << currentToken;

        std::stack<int> stack;
        int state = 0;

        stack.push(state);

        string action = ACTION_TABLE[stack.top()][currentToken];
        
        int cont = 0;

        // CONTROL + ; para descomentar múltiplas linhas

        while (action != "acc") {

            if (action[0] == 's') {
                state = action_to_int(action);
                stack.push(state);
                currentToken = nextToken();
                action = ACTION_TABLE[state][currentToken];
                cont += 1;
            } else if (action[0] == 'r') {
                int rule = action_to_int(action);
                for (int i=0; i<RIGHT_OF_RULE[rule]; i++)
                    stack.pop();

                try {
                    state = action_to_int(ACTION_TABLE[stack.top()][LEFT_OF_RULE[rule]]);
                } catch(...) {
                    syntaticalError = true;
                    cout << "Sintaxe Error in line " << endl;
                    break;
                };

                stack.push(state);
                action = ACTION_TABLE[state][currentToken];
                cont += 1;
                // Semantic_Analysis(self.lexical, rule)
            } else{
                syntaticalError = true;
                cout << "Sintaxe Error in line " << endl;
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
