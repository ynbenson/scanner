/*
 
 --- Author ---
 Yujiro Nakanishi
 
 --- File Name ---
 scanner.cpp

 --- File Description ---
 This file contains the actual implementation of various methods of scanner class.
 
 */



#include "scanner.hpp"

//----------------------------------------------------------------------
//                             constructor
//----------------------------------------------------------------------
scanner::scanner(){
    // initializing varibles
    lineNum=1; colNum=1; catNum=0; curr_line=1; curr_col=0; currState=FS_START; lex="";
    put_back_char=false; error_occurred=false;
    num_pd_pd_found=false; in_comment_or_string=false; unrec_char=false; is_eof=false;
}


//----------------------------------------------------------------------
//                            fpeek method
//----------------------------------------------------------------------
int scanner::fpeek() {
    // Takes out 1 char, and immediately put it back (thus acting like peek).
    // This is only used situation where we encounter # followed by .
    int i=fgetc(fp);
    ungetc(i,fp);
    return i;
}

//----------------------------------------------------------------------
//                          handle_error method
//----------------------------------------------------------------------
void scanner::handle_error(int &col_err, int &line_err, string &err_msg) {
    col_err=curr_col;
    line_err=curr_line;
    
    if (unrec_char) {
        err_msg = "SCANNER ERRROR: Invalid character on ";
    }
    else if (currState==FS_AFT_PD) {
        err_msg = "SCANNER ERRROR: Digit expected on ";
    }
    else if (currState==FS_IN_COM) {
        err_msg = "SCANNER ERRROR: End of comment expected on ";
    }
    else if (currState==FS_IN_STRING) {
        err_msg = "SCANNER ERRROR : End of string literal expected (') on ";
    }
}


//----------------------------------------------------------------------
//                       handle_num_pd_pd method
//----------------------------------------------------------------------
int scanner::handle_num_pd_pd() {
    catNum=CAT_INT;
    num_pd_pd_found=true;
    put_back_char=true;
    return FS_DONE;
}


//----------------------------------------------------------------------
//                         strip_quotes method
//----------------------------------------------------------------------
void scanner::strip_quotes() {
    string temp="";
    for(int i=0; i<lex.length(); ++i) {
        if(i!=0 && i!=lex.length()-1){
            temp+=lex[i];
        }
    }
    lex=temp;
}


//----------------------------------------------------------------------
//                     init_get_next_lexeme method
//----------------------------------------------------------------------
void scanner::init_get_next_lexeme() {
    lex="";
    error_msg="";
    if (!error_occurred)
        currState=FS_START;
    in_comment_or_string=false;
}


//----------------------------------------------------------------------
//                     update_column_and_line method
//----------------------------------------------------------------------
void scanner::update_column_and_line() {
    if (!is_eof){
        if(c!='\n')
            curr_col++;
        else {
            curr_line++;
            curr_col=0;
        }
    }
}

//----------------------------------------------------------------------
//                       append_to_lexeme method
//----------------------------------------------------------------------
void scanner::append_to_lexeme(){
    if (!put_back_char && !is_eof && c!='\0' && !error_occurred) {
        string str(1,c);
        if (in_comment_or_string)
            lex+=str;
        else{
            if (!isspace(c))
                lex+=str;
        }
    }
}

//----------------------------------------------------------------------
//                      open_file_failed method
//----------------------------------------------------------------------
int scanner::open_file_failed(char *filename){
    if ( (fp = fopen(filename,"r"))==NULL ){
        cout << "Can't open file name: " << filename << endl;
        return 1;
    }
    else {
        return 0;
    }
}


