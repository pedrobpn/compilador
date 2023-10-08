#include <iostream>
#include <vector>
#include "tokens.h"

enum ASTNodeType {
    NODE_P, NODE_LDE, NODE_DE, NODE_T, NODE_DT, NODE_DC, NODE_DF, 
    NODE_LP, NODE_B, NODE_LDV, NODE_LS, NODE_DV, NODE_LI, NODE_S, 
    NODE_U, NODE_M, NODE_E, NODE_L, NODE_R, NODE_Y, NODE_F, NODE_LE, 
    NODE_LV, NODE_IDD, NODE_IDU, NODE_ID, NODE_TRUE, NODE_FALSE, NODE_CHR, 
    NODE_STR, NODE_NUM, NODE_NB, NODE_MF, NODE_MC, NODE_NF, NODE_MT, 
    NODE_ME, NODE_MW
};


struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<ASTNode*> children;

    // Construtor
    ASTNode(ASTNodeType nodeType) : type(nodeType) {}

    // Adicionar um filho a este nodo
    void addChild(ASTNode* child) {
        children.push_back(child);
    }
};

ASTNode* createNode(ASTNodeType type) {
    ASTNode* node = new ASTNode(type);
    node->type = type;
    return node;
}

ASTNode* P_prime(); // P'
ASTNode* P();
ASTNode* LDE();
ASTNode* DE();
ASTNode* T();
ASTNode* DT();
ASTNode* DC();
ASTNode* DF();
ASTNode* LP();
ASTNode* B();
ASTNode* LDV();
ASTNode* LS();
ASTNode* DV();
ASTNode* LI();
ASTNode* S();
ASTNode* U();
ASTNode* M();
ASTNode* E();
ASTNode* L();
ASTNode* R();
ASTNode* Y();
ASTNode* F();
ASTNode* LE();
ASTNode* LV();
ASTNode* IDD();
ASTNode* IDU();
ASTNode* ID();
ASTNode* TRUE();
ASTNode* FALSE();
ASTNode* CHR();
ASTNode* STR();
ASTNode* NUM();
ASTNode* NB();
ASTNode* MF();
ASTNode* MC();
ASTNode* NF();
ASTNode* MT();
ASTNode* ME();
ASTNode* MW();

t_token currentToken;

