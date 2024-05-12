#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_RULES 10
#define MAX_SYMBOLS 20

// Example grammar
char *grammar[MAX_RULES] = {
    "E=TX",
    "X=+TX",
    "X=#",    // # represents epsilon
    "T=FY",
    "Y=*FY",
    "Y=#",    // epsilon
    "F=(E)",
    "F=i"
};
int num_rules = 8;

// Function to check if a symbol is non-terminal
bool is_non_terminal(char symbol) {
    return symbol >= 'A' && symbol <= 'Z';
}

// Function to check if a symbol is terminal or epsilon
bool is_terminal(char symbol) {
    return !is_non_terminal(symbol) && symbol != '#';
}

// Function to add to a set avoiding duplicates
void add_to_set(char set[], char symbol) {
    int len = strlen(set);
    for (int i = 0; i < len; i++) {
        if (set[i] == symbol)
            return;
    }
    set[len] = symbol;
    set[len + 1] = '\0';
}

// Function to compute FIRST set for a given non-terminal
void compute_first(char first[], char non_terminal) {
    bool added;
    do {
        added = false;
        for (int i = 0; i < num_rules; i++) {
            if (grammar[i][0] == non_terminal) {
                char *production = grammar[i] + 2; // Skip over 'X='
                if (is_terminal(production[0]) || production[0] == '#') {
                    if (strchr(first, production[0]) == NULL) {
                        add_to_set(first, production[0]);
                        added = true;
                    }
                } else if (is_non_terminal(production[0])) {
                    char temp_first[MAX_SYMBOLS] = "";
                    compute_first(temp_first, production[0]);
                    for (int j = 0; j < strlen(temp_first); j++) {
                        if (temp_first[j] != '#' && strchr(first, temp_first[j]) == NULL) {
                            add_to_set(first, temp_first[j]);
                            added = true;
                        }
                    }
                }
            }
        }
    } while (added);
}

// Function to compute FOLLOW set for a given non-terminal
void compute_follow(char follow[], char non_terminal, char start_symbol) {
    // Add '$' to the FOLLOW set of the start symbol
    if (non_terminal == start_symbol) {
        add_to_set(follow, '$');
    }

    for (int i = 0; i < num_rules; i++) {
        char *production = grammar[i] + 2; // Skip over 'X='
        int len = strlen(production);
        for (int j = 0; j < len; j++) {
            if (production[j] == non_terminal) {
                // Check the next symbol in the production
                if (j + 1 < len && is_terminal(production[j + 1])) {
                    add_to_set(follow, production[j + 1]);
                } else if (j + 1 < len && is_non_terminal(production[j + 1])) {
                    char temp_first[MAX_SYMBOLS] = "";
                    compute_first(temp_first, production[j + 1]);
                    for (int k = 0; k < strlen(temp_first); k++) {
                        if (temp_first[k] != '#') {
                            add_to_set(follow, temp_first[k]);
                        }
                    }
                    if (strchr(temp_first, '#') != NULL) {
                        if (j + 2 < len) {
                            char next_first[MAX_SYMBOLS] = "";
                            compute_first(next_first, production[j + 2]);
                            for (int m = 0; m < strlen(next_first); m++) {
                                if (next_first[m] != '#') {
                                    add_to_set(follow, next_first[m]);
                                }
                            }
                        } else {
                            char temp_follow[MAX_SYMBOLS] = "";
                            compute_follow(temp_follow, grammar[i][0], start_symbol);
                            for (int n = 0; n < strlen(temp_follow); n++) {
                                add_to_set(follow, temp_follow[n]);
                            }
                        }
                    }
                } else if (j + 1 == len) {
                    char temp_follow[MAX_SYMBOLS] = "";
                    compute_follow(temp_follow, grammar[i][0], start_symbol);
                    for (int l = 0; l < strlen(temp_follow); l++) {
                        add_to_set(follow, temp_follow[l]);
                    }
                }
            }
        }
    }
}

int main() {
    char non_terminals[] = "EXTYF";  // List of non-terminals in the grammar
    char first[MAX_SYMBOLS] = "";
    char follow[MAX_SYMBOLS] = "";

    for (int i = 0; non_terminals[i] != '\0'; i++) {
        memset(first, 0, sizeof(first));  // Clear the FIRST set
        compute_first(first, non_terminals[i]);
        printf("FIRST(%c) = {%s}\n", non_terminals[i], first);

        memset(follow, 0, sizeof(follow));  // Clear the FOLLOW set
        compute_follow(follow, non_terminals[i], 'E');
        printf("FOLLOW(%c) = {%s}\n", non_terminals[i], follow);
    }

    return 0;
}






