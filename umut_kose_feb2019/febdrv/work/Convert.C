#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  uint16_t mac5;
  uint16_t flags;
  uint16_t lostcpu;
  uint16_t lostfpga;
  uint32_t ts0;
  uint32_t ts1;
  uint16_t adc[32];
  //  int s;
  //  int ms;
   uint32_t coinc;
} EVENT_t_pac;

EVENT_t_pac evbufr[4];

#define events_per_pack 1024  //number of events send per package
/* writes and then reads 10 arbitrary records
   from the file "junk". */

int Convert()
{
  int i,j;
  FILE *data;
  uint16_t mac5;
  uint16_t flags;
  uint16_t lostcpu;
  uint16_t lostfpga;
  uint32_t ts0;
  uint32_t ts1;
  uint16_t adc[32];
  uint32_t coinc; 

  //EVENT_t_pac r;
  /* read the 10 records */
  
  data=fopen("junk","r");
  if (!data)
    return 1;
  fseek(data, 0, SEEK_END); // seek to end of file

  int size = ftell(data); // get current file pointer
  cout << "size " << size << " " << sizeof(EVENT_t_pac)<< endl;

  fseek(data, 0, SEEK_SET); // seek back to beginning of file

  int size_ev=size/sizeof(EVENT_t_pac); //number of total events
  printf("Total Number of events: %d\n",size_ev);

   
  EVENT_t_pac evbuf[events_per_pack+1];
  int ev_counter=0;
 
  TFile *myfile = new TFile("mppc.root","UPDATE");
  TTree *tr=new TTree("mppc","mppc");
  tr->Branch("mac5",&mac5,"mac5/b");
  tr->Branch("flags",&flags,"flags/b");
  tr->Branch("chg",adc,"chg[32]/s");
  tr->Branch("ts0",&ts0,"ts0/i");
  tr->Branch("ts1",&ts1,"ts1/i");
  tr->Branch("coincidence",&coinc,"coinc/i"); 
  
  while(ev_counter<size_ev)
    {
      //Fill the buffer
      for (int counter=0; counter < events_per_pack; counter++)
	{
	  fread(&evbuf[counter],sizeof(EVENT_t_pac),1,data);
	  if(evbuf[counter].mac5<0xFF)
	    {
	      
	      flags = evbuf[counter].flags;
	      mac5  = evbuf[counter].mac5;
	      ts0   = evbuf[counter].ts0;
	      ts1   = evbuf[counter].ts1;
	      for(int i = 0; i<32; i++) adc[i] = evbuf[counter].adc[i];
	      coinc = evbuf[counter].coinc;
	      cout << mac5 
		   << " " << flags 
		   << " " << ts0 
		   << " " << ts1
		   << " " << coinc;
	      for(int i = 0; i<32; i++) cout << " " << adc[i];
	      cout << endl;
	      tr->Fill();
	    }
	}
      ev_counter+=events_per_pack;
      printf("sent events: %d\n", ev_counter);
    }

  tr->Write(0,TObject::kOverwrite);
  myfile->Close();
  fclose(data);

  printf("\n");
  return 0;
}


