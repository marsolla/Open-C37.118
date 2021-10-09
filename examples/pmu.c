/**(C) Copyright 2014 Rafael Marsolla - rafamarsolla@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "c37118.h"
#include "c37118configuration.h"
#include "c37118pmustation.h"
#include "c37118data.h"
#include "c37118header.h"
#include "c37118command.h"
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

// off the top of my head
#define SET_BIT(val, bitIndex) val |= (1 << bitIndex)
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << bitIndex)
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << bitIndex)
#define BIT_IS_SET(val, bitIndex) (val & (1 << bitIndex))

/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0xAA = Frame Synchronization byte.  Start of Message Character
*/
#define A_SYNC_AA 0xAA     
// SYNC[1] Second byte
// bit[7] = Reserved
// bits[6,5,4] = Frame Type, bits[3,2,1,0] = Version Number (IEEE C35.118-2005 = 0001
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Data Frame byte.
*/
//      r000 = Data Frame
#define A_SYNC_DATA 0x01
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x11 = Header Frame byte. 
*/
//      r001 = Header Frame
#define A_SYNC_HDR 0x11
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config1 Frame byte. 
*/
//      r010 = Config Frame 1
#define A_SYNC_CFG1 0x21
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config2 Frame byte.
*/
//      r011 = Config Frame 2
#define A_SYNC_CFG2 0x31
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Command Frame byte. 
*/
//      r100 = Command Frame
#define A_SYNC_CMD 0x41


#define SIZE_BUFFER 80000
#define TCP_PORT 4712
/// Protocol
// I/O Default End of Line, EOL sequence
// Windows style EOL = CR+LF default Data Record Terminator
#define A_CRLF 0x0d0a   
#define A_CR 0x0d
#define A_LF 0x0a
// "," character default Data Field Delimiter
#define A_COMMA 0x2C    

struct threadarg{
	int sock ;
	DATA_Frame *my_data;
	bool send_data_flag;
};



/**
* Process Socket Packet C37.118-2011
*/
void doprocessing (int sock, CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data,HEADER_Frame *my_header);
void select_cmd_action(int sock, CMD_Frame *cmd,CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data, HEADER_Frame *my_header);
void *tx_data(void *threadarg);

