#include "User.h"


//============== Glabe variable definition========================================//
//--globle definition------//
unsigned char DEVICE_ADDR_Reg1;
unsigned char DEVICE_ADDR_Reg2;
int  CMI_RegSize=270;

bool print_Flag=false;
bool QC_Flag=false, GNG_Flag=false;
unsigned char CMI_Rev_No=0;
int Testboard_V=0;
long int Program_update_data=0;
bool HTOL_CMI=false;
char* part_number="";
int nTILE;
int Finish_reading_TrimByte=0;
bool I2C_Check_Flag=false;//0: no test;1: test
int Eable_reflash=0;
int Fail_bin_No[SITE_MAX]={0};
bool	Site_check_flag[SITE_MAX]={false,false};
bool Wafer_ID_Flash=0;
int flash_fail_count[SITE_MAX]={0};
int i=0,j=0;
bool Finish_reading_CMI_file=false;
double i_meas[SITE_MAX],v_meas[SITE_MAX];
bool FindToggle[SITE_MAX];
unsigned char readback[SITE_MAX];
unsigned char readbackFD[SITE_MAX];
unsigned char readbackF5[SITE_MAX];
double VAOQ_global[SITE_MAX]={0};

unsigned int double_unit_reg[2][SITE_MAX][300]={0};
double HighLevel=3.3;
double StartPoint,StopPoint,Sweeppoint,Step;
double StartPoint_MultiSite[SITE_MAX],StopPoint_MultiSite[SITE_MAX],Step_MultiSite[SITE_MAX],Toggle_MultiSite[SITE_MAX];
double Istart_up,Istop_up,Istart_down,Istop_down;
double Vstart_up,Vstop_up,Vstart_down,Vstop_down;
double Vtrig[SITE_MAX]={0},Itrig[SITE_MAX]={0};
double I1[SITE_MAX]={0},I2[SITE_MAX]={0},V1[SITE_MAX]={0},V2[SITE_MAX]={0},V3[SITE_MAX]={0};
double trig_L[SITE_MAX]={0},trig_H[SITE_MAX]={0},trig_L1[SITE_MAX]={0},trig_H1[SITE_MAX]={0};

double LSB_L,LSB_H;
double HiLimit,LowLimit,HiLimit1,LowLimit1,HiLimit_hys, LowLimit_hys; 

//double Buck_IOS[PMU_MAX][SITE_MAX]={0}, Buck_IOS_temp[PMU_MAX][SITE_MAX]={0};
//double Buck_HSILIM_normal[PMU_MAX][SITE_MAX]={0}, Buck_HSILIM_normal_temp[PMU_MAX][SITE_MAX]={0};

//--------definition for CMI setting------------------//
//+++++  for master define  ++++++//
double VCC_SET, ITHR1, ITHR2, ITHR3, HMON_TSET, V_Isense12, V_Isense5;
double VIN12_UV_SUP, VIN12_UV_MON, VIN12_OV_SUP, START_DLY, eF5_OC, eF12_OC;
double HCHK_ISNK, VIN5_UV_SUP, VIN5_UV_MON, VIN5_OV_SUP, eF5_ILIM, eF5_SS;
double BFET_ISS, OV12_SUP_DGL, UV12_SUP_DGL, OV5_SUP_DGL, UV5_SUP_DGL, UV12_MON_DGL, UV5_MON_DGL;
double eF12_PG_DL, eF5_PG_DL, HSUVLO_BK, HSUVLO_BST;
double LS_GM_ITH3, LS_GM_ITH1, LS_RCOMP_ITH3, LS_RCOMP_ITH1, LS_LSON_DL, LS_HSOFF_DL, LSDT_L2H, LS_ADJ_ISTH;
double BK_ADJ_ISTH, VSTR, VBUS_REG, PLP_OK, HSILIM, STR_PG, VSTR_DST,LSILIM_PLP;
double BK_VOUV_SET, LSILIM, BK_FREQ, BK_LSOFF_DL, BK_HSON_DL, BK_TRDT_H2L, BST_MAXON_ADJ;
double IOD1_DLF, IOD1_DLR, IOD2_DLF, IOD2_DLR, IOD3_DLF, IOD3_DLR, IOD4_DLF, IOD4_DLR;
double IOD5_DLF, IOD5_DLR, IOD6_DLF, IOD6_DLR, IOD7_DLF, IOD7_DLR, IOD8_DLF, IOD8_DLR;
double VBUS_MON_R, VBUS_MON_F, TRST_DLY, I2C_ADDR, VIO, PWREN_IO;
double ONDLY_L1, OFFDLY_L1, ONDLY_L2, OFFDLY_L2;
double ILIM_L1, ILIM_L2, SST_L1, SST_L2, L1_VSET, L2_VSET;
double VAOQ[SITE_MAX]={0}, VINTQ[SITE_MAX]={0}, VBG_VREG, VIN_SET, VIN5_SET;
double VN_1[SITE_MAX]={0}, VN_2[SITE_MAX]={0}, VP_1[SITE_MAX]={0}, VP_2[SITE_MAX]={0}, VN[SITE_MAX]={0}, VP[SITE_MAX]={0}, VPTAT[SITE_MAX]={0};
double VREF_0P6[SITE_MAX];
double IOS_eFUSE12[SITE_MAX];
double Vref_eFUSE12[SITE_MAX];
double IOS_eFUSE5[SITE_MAX];
double Vref_eFUSE5[SITE_MAX];
double MSTR_CLK[SITE_MAX];
double VAOD[SITE_MAX];
double INSK_POST[SITE_MAX];
double PLPBUCK_ILIM_OS[SITE_MAX], PLPBUCK_VREFILIM[SITE_MAX], PLPBUCK_K_TARGET[SITE_MAX];
double PLPBUCK_ILIM_OS_PRE[SITE_MAX],PLPBUCK_ILIM_OS_POST[SITE_MAX], PLPBUCK_ILIM_PRE[SITE_MAX], PLPBUCK_ILIM_POST[SITE_MAX], PLPBUCK_ILIM[SITE_MAX];
double Efuse12_ILIM_meas[SITE_MAX]={0}, Efuse5_ILIM_meas[SITE_MAX]={0};
double AVINMON_SET=0;
double ILIMSAS[SITE_MAX]={0}, IRQ_ISET_R[SITE_MAX]={0}, IRQ_ISET_F[SITE_MAX]={0};
double LSB_ILIM[SITE_MAX]={0}, LSB_Freq[SITE_MAX]={0};
double GMx2[SITE_MAX]={0}, Vrefss[SITE_MAX]={0};
double VSWpre[SITE_MAX]={0}, VSW600[SITE_MAX]={0}, VSW1200[SITE_MAX]={0}, VSW1600[SITE_MAX]={0}, VSWpre2[SITE_MAX]={0}, VSW700[SITE_MAX]={0}, VSWpost[SITE_MAX]={0};
double VFB_EA[SITE_MAX]={0};
double VIO_Test[SITE_MAX]={0};
double LDOVout[2][SITE_MAX]={0};
double EF12_ISNS_OS[SITE_MAX]={0},EF5_ISNS_OS[SITE_MAX]={0};
double EA1[SITE_MAX]={0},EA3[SITE_MAX]={0};
//double VIN_LDOmode_Set[LDO_MAX]={0};
//double MASTER_VIN_OV=0,MASTER_VIN_UV=0,MASTER_POK_OV=0;
//double VIO_target=0;
//double MASTER_VSYSMON=0;
//double MASTER_VSYSWRN=0;
//double VREF0P5_CL[SITE_MAX]={0};
/*
//+++++  for buck define  ++++++//
double Buck_Vout_VNOM_target[PMU_MAX]={0},Buck_HSLIMIT_target[PMU_MAX]={0},Buck_Islope_target[PMU_MAX][SITE_MAX]={0};
double Buck_VoutLP_target[PMU_MAX]={0};
double Buck_ZC_target[PMU_MAX]={0};
double Buck_IMINPK_target[PMU_MAX]={526,500,472,472,450,450,526}; //Update TM by Alonso -- Added by Pauline 10/22/2020
double Buck_LSLIMIT_target[PMU_MAX]={0};
double Buck_HSLIMIT_F[PMU_MAX][SITE_MAX]={0};
double Buck_Frequency_target[PMU_MAX]={0};
double Buck_Idp_set[PMU_MAX]={0};
double Buck_SST_set[PMU_MAX]={0};
*/
/*
//+++++  for LDO define  ++++++//
double LDO_Vout_VNOM_target[LDO_MAX]={0};
double LDO_ILIMIT_target[LDO_MAX]={0};
double LDOs_NLSW_ILIM_target;
*/
//+++++  for leakage define  ++++++//
double Leakage_master_GPIO, Leakage_master_GPIO_Abs;
double V_miniBuck;
double VCC_LEAK;
bool ABS_flag;

