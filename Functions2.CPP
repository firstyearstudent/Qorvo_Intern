/*
******************************************************************************
* @file    PPM_function_R100.CPP
* @brief: PPM funtion for STT test  
*  Nina 
*  R100 
*  2020-06-27 
* update item: 1.Initial program     
******************************************************************************
*  R100(101) 
*  2020-08-7
* update item: 1.optimze trim functions
******************************************************************************
*  R100(102) 
*  2020-08-10
* update item: 1.optimze shift trim code function
******************************************************************************
******************************************************************************
*  R100(103) 
*  2020-09-4
* update item: 1.add function "isOneFlagTrue" for re-trim 
*			   2.add functions to calculate test time
******************************************************************************
******************************************************************************
*  R100(104) 
*  2020-09-27
* update item: 1.Optimize sweep VI function of GNG mode
******************************************************************************
*/


#include "PPM_function_R100.h"

CMI CMI_var;
trimtable global_trim_table_mintomax[Max_trimtable_size];
myReg	Reg[MAX_REG_NUM];
PCA_Control	PCA9698;

//============== Glabe variation definition========================================//
bool Test_debug;
FILE *Test_debug_file;
bool print_test_detail=false;

int parallel_type;
int nSiteIndex;
int nResult = 0;
bool Flag[SITE_MAX]={false};
int I2C_BOX_CH_SEL;

int global_nTILE_MAX=0;

//============For Trim table &code===========//
double global_trim_table[128]={0};
double trim_table_value[Max_trimtable_size]={0};	

double global_Target=0;
double global_LSB=0;
double global_Pre[SITE_MAX]={0},global_post[SITE_MAX]={0},global_temp[SITE_MAX]={0},global_Target_array[SITE_MAX]={0},global_LSB_array[SITE_MAX]={0};
double Multi_global_Pre[MultiTile_Max][SITE_MAX]={0},Multi_global_post[MultiTile_Max][SITE_MAX]={0},Multi_global_temp[MultiTile_Max][SITE_MAX]={0},Multi_global_Target[MultiTile_Max][SITE_MAX]={0},Multi_global_LSB[MultiTile_Max][SITE_MAX]={0};

double store_Post_value_array[Max_trimtable_size]={0};
double store_Post_value[Max_trimtable_size][SITE_MAX]={0};
double Multi_store_Post_value_array[Max_trimtable_size][MultiTile_Max][SITE_MAX]={0};

unsigned char  store_Global_trimcode_array[Max_trimtable_size]={0};
unsigned char  store_Global_trimcode[Max_trimtable_size][SITE_MAX]={0};
unsigned char Multi_store_Global_trimcode_array[Max_trimtable_size][MultiTile_Max][SITE_MAX]={0};
int code_position=0;

unsigned char Global_trimcode[SITE_MAX]={0},trimcode[SITE_MAX]={0},trimcodeLDO[nTILE_MAX_2][SITE_MAX]={0};
unsigned char Multi_Global_trimcode[MultiTile_Max][SITE_MAX]={0};


int Eable_retrim_Flag[SITE_MAX]={0};

int global_count=0;
int global_count_array[SITE_MAX]={0};
int Multi_global_count[MultiTile_Max][SITE_MAX]={0};

int global_retrim_total=0;
int global_Trim_table_size=0;
int global_retrim_loop=0;
int Multi_global_retrim_loop[MultiTile_Max][SITE_MAX]={0};

int global_table_queue={0};
int global_table_queue_array[SITE_MAX]={0};
int Multi_global_table_queue[MultiTile_Max][SITE_MAX]={0};
unsigned char global_BitHigh=0,global_BitLow=0,global_ShiftBITcount=0,global_BitHigh_2nd=0,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0;
unsigned int global_trimReg_addr=0,global_trimReg_addr_2nd=0;
unsigned char Multi_global_BitHigh[MultiTile_Max]={0},Multi_global_BitLow[MultiTile_Max]={0},Multi_global_ShiftBITcount[MultiTile_Max]={0};
unsigned char Multi_global_BitHigh_2nd[MultiTile_Max]={0},Multi_global_BitLow_2nd[MultiTile_Max]={0},Multi_global_ShiftBITcount_2nd[MultiTile_Max]={0};
unsigned int Multi_global_trimReg_addr[MultiTile_Max]={0},Multi_global_trimReg_addr_2nd[MultiTile_Max]={0};

bool Eable_retrim=true;
bool global_retrim_Flag[SITE_MAX]={false};
bool Multi_global_retrim_Flag[MultiTile_Max][SITE_MAX]={false};

bool global_trim_tend=true;
bool global_trim_tend_array[SITE_MAX]={false};
bool Multi_global_trim_tend[MultiTile_Max][SITE_MAX]={false};

//============For Trim table &code end===========//
//-----------digital definition---------------//
char sResrcName[128] = "RIO0", sErrorMsg[256] = "";
int nChannelNum = 0, nErrorMsgLen = 256;


int naPatternFinished[8],nPatternFinishedLen = 2,naPatternPassed[8],nPatternPassedLen = 2,naAcquiredCycles[8],nAcquiredCyclesLen =  2,
naFirstFailedCycle[8],nFirstFailedCycleLen = 2,naTotalCycles[8],nTotalCyclesLen = 2,nFailedPinListLen = 2;
char sFailedPinList[128];
char sOperationMode[128]="NORMAL";
char sSITE[128]="0,1";// Dual sites
int nx=0;
double Revsion=0.0;
double output1[10];
double output2[10];
//-----------digital definition end---------------//

//============== Glabe variation definition end ========================================//
//------record test time------------//
int DISP=0;
bool comment_flag=false;
LARGE_INTEGER nFreq;
LARGE_INTEGER nBeginTime;
LARGE_INTEGER nEndTime;

char* FullPath(char* s1)
{
	static char s[256];
	char *path=getenv("SHELL_TP_PATH");
	if(!path)
	printf("Must be started with tp_start.bat\n");
	sprintf(s,"%s%s",path,s1);
	printf("%s\n",s);
	return s;
}
void savetesttime(char *s,int FLG)
{
	stoptest(FLG);

	if(FLG==1)
	{
		double TT1;
		TT1=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart*1000;
		string testfilepath ="C:\\VisualTest\\RecordTestTime.csv";
		
		FILE *testtime_file = NULL;
		testtime_file=fopen(testfilepath.c_str(), "a+");
	
		if(comment_flag==false)
		{
			fprintf(testtime_file,"Test_item, Test_time(mS)\n");
			comment_flag=true;
		}
		fprintf(testtime_file,"%s, %5.1f\n",s,TT1);
		fclose (testtime_file);
	}
}

void clrdata(int FLG)
{
	if(FLG==1)
	{
		string testfilepath ="C:\\VisualTest\\RecordTestTime.csv";
		
		FILE *testtime_file = NULL;
		testtime_file=fopen(testfilepath.c_str(), "w+");
		fclose (testtime_file);
		comment_flag=false;
	}
}

void starttest(int FLG)
{
	if(FLG==1)
	{
		QueryPerformanceFrequency(&nFreq);	 
		QueryPerformanceCounter(&nBeginTime); 
	}
}
void stoptest(int FLG)
{
	if(FLG==1)
		QueryPerformanceCounter(&nEndTime);
}
//-------------------------end-----------------------------------//
//============== Glabe function definition  ========================================//
//-------------i2c definition-------------------//
void i2c_init(bool mode_sel, int clockspeed_set, int channel_sel)
{
	//Only at SCTL mode,if Dual sites channel_sel=2, if Quals channel_sel=1;
	
	if (mode_sel==HCTL_MODE)//HW mode
	{
		I2C_BOX_CH_SEL=1;
		
		long ret;
		VII_INIT_CONFIG I2C_Config;
		ret = VII_ScanDevice(1);
		delayms(5);
		ret = VII_OpenDevice(VII_USBI2C, 0, 0);
		I2C_Config.AddrType = VII_ADDR_7BIT;
		I2C_Config.ClockSpeed = clockspeed_set;
		I2C_Config.ControlMode = VII_HCTL_MODE;
		I2C_Config.MasterMode = VII_MASTER;
		I2C_Config.Addr1 = 0;
		I2C_Config.SubAddrWidth = VII_SUB_ADDR_1BYTE;
		ret = VII_InitI2C(VII_USBI2C, 0, 0, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 1, &I2C_Config);
	}
	if (mode_sel==SCTL_MODE)//soft mode
	{
		I2C_BOX_CH_SEL=channel_sel;
		
		long ret;
		VII_INIT_CONFIG I2C_Config;
		ret = VII_ScanDevice(1);
		delayms(2);
		ret = VII_OpenDevice(VII_USBI2C, 0, 0);
		I2C_Config.AddrType = VII_ADDR_7BIT;
		I2C_Config.ClockSpeed = clockspeed_set;
		I2C_Config.ControlMode = VII_SCTL_MODE;
		I2C_Config.MasterMode = VII_MASTER;
		I2C_Config.Addr1 = 0;
		I2C_Config.SubAddrWidth = VII_SUB_ADDR_1BYTE;
		ret = VII_InitI2C(VII_USBI2C, 0, 0, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 1, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 2, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 3, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 4, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 5, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 6, &I2C_Config);
		ret = VII_InitI2C(VII_USBI2C, 0, 7, &I2C_Config);
		
		//Config timing,Unit of time for microseconds
		
		/*	VII_TIME_CONFIG I2C_TimeConfig;
		I2C_TimeConfig.tSU_STA = 5;
		I2C_TimeConfig.tHD_STA = 4;
		I2C_TimeConfig.tLOW = 3.33*0.6; 
		I2C_TimeConfig.tHIGH =  3.33*0.4;
		I2C_TimeConfig.tSU_DAT = 1;
		I2C_TimeConfig.tSU_STO = 4;
		I2C_TimeConfig.tBuf = 5;  
		ret = VII_TimeConfig(VII_USBI2C, 0, 0, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 1, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 2, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 3, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 4, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 5, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 6, &I2C_TimeConfig);
		ret = VII_TimeConfig(VII_USBI2C, 0, 7, &I2C_TimeConfig);
		*/
		
		// 		unsigned short tHD_STA;   //????????
		// 		unsigned short tSU_STA;   //????????
		// 		unsigned short tLOW;      //???????
		// 		unsigned short tHIGH;     //???????
		// 		unsigned short tSU_DAT;   //????????
		// 		unsigned short tSU_STO;   //????????
		// 		unsigned short tDH;       //????????
		// 		unsigned short tDH_DAT;   //????????
		// 		unsigned short tAA;       //SCL???SDA?????????
		// 		unsigned short tR;        //SDA?SCL????
		// 		unsigned short tF;        //SDA?SCL????
		// 		unsigned short tBuf;      //?????????????
		// 		unsigned char tACK[4];
		// 		unsigned short tStart;
		// 		unsigned short tStop;
	}
	
}

