#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
    This section is designated solely for storing and locating operator names given the operator string.
*/
struct operators {
    char *key;          //symbol
    char *value;        //symbol text
};
typedef struct operators operators;
operators *hashTable[79];

int hash(char *symbol) {
    int hashVal = 0;
    for (int i = 0; symbol[i] != '\0'; symbol++)
      hashVal = (int)symbol[i] + 13 * hashVal;
    return hashVal % 79;
}
operators *lookup(char *symbol) {
    int hashVal = hash(symbol);
    while (hashTable[hashVal] != NULL) {
        if(strcmp(hashTable[hashVal]->key, symbol) == 0) {
            return hashTable[hashVal];
        }
        ++hashVal;
        hashVal %= 79;
    }
    return NULL;
}
void insert(char *key, char *value) {
    operators *op = (operators*)malloc(sizeof(operators));
    op->key = key;
    op->value = value;
    int hashVal = hash(key);
    while(hashTable[hashVal] != NULL) {
        ++hashVal;
        hashVal %= 79;
    }
    hashTable[hashVal] = op;
    return;
}

char* symbols[41] = {"!", "&", "(", ")", "*", "+", ",", "-", ".", "/", ":", "<", "=", ">", "?", "&&", "++", "--", "[", "]", "^", "!=", "%=", "&=", "*=", "+=", "-=", "->", "/=", "<<", "<=", "==", ">=", "|", ">>", "~", "^=", "<<=", "|=", ">>=", "||"};
char* definitions[41] = {"negate", "AND/address operator", "left parenthesis", "right parenthesis", "multiply/dereference operator", "addition", "comma", "minus/subtract operator", "structure member", "division", "conditional false",
                        "less than test", "assignment", "greater than test", "conditional true", "logical AND", "increment", "decrement", "left bracket", "right bracket", "bitwise XOR", "inequality test", "mod equals", "bitwise AND equals", "times equals", 
                        "plus equals", "minus equals", "structure pointer", "divide equals", "shift left", "less than or equal test",  "equality test", "greater than or equal test", "bitwise OR", "shift right", 
                        "1s complement", "bitwise XOR equals", "shift left equals", "bitwise OR equals", "shift right equals", "logical OR"};

