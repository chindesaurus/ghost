/*
 * ghost.c
 *
 * Implements the word game Ghost.
 *
 * Usage: ./ghost N
 * where N is the number of players (must be >= 2).
 *
 * @author chindesaurus
 * @version 1.00
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* dictionary to check against */
#define DICTIONARY "./words.txt"

/* letters in the alphabet */
#define CHARS 26

/* maximum and minimum word length */
#define MAX_LENGTH 45
#define MIN_LENGTH 3


/* defines a node of the trie */
typedef struct node {
    bool is_word;
    struct node *children[CHARS];
} node;

/* the root of the trie */
node *root = NULL;


/* function prototypes */
void greet(void);
void clear(void);
bool load(const char *dictionary);
bool unload(void);
void freeNode(node *curr);


int 
main(int argc, char **argv) {

    // greet the players
    greet(); 

    // check for correct number of args
    if (argc != 2) {
        fprintf(stderr, "Usage: ghost N\nwhere N is the number of players.\n");
        return 1;
    }
    
    // check for valid N
    int N = atoi(argv[1]);
    if (N < 2) {
        fprintf(stderr, "Invalid argument. N must be an integer >= 2.\n");
        return 2;
    }
    
    // load the dictionary
    bool loaded = load(DICTIONARY);

    // abort if dictionary not loaded
    if (!loaded) {
        fprintf(stderr, "Could not load %s.\n", DICTIONARY);
        return 3;
    }


    int curr_player = 1;            // whose turn is it? 
    int letter_count = 0;           // letters in the current fragment
    char fragment[MAX_LENGTH] = ""; // the current word fragment 
    char c;                         // character read from players
    node *letter = root;            // track letters down through trie

 
    /* main gameplay loop */
    while (true) {
        
        printf("\nCurrent word fragment: %s\n", fragment);
        do { 
            printf("Player %i says letter: ", curr_player);
            scanf(" %c", &c);       // read a letter into c
            scanf("%*[^\n]%*c");    // consume remainder of line, if any 
        } while (!isalpha(c));

        // convert to lowercase
        c = tolower(c);


        // determine index of next node in the trie
        int index = c - 'a';
        if (letter->children[index] == NULL) {
            printf("There's no word that begins with \"%s%c\".\n", fragment, c);
            printf("Try again.\n");
            continue;
        }    
        else {
            
            // append character to current word fragment
            fragment[letter_count] = c;

            // continue down the trie
            letter = letter->children[index];

            // check if the fragment now spells a word that is
            // more than MIN_LENGTH letters long
            if (letter->is_word && letter_count >= MIN_LENGTH)
                break;
 
            // keep track of whose turn it is 
            curr_player = (curr_player % N) + 1; 
    
            // keep track of how many letters are in the current fragment
            letter_count += 1;
        }
    }

    // Game Over
    printf("\nPlayer %i loses!\n", curr_player);
    printf("He/She spelled the word \"%s\".\n", fragment);
    printf("Thanks for playing!\n\n");

    // unload the dictionary
    bool unloaded = unload();
    
    // abort if dictionary not unloaded
    if (!unloaded) {
        fprintf(stderr, "Could not unload %s.\n", DICTIONARY);
        return 4;
    }
 
    // kthxbai
    return EXIT_SUCCESS;
}


/*
 * Greets players.
 */
void
greet(void) {
    clear();
    
    printf("_______           _______  _______ _________\n");
    printf("\(  ____ \\|\\     /|(  ___  )(  ____ \\\\__   __/\n");
    printf("| (    \\/| )   ( || (   ) || (    \\/   ) (\n");   
    printf("| |      | (___) || |   | || (_____    | |\n");   
    printf("| | ____ |  ___  || |   | |(_____  )   | |\n");   
    printf("| | \\_  )| (   ) || |   | |      ) |   | |\n");   
    printf("| (___) || )   ( || (___) |/\\____) |   | |\n");
    printf("(_______)|/     \\|(_______)\\_______)   )_(\n");       

}


/**
 * Clears screen using ANSI escape sequences.
 */
void
clear(void) {
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}


/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool 
load(const char *dictionary) {
    
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
        return false;

    /* initialize the trie */
    root = malloc(sizeof(*root));
    if (root == NULL) {
        fclose(dict); 
        return false;
    }  
    root->is_word = false;
    for (int i = 0; i < CHARS; i++)
        root->children[i] = NULL;

    // for iterating through the trie
    node *iterator = root;

    char input;
    while ( (input = fgetc(dict)) != EOF) {
        
        // ensure every letter is lower case
        input = tolower(input);

        int index = input - 'a';
            
        // end of word 
        if (input == '\n') {
            iterator->is_word = true;

            // reset iterator
            iterator = root;
        }

        else { 
            if (iterator->children[index] == NULL) {
                
                // if NULL, allocate space for new node, initialize it,
                // and have children[index] point to it
                node *new = malloc(sizeof(*new));
                if (new == NULL) {
                    fclose(dict); 
                    return false;
                }

                for (int i = 0; i < CHARS; i++)
                    new->children[i] = NULL;
                new->is_word = false;

                iterator->children[index] = new;
            }
            // move iterator to new node
            iterator = iterator->children[index];
       }
    }
    
    fclose(dict);
   
    // w00t 
    return true;
}


/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool 
unload(void) {
    freeNode(root);
    free(root);
    return true;
}


/**
 * Frees all nodes above curr.
 */
void
freeNode(node *curr) {

    if (curr == NULL)
        return;

    // iterate through all of curr's children
    for (int i = 0; i < CHARS; i++) {
        // for each non-NULL child, recursively free it
        if (curr->children[i] != NULL)
            freeNode(curr->children[i]);
        
        // if node has no children, free it
        free(curr->children[i]);
    }
}