//---------------------------------------------//
//=========================digital initial========================//
void Digital_init()
{
	Initialise_full_path();
		//GETuPDLL();
	nx=nx+1;
	if(nx<2)
	{	 SetOperationMode(sOperationMode, sErrorMsg, nErrorMsgLen);
	//Initialization
	nResult = Initialization(sResrcName,&nChannelNum,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("Initialization has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
		//	return TRUE;
	}
	else
	{
		printf ("Number of channels = %d\n", nChannelNum);
	}
	
	
	//Pin Channel map 
	nResult = ChannelMapFromFile(Scan_sPinGrpList,Scan_sChannelMapFile,Scan_sPinMapFile,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("ChannelMapFromFile has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
		//	return TRUE;
	}
	else
	{
		printf ("ChannelMapFromFile Successful.\n");
	}

	//Set Pin Level
	nResult = SetDriverComLoadFromFileByPinGrp(Scan_sPinGrpList,Scan_sPinLevelFile,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("SetDriverComLoadFromFileByPinGrp has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
		//	return TRUE;
	}
	else
	{
		printf ("Set Pin Level Successful.\n");
	}

	//Download timeset
	nResult = DownloadTSFromFileByPinGrp(Scan_sPinGrpList,Scan_sTimeSetFile,Scan_sTimeSetList,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadTSFromFileByPinGrp has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
		//	return TRUE;
	}
	else
	{
		printf ("Download timeset Successful.\n");
	}
	
	//I2C_Pattern_Download
	
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,sPattern_W55,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadPattern has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download sPattern_W55 Successful.\n");
	}
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,sPattern_WAA,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadPattern has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download sPattern_WAA Successful.\n");
	}
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,sPattern_W55_1,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadPattern has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download sPattern_W55 Successful.\n");
	}
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,sPattern_WAA_1,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadPattern has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download sPattern_WAA Successful.\n");
	}
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,sPattern_W4A,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("DownloadPattern has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download sPattern_W4A Successful.\n");
	}
	
	//Scan_Pattern1_Download
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,Scan_file1,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("Scan_file1 has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download Scan_file1 Successful.\n");
	}
	
	//Scan_Pattern2_Download
	nResult = DownloadPatternByPinGrp(Scan_sPinGrpList,Scan_file2,sErrorMsg,nErrorMsgLen);
	if(nResult < 0)
	{
		printf ("Scan_file2 has error and ErrorMsg is:\n");
		printf (sErrorMsg);
		printf("\n");
		output1[1]=-12;	output1[2]=-12;
	}
	else
	{
		printf ("Download Scan_file2 Successful.\n");
	}
	naPatternPassed[0]=0;naPatternPassed[1]=0;	
	}
}
//-------------------------------------------------------------------------------------//

//----------Get CMI-----------------------------------------------------//
bool getCMI( char *path, CMI* CMI_var1)
{
	char *name,*Code,*codingmatrixid,*rawpartnumber;
	char *const_name;
	unsigned char result[100];
	bool valid=FALSE;
	unsigned int vrui_size_block2 = 0;          // block2: value_register
	unsigned int vrui_size_block3 = 0;         // block3: groupid + expansion

	//int CPQ;
	//int count_IC;
	unsigned int size;
	//------------------------------------------------------------------
	//CMI_var1->valid=false;
	CMI_var1->RegisterSize=0;
	CMI_var1->rawpartnumber_length=0;
	CMI_var1->codingmatrix_length=0;
	CMI_var1->Code_length=0;

	//-----------------------------------------------------------------
	int Mcode[]={179,102,43,2,235,230,243,18,67,134,219,66,187,70,227,146,83,38,11,2,11,38,83,146,227,70,187,66,219,134,67,18,243,230,235,2,43,102,179,18,131,6,155,66,251,198,163,146,147,166,203,2,75,166,19,146,35,198,123,66,27,6,3,18,51,102,171,2,107,230,115,18,195,134,91,66,59,70,99,146,211,38,139,2,139,38,211,146,99,70,59,66,91,134,195,18,115,230,107,2,171,102,51,18,3,6,27,66,123,198,35,146,19,166,75,2,203,166,147,146,163,198,251,66,155,6,131,18};
	int count;
	int number; 
	int j;
	//int size;
	const_name = (char*)malloc(31);
	const_name="E0C8BB77FF3703F59C0A1B4A983992C";
	////////////////////////////////////////////////////////////////////////////////////////////

	int H1 = _open(path,_O_BINARY);
	if (H1==-1) return valid;
	count=0;
	_read(H1,result,4);
	_read(H1,result,32);
	name = (char*)malloc(31);
	//realloc(name,2);
	for (j=1 ; j<=31;j++)
	{
		result[j]= (result[j] ^ Mcode[count]);
		name[j-1]= char( result[j] );
		//------------------------------------------------------------------
		CMI_var1->Code[j-1]=name[j-1];
		CMI_var1->Code_length+=1;
	}
	Code=name;
	count =count+1;		

	_read(H1,result,9);
	count =count+1;

	_read(H1,result,4);
	result[0]=int( result[0]^Mcode[count]);
	result[1]= int(result[1]^Mcode[count]);
	result[3]= int(result[3]^Mcode[count]);

	//------------------------------------------------------------------
	CMI_var1->Device_ID=result[3];

	//////////////////////////////////////////////////////////////////////
	unsigned char CMID_length = 8;// result[0]*256+result[1]-1;
	_read(H1,result,CMID_length);
	name = (char*)malloc(CMID_length);
	for(j=0;j<CMID_length;j++)
	{
		result[j]=(result[j] ^ Mcode[count]);
		name[j]=char(result[j]);
		//------------------------------------------------------------------
		CMI_var1->codingmatrixid[j]=name[j];
		CMI_var1->codingmatrix_length+=1;
	}
	codingmatrixid =name;
	count =count+1;
	//------------------------------------------------------------------  
	_read(H1,result,4);
	result[0]= result[0]^Mcode[count];
	result[1]= result[1]^Mcode[count];
	unsigned char PN_length = result[0]*256+result[1];
	name="";
	_read(H1,result,PN_length);
	name = (char*)malloc(PN_length);
	for(j=0;j<PN_length;j++)
	{
		result[j]= (result[j] ^ Mcode[count]  ) ;
		if(result[j] !=0)
		name [j]= char(result[j]);
		//------------------------------------------------------------------
		CMI_var1->rawpartnumber[j]=name[j];
		CMI_var1->rawpartnumber_length+=1;
	}
	rawpartnumber=name;
	count =count+1;
	//------------------------------------------------------------------
  	size=0;
	do
	{
		int kt=_read(H1,result,4);
		if(kt==0) break;
		
		
		
		for(j=0;j<=3;j++)
			result[j]=result[j] ^ Mcode[count];
		if (result[2] == 2)
		{
			unsigned char address = result[3];//unsingned, tung			
   
   
			number = result[0]*256+result[1]-1;
			name="";
			_read(H1,result,number+1);

			for(j=0;j<=number;j++)
			{
				result[j]= (result[j] ^ Mcode[count]);
				if(result[j] !=0)
				name +=char(result[j]);
			}
			CMI_var1->registers[size].address=address;			
		   
		}
		if (result[2] == 3)
		{
			vrui_size_block2 ++;
			number = result[0]*256+result[1]-1;
			name="";
			_read(H1,result,number+1);
			unsigned char temp=0,exp=128;
			for(j=0;j<=number;j++)
			{
				result[j]= (result[j] ^ Mcode[count]  ) ;
				if(result[j] !=0)
				{
					name +=char(result[j]);
					if (result[j]==48 || result[j]==49)
						temp += exp*(result[j]-48);
				}
				if(result[j]==44)
					exp/=2;
			}
			CMI_var1->registers[size].value=temp;
		}
		if (result[2] == 4)
		{
			vrui_size_block3 ++;
			CMI_var1->vrb_new_format = true ;
			number = result[0]*256+result[1]-1;
			_read(H1,result,number+1);
			for(j=0;j<=number;j++)
			{
				result[j]= (result[j] ^ Mcode[count]);
			}
			CMI_var1->registers[size].vruc_expansion = result[0];
			CMI_var1->registers[size].vruc_groupid  = result[1];
			size ++;
		}
		
		//------------------------------------------------------------------
		count =count+1;
		if( count >127)
			count = count-128;
		
	} while (1);
	CMI_var1->RegisterSize=size;
	_close(H1);
	//------------------------------------------------------------------
	if(	(CMI_var1->RegisterSize!=0)&&
		(CMI_var1->rawpartnumber_length!=0)&&
		(CMI_var1->codingmatrix_length!=0)&&
		(CMI_var1->Code_length!=0)&&
		(CMI_var1->RegisterSize<512)&&
		(CMI_var1->rawpartnumber_length<254)&&
		(CMI_var1->codingmatrix_length<254)&&
		(CMI_var1->Code_length<254) &&
		(size == vrui_size_block2) &&
		(size == vrui_size_block3)
	) valid=true;
	return valid;
 }