/**
* Simulate a PMU server/dispatcher C37.118-2011
*/
int main( int argc, char *argv[] )
{
    int sockfd, newsockfd, portno, clilen;
    int pid;
    char buffer[SIZE_BUFFER];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    CONFIG_Frame *my_config2 = new CONFIG_Frame();
    CONFIG_1_Frame *my_config1 = new CONFIG_1_Frame();
    DATA_Frame *my_dataframe = new DATA_Frame(my_config2);
    HEADER_Frame *my_header = new HEADER_Frame("PMU VERSAO 1.0 LSEE");
    
    // Create config packet
    my_config2->IDCODE_set(7);
    my_config2->SOC_set(1149577200);
    my_config2->FRACSEC_set(1443303576);
    my_config2->TIME_BASE_set(1000000);
    my_config2->DATA_RATE_set(15);
    
    my_config1->IDCODE_set(7);
    my_config1->SOC_set(1149577200);
    my_config1->FRACSEC_set(1443303576);
    my_config1->TIME_BASE_set(1000000);
    my_config1->DATA_RATE_set(15);
    
    my_dataframe->IDCODE_set(7);
    my_dataframe->SOC_set(1149577200);
    my_dataframe->FRACSEC_set(0);
 
    //Create a ONE pmu instance
    //name, idcode, freq,analog,phasor,coord
    // true = float		    
    // coord 0 = rect
    PMU_Station pmu ("PMU LSEE",666,false,true,true,true);
    PMU_Station pmu2 ("PMU2 LSEE",667,false,false,false,false);

    // 2 phasors
    pmu.PHASOR_add("FasorTensaoA",915527,VOLTAGE);
    pmu.PHASOR_add("FasorCorrenteA",45776,CURRENT);
    pmu.ANALOG_add("Ana_TensaoA",100,0);
    pmu.ANALOG_add("Ana_CorrenteA",100,PEAK_ANALOG_INPUT);
    vector<string> namesdig;
    namesdig.push_back("Bit0");
    namesdig.push_back("Bit1");
    namesdig.push_back("Bit2");
    namesdig.push_back("Bit3");
    namesdig.push_back("Bit4");
    namesdig.push_back("Bit5");
    namesdig.push_back("Bit6");
    namesdig.push_back("Bit7");
    namesdig.push_back("Bit8");
    namesdig.push_back("Bit9");
    namesdig.push_back("Bit10");
    namesdig.push_back("Bit11");
    namesdig.push_back("Bit12");
    namesdig.push_back("Bit13");
    namesdig.push_back("Bit14");
    namesdig.push_back("Bit15");
    
    pmu.DIGITAL_add(namesdig,0x0000,0xFFFF);
    // Set nominal Freq
    pmu.FNOM_set(FN_60HZ);        
    // default config = 0;
    pmu.CFGCNT_set(1);
    pmu.STAT_set(2048);
    pmu.PHASOR_VALUE_set(Complex(3E4,3E4),0);
    pmu.PHASOR_VALUE_set(Complex(1E4,1E4),1);
    pmu.ANALOG_VALUE_set(34.15,0);
    pmu.ANALOG_VALUE_set(-100.10,1);
    pmu.DIGITAL_VALUE_set(true,0,1);
    pmu.DIGITAL_VALUE_set(true,0,3);
    pmu.FREQ_set(60.5); //in Hz here
    pmu.DFREQ_set(1.2);
    
    // 2 analogs
    pmu2.ANALOG_add("TensaoA",1,RMS_ANALOG_INPUT);
    pmu2.ANALOG_add("CorrenteA",1,PEAK_ANALOG_INPUT);
    // Set nominal Freq
    pmu2.FNOM_set(FN_50HZ);        
    // default config = 0;
    pmu2.CFGCNT_set(1);
    pmu2.STAT_set(2048);
    pmu2.ANALOG_VALUE_set(3.1415,0);
    pmu2.ANALOG_VALUE_set(6.2830,1);
    pmu2.FREQ_set(2000);
    pmu2.DFREQ_set(200);
    
    //Add PMU to Config Packet
    my_config2->PMUSTATION_ADD(&pmu);
  //  my_config2->PMUSTATION_ADD(&pmu2);
    
      //Add PMU to Config Packet
    my_config1->PMUSTATION_ADD(&pmu);
 //   my_config1->PMUSTATION_ADD(&pmu2);
    
	//std::cout << pmu.STN_get();

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int on =1;
     int status = setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = TCP_PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
    /* Now start listening for the clients, here 
     * process will go in sleep mode and will wait 
     * for the incoming connection
     */
    printf("Connected. Waiting for message:\n") ;
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    // While true
    while (1) 
    {
    	// Accept all requests
        newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        
        /* Create child process */
        pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
        if (pid == 0)  
        {
            /* This is the client process */
            close(sockfd);
            doprocessing(newsockfd,my_config1,my_config2,my_dataframe,my_header);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    } /* end of while */
}
/**
* Process Socket Packet C37.118-2011
*/
void doprocessing (int sock, CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data,HEADER_Frame *my_header){
    int n;
    unsigned char *buffer;
    buffer = (unsigned char *)malloc(SIZE_BUFFER*sizeof(char));
    CMD_Frame *cmd = new CMD_Frame();
   
    while (1){
    bzero(buffer,SIZE_BUFFER);
    n = read(sock,buffer,SIZE_BUFFER);
    usleep(1E6);
  //  cout << "Valor N:" << n << endl;
    if (n < 0) { perror("ERROR reading from socket"); exit(1);}
    //C37118 received
  //  printf("Buffer Recebido: %x%x\n",buffer[0],buffer[1]);
    if(buffer[0] == A_SYNC_AA){
		switch(buffer[1]){
			case  A_SYNC_CMD : cmd->unpack(buffer); 
				select_cmd_action(sock,cmd,myconf1,myconf2,my_data,my_header);
				break;
			default: printf ("Unknown Packet\n"); break;
		}
    }
  }
}


void select_cmd_action(int sock,CMD_Frame *cmd,CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data, HEADER_Frame *my_header){
	int n,i;
	unsigned short size ;
	unsigned char *buffer;
	unsigned char *buffer2;
	bool send_data_flag = false;
	CONFIG_Frame *aux = new CONFIG_Frame();
    pthread_t threads;
    struct threadarg t_arg;
    
     t_arg.sock = sock;
     t_arg.my_data = my_data;
     t_arg.send_data_flag = false;
     long rc;
    rc = pthread_create(&threads, NULL, tx_data, (void *) &t_arg );
    
	//printf("Comando Recebido: %x\n",cmd->CMD_get());
  	switch (cmd->CMD_get()){

          case 0x01:  // Disable Data Output
          	  t_arg.send_data_flag = false;
            break;

          case 0x02:  // Enable Data Output
          	  t_arg.send_data_flag = true;
            break;

          case 0x03:  // Transmit Header Record Frame
           	  size = my_header->pack(&buffer);
          	  n = write(sock,buffer,size); 
          	  if (n < 0) {
          	  	  perror("ERROR writing to socket");
          	  	  exit(1);
          	  }
            break;

          case 0x04:  // Transmit Configuration #1 Record Frame
           	  size = myconf1->pack(&buffer);
          	  n = write(sock,buffer,size); 
          	  if (n < 0) {
          	  	  perror("ERROR writing to socket");
          	  	  exit(1);
          	  }	
            break;

          case 0x05:  // Transmit Configuration #2 Record Frame
          	  // copiar current config quando requisitado, senao sobrescreve a cada
          	  size = myconf2->pack(&buffer);
          	  n = write(sock,buffer,size); 
          	  if (n < 0) {
          	  	  perror("ERROR writing to socket");
          	  	  exit(1);
          	  }
          	  
            break;

          default:
            break;
    }
}

void *tx_data(void *args){
	
	unsigned short size,n;
	unsigned char *buffer;
	struct threadarg *my_args = (struct threadarg*)(args);
	int i =0;
	unsigned short rate = my_args->my_data->associate_current_config->DATA_RATE_get();
	
        while(my_args->send_data_flag){
        	my_args->my_data->SOC_set(1149577200+i);
        	size = my_args->my_data->pack(&buffer);
        	n = write(my_args->sock,buffer,size); 
        	usleep(1E6/rate);
          	cout << "N: " << n << endl;
          	if (n < 0) {
          		perror("ERROR writing to socket");
          	  	exit(1);
          	} 
          	i++;
          	if(i>60) i =0;
          }
}