//--------for Reg and parameter definition-------//
unsigned int CMI_Full_address[271]={0};
bool Factory_Mode_Flag[SITE_MAX]={false};

//double Vout_Bucks_F[PMU_MAX][SITE_MAX]={0};
//double Vout_LDOs_F[LDO_MAX][SITE_MAX]={0};

//-----------digital definition---------------//
char Scan_sPinGrpList[128]      = "SCL,SDA,EN_PIN,MODE_IN,OUT";
char Scan_sChannelMapFile[256]  = "\\Pattern\\Scan_Channel Map.txt";
char Scan_sPinMapFile[256]      = "\\Pattern\\Scan_Pin Map.txt";

char Scan_sPinLevelFile[256]    = "\\Pattern\\Scan_Pin Voltage Levels.txt";
//char Scan_sTimeSetFile[256]     = "C:\\VisualTest\\apps\\ACT85220_E3\\Pattern\\Scan_TimingSet_1M.txt";
//char Scan_sTimeSetFile[256]     = "C:\\VisualTest\\apps\\ACT85220_E3\\Pattern\\Scan_TimingSet_100K.txt";
//char Scan_sTimeSetFile[256]     = "C:\\VisualTest\\apps\\ACT85220_E3\\Pattern\\Scan_TimingSet_10KDebug.txt";
char Scan_sTimeSetFile[256]     = "\\Pattern\\Scan_TimingSet_I2CDebug.txt";


char Scan_sTimeSetList[128]     = "test_cycle,scan_cycle_TM_1,TS_NR,TS_ST,TS_CK_A,TS_SP,TS_CK_B,TS_CK_C,TS_NR_1,TS_ST_1,TS_CK_A_1,TS_SP_1,TS_CK_B_1,TS_CK_C_1";  //must key in all timeset strings here

char sPattern_W55[256]     = "\\Pattern\\IIC_Pattern W e0 55.atp";
char sPattern_WAA[256]     = "\\Pattern\\IIC_Pattern W e0 AA.atp";
char sPattern_W55_1[256]     = "\\Pattern\\IIC_Pattern W e0 55_1.atp";
char sPattern_WAA_1[256]     = "\\Pattern\\IIC_Pattern W e0 AA_1.atp";
char sPattern_W4A[256]     = "\\Pattern\\IIC_Pattern W e0 00.atp";

char Scan_file1[256]     = "\\Pattern\\AA86AZ_SCAN_Pattern1.atp";
char Scan_file2[256]     = "\\Pattern\\AA86AZ_SCAN_Pattern2.atp";
char sCaptureDataFile_pattern1[256] = "\\Pattern\\capture_scan_vector.txt";
char sCaptureDataFile_pattern2[256] = "\\Pattern\\capture_logic_vector.txt";
//-----------digital definition---------------//

//============== Glabe variation definition end ========================================//

//============== Glabe function definition  ========================================//

