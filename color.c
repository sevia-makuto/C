#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{

  srand(time(NULL));
  char name[30];
  // char nameInput[30];

  int age;
  char ageInput[30];

  int number;
  char numberInput[30];

  printf("Enter your name :");
  fgets(name, sizeof(name), stdin);

  printf("Enter your age : ");
  while (fgets(ageInput, sizeof(ageInput), stdin))
  {
    if (sscanf(ageInput, "%d", &age) == 1)
    {
      break;
    }
    else
    {
      printf("Invalid input try again : ");
    }
  }

  printf("Enter your  lucky number (0-9) : ");
  while (fgets(numberInput, sizeof(numberInput), stdin))
  {
    if (sscanf(numberInput, "%d", &number) == 1)
    {
      break;
    }
    else
    {
      printf("Invalid input try again : ");
    }
  }

  int luckyNumber = rand() % 10;
  while (1)
  {
    if (number < 0 || number > 9)
    {
      printf("You are out of bound !!!\n");
      printf("Try again : ");
    }
    if (luckyNumber == number)
    {
      printf("You got the target !!!!\n");
      break;
    }
    else if (luckyNumber >= number)
    {
      printf("you suppased the target number . Try again : ");
    }
    else if (luckyNumber <= number)
    {
      printf("Your were almost the target . Try again : ");
    }
    else
    {
      printf("program in use");
    }
    }

  printf("The lucky number was %d\n", luckyNumber);

  return 0;
}