slr========================




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char stack[100];
    int top;
} Stack;

void push(Stack *s, char symbol) {
    s->stack[++s->top] = symbol;
}

char pop(Stack *s) {
    if (s->top == -1) return -1;
    return s->stack[s->top--];
}

char peek(Stack *s) {
    if (s->top == -1) return -1;
    return s->stack[s->top];
}

void SLR_parsing(const char *input) {
    Stack s;
    s.top = -1;
    push(&s, '0'); // push initial state
   
    int i = 0;
    char action;
    int state, len = strlen(input);

    printf("Input: %s\n", input);
    while (i < len) {
        state = peek(&s) - '0'; // get the top state from the stack
        switch (input[i]) {
            case 'i': // id found
                if (state == 0 || state == 4) {
                    push(&s, 'i');
                    push(&s, '5'); // shift to state 5
                }
                break;
            case '+':
                if (state == 6) {
                    pop(&s); // reduce T -> F
                    pop(&s);
                    state = peek(&s) - '0';
                    push(&s, 'T');
                    if (state == 0 || state == 4) push(&s, '2'); // goto state 2
                } else if (state == 2) {
                    push(&s, '+');
                    push(&s, '4'); // shift to state 4
                }
                break;
            case '*':
                if (state == 7) {
                    pop(&s); // reduce F -> id
                    pop(&s);
                    state = peek(&s) - '0';
                    push(&s, 'F');
                    if (state == 2 || state == 5) push(&s, '7'); // goto state 7
                } else if (state == 5) {
                    push(&s, '*');
                    push(&s, '6'); // shift to state 6
                }
                break;
            case '$':
                if (state == 1) {
                    printf("Parsing completed successfully.\n");
                    return;
                } else {
                    printf("Syntax error.\n");
                    return;
                }
            default:
                printf("Syntax error at %c\n", input[i]);
                return;
        }
        i++;
    }
}

int main() {
    // Simulate the input: id * id + id
    SLR_parsing("i*i+i$");
    return 0;
}





syntax tree==========================


AKHILA KUMAR (RA2111026010397) <aa9270@srmist.edu.in>
Thu, Apr 18, 6:28 AM
to me

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    char operator;  // Use '\0' for non-operators
    int value;
    struct Node *left;
    struct Node *right;
} Node;

// Function to create a new node
Node* newNode(int value, char operator) {
    Node *temp = (Node*) malloc(sizeof(Node));
    temp->value = value;
    temp->operator = operator;
    temp->left = temp->right = NULL;
    return temp;
}

// Function to evaluate the syntax tree
int evaluate(Node *root) {
    if (root == NULL) return 0;
    // If it is a leaf node (i.e., integer value)
    if (root->left == NULL && root->right == NULL)
        return root->value;

    // Evaluate left subtree
    int left_val = evaluate(root->left);

    // Evaluate right subtree
    int right_val = evaluate(root->right);

    // Check which operator to apply
    if (root->operator == '+')
        return left_val + right_val;
    else if (root->operator == '*')
        return left_val * right_val;

    return 0; // default return value in case of error
}

// Function to print the syntax tree
void printTree(Node *root, int space) {
    if (root == NULL) return;

    // Increase distance between levels
    space += 10;

    // Process right child first
    printTree(root->right, space);

    // Print current node after space
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    if (root->operator != '\0')
        printf("%c\n", root->operator);
    else
        printf("%d\n", root->value);

    // Process left child
    printTree(root->left, space);
}

int main() {
    /* Manually constructing the tree:
     *         *
     *        / \
     *       +   2
     *      / \
     *     3   5
     */
    Node *root = newNode(0, '*');
    root->left = newNode(0, '+');
    root->right = newNode(2, '\0');

    root->left->left = newNode(3, '\0');
    root->left->right = newNode(5, '\0');

    // Evaluate the syntax tree
    int result = evaluate(root);
    printf("Result of the expression is: %d\n", result);

    // Print the syntax tree
    printf("Visual representation of the syntax tree:\n");
    printTree(root, 0);

    return 0;
}