void Initialise_full_path()//The list of strcpy deponding on projects needs
{
	strcpy(Scan_sChannelMapFile,FullPath(Scan_sChannelMapFile)); //yellow file name depending on project
	strcpy(Scan_sPinMapFile,FullPath(Scan_sPinMapFile));
	strcpy(Scan_sPinLevelFile,FullPath(Scan_sPinLevelFile));
	strcpy(Scan_sTimeSetFile,FullPath(Scan_sTimeSetFile));
	strcpy(sPattern_W55,FullPath(sPattern_W55));
	strcpy(sPattern_W55_1,FullPath(sPattern_W55_1));
	strcpy(sPattern_WAA,FullPath(sPattern_WAA));
	strcpy(sPattern_WAA_1,FullPath(sPattern_WAA_1));
	strcpy(sPattern_W4A,FullPath(sPattern_W4A));
	strcpy(Scan_file1,FullPath(Scan_file1));
	strcpy(Scan_file2,FullPath(Scan_file2));
	strcpy(sCaptureDataFile_pattern1,FullPath(sCaptureDataFile_pattern1));
	strcpy(sCaptureDataFile_pattern2,FullPath(sCaptureDataFile_pattern2));
}
void resource_init(int site)
{

	if(site < SITE_MAX)
	{
		Relay.BIT[K1_VIN_12]				.clr(site);
		Relay.BIT[K2_VIN_5]					.clr(site);
		Relay.BIT[K3_VBUS]					.clr(site);
		Relay.BIT[K4_VCC]					.clr(site);
		Relay.BIT[K5_VSPL]					.clr(site);
		Relay.BIT[K6_VOUT_5]				.clr(site);
		Relay.BIT[K7_SS]					.clr(site);
		Relay.BIT[K8_LDOIN]					.clr(site);
		Relay.BIT[K9_LDO1]					.clr(site);
		Relay.BIT[K10_LDO2]					.clr(site);
		Relay.BIT[K11_SITE_CHECK]			.clr(site);
		Relay.BIT[K12_SCAN_OUT]				.clr(site);
		Relay.BIT[K13_GPIO4_SS]				.clr(site);
		Relay.BIT[K14_GPIO5_COMP]			.clr(site);
		Relay.BIT[K15_SCL_SDA]				.clr(site);
		Relay.BIT[K16_COMP_AMP]				.clr(site);
		Relay.BIT[K17_COMP_RC]				.clr(site);
		Relay.BIT[K18_GPIO8_AMUX]			.clr(site);
		Relay.BIT[K19_GPIO9_AMUX]			.clr(site);
		Relay.BIT[K20_GPIO10_AMUX]			.clr(site);
		Relay.BIT[K21_TMU_CH1A_GPIO14]		.clr(site);
		Relay.BIT[K22_TMU_CH1B_GPIO15]		.clr(site);
		Relay.BIT[K23_TMU_CH3A_GPIO16]		.clr(site);
		Relay.BIT[K24_GPIO14_DMUX]			.clr(site);
		Relay.BIT[K25_GPIO15_DMUX]			.clr(site);
		Relay.BIT[K26_GPIO16_DMUX]			.clr(site);
		Relay.BIT[K27_FQVI_SW_VCC_VBUS]		.clr(site);
		Relay.BIT[K28_FQVI_SW_PLP_STR]		.clr(site);
		Relay.BIT[K29_FQVI_SW_BST_VBUS]		.clr(site);
		Relay.BIT[K30_FQVI_CH5_VBUS]		.clr(site);
		Relay.BIT[K31_FQVI_CH6_VBUS]		.clr(site);
		Relay.BIT[K32_FQVI_LD01_LDOIN]		.clr(site);
		Relay.BIT[K33_FQVI_LDO2_LDOIN]		.clr(site);
		Relay.BIT[K34_STR_CAP]		.clr(site);
		
		Relay.BIT[K41_FQVI_CH1_CH5]			.clr(site);
		Relay.BIT[K42_FQVI_CH2_CH6]			.clr(site);
		Relay.BIT[K43_FQVI_CH3_CH7]			.set(site);   // Set FQVI_7 to LDO1 
		Relay.BIT[K44_FQVI_CH4_CH8]			.set(site);	  // Set FQVI_8 to LDO2 
		Relay.BIT[K45_DDD_SCAN3]			.clr(site);
		Relay.BIT[K46_DDD_SDA_SCL]			.clr(site);		
		Relay.BIT[K47_TMU]					.clr(site);
		Relay.BIT[K48_SDA_SCL_I2C]			.clr(site);
		delayms(1);

		//---discharge caps---//
		Fvi45.CH[VIN_12]		.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VIN_5]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VBUS]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VCC]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VSPL]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VBUS]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[VOUT_5]		.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi45.CH[LDOIN]			.fv(0,site,SET_CLAMP,2,-2,350,-350);
		Fvi16.CH[SS]			.fv(0,site,Stall1V1,Stall50mA);
		Fqvi.CH[FQVI_CH3]		.fv(0,site,SET_CLAMP,2,-2,400,-400);	
		Fqvi.CH[FQVI_CH4]		.fv(0,site,SET_CLAMP,2,-2,400,-400);	
		delayms(10);

		//================SOURCE  set 0 =================//
		Fvi45.CH[VIN_12]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VIN_5]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VBUS]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VCC]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VSPL]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VBUS]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[VOUT_5]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[LDOIN]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi16.CH[SS]			.fv(0,site,Stall1V1,Stall50uA);
		Fqvi.CH[FQVI_CH3]		.fv(0,site,SET_CLAMP,2,-2,4,-4);	
		Fqvi.CH[FQVI_CH4]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		delayms(1);

		Relay.BIT[K43_FQVI_CH3_CH7]			.clr(site);	  // Clr FQVI_7 to LDO1 
		Relay.BIT[K44_FQVI_CH4_CH8]			.clr(site);   // Clr FQVI_8 to LDO2 
		delayms(1);

		Fvi45.CH[VIN_12]		.open(site);
		Fvi45.CH[VIN_5]			.open(site);
		Fvi45.CH[VBUS]			.open(site);
		Fvi45.CH[VCC]			.open(site);
		Fvi45.CH[VSPL]			.open(site);
		Fvi45.CH[VBUS]			.open(site);
		Fvi45.CH[VOUT_5]		.open(site);
		Fvi45.CH[LDOIN]			.open(site);
		Fvi16.CH[SS]			.open(site);
		
		Fvi45.CH[SW_VCC]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[SW_PLP]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[SW_BST]		.fv(0,site,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[STR]			.fv(0,site,SET_CLAMP,2,-2,4,-4);
		
		Fvi45.CH[SW_VCC]		.open(site);
		Fvi45.CH[SW_PLP]		.open(site);
		Fvi45.CH[SW_BST]		.open(site);
		Fvi45.CH[STR]			.open(site);

		//FVI16
		Fvi16.CH[GPIO1]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO2]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO3]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO4]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO5]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO6]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO7]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO8]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO9]		.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO10]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO11]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO12]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO13]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO14]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO15]	.fv(0,site,Stall1V1,Stall50uA);
		Fvi16.CH[GPIO16]	.fv(0,site,Stall1V1,Stall50uA);		

		Fvi16.CH[GPIO1]		.open(site);
		Fvi16.CH[GPIO2]		.open(site);
		Fvi16.CH[GPIO3]		.open(site);
		Fvi16.CH[GPIO4]		.open(site);
		Fvi16.CH[GPIO5]		.open(site);
		Fvi16.CH[GPIO6]		.open(site);
		Fvi16.CH[GPIO7]		.open(site);
		Fvi16.CH[GPIO8]		.open(site);
		Fvi16.CH[GPIO9]		.open(site);
		Fvi16.CH[GPIO10]	.open(site);
		Fvi16.CH[GPIO11]	.open(site);
		Fvi16.CH[GPIO12]	.open(site);
		Fvi16.CH[GPIO13]	.open(site);
		Fvi16.CH[GPIO14]	.open(site);
		Fvi16.CH[GPIO15]	.open(site);
		Fvi16.CH[GPIO16]	.open(site);	
				
		//FQVI
		Fqvi.CH[FQVI_CH1]		.fi(0,site,SET_CLAMP,1,-1,10,-10);	//why fi?
		Fqvi.CH[FQVI_CH2]		.fi(0,site,SET_CLAMP,1,-1,10,-10);	//why fi?
		Fqvi.CH[FQVI_CH3]		.fi(0,site,SET_CLAMP,1,-1,10,-10);	//why fi?
		Fqvi.CH[FQVI_CH4]		.fi(0,site,SET_CLAMP,1,-1,10,-10);	//why fi?

		Fqvi.CH[FQVI_CH1]		.open(site);
		Fqvi.CH[FQVI_CH2]		.open(site);
		Fqvi.CH[FQVI_CH3]		.open(site);
		Fqvi.CH[FQVI_CH4]		.open(site);

//		Relay.BIT[K47_SDA_SCL_AB]			.clr(site); // set at the end to aviod AVIN power on 
	}

	if(site == SITE_ALL)
	{
		TMU_CH1A.switchset(0);	
		TMU_CH1A.arm(1);
		TMU_CH1B.switchset(0);	
		TMU_CH1B.arm(1);
		TMU_CH3A.switchset(0);	
		TMU_CH3A.arm(1);
	}
	else if(site<SITE_MAX) 
	{
		TMU_CH1A.switchset(0,site+1);	
		TMU_CH1A.arm(1,site+1);	
		TMU_CH1B.switchset(0,site+1);	
		TMU_CH1B.arm(1,site+1);	
		TMU_CH3A.switchset(0,site+1);		
		TMU_CH3A.arm(1,site+1);
	}
		
}

//----------------------------------//
bool Check_Factory_Mode()
{
	bool flag = false;
	
	Reg[0x2002].read();	 
	Reg[0x10F0].read();	
//	Reg[0x10F0].read_device(readback,DEVICE_ADDR,0xF0);	

	FOR_EACH_SITE(nSiteIndex)
	{
		if((Reg[0x2002].Data[nSiteIndex]&BIT2)==BIT2)
		{
			Factory_Mode_Flag[nSiteIndex]=true;
			
			if((Reg[0x10F0].Data[nSiteIndex]&BIT5)==BIT5)
					Factory_Mode_Flag[nSiteIndex]=true;
				else
					Factory_Mode_Flag[nSiteIndex]=false;
		}


		else
			Factory_Mode_Flag[nSiteIndex]=false;
	}

	return flag;
}
//----------------------------------//
bool Write_CMI_ALL()
{
	
	//if(CMI_var.valid)
	if(1)
	{
		
		for(int i=0;i<CMI_var.RegisterSize;i++)
		{
			if(isVMreg(CMI_Full_address[i]))	
				continue;
			else 
			{
		//		if( Reg[CMI_Full_address[i]].Expansion==1)
		//			Reg[0x102C].set_clr_inv(BIT0); // BAND_SEL bit 0x2C[0]=1 for _2
				
					Reg[CMI_Full_address[i]].write_Trimmed();
					//delayms(1);
					//Reg[CMI_Full_address[i]].read(readback);
				
		//		if( Reg[CMI_Full_address[i]].Expansion==1)
		//			Reg[0x102C].set_clr_inv(BITNONE,BIT0); // BAND_SEL bit 0x2C[0] 
				
			}
		}
		
		return true;
	}
	else
		return false;
}

bool Write_CMI_ALL_PWREN0()
{
	
	//if(CMI_var.valid)
	if(1)
	{
		
		for(int i=0;i<CMI_var.RegisterSize;i++)
		{
			if(isVMreg(CMI_Full_address[i]))	
				continue;
			else 
			{
				//		if( Reg[CMI_Full_address[i]].Expansion==1)
				//			Reg[0x102C].set_clr_inv(BIT0); // BAND_SEL bit 0x2C[0]=1 for _2
				if(CMI_Full_address[i]==0x2035)
					Reg[CMI_Full_address[i]].write(Reg[CMI_Full_address[i]].CMI&0xfd);
				Reg[0x2035].read(readback);
				if(CMI_Full_address[i]!=0x2035)
					Reg[CMI_Full_address[i]].write_Trimmed();
				//delayms(1);
				//Reg[CMI_Full_address[i]].read(readback);
				
				//		if( Reg[CMI_Full_address[i]].Expansion==1)
				//			Reg[0x102C].set_clr_inv(BITNONE,BIT0); // BAND_SEL bit 0x2C[0] 
				
			}
		}
		
		return true;
	}
	else
		return false;
}

//-------------------------------------------//
bool isVMreg(unsigned int addr)
{
	return 	(0x10E0<=addr && addr<=0x10EF)|| (0x10F0<=addr && addr<=0x10FF)||
			(0x10C0<=addr && addr<=0x10C6)||
			(0x2000<=addr && addr<=0x2008)|| 
			(0x200B<=addr && addr<=0x2010)|| (0x2011<=addr && addr<=0x2014)||
			(addr==0x2060)|| (addr==0x2066)|| (0x2072<=addr && addr<=0x2075); 
}

bool isTrimreg(unsigned int addr)
{
	return  (addr==0x102D)||(addr==0x102E)||(0x1040<=addr && addr<=0x104D)||(0x104E<=addr && addr<=0x104F)||
			(addr==0x1050)||(0x1053<=addr && addr<=0x1055)||(0x1057<=addr && addr<=0x105B)||
			(0x2038<=addr && addr<=0x2039)||(0x2046<=addr && addr<=0x2047)||(0x204B<=addr && addr<=0x204F)||
			(0x206C<=addr && addr<=0x206F)||(0x2070<=addr && addr<=0x2071);
}

unsigned char get_read_only_bit(unsigned int addr)		//table base on CMI X101.E1
{
	switch (addr)
	{
		case 0x102D:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x102E:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E0:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E1:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E2:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E3:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E4:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E5:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E6:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E7:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E8:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10E9:	return (BIT7|BIT6|BIT5						   );
		case 0x10EB:	return (BIT7|BIT6|BIT5|BIT4|BIT3|     BIT1|BIT0);
		case 0x10EC:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2		   );
		case 0x10ED:	return (BIT7|BIT6							   );
		case 0x10EE:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10EF:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10F0:	return (	 BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x10F3:	return (				    BIT3|BIT2|BIT1|BIT0);
		case 0x10F4:	return (								   BIT0);
		case 0x10F8:	return (		  BIT5						   );
		case 0x10C1:	return (		       BIT4					   );
		case 0x10C6:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2		   );
		case 0x1025:	return (BIT7								   );
		case 0x1039:	return (					 	 BIT2|BIT1|BIT0);
		case 0x104D:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1056:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2000:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2002:	return (							  BIT1|BIT0);
		case 0x2003:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2004:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2005:	return (	 BIT6|	        BIT3			   );
		case 0x2008:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x200F:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2010:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2031:	return (					BIT3			   );
		case 0x2034:	return (BIT7|BIT6|BIT5|BIT4					   );
		case 0x2038:	return (					BIT3|BIT2|BIT1|BIT0);
		case 0x2039:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2012:	return (BIT7								   );
		case 0x2013:	return (BIT7|BIT6|BIT5|BIT4					   );
		case 0x2014:	return (								   BIT0);
		case 0x2035:	return (							  BIT1	   );
		case 0x203B:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x203C:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2045:	return (BIT7								   );
		case 0x2048:	return (	 BIT6|BIT5|BIT4|			   BIT0);
		case 0x204D:	return (	 BIT6|BIT5						   );
		case 0x204E:	return (			   BIT4|BIT3			   );
		case 0x2060:	return (BIT7|BIT6|BIT5|BIT4|               BIT0);
		case 0x2062:	return (	 BIT6|BIT5						   );
		case 0x2066:	return (BIT7|BIT6|BIT5|BIT4|		       BIT0);
		case 0x2068:	return (     BIT6|BIT5 						   );
		case 0x206D:	return (     BIT6    						   );
		case 0x206E:	return (BIT7|BIT6|BIT5|BIT4    				   );
		case 0x2070:	return (     BIT6		    				   );
		case 0x2071:	return (BIT7|BIT6|BIT5|BIT4		    		   );
		case 0x2072:	return (		    		     BIT2|BIT1|BIT0);
		case 0x2073:	return (	 BIT6	    					   );
		case 0x2074:	return (	 BIT6	    					   );
		case 0x2075:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		default:	return BITNONE;
	}
}

unsigned char get_trim_bit(unsigned int addr)		//table base on CMI X101.E1
{
	switch (addr)
	{
		case 0x1040:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1041:	return (BIT7|BIT6|BIT5|BIT4|BIT3			   );
		case 0x1042:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1043:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1044:	return (			   BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1045:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1046:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1047:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1048:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1049:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x104A:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x104B:	return (			   BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x104C:	return (	 BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x104D:	return (BIT7								   );
		case 0x104E:	return (	 BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x104F:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1050:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1053:	return (			   BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1054:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1055:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1057:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1058:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x1059:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x105A:	return (BIT7|BIT6|BIT5|BIT4|	 BIT2|BIT1|BIT0);
		case 0x105B:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2046:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2047:	return (BIT7|BIT6							   );
		case 0x204B:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x204C:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x204D:	return (		       BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x204E:	return (BIT7|BIT6|BIT5|BIT4					   );
		case 0x204F:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x206C:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);			
		case 0x206D:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x206E:	return (					BIT3|BIT2|BIT1|BIT0);
		case 0x206F:	return (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2070:	return (		  BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		case 0x2071:	return (					BIT3|BIT2|BIT1|BIT0);
		default:	return BITNONE;
	}
}
//-----------------------------------------------------//
bool Enter_Factory(double VSET, bool doCheck, bool debug)
{

	Relay.BIT[K48_SDA_SCL_I2C]			.set();
	delayms(1);
	i2c_init(HCTL_MODE, 300000, 2);
	delayms(1);
 	Fvi45.CH[VBUS].fv(0,SITE_ALL,SET_CLAMP,6,-6,10,-10);
 	Fvi45.CH[VIN_12].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
 	Fvi45.CH[VIN_5].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
 	Fvi45.CH[VCC].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
	Fvi45.CH[VBUS].close();
	Fvi45.CH[VIN_12].close();
	Fvi45.CH[VIN_5].close();
	Fvi45.CH[VCC].close();
	delayms(1);

	StartPoint=0;
	StopPoint =VSET;
	Step=0.5;
	Sweeppoint=StartPoint;
	for(i=0;i<fabs((StartPoint-StopPoint)/Step);i++)   
	{	
		Fvi45.CH[VIN_12].fv(Sweeppoint);
		
		Fvi45.CH[VBUS].fv(Sweeppoint);	
		Fvi45.CH[VCC].fv(Sweeppoint);	
		Fvi45.CH[VIN_5].fv(Sweeppoint);
		Sweeppoint += Step;
		delay(100);
	}
	Fvi45.CH[VIN_12].fv(VSET);

	Fvi45.CH[VBUS].fv(VSET);
	Fvi45.CH[VCC].mv(V1);
	Fvi45.CH[VCC].mi(I1);
	Fvi45.CH[VCC].fv(VSET+0.2);
	Fvi45.CH[VCC].mv(V1);
	Fvi45.CH[VCC].mi(I1);
	Fvi45.CH[VIN_5].fv(VSET);


	Reg[0x2002].read(readback);	
	Reg[0x10F0].read(readback);
	delayms(1);

	//Enter Factory Mode
	// Unlock PMU
	DEVICE_ADDR=0x25<<1;//{7'h25,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	

	DEVICE_ADDR=0x27<<1;//{7'h27,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	
	DEVICE_ADDR=0x67<<1;//{7'h67,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	
	DEVICE_ADDR=0x6B<<1;//{7'h6B,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	delayms(2);

	Reg[0x2002].read(readback);
	Reg[0x200A].read(readback);

	// Unlock PLP
	DEVICE_ADDR=0x24<<1;//{7'h6B,1'h0}
	Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
	
	if(QC_Flag==1)
	{
		Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
		Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
	}
	else
	Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
	Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);

	DEVICE_ADDR=0x26<<1;//{7'h6B,1'h0}
	Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
	if(QC_Flag==1)
	{
		Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
		Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
	}
	else
	Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
	Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);

	//Disable force off function by GPIO9
	Reg[0x10C3].set_clr_inv(BIT3);
	Reg[0x10C3].read(readback);




	if(doCheck)
	{
		Check_Factory_Mode();
		if(((Factory_Mode_Flag[0]==0)&&(Site_check_flag[0]==1))||((Factory_Mode_Flag[1]==0)&&(Site_check_flag[1]==1)))
		{
			Relay.BIT[K48_SDA_SCL_I2C]			.set();
			delayms(1);
			i2c_init(HCTL_MODE, 300000, 2);
			Fvi45.CH[VBUS].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VIN_12].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VIN_5].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VCC].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VBUS].close();
			Fvi45.CH[VIN_12].close();
			Fvi45.CH[VIN_5].close();
			Fvi45.CH[VCC].close();
			delayms(1);
			
			StartPoint=0;
			StopPoint =VSET;
			Step=0.5;
			Sweeppoint=StartPoint;
			for(i=0;i<fabs((StartPoint-StopPoint)/Step);i++)   
			{	
				Fvi45.CH[VIN_12].fv(Sweeppoint);
				
				Fvi45.CH[VBUS].fv(Sweeppoint);	
				Fvi45.CH[VCC].fv(Sweeppoint);	
				Fvi45.CH[VIN_5].fv(Sweeppoint);
				Sweeppoint += Step;
				delay(100);
			}
			Fvi45.CH[VIN_12].fv(VSET);
			
			Fvi45.CH[VBUS].fv(VSET);
			Fvi45.CH[VCC].mv(V1);
			Fvi45.CH[VCC].mi(I1);
			Fvi45.CH[VCC].fv(VSET+0.2);
			Fvi45.CH[VCC].mv(V1);
			Fvi45.CH[VCC].mi(I1);
			Fvi45.CH[VIN_5].fv(VSET);
			
			
			Reg[0x2002].read(readback);	
			Reg[0x10F0].read(readback);
			delayms(1);
			
			//Enter Factory Mode
			// Unlock PMU
			DEVICE_ADDR=0x25<<1;//{7'h25,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x27<<1;//{7'h27,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x67<<1;//{7'h67,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x6B<<1;//{7'h6B,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			delayms(2);
			
			Reg[0x2002].read(readback);
			Reg[0x200A].read(readback);
			
			// Unlock PLP
			DEVICE_ADDR=0x24<<1;//{7'h6B,1'h0}
			Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
			if(QC_Flag==1)
			{
				Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
				Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
			}
			else
				Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
			Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
			
			DEVICE_ADDR=0x26<<1;//{7'h6B,1'h0}
			Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
			if(QC_Flag==1)
			{
				Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
				Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
			}
			else
				Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
			Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
			
			//Disable force off function by GPIO9
			Reg[0x10C3].set_clr_inv(BIT3);
		}
			
	}

	//Force PMU Address to 4A
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if (CMI_var.registers[i].vruc_groupid==2)
		{
			Reg[CMI_Full_address[i]].Device_addr = 0x4A;
		}
	}	


	Reg[0x10C3].set_clr_inv(BIT3); // Disable force off function by GPIO

	if((QC_Flag==1)&&(Finish_reading_TrimByte==0))
	{
		for(int i=0;i<CMI_var.RegisterSize;i++)
		{

			if(isTrimreg(CMI_Full_address[i]))	//read all trim bytes
			{
				Reg[CMI_Full_address[i]].read();
				Reg[CMI_Full_address[i]].init_Flashed_Locked();
				delay(100);
			}
		}

		Finish_reading_TrimByte=true;
	}

	delayms(2);
	Reg[0x200A].read(readback);
	Reg[0x100A].read(readback);
	Reg[0x10FD].write(0x80);
	Reg[0x10FD].read(readback);
	delayms(1);
	Fvi45.CH[STR].fv(6.0,SITE_ALL,SET_CLAMP,20,-20,40,-40);
	delayms(0.5);
	Fvi45.CH[STR].fv(9.0,SITE_ALL,SET_CLAMP,20,-20,40,-40);
	delayms(0.5);
	Fvi45.CH[STR].fv(12.0,SITE_ALL,SET_CLAMP,20,-20,40,-40);
	delayms(1);
	return true;
}

bool Enter_Factory_NoSTR(double VSET, bool doCheck, bool debug)
{

	Relay.BIT[K48_SDA_SCL_I2C]			.set();
	delayms(1);
	i2c_init(HCTL_MODE, 300000, 2);
	delayms(1);
 	Fvi45.CH[VBUS].fv(0,SITE_ALL,SET_CLAMP,6,-6,10,-10);
 	Fvi45.CH[VIN_12].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
 	Fvi45.CH[VIN_5].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
 	Fvi45.CH[VCC].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
	Fvi45.CH[VBUS].close();
	Fvi45.CH[VIN_12].close();
	Fvi45.CH[VIN_5].close();
	Fvi45.CH[VCC].close();
	delayms(1);

	StartPoint=0;
	StopPoint =VSET;
	Step=0.5;
	Sweeppoint=StartPoint;
	for(i=0;i<fabs((StartPoint-StopPoint)/Step);i++)   
	{	
		Fvi45.CH[VIN_12].fv(Sweeppoint);
		
		Fvi45.CH[VBUS].fv(Sweeppoint);	
		Fvi45.CH[VCC].fv(Sweeppoint);	
		Fvi45.CH[VIN_5].fv(Sweeppoint);
		Sweeppoint += Step;
		delay(100);
	}
	Fvi45.CH[VIN_12].fv(VSET);

	Fvi45.CH[VBUS].fv(VSET);
	Fvi45.CH[VCC].mv(V1);
	Fvi45.CH[VCC].mi(I1);
	Fvi45.CH[VCC].fv(VSET+0.2);
	Fvi45.CH[VCC].mv(V1);
	Fvi45.CH[VCC].mi(I1);
	Fvi45.CH[VIN_5].fv(VSET);


	Reg[0x2002].read(readback);	
	Reg[0x10F0].read(readback);
	delayms(1);

	//Enter Factory Mode
	// Unlock PMU
	DEVICE_ADDR=0x25<<1;//{7'h25,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	

	DEVICE_ADDR=0x27<<1;//{7'h27,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	
	DEVICE_ADDR=0x67<<1;//{7'h67,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	
	DEVICE_ADDR=0x6B<<1;//{7'h6B,1'h0}
	Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
	Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
	Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
	delayms(2);

	Reg[0x2002].read(readback);
	Reg[0x200A].read(readback);

	// Unlock PLP
	DEVICE_ADDR=0x24<<1;//{7'h6B,1'h0}
	Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
	
	if(QC_Flag==1)
	{
		Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
		Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
	}
	else
	Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
	Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);

	DEVICE_ADDR=0x26<<1;//{7'h6B,1'h0}
	Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
	Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
	if(QC_Flag==1)
	{
		Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
		Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
	}
	else
	Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
	Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);

	//Disable force off function by GPIO9
	Reg[0x10C3].set_clr_inv(BIT3);
	Reg[0x10C3].read(readback);




	if(doCheck)
	{
		Check_Factory_Mode();
		if(((Factory_Mode_Flag[0]==0)&&(Site_check_flag[0]==1))||((Factory_Mode_Flag[1]==0)&&(Site_check_flag[1]==1)))
		{
			Relay.BIT[K48_SDA_SCL_I2C]			.set();
			delayms(1);
			i2c_init(HCTL_MODE, 300000, 2);
			Fvi45.CH[VBUS].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VIN_12].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VIN_5].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VCC].fv(0,SITE_ALL,SET_CLAMP,6,-6,380,-380);
			Fvi45.CH[VBUS].close();
			Fvi45.CH[VIN_12].close();
			Fvi45.CH[VIN_5].close();
			Fvi45.CH[VCC].close();
			delayms(1);
			
			StartPoint=0;
			StopPoint =VSET;
			Step=0.5;
			Sweeppoint=StartPoint;
			for(i=0;i<fabs((StartPoint-StopPoint)/Step);i++)   
			{	
				Fvi45.CH[VIN_12].fv(Sweeppoint);
				
				Fvi45.CH[VBUS].fv(Sweeppoint);	
				Fvi45.CH[VCC].fv(Sweeppoint);	
				Fvi45.CH[VIN_5].fv(Sweeppoint);
				Sweeppoint += Step;
				delay(100);
			}
			Fvi45.CH[VIN_12].fv(VSET);
			
			Fvi45.CH[VBUS].fv(VSET);
			Fvi45.CH[VCC].mv(V1);
			Fvi45.CH[VCC].mi(I1);
			Fvi45.CH[VCC].fv(VSET+0.2);
			Fvi45.CH[VCC].mv(V1);
			Fvi45.CH[VCC].mi(I1);
			Fvi45.CH[VIN_5].fv(VSET);
			
			
			Reg[0x2002].read(readback);	
			Reg[0x10F0].read(readback);
			delayms(1);
			
			//Enter Factory Mode
			// Unlock PMU
			DEVICE_ADDR=0x25<<1;//{7'h25,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x27<<1;//{7'h27,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x67<<1;//{7'h67,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			
			DEVICE_ADDR=0x6B<<1;//{7'h6B,1'h0}
			Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
			Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
			Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);	
			delayms(2);
			
			Reg[0x2002].read(readback);
			Reg[0x200A].read(readback);
			
			// Unlock PLP
			DEVICE_ADDR=0x24<<1;//{7'h6B,1'h0}
			Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
			if(QC_Flag==1)
			{
				Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
				Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
			}
			else
				Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
			Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
			
			DEVICE_ADDR=0x26<<1;//{7'h6B,1'h0}
			Reg[0x100A].write_device(0xAA,DEVICE_ADDR,0x0A);
			Reg[0x100A].read_device(readback,DEVICE_ADDR,0x0A);	
			if(QC_Flag==1)
			{
				Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
				Reg[0x104E].write_device(readback,DEVICE_ADDR,0x4E);
			}
			else
				Reg[0x104E].write_device(Reg[0x104E].Trimmed,DEVICE_ADDR,0x4E);
			Reg[0x104E].read_device(readback,DEVICE_ADDR,0x4E);
			
			//Disable force off function by GPIO9
			Reg[0x10C3].set_clr_inv(BIT3);
		}
			
	}

	//Force PMU Address to 4A
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if (CMI_var.registers[i].vruc_groupid==2)
		{
			Reg[CMI_Full_address[i]].Device_addr = 0x4A;
		}
	}	


	Reg[0x10C3].set_clr_inv(BIT3); // Disable force off function by GPIO

	if((QC_Flag==1)&&(Finish_reading_TrimByte==0))
	{
		for(int i=0;i<CMI_var.RegisterSize;i++)
		{

			if(isTrimreg(CMI_Full_address[i]))	//read all trim bytes
			{
				Reg[CMI_Full_address[i]].read();
				Reg[CMI_Full_address[i]].init_Flashed_Locked();
				delay(100);
			}
		}

		Finish_reading_TrimByte=true;
	}

	delayms(2);
	Reg[0x200A].read(readback);
	Reg[0x100A].read(readback);
	Reg[0x10FD].write(0x80);
	Reg[0x10FD].read(readback);
	delayms(1);
	Fvi45.CH[STR].fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	delayms(1);
	Fvi45.CH[STR].open();
	delayms(1);
	return true;
}

