#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>

#define CMD_RESULT_BUF_SIZE 1024

int ExecuteCMD(const char *cmd, char *result)
{
    int iRet = -1;
    char buf_ps[CMD_RESULT_BUF_SIZE];
    char ps[CMD_RESULT_BUF_SIZE] = {0};
    FILE *ptr;

    strcpy(ps, cmd);

    if((ptr = popen(ps, "r")) != NULL)
    {
        while(fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
        {
           strcat(result, buf_ps);
           if(strlen(result) > CMD_RESULT_BUF_SIZE)
           {
               break;
           }
        }
        pclose(ptr);
        ptr = NULL;
        iRet = 0;  // 处理成功
    }
    else
    {
        //printf("popen %s error\n", ps);
        iRet = -1; // 处理失败
    }

    return iRet;
}

int strim(char str[])
{
	char *str_c=str;
	int i,j=0;
	for(i=0;str[i]!='\0';i++)
	{
		if(str[i]!=' ' && str[i]!='\n' && str[i]!='\r')
			str_c[j++]=str[i];
	}
	str_c[j]='\0';
	str=str_c;
}

void getMacAddress(char *mac_address) {
    struct ifaddrs *ifaddr;       //Creo le due struct che conterranno i dati delle interfacce

    if (getifaddrs(&ifaddr) == -1) {    //il metodo crea una lista collegata di struct ifaddrs, inizializzando *ifaddr
        perror("Error getting interfaces");
        return;
    }

    while (1==1)
    {
        ifaddr = ifaddr->ifa_next;      //iterazione su tutte le interfacce
        if (ifaddr == NULL)      //se l'interaffia corrrente Non contiene un indirizzo, allora si è arrivati alla fine dell'iterazione
            break;
        if ((ifaddr->ifa_flags & IFF_UP) && (ifaddr->ifa_addr->sa_family == AF_PACKET)) {     //controllo se l'interfaccia è attiva ed è un AF_PACKET
            char *interface_name = ifaddr->ifa_name;
            if (interface_name[0] == 'e' || interface_name[0] == 'w') {
                struct sockaddr_ll *s = (struct sockaddr_ll*)ifaddr->ifa_addr;     //cast da iffadrs a sockaddr_ll
				int i;
                for(i = 0; i < 6; i++) {        //estrazione del valore MAC un byte alla volta...
                    sprintf(mac_address+i*3, "%02X:", s->sll_addr[i]);  //...perché è necessario costruire una sringa
                }
                mac_address[strlen(mac_address)-1] = '\0';  //rimozione dell'ultimo carattere ":"
                break;
            }
        }
    }
}

int main()
{
	char result[1024]={0};
	char result0[CMD_RESULT_BUF_SIZE]={0};
    char result1[CMD_RESULT_BUF_SIZE]={0};
    char result2[CMD_RESULT_BUF_SIZE]={0};
	getMacAddress(result0);
    ExecuteCMD("dmidecode -t processor | grep 'ID' | awk -F ':' '{print $2}' | head -n 1", result1);
	ExecuteCMD("dmidecode | grep 'Serial Number' | awk -F ':' '{print $2}' | head -n 1", result2);
    strim(result1);
	strim(result2);
	//printf("MAC address:%s\r\n", result0);
	//printf("CPUSerial:%s\r\n", result1);
    //printf("MainBoardSerial:%s\r\n", result2);
	strcpy(result, result0);
	strcat(result, result1);
	strcat(result, result2);
	printf("%s\r\n", result);
    return 0;
}
