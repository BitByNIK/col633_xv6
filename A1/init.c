// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_LENGTH 128
#define TRIES 3

int isInputValid(char *input)
{
    int i = 0;
    while (input[i] != '\0')
    {
        if (!((input[i] >= 'A' && input[i] <= 'Z') ||
              (input[i] >= 'a' && input[i] <= 'z') ||
              (input[i] >= '0' && input[i] <= '9')))
        {
            return 0;
        }

        i++;
    }

    return 1;
}

int login()
{
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];

    for (int i = 0; i < TRIES; i++)
    {
        printf(1, "Enter username: ");
        int usernameLength = read(0, username, MAX_LENGTH - 1);
        if (usernameLength > 0)
            username[usernameLength - 1] = '\0';
        if (!isInputValid(username) || strcmp(username, USERNAME))
        {
            printf(1, "Invalid Username\n");
            continue;
        }

        printf(1, "Enter password: ");
        int passwordLength = read(0, password, MAX_LENGTH - 1);
        if (passwordLength > 0)
            password[passwordLength - 1] = '\0';
        if (!isInputValid(password) || strcmp(password, PASSWORD))
        {
            printf(1, "Invalid Password\n");
            continue;
        }
        else
        {
            printf(1, "Login successful\n");
            return 1;
        }
    }

    printf(1, "Too many failed attempts!!! Login disabled\n");
    return 0;
}

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  if(!login())
    while(1) sleep(100);

  for(;;){
    printf(1, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
