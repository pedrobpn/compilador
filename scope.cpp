#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include <unordered_map>
#include <vector>
#include "scope.h"
#include "tokens.h"

string ACTION_TABLE[182][89];
string RULES_RIGHT_LEFT[87][2];
t_token currentToken;

enum TAB_RULES_COL {
    P_PRIME=49,P,LDE,DE,T,DT,DC,DF,LP,B,LDV,LS,DV,LI,S,U,M,E,L,R,Y,F,LE,LV,IDD,IDU,ID,TRUE,FALSE,CHR,STR,NUM,NB,MF,MC,NF,MT,ME,MW
};

enum RULES_NUMBER {
    PLINE_P_RULE = 0,
    P_LDE_RULE = 1,
    LDE_LDE_RULE = 2,
    LDE_DE_RULE = 3,
    DE_DF_RULE = 4,
    DE_DT_RULE = 5,
    T_INTEGER_RULE = 6,
    T_CHAR_RULE  = 7,
    T_BOOL_RULE = 8,
    T_STRING_RULE = 9,
    T_IDU_RULE = 10,
    DT_ARRAY_RULE = 11,
    DT_STRUCT_RULE = 12,
    DT_ALIAS_RULE = 12,
    DC_DC_RULE = 14,
    DC_LI_RULE = 15,
    DF_RULE = 16,
    LP_LP_RULE = 17,
    LP_IDD_RULE = 18,
    B_LS_RULE = 19,
    LDV_LDV_RULE = 20,
    LDV_DV_RULE = 21,
    LS_LS_RULE = 22,
    LS_S_RULE = 23,
    DV_VAR_RULE = 24,
    LI_COMMA_RULE = 25,
    LI_IDD_RULE = 26,
    S_M_RULE = 27,
    S_U_RULE = 28,
    U_IF_RULE = 29,
    U_IF_ELSE_U_RULE = 30,
    M_IF_ELSE_M_RULE = 31,
    M_WHILE_RULE = 32,
    M_DO_WHILE_RULE = 33,
    M_BLOCK_RULE = 34,
    M_E_SEMICOLON = 35,
    M_BREAK_RULE = 36,
    M_CONTINUE_RULE = 37,
    E_AND_RULE = 38,
    E_OR_RULE = 39,
    E_L_RULE = 40,
    L_LESS_THAN_RULE = 41,
    L_GREATER_THAN_RULE = 42,
    L_LESS_EQUAL_RULE = 43,
    L_GREATER_EQUAL_RULE = 44,
    L_EQUAL_EQUAL_RULE = 45,
    L_NOT_EQUAL_RULE = 46,
    L_R_RULE = 47,
    R_PLUS_RULE = 48,
    R_MINUS_RULE = 49,
    R_Y_RULE = 50,
    Y_TIMES_RULE = 51,
    Y_DIVIDE_RULE = 52,
    Y_F_RULE = 53,
    F_LV_RULE = 54,
    F_LEFT_PLUS_PLUS_RULE = 55,
    F_LEFT_MINUS_MINUS_RULE = 56,
    F_RIGHT_PLUS_PLUS_RULE = 57,
    F_RIGHT_MINUS_MINUS_RULE = 58,
    F_PARENTHESIS_E_RULE = 59,
    F_IDU_MC_RULE = 60,
    F_MINUS_F_RULE = 61,
    F_NOT_F_RULE = 62,
    F_TRUE_RULE = 63,
    F_FALSE_RULE = 64,
    F_CHR_RULE = 65,
    F_STR_RULE = 66,
    F_NUM_RULE = 67,
    LE_LE_RULE = 68,
    LE_E_RULE = 69,
    LV_DOT_RULE = 70,
    LV_SQUARE_RULE = 71,
    LV_IDU_RULE = 72,
    IDD_RULE = 73,
    IDU_RULE = 74,
    ID_RULE = 75,
    TRUE_RULE = 76,
    FALSE_RULE = 77,
    CHR_RULE = 78,
    STR_RULE = 79,
    NUM_RULE = 80,
    NB_RULE = 81,
    MF_RULE = 82,
    MC_RULE = 83,
    NF_RULE = 84,
    MT_RULE = 85,
    ME_RULE = 86,
    MW_RULE = 87
};

unordered_map<string, TAB_RULES_COL> TOKEN_RULES = {
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

std::string findStringWithInt(const std::unordered_map<std::string, int>& identifierTable, int value) {    
    for (const auto& pair : identifierTable) {
        if (pair.second == value) {
            return pair.first;
        }
    }
    return "";
}

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

        currentToken = nextToken();  // Inicializa o currentToken com o primeiro token do arquivo
        // std::cerr << endl << "Current = " << currentToken << endl;

        // Cria stack e insere o 1o estado (0)
        std::stack<int> stack;
        int state = 0;
        stack.push(state);

        string action = ACTION_TABLE[stack.top()][currentToken];

        t_token oldToken;
        int oldSecondToken;
        
        int cont = 0;

        while (action != "acc") {

            if (IS_SHIFT(action)) {
                // cout << endl << "SHIFT - " << action << endl;

                state = action_to_int(action);
                stack.push(state);

                oldToken = currentToken;
                oldSecondToken = secondToken;
                currentToken = nextToken();

                action = ACTION_TABLE[state][currentToken];
                cont += 1;

            } else if (IS_REDUCTION(action)) {
                // cout << endl << "REDUCTION - " << action << endl;

                int rule = action_to_int(action);
                
                // Dando pop o número de elementos q a regra reduz
                for (int i=0; i<stoi(RULES_RIGHT_LEFT[rule-1][0]); i++)
                    stack.pop();

                try {
                    // Atualiza state com base na regra usada para a redução
                    state = stoi(ACTION_TABLE[stack.top()][TOKEN_RULES[RULES_RIGHT_LEFT[rule-1][1]]]);
                    
                    // Realiza a análise de escopo com base na regra
                    std::string currentName;  
                    switch (rule) {
                        case DF_RULE:  // Definição de Função (DF)
                            enterNewScope();
                            break;
                        case B_LS_RULE:  // Começo de um bloco (B)
                            enterNewScope();
                            break;
                        case LS_LS_RULE:
                        case LS_S_RULE: // Fim de um statement, potencialmente fechando um bloco (LS)
                            if (scopeStack.size() > 1)
                                leaveScope();
                            break;
                        case DV_VAR_RULE: // Declaração de variável (DV)
                            currentName = findStringWithInt(identifierTable, oldSecondToken);
                            checkVariableDeclaration(currentName, oldToken);
                            break;
                        case LV_DOT_RULE:
                        case LV_SQUARE_RULE:
                        case LV_IDU_RULE: // Uso de variável (LV)
                            currentName = findStringWithInt(identifierTable, oldSecondToken);
                            checkVariableUsage(currentName);
                            break;
                    }

                } catch(string err) {
                    syntaticalError = true;
                    syntaxError(err);
                    break;
                };

                stack.push(state);
                action = ACTION_TABLE[state][currentToken];
                cont += 1;
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