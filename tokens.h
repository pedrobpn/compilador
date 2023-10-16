#ifndef TOKENS_H
#define TOKENS_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;

#define MAX_CONSTS 10000000

typedef enum {
// palavras reservadas
ARRAY, BOOLEAN, BREAK, CHAR, CONTINUE, DO, ELSE, TOKEN_FALSE, FUNCTION,
IF, INTEGER, OF, STRING, STRUCT, TOKEN_TRUE, TYPE, VAR, WHILE,
// simbolos
COLON, SEMI_COLON, COMMA, EQUALS, LEFT_SQUARE, RIGHT_SQUARE,
LEFT_BRACES, RIGHT_BRACES, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, AND,
OR, LESS_THAN, GREATER_THAN, LESS_OR_EQUAL, GREATER_OR_EQUAL,
NOT_EQUAL, EQUAL_EQUAL, PLUS, PLUS_PLUS, MINUS, MINUS_MINUS, TIMES,
DIVIDE, DOT, NOT,
// tokens regulares
CHARACTER, NUMERAL, STRINGVAL, TOKEN_ID,
// token fim de arquivo
END_OF_FILE,
// token deconhecido
UNKNOWN } t_token;

string keywords[] = {
    "array", "boolean", "break", "char", "continue", "do", "else", "false", "function",
    "if", "integer", "of", "string", "struct", "true", "type", "var", "while"
}; // em ordem alfabética


unordered_map<string, t_token> symbolMap = {
        {"(", LEFT_PARENTHESIS},
        {")", RIGHT_PARENTHESIS},
        {":", COLON},
        {";", SEMI_COLON},
        {",", COMMA},
        {"=", EQUALS},
        {"[", LEFT_SQUARE},
        {"]", RIGHT_SQUARE},
        {"{", LEFT_BRACES},
        {"}", RIGHT_BRACES},
        {"&&", AND},
        {"||", OR},
        {"<", LESS_THAN},
        {">", GREATER_THAN},
        {"<=", LESS_OR_EQUAL},
        {">=", GREATER_OR_EQUAL},
        {"!=", NOT_EQUAL},
        {"==", EQUAL_EQUAL},
        {"+", PLUS},
        {"++", PLUS_PLUS},
        {"-", MINUS},
        {"--", MINUS_MINUS},
        {"*", TIMES},
        {"/", DIVIDE},
        {".", DOT},
        {"!", NOT}
    };

unordered_map<string, int> identifierTable; // Tabela de identificadores
int secondaryToken = 0; // Inicializando o valor do token secundário

// Estrutura para armazenar constantes
typedef struct {
    int type; // 0-char, 1-int, 2-string
    union {
        char cVal;
        int nVal;
        char *sVal;
    } value;
} t_const;

t_const vConsts[MAX_CONSTS];
int nNumConsts = 0;

int contInputChar = 0;

std::ifstream inputFile;

int addCharConst(char c) {
    t_const constant;
    constant.type = 0;
    constant.value.cVal = c;
    vConsts[nNumConsts++] = constant;
    return nNumConsts - 1;
}

int addIntConst(int n) {
    t_const constant;
    constant.type = 1;
    constant.value.nVal = n;
    vConsts[nNumConsts++] = constant;
    return nNumConsts - 1;
}

int addStringConst(char *s) {
    t_const constant;
    constant.type = 2;
    constant.value.sVal = s;
    vConsts[nNumConsts++] = constant;
    return nNumConsts - 1;
}

// Funções para recuperação do valor de uma constante
char getCharConst(int n) {
    return vConsts[n].value.cVal;
}

int getIntConst(int n) {
    return vConsts[n].value.nVal;
}

char *getStringConst(int n) {
    return vConsts[n].value.sVal;
}


bool isAlphanumeric(char c) {
    string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
    return (alphabet.find(c) != string::npos);
}

bool isNumeral(char c) {
    string numeral = "0123456789"; 
    return (numeral.find(c) != string::npos);
}

bool isSpace(char c) {
    char space[] = { '\n', '\r', '\f', '\v', '\t', ' ' };
    for (char i : space) 
        if ( c == i )
            return true;
    return false;
}

bool isOtherUsedChar(char c) {
    char otherChars[] = { '(', ')', ':', ';', ',', '[', ']', '{', '}', '*', '/', '.'};
    for (char i : otherChars) 
        if ( c == i )
            return true;
    return false;
}


