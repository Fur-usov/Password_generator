#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MIN_PASSWORD_LENGTH 14

//функция генерации пароля
char* generate_password(int length) 
{
    if (length < MIN_PASSWORD_LENGTH) 
    {
        printf("Password length cannot be less than %d\n", MIN_PASSWORD_LENGTH);
        return NULL;
    }

    char* password = malloc(length + 1);
    if (password == NULL) 
    {
        perror("Error allocating memory");
        return NULL;
    }

    //мощность алфовита - 76 символов
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&*()-_{}`"; 
    unsigned char random_data[length];

    int fd = open("/dev/random", O_RDONLY);
    if (fd == -1) 
    {
        perror("Error opening /dev/random");
        free(password);
        return NULL;
    }

    if (read(fd, random_data, length) != length) 
    {
        perror("Error reading from /dev/random");
        free(password);
        close(fd);
        return NULL;
    }
    close(fd);

    int upper = 0, lower = 0, digit = 0, special = 0;
    do 
    {
        for (int i = 0; i < length; i++) 
        {
            unsigned char ch = characters[random_data[i] % (sizeof(characters) - 1)];
            password[i] = ch;

            if (ch >= 'A' && ch <= 'Z') upper = 1;
            else if (ch >= 'a' && ch <= 'z') lower = 1;
            else if (ch >= '0' && ch <= '9') digit = 1;
            else special = 1;
        }
    } 
    while (upper == 0 || lower == 0 || digit == 0 || special == 0);

    password[length] = '\0';
    return password;
}


// Функция для проверки пароля
bool is_password_safe(char *password) 
{
    // Проверка на длину
    if (strlen(password) < 8) {
        printf("Password is too short\n");
        return false;
    }

    // Проверка на наличие цифр, строчных и прописных букв, специальных символов
    int upper = 0, lower = 0, digit = 0, special = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (password[i] >= 'A' && password[i] <= 'Z') upper = 1;
        else if (password[i] >= 'a' && password[i] <= 'z') lower = 1;
        else if (password[i] >= '0' && password[i] <= '9') digit = 1;
        else special = 1;
    }

    if (upper == 0 || lower == 0 || digit == 0 || special == 0) {
        printf("Password should contain at least one uppercase letter, one lowercase letter, one digit, and one special character\n");
        return false;
    }

    // Проверка на наличие в списке самых часто используемых паролей
    FILE *file = fopen("top_passwords.txt", "r");
    if (file == NULL) {
        printf("Could not open file with passwords\n");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(password, line) == 0)
        {
            printf("Password is too common\n");
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}



// функция подсчета энтропии пароля
void calculate_entropy(char* password) 
{
    int length = strlen(password);

    int count[256] = {0};
    for (int i = 0; i < length; i++)
    {
        count[(int)password[i]]++;
    }

    int n = 0; // количество уникальных символов
    for (int i = 0; i < 256; i++)
    {
        if (count[i] > 0)n++;
    }

    double entropy = length * log2(n);

    printf("entropy is: %.2f ", entropy);

    if (entropy < 72) 
    {
        printf("(low)\n");
    } 
    else if (entropy >= 72 && entropy <= 75) 
    {
        printf("(acceptable)\n");
    } 
    else 
    {
        printf("(high)\n");
    }
}



int main() 
{
    printf("Enter:\n1 to generate secure password\n2 to check safety of your password\n");
    int flag = 1;
    scanf("%d", &flag);

    if (flag == 1) //генерация пароля
    {
        int length;
        printf("Enter the length of the password (minimum %d):\n", MIN_PASSWORD_LENGTH);
        scanf("%d", &length);
        char* password = generate_password(length);
        if (password == NULL) {
            return 1;
        }
        printf("Generated password: %s\n", password);
        printf("For generated password ");
        calculate_entropy(password);
        free(password);
    }
    else if(flag == 2) //проверка пароля
    {
        char user_password[200]; 
        printf("Enter your password\n");
        scanf("%s", user_password); 
        if (is_password_safe(user_password) == true)
        {
            printf("password is secure\n");
        }
        printf("For your password ");
        calculate_entropy(user_password);
    }

    else
    {
        printf("wrong key\n");
    }

    return 0;
}