//----------------------------------------------------------------------
//                      find_char_type method
//----------------------------------------------------------------------
int scanner::find_char_type(){
    if (c=='\n'||c=='\r')
        return CT_NL;
    else if (isspace(c))
        return CT_WS;
    else if (c<' '||c>='~'){
        unrec_char=true;
        return CT_INVALID;
    }
    else if (c>='0' && c<='9')
        return CT_INT;
    else if ((c>='a' && c<='z') || (c>='A' && c<='Z'))
        return CT_ALPHA;
    else if (c=='(')
        return CT_LP;
    else if (c==')')
        return CT_RP;
    else if (c=='.')
        return CT_PD;
    else if (c=='*')
        return CT_STAR;
    else if (c=='\'')
        return CT_SQ;
    else if (c=='+'||c=='-'||c=='/'||c=='['||c==']'||c==','||c==';'||c=='^')
        return CT_PSI;
    else if (c=='>')
        return CT_GT;
    else if (c=='<')
        return CT_LT;
    else if (c==':')
        return CT_COLON;
    else if (c=='=')
        return CT_EQ;
    else if (c>=' '&&c<'~')
        return CT_OTHER;
    else {
        cout << "type was not specified...\n";
        return -1;
    }
}

//----------------------------------------------------------------------
//                     find_next_transition method
//----------------------------------------------------------------------
int scanner::find_next_transition(){
    int ret=currState;
    int char_type = find_char_type();
    
    if (unrec_char) {
        error_occurred=true;
    }
    else {
        switch (currState) {
            case FS_START:{
                if (num_pd_pd_found) {
                    ret=FS_PD_PD;
                    catNum=CAT_RECSYM;
                    num_pd_pd_found=false;
                }
                else if (char_type==CT_LP)
                    ret=FS_COM_OR_RECSYM;
                else if (char_type==CT_INT){
                    catNum=CAT_INT;
                    ret=FS_INT_OR_FLOAT;
                }
                else if (char_type==CT_SQ){
                    in_comment_or_string=true;
                    ret=FS_IN_STRING;
                    catNum=CAT_STR;
                }
                else if (char_type==CT_ALPHA) {
                    ret=FS_ID;
                    catNum=CAT_ID;
                }
                else if (char_type==CT_PSI||char_type==CT_EQ||char_type==CT_STAR||char_type==CT_RP) {
                    ret=FS_DONE;
                    catNum=CAT_RECSYM;
                } else if (char_type==CT_LT){
                    ret=FS_AFT_LT;
                    catNum=CAT_RECSYM;
                } else if (char_type==CT_GT){
                    ret=FS_AFT_GT;
                    catNum=CAT_RECSYM;
                } else if (char_type==CT_COLON){
                    ret=FS_AFT_CLN;
                    catNum=CAT_RECSYM;
                } else if (char_type==CT_PD){
                    ret=FS_PD;
                    catNum=CAT_RECSYM;
                }
                
                if (char_type!=CT_WS && char_type!=CT_NL) {
                    colNum=curr_col;
                    lineNum=curr_line;
                }
                break;
            }
            case FS_COM_OR_RECSYM:{
                if (char_type==CT_STAR)
                    ret=FS_IN_COM;
                else {
                    ret=FS_DONE;
                    put_back_char=true;
                    catNum=CAT_RECSYM;
                }
                break;
            }
            case FS_IN_COM:{
                in_comment_or_string=true;
                catNum=CAT_COM;
                if (is_eof){
                    error_occurred=true;
                    
                }
                else if (char_type==CT_STAR)
                    ret=FS_COM_STAR;
                break;
            }
            case FS_COM_STAR:{
                if (char_type==CT_RP){
                    ret=FS_DONE;
                }
                break;
            }
            case FS_ID:{
                if (char_type!=CT_ALPHA && char_type!=CT_INT) {
                    put_back_char=true;
                    ret=FS_DONE;
                }
                break;
            }
            case FS_INT_OR_FLOAT:{
                if (char_type!=CT_PD && char_type!=CT_INT) {
                    ret=FS_DONE;
                    put_back_char=true;
                }
                else if (char_type==CT_PD) {
                    ret=FS_AFT_PD;
                    catNum=CAT_FLOAT;
                    char p = fpeek();
                    if (p=='.'){
                        ret=handle_num_pd_pd();
                    }
                }
                break;
            }
            case FS_PD:{
                ret=FS_DONE;
                if (char_type!=CT_PD){
                    put_back_char=true;
                }
                break;
            }
            case FS_AFT_PD:{
                if (char_type==CT_INT) {
                    catNum=CAT_FLOAT;
                    ret=FS_FLOAT;
                } else {
                    //  If #. not followed by # then error
                    error_occurred=true;
                }
                
                break;
            }
            case FS_AFT_GT:{
                ret=FS_DONE;
                if (char_type!=CT_EQ){
                    put_back_char=true;
                }
                break;
            }
            case FS_AFT_LT:{
                ret=FS_DONE;
                if(char_type!=CT_EQ && char_type!=CT_GT){
                    put_back_char=true;
                }
                break;
            }
            case FS_AFT_CLN:{
                ret=FS_DONE;
                if (char_type!=CT_EQ){
                    put_back_char=true;
                }
                break;
            }
            case FS_FLOAT:{
                if (char_type!=CT_INT) {
                    ret=FS_DONE;
                    put_back_char=true;
                }
                break;
            }
            case FS_IN_STRING:{
                if (char_type==CT_SQ){
                    ret=FS_SQ_IN_STRING;
                }
                else if (char_type==CT_NL){
                    error_occurred=true;
                }
                break;
            }
            case FS_SQ_IN_STRING:{
                if (char_type!=CT_SQ){
                    ret=FS_DONE;
                    put_back_char=true;
                } else if (char_type==CT_SQ){
                    ret=FS_IN_STRING;
                    c='\0';
                }
                break;
            }
            case FS_PD_PD:{
                ret=FS_DONE;
                curr_col++;
                break;
            }
            case FS_DONE:{
                in_comment_or_string=false;
                if (!is_eof)
                    ret=FS_START;
                break;
            }
            default:
                break;
        }
    }
    return ret;
}


