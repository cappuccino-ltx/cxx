
#include <stdio.h>



int main() {
    //FILE* pf1 = fopen("log1.txt", "w");
    FILE* pf1 = fopen("log1.txt", "r");
    
    //char str[] = "linux so easy!\n";
    //fwrite(str, sizeof(str), 1, pf1);
    //fflush(pf1);

    char buffer[128];
    fread(buffer, sizeof(char), sizeof buffer, pf1);
    printf("%s\n", buffer);
    
    fclose(pf1);
    return 0;
}
