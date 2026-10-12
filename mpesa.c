#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    int service, agentNumber, action, number, pin, storeNumber;
    char serviceInput[30];
    float amount;

    printf("Welcome to miniMpesa..........\n");

    printf("1.Send money\n");
    printf("2.Withdraw cash\n");
    printf("3.Buy airtime\n");
    printf("4.Lipa na mpesa\n");
    printf("5.My Account\n");
    printf("6.Deposit\n");

    printf("Choose on the the service  you like to be offered to you : ");
    while (fgets(serviceInput, sizeof(serviceInput), stdin))
    {
        if (sscanf(serviceInput, "%d", &service) == 1)
        {
            break;
        }
        else
        {
            printf("Invalid input !!!\n");
            printf("Reenter the service : ");
        }
    }

    switch (service)
    {
    case 1:
        printf("Send Money\n");
        break;
    case 2:
        printf("Withdraw cash\n");
        break;
    case 3:
        printf("Buy airtime\n");
        break;
    case 4:
        printf("Lipa na mpesa\n");
        break;
    case 5:
        printf("My Account\n");
        break;
    case 6:
        printf("Deposit\n");
    default:
        printf("Invalid input !!\n");
    }

    if (service == 1)
    {
        printf("1.Send money to any Network\n");
        printf("2.Mpesa global\n");
        printf("3.Pochi la biashara\n");
    }
    else if (service == 2)
    {
        printf("1.From an agent\n");
        printf("2.From ATM\n");
        printf("3.From bank to mpesa\n");
    }
    else if (service == 3)
    {
        printf("1.Buy airtime\n");
        printf("2.Data deals\n");
        printf("3.Minutes\n");
    }
    else if (service == 4)
    {
        printf("1.Paybill\n");
        printf("2.Buy Goods\n");
        printf("3.Pochi la biashara\n");
    }
    else if (service == 5)
    {
        printf("1.Unlock Mpesa pin\n");
        printf("2.Mpesa pin manager\n");
        printf("3.mpesa statement\n");
    }
    else if (service == 6)
    {
        printf("1.Deposit to invest....\n");
    }
    else
    {
        printf("Invalid input!!!!");
    }
    printf("Choose action : ");
    scanf("%d", &action);
    if (service == 1 && action == 1)
    {
        printf("Send money to any Network\n");
        printf("Enter phone number : ");
        scanf("%d", &number);
        printf("Enter amount : ");
        scanf("%f", &amount);
        printf("Enter Mpesa pin : ");
        scanf("%d", &pin);
    }
    if (service == 1 && action == 2)
    {
        printf("2.Mpesa global\n");
    }
    if (service == 1 && action == 3)
    {
        printf("3.Pochi la biashara\n");
    }

    if (service == 2 && action == 1)
    {
        printf("Enter  agent number : ");
        scanf("%d", &agentNumber);
        printf("Enter store number : ");
        scanf("%d", &storeNumber);
        printf("Enter a mount : ");
        scanf("%f", &amount);
        printf("Enter m-pesa pin : ");
        scanf("%d", &pin);
        printf("money send successfully !!!\n");
    }
    printf("money send successfully !!!\n");
    if (service == 2 && action == 3)
    {
        printf("FROM BANK TO MPESA\n");
        printf("1.Equity Bank LTD\n");
        printf("2.COOPERATIVE BANK\n");
        printf("3.KCB BANK\n");
        printf("4.NCBA\n");
        printf("5.ABSA\n");
    }

    return 0;
}