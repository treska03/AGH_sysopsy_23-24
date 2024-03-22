#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


void filePrint(const char* fileName, const __off_t fileSize) {
    printf("%s has size of: %ld \n", fileName, fileSize);
}

int main() {

    DIR* currDir = opendir(".");

    if(currDir == NULL) {
        printf("Something went wrong with opening current dir!");
        return -1;
    }

    struct dirent* currFile; // declare variable
    struct stat st; // declare variable
    __off_t count=0; // define variable

    while((currFile = readdir(currDir)) != NULL) {
        stat(currFile->d_name, &st); // get info from 
        if(!S_ISDIR(st.st_mode)) {
            filePrint(currFile->d_name, st.st_size); // CZEMU TU NIE MUSI BYĆ STRZAŁKI
            count += st.st_size;
        }
    }
    printf("Total directory size: %ld\n", count);
    
    return 0;
}