/*
 --- Author ---
 Yujiro Nakanishi
 
 --- File Name ---
 main.cpp
 
 --- File Description ---
 This is the file that contains the main function.
 The main function will first check if the argument count is correct
 It also checked if the provided file opens without any error, by creating an instance of scanner class, and passing in the filename.
 After validation is done, get_next_lexeme method of scanner class is repeatedly called until the file reaches end or some kind of error occures.
 For each successful returning of lexeme, line number, column number, category number, and actual lexeme will be printed out.
 A prompt "END OF FILE" shall be displayed when a file reaches EOF.
 
 */

#include "scanner.hpp"

int main(int argc, char *argv[]){
    
    // check for correct argument count
    const int ARGUMENTS = 2;
    if(argc != ARGUMENTS){
        cout << "Usage: " << argv[0] << " <input file name>" << endl;
        return -1;
    }
    
    // open file, if failed then close program
    scanner s;
    if (s.open_file_failed(argv[1])){
        return -1;
    }
    
    // until EOF or error occurs, get the next lexeme,
    // and print line number, column number, and category number
    string str,err_msg;
    int col, line, cat, col_err=-1, line_err=-1;
    bool stop=false;
    do {
        str = s.get_next_lexeme(stop,col,line,cat,col_err,line_err,err_msg);
        
        if(!stop || str!="") {
            if(cat>=1 && cat<=5){
                cout << setw(5) << line << setw(5) << col;
                cout << setw(5) << cat << "\t" << str << endl;
            }
        }
    }
    while (!stop);
    
    if (col_err==-1 && line_err==-1)
        cout << "END OF FILE" << endl;
    else {
        cout << err_msg << "line " << line_err << " column " << col_err << endl;
    }
    
    return 0;
}