//===========SEARCH DATA========================================================
float data_search(unsigned char address, unsigned char groupid, unsigned char expansion, CMI data_CMI)
{
	float data_got;
	for(int i=0;i<data_CMI.RegisterSize;i++)
	{			
		if ((address == data_CMI.registers[i].address) && (groupid == data_CMI.registers[i].vruc_groupid) && (expansion == data_CMI.registers[i].vruc_expansion))
		{
			data_got = data_CMI.registers[i].value;
			break;
		}
	}
	return data_got;
}

//--------------------------------------------------------------------------------------------------//
bool isAllSiteTrue(bool SiteFlag[SITE_MAX])
{
	bool FlagTemp = true;
	FOR_EACH_SITE(nSiteIndex)
		FlagTemp &= SiteFlag[nSiteIndex];
	return FlagTemp;
}

bool isAllFlagSet(bool clrFlag)
{
	bool FlagTemp = true;
	FOR_EACH_SITE(nSiteIndex)
		FlagTemp &= Flag[nSiteIndex];
	if(FlagTemp && clrFlag)
		clrAllFlag();
	return FlagTemp;
}

bool clrAllFlag()
{
	FOR_EACH_SITE(nSiteIndex)
		Flag[nSiteIndex] = false;
	return true;
}

bool isOneFlagTrue( bool SiteFlag[SITE_MAX])
{
	bool FlagTemp = false;
	FOR_EACH_SITE(nSiteIndex)
	{
		FlagTemp |= SiteFlag[nSiteIndex];
	}
	
	return FlagTemp;
}

bool isAllFlagClr_MultiTile(int tile_max, bool retrim_Flag[][SITE_MAX])
{
	bool FlagTemp = true;
	for (int tile = 0; tile < tile_max; tile++)
	{
		FOR_EACH_SITE(nSiteIndex)
			FlagTemp &= (!retrim_Flag[tile][nSiteIndex]);
	}

	return FlagTemp;
}


//---------------------------------------------//
bool sweep_VI(		char sweep_resource, char sweep_pin, bool sweep_VorI, 
					char measure_resource, char measure_pin, bool measure_VorI,
					double sweep_start, double sweep_stop, double sweep_step, 
					bool measure_GreaterorLess, double measure_flagvalue, 
					double return_result[SITE_MAX], int wait_time_us, bool GNG)
{

	if(sweep_start<sweep_stop)
		sweep_step = fabs(sweep_step);
	else
		sweep_step = -fabs(sweep_step);
	double measure_val[SITE_MAX];
	double sweep_val;

	double sweep_start1,sweep_step1,sweep_loop,step_count;
	
	//*******************************************************//
	//*add more step before force sweep_start 20200927
	//******************************************************//
	sweep_loop=5;	
	step_count=5;

	sweep_start1=sweep_start-step_count*sweep_step;
	
	sweep_step1=fabs(sweep_start1-sweep_start)/sweep_loop;
	
	if(sweep_start1<sweep_stop)
		sweep_step1 = fabs(sweep_step1);
	else
		sweep_step1 = -fabs(sweep_step1);
			
	sweep_val = sweep_start1;
	for(int i=0 ; i<sweep_loop ; i++)
	{	
		if		(sweep_resource == RESOURCE_FVI16)
			if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val);
			else						Fvi16.CH[sweep_pin].fi(sweep_val);
		else if	(sweep_resource == RESOURCE_FVI45)
			if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val);
			else						Fvi45.CH[sweep_pin].fi(sweep_val);
		else if	(sweep_resource == RESOURCE_FQVI)
			if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_val);
			else						Fqvi.CH[sweep_pin]	.fi(sweep_val);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex]=-9999;
				return false;
		}
		sweep_val += sweep_step1;			
		delay(wait_time_us);
	}
	//***************************************************//
	//*add more step before force sweep_start end
	//**************************************************//

	if(!GNG)
	{
		sweep_val = sweep_start;
		
		for(int i=0 ; i<int((sweep_stop-sweep_start)/sweep_step) ; i++)
		{
			//	1. sweep on one pin
			if		(sweep_resource == RESOURCE_FVI16)
				if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val);
				else						Fvi16.CH[sweep_pin].fi(sweep_val);
			else if	(sweep_resource == RESOURCE_FVI45)
				if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val);
				else						Fvi45.CH[sweep_pin].fi(sweep_val);
			else if	(sweep_resource == RESOURCE_FQVI)
				if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_val);
				else						Fqvi.CH[sweep_pin]	.fi(sweep_val);
			else
			{
				FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex]=-9999;
				return false;
			}

			// 2. wait another pin response
			delay(wait_time_us);
			// 3. measure another pin
			if		(measure_resource == RESOURCE_FVI16)
				if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val);
				else						Fvi16.CH[measure_pin]	.mi(measure_val);
			else if	(measure_resource == RESOURCE_FVI45)
				if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val);
				else						Fvi45.CH[measure_pin]	.mi(measure_val);
			else if	(measure_resource == RESOURCE_FQVI)
				if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val);
				else						Fqvi.CH[measure_pin]	.mi(measure_val);
			else
			{
				FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex] = -9999;
				return false;
			}

			// 4. If measure_val reach flagvalue, return sweep_val - sweep_step/2
			FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
			{

				if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue) ||
					(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue))
				{
					return_result[nSiteIndex] = sweep_val - sweep_step/2;
					Flag[nSiteIndex] = true;
				}
				else
				{
					
					return_result[nSiteIndex] = sweep_val+2*sweep_step;
				}
			}
			sweep_val += sweep_step;
			if(isAllFlagSet(false))
				break;
		}
	}
	//----------------------------------------------GNG_test---------------------------------------//
	else
	{
		//	1. Force input pin to start value
		if		(sweep_resource == RESOURCE_FVI16)
			if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_start);
			else						Fvi16.CH[sweep_pin].fi(sweep_start);
		else if	(sweep_resource == RESOURCE_FVI45)
			if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_start);
			else						Fvi45.CH[sweep_pin].fi(sweep_start);
		else if	(sweep_resource == RESOURCE_FQVI)
			if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_start);
			else						Fqvi.CH[sweep_pin]	.fi(sweep_start);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex]=-9999;
			return false;
		}
		// 2. wait another pin response
		delay(wait_time_us);
		// 3. measure another pin
		if		(measure_resource == RESOURCE_FVI16)
			if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val);
			else						Fvi16.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FVI45)
			if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val);
			else						Fvi45.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FQVI)
			if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val);
			else						Fqvi.CH[measure_pin]	.mi(measure_val);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex] = -9999;
			return false;
		}
		// 4. If measure_val already toggle, return start_val - sweep_step/2
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue) ||
				(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue))
			{
				return_result[nSiteIndex] = sweep_start - sweep_step/2;
				Flag[nSiteIndex] = true;
			}
		}
		//**************************************//
		//*sweep to stop value step by step  20200927 
		//**************************************//
		sweep_step=fabs(sweep_start-sweep_stop)/sweep_loop;
		
		if(sweep_start<sweep_stop)
			sweep_step = fabs(sweep_step);
		else
			sweep_step = -fabs(sweep_step);
		
		sweep_val = sweep_start;
		//	1. sweep for set initial status
		for(int i=0 ; i<sweep_loop ; i++)
		{	
			if		(sweep_resource == RESOURCE_FVI16)
				if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val);
				else						Fvi16.CH[sweep_pin].fi(sweep_val);
			else if	(sweep_resource == RESOURCE_FVI45)
				if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val);
				else						Fvi45.CH[sweep_pin].fi(sweep_val);
			else if	(sweep_resource == RESOURCE_FQVI)
				if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin] .fv(sweep_val);
				else						Fqvi.CH[sweep_pin] .fi(sweep_val);
			else
			{
				FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex]=-9999;
					return false;
			}
			sweep_val += sweep_step;			
		    delay(wait_time_us);
		}
		//**************************************//
		//*sweep to stop value step by step end
		//**************************************//

		//	5. Force input pin to stop value
		if		(sweep_resource == RESOURCE_FVI16)
			if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_stop);
			else						Fvi16.CH[sweep_pin].fi(sweep_stop);
		else if	(sweep_resource == RESOURCE_FVI45)
			if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_stop);
			else						Fvi45.CH[sweep_pin].fi(sweep_stop);
		else if	(sweep_resource == RESOURCE_FQVI)
			if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_stop);
			else						Fqvi.CH[sweep_pin]	.fi(sweep_stop);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex]=-9999;
			return false;
		}
		// 6. wait another pin response
		delay(wait_time_us);
		// 7. measure another pin
		if		(measure_resource == RESOURCE_FVI16)
			if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val);
			else						Fvi16.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FVI45)
			if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val);
			else						Fvi45.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FQVI)
			if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val);
			else						Fqvi.CH[measure_pin]	.mi(measure_val);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex] = -9999;
			return false;
		}
		// 8. If measure_val haven't toggle, return stop_val + sweep_step/2
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]<measure_flagvalue) ||
				(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]>measure_flagvalue))
			{
				return_result[nSiteIndex] = sweep_stop + sweep_step/2;
				Flag[nSiteIndex] = true;
			}
		}
		// 9. If no toggle at start, and toggle at stop, return (start_val+stop_val)/2
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			return_result[nSiteIndex] = (sweep_start+sweep_stop)/2;
			Flag[nSiteIndex] = true;
		}
	}
	return clrAllFlag();
}