t_token searchKeyword(const string& name) {

    auto it = symbolMap.find(name);
    if (it != symbolMap.end()) {
        return it->second;
    }

    int low = 0;
    int high = sizeof(keywords) / sizeof(keywords[0]) - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (keywords[mid] == name) {
            return static_cast<t_token>(mid);
        } else if (keywords[mid] < name) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return TOKEN_ID;
}
int secondToken;

int searchName(const string& name) {
    auto it = identifierTable.find(name);
    if (it != identifierTable.end()) {
        return it->second;
    } else {
        identifierTable[name] = secondaryToken++;
        return secondaryToken - 1;
    }
}


char readChar() {
    contInputChar++;
    return inputFile.get();
}


char nextChar;
bool lexicalError = false;
vector<pair<char,int>> vecLexError;
t_token lookahead;
bool hasLookahead = false;
std::string lastIdentifierValue;
char lastCharacterValue;
std::string lastStringValue;
int lastNumeralValue;


t_token nextToken() {
    
    if (hasLookahead) {
        hasLookahead = false;
        return lookahead;
    }

    t_token token;

    while (isSpace(nextChar) && !(inputFile.eof())) {
        nextChar = readChar();
        // cout << "Jumping Spaces --- Cont: " << contInputChar << " - nextChar = " << nextChar << endl;
    };
    // cout << " Depois de pular espacos --- Cont: " << contInputChar << " - nextChar = " << nextChar << endl;
    cout << " Cont: " << contInputChar << " - nextChar = " << nextChar << " - ";

    
    if (inputFile.eof())
        return END_OF_FILE; // RETURN EOF -----------------------------------------------------------------------------------------------------------------

    if (isAlphanumeric(nextChar)) {// se for alfa, pode ser tanto um identificador (ex:"a") ou o inicio de uma keyword (ex: "array")
        // leio toda a string e procuro (searchKeyword)
        // se for identificador, atribuo token secundário
        string word;

        do {
            // cout << "loop alphanum: " << contInputChar << "- nextChar = " << nextChar << endl;
            word.push_back(nextChar);
            nextChar = readChar();
        } while (isAlphanumeric(nextChar) || nextChar == '_');

        // Armazenar o valor do identificador na variável global
        lastIdentifierValue = word;

        // cout << "Depois do alpha: " << contInputChar << " - nextChar = " << nextChar << endl;

        token = searchKeyword(word);
        if (token == TOKEN_ID) {
            secondToken = searchName(word);
        }

    } else if (isNumeral(nextChar)) { // se for um digito, é um literal numerico, e nesse caso temos que atribuir token secundario e colocar no vetor vConst
        string numStr = "";

        do {
            numStr.push_back(nextChar);
            nextChar = readChar();
        } while ((isNumeral(nextChar)));

        int numInt = stoi(numStr);// transformando string em int

        // Armazenar o valor numeral na variável global
        lastNumeralValue = numInt;
        // cout << "Last Numeral Value: " << lastNumeralValue << endl;
        token = NUMERAL;
        secondToken = addIntConst(numInt);

    } else if (nextChar == '\"') {// se começar com " , é um literal string, temos que percorrer ate encontrar outro " e atribuir token secundario e colocar no vetor vConst
        string str = "";

        nextChar = readChar();

        while (nextChar != '\"' && nextChar != EOF) {
            str.push_back(nextChar);
            nextChar = readChar();
        };
        // cout << "Str String literal = " << str << endl;

        // Armazenar o valor da string na variável global
        lastStringValue = str;


        if (nextChar == EOF) {
            token = UNKNOWN;
            lexicalError = true;
            vecLexError.push_back(make_pair(nextChar,contInputChar));
        } else {
            token = STRINGVAL;
            char *strChar = &str[0]; // transformando string em char*
            secondToken = addStringConst(strChar);
        }
        nextChar = readChar();

    } else if (nextChar == '\'') {// se começar com ' é literal char, lemos o char e devemos atribuir token secundario e colocar no vetor vConst
        nextChar = readChar();

        char chr = nextChar; // armazeno o char que está entre ' '
        
        // Armazenar o valor do caractere na variável global
        lastCharacterValue = chr;
        cout << "lastCharacterValue: " << chr << endl;

        nextChar = readChar(); // leio o segundo '

        if (nextChar != '\'') {
            cout << "Esperado \' depois de char" << endl;
            lexicalError = true;
            vecLexError.push_back(make_pair(nextChar,contInputChar));
        }

        token = CHARACTER;
        secondToken = addCharConst(chr);  
        nextChar = readChar();



    // obs: a manipulação do vetor vConst é feita pelas funções addCharConst, etc.. 
    } else {  // caso contrário, será um símbolo, e devemos abrir em casos de símbolos de 1 caractere ou 2
        // CASOS        
        if (isOtherUsedChar(nextChar)) {
            token = symbolMap[std::string(1,nextChar)];
            nextChar = readChar();
        } else {
            
            switch (nextChar) {
                case '=':
                    nextChar = readChar();

                    if (nextChar == '=') {
                        token = EQUAL_EQUAL;
                        nextChar = readChar();
                    } else {
                        token = EQUALS;
                    }
                    break;
                case '<':
                    nextChar = readChar();

                    if (nextChar == '=') {
                        token = LESS_OR_EQUAL;
                        nextChar = readChar();
                    } else {
                        token = LESS_THAN;
                    }
                    break;
                case '>':
                    nextChar = readChar();

                    if (nextChar == '=') {
                        token = GREATER_OR_EQUAL;
                        nextChar = readChar();
                    } else {
                        token = GREATER_THAN;
                    }
                    break;
                case '!':
                    nextChar = readChar();

                    if (nextChar == '=') {
                        token = NOT_EQUAL;
                        nextChar = readChar();
                    } else {
                        token = NOT;
                    }
                    break;
                case '+':
                    nextChar = readChar();

                    if (nextChar == '+') {
                        token = PLUS_PLUS;
                        nextChar = readChar();
                    } else {
                        token = PLUS;
                    }
                    break;
                case '-':
                    nextChar = readChar();

                    if (nextChar == '-') {
                        token = MINUS_MINUS;
                        nextChar = readChar();
                    } else {
                        token = MINUS;
                    }
                    break;
                case '&':
                    nextChar = readChar();

                    if (nextChar == '&') {
                        token = AND;
                        nextChar = readChar();
                    } else {
                        token = UNKNOWN;
                        lexicalError = true;
                        vecLexError.push_back(make_pair(nextChar,contInputChar));
                    }
                    break;
                case '|':
                    nextChar = readChar();

                    if (nextChar == '|') {
                        token = OR;
                        nextChar = readChar();
                    } else {
                        token = UNKNOWN;
                        lexicalError = true;
                        vecLexError.push_back(make_pair(nextChar,contInputChar));
                    }
                    break;
                default:
                    token = UNKNOWN;
                    lexicalError = true;
                    vecLexError.push_back(make_pair(nextChar,contInputChar));
            }
        }
    }
    cout << "Token: " << token << endl;
    return token;
}

