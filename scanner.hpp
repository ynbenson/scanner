/*
 --- Author ---
 Yujiro Nakanishi
 
 --- File Name ---
 scanner.hpp
 
 --- Class Description ---
    The  "scanner" class is a class that acts as a scanner. It is specifically designed for scanning a pascal language document file and printing out the lexeme and it's location. In case where error is detected the corresponding error message and location will be printed.
 
    Here's are the description of each methods in this class.
    - get_next_lexeme : The method consist of while loop which will read a character from first read file pointer(fp) and append it to lexme until it finds a legal lexeme or detects an error.
 
    - find_next_transition : Based on the current state and character type, this function will return a value that corresponds to our next state.
 
    - open_file_failed : Returns 1 when the error occurs during file open step. Called by main to check the file properly opens.
 
    - find_char_type : Returns what kind of character (character type) is being held in char c. The list of character type is listed as a enum type.
 
    - init_get_next_lexeme : This method initialize data members that need to be reset whenever we try to look for the next lexeme (thus it gets called at top of get_next_lexeme method).
 
    - handle_error : This method gets called whenever an error is detected. It'll set corresponding error message and error location.

    - handle_num_pd_pd : When we see digit followed by period, then we have to check whether the next character is another period. Thus this function invokes fpeek().
 
    - fpeek: take out character, put it back, and then returns the value of the taken out character. (Essentially acts as a peek() )
 
    - update_column_and_line : updates line and column number. Gets called right after reading a character.
 
    - append_to_lexeme : append character to lexeme.
 
 */



#ifndef scanner_hpp
#define scanner_hpp

#include <iostream>
#include <iomanip> // setw
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h> // getc
using namespace std;

class scanner {
private:
    char c; // stores the result of get()
    int ch;
    int lineNum, colNum, curr_line, curr_col, catNum, currState;
    bool put_back_char,error_occurred,num_pd_pd_found,is_eof,in_comment_or_string, unrec_char;
    FILE* fp;
    string lex, error_msg;

    // States in FSA
    enum States {
        FS_START=0,
        FS_DONE=1,
        FS_AFT_LP=2,
        FS_IN_COM=3,
        FS_COM_STAR=4,
        FS_AFT_GT=5,
        FS_AFT_LT=6,
        FS_AFT_PD=7,
        FS_AFT_CLN=8,
        FS_SQ_IN_STRING=9,
        FS_IN_STRING=10,
        FS_STR_SQ=11,
        FS_ID=12,
        FS_RECSYM=13,
        FS_FLOAT=14,
        FS_PD_PD=15,
        FS_PD=16,
        FS_COM_OR_RECSYM=100,
        FS_INT_OR_FLOAT=101,
        FS_STR_OR_RECSYM=102,
        
    };
    
    // Character Types
    enum CharType {
        CT_INT=0,
        CT_PD=1,
        CT_LP=2,
        CT_RP=3,
        CT_ALPHA=4,
        CT_STAR=5,
        CT_SQ=6,
        CT_WS=7,
        CT_PSI=8,
        CT_EQ=9,
        CT_GT=10,
        CT_LT=11,
        CT_COLON=12,
        CT_INVALID=13,
        CT_NL=14,
        CT_OTHER=15,
    };

    // Category Numbers
    const int CAT_ID=1,CAT_RECSYM=2,CAT_INT=3,CAT_FLOAT=4,CAT_STR=5,CAT_COM=6;
    
public:

    scanner();
    int open_file_failed(char* filename);
    int find_char_type();
    int find_next_transition();
    int handle_num_pd_pd();
    int fpeek();
    void append_to_lexeme();
    void update_column_and_line();
    void init_get_next_lexeme();
    void strip_quotes();
    void handle_error(int &col_err, int &line_err, string &err_msg);
    string get_next_lexeme(bool &stop, int &col, int &line, int &cat, int &col_err, int &line_err, string &err_msg);
};

#endif /* scanner_hpp */