bool sweep_VI_var_pts(		char sweep_resource, char sweep_pin, bool sweep_VorI, 
					char measure_resource, char measure_pin, bool measure_VorI,
					double sweep_start[SITE_MAX], double sweep_stop[SITE_MAX], double sweep_step[SITE_MAX], 
					bool measure_GreaterorLess, double measure_flagvalue[SITE_MAX], 
					double return_result[SITE_MAX], int wait_time_us, bool GNG)
{
	double measure_val[SITE_MAX];
	double sweep_val[SITE_MAX];
	double sweep_start1[SITE_MAX],sweep_step1[SITE_MAX],sweep_loop,step_count;
	FOR_EACH_SITE(nSiteIndex)
	{
		if(sweep_start[nSiteIndex]<sweep_stop[nSiteIndex])
			sweep_step[nSiteIndex] = fabs(sweep_step[nSiteIndex]);
		else
			sweep_step[nSiteIndex] = -fabs(sweep_step[nSiteIndex]);

		
		////
		//add more step before force sweep_start 20200927
		////
		sweep_loop=5;	
		step_count=5;

		sweep_start1[nSiteIndex]=sweep_start[nSiteIndex]-step_count*sweep_step[nSiteIndex];
		
		sweep_step1[nSiteIndex]=fabs(sweep_start1[nSiteIndex]-sweep_start[nSiteIndex])/sweep_loop;
		
		if(sweep_start1[nSiteIndex]<sweep_stop[nSiteIndex])
			sweep_step1[nSiteIndex] = fabs(sweep_step1[nSiteIndex]);
		else
			sweep_step1[nSiteIndex] = -fabs(sweep_step1[nSiteIndex]);
				
		sweep_val[nSiteIndex] = sweep_start1[nSiteIndex];
	}
	for(int i=0 ; i<sweep_loop ; i++)
	{	
		FOR_EACH_SITE(nSiteIndex)
		{
			if	(sweep_resource == RESOURCE_FVI16)
				if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
				else						Fvi16.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FVI45)
				if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
				else						Fvi45.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FQVI)
				if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_val[nSiteIndex],nSiteIndex);
				else						Fqvi.CH[sweep_pin]	.fi(sweep_val[nSiteIndex],nSiteIndex);
			else
			{
				return_result[nSiteIndex]=-9999;
				return false;
			}
		}
			sweep_val[nSiteIndex] += sweep_step1[nSiteIndex];			
			delay(wait_time_us);
	}
	////
	//add more step before force sweep_start end
	////

	if(!GNG)
	{
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			sweep_val[nSiteIndex] = sweep_start[nSiteIndex];
		
			for(int i=0 ; i<int((sweep_stop[nSiteIndex]-sweep_start[nSiteIndex])/sweep_step[nSiteIndex]) ; i++)
			{
					//	1. sweep on one pin
					if		(sweep_resource == RESOURCE_FVI16)
						if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
						else						Fvi16.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
					else if	(sweep_resource == RESOURCE_FVI45)
						if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
						else						Fvi45.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
					else if	(sweep_resource == RESOURCE_FQVI)
						if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_val[nSiteIndex],nSiteIndex);
						else						Fqvi.CH[sweep_pin]	.fi(sweep_val[nSiteIndex],nSiteIndex);
					else
					{

							return_result[nSiteIndex]=-9999;
						return false;
					}

					// 2. wait another pin response
					delay(wait_time_us);
					// 3. measure another pin
					if		(measure_resource == RESOURCE_FVI16)
						if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val,nSiteIndex);
						else						Fvi16.CH[measure_pin]	.mi(measure_val,nSiteIndex);
					else if	(measure_resource == RESOURCE_FVI45)
						if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val,nSiteIndex);
						else						Fvi45.CH[measure_pin]	.mi(measure_val,nSiteIndex);
					else if	(measure_resource == RESOURCE_FQVI)
						if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val,nSiteIndex);
						else						Fqvi.CH[measure_pin]	.mi(measure_val,nSiteIndex);
					else
					{
						return_result[nSiteIndex] = -9999;
						return false;
					}

					// 4. If measure_val reach flagvalue, return sweep_val - sweep_step/2
					//FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
					//{

						if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue[nSiteIndex]) ||
							(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue[nSiteIndex]))
						{
							return_result[nSiteIndex] = sweep_val[nSiteIndex] - sweep_step[nSiteIndex]/2;
							Flag[nSiteIndex] = true;
						}
						else
						{
							
							return_result[nSiteIndex] = sweep_val[nSiteIndex]+2*sweep_step[nSiteIndex];
						}
					//}
				sweep_val[nSiteIndex] += sweep_step[nSiteIndex];
				if(Flag[nSiteIndex])
					break;
				
			}
		}
	}
	//----------------------------------------------GNG_test---------------------------------------//
	else
	{
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
						//	1. Force input pin to start value
			if		(sweep_resource == RESOURCE_FVI16)
				if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_start[nSiteIndex],nSiteIndex);
				else						Fvi16.CH[sweep_pin].fi(sweep_start[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FVI45)
				if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_start[nSiteIndex],nSiteIndex);
				else						Fvi45.CH[sweep_pin].fi(sweep_start[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FQVI)
				if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_start[nSiteIndex],nSiteIndex);
				else						Fqvi.CH[sweep_pin]	.fi(sweep_start[nSiteIndex],nSiteIndex);
			else
			{
				//FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex]=-9999;
				return false;
			}
			// 2. wait another pin response
			delay(wait_time_us);
			// 3. measure another pin
			if		(measure_resource == RESOURCE_FVI16)
				if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fvi16.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else if	(measure_resource == RESOURCE_FVI45)
				if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fvi45.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else if	(measure_resource == RESOURCE_FQVI)
				if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fqvi.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else
			{
				//FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex] = -9999;
				return false;
			}
			// 4. If measure_val already toggle, return start_val - sweep_step/2
			//FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
			//{
				if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue[nSiteIndex]) ||
					(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue[nSiteIndex]))
				{
					return_result[nSiteIndex] = sweep_start[nSiteIndex] - sweep_step[nSiteIndex]/2;
					Flag[nSiteIndex] = true;
				}
			//}
			////
			//sweep to stop value step by step  20200927 
			////
			sweep_step[nSiteIndex]=fabs(sweep_start[nSiteIndex]-sweep_stop[nSiteIndex])/sweep_loop;
			
			if(sweep_start[nSiteIndex]<sweep_stop[nSiteIndex])
				sweep_step[nSiteIndex] = fabs(sweep_step[nSiteIndex]);
			else
				sweep_step[nSiteIndex] = -fabs(sweep_step[nSiteIndex]);
			
			sweep_val[nSiteIndex] = sweep_start[nSiteIndex];
			//	1. sweep for set initial status
			for(int i=0 ; i<sweep_loop ; i++)
			{	
				if		(sweep_resource == RESOURCE_FVI16)
					if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
					else						Fvi16.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
				else if	(sweep_resource == RESOURCE_FVI45)
					if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val[nSiteIndex],nSiteIndex);
					else						Fvi45.CH[sweep_pin].fi(sweep_val[nSiteIndex],nSiteIndex);
				else if	(sweep_resource == RESOURCE_FQVI)
					if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin] .fv(sweep_val[nSiteIndex],nSiteIndex);
					else						Fqvi.CH[sweep_pin] .fi(sweep_val[nSiteIndex],nSiteIndex);
				else
				{
					//FOR_EACH_SITE(nSiteIndex)
						return_result[nSiteIndex]=-9999;
						return false;
				}
				sweep_val[nSiteIndex] += sweep_step[nSiteIndex];			
				delay(wait_time_us);
			}
			////
			//sweep to stop value step by step end
			////

			//	5. Force input pin to stop value
			if		(sweep_resource == RESOURCE_FVI16)
				if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_stop[nSiteIndex],nSiteIndex);
				else						Fvi16.CH[sweep_pin].fi(sweep_stop[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FVI45)
				if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_stop[nSiteIndex],nSiteIndex);
				else						Fvi45.CH[sweep_pin].fi(sweep_stop[nSiteIndex],nSiteIndex);
			else if	(sweep_resource == RESOURCE_FQVI)
				if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_stop[nSiteIndex],nSiteIndex);
				else						Fqvi.CH[sweep_pin]	.fi(sweep_stop[nSiteIndex],nSiteIndex);
			else
			{
				//FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex]=-9999;
				return false;
			}
			// 6. wait another pin response
			delay(wait_time_us);
			// 7. measure another pin
			if		(measure_resource == RESOURCE_FVI16)
				if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fvi16.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else if	(measure_resource == RESOURCE_FVI45)
				if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fvi45.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else if	(measure_resource == RESOURCE_FQVI)
				if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val,nSiteIndex);
				else						Fqvi.CH[measure_pin]	.mi(measure_val,nSiteIndex);
			else
			{
				//FOR_EACH_SITE(nSiteIndex)
					return_result[nSiteIndex] = -9999;
				return false;
			}
			// 8. If measure_val haven't toggle, return stop_val + sweep_step/2
			//FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
			//{
				if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]<measure_flagvalue[nSiteIndex]) ||
					(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]>measure_flagvalue[nSiteIndex]))
				{
					return_result[nSiteIndex] = sweep_stop[nSiteIndex] + sweep_step[nSiteIndex]/2;
					Flag[nSiteIndex] = true;
				}
			//}
			// 9. If no toggle at start, and toggle at stop, return (start_val+stop_val)/2
			//FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
			//{
				return_result[nSiteIndex] = (sweep_start[nSiteIndex]+sweep_stop[nSiteIndex])/2;
				Flag[nSiteIndex] = true;
			//}
		}

	}
	return clrAllFlag();
}