void GPIO_OFF()
{	
	//----------- Mode=Mux=0 ----------------//
	Reg[0x2015].write(_0x00); 
	Reg[0x2016].write(_0x00); 
	Reg[0x2017].write(_0x00); 
	Reg[0x2018].write(_0x00); 
	Reg[0x2019].write(_0x00); 
	Reg[0x201A].write(_0x00); 
	Reg[0x201B].write(_0x00); 
	Reg[0x201C].write(_0x00); 
	Reg[0x201D].write(_0x00); 
	Reg[0x201E].write(_0x00); 
	Reg[0x201F].write(_0x00); 
	Reg[0x2020].write(_0x00); 
	Reg[0x2021].write(_0x00); 
	Reg[0x2022].write(_0x00); 
	Reg[0x2023].write(_0x00); 
	Reg[0x2024].write(_0x00); 
 
	//----------- Set GPIOx trigger without rising/falling delay ----------------//
	Reg[0x2025].write(_0x00); 
	Reg[0x2026].write(_0x00); 
	Reg[0x2027].write(_0x00); 
	Reg[0x2028].write(_0x00); 
	Reg[0x2029].write(_0x00); 
	Reg[0x202A].write(_0x00); 
	Reg[0x202B].write(_0x00); 
	Reg[0x202C].write(_0x00); 
}