int keycheck(char* inStr) {
    if(strcmp(inStr, "sizeof") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
    To increase modularity and reduce redundancy, we use a helper function
    to print out our tokens in the desired format.
*/
void printFormat(char* endToken, char endFlag) {
    if(endToken == "" || endFlag == '\0') {
        return;
    }

    switch (endFlag)
    {
        case 'w':
            printf("word: %s\n", endToken);
            break;
        case 'd':
            printf("decimal integer: %s\n", endToken);
            break;
        case 'o':
            printf("octal integer: %s\n", endToken);
            break;
        case 'h':
            printf("hexadecimal integer: %s\n", endToken);
            break;
        case 'f':
            printf("floating point: %s\n", endToken);
            break;
        case 'e':
            printf("floating point: %s\n", endToken);
            break;
        case 'p':
            printf("%s: %s\n", lookup(endToken)->value, endToken);
            break;
        case 'k':
            printf("operator: %s\n", endToken);
            break;
    }
}

int main(int argc, char** argv) {
    if(argv[1][0] == '\0') {
        printf("ERROR: EMPTY STRING");
        return -1;
    } else if(argc > 2) {
        printf("ERROR: TOO MANY ARGUMENTS");
        return -1;
    }
    for(int i = 0; i < 41; i++) {
        insert(symbols[i], definitions[i]);
    }
    size_t inputLength = strlen(argv[1]);
    char currChar;
    char *currToken = malloc(inputLength * sizeof(argv[1])); //make sure we have all the space we need--we can never have a token larger than our original argument!
    currToken[0] = '\0';
    int currIndex = 0;
    char flag;

    for(int i = 0; i <= inputLength; i++) {
        //printf("[%c] %s\n", flag, currToken);
        currChar = argv[1][i];    //This just makes our life easier

        /*
        We can use a blanket statement for white space/string delimeter and then handle the print 
        with our helper function to reduce redundancy, depending on whether we have something 
        to print or not.
        */
        if(isspace(currChar) || currChar == '\0') {
            if(keycheck(currToken) == 1) {
                printFormat(currToken, 'k');
            } else {
                printFormat(currToken, flag);
            }
            flag = '\0';
            currToken[0] = '\0';        //Though this method makes our code more cluttered, we are trading readibility for the
            currIndex = 0;              //best BigO--str(n)cat, strcpy, and strlen are all O(n), using them would mean >= O(n^2)
            continue;
        }

        /*
        If the begining of our token has a key character (i.e. 0) and if we havent started
        a new token, this is where we can flag our token as we sit fit.

        The idea behind this method is essentially "guess and check"--we guess what the best 
        way to handle our token is now and then dynamically check that the token fits within
        the guidelines of whatever flag we placed it in.
        */
        if(currToken[0] == '\0') {
            if(isalpha(currChar)) {
                flag = 'w';
            } else if(isdigit(currChar)) {
                int firstDig = currChar - '0';
                flag = 'd';
                if(firstDig == 0) {
                    flag = 'o';
                    if(inputLength > 2 && (argv[1][i+1] == 'x' ||  argv[1][i+1] == 'X')) {
                        flag = 'h';
                    }
                }
            } else {
                char temp[2];
                temp[0] = currChar;
                temp[1] = '\0';
                if(lookup(temp) != NULL) {
                    flag = 'p';
                }
            }
        }

        /*
        Now that we have our flags, we have an idea of how we should treat this token,
        when to stop the token, and we can set up edge cases--maybe our original guess
        was wrong--we can change flags relative to what we once thought this was to give
        more context or "memory".

        In essence, this is a very simple state machine.
        */
        switch (flag)
        {
            //if we think its a word
            case 'w':
                if(isalnum(currChar)) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else {
                    if(strcmp(currToken, "sizeof") == 0) {
                        printFormat(currToken, 'k');
                    } else {
                        printFormat(currToken, flag);
                    }
                }
                break;
            //if we think its a decimal
            case 'd':
                if(isdigit(currChar)) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else if(currChar == '.') {
                        if(argv[1][i+1] != '\0' && isdigit(argv[1][i+1])) {
                            flag = 'f';
                            currToken[currIndex] = currChar;
                            currIndex++;
                            currToken[currIndex] = '\0';
                            continue;
                        } else {
                            printFormat(currToken, flag);
                        }
                } else if(isalpha(currChar)) {
                    printFormat(currToken, flag);
                }
                break;

            //if we think its hexadecimal
            case 'h':
                //We have to do this check explicitly because isxdigit will mark 'x' as not hexadecimal
                if(isxdigit(currChar)) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else if(currIndex == 1 && (currChar == 'x' || currChar == 'X')) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else {
                    if(currIndex == 2) {
                        char temp[2];
                        temp[0] = currToken[0];
                        temp[1] = '\0';
                        currToken[0] = '\0';
                        currIndex = 0;
                        printFormat(temp, 'd');
                        i = i - 2;
                        continue;
                    } else {
                        printFormat(currToken, flag);
                    }
                }
                break;

            //if we think its octal
            case 'o':
                    if(isdigit(currChar)) {
                        int currInt = currChar - '0';
                        if(currInt < 8 && currInt >= 0) {
                            currToken[currIndex] = currChar;
                            currIndex++;
                            currToken[currIndex] = '\0';
                            continue;
                        } else {
                            flag = 'd';
                            i--;
                            continue;
                        }
                    } else {
                        if(currChar == '.') {
                            if(argv[1][i+1] != '\0' && isdigit(argv[1][i+1])) {
                                flag = 'f';
                                currToken[currIndex] = currChar;
                                currIndex++;
                                currToken[currIndex] = '\0';
                                continue;
                            }
                        } else {
                            if (currIndex > 1) {
                                printFormat(currToken, flag);
                            } else {
                                printFormat(currToken, 'd');
                            }
                        }
                    }
                break;

            //if we think its floating point
            case 'f':
                if(isdigit(currChar)) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else {
                    if((currChar == 'e' || currChar == 'E') && argv[1][i+1] != '\0') {
                        flag = 'e';
                        if(argv[1][i+1] == '+' || argv[1][i+1] == '-') {
                            if(argv[1][i+2] != '\0' && isdigit(argv[1][i+2])) {
                                currToken[currIndex] = currChar;
                                currIndex++;
                                currToken[currIndex] = argv[1][i+1];
                                currIndex++;
                                currToken[currIndex] = '\0';
                                i++;
                                continue;
                            } else {
                                printFormat(currToken, flag);
                            }
                        } else if(argv[1][i+1] != '\0' && !isdigit(argv[1][i+1])) {
                            printFormat(currToken, 'f');
                        }
                    } else {        //weve hit a word, so lets print our float and deal with that later
                        printFormat(currToken, flag);
                    }
                }
                break;

            //floating points with an "E" must be handled differently
            case 'e':
                if(isdigit(currChar)) {
                    currToken[currIndex] = currChar;
                    currIndex++;
                    currToken[currIndex] = '\0';
                    continue;
                } else {
                    printFormat(currToken, flag);
                }
                break;

            //If we think its an operator
            case 'p':
                currToken[currIndex] = currChar;
                currIndex++;
                currToken[currIndex] = '\0';
                if(lookup(currToken) == NULL) {
                    currIndex--;
                    currToken[currIndex] = '\0';
                    printFormat(currToken, flag);
                } else {
                    continue;
                }
                break;
        }
        flag = '\0';
        currToken[0] = '\0';
        currIndex = 0;
        i--;
    }
    free(currToken);
    for(int i = 0; i < 79; i++) {
        free(hashTable[i]);
    }
    return 1;
}