bool sweep_VI_meas_code(	char sweep_resource, char sweep_pin, bool sweep_VorI, 
							unsigned int measure_code_address, unsigned char measure_code_bit,
							double sweep_start, double sweep_stop, double sweep_step, 
							bool measure_To1orTo0,
							double return_result[SITE_MAX], int wait_time_us)
{
	if(sweep_start<sweep_stop)
		sweep_step = fabs(sweep_step);
	else
		sweep_step = -fabs(sweep_step);
	double sweep_val = sweep_start;
	
	for(int i=0 ; i<int((sweep_stop-sweep_start)/sweep_step) ; i++)
	{
		//	1. sweep on one pin
		if		(sweep_resource == RESOURCE_FVI16)
			if(sweep_VorI == VOLTAGE)	Fvi16.CH[sweep_pin].fv(sweep_val);
			else						Fvi16.CH[sweep_pin].fi(sweep_val);
		else if	(sweep_resource == RESOURCE_FVI45)
			if(sweep_VorI == VOLTAGE)	Fvi45.CH[sweep_pin].fv(sweep_val);
			else						Fvi45.CH[sweep_pin].fi(sweep_val);
		else if	(sweep_resource == RESOURCE_FQVI)
			if(sweep_VorI == VOLTAGE)	Fqvi.CH[sweep_pin]	.fv(sweep_val);
			else						Fqvi.CH[sweep_pin]	.fi(sweep_val);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
				return_result[nSiteIndex]=-9999;
			return false;
		}
		// 2. wait register change
		delay(wait_time_us);
		// 3. read register
		Reg[measure_code_address].read();
		// 4. target bit already set, return sweep_val - sweep_step/2
		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			if(	(((Reg[measure_code_address].Data[nSiteIndex] & measure_code_bit) == measure_code_bit) && ( measure_To1orTo0)) ||
				(((Reg[measure_code_address].Data[nSiteIndex] & measure_code_bit) != measure_code_bit) && (!measure_To1orTo0)) )
			{
				return_result[nSiteIndex] = sweep_val - sweep_step/2;
				Flag[nSiteIndex] = true;
			}
			else
			{
				sweep_val += sweep_step;
				return_result[nSiteIndex] = sweep_val;
			}
		}
		if(isAllFlagSet(false))
			break;
	}
	return clrAllFlag();
}

bool sweep_code(unsigned char sweep_code_max, unsigned int sweep_code_address, unsigned char sweep_code_queue[], 
				unsigned char sweep_code_high_bit,unsigned char sweep_code_low_bit,
				char measure_resource,		char measure_pin,			bool measure_VorI,
				bool measure_GreaterorLess, double measure_flagvalue, 
				unsigned char return_code[SITE_MAX], bool return_isfound[SITE_MAX], int delay_us, 
				bool double_address,	unsigned int sweep_code2_address,	
				unsigned char sweep_code2_high_bit, unsigned char sweep_code2_low_bit, char sweep_code2_shiftbitcount)
{
	double measure_val[SITE_MAX];
	for(unsigned char i=0x00; i<sweep_code_max; i++)
	{
		Reg[sweep_code_address].write_Data_trimvalue(sweep_code_high_bit,sweep_code_low_bit,sweep_code_queue[i]);
		if(double_address)
			Reg[sweep_code2_address].write_Data_trimvalue(sweep_code2_high_bit,sweep_code2_low_bit,sweep_code_queue[i],sweep_code2_shiftbitcount);
		delay(delay_us);
		//--for debug---//
		//Reg[sweep_code_address].read(readback);
		//Reg[sweep_code2_address].read(readback);


		if		(measure_resource == RESOURCE_FVI16)
			if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val);
			else						Fvi16.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FVI45)
			if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val);
			else						Fvi45.CH[measure_pin]	.mi(measure_val);
		else if	(measure_resource == RESOURCE_FQVI)
			if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val);
			else						Fqvi.CH[measure_pin]	.mi(measure_val);
		else
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				return_isfound[nSiteIndex] = false;
				return_code[nSiteIndex] = 255;
			}
			return false;
		}

		FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
		{
			if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue) ||
				(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue))
			{
				return_isfound[nSiteIndex] = true;
				return_code[nSiteIndex] = sweep_code_queue[i];
				Flag[nSiteIndex] = true;
			}
			else if(i == sweep_code_max-1)
			{
				return_isfound[nSiteIndex] = false;
				return_code[nSiteIndex] = sweep_code_queue[i];
				Flag[nSiteIndex] = true;
			}
		}

		if(isAllFlagSet(true))
			break;
	}
	Reg[sweep_code_address].write_Data_trimvalue(sweep_code_high_bit,sweep_code_low_bit,return_code);		
	if(double_address)
		Reg[sweep_code2_address].write_Data_trimvalue(sweep_code2_high_bit,sweep_code2_low_bit,return_code,sweep_code2_shiftbitcount);
	
	return true;
}

bool sweep_code_var_flag(unsigned char sweep_code_max, unsigned int sweep_code_address, unsigned char sweep_code_queue[], 
				unsigned char sweep_code_high_bit,unsigned char sweep_code_low_bit,
				char measure_resource,		char measure_pin,			bool measure_VorI,
				bool measure_GreaterorLess, double measure_flagvalue[SITE_MAX], 
				unsigned char return_code[SITE_MAX], bool return_isfound[SITE_MAX], int delay_us, 
				bool double_address,	unsigned int sweep_code2_address,	
				unsigned char sweep_code2_high_bit, unsigned char sweep_code2_low_bit, char sweep_code2_shiftbitcount)
{
	double measure_val[SITE_MAX];
	for(unsigned char i=0x00; i<sweep_code_max; i++)
	{
		Reg[sweep_code_address].write_Data_trimvalue(sweep_code_high_bit,sweep_code_low_bit,sweep_code_queue[i]);
		if(double_address)
			Reg[sweep_code2_address].write_Data_trimvalue(sweep_code2_high_bit,sweep_code2_low_bit,sweep_code_queue[i],sweep_code2_shiftbitcount);
		delay(delay_us);
		//--for debug---//
		//Reg[sweep_code_address].read(readback);
		//Reg[sweep_code2_address].read(readback);
		
		
		if		(measure_resource == RESOURCE_FVI16)
			if(measure_VorI == VOLTAGE)	Fvi16.CH[measure_pin]	.mv(measure_val);
			else						Fvi16.CH[measure_pin]	.mi(measure_val);
			else if	(measure_resource == RESOURCE_FVI45)
				if(measure_VorI == VOLTAGE)	Fvi45.CH[measure_pin]	.mv(measure_val);
				else						Fvi45.CH[measure_pin]	.mi(measure_val);
				else if	(measure_resource == RESOURCE_FQVI)
					if(measure_VorI == VOLTAGE)	Fqvi.CH[measure_pin]	.mv(measure_val);
					else						Fqvi.CH[measure_pin]	.mi(measure_val);
					else
					{
						FOR_EACH_SITE(nSiteIndex)
						{
							return_isfound[nSiteIndex] = false;
							return_code[nSiteIndex] = 255;
						}
						return false;
					}
					
					FOR_EACH_SITE_FLAG_NO_SITE(nSiteIndex)
					{
						if(	(measure_GreaterorLess == GREATER_THAN	&& measure_val[nSiteIndex]>measure_flagvalue[nSiteIndex]) ||
							(measure_GreaterorLess == LESS_THAN		&& measure_val[nSiteIndex]<measure_flagvalue[nSiteIndex]))
						{
							return_isfound[nSiteIndex] = true;
							return_code[nSiteIndex] = sweep_code_queue[i];
							Flag[nSiteIndex] = true;
						}
						else if(i == sweep_code_max-1)
						{
							return_isfound[nSiteIndex] = false;
							return_code[nSiteIndex] = sweep_code_queue[i];
							Flag[nSiteIndex] = true;
						}
					}
					
					if(isAllFlagSet(true))
						break;
	}
	Reg[sweep_code_address].write_Data_trimvalue(sweep_code_high_bit,sweep_code_low_bit,return_code);		
	if(double_address)
		Reg[sweep_code2_address].write_Data_trimvalue(sweep_code2_high_bit,sweep_code2_low_bit,return_code,sweep_code2_shiftbitcount);
	
	return true;
}



//------------------------------------------------//
double get_site_max_value(double all_site_value[SITE_MAX])
{
	bool flag = false;
	double return_value;
	FOR_EACH_SITE(nSiteIndex)
	{
		if(!flag)
		{
			return_value = all_site_value[nSiteIndex];
			flag = true;
		}
		else if(all_site_value[nSiteIndex] > return_value)
		{
			return_value = all_site_value[nSiteIndex];
		}
	}
	return return_value;
}

double get_site_min_value(double all_site_value[SITE_MAX])
{
	bool flag = false;
	double return_value;
	FOR_EACH_SITE(nSiteIndex)
	{
		if(!flag)
		{
			return_value = all_site_value[nSiteIndex];
			flag = true;
		}
		else if(all_site_value[nSiteIndex] < return_value)
		{
			return_value = all_site_value[nSiteIndex];
		}
	}
	return return_value;
}