void write_ONOFF_setting()
{	
	
	//-----------DisableONOFF----------------//
	//++++++Disable_bucks+++//	
	Reg[0x10FD].write(_0x00); 
	Reg[0x10C0].write(_0x00);  
	Reg[0x1033].write(_0x00); 

	//++++++Disable_all_regulator+++//
	Reg[0x1047].write(_0x00);	
	Reg[0x1067].write(_0x00);	
	Reg[0x1087].write(_0x00);	
	Reg[0x10A7].write(_0x00);	
	Reg[0x1048].write(_0x00);	
	Reg[0x1068].write(_0x00);
	Reg[0x1088].write(_0x00);	
	Reg[0x10A8].write(_0x00);
	Reg[0x1049].write(_0x00);	
	Reg[0x1069].write(_0x00);
	Reg[0x1089].write(_0x00);	
	Reg[0x10A9].write(_0x00);
	Reg[0x10C5].write(_0x00);	
	Reg[0x10C6].write(_0x00);
	Reg[0x10C7].write(_0x00);	
	Reg[0x10C8].write(_0x00);

	//++++++Turn_off_BFET+++//	
	Reg[0x1030].set_clr_inv(BIT3); 
	Reg[0x1034].set_clr_inv(BIT1|BIT0);  
	Reg[0x1033].set_clr_inv(BITNONE,BIT1); 

	//++++++Turn_off_ADC+++//	
	Reg[0x10E8].write(_0x00);

	/*
	//++++++Disable_GPIOs+++//
	Reg[0x100D].write(_0x00);	//GPIO1
	Reg[0x100E].write(_0x00);	//GPIO2
	Reg[0x100F].write(_0x00);	//GPIO3
	Reg[0x1010].write(_0x00);	//GPIO4
	Reg[0x1011].write(_0x00);	//GPIO5
	Reg[0x1012].write(0x40);	//GPIO6
	Reg[0x1013].write(0x40);	//GPIO7
	Reg[0x1027].write(_0x00);	//GPIO8
	Reg[0x1028].write(0x60);	//GPIO9
	Reg[0x1029].write(0x60);	//GPIO10
	Reg[0x102A].write(_0x00);	//GPIO11
	
	//++++++Disable_PushPul/Disable_Pulup_Function+++//	
	Reg[0x1034].set_clr_inv(BITNONE,BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);//Disable_PushPul 
	Reg[0x1030].set_clr_inv(BITNONE,BIT0);	//Disable_PushPul 
	Reg[0x1041].set_clr_inv(BITNONE,BIT5|BIT3);	//Disable_Pulup_Function 
	Reg[0x1047].set_clr_inv(BITNONE,BIT7);	//Disable_Pulup_Function 
	Reg[0x2044].write(_0x00);
	Reg[0x2045].set_clr_inv(BITNONE,BIT6|BIT5|BIT3);//Disable_Pulup_Function 	
	Reg[0x2065].set_clr_inv(BITNONE,BIT6|BIT5|BIT3);//Disable_Pulup_Function 		
	//++++++Disable_VIO+++//	
	Reg[0x1023].set_clr_inv(BITNONE,BIT5);	
	//++++++Mask_All_Fault+++//	
	Reg[0x1009].set_clr_inv(BIT0);	
	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x00].write(0x0F);
	//-LDO--//
	Reg[0x2020].write(0xFF);
	Reg[0x2026].write(0xFF);
	Reg[0x2040].write(0xFF);
	Reg[0x2046].write(0xFF);
	Reg[0x2066].write(0x0E);
	
	Reg[0x101D].write(_0x00);	
	Reg[0x101E].write(_0x00);	
	Reg[0x101F].set_clr_inv(BIT7|BIT6|BIT5);	
	
	//-------reset_test_Bucks-----------//
	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1D].write(_0x00);	
	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1E].write(_0x00);	
	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1F].write(_0x00);	
	FOR_EACH_TILE(nTILE,nTILE_MAX_3) Reg[GP_PMU127_addr[nTILE]+0x10].set_clr_inv(BITNONE,BIT3|BIT2);	
	*/
}
void reset_test_GPIOs()
{	
	/*
	//++++++Disable_GPIOs+++//
	Reg[0x100D].write(_0x00);	//GPIO1
	Reg[0x100E].write(_0x00);	//GPIO2
	Reg[0x100F].write(_0x00);	//GPIO3
	Reg[0x1010].write(_0x00);	//GPIO4
	Reg[0x1011].write(_0x00);	//GPIO5
	Reg[0x1012].write(0x40);	//GPIO6
	Reg[0x1013].write(0x40);	//GPIO7
	Reg[0x1027].write(_0x00);	//GPIO8
	Reg[0x1028].write(0x60);	//GPIO9
	Reg[0x1029].write(0x60);	//GPIO10
	Reg[0x102A].write(_0x00);	//GPIO11
	
	//++++++Disable_PushPul/Disable_Pulup_Function+++//	
	Reg[0x1034].set_clr_inv(BITNONE,BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);//Disable_PushPul 
	Reg[0x1030].set_clr_inv(BITNONE,BIT7);	//Disable_PushPul 
	Reg[0x1041].set_clr_inv(BITNONE,BIT5|BIT3);	//Disable_Pulup_Function 
	Reg[0x1047].set_clr_inv(BITNONE,BIT7);	//Disable_Pulup_Function 
	Reg[0x2044].write(_0x00);
	Reg[0x2045].set_clr_inv(BITNONE,BIT6|BIT5|BIT3);//Disable_Pulup_Function 	
	Reg[0x2065].set_clr_inv(BITNONE,BIT6|BIT5|BIT3);//Disable_Pulup_Function 		
	//++++++Disable_VIO+++//	
	Reg[0x1023].set_clr_inv(BITNONE,BIT5);	
	*/
}
void reset_test_Bucks()
{	
//	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x04].write(_0x00);	
//	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1D].write(_0x00);	
///	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1E].write(_0x00);	
//	FOR_EACH_TILE(nTILE,PMU_MAX) Reg[GP_PMU_addr[nTILE]+0x1F].write(_0x00);	
	//--------------Disable MTP mode buck127------//
//	FOR_EACH_TILE(nTILE,nTILE_MAX_3) Reg[GP_PMU127_addr[nTILE]+0x0D].set_clr_inv(BITNONE,BIT4);
	
}
void reset_test_LDOs()
{
	/*
	FOR_EACH_TILE(nTILE,LDO_MAX) Reg[GP_LDO_addr[nTILE]+0x02].write(_0x00);	
	FOR_EACH_TILE(nTILE,LDO_MAX) Reg[GP_LDO_addr[nTILE]+0x03].write(_0x00);	
	FOR_EACH_TILE(nTILE,LDO_MAX) Reg[GP_LDO_addr[nTILE]+0x04].write(_0x00);	

//	FOR_EACH_TILE(nTILE,LDO_MAX) Reg[GP_LDO_addr[nTILE]+0x00].write(0x0E);	
	FOR_EACH_TILE(nTILE,LDO_MAX) Reg[GP_LDO_addr[nTILE]+0x00].write(0xFF);	//mask LDO1 UV/OV/ILIM fault

	//--------------LDO 12 reset--------------------------------//
	Reg[0x203C].write(_0x00);	
	Reg[0x203E].write(_0x00);	
	//--------------LDO 3456 reset--------------------------------//
	Reg[0x204D].set_clr_inv(BITNONE,BIT7|BIT6);// L5 00=LDO mode, 01=PLSW mode 10=NLSW mode 
	Reg[0x206D].set_clr_inv(BITNONE,BIT7|BIT6);// L6 00=LDO mode, 01=PLSW mode 10=NLSW mode 
	Reg[0x204E].set_clr_inv(BITNONE,BIT6);// L3 0=LDO mode, 1=PLSW mode 
	Reg[0x206E].set_clr_inv(BITNONE,BIT6);// L4 0=LDO mode, 1=PLSW mode 
 	Reg[0x205C].write(_0x00);	
 	Reg[0x205E].write(_0x00);	
 	Reg[0x207C].write(_0x00);	
 	Reg[0x207E].write(_0x00);	
	*/
}



void Unlock_passcode()
{
    //Unlock passcode to write VM Basic bytes
    DEVICE_ADDR=0x25<<1;//{7'h25,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
    Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
    Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);    
	
	
	
    DEVICE_ADDR=0x27<<1;//{7'h27,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
    Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
    Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);    
    
    DEVICE_ADDR=0x67<<1;//{7'h67,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
    Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
    Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);    
    
    DEVICE_ADDR=0x6B<<1;//{7'h6B,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
    Reg[0x2009].write_device(0x74,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0x1A,DEVICE_ADDR,0x09);
    Reg[0x2009].write_device(0xE5,DEVICE_ADDR,0x09);
    Reg[0x2009].read_device(readback,DEVICE_ADDR,0x09);    
    delayms(100);
	
	
	
    Reg[0x2002].read(readback);
	
	
	
    // Unlock PLP
    DEVICE_ADDR=0x24<<1;//{7'h6B,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);    
    DEVICE_ADDR=0x26<<1;//{7'h6B,1'h0}
    Reg[0x200A].write_device(0xAA,DEVICE_ADDR,0x0A);
}


void Default_Address()
{
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if (CMI_var.registers[i].vruc_groupid==2)
		{
			//	Reg[CMI_Full_address[i]].Device_addr = 0x4A;
			switch((Reg[0x2035].CMI&0xC0)>>6)		//Reg0x2035[7:6]
			{
			case 0x00:	
				{	
					Reg[CMI_Full_address[i]].Device_addr = 0x4A;
					Reg[0x200a].Device_addr=0x4A;
				}				
				break; // V
			case 0x01:	
				{	
					Reg[CMI_Full_address[i]].Device_addr = 0x4E;	
					Reg[0x200a].Device_addr=0x4E;
				}
				break;
			case 0x02:	
				{
					Reg[CMI_Full_address[i]].Device_addr = 0xCE;
					Reg[0x200a].Device_addr=0xCE;
				}				
				break;
			case 0x03:
				{
					Reg[CMI_Full_address[i]].Device_addr = 0xD6;	
					Reg[0x200a].Device_addr=0xD6;
				}
				break;
			}
		}
	}
}