void syntaxError(const std::string& errMsg) {
    std::cerr << "Erro sintático: " << errMsg << std::endl;
    exit(EXIT_FAILURE);
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

ASTNode* P() {
    ASTNode* node = createNode(NODE_P);
    node->children.push_back(LDE());
    if (currentToken != END_OF_FILE) {
        syntaxError("Esperado fim do arquivo");
    }
    return node;
}

ASTNode* LDE() {
    ASTNode* node = createNode(NODE_LDE);

    while (currentToken == TYPE || currentToken == FUNCTION) {
        node->children.push_back(DE());
    }

    return node;
}

ASTNode* DE() {
    ASTNode* node = nullptr;
    
    if (currentToken == TYPE) {
        node = DT();
    } else if (currentToken == FUNCTION) {
        node = DF();
    } else {
        syntaxError("Esperado 'type' ou 'function'");
    }

    return node;
}

ASTNode* T() {
    ASTNode* node = createNode(NODE_T);

    switch (currentToken) {
        case INTEGER:
            match(INTEGER);
            break;
        case CHAR:
            match(CHAR);
            break;
        case BOOLEAN:
            match(BOOLEAN);
            break;
        case STRING:
            match(STRING);
            break;
        case TOKEN_ID:
            node->children.push_back(IDU());
            break;
        default:
            syntaxError("Token inesperado em T");
            delete node;  // Libera a memória para evitar vazamento
            return nullptr;
    }

    return node;
}

ASTNode* DT() {
    ASTNode* node = createNode(NODE_DT);

    if (currentToken != TYPE) {
        syntaxError("Esperado 'type' em DT");
        delete node;
        return nullptr;
    }

    match(TYPE); // Consumimos o token 'type'

    node->children.push_back(IDD()); // Esperamos um identificador após 'type'

    if (currentToken == EQUALS) {
        match(EQUALS);

        if (currentToken == ARRAY) {
            match(ARRAY);
            match(LEFT_SQUARE);
            node->children.push_back(NUM()); // Adiciona um nó para o número após '['
            match(RIGHT_SQUARE);
            match(OF);
            node->children.push_back(T()); // Esperamos um tipo após 'of'
        } else if (currentToken == STRUCT) {
            match(STRUCT);
            node->children.push_back(NB());
            match(LEFT_BRACES);
            node->children.push_back(DC());
            match(RIGHT_BRACES);
        } else {
            node->children.push_back(T()); // Para produção DT -> 'type' IDD '=' T
        }
    } else {
        syntaxError("Esperado '=' em DT");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* DC() {
    ASTNode* node = createNode(NODE_DC);

    if (currentToken == TYPE || currentToken == FUNCTION || currentToken == VAR || isIdToken(currentToken)) {
        node->children.push_back(LI());
        match(COLON);
        node->children.push_back(T());
        if (currentToken == SEMI_COLON) {
            match(SEMI_COLON);
            node->children.push_back(DC());
        }
    } else {
        syntaxError("Token inesperado em DC");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* DF() {
    ASTNode* node = createNode(NODE_DF);

    if (currentToken != FUNCTION) {
        syntaxError("Esperado 'function' em DF");
        delete node;
        return nullptr;
    }
    match(FUNCTION);

    node->children.push_back(IDD());

    node->children.push_back(NF());

    match(LEFT_PARENTHESIS);
    if (currentToken == TOKEN_ID) {
        node->children.push_back(LP());
    }
    match(RIGHT_PARENTHESIS);

    match(COLON);
    node->children.push_back(T());

    node->children.push_back(MF());

    node->children.push_back(B());

    return node;
}

ASTNode* LP() {
    ASTNode* node = createNode(NODE_LP);

    node->children.push_back(IDD());
    match(COLON);
    node->children.push_back(T());

    while (currentToken == COMMA) {
        match(COMMA);
        node->children.push_back(IDD());
        match(COLON);
        node->children.push_back(T());
    }

    return node;
}

bool isStatementStartToken(t_token token) {
    // Esta função verifica se o token atual é o início de uma sentença/comando. 
    return token == IF || token == WHILE || token == DO || isIdToken(token) || token == BREAK || token == CONTINUE;
}

ASTNode* B() {
    ASTNode* node = createNode(NODE_B);

    match(LEFT_BRACES);

    while (currentToken != RIGHT_BRACES && currentToken != END_OF_FILE) {
        if (currentToken == VAR) {
            node->children.push_back(LDV());
        } else if (isStatementStartToken(currentToken)) {
            node->children.push_back(LS());
        } else {
            syntaxError("Token inesperado em B");
            delete node;
            return nullptr;
        }
    }

    match(RIGHT_BRACES);

    return node;
}

ASTNode* LDV() {
    ASTNode* node = createNode(NODE_LDV);

    node->children.push_back(DV());

    while (currentToken == VAR) {
        node->children.push_back(DV());
    }

    return node;
}

ASTNode* LS() {
    ASTNode* node = createNode(NODE_LS);

    node->children.push_back(S());

    return node;
}

ASTNode* DV() {
    ASTNode* node = createNode(NODE_DV);

    if (currentToken != VAR) {
        syntaxError("Esperado 'var' em DV");
        delete node;
        return nullptr;
    }
    match(VAR);

    node->children.push_back(LI());

    match(COLON);
    node->children.push_back(T());

    match(SEMI_COLON);

    return node;
}

ASTNode* LI() {
    ASTNode* node = createNode(NODE_LI);

    node->children.push_back(IDD());

    while (currentToken == COMMA) {
        match(COMMA);
        node->children.push_back(IDD());
    }

    return node;
}

ASTNode* S() {
    ASTNode* node = createNode(NODE_S);

    switch (currentToken) {
        case IF:
            node->children.push_back(U());
            break;
        case WHILE:
        case DO:
        case BREAK:
        case CONTINUE:
            node->children.push_back(M());
            break;
        case LEFT_BRACES: // Se começar com '{', assume-se que é um bloco
            node->children.push_back(B());
            break;
        case VAR: // Se começar com 'var', assume-se que é uma declaração de variável
            node->children.push_back(DV());
            break;
        default:
            if (isIdToken(currentToken)) {
                node->children.push_back(LV());
                match(EQUALS);
                node->children.push_back(E());
                match(SEMI_COLON);
            } else {
                syntaxError("Token inesperado em S");
                delete node;
                return nullptr;
            }
            break;
    }

    return node;
}

ASTNode* U() {
    ASTNode* node = createNode(NODE_U);

    if (currentToken != IF) {
        syntaxError("Esperado 'if' em U");
        delete node;
        return nullptr;
    }

    match(IF);
    match(LEFT_PARENTHESIS);
    node->children.push_back(E());
    match(RIGHT_PARENTHESIS);

    node->children.push_back(MT());

    if (currentToken == IF || currentToken == WHILE || isStatementStartToken(currentToken)) {
        node->children.push_back(U());
    } else {
        node->children.push_back(M());
        if (currentToken == ELSE) {
            match(ELSE);
            node->children.push_back(ME());
            node->children.push_back(U());
        }
    }

    return node;
}

ASTNode* M() {
    ASTNode* node = createNode(NODE_M);

    switch (currentToken) {
        case IF:
            match(IF);
            match(LEFT_PARENTHESIS);
            node->children.push_back(E());
            match(RIGHT_PARENTHESIS);
            node->children.push_back(MT());
            node->children.push_back(M());
            if (currentToken == ELSE) {
                match(ELSE);
                node->children.push_back(ME());
                node->children.push_back(M());
            }
            break;
        case WHILE:
            match(WHILE);
            node->children.push_back(MW());
            match(LEFT_PARENTHESIS);
            node->children.push_back(E());
            match(RIGHT_PARENTHESIS);
            node->children.push_back(MT());
            node->children.push_back(M());
            break;
        case DO:
            match(DO);
            node->children.push_back(MW());
            node->children.push_back(M());
            match(WHILE);
            match(LEFT_PARENTHESIS);
            node->children.push_back(E());
            match(RIGHT_PARENTHESIS);
            match(SEMI_COLON);
            break;
        case LEFT_BRACES:
            node->children.push_back(NB());
            node->children.push_back(B());
            break;
        default:
            if (isIdToken(currentToken)) {
                node->children.push_back(LV());
                match(EQUALS);
                node->children.push_back(E());
                match(SEMI_COLON);
            } else if (currentToken == BREAK) {
                match(BREAK);
                match(SEMI_COLON);
            } else if (currentToken == CONTINUE) {
                match(CONTINUE);
                match(SEMI_COLON);
            } else {
                syntaxError("Token inesperado em M");
                delete node;
                return nullptr;
            }
            break;
    }

    return node;
}

ASTNode* E() {
    ASTNode* node = createNode(NODE_E);

    node->children.push_back(L());

    while (currentToken == AND || currentToken == OR) {
        if (currentToken == AND) {
            match(AND);
        } else if (currentToken == OR) {
            match(OR);
        }
        node->children.push_back(L());
    }

    return node;
}

ASTNode* L() {
    ASTNode* node = createNode(NODE_L);

    node->children.push_back(R());

    while (currentToken == LESS_THAN || currentToken == GREATER_THAN || currentToken == LESS_OR_EQUAL || 
           currentToken == GREATER_OR_EQUAL || currentToken == EQUAL_EQUAL || currentToken == NOT_EQUAL) {
        switch (currentToken) {
            case LESS_THAN:
                match(LESS_THAN);
                break;
            case GREATER_THAN:
                match(GREATER_THAN);
                break;
            case LESS_OR_EQUAL:
                match(LESS_OR_EQUAL);
                break;
            case GREATER_OR_EQUAL:
                match(GREATER_OR_EQUAL);
                break;
            case EQUAL_EQUAL:
                match(EQUAL_EQUAL);
                break;
            case NOT_EQUAL:
                match(NOT_EQUAL);
                break;
            default:
                syntaxError("Token inesperado em L");
                delete node;
                return nullptr;
        }
        node->children.push_back(R());
    }

    return node;
}

ASTNode* R() {
    ASTNode* node = createNode(NODE_R);

    node->children.push_back(Y());

    while (currentToken == PLUS || currentToken == MINUS) {
        if (currentToken == PLUS) {
            match(PLUS);
        } else if (currentToken == MINUS) {
            match(MINUS);
        }
        node->children.push_back(Y());
    }

    return node;
}

ASTNode* Y() {
    ASTNode* node = createNode(NODE_Y);

    node->children.push_back(F());

    while (currentToken == TIMES || currentToken == DIVIDE) {
        if (currentToken == TIMES) {
            match(TIMES);
        } else if (currentToken == DIVIDE) {
            match(DIVIDE);
        }
        node->children.push_back(F());
    }

    return node;
}

// Função auxiliar hipotética
bool isExpressionStartToken(t_token token) {
    return isIdToken(token) || token == NUMERAL || token == CHARACTER || token == STRINGVAL || token == LEFT_PARENTHESIS;
}

ASTNode* F() {
    ASTNode* node = createNode(NODE_F);

    switch (currentToken) {
        case PLUS_PLUS:
            match(PLUS_PLUS);
            node->children.push_back(LV());
            break;
        case MINUS_MINUS:
            match(MINUS_MINUS);
            node->children.push_back(LV());
            break;
        case LEFT_PARENTHESIS:
            match(LEFT_PARENTHESIS);
            node->children.push_back(E());
            match(RIGHT_PARENTHESIS);
            break;
        case MINUS:
            match(MINUS);
            node->children.push_back(F());
            break;
        case NOT:
            match(NOT);
            node->children.push_back(F());
            break;
        case TOKEN_TRUE:
            match(TOKEN_TRUE);
            break;
        case TOKEN_FALSE:
            match(TOKEN_FALSE);
            break;
        case CHARACTER:
            node->children.push_back(CHR());
            break;
        case STRINGVAL:
            node->children.push_back(STR());
            break;
        case NUMERAL:
            node->children.push_back(NUM());
            break;
        default:
            if (isIdToken(currentToken)) {
                if (lookaheadToken() == LEFT_PARENTHESIS) {
                    node->children.push_back(IDU());
                    match(LEFT_PARENTHESIS);
                    if (isExpressionStartToken(currentToken)) { // Função para verificar o início de uma expressão
                        node->children.push_back(LE());
                    }
                    match(RIGHT_PARENTHESIS);
                } else {
                    node->children.push_back(LV());
                    if (currentToken == PLUS_PLUS || currentToken == MINUS_MINUS) {
                        if (currentToken == PLUS_PLUS) {
                            match(PLUS_PLUS);
                        } else {
                            match(MINUS_MINUS);
                        }
                    }
                }
            } else {
                syntaxError("Token inesperado em F");
                delete node;
                return nullptr;
            }
            break;
    }

    return node;
}


ASTNode* LE() {
    ASTNode* node = createNode(NODE_LE);

    node->children.push_back(E());

    while (currentToken == COMMA) {
        match(COMMA);
        node->children.push_back(E());
    }

    return node;
}

ASTNode* LV() {
    ASTNode* node = createNode(NODE_LV);

    node->children.push_back(IDU());

    while (currentToken == DOT || currentToken == LEFT_SQUARE) {
        if (currentToken == DOT) {
            match(DOT);
            node->children.push_back(ID());
        } else if (currentToken == LEFT_SQUARE) {
            match(LEFT_SQUARE);
            node->children.push_back(E());
            match(RIGHT_SQUARE);
        }
    }

    return node;
}

ASTNode* IDD() {
    ASTNode* node = createNode(NODE_IDD);

    if (currentToken == TOKEN_ID) {

        node->value = getIdentifierValue(); // Função que retorna o valor do identificador atual.
        match(TOKEN_ID);
    } else {
        syntaxError("Esperado identificador em IDD");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* IDU() {
    ASTNode* node = createNode(NODE_IDU);

    if (currentToken == TOKEN_ID) {
        node->value = getIdentifierValue(); // Função que retorna o valor do identificador atual.
        match(TOKEN_ID);
    } else {
        syntaxError("Esperado identificador em IDU");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* ID() {
    ASTNode* node = createNode(NODE_ID);

    if (currentToken == TOKEN_ID) {

        node->value = getIdentifierValue(); // Função ue retorna o valor do identificador atual.
        match(TOKEN_ID);
    } else {
        syntaxError("Esperado identificador em ID");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* TRUE() {
    ASTNode* node = createNode(NODE_TRUE);

    if (currentToken == TOKEN_TRUE) {
        match(TOKEN_TRUE);
    } else {
        syntaxError("Esperado 'true' em TRUE");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* FALSE() {
    ASTNode* node = createNode(NODE_FALSE);

    if (currentToken == TOKEN_FALSE) {
        match(TOKEN_FALSE);
    } else {
        syntaxError("Esperado 'false' em FALSE");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* CHR() {
    ASTNode* node = createNode(NODE_CHR);

    if (currentToken == CHARACTER) {
        node->value = getCharacterValue(); // Função que retorna o valor do caractere atual.
        match(CHARACTER);
    } else {
        syntaxError("Esperado literal de caractere em CHR");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* STR() {
    ASTNode* node = createNode(NODE_STR);

    if (currentToken == STRINGVAL) {
        node->value = getStringValue(); // Função que retorna o valor da string atual.
        match(STRINGVAL);
    } else {
        syntaxError("Esperado literal de string em STR");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* NUM() {
    ASTNode* node = createNode(NODE_NUM);

    if (currentToken == NUMERAL) {
        node->value = to_string(getNumeralValue()); // Função que retorna o valor numérico atual.
        match(NUMERAL);
    } else {
        syntaxError("Esperado literal numérico em NUM");
        delete node;
        return nullptr;
    }

    return node;
}

ASTNode* NB() {
    // Assumindo que NB é apenas uma marcação sintática e não tem uma representação direta na entrada,
    // a função de análise apenas cria um nó vazio.
    return createNode(NODE_NB);
}

ASTNode* MF() {
    return createNode(NODE_MF);
}

ASTNode* MC() {
    return createNode(NODE_MC);
}

ASTNode* NF() {
    return createNode(NODE_NF);
}

ASTNode* MT() {
    return createNode(NODE_MT);
}

ASTNode* ME() {
    return createNode(NODE_ME);
}

ASTNode* MW() {
    return createNode(NODE_MW);
}

std::string ASTNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case NODE_P: return "P";
        case NODE_LDE: return "LDE";
        case NODE_DE: return "DE";
        case NODE_T: return "T";
        case NODE_DT: return "DT";
        case NODE_DC: return "DC";
        case NODE_DF: return "DF";
        case NODE_LP: return "LP";
        case NODE_B: return "B";
        case NODE_LDV: return "LDV";
        case NODE_LS: return "LS";
        case NODE_DV: return "DV";
        case NODE_LI: return "LI";
        case NODE_S: return "S";
        case NODE_U: return "U";
        case NODE_M: return "M";
        case NODE_E: return "E";
        case NODE_L: return "L";
        case NODE_R: return "R";
        case NODE_Y: return "Y";
        case NODE_F: return "F";
        case NODE_LE: return "LE";
        case NODE_LV: return "LV";
        case NODE_IDD: return "IDD";
        case NODE_IDU: return "IDU";
        case NODE_ID: return "ID";
        case NODE_TRUE: return "TRUE";
        case NODE_FALSE: return "FALSE";
        case NODE_CHR: return "CHR";
        case NODE_STR: return "STR";
        case NODE_NUM: return "NUM";
        case NODE_NB: return "NB";
        case NODE_MF: return "MF";
        case NODE_MC: return "MC";
        case NODE_NF: return "NF";
        case NODE_MT: return "MT";
        case NODE_ME: return "ME";
        case NODE_MW: return "MW";
        default: return "UNKNOWN";
    }
}


void printAST(ASTNode* node, int depth = 0) {
    if (!node) return;

    // Para melhor visualização, indente com base na profundidade do nó.
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";  // Use dois espaços para indentação.
    }

    // Imprima o tipo e o valor do nó.
    std::cout << ASTNodeTypeToString(node->type);  // Converta o tipo enum para string.
    if (!node->value.empty()) {
        std::cout << ": " << node->value;
    }
    std::cout << std::endl;

    // Chame recursivamente para cada filho.
    for (ASTNode* child : node->children) {
        printAST(child, depth + 1);
    }
}


int main() {
    try {
        initializeLexer("input.txt");

        currentToken = nextToken();  // Inicializa o currentToken com o primeiro token do arquivo
        // cout << currentToken;
        ASTNode* rootNode = P(); // Chame a função do parser que inicia a análise, geralmente a função correspondente ao símbolo inicial da gramática.

        std::cout <<"\n\nÁrvore: " << std::endl;
        printAST(rootNode);
        
        finalizeLexer();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