double Min(double a, double b)	{return a<b?a:b;}
double Max(double a, double b)	{return a>b?a:b;}
//-------------------below for read all board status-----------------------------//
bool Record_allstatus()
{
	int i;
	for(i=0;i<MAX_REG_NUM;i++)
		Reg[i].record();
	for(i=0;i<MAX_CBIT_NUM;i++)
		Relay.BIT[i].record();
	for(i=0;i<MAX_FQVI_NUM;i++)
		Fqvi.CH[i].record();
	for(i=0;i<MAX_FVI16_NUM;i++)
		Fvi16.CH[i].record();
	for(i=0;i<MAX_FVI45_NUM;i++)
	{
		Fvi45.CH[i].record();
	}
	return true;
}
bool Compare_CMI(char* comment, int reg_size, unsigned int reg_addr[])
{
	int i;
	if(Test_debug)
	{
		if(comment!="")
			fprintf(Test_debug_file,comment);
		FOR_EACH_SITE(nSiteIndex)
		{
			fprintf(Test_debug_file,"\nCompare for SITE%d\n",nSiteIndex);
			fprintf(Test_debug_file,"Register compare\nReg_addr,CMI,Flashed,compare_result\n");
			
			for(i=0;i<reg_size;i++)
			{
				fprintf(Test_debug_file,"Reg0x%02X,0x%02X,0x%02X,%s\n", reg_addr[i], 
					Reg[reg_addr[i]].CMI, 
					Reg[reg_addr[i]].Data[nSiteIndex],
					Reg[reg_addr[i]].CMI==Reg[reg_addr[i]].Data[nSiteIndex]?"SAME":"DIFF");
			}
		}
	}
	return true;

}
bool Compare_allstatus(char* comment, int reg_size, unsigned int reg_addr[])
{
	int i;
	if(Test_debug)
	{
		if(comment!="")
			fprintf(Test_debug_file,comment);
		FOR_EACH_SITE(nSiteIndex)
		{
			fprintf(Test_debug_file,"\nCompare for SITE%d\n",nSiteIndex);
			fprintf(Test_debug_file,"Register compare\nReg_addr,Record,Current,compare_result\n");
			
			for(i=0;i<reg_size;i++)
			{
				fprintf(Test_debug_file,"Reg0x%02X,0x%02X,0x%02X,%s\n", reg_addr[i], 
					Reg[reg_addr[i]].Data_Record[nSiteIndex], 
					Reg[reg_addr[i]].Data[nSiteIndex],
					Reg[reg_addr[i]].Data_Record[nSiteIndex]==Reg[reg_addr[i]].Data[nSiteIndex]?"SAME":"DIFF");
			}
			fprintf(Test_debug_file,"\nRelay Status Compare\nCBIT_NO,Record,Current,compare_result\n");
			for(i=0;i<MAX_CBIT_NUM;i++)
			{
				fprintf(Test_debug_file,"CBIT%d,%s,%s,%s\n", i+1, 
					Relay.BIT[i].status_Record[nSiteIndex]?"SET":"CLR", 
					Relay.BIT[i].status[nSiteIndex]?"SET":"CLR", 
					Relay.BIT[i].status_Record[nSiteIndex]==Relay.BIT[i].status[nSiteIndex]?"SAME":"DIFF");
			}
			fprintf(Test_debug_file,"\nFloating FQVI Status Compare\nChannel,Parameter,Record,Current,compare_result\n");
			for(i=0;i<MAX_FQVI_NUM;i++)
			{
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"Connection",
					Fqvi.CH[i].status_Record.isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fqvi.CH[i].isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fqvi.CH[i].status_Record.isClosed[nSiteIndex]==Fqvi.CH[i].isClosed[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"ForceVorI",
					Fqvi.CH[i].status_Record.isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fqvi.CH[i].isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fqvi.CH[i].status_Record.isForceV[nSiteIndex]==Fqvi.CH[i].isForceV[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"SetValue",
					Fqvi.CH[i].status_Record.lastSettingValue[nSiteIndex], 
					Fqvi.CH[i].lastSettingValue[nSiteIndex], 
					Fqvi.CH[i].status_Record.lastSettingValue[nSiteIndex]==Fqvi.CH[i].lastSettingValue[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampV_Max",
					Fqvi.CH[i].status_Record.lastV_Max[nSiteIndex], 
					Fqvi.CH[i].lastV_Max[nSiteIndex], 
					Fqvi.CH[i].status_Record.lastV_Max[nSiteIndex]==Fqvi.CH[i].lastV_Max[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampV_Min",
					Fqvi.CH[i].status_Record.lastV_Min[nSiteIndex], 
					Fqvi.CH[i].lastV_Min[nSiteIndex], 
					Fqvi.CH[i].status_Record.lastV_Min[nSiteIndex]==Fqvi.CH[i].lastV_Min[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampI_Max",
					Fqvi.CH[i].status_Record.lastI_Max[nSiteIndex], 
					Fqvi.CH[i].lastI_Max[nSiteIndex], 
					Fqvi.CH[i].status_Record.lastI_Max[nSiteIndex]==Fqvi.CH[i].lastI_Max[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampI_Min",
					Fqvi.CH[i].status_Record.lastI_Min[nSiteIndex], 
					Fqvi.CH[i].lastI_Min[nSiteIndex], 
					Fqvi.CH[i].status_Record.lastI_Min[nSiteIndex]==Fqvi.CH[i].lastI_Min[nSiteIndex]?"SAME":"DIFF");
			}
			fprintf(Test_debug_file,"\nFVI45 to GND Status Compare\nChannel,Parameter,Record,Current,compare_result\n");
			for(i=0;i<MAX_FVI45_NUM;i++)
			{
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"Connection",
					Fvi45.CH[i].status_Record.isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fvi45.CH[i].isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fvi45.CH[i].status_Record.isClosed[nSiteIndex]==Fvi45.CH[i].isClosed[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"ForceVorI",
					Fvi45.CH[i].status_Record.isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fvi45.CH[i].isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fvi45.CH[i].status_Record.isForceV[nSiteIndex]==Fvi45.CH[i].isForceV[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"SetValue",
					Fvi45.CH[i].status_Record.lastSettingValue[nSiteIndex], 
					Fvi45.CH[i].lastSettingValue[nSiteIndex], 
					Fvi45.CH[i].status_Record.lastSettingValue[nSiteIndex]==Fvi45.CH[i].lastSettingValue[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampV_Max",
					Fvi45.CH[i].status_Record.lastV_Max[nSiteIndex], 
					Fvi45.CH[i].lastV_Max[nSiteIndex], 
					Fvi45.CH[i].status_Record.lastV_Max[nSiteIndex]==Fvi45.CH[i].lastV_Max[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampV_Min",
					Fvi45.CH[i].status_Record.lastV_Min[nSiteIndex], 
					Fvi45.CH[i].lastV_Min[nSiteIndex], 
					Fvi45.CH[i].status_Record.lastV_Min[nSiteIndex]==Fvi45.CH[i].lastV_Min[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampI_Max",
					Fvi45.CH[i].status_Record.lastI_Max[nSiteIndex], 
					Fvi45.CH[i].lastI_Max[nSiteIndex], 
					Fvi45.CH[i].status_Record.lastI_Max[nSiteIndex]==Fvi45.CH[i].lastI_Max[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"ClampI_Min",
					Fvi45.CH[i].status_Record.lastI_Min[nSiteIndex], 
					Fvi45.CH[i].lastI_Min[nSiteIndex], 
					Fvi45.CH[i].status_Record.lastI_Min[nSiteIndex]==Fvi45.CH[i].lastI_Min[nSiteIndex]?"SAME":"DIFF");
			}
			fprintf(Test_debug_file,"\nFVI16 to GND Status Compare\nChannel,Parameter,Record,Current,compare_result\n");
			for(i=0;i<MAX_FVI16_NUM;i++)
			{
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"Connection",
					Fvi16.CH[i].status_Record.isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fvi16.CH[i].isClosed[nSiteIndex]?"CLOSE":"OPEN", 
					Fvi16.CH[i].status_Record.isClosed[nSiteIndex]==Fvi16.CH[i].isClosed[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%s,%s,%s\n", i+1,"ForceVorI",
					Fvi16.CH[i].status_Record.isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fvi16.CH[i].isForceV[nSiteIndex]?"VOLTAGE":"CURRENT", 
					Fvi16.CH[i].status_Record.isForceV[nSiteIndex]==Fvi16.CH[i].isForceV[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%f,%f,%s\n", i+1,"SetValue",
					Fvi16.CH[i].status_Record.lastSettingValue[nSiteIndex], 
					Fvi16.CH[i].lastSettingValue[nSiteIndex], 
					Fvi16.CH[i].status_Record.lastSettingValue[nSiteIndex]==Fvi16.CH[i].lastSettingValue[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%d,%d,%s\n", i+1,"Vrange",
					Fvi16.CH[i].status_Record.lastVrange[nSiteIndex], 
					Fvi16.CH[i].lastVrange[nSiteIndex], 
					Fvi16.CH[i].status_Record.lastVrange[nSiteIndex]==Fvi16.CH[i].lastVrange[nSiteIndex]?"SAME":"DIFF");
				fprintf(Test_debug_file,"CH%d,%s,%d,%d,%s\n", i+1,"Irange",
					Fvi16.CH[i].status_Record.lastIrange[nSiteIndex], 
					Fvi16.CH[i].lastIrange[nSiteIndex], 
					Fvi16.CH[i].status_Record.lastIrange[nSiteIndex]==Fvi16.CH[i].lastIrange[nSiteIndex]?"SAME":"DIFF");
			}
		}
	}
	return true;

}

//---------------------------below for trim function-------------------------------//
short Update_Trimmed_Byte(unsigned char reg_byte, unsigned char trimvalue, char HiBITcount, char LoBITcount, char ShiftBITcount)
{
	unsigned char temp = 0x00;
	unsigned char update_reg_byte;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;
	
	update_reg_byte= (reg_byte & (~temp)) | (((trimvalue>>ShiftBITcount)<<LoBITcount)&temp);
	
	return update_reg_byte;
}

//*********************************************NEW trim and re-trim functions**********************************************************************//

unsigned char get_trim_code_LookupTable(double target,double pre_value, double lsb, int table_size, unsigned char code_queue[], int ratio_queue[],
								unsigned char bithigh, unsigned char bitlow, unsigned int trim_adr ,bool RatioOrDiff)
{
	//--------explain----//
	//--------1. if table is linear, please set true LSB value-----------//
	//--------2. if table is Non-linear, please set LSB=1, and ratio_queue[] table is true range table--------//
	int i=0;
	double 	variation;
	
	if(RatioOrDiff == DIFF)	
		variation=target-pre_value; 
	else	
		variation=100*(target-pre_value)/pre_value; 
	
	//---------initial-----//
	for(i=0;i<Max_trimtable_size;i++)
	{
		global_trim_table_mintomax[i].code=0;	
		global_trim_table_mintomax[i].range=0;	
		trim_table_value[i]=0;	
	}
	for(i=0;i<table_size;i++)// trim tabel range from min to max Nina 20200605
	{
		global_trim_table_mintomax[i].code=code_queue[i];	
		global_trim_table_mintomax[i].range=lsb*ratio_queue[i];	
		trim_table_value[i]=lsb*ratio_queue[i];	
	}
	
	//==========get trim code ==========//
	int table_queue=0;
	unsigned char trim_code=global_trim_table_mintomax[0].code;
	double delta[Max_trimtable_size],min_delta;
	
	for(i=0;i<table_size;i++)
	{
		delta[i]=fabs(global_trim_table_mintomax[i].range-variation);
	}
	min_delta=delta[0];
	for(i=0;i<table_size;i++)
	{		
		if(delta[i]<min_delta)		
		{
			min_delta=delta[i];
			trim_code=global_trim_table_mintomax[i].code;
			table_queue=i;		
		}
	}	
	
	//----------write trim bit------------------//
	Reg[trim_adr].write_Data_trimvalue_singlesite(bithigh,bitlow,trim_code);
	
    return (trim_code); 
}
bool shift_one_trim_code(double target, double post_value, int *table_queue, double lsb, int table_size, unsigned char code_queue[],
						 unsigned char *return_trim_code, int *return_retrim_count, bool *return_retrim_trend, bool *return_flag,
						 unsigned char bithigh, unsigned char bitlow, unsigned int trim_adr,
						 bool RatioOrDiff, double percent,double Lolimit, double Hilimit)
{
	double 	variation;
	unsigned char find_trim_code=0;
	bool flag=false;
	bool trend=*return_retrim_trend;
	int trim_count=*return_retrim_count;
	int new_table_queue=0;

	if (global_retrim_loop==0)// for sweep code to find table queue
	{
		trim_count=0;	
		for(i=0;i<table_size;i++) // get the table queue 
		{
			if (code_queue[i]==*return_trim_code)
			{
				*table_queue=i;
					break;
			}	
		}

		double int_target=target;
		if(Lolimit!=-999999)
			 int_target=Lolimit;
		if(Hilimit!=999999)
 			int_target=Hilimit;

		if ((post_value-int_target)>0)
			trend=TRIM_DOWN;
		else
			trend=TRIM_UP;

		*return_retrim_trend=trend;
	}
	
	if(RatioOrDiff == DIFF)	
		variation=fabs((post_value-target)/lsb); 
	else
		variation=fabs(((post_value-target)/target)/lsb); 
	
	if((Lolimit==-999999)&&(Hilimit==999999))		
	{
		if (variation<=percent)
			flag=false;
		else
			flag=true;	
	}
	else
	{
		if ((variation<=percent)&&(post_value>=Lolimit)&&(post_value<=Hilimit))		
			flag=false;
		else
			flag=true;	
	}
	if(flag==true)
	{
		flag=false;

		for(i=0;i<table_size;i++) // get the real table queue 
		{
			if (code_queue[i]==*return_trim_code)
			{
				new_table_queue=i;
				break;
			}	
		}
	
		if (trend==TRIM_DOWN)// post > target
		{	 
			if ((new_table_queue>0)&&(((post_value-target)>0)||(post_value<=Lolimit)||(post_value>=Hilimit))) // shift down trim code
			{
				find_trim_code=code_queue[*table_queue-1-global_retrim_loop];
				trim_count=global_retrim_loop+1;
				flag=true;
			}
		}
		if (trend==TRIM_UP)
		{
			if ((new_table_queue<(table_size-1))&&(((post_value-target)<0)||(post_value<=Lolimit)||(post_value>=Hilimit)))// shift down trim code
			{
				find_trim_code=code_queue[*table_queue+1+global_retrim_loop];
				trim_count=global_retrim_loop+1;
				flag=true;
			}
		}
		
		

		if (flag==1)
		{
			*return_trim_code=find_trim_code;
			//----------write trim bit------------------//
			Reg[trim_adr].write_Data_trimvalue_singlesite(bithigh,bitlow,find_trim_code);
		}	
		
	}
	
	*return_retrim_count=trim_count;
	*return_flag=flag;
	return (flag); 
}
bool search_Best_trim_code(double trim_target, int trim_count, double *return_post_value,  unsigned char *retrun_trim_code, 
						   unsigned char bithigh, unsigned char bitlow, unsigned int trim_adr,
						   double Lolimit, double Hilimit  )
{
	bool flag=false;
	int position=0; // return the first post value if no correct value
	int i;
	int length=trim_count+1;
	double *delta=new double[length];
	double min_delta;
    
	min_delta=9e10;
	if (trim_count>=1)
	{
		flag=true;
				
		for(i=0;i<length;i++)
		{
			if((store_Post_value_array[i]>=Lolimit)&&(store_Post_value_array[i]<=Hilimit))
			{
				delta[i]=fabs(store_Post_value_array[i]-trim_target);

				if(delta[i]<=min_delta)	
				{
					min_delta=delta[i];
					position=i;
				}	
			}
		}
		
		*return_post_value=store_Post_value_array[position];
		*retrun_trim_code=store_Global_trimcode_array[position];
		//----------write trim bit------------------//
		Reg[trim_adr].write_Data_trimvalue_singlesite(bithigh,bitlow,store_Global_trimcode_array[position]);
	}
	
	return flag;
}

bool table_code_from_max_to_min(bool flag, int table_size, unsigned char code_queue[], unsigned char return_code_queue_new[])
{
	if (flag==true)// invert the code_queue value to code_queue_new
	{
		for(int i=0;i<table_size;i++)
		{
			return_code_queue_new[i]=code_queue[table_size-1-i];
		}
	}	
	else//copy the code_queue value to code_queue_new
	{
		for(int i=0;i<table_size;i++)
		{
			return_code_queue_new[i]=code_queue[i];
		}
	}
	return true;
}



//***********************************************************REG_Define ***********************************************************************//
unsigned char DEVICE_ADDR;
unsigned char write_buffer[8]={0};
unsigned char read_buffer[8]={0};
long ret = 0;

bool myReg::init_data(int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			LastWrite[nSiteIndex]	= 0xFF;
			LastRead[nSiteIndex]	= 0xFF;
			Data[nSiteIndex]		= CMI;
			Trimmed[nSiteIndex]	= CMI;
			Flashed[nSiteIndex]	= CMI;
			Locked[nSiteIndex]	= CMI;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		LastWrite[SiteNo]	= 0xFF;
		LastRead[SiteNo]	= 0xFF;
		Data[SiteNo]		= CMI;
		Trimmed[SiteNo]		= CMI;
		Flashed[SiteNo]		= CMI;
		Locked[SiteNo]		= CMI;
	}
	else
		return false;
	return true;
}

bool myReg::read_device(unsigned char readback[SITE_MAX],unsigned char Device_address, unsigned char address, int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			read_buffer[0]=0;
			ret = VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_address, address, read_buffer, 1);
			LastRead[nSiteIndex] = read_buffer[0];
			readback[nSiteIndex] = LastRead[nSiteIndex];
			Data[nSiteIndex]	 = LastRead[nSiteIndex];
			read_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		read_buffer[0]=0;
		ret =VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*SiteNo, Device_address, address, read_buffer, 1);
		LastRead[SiteNo] = read_buffer[0];
		readback[SiteNo] = LastRead[SiteNo];
		Data[SiteNo]	 = LastRead[SiteNo];
		read_buffer[0]=0;
	}
	else 
		return false;
	return true;
}

bool myReg::read(unsigned char readback[SITE_MAX],int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			read_buffer[0]=0;
			ret = VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_addr, address, read_buffer, 1);
			LastRead[nSiteIndex] = read_buffer[0];
			readback[nSiteIndex] = LastRead[nSiteIndex];
			Data[nSiteIndex]	 = LastRead[nSiteIndex];
			read_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		read_buffer[0]=0;
		ret =VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*SiteNo, Device_addr, address, read_buffer, 1);
		LastRead[SiteNo] = read_buffer[0];
		readback[SiteNo] = LastRead[SiteNo];
		Data[SiteNo]	 = LastRead[SiteNo];
		read_buffer[0]=0;
	}
	else 
		return false;
	return true;
}

bool myReg::read(int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			read_buffer[0]=0;
			ret = VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_addr, address, read_buffer, 1);
			LastRead[nSiteIndex] = read_buffer[0];
			Data[nSiteIndex]	 = LastRead[nSiteIndex];
			read_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		read_buffer[0]=0;
		ret =VII_ReadBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*SiteNo, Device_addr, address, read_buffer, 1);
		LastRead[SiteNo] = read_buffer[0];
		Data[SiteNo]	 = LastRead[SiteNo];
		read_buffer[0]=0;
	}
	else 
		return false;
	return true;
}

bool myReg::write_device(unsigned char data, unsigned char Device_address, unsigned char address, int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			write_buffer[0]=data;
			ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_address, address, write_buffer, 1);
			write_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		write_buffer[0]=data;
		ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*SiteNo, Device_address, address, write_buffer, 1);
	    write_buffer[0]=0;
	}
	else 
		return false;
	return true;
}
bool myReg::write_device(unsigned char data[SITE_MAX], unsigned char Device_address, unsigned char address )
{
	FOR_EACH_SITE(nSiteIndex)
	{
		write_buffer[0]=data[nSiteIndex];
		ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_address, address, write_buffer, 1);
		write_buffer[0]=0;
	}
	return true;
}