//----------------------------------------------------------------------
//                      get_next_lexeme method
//----------------------------------------------------------------------
string scanner::get_next_lexeme(bool &stop, int &col, int &line, int &cat, int &col_err, int &line_err, string &err_msg){
    init_get_next_lexeme();
    
    // get the first character if we already haven't
    if (!put_back_char){
        if ((ch = getc(fp))==EOF)
            is_eof=true;
        else
            c=ch;
        
        // check if it is unrecognized character or not
        unrec_char = (find_char_type()==CT_INVALID);
        if (unrec_char) {
            error_occurred=true;
        }
        
        update_column_and_line();
    } else {
        put_back_char=false;
    }
    
    while(!is_eof && !error_occurred){
        currState=find_next_transition();
        
        // add c to existing lexeme if it is not done
        append_to_lexeme();
        
        // if done, then return string
        if (currState==FS_DONE) {
            col=colNum; line=lineNum; cat=catNum;
            return lex;
        }
        
        // if c is unrecognized character, set error flag
        if (unrec_char) {
            error_occurred=true;
        }
        
        // if error flag is up, handle error and stop FSA
        if (error_occurred) {
            handle_error(col_err,line_err,err_msg);
            stop=true;
            col=colNum; line=lineNum; cat=catNum;
            return lex;
        }
        
        // get next character
        if ((ch = getc(fp))==EOF)
            is_eof=true;
        else
            c=ch;
        
        update_column_and_line();
    }
    
    if (is_eof || error_occurred)
        stop=true;
    
    if (is_eof && (currState==FS_IN_STRING|| currState==FS_IN_COM))
        error_occurred=true;
    
    if (error_occurred){
        handle_error(col_err,line_err,err_msg);
    }
    
    col=colNum; line=lineNum; cat=catNum;
    return lex;
}
