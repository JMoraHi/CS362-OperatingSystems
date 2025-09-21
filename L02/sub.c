//CS362 Operating Systems L02 by Jose Morales Hilario 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
//print errors to stderr
// stdin -> fprintf(stderr, "error ....\n");

//freopen(inFile,"r", stdin); // for opening files
//freopen(out , "w", stdout); // for outputting to a file


void printhelp() //print help message for user
{
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "\tsub [-h] [-v] [-i inputFile] [-o outputFile] fromChars toChars\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "DESCRIPTION:\n");
    fprintf(stderr, "\tThis utility copies text from an input stream to an output stream replacing every\n");
    fprintf(stderr, "\tinstance of a specific character in fromChars with a corresponding (position-wise)\n");
    fprintf(stderr, "\tcharacter from toChars. Any characters in fromChars and in toChars that do not\n");
    fprintf(stderr, "\thave corresponding counterparts in the other one are ignored. If an input file is\n");
    fprintf(stderr, "\tprovided, the content is read from that file; otherwise, the standard input is used.\n");
    fprintf(stderr, "\tIf an output file is provided, then the modified content is written to that file;\n");
    fprintf(stderr, "\totherwise, the standard output is used.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "\tfromChars (followed by a string without separating space)\n");
    fprintf(stderr, "\t\tindicates all characters that will be replaced in the processed text\n");
    fprintf(stderr, "\ttoChars\n");
    fprintf(stderr, "\t\tindicates the characters that will be used to replace corresponding (position-wise) characters from fromChars in the processed text\n");
    fprintf(stderr, "\t-i (followed by input file name)\n");
    fprintf(stderr, "\t\tuse the provided file as an input stream instead of standard input\n");
    fprintf(stderr, "\t-o (followed by output file name)\n");
    fprintf(stderr, "\t\tuse the provided file as an output stream instead of standard output\n");
    fprintf(stderr, "\t-h\n");
    fprintf(stderr, "\t\tprints this help message; it is also a default if no command line arguments are provided\n");
    fprintf(stderr, "\t-v\n");
    fprintf(stderr, "\t\tverbose mode; displays information the program received to stderr\n\n");

    return;
}


//argc holds the int amount of arguments on command line. Ex: ./sub -i in abc xyz is five arguments so, int argc = 5
//argv holds the array of the arguments from the command line. Ex: first char arr = ./sub\0, second = -i\0. \0 is a terminating character
int main(int argc, char** argv)
{

    char* inFile = NULL; //initialize File for possible assignment later
    char* outFile = NULL;
    bool iFlag = false;
    int iLoc = 0;
    bool oFlag = false;
    int oLoc = 0;
    bool vFlag = false;
    int vLoc = 0;
    bool hFlag = false;
    int hLoc = 0;
    char *fromChars = argv[argc-2];
    char *newChars = argv[argc-1];
    char buffer[100];

    if(argc<3) //if we have less than 3 arguments,
    {
        fprintf(stderr,"ERROR: WE NEED 3 ARGUMENTS.\n\n");
        printhelp(); 
        exit(EXIT_FAILURE);
    } 

    /*
    char* strCheck = strchr(argv[argc - 1], '.'); //returns ".txt" if last argument ends with ".txt"
    char* ender = ".txt"; //for comparison case later

    
    if(strcmp(strCheck, ender) == 0){ //if last argument ends with ".txt" OR last argument starts with '-'
        printhelp();
        exit(EXIT_FAILURE);
    } 

    
    if(argv[argc-1][0] == '-') //if last argument string begins with '-' 
    {
        printhelp();
        exit(EXIT_FAILURE);

    }
    */


    //go thru argv to find which arguments were triggered
    for(int i = 1; i<argc-2; i++){ //last two arguments should be the characters we are going to change
        
        //assuming we get -i,-o,-h,-v, we do the for loop
        if(argv[i][0] == '-'){ //char* at current index and then we look at the first character of string
        
            switch(argv[i][1]){
                case 'h': //print help message
                    hFlag = true;
                    hLoc = i; //remember h's loc
                    break;
                case 'i': 
                    iFlag = true;
                    iLoc = i; //remember i's loc
                    break;
                case 'o': //accepts inputted file as a place to produce the output of the program
                    oFlag = true; //remember o's loc
                    oLoc = i;
                    break;
                case 'v': //display error information the program got and display it to stderr
                    vFlag = true; //remember v's loc
                    vLoc = i;
                    break;
            }
        }
    }

    //seeing whether to stops or continue program with amount of characters we were given with fromChars and toChars
    if(strlen(fromChars) != strlen(newChars)){
        if(strlen(fromChars) < strlen(newChars)){
            fprintf(stderr,"sub: WARNING - extraneous replacement character\n");
        } else if(strlen(fromChars) > strlen(newChars)){ 
            fprintf(stderr,"sub: ERROR - missing replacement character\n");
            printhelp();
            exit(EXIT_FAILURE);

        }
    }

    //print out help message for how program works
    if(hFlag == true){
    printhelp();
    }

    //print out to the user what you have accepted from the agrument line they inputted when the program started
    if(vFlag == true){
        fprintf(stderr, "Received %d arguments: ", argc); 
        for(int i = 0; i < argc; i++){
            fprintf(stderr, "%s ", argv[i]); 
                      
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "fromChars: %s\n",argv[argc - 2]);
        fprintf(stderr, "toChars: %s\n",argv[argc - 1]);

        if(iFlag == true){
            fprintf(stderr, "Using %s as input\n", argv[iLoc + 1]);
        } else {
            fprintf(stderr, "Using STDIN as input\n");
        }
        if(oFlag == true){
            fprintf(stderr, "Using %s as output\n", argv[oLoc + 1]); 
        } else {
            fprintf(stderr, "Using STDOUT as output\n");
        }
        
    }

    //checking whether or not to redirect STDIN and/or STDOUT to a file of that name
    if(iFlag == true && oFlag == true){ //redirect stdin to inFile and stdout to outFile
        inFile = argv[iLoc + 1];
        outFile = argv[oLoc + 1];
        freopen(inFile,"r", stdin); 
        freopen(outFile,"w", stdout);
    } else if(iFlag == true && oFlag == false){ //redirect ONLY stdin to inFile 
        inFile = argv[iLoc + 1];
        freopen(inFile,"r", stdin);
        
    } else if(iFlag == false && oFlag == true){ //redirect ONLY stdout to outFile then wait for an input from user until we get a EOF (Ctrl + D)
        outFile = argv[oLoc + 1];
        freopen(outFile,"w", stdout);   
    }
    
    int currCh;
    while((currCh = getchar()) != EOF) //We now accept input character by character from stdin 
    {
        //check for each character and check whether or not you have to change it, for as long as the length of toChars string
        for(int i = 0;  i < strlen(fromChars) ; i++ )
        {
            if(currCh == fromChars[i]) //if the current character is equal to the character in the fromChar string at i
            {
                currCh  = newChars[i]; //then replace current character with appropriate character from newChars string at i
            }

        }
        putchar(currCh); // and output to stdout, character by character
        
    }
    
    printf("\n");
    return 0;  
} 
    