bool myReg::write(unsigned char data, int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			write_buffer[0]=data;
			ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_addr, address, write_buffer, 1);
			LastWrite[nSiteIndex] = write_buffer[0];
			Data[nSiteIndex]	  = LastWrite[nSiteIndex];
			write_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		write_buffer[0]=data;
		ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*SiteNo, Device_addr, address, write_buffer, 1);
		LastWrite[SiteNo] = write_buffer[0];
		Data[SiteNo]	  = LastWrite[SiteNo];
	    write_buffer[0]=0;
	}
	else 
		return false;
	return true;
}

bool myReg::write(unsigned char data[SITE_MAX])
{
	FOR_EACH_SITE(nSiteIndex)
	{
		write_buffer[0]=data[nSiteIndex];
		ret = VII_WriteBytes(VII_USBI2C, DEVICE_1, I2C_BOX_CH_SEL*nSiteIndex, Device_addr, address, write_buffer, 1);//site1 ch0 site2 ch2
		LastWrite[nSiteIndex] = write_buffer[0];
		Data[nSiteIndex]	  = LastWrite[nSiteIndex];
		write_buffer[0]=0;
	}
	return true;
}

bool myReg::set_clr_inv(unsigned char set, unsigned char clear,unsigned char inverse,int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
			write(((myReg::Trimmed[nSiteIndex]|set)&(~clear))^inverse,nSiteIndex);
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
		write(((myReg::Trimmed[SiteNo]|set)&(~clear))^inverse,SiteNo);
	else 
		return false;
	return true;
}

