#include <stdio.h>
#include <string.h>
#include <time.h>

int copy_file(const char *copiedFilePath, const char *pasteFilePath)
{
  char c;
  int* we,wy;
  FILE *toCopy = fopen(copiedFilePath, "r");
  FILE *toPaste = fopen(pasteFilePath, "w");

  if(toCopy == NULL && toPaste == NULL) {
    return -1;
  }

  int offset = -1;
  while(fseek(toCopy, offset, SEEK_END) != -1 && fread(&c, sizeof(char), 1, toCopy)==1) {
    fwrite(&c, sizeof(char), 1, toPaste);
    offset--;
  }

  fclose(toCopy);
  fclose(toPaste);

  return 0;
}

char *reverse_string(char *stringToReverse, int len) {
  for (int i = 0; i < len / 2; i++) {
      char temp = stringToReverse[i];
      stringToReverse[i] = stringToReverse[len - i - 1];
      stringToReverse[len - i - 1] = temp;
  }
}

int copy_file_by_chunks(const char *copiedFilePath, const char *pasteFilePath) {
  char c[1024];
  int* we,wy;
  FILE *toCopy = fopen(copiedFilePath, "r");
  FILE *toPaste = fopen(pasteFilePath, "w");

  if(toCopy == NULL && toPaste == NULL) {
    return -1;
  }
  
  int offset = -1024;
  int bytesRemaining;

  while(fseek(toCopy, offset, SEEK_END) == 0 && fread(&c, sizeof(char), 1024, toCopy) > 0) {
    reverse_string(c, 1024);
    fwrite(&c, sizeof(char), 1024, toPaste);
    offset -= 1024;
    bytesRemaining = ftell(toCopy);
  }
  bytesRemaining = bytesRemaining-1024; // fread moves pointer 1024 bytes
  fseek(toCopy, 0, SEEK_SET);
  fread(&c, sizeof(char), bytesRemaining, toCopy);
  reverse_string(c, bytesRemaining);
  fwrite(&c, sizeof(char), bytesRemaining, toPaste);

  fclose(toCopy);
  fclose(toPaste);

  return 0;
}

double measure_execution_time(int (*function)(), const char *copiedFilePath, const char *pasteFilePath) {
    clock_t start, end;
    double cpu_time_used;

    start = clock(); // Zapisujemy czas rozpoczęcia pomiaru

    // Wywołujemy funkcję, której czas wykonania chcemy zmierzyć
    function(copiedFilePath, pasteFilePath);

    end = clock(); // Zapisujemy czas zakończenia pomiaru

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    return cpu_time_used;
}

int main(int argc, char *argv[]) {

  if(argc == 3) {
    double timeUsed;
    #ifdef BY_ONE
    timeUsed = measure_execution_time(copy_file, argv[1], argv[2]);
    #else
    timeUsed = measure_execution_time(copy_file_by_chunks, argv[1], argv[2]);
    #endif
    printf("Czas wykonywania funkcji: %f\n", timeUsed);
    return 0;
  }
  return -1;

}