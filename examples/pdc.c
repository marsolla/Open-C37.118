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
#include <arpa/inet.h>

#include "c37118.h"
#include "c37118configuration.h"
#include "c37118pmustation.h"
#include "c37118data.h"
#include "c37118header.h"
#include "c37118command.h"
#include <unistd.h>
#include <iostream>

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



/**
* Process Socket Packet C37.118-2011
*/
void doprocessing (int sock, CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data,HEADER_Frame *my_header);
void select_cmd_action(int sock, CMD_Frame *cmd,CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data, HEADER_Frame *my_header);
/**
* Simulate a PMU server/dispatcher C37.118-2011
*/


int main( int argc, char *argv[] ){
    int sockfd, newsockfd, portno, clilen;
    int pid;
    unsigned char *buffer_tx, buffer_rx[SIZE_BUFFER];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    	unsigned short size ;
    	
    	
    CMD_Frame *my_cmd = new CMD_Frame();
    CONFIG_Frame *my_config2 = new CONFIG_Frame();
    CONFIG_1_Frame *my_config1 = new CONFIG_1_Frame();
    HEADER_Frame *my_header = new HEADER_Frame("");

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
    //serv_addr.sin_addr.s_addr = inet_addr("192.168.0.11");
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    if (connect(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("Connect Failed !\n");
         exit(1);
    }
    
    //Request Config 2 frame
    my_cmd->CMD_set(0x05);
    my_cmd->SOC_set(1392733934);
    my_cmd->IDCODE_set(7);
    
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
   
    //REceive Config 2 
    size = read(sockfd, buffer_rx, SIZE_BUFFER);
    my_config2->unpack(buffer_rx);
    
   // Create data with config frame received 
   DATA_Frame *my_data = new DATA_Frame(my_config2);
    
    // Request Header Frame
    
    my_cmd->CMD_set(0x03);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
    
    //REceive Header 2 
    size = read(sockfd, buffer_rx, SIZE_BUFFER);
    my_header->unpack(buffer_rx);
    cout << "INFO: " << my_header->DATA_get() << endl;
    
    
    PMU_Station *pmu_aux;
    pmu_aux = my_config2->PMUSTATION_GETbyIDCODE(2);
    cout << "PMU: " << pmu_aux->STN_get() << endl;
    
    // Enable DATA ON 
    my_cmd->CMD_set(0x02);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
    
    int k, i = 0;
    //REceive Data
    for(i = 0 ; i < 200 ; i++){
    	    size = read(sockfd, buffer_rx, SIZE_BUFFER);
    	  if (size > 0) {
    	    my_data->unpack(buffer_rx);
    	   // for (k =0 ; k < my_data->associate_current_config->pmu_station_list[0]->PHNMR_get(); k++){
    	   // 	    cout <<my_data->associate_current_config->pmu_station_list[0]->PH_NAME_get(k) << ":" << abs(my_data->associate_current_config->pmu_station_list[0]->PHASOR_VALUE_get(k)) <<"|_ " << arg(my_data->associate_current_config->pmu_station_list[0]->PHASOR_VALUE_get(k))*180/M_PI << endl;
    	   // }
    	    
    	    cout << "MY PMU---------------------------------------- "<< endl;
    	    for (k =0 ; k < pmu_aux->PHNMR_get(); k++){
    	    	    cout <<pmu_aux->PH_NAME_get(k) << ":" << abs(pmu_aux->PHASOR_VALUE_get(k)) <<"|_ " << arg(pmu_aux->PHASOR_VALUE_get(k))*180/M_PI << endl;
    	    }
    	    for (k =0 ; k < pmu_aux->ANNMR_get(); k++){
    	    	    cout <<pmu_aux->AN_NAME_get(k) << ":" << pmu_aux->ANALOG_VALUE_get(k) << endl;
    	    }
    	    
      	   // cout << "Analog Value: " << my_data->associate_current_config->pmu_station_list[0]->ANALOG_VALUE_get(0) <<endl << endl;
       	    //cout << "Analog Value: " << my_data->associate_current_config->pmu_station_list[0]->ANALOG_VALUE_get(1) <<endl;
       	  }
    }
    
    // Enable DATA OFF 
    my_cmd->CMD_set(0x01);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
   close(sockfd);
   return EXIT_SUCCESS;
} 
  