bool myReg::set_clr_inv_Looked(unsigned char set, unsigned char clear,unsigned char inverse,int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
			write(((myReg::Locked[nSiteIndex]|set)&(~clear))^inverse,nSiteIndex);
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
		write(((myReg::Locked[nSiteIndex]|set)&(~clear))^inverse,nSiteIndex);
	else 
		return false;
	return true;
}


bool myReg::get_Data(char HiBITcount, char LoBITcount, unsigned char returndata[SITE_MAX])
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	FOR_EACH_SITE(nSiteIndex)
		returndata[nSiteIndex] = (Data[nSiteIndex]>>LoBITcount)&temp;
	return true;
}

bool myReg::write_Data_trimvalue(char HiBITcount, char LoBITcount, unsigned char trimvalue[SITE_MAX], char ShiftBITcount)
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;
	FOR_EACH_SITE(nSiteIndex)
		Trimmed[nSiteIndex] = (Trimmed[nSiteIndex] & (~temp)) | (((trimvalue[nSiteIndex]>>ShiftBITcount)<<LoBITcount)&temp);

	FOR_EACH_SITE(nSiteIndex)
		Data[nSiteIndex] = Trimmed[nSiteIndex];
	write(Data);
	return true;
}

bool myReg::write_Data_trimvalue(char HiBITcount, char LoBITcount, unsigned char trimvalue, char ShiftBITcount)
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;
	FOR_EACH_SITE(nSiteIndex)
		Trimmed[nSiteIndex] = (Trimmed[nSiteIndex] & (~temp)) | (((trimvalue>>ShiftBITcount)<<LoBITcount)&temp);

	FOR_EACH_SITE(nSiteIndex)
		Data[nSiteIndex] = Trimmed[nSiteIndex];
	write(Data);
	return true;
}
bool myReg::write_Data_trimvalue_byBeforeSet(char HiBITcount, char LoBITcount, unsigned char trimvalue[SITE_MAX], char ShiftBITcount)
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;
	FOR_EACH_SITE(nSiteIndex)
		Trimmed[nSiteIndex] = (Trimmed[nSiteIndex] & (~temp)) | (((trimvalue[nSiteIndex]>>ShiftBITcount)<<LoBITcount)&temp);

	FOR_EACH_SITE(nSiteIndex)
		Data[nSiteIndex] = (Data[nSiteIndex] & (~temp)) | (((trimvalue[nSiteIndex]>>ShiftBITcount)<<LoBITcount)&temp);
	write(Data);
	return true;
}

bool myReg::write_Data_trimvalue_byBeforeSet(char HiBITcount, char LoBITcount, unsigned char trimvalue, char ShiftBITcount)
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;

	FOR_EACH_SITE(nSiteIndex)
		Trimmed[nSiteIndex] = (Trimmed[nSiteIndex] & (~temp)) | (((trimvalue>>ShiftBITcount)<<LoBITcount)&temp);

	FOR_EACH_SITE(nSiteIndex)
		Data[nSiteIndex] = (Data[nSiteIndex] & (~temp)) | (((trimvalue>>ShiftBITcount)<<LoBITcount)&temp);
	write(Data);
	return true;
}
bool myReg::write_Data_trimvalue_singlesite(char HiBITcount, char LoBITcount, unsigned char trimvalue, char ShiftBITcount)
{
	unsigned char temp = 0x00;
	for(char i=0; i<=HiBITcount-LoBITcount; i++)
		temp |= (1<<i);
	temp = temp<<LoBITcount;
	Trimmed[nSiteIndex] = (Trimmed[nSiteIndex] & (~temp)) | (((trimvalue>>ShiftBITcount)<<LoBITcount)&temp);
	Data[nSiteIndex] = Trimmed[nSiteIndex];
	write(Data[nSiteIndex],nSiteIndex);
	return true;
}


bool myReg::init_Flashed_Locked()
{
	FOR_EACH_SITE(nSiteIndex)	Trimmed[nSiteIndex]  = Data[nSiteIndex];// add by Nina
	return true;
}

bool myReg::set_Locked()
{
	FOR_EACH_SITE(nSiteIndex)	Locked[nSiteIndex]  = Data[nSiteIndex];
	return true;
}

bool myReg::record()
{
	FOR_EACH_SITE(nSiteIndex)	Data_Record[nSiteIndex]  = Data[nSiteIndex];
	return true;
}

bool myReg::get_Trimmed(unsigned char returndata[SITE_MAX])
{
	FOR_EACH_SITE(nSiteIndex)	returndata[nSiteIndex]  = Trimmed[nSiteIndex];
	return true;
}

bool myReg::write_CMI(int SiteNo)
{
	return myReg::write(myReg::CMI,SiteNo);
}

bool myReg::write_Trimmed()
{
	return myReg::write(myReg::Trimmed);
}


bool myReg::write_Locked()
{
	return myReg::write(myReg::Locked);
}

//---------------------------------I2C-bus advanced I/O port control define-------------------------------------------//
bool PCA_Control::set(unsigned char address,unsigned char data,int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			write_buffer[0]=data;
			ret = VII_WriteBytes(VII_USBI2C, 0, I2C_BOX_CH_SEL*nSiteIndex+4, Device_addr, address, write_buffer, 1);//site1 ch4 site2 ch6
			LastWrite[nSiteIndex][address] = write_buffer[0];
			Data[nSiteIndex][address] = write_buffer[0];
			write_buffer[0]=0;
		}
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
	{
		write_buffer[0]=data;
		ret = VII_WriteBytes(VII_USBI2C, 0, I2C_BOX_CH_SEL*SiteNo+4, Device_addr, address, write_buffer, 1);//site1 ch4 site2 ch6
		LastWrite[nSiteIndex][address] = write_buffer[0];
		Data[nSiteIndex][address] = write_buffer[0];
		write_buffer[0]=0;
	}
	else 
		return false;
	return true;
}
bool PCA_Control::close(unsigned char address, unsigned char set,int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
			PCA_Control::set(address,(PCA_Control::LastWrite[nSiteIndex][address]|set),nSiteIndex);
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
		PCA_Control::set(address,(PCA_Control::LastWrite[SiteNo][address]|set),SiteNo);
	else 
		return false;
	return true;
}
bool PCA_Control::open(unsigned char address, unsigned char clear,int SiteNo)
{
	if(SiteNo==SITE_ALL)
	{
		FOR_EACH_SITE(nSiteIndex)
			PCA_Control::set(address,(PCA_Control::LastWrite[nSiteIndex][address]&(~clear)),nSiteIndex);
	}
	else if(SiteNo < SITE_MAX && IsSiteActive(SiteNo))
		PCA_Control::set(address,(PCA_Control::LastWrite[SiteNo][address]&(~clear)),SiteNo);
	else 
		return false;
	return true;
}
void PCA_Control::init(unsigned char pca_device_addr,unsigned char IO_address,unsigned char IOC_address, unsigned char banks_Num)
{		
	FOR_EACH_SITE(nSiteIndex)
	{	
		Device_addr=pca_device_addr;
		for(int i=0;i<255;i++)// init OP bank and IOC bank
		{		
			Data_Record[nSiteIndex][i]	= 0x00;
			LastWrite[nSiteIndex][i]	= 0x00;
			Data[nSiteIndex][i]		= 0x00;
		}
	}
	for(i=0;i<banks_Num;i++)
	{
		PCA_Control::set(i+IOC_address,0x00); //IO config to Output mode 0=output 1=input
		delayms(2);
		PCA_Control::set(i+IO_address,0x00);  // set all banks output to 0
		delayms(1);
	}

}
void PCA_Control::reset(unsigned char IO_address,unsigned char banks_Num,int SiteNo )
{
	for(int i=0;i<banks_Num;i++)
	{		
		PCA_Control::set(IO_address+i,0x00,SiteNo);  // set all banks output to 0	
	}
}