t_token lookaheadToken() {
    if (!hasLookahead) {
        lookahead = nextToken();
        hasLookahead = true;
    }
    return lookahead;
}

std::string getIdentifierValue() {
    return lastIdentifierValue;
}

char getCharacterValue() {
    return lastCharacterValue;
}

std::string getStringValue() {
    return lastStringValue;
}

int getNumeralValue() {
    return lastNumeralValue;
}

void initializeLexer(const std::string& filename) {
    // Recebe o input: código fonte
    inputFile.open(filename);
    if (inputFile.fail()) {
        throw std::runtime_error("Error opening file");
    }
    nextChar = readChar();
}

void finalizeLexer() {
    inputFile.close();
}


int old_main() {

    t_token t;
    // recebe o input: código fonte
    inputFile.open("input.txt");

    if (inputFile.fail()) {
        cout << "Error opening file";
        return 1;
    }

    nextChar = readChar();
    // faz um loop percorrendo cada caractere
    while (!(inputFile.eof())) {
        cout << "Cont: " << contInputChar << " - nextChar = " << nextChar << endl;
        //começo pulando os espaços ate o prox caractere
        // nextChar = readChar();
        if (nextChar != EOF) {
            t = nextToken();
            cout << "Token = " << t << endl << endl;
        }
    };
    
    if (lexicalError) {
        cout << endl << " ---------- Houve erro Lexico ---------- " << endl;
        for (pair<char,int> x :vecLexError)
            cout << "Char error = " << x.first << " - Reading order = " << x.second << endl;
        
    } else   
        cout << endl << " ---------- Nao houve erro Lexico ---------- " << endl;
    
    return 0;
}

#endif