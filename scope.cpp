#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include "scope.h"
#include "tokens.h"

string ACTION_TABLE[182][89];
string RULES_RIGHT_LEFT[87][2];
t_token currentToken;

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

    Scope* globalScope = new Scope();
    scopeStack.push(globalScope);

    try {
        initializeLexer("input.txt");

        // Inicializa o currentToken com o primeiro token do arquivo
        currentToken = nextToken();  
        std::cerr << endl << "Current = " << currentToken << endl;

        // Cria stack e insere o 1o estado (0)
        std::stack<int> stack;
        int state = 0;
        stack.push(state);

        // Pega a 1a ação relativa ao state atual com base no currentToken
        string action = ACTION_TABLE[stack.top()][currentToken];
        
        int cont = 0; // ?????????????????????? TIRAR?

        while (action != "acc") {

            if (IS_SHIFT(action)) {
                cout << endl << "SHIFT - " << action << endl;

                state = action_to_int(action);
                stack.push(state);
                currentToken = nextToken();
                action = ACTION_TABLE[state][currentToken];
                cont += 1;

            } else if (IS_REDUCTION(action)) {
            cout << endl << "REDUCTION - " << action << endl;

            int rule = action_to_int(action);
            
            // Dando pop o número de elementos q a regra reduz
            for (int i=0; i<stoi(RULES_RIGHT_LEFT[rule-1][0]); i++)
                stack.pop();

            try {
                cout << stack.top() << " - " << RULES_RIGHT_LEFT[rule-1][1] << endl;

                // for (int i=0; i<89; i++){
                //     if (ACTION_TABLE[stack.top()][i] != "")
                //         cout << "(" << i << ":" << ACTION_TABLE[stack.top()][i] << ") --- ";
                // }

                // Atualiza state com base na regra usada para a redução 
                state = stoi(ACTION_TABLE[stack.top()][TOKEN_RULES[RULES_RIGHT_LEFT[rule-1][1]]]);

                // Handle scope analysis based on the rule
                std::cerr << "Rule: " << rule << endl; 
                switch (rule) {
                    case DF:  // Function definition
                        enterNewScope();
                        break;
                    case B:  // Start of a block
                        enterNewScope();
                        break;
                    case LS: // End of a statement, potentially ending a block
                        if (scopeStack.size() > 1) {
                            leaveScope();
                        }
                        break;
                    case DV: // Variable declaration
                        std::cerr << "aqui";
                        checkVariableDeclaration(getIdentifierValue(), currentToken);
                        break;
                    case LV: // Variable usage
                        checkVariableUsage(getIdentifierValue());
                        break;
                    // Potentially add more rules if necessary.
                }

            } catch(string err) {
                syntaticalError = true;
                syntaxError(err);
                break;
            };

            stack.push(state);
            action = ACTION_TABLE[state][currentToken];
            cont += 1;
            // Semantic_Analysis(self.lexical, rule)
        }
        
        else{
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