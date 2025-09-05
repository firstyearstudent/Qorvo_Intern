//=============  program version information ===============
// program version:  AA83Bx_9052x_Dual_V0_SDR100.prg
// date:			 2020.07.30
// Engineer:		 Penn Yin 
// update item:	     1.Initial program     
//=========================================================== 


#include "AA83Bx_9052x_Dual_V0.h"

#pragma comment(lib, "C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\Pattern\\MES.lib")
#pragma comment(lib,"system.lib") 

#include "C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\Pattern\\MES.h"

#pragma comment(lib,"C:\\VisualTest\\PEonyDriver.lib") // I2C check

CAA83Bx_9052x_Dual_V0 theApp;
#pragma comment(lib,"Ginkgo_Driver.lib") 

BOOL CAA83Bx_9052x_Dual_V0::OnUserLoad()
{
	//********************Handler_INPUT**********************
	char pszHandlerName[200];
	GetHandlerName(pszHandlerName, 200);
	//printf("Handler Name: %s\n", pszHandlerName);
		   
	if((pszHandlerName[0]==0x4d&&pszHandlerName[1]==0x61&&pszHandlerName[2]==0x6e&&pszHandlerName[3]==0x75&&pszHandlerName[4]==0x61&&pszHandlerName[5]==0x6c)!=1)
	{
		I2C_Check_Flag=true;
		Fail_Reflash=true;
		DU_Check_Flag=true;
		Flush_Check_Flag=false;
		Wafer_ID_Flash=true;
	}
	//********************Handler_INPUT**********************
	if (I2C_Check_Flag==1)
	{
		Digital_init();		
	}
	parallel_type = GetParallelType();	// Get Parallel Type (INDEX_PARALLEL or TRUE_PARALLEL)
	resource_locate();	// Set locate for each resource
	return TRUE;
}

BOOL CAA83Bx_9052x_Dual_V0::OnUserUnload()
{
	CloseAll(sErrorMsg,nErrorMsgLen);
	return TRUE;
}

BOOL CAA83Bx_9052x_Dual_V0::OnTestStartup()
{
	
	Test_debug = true;
	if(Test_debug){
		Test_debug_file=fopen("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\DebugReport.csv","w");
		fprintf(Test_debug_file,"Auto created Debug info -- by Ash Zhou\n\n");
	}
	//printf("AA83Bx_9052x_Dual_V0::UserInit()\n");
	return TRUE;
}

BOOL CAA83Bx_9052x_Dual_V0::OnTestCompletion()
{
	
	if(Test_debug){
		fprintf(Test_debug_file,"Test Completion");
		fclose(Test_debug_file);
	}
	//resource_init();
	Finish_reading_TrimByte=false;

	return TRUE;
}

BOOL CAA83Bx_9052x_Dual_V0::OnFailSite(int site)
{
	int SITE_No=site-1;
	resource_init(site-1);
	/*
	if((QC_Flag==0)&&(Fail_bin_No[SITE_No]!=1)&&(Fail_bin_No[SITE_No]!=2)&&(Fail_bin_No[SITE_No]!=3)&&Fail_Reflash)
	{
		Reg[0x10af].address=0xaf;
		Fvi45.CH[VOUT_SW]	.fv(0,SITE_No,SET_CLAMP,7,-7,400,-400);
		Fvi45.CH[VBYP]		.fv(0,SITE_No,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VIN_3V3]	.fv(0,SITE_No,SET_CLAMP,7,-7,400,-400);
		delayms(1);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5,SITE_No,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_No,SET_CLAMP,7,-7,400,-400);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_No,SET_CLAMP,7,-7,400,-400);
		delayms(5);
		Relay.BIT[K48_I2C].set(SITE_No);
		delayms(2);
		
		//Reg[0x1012].write_device(0xff,0xe0,0x12);	
		Reg[0x2007].write(0x80);
		//Reg[0x1016].write_device(0x80,0xe0,0x16);

		//Reg[0x10af].write_device(0x56,0xe0,0xaf);
		delayms(1);
		//Reg[0x10af].write_device(0xb1,0xe0,0xaf);
		delayms(1);
		//Reg[0x10af].write_device(0x4d,0xe0,0xaf);
		delayms(1);
		Reg[0x20fa].write(0x20);
		delayms(1);
		Reg[0x2000].read(readback);
		readback[0]=(Reg[0x2000].CMI&0x03)|(0xfc&readback[0]);
		readback[1]=(Reg[0x2000].CMI&0x03)|(0xfc&readback[1]);

		Reg[0x2000].write(readback,);
		//Power_off_all();
		Fvi45.CH[VBYP]		.fv(0,SITE_No,SET_CLAMP,7,-7,40,-40);
		Reg[0x2007].write(0xa0,SITE_No);

		Fvi45.CH[VBYP]		.fv(0,SITE_No,SET_CLAMP,18,-18,40,-40);
		Disable_Sequence();
		Buck_TM_LS_ON();
		Enable_Erase_TM();
		Slow_AVIN_power(VIN_SET,0.2,5.0f);
		Fvi45.CH[VOUT_SW]	.fv(5);
		Fvi45.CH[VIN_3V3]	.fv(5);
		Slow_VBYP_power(0,0.5,8.75);
		Fvi45.CH[VBYP]      .fv(8.75);
		//delayms(5000);//need increase the delay time to 4S to make the 0x55/0xaa check pass, the normal hold time is 100mS.
		delayms(300);	
		Slow_VBYP_power(8.75f,-0.5,0);
		Fvi45.CH[VBYP] .fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);	
		Slow_AVIN_power(5.0,-0.2,VIN_SET);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET);
		Fvi45.CH[VIN_3V3]	.fv(VIN_SET);
		Disable_Erase_TM();
		Reg[0x2007].write(0xa0);
		Reg[0x20fa].write(0x20);
		Buck_TM_LS_ON();
		//Flash procedure
		//Write_CMI_ALL(Flash_data);//write 0x55,0xaa,default trimmed cmi;
		Slow_AVIN_power(VIN_SET,0.2,6);	
		Fvi45.CH[VOUT_SW]	.fv(6);
		Fvi45.CH[VIN_3V3]	.fv(6);
		Enable_Flash_TM();
		delayms(2);	
		delayms(50);
		Disable_Flash_TM();	
		Slow_AVIN_power(6.0,-0.2,VIN_SET);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET);
		Fvi45.CH[VIN_3V3]	.fv(VIN_SET);
		Relay.BIT[K48_I2C].clr();
		delayms(5);
		Fvi45.CH[VOUT_SW]	.fv(0);	
		Fvi45.CH[VIN_3V3]	.fv(0);	
		delayms(2);
	}
	*/
	return TRUE;
}
// Test Function: GET_CMI
STT_TESTFUNC GET_CMI(CFunction *pFunction, int nSiteNo, int nTestFlag, GET_CMI_params* ours)
{
	//=====================External variable to control the program==========================
	int Convert_Value=0;

	Convert_Value=ours->QC;//input QC or not, 0:FT program,1:QA program
	if(Convert_Value==0) 
		QC_Flag=false;
	else 
		QC_Flag=true;
	Convert_Value=ours->GNO;//input CTP or GNG, 0:CTP program,1:GNG program
	if(Convert_Value==0) 
		GNG_Flag=false;
	else 
		GNG_Flag=true;
 	CMI_Rev_No=ours->SPN;//input the CMI No here.	
	VIN_SET=ours->VIN;//input 3.0/3.3/3.6V for temp char test, normal set 3.3;
	//=====================External variable to control the program==========================

 	bool getCMI_success=FALSE;
	//=====================Get_Original_Register==========================

	if(Finish_reading_CMI_file==0)
	{
		//WLSCP parts: 90520
		
		/*
		if(CMI_Rev_No==520100)//multiphase draft cmi
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\E2_MTP.E0.B70CE283.cmi",&CMI_var);	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520.E100.ECA576B6.cmi",&CMI_var);	
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-MTP.E100.AAD9F0E7.cmi",&CMI_var);	
		}
		*/
		if(CMI_Rev_No==520101)//BZ start from 20201205
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-101.E1.36147F69.cmi",&CMI_var);//20201205				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-101.E2.786FBBB6.cmi",&CMI_var);//20201207				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-101.E2B.FF73B2F9.cmi",&CMI_var);//20201209
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-101.E2D.1A78E09C.cmi",&CMI_var);//20210106				
		}
		if(CMI_Rev_No==520102)//20200928 in_use for CSP part
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E1.1D57088E.cmi",&CMI_var);
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E2.C11A127A.cmi",&CMI_var);
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E3.699D6161.cmi",&CMI_var);
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E3B.9B788C5C.cmi",&CMI_var);		
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E3B.1DBD0FE1.cmi",&CMI_var);
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E4.38D5A84C.cmi",&CMI_var);	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E4B.0D9C8855.cmi",&CMI_var);//20200928				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E6.6800BAC7.cmi",&CMI_var);//20201207				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E6B.D2A4D775.cmi",&CMI_var);//20201209				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E6C.EEB977EE.cmi",&CMI_var);//20201210	
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E6E.AFE0A297.cmi",&CMI_var);//20210106				
		}
		
		if(CMI_Rev_No==520103)//20200928 in_use for CSP part
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E1.4EFD2629.cmi",&CMI_var);		
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E1B.F0C33CCA.cmi",&CMI_var);		
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E2.CF815869.cmi",&CMI_var);
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E3.AE699216.cmi",&CMI_var);//20200911	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E3B.F7BC3429.cmi",&CMI_var);//20200917	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E3C.75F3FD07.cmi",&CMI_var);//20200922
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E4B.9AD72329.cmi",&CMI_var);//20201028				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E5.E81FC56C.cmi",&CMI_var);//20201205				
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E5B.16D18811.cmi",&CMI_var);//20201209	
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E5D.9290D851.cmi",&CMI_var);//20210106					
		}	
		/*
		if(CMI_Rev_No==520105)//multiphase draft cmi
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-105-MUL_Draft.E1.104BD3F9.cmi",&CMI_var);//20201223	
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-105-MUL_DRAFT.E2.8F4B7BF4.cmi",&CMI_var);//20201230		
		}
		*/
		if(CMI_Rev_No==521101)//multiphase draft cmi
		{
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90521-101.E1.ED8ED2AE.cmi",&CMI_var);//20210119	
		}

		//BGA parts: 90522
		/*
		if(CMI_Rev_No==522100)//multiphase draft cmi
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\E2_MTP.E0.B70CE283.cmi",&CMI_var);	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520.E100.ECA576B6.cmi",&CMI_var);
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-MTP.E100.AAD9F0E7.cmi",&CMI_var);	
		}
		*/
		if(CMI_Rev_No==522101)//20201216 in_use for BGA part
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-101.E2.5C1AC217.cmi",&CMI_var);	//20201216
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-101.E2B.70CE4BC2.cmi",&CMI_var);	//20210105
		}
		if(CMI_Rev_No==522102)//20201009 in_use for BGA part
		{
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-102.E4B.0D9C8855.cmi",&CMI_var);//20200928		
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E4C.B43B157E.cmi",&CMI_var);//20201009
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E5.FD6B2F46.cmi",&CMI_var);//20201009	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E5B.6D0B1C8C.cmi",&CMI_var);//20201110		
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E6B.2C700811.cmi",&CMI_var);//20201205	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E6C.C7922317.cmi",&CMI_var);//20201215
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-102.E6D.204F61CC.cmi",&CMI_var);//20210105			
		}	
		if(CMI_Rev_No==522103)//20201009 in_use for BGA part
		{	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-103.E3C.75F3FD07.cmi",&CMI_var);//20200922
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-103.E3E.CE5AAD9D.cmi",&CMI_var);//20201009
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-103.E5.AF26A195.cmi",&CMI_var);//20201215
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-103.E5B.698B9926.cmi",&CMI_var);//20210105
		}
		if(CMI_Rev_No==522104)//BZ start from 20201205
		{	
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-104.E1.82D39D8E.cmi",&CMI_var);//20201205
			//getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-104.E1B.2C56AD72.cmi",&CMI_var);//20201215
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-104.E1C.3F3C470D.cmi",&CMI_var);//20210105
		}
		/*
		if(CMI_Rev_No==522105)//multiphase draft cmi
		{
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90520-105-MUL_DRAFT.E2.8F4B7BF4.cmi",&CMI_var);//20201230		
		}
		*/
		if(CMI_Rev_No==523103)//multiphase draft cmi
		{
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90523-101.E1.CE94BCBB.cmi",&CMI_var);//20210119	
		}
		if(CMI_Rev_No==522601)//20210120 in_use for BGA part
		{
			getCMI_success = getCMI("C:\\VisualTest\\apps\\AA83Bx_9052x_Dual_V0\\programs\\ACT90522-601.E1.D078AC7E.cmi",&CMI_var);//20210120	
		}

		for(i=0;i<CMI_var.RegisterSize;i++)
		{
		    CMI_Full_address[i]=CMI_var.registers[i].vruc_groupid<<12|CMI_var.registers[i].vruc_expansion<<8|CMI_var.registers[i].address;
			Reg[CMI_Full_address[i]].CMI_exist    = true;
			Reg[CMI_Full_address[i]].address      = CMI_var.registers[i].address;
			Reg[CMI_Full_address[i]].CMI	      = CMI_var.registers[i].value;
			Reg[CMI_Full_address[i]].GroupID	  = CMI_var.registers[i].vruc_groupid;
			Reg[CMI_Full_address[i]].Expansion	  = CMI_var.registers[i].vruc_expansion;
		}
		//CMI debug here
		//CMI debug here

		//*************Get_Package_type***************
		package_type=(Reg[0x2003].CMI&0x20)>>5;//bit=0: WLCSP; bit=1: BGA
		//*************Get_Package_type Lot***************

		//*************Get_Wafer Lot***************
		char Wafer_DieID[200] = {'\0'};//Disable it when manual check
		//char Wafer_DieID[200] = {'H','S','0','1','0','3','1','-','1','4','A','1'};//For manual check	
		
		//if(Wafer_ID_Flash&&(CMI_Rev_No==520100||CMI_Rev_No==520101||CMI_Rev_No==520102||CMI_Rev_No==520103))
		if(Wafer_ID_Flash&&(package_type==0))
		{
			GetWaferID(Wafer_DieID,20);//Disable it when manual check
			GetWaferDieID(Wafer_DieID,&waferlot,&wafer_Laser_no,&WaferIDflg);//decoding the GPIB laser code to strings
			Wafer_Slot_No=GetSlotNo();//decoding the GPIB laser code to strings
			//Wafer_Slot_No=wafer_Laser_no;//enable it for manual check
	
			printf("*****Code Wafer Lot*****\n");					
			printf("Wafer Lot: %d\n", waferlot);
			printf("Wafer Laser No: %d\n", wafer_Laser_no);
			printf("Wafer Slot No: %d\n", Wafer_Slot_No);
			printf("Wafer IDflg: %d\n", WaferIDflg);
			
			char message[]="WARNING:\nWafer Laser Num is NOT match with Slot Num!!!";
			if((Wafer_Slot_No==wafer_Laser_no)&&wafer_Laser_no!=0)
				Wafer_No_Flag=true;//Wafer_Slot_No,wafer_Laser_no: 01-25
			else
				MessageBox(NULL,message,"WARNING",MB_OK);
			
			//Wafer Lot
			Wafer_Lot_3=(waferlot&0x3f000000)>>24;
			Wafer_Lot_4=(waferlot&0x00fc0000)>>18;
			Wafer_Lot_5=(waferlot&0x0003f000)>>12;
			Wafer_Lot_6=(waferlot&0x00000fc0)>>6;
			Wafer_Lot_7= waferlot&0x0000003f;
			//Wafer Lot
			Reg[0x103b].CMI=Reg[0x103b].CMI|(Wafer_Lot_3<<2);//slot3
			
			Reg[0x103b].CMI=Reg[0x103b].CMI|(Wafer_Lot_4&0x30)>>4;//slot4
			Reg[0x103c].CMI=Reg[0x103c].CMI|(Wafer_Lot_4&0x0f)<<4;
			
			Reg[0x103c].CMI=Reg[0x103c].CMI|(Wafer_Lot_5&0x3c)>>2;//slot5
			Reg[0x103d].CMI=Reg[0x103d].CMI|(Wafer_Lot_5&0x03)<<6;
			
			Reg[0x103d].CMI=Reg[0x103d].CMI|Wafer_Lot_6;//slot6
			Reg[0x103e].CMI=Reg[0x103e].CMI|Wafer_Lot_7<<2;//slot7
			//Wafer No
			Reg[0x109c].CMI=Reg[0x109c].CMI|(wafer_Laser_no<<3);
		}
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			Reg[CMI_Full_address[i]].init_data();
		}		
		//*************Get_Datecode***************
		//********************DateCode_INPUT**********************

		//========= Set I2C addr for reg1 and reg 2===================
		//User_Addr=0xe0+2*(Reg[0x2000].CMI&0x03);
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			if (CMI_var.registers[i].vruc_groupid==1)
				Reg[CMI_Full_address[i]].Device_addr = 0xe0;//Use fixed i2c addr
			if (CMI_var.registers[i].vruc_groupid==2)
				Reg[CMI_Full_address[i]].Device_addr = 0xc6;
		}
		//========= read Buck1/2/3/4/5_Set===================
		double Ind[PMU_MAX]={1,1,1,1,0.47},Ton_offset[PMU_MAX]={0,0,-10,20,-30};
		for(nTILE =0; nTILE<PMU_MAX; nTILE++)
		{
			if(nTILE==0||nTILE==1)
			{
				if((Reg[0x2047].CMI>>5)&0x01==1)
				{
					PMU_ACOT_MODE[nTILE]=false;
					PMU_MultiP_Mode[nTILE]=true;
				}				
			}
			if((Reg[GP_Factory_Buck_Addr[nTILE]+0x07].CMI>>6)&0x01==1)
			{
				PMU_VTH_HDUTY[nTILE]=true;
			}				

			//Buck_Vset
			Buck_Vset[nTILE]=0.6+0.005*(1+((Reg[GP_Buck_Vrange_Addr[nTILE]].CMI&0x02)>>1))*Reg[GP_Buck_Vset_Addr[nTILE]].CMI;

			//Buck_burst_Vset
			Buck_burst_Vset[nTILE]=1.04*Buck_Vset[nTILE];

			//Buck_Ton_Trim_Target
			//FOR_EACH_SITE(nSiteIndex)	
				//Buck_Ton_Trim_Target[nTILE][nSiteIndex]=1e3*Buck_Vset[nTILE]/VIN_SET/2/0.95-Ton_offset[nTILE];	// Ton_target = 1000*VOUT_SET/VIN/(Freq*Kfreq)
																			// ->VIN = 3.3V
																			// ->VOUT_SET = 0.6V + 5mV*VSET0*(1+VRANGE)
																			// ->Freq = 2.0 MHz
																			// -> Kfreq=0.95
			//Buck_ILIMIT_Target
			Buck_HSILIM_Target[nTILE]=(Buck_ILIMIT_Set[nTILE][(Reg[GP_Buck_ILIMIT_Set_Addr[nTILE]].CMI&0x0c)>>2])/5;
			Buck_HSILIM_Warn_Target[nTILE]=0.8*Buck_HSILIM_Target[nTILE];
			Buck_HSILIM_Shutdown_Target[nTILE]=1.225*Buck_HSILIM_Target[nTILE];

			Buck_LSILIM_Target[nTILE]=1.05*Buck_HSILIM_Target[nTILE];//Updated to 1.1 by Joe on 9/16/2020
            //Buck_LSILIM_Target[nTILE]=(Buck_HSILIM_Target[nTILE]*5-1e3*0.5*Buck_Ton_Trim_Target[nTILE]/1e9*(3.8-Buck_Vset[nTILE])/Ind[nTILE]*1e6)/5; //Internal check only
																			//Request by Joe on 22 Sept
																			//IOUT_MAX = HSILIM - delta_IL/2
																			//delta_IL = TON*(Vin_max - VOUT_setting)/L
																			//HSILIM is follow CMI
																			//VOUT_setting is follow CMI
																			//Vin_max = 3.8V
																			//TON is follow the TON trim target.
																			//L = 0.47uH for BUCK1/2/3/4 //L = 1uH for BUCK5 

			//Buck_ZC_Target	
			/*
			Buck_ZC_Target[nTILE][0]=-((19*pow(Buck_Vset[nTILE],2)-117*Buck_Vset[nTILE])+263)*Buck_Vset[nTILE]/0.47;
			if(tile==4)
				Buck_ZC_Target[nTILE][0]=-((19*pow(Buck_Vset[nTILE],2)-117*Buck_Vset[nTILE])+263)*Buck_Vset[nTILE]/1.0;
				*/
			
		}
		
		//========= read VLDO_SET ===================
		//LDO1
		LDO_Vset[0]=0.6+0.005*(1+((Reg[0x1092].CMI&0x08)>>3))*Reg[0x1096].CMI;
		//LDO2
		LDO_Vset[1]=0.6+0.005*(1+((Reg[0x1092].CMI&0x04)>>2))*Reg[0x1098].CMI;

		if(!Finish_reading_CMI_file)
		{				
			//---------------------------Device ID
			printf("\n---------------------------------------------------");
			printf("\n Device ID: %-x ",CMI_var.Device_ID);
			
			//---------------------------CODE
			printf("\n---------------------------------------------------");
			printf("\n Code: ");
			for(i=0;i<CMI_var.Code_length;i++)
			{			
				printf("%c",CMI_var.Code[i]);
			}
			
			//---------------------------CODING MATRIX ID
			printf("\n---------------------------------------------------");
			printf("\n Coding Matrix ID: ");
			for(i=0;i<CMI_var.codingmatrix_length;i++)
			{			
				printf("%c",CMI_var.codingmatrixid[i]);
			}
			
			//---------------------------Rawpartnumber.
			printf("\n---------------------------------------------------");
			printf("\n Raw Part Number: ");
			for(i=0;i<CMI_var.rawpartnumber_length;i++)
			{			
				printf("%c",CMI_var.rawpartnumber[i]);
			}
			//CMI_Rev_No=CMI_var.rawpartnumber[i-1];
			
			printf("\n---------------------------------------------------");
			printf("\n Number of Registors is: %d",CMI_var.RegisterSize);	
			printf("\n---------------------------------------------------");
										
			// Print Vset_Vout
			printf("\n-------------------------------------------------");
			printf("\n VIN_SET=%2.3fV", VIN_SET); 
			printf("\n----------------------Bucks_Vout_set-----------------------------");
 			printf("\n Buck1_Vout=%2.3fV", Buck_Vset[0]); 
 			printf("\n Buck2_Vout=%2.3fV", Buck_Vset[1]); 
 			printf("\n Buck3_Vout=%2.3fV", Buck_Vset[2]); 
 			printf("\n Buck4_Vout=%2.3fV", Buck_Vset[3]); 
			printf("\n Buck5_Vout=%2.3fV", Buck_Vset[4]); 
			//printf("\n----------------------Bucks_Ton_set-----------------------------");
			/*
			FOR_EACH_SITE(nSiteIndex)	
			{
				printf("\n Buck1_Ton=%3.3fnS", Buck_Ton_Trim_Target[0][nSiteIndex]); 
				printf("\n Buck2_Ton=%3.3fnS", Buck_Ton_Trim_Target[1][nSiteIndex]); 
				printf("\n Buck3_Ton=%3.3fnS", Buck_Ton_Trim_Target[2][nSiteIndex]); 
				printf("\n Buck4_Ton=%3.3fnS", Buck_Ton_Trim_Target[3][nSiteIndex]); 
				printf("\n Buck5_Ton=%3.3fnS", Buck_Ton_Trim_Target[4][nSiteIndex]); 
			}
			*/
			printf("\n----------------------Bucks_ILIMIT_set-----------------------------");
			printf("\n Buck1_HSILIM_Target=%4.1fmA", Buck_HSILIM_Target[0]); 
			printf("\n Buck2_HSILIM_Target=%4.1fmA", Buck_HSILIM_Target[1]); 
			printf("\n Buck3_HSILIM_Target=%4.1fmA", Buck_HSILIM_Target[2]); 
			printf("\n Buck4_HSILIM_Target=%4.1fmA", Buck_HSILIM_Target[3]); 
			printf("\n Buck5_HSILIM_Target=%4.1fmA", Buck_HSILIM_Target[4]); 
			//printf("\n----------------------Bucks_ZC_set-----------------------------");
			/*
			printf("\n Buck1_ZC_Target=%4.1fmA", Buck_ZC_Target[0]); 
			printf("\n Buck2_ZC_Target=%4.1fmA", Buck_ZC_Target[1]); 
			printf("\n Buck3_ZC_Target=%4.1fmA", Buck_ZC_Target[2]); 
			printf("\n Buck4_ZC_Target=%4.1fmA", Buck_ZC_Target[3]); 
			printf("\n Buck5_ZC_Target=%4.1fmA", Buck_ZC_Target[4]); 
			*/
			printf("\n----------------------LDO_Vout_set-----------------------------");
			printf("\n LDO1_Vout=%2.3fV", LDO_Vset[0]); 
			printf("\n LDO2_Vout=%2.3fV", LDO_Vset[1]); 
			printf("\n----------------------LDO_ILIMIT_set-----------------------------");
			printf("\n LDO1_ILIM_Target=%4.1fmA", LDO_ILIM[0]); 
			printf("\n LDO1_ILIM_Target=%4.1fmA", LDO_ILIM[1]); 	
			// Print All CMI		
			//printf("\n----------------------Register Lists-------------------------");	
			
			//for(i=0;i<CMI_var.RegisterSize;i++)
				//printf("\n reg0X%02X data=0X%02X Group=%d exp=%d",CMI_Full_address[i],Reg[CMI_Full_address[i]].CMI,Reg[CMI_Full_address[i]].GroupID,Reg[CMI_Full_address[i]].Expansion);
            
 			printf("\n--------------------------Read CMI Done");	
 			//printf("\n");
 			Finish_reading_CMI_file=TRUE;
 		}
	}
 	else
	{	
		for(i=0;i<CMI_var.RegisterSize;i++)
			if(isVMreg(CMI_Full_address[i]))
			{
				Reg[CMI_Full_address[i]].init_data();
			}
	}
	Buck_MultiPhaseEN[0]=(Reg[0x2047].CMI&0x20)>>5;
	Buck_MultiPhaseEN[1]=Buck_MultiPhaseEN[0];
	//********************Wafer XY location**********************
	int X_location[SITE_MAX]={0},Y_location[SITE_MAX]={0},X_location_Flash[SITE_MAX]={0},Y_location_Flash[SITE_MAX]={0};
	
	//=====================Get_X/Y location for all sites==========================
	FOR_EACH_SITE(nSiteIndex)
	{
		GetXYCoordinate(nSiteIndex+1,X_location[nSiteIndex],Y_location[nSiteIndex]);
	}
	//=====================Get_X/Y location for all sites==========================
	//if(Wafer_ID_Flash&&(CMI_Rev_No==520100||CMI_Rev_No==520101||CMI_Rev_No==520102||CMI_Rev_No==520103))
	if(Wafer_ID_Flash&&(package_type==0))
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			//X/Y location
			Reg[0x109d].Trimmed[nSiteIndex]=Reg[0x109d].CMI|(X_location[nSiteIndex]<<1);
			Reg[0x109e].Trimmed[nSiteIndex]=Reg[0x109e].CMI|(Y_location[nSiteIndex]<<0);
		}
	}
	//********************Wafer XY location**********************
	//********************Site_Check**********************
	if((Site_Flag[0]&&Site_Flag[1])==0)
	{
		Relay.BIT[K44_FQVI8].set();
		Fqvi.CH[Site_Check]	.fv(0,SITE_ALL,SET_CLAMP,4,-4,4,-4);
		delayms(1);
		Fqvi.CH[Site_Check]	.fv(2);
		delayms(2);
		Fqvi.CH[Site_Check]	.mi(I_read);
		
		if(I_read[0]>1.5&&I_read[0]<2.5)
			Site_Flag[0]=1;
		
		if(I_read[1]>0.8&&I_read[1]<1.2)
			Site_Flag[1]=1;
		Relay.BIT[K44_FQVI8].clr();
		delayms(2);
		Fqvi.CH[Site_Check]	.fv(0);
		Fqvi.CH[Site_Check]	.open();

	}
	//********************Site_Check**********************
 	FOR_EACH_SITE(nSiteIndex)
 	{
		pFunction->LogResult(NSITEINDEX, 1, getCMI_success);		//First read CMI successful
 		pFunction->LogResult(NSITEINDEX, 2, QC_Flag);
 		pFunction->LogResult(NSITEINDEX, 3, GNG_Flag);
 		pFunction->LogResult(NSITEINDEX, 4, CMI_Rev_No);
		pFunction->LogResult(NSITEINDEX, 5, VIN_SET);
		//if(Wafer_ID_Flash&&(CMI_Rev_No==520100||CMI_Rev_No==520101||CMI_Rev_No==520102||CMI_Rev_No==520103))
		if(Wafer_ID_Flash&&(package_type==0))
		{
			pFunction->LogResult(NSITEINDEX, 6, Wafer_Lot_3);
			pFunction->LogResult(NSITEINDEX, 7, Wafer_Lot_4);
			pFunction->LogResult(NSITEINDEX, 8, Wafer_Lot_5);
			pFunction->LogResult(NSITEINDEX, 9, Wafer_Lot_6);
			pFunction->LogResult(NSITEINDEX, 10, Wafer_Lot_7);
			pFunction->LogResult(NSITEINDEX, 11, wafer_Laser_no);
			pFunction->LogResult(NSITEINDEX, 12, Wafer_Slot_No);
			pFunction->LogResult(NSITEINDEX, 13, Wafer_No_Flag);
			pFunction->LogResult(NSITEINDEX, 14, X_location[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15, Y_location[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 16, WaferIDflg);
		}
		pFunction->LogResult(NSITEINDEX, 17, Site_Flag[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 18, Buck_MultiPhaseEN[0]);
		pFunction->LogResult(NSITEINDEX, 19, Buck_MultiPhaseEN[1]);
 	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: OS
STT_TESTFUNC OS(CFunction *pFunction, int nSiteNo, int nTestFlag, OS_params* ours)
{
	//OS_Main(pFunction, nSiteNo, nTestFlag, ours);
	double	OS_VIN_BUCK[5][SITE_MAX],OS_SW[5][SITE_MAX],OS_FB[5][SITE_MAX];//14
	double	OS_VINLDO[2][SITE_MAX],OS_OUTLDO[2][SITE_MAX];//18
	double  OS_nRESET[SITE_MAX],OS_nIRQ[SITE_MAX],OS_GPIO1[SITE_MAX],OS_GPIO0[SITE_MAX],OS_LPM[SITE_MAX],//23
		    OS_EXT_PG[SITE_MAX],OS_EXT_EN[SITE_MAX],OS_SCL[SITE_MAX],OS_SDA[SITE_MAX],OS_VOUT_SW[SITE_MAX],//28
			OS_GATE[SITE_MAX],OS_CORE_PG[SITE_MAX],OS_VIN_3V3[SITE_MAX],OS_VBYP[SITE_MAX],OS_PWRDN0[SITE_MAX],OS_PWRDN1[SITE_MAX];//34

	FOR_EACH_SITE(nSiteIndex)
	{
		if(IsSiteActive(nSiteIndex))
			Fail_bin_No[nSiteIndex]=1;
	}
	Relay.BIT[K48_I2C].clr();
	Disconnect_all_cap();
	for(nTILE=0;nTILE<PMU_MAX;nTILE++)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
		Fqvi.CH[GP_SW_Buck[nTILE]]			.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	}

	for(nTILE=0;nTILE<LDO_MAX;nTILE++)
	{
		Fvi16.CH[GP_INLDO[nTILE]]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
		Fqvi.CH[GP_OUTLDO[nTILE]]		.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	}

	Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	Fvi45.CH[VBYP]		    .fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	Fvi45.CH[VIN_3V3]	    .fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);

													   													 													 											
	Fvi16.CH[PWRDN0]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	Fvi16.CH[PWRDN1]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	
	Fvi16.CH[SDA]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	Fvi16.CH[SCL]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	
	Fvi16.CH[GPIO0]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	Fvi16.CH[GPIO1]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);	

																									 
	Fvi16.CH[nIRQ]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	Fvi16.CH[CORE_PG]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);

	Fvi16.CH[LPM]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);
	Fvi16.CH[EXT_EN]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);

	Fvi16.CH[GATE]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);

	Fvi16.CH[nRESET]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);

	Fvi16.CH[EXT_PG]		.fv(0,SITE_ALL,Stall3V3,Stall500uA);


	for(nTILE=0;nTILE<PMU_MAX;nTILE++)
	{
		if(tile==4)
		{
			Relay.BIT[K41_FQVI5].set();
			delayms(2);
		}
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.fi(-0.5);
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.fi(-0.1);
		delayms(1);
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mv(OS_VIN_BUCK[nTILE]);
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.fv(0);

		Fvi45.CH[GP_FB_Buck[nTILE]]		.fi(-0.5);
		delayms(2);
		Fvi45.CH[GP_FB_Buck[nTILE]]		.fi(-0.1);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]]		.mv(OS_FB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]		.fv(0);

		Fqvi.CH[GP_SW_Buck[nTILE]]		.fi(-0.1);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]]		.mv(OS_SW[nTILE]);
		Fqvi.CH[GP_SW_Buck[nTILE]]		.fv(0);
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
    Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();
	delayms(2);

	for(nTILE=0;nTILE<LDO_MAX;nTILE++)
	{
		Fvi16.CH[GP_INLDO[nTILE]]		.fi(-0.5);
		delayms(2);
		Fvi16.CH[GP_INLDO[nTILE]]		.fi(-0.1);
		delayms(1);
		Fvi16.CH[GP_INLDO[nTILE]]		.mv(OS_VINLDO[nTILE]);
		Fvi16.CH[GP_INLDO[nTILE]]		.fv(0);
				
		Fqvi.CH[GP_OUTLDO[nTILE]]		.fi(-0.1);
		delayms(1);
		Fqvi.CH[GP_OUTLDO[nTILE]]		.mv(OS_OUTLDO[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]]		.fv(0);		
	}

    Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();


	Fvi45.CH[VOUT_SW]		.fi(-0.5);
	delayms(5);
	Fvi45.CH[VOUT_SW]		.fi(-0.1);
	delayms(1);
	Fvi45.CH[VOUT_SW]		.mv(OS_VOUT_SW);
	Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);

	
	Fvi45.CH[VBYP]		.fi(-0.5);
	delayms(1);
	Fvi45.CH[VBYP]		.fi(-0.1);
	delayms(1);
	Fvi45.CH[VBYP]		.mv(OS_VBYP);
	Fvi45.CH[VBYP]		.fv(0);

	Fvi45.CH[VIN_3V3]		.fi(-0.5);
	delayms(1);
	Fvi45.CH[VIN_3V3]		.fi(-0.1);
	delayms(1);
	Fvi45.CH[VIN_3V3]		.mv(OS_VIN_3V3);
	Fvi45.CH[VIN_3V3]		.fv(0,SITE_ALL,SET_CLAMP,2,-2,4,-4);
	
	Fvi16.CH[SCL]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[SCL]		.mv(OS_SCL);
	Fvi16.CH[SCL]		.fv(0);

	Fvi16.CH[SDA]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[SDA]		.mv(OS_SDA);
	Fvi16.CH[SDA]		.fv(0);

	Fvi16.CH[PWRDN0]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[PWRDN0]		.mv(OS_PWRDN0);
	Fvi16.CH[PWRDN0]		.fv(0);

	Fvi16.CH[PWRDN1]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[PWRDN1]		.mv(OS_PWRDN1);
	Fvi16.CH[PWRDN1]		.fv(0);

	Fvi16.CH[GPIO0]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[GPIO0]		.mv(OS_GPIO0);
	Fvi16.CH[GPIO0]		.fv(0);

	Fvi16.CH[GPIO1]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[GPIO1]		.mv(OS_GPIO1);
	Fvi16.CH[GPIO1]		.fv(0);

	Fvi16.CH[nIRQ]		.fi(-0.1);
	Fvi16.CH[nIRQ]		.mv(OS_nIRQ);
	Fvi16.CH[nIRQ]		.fv(0);

	Fvi16.CH[CORE_PG]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[CORE_PG]		.mv(OS_CORE_PG);
	Fvi16.CH[CORE_PG]		.fv(0);

	Fvi16.CH[LPM]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[LPM]		.mv(OS_LPM);
	Fvi16.CH[LPM]		.fv(0);

	Fvi16.CH[EXT_EN]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[EXT_EN]		.mv(OS_EXT_EN);
	Fvi16.CH[EXT_EN]		.fv(0);

	Fvi16.CH[GATE]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[GATE]		.mv(OS_GATE);
	Fvi16.CH[GATE]		.fv(0);

	Fvi16.CH[nRESET]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[nRESET]		.mv(OS_nRESET);
	Fvi16.CH[nRESET]		.fv(0);
	
	Fvi16.CH[EXT_PG]		.fi(-0.1);
	delayms(1);
	Fvi16.CH[EXT_PG]		.mv(OS_EXT_PG);
	Fvi16.CH[EXT_PG]		.fv(0);

	for(nTILE=0;nTILE<PMU_MAX;nTILE++)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.open();
		Fvi45.CH[GP_FB_Buck[nTILE]]			.open();
		Fqvi.CH[GP_SW_Buck[nTILE]]			.open();
	}
	for(nTILE=0;nTILE<LDO_MAX;nTILE++)
	{
		Fvi16.CH[GP_INLDO[nTILE]]		.open();
		Fqvi.CH[GP_OUTLDO[nTILE]]		.open();
	}
	Fvi45.CH[VOUT_SW]		.open();
	Fvi45.CH[VBYP]		    .open();
	Fvi45.CH[VIN_3V3]	    .open();
	
	Fvi16.CH[SCL]		.open();
	Fvi16.CH[SDA]		.open();
	
	Fvi16.CH[PWRDN0]		.open();
	Fvi16.CH[PWRDN1]		.open();
	
	Fvi16.CH[GPIO0]		.open();
	Fvi16.CH[GPIO1]		.open();
	
	
	Fvi16.CH[nIRQ]		.open();
	Fvi16.CH[CORE_PG]		.open();
	
	Fvi16.CH[LPM]		.open();
	Fvi16.CH[EXT_EN]		.open();
	
	Fvi16.CH[GATE]		.open();
		
	Fvi16.CH[nRESET]		.open();
	
	Fvi16.CH[EXT_PG]		.open();
	delayms(2);
	FOR_EACH_SITE(nSiteIndex)
	{	
		//int print_Index=1;
		pFunction->LogResult(NSITEINDEX,  1, OS_VIN_BUCK[0][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  2, OS_VIN_BUCK[2][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  3, OS_VIN_BUCK[3][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  4, OS_VIN_BUCK[4][nSiteIndex]); //4

		pFunction->LogResult(NSITEINDEX,  5, OS_SW[0][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  6, OS_SW[1][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  7, OS_SW[2][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  8, OS_SW[3][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  9, OS_SW[4][nSiteIndex]); //9

		pFunction->LogResult(NSITEINDEX,  10, OS_FB[0][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  11, OS_FB[1][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  12, OS_FB[2][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  13, OS_FB[3][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  14, OS_FB[4][nSiteIndex]); //14

		pFunction->LogResult(NSITEINDEX,  15, OS_VINLDO[0][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  16, OS_VINLDO[1][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  17, OS_OUTLDO[0][nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  18, OS_OUTLDO[1][nSiteIndex]); //18

		pFunction->LogResult(NSITEINDEX,  19, OS_VOUT_SW[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  20, OS_VIN_3V3[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  21, OS_VBYP[nSiteIndex]); //21

		pFunction->LogResult(NSITEINDEX,  22, OS_SCL[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  23, OS_SDA[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  24, OS_PWRDN0[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  25, OS_PWRDN1[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  26, OS_GPIO0[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  27, OS_GPIO1[nSiteIndex]); //27

		pFunction->LogResult(NSITEINDEX,  28, OS_nIRQ[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  29, OS_CORE_PG[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  30, OS_LPM[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  31, OS_EXT_EN[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  32, OS_GATE[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  33, OS_nRESET[nSiteIndex]); 
		pFunction->LogResult(NSITEINDEX,  34, OS_EXT_PG[nSiteIndex]); //34
	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: DU_Check
STT_TESTFUNC DU_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, DU_Check_params* ours)
{
	int DU_Check_result[SITE_MAX]={0},Diff_count[SITE_MAX]={0}, wafer_ID_No=0;
	int Wafer_ID_Addr[7]={0x103b,0x103c,0x103d,0x103e,0x109c,0x109d,0x109e},Wafer_ID_Data[SITE_MAX][7]={0};
	Reg[0x10af].address=0xaf;
	if((QC_Flag==0)&&DU_Check_Flag&&(package_type==1))
	{
		Connect_all_cap();
		delayms(1);
		Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
		Fvi45.CH[VBYP]		.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
		delayms(1);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		delayms(5);
		Relay.BIT[K48_I2C].set();
		delayms(2);
		i2c_init(HCTL_MODE,300000,2);
		delayms(2);	
		//==============================check register code===========================
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			Reg[cmi_mask[i].reg_addr].read(readback);
			
			for(j=0;j<7;j++)
			{
				if(cmi_mask[i].reg_addr==Wafer_ID_Addr[wafer_ID_No])
					FOR_EACH_SITE(nSiteIndex) Wafer_ID_Data[nSiteIndex][wafer_ID_No]=readback[nSiteIndex];
				wafer_ID_No++;
			}
			
			if(isVMreg_No_Inrush(cmi_mask[i].reg_addr))
			{
				FOR_EACH_SITE(nSiteIndex)
				{
					double_unit_reg[0][nSiteIndex][i]=readback[nSiteIndex];
					/*
					if((cmi_mask[i].reg_addr==0x2040||cmi_mask[i].reg_addr==0x2050)&&(CMI_Rev_No==520100||CMI_Rev_No==522100))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0xff;
					if((cmi_mask[i].reg_addr==0x2046||cmi_mask[i].reg_addr==0x2056)&&(CMI_Rev_No==520100||CMI_Rev_No==522100))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0xe7;
					*/
					if((double_unit_reg[0][nSiteIndex][i]^double_unit_reg[1][nSiteIndex][i])>0)
					{
						Diff_count[nSiteIndex]++;
						/*
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex]);
						printf("data_readback value = %d\n",double_unit_reg[0][nSiteIndex][i]);
						printf("data_readback value = %d\n",double_unit_reg[1][nSiteIndex][i]);
						printf("\n");
						*/
					}
				}
			}
		}
		
		for(i=0;i<7;i++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Reg[Wafer_ID_Addr[i]].Data[nSiteIndex]		= Reg[Wafer_ID_Addr[i]].CMI|Wafer_ID_Data[nSiteIndex][i];
				Reg[Wafer_ID_Addr[i]].Trimmed[nSiteIndex]	= Reg[Wafer_ID_Addr[i]].CMI|Wafer_ID_Data[nSiteIndex][i];
			}
		}
		
		FOR_EACH_SITE(nSiteIndex)
		{
			if(Diff_count[nSiteIndex]>0)
				DU_Check_result[nSiteIndex]=1;
		}
		Relay.BIT[K48_I2C].clr();
		delayms(2);

		Fvi45.CH[VBYP]		.fv(0);
		Fvi45.CH[VIN_3V3]		.fv(0);
		Fvi45.CH[VOUT_SW]		.fv(0);
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, 1, DU_Check_result[nSiteIndex]);
	}

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Flush_Check
STT_TESTFUNC Flush_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Flush_Check_params* ours)
{
	FOR_EACH_SITE(nSiteIndex)
	{	
		Fail_bin_No[nSiteIndex]=6;
	}
	int fail_count_0x55[SITE_MAX]={0},fail_count_0xaa[SITE_MAX]={0},fail_count_cmi[SITE_MAX]={0};
	unsigned char data_readback[SITE_MAX]={0};
	int i2c_slave_addr[SITE_MAX]={0};
	Connect_all_cap();
	if (QC_Flag==0&&Flush_Check_Flag)
	{
		//************default_cmi**********
		Erase_Flash(0,200,true);		
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		Relay.BIT[K48_I2C].set();
		delayms(5);
		Reg[0x2000].read(readback);
		FOR_EACH_SITE(nSiteIndex)
		{
			i2c_slave_addr[nSiteIndex]=0x03&readback[nSiteIndex];
			readback[nSiteIndex]=0xfc&readback[nSiteIndex];
		}
		Reg[0x2000].write(readback);
		
		//i2c_init(HCTL_MODE,300000,2);
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			if(cmi_mask[i].reg_addr==0x10f2) 
			{
				Reg[0x1016].set_clr_inv(BIT7,BIT6);
				delayms(1);
				Fvi45.CH[VBYP].fv(0);
				delayms(1);
			}
			Reg[cmi_mask[i].reg_addr].read(readback);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(cmi_mask[i].reg_addr==0x2000) readback[nSiteIndex]=readback[nSiteIndex]+i2c_slave_addr[nSiteIndex];
			}
		
			if(cmi_mask[i].reg_addr==0x10f7)
			{
				Fvi45.CH[VBYP].fv(VIN_SET+1.5);
				delayms(1);
			}
			if(isVMreg(cmi_mask[i].reg_addr))
			{
				FOR_EACH_SITE(nSiteIndex)
				{
					data_readback[nSiteIndex]=readback[nSiteIndex];
					if(((data_readback[nSiteIndex]&cmi_mask[i].read_bit)-Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex])!=0)
					{
						fail_count_cmi[nSiteIndex]++;
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex]);
						printf("data_readback value = %d\n",data_readback[nSiteIndex]);
						printf("\n");
					}
				}
			}
		}
		delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, fail_count_cmi[nSiteIndex]);
		}
		//=========================flush check 0x55/0xaa============================================//		
		//************0x55**********
		/*
		Erase_Flash(0x55,300);
		delayms(2);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		delayms(2);
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			if (CMI_var.registers[i].vruc_groupid==1)
				Reg[cmi_mask[i].reg_addr].Device_addr = 0xe0;

			if(isVMreg_No_Inrush(cmi_mask[i].reg_addr))
			{
				Reg[cmi_mask[i].reg_addr].read(readback);
					
				FOR_EACH_SITE(nSiteIndex)
				{
					if((readback[nSiteIndex]-flush_addr_0x55_0xaa[i].flush_0x55)!=0)
					{
						fail_count_0x55[nSiteIndex]++;
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",0x55);
						printf("readback = %d\n",readback[nSiteIndex]);
						printf("\n");
					}
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 2, fail_count_0x55[nSiteIndex]);
		}

		//************0xaa**********
		Erase_Flash(0xaa,300);
		delayms(2);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		delayms(2);
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			if (CMI_var.registers[i].vruc_groupid==1)
				Reg[cmi_mask[i].reg_addr].Device_addr = 0xe0;

			if(isVMreg_No_Inrush(cmi_mask[i].reg_addr))
			{
				Reg[cmi_mask[i].reg_addr].read(readback);
					
				FOR_EACH_SITE(nSiteIndex)
				{
					if((readback[nSiteIndex]-flush_addr_0x55_0xaa[i].flush_0xaa)!=0)
					{
						fail_count_0xaa[nSiteIndex]++;
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",0xaa);
						printf("readback = %d\n",readback[nSiteIndex]);
						printf("\n");
					}
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 3, fail_count_0xaa[nSiteIndex]);
		}

		//Reset the User I2C Addr
		for(i=0;i<CMI_var.RegisterSize;i++)
		{
			if (CMI_var.registers[i].vruc_groupid==1)
				Reg[cmi_mask[i].reg_addr].Device_addr = User_Addr;
		}
		*/
		//=========================flush check 0x55/0xaa============================================//		
		Relay.BIT[K48_I2C].clr();
		delayms(2);
		Fvi45.CH[VOUT_SW]   .fv(0);	
		Fvi45.CH[VIN_3V3]   .fv(0);	
		Fvi45.CH[VBYP]      .fv(0);	
		delayms(2);
		Fvi45.CH[VOUT_SW]	.open();
		Fvi45.CH[VIN_3V3]	.open();
		Fvi45.CH[VBYP]      .open();
	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Master
STT_TESTFUNC Trim_Master(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Master_params* ours)
{
	int DEV_ID=0,Vendor_ID=0,MeLayRevID=0,BsLayRevID=0,NVM_RevID=0;
	double VREF0P6_Voltage[SITE_MAX]={0};
	FOR_EACH_SITE(nSiteIndex)
	{
		if(IsSiteActive(nSiteIndex))
			Fail_bin_No[nSiteIndex]=4;
	}

	//FTP Limit value
	double Master_Trim_HighLi[201]={0},Master_Trim_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Master_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Master_Trim_LowLi[i+1]);//item 1-48
	}
    //************Power_Up&Var_Initial**********
	//**Write NON_VOLATILE to registers**
	Enter_Factory(true);
	delayms(1);
	Reg[0x10f2].set_clr_inv(BIT3,BIT2|BIT1|BIT0);

	if(QC_Flag==0)//debugon20201224 0->1
	{
		Write_CMI_ALL(0);
	}
	Disable_Sequence();

    //************Power_Up&Var_Initial**********
    //************Readback device infos**********
    Factory_Mode_Check();
	Reg[0x1008].read(readback);
	if(readback[0]!=0x00) delayms(50);

	Reg[0x1001].read(readback);
    DEV_ID=readback[0]&0x1f;
	Vendor_ID=(readback[0]&0xe0)>>5;
	Reg[0x1002].read(readback);
    MeLayRevID=readback[0]&0x0f;
	BsLayRevID=(readback[0]&0xf0)>>4;
	Reg[0x1003].read(readback);
	NVM_RevID=readback[0];
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 2, DEV_ID);
		pFunction->LogResult(NSITEINDEX, 3, Vendor_ID);
		pFunction->LogResult(NSITEINDEX, 4, MeLayRevID);
		pFunction->LogResult(NSITEINDEX, 5, BsLayRevID);
		pFunction->LogResult(NSITEINDEX, 6, NVM_RevID);
	}
    //************Readback device infos**********
    //************BGTC_Trim**********
	double BGTC_Trim_Target=1.227;//V
	double EXT_PG_V1[SITE_MAX]={0},EXT_PG_V2[SITE_MAX]={0};
	TestNO_target=7;
	Relay.BIT[K26_AMUX_Buffer].set(); // CLOSE AMUX_BUF
	delayms(2);
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);//use measure voltage
	Reg[0x1019].write(0x08);
	Reg[0x101a].write(0x80);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(_0x00);//Config EXT_PG pin

	Reg[0x20fa].write(0x60);
	Reg[0x20f9].write(0x04);
	/*
	for(i=0;i<32;i++)
	{
		Reg[0x20f4].write_Data_trimvalue(7,3,i);
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		Trim_Code_Value[i]=(EXT_PG_V1[0]+EXT_PG_V2[0])/2;
	}
	*/

	if(QC_Flag==0)
	{	
		Reg[0x20f4].write_Data_trimvalue(7,3,0x1c);
	}
	delayms(2);
	////
	Reg[0x2008].write(0x28);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
	Reg[0x2008].write(0x38);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
		global_Pre[nSiteIndex]=global_Pre[nSiteIndex];
		global_post[nSiteIndex]=global_Pre[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f4;//--> set trim address
		global_BitHigh=7,global_BitLow=3,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=32;//--> set table size
		//unsigned char code_queue[32] =  {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
		unsigned char code_queue[32] =  {16,	17,	20,	18,	21,	19,	22,	23,	24,	25,	28,	26,	29,	27,	30,	31,	0,	1,	4,	2,	5,	3,	6,	7,	8,	9,	12,	10,	13,	11,	14,	15};
		int ratio_queue[32] = {-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17,	18,	19,	20,	21};
		print_index=TestNO_target+4;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.005;//mV
			global_Target_array[nSiteIndex]=BGTC_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				Reg[0x2008].write(0x28);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
				Reg[0x2008].write(0x38);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;

				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=BGTC_Trim_Target;

		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
	}
    //************BGTC_Trim**********
    //************VREF1P1_Trim**********
	
	double VREF1P1_Trim_Target=1.1;//V
	TestNO_target=14;
	Reg[0x20f9].write(0x02);
	delayms(2);
	/*
	for(i=0;i<64;i++)
	{
		Reg[0x20f1].write_Data_trimvalue(7,2,i);
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		Trim_Code_Value[i]=(EXT_PG_V1[0]+EXT_PG_V2[0])/2;
	}
    */
	
	if(QC_Flag==0)
	{
		Reg[0x20f1].write_Data_trimvalue(7,2,0x05);
	}
	Reg[0x2008].write(0x28);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
	Reg[0x2008].write(0x38);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
		global_Pre[nSiteIndex]=global_Pre[nSiteIndex];
		global_post[nSiteIndex]=global_Pre[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f1;//--> set trim address
		global_BitHigh=7,global_BitLow=2,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=64;//--> set table size
		unsigned char code_queue[64] =  {32,	33,	34,	35,	36,	37,	38,	39,	40,	41,	42,	43,	44,	45,	46,	47,	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	62,	63,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31};
		int ratio_queue[64] = {-37,	-36,	-35,	-34,	-33,	-32,	-31,	-30,	-29,	-28,	-27,	-26,	-25,	-24,	-23,	-22,	-21,	-20,	-19,	-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26};
		print_index=TestNO_target+4;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.0038;//mV
			global_Target_array[nSiteIndex]=VREF1P1_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				Reg[0x2008].write(0x28);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
				Reg[0x2008].write(0x38);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VREF1P1_Trim_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
	}
    //************VREF0P6**********
	
	Reg[0x20f9].write(0x01);
	delayms(1);
	Reg[0x2008].write(0x28);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
	Reg[0x2008].write(0x38);
	delayms(1);
	Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
	FOR_EACH_SITE(nSiteIndex)
	{
		VREF0P6_Voltage[nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;

	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 21, VREF0P6_Voltage[nSiteIndex]);
	}
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//
	
    //************VREF0P6**********

    //************IPU5U**********
	
	double IPU5U_Trim_Target=5.0;//uA
	TestNO_target=22;
	Relay.BIT[K26_AMUX_Buffer].clr(); // CLOSE AMUX_BUF
	delayms(2);
	Fvi16.CH[EXT_PG].fv(0,SITE_ALL,Stall1V1,Stall50uA);//use measure current
	
	Reg[0x2008].write(0x08);
	Reg[0x20f9].write(0x06);
	Reg[0x1084].set_clr_inv(BITNONE,BIT7);//Add it for 90522-102 E5 cmi
	Reg[0x2008].read(readback);
	Reg[0x20f9].read(readback);
	
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f0].write_Data_trimvalue(7,5,i);

		Fvi16.CH[EXT_PG].mi(global_temp);
        Trim_Code_Value[i]=-1e3*global_temp[0];
		Reg[0x20f0].read(readback);
	}
    */
	
	if(QC_Flag==0)
	{	
		Reg[0x20f0].write_Data_trimvalue(7,5,zero);
	}

	delayms(2);
	Fvi16.CH[EXT_PG].mi(global_temp);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=-1e3*global_temp[nSiteIndex];
		global_post[nSiteIndex]=-1e3*global_temp[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f0;//--> set trim address
		global_BitHigh=7,global_BitLow=5,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
	    print_index=TestNO_target+4;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.17;//uA
			global_Target_array[nSiteIndex]=IPU5U_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		Fvi16.CH[EXT_PG].mi(global_temp);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=-1e3*global_temp[nSiteIndex];
		}
		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				Fvi16.CH[EXT_PG].mi(global_temp);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=-1e3*global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//
	Reg[0x1084].write_CMI();//Add it for 90522-102 E5 cmi

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=IPU5U_Trim_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
	}
	
    //************IPU5U**********
    //************HF_OSC**********
	
	double OSC_Trim_Target_Period=2.25;//us:1/Mhz
	TestNO_target=29;
	Fvi16.CH[EXT_PG].open();
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K18_EXTEN_TMUA].set(); // CLOSE AMUX_BUF

	TMU_CH1.stallset(Stall7V,TMUCOUNT,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	delayms(2);
	
	Reg[0x20f9].write(_0x00);
	Reg[0x2007].write(0x01);
	Reg[0x2008].write(0x04);

	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_EN pin
	
	/*
	for(i=0;i<64;i++)
	{
		Reg[0x2000].write_Data_trimvalue(7,2,i);
		delay(200);
		TMU_CH1.arm(1);
		delayms(2);
		TMU_CH1.read();
        Trim_Code_Value[i]=meas_data[1]/1e3;
	}
    */
	
	if(QC_Flag==0)
	{	
		Reg[0x2000].write_Data_trimvalue(7,2,zero);
	}
	measure_TMUCH1();
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=global_temp[nSiteIndex];
		global_post[nSiteIndex]=global_temp[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x2000;//--> set trim address
		global_BitHigh=7,global_BitLow=2,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=64;//--> set table size
		unsigned char code_queue[64] = {32	,33	,34	,35	,36	,37	,38	,39	,40	,41	,42	,43	,44	,45	,46	,47	,48	,49	,50	,51	,52	,53	,54	,55	,56	,57	,58	,59	,60	,61	,62,	63	,0	,1	,2	,3	,4	,5	,6	,7	,8	,9	,10	,11	,12	,13	,14	,15	,16	,17	,18	,19	,20	,21	,22	,23	,24	,25	,26	,27	,28	,29	,30	,31};
		int ratio_queue[64] = {-32	,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
		print_index=TestNO_target+4;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.035;//Mhz
			global_Target_array[nSiteIndex]=OSC_Trim_Target_Period;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		measure_TMUCH1();
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}	
		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);

	Reg[0x2008].write_CMI();//Dmux/Amux OFF//
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//
	Reg[0x2007].write(0xa0);

	Reg[0x101d].write_CMI();
	Reg[0x101e].write_CMI();
	Reg[0x101f].write_CMI();
	Reg[0x1020].write_CMI();//Configure EXT_EN pin
	Relay.BIT[K18_EXTEN_TMUA].clr(); // CLOSE AMUX_BUF
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=OSC_Trim_Target_Period;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
	}
	
    //************HF_OSC**********	
	//************INRUSH_FSW1**********	
	double INRUSH_FSW1_Acc[SITE_MAX]={0};
	int HBIT_Set=0,Value_Set=0;
    Value_Set=(Reg[0x10f3].CMI>>5)&0x07;
	double INRUSH_FSW1_Trim_Target=200+Value_Set*225;//us:1/Khz
	TestNO_target=36;
	Relay.BIT[K21_GPIO1_TMUA].set(); // CLOSE AMUX_BUF

	TMU_CH1.stallset(Stall7V,TMU4mS,10);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	delayms(2);
	//Reg[0x10f2].write(0x08);//reload issue
	Reg[0x10f3].set_clr_inv(BIT2,BIT3|BIT1|BIT0);
	Reg[0x20f9].write(0x06);
	Reg[0x20fa].write(0xa0);
	Reg[0x2008].write(0x05);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	/*
	for(i=0;i<16;i++)
	{
		Reg[0x20f2].write_Data_trimvalue(7,4,i);
		delay(200);
		TMU_CH1.arm(1);
		delayms(2);
		TMU_CH1.read();
        Trim_Code_Value[i]=meas_data[1];
	}
	delayms(2);
    */
	
	if(QC_Flag==0)
	{	
		Reg[0x20f2].write_Data_trimvalue(7,4,zero);
	}

	measure_TMUCH1();
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=1e3*global_temp[nSiteIndex];
		global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f2;//--> set trim address
		global_BitHigh=7,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=16;//--> set table size
		//unsigned char code_queue[16] = {8,9,10,11,12,13,14,15,0	,1	,2	,3	,4	,5	,6	,7};
		//int ratio_queue[16] = {-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
		unsigned char code_queue[16] = {7,6,5,4,3,2,1,0,15	,14	,13	,12	,11	,10	,9	,8};
		int ratio_queue[16] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8};

		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.11;//uS
			global_Target_array[nSiteIndex]=1e3/INRUSH_FSW1_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		measure_TMUCH1();
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
		}	
		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	FOR_EACH_SITE(nSiteIndex)
	{
		INRUSH_FSW1_Acc[nSiteIndex]=1e2*(1e3/global_post[nSiteIndex]-INRUSH_FSW1_Trim_Target)/INRUSH_FSW1_Trim_Target;//mV
	}

	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=INRUSH_FSW1_Trim_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3/global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3/global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, INRUSH_FSW1_Acc[nSiteIndex]);

	}
    //************INRUSH_FSW1**********	
	//************INRUSH_FSW2**********	
	double INRUSH_FSW2_Acc[SITE_MAX]={0};
    Value_Set=(Reg[0x10f3].CMI>>4)&0x01;
	double INRUSH_FSW2_Trim_Target=50+Value_Set*50;//us:1/Khz
	TestNO_target=44;
	Reg[0x20f9].write(0x01);
	Reg[0x10f3].set_clr_inv(BITNONE,BIT3|BIT2|BIT1|BIT0);

	/*
	for(i=0;i<16;i++)
	{
		Reg[0x20f2].write_Data_trimvalue(3,0,i);
		delay(200);
		TMU_CH1.arm(1);
		delayms(2);
		TMU_CH1.read();
        Trim_Code_Value[i]=meas_data[1];
	}
	delayms(2);
    */
	
	if(QC_Flag==0)
	{	
		Reg[0x20f2].write_Data_trimvalue(3,0,zero);
	}

	delayms(2);
	measure_TMUCH1();
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=1e3*global_temp[nSiteIndex];
		global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f2;//--> set trim address
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=16;//--> set table size
		//unsigned char code_queue[16] = {8,9,10,11,12,13,14,15,0	,1	,2	,3	,4	,5	,6	,7};
		//int ratio_queue[16] = {-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
		unsigned char code_queue[16] = {7,6,5,4,3,2,1,0,15	,14	,13	,12	,11	,10	,9	,8};
		int ratio_queue[16] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8};

		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.8;//uS
			global_Target_array[nSiteIndex]=1e3/INRUSH_FSW2_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		measure_TMUCH1();
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
		}	
		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=1e3*global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	FOR_EACH_SITE(nSiteIndex)
	{
		INRUSH_FSW2_Acc[nSiteIndex]=1e2*(1e3/global_post[nSiteIndex]-INRUSH_FSW2_Trim_Target)/INRUSH_FSW2_Trim_Target;//mV
	}

	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//	
	//Reg[0x10f2].set_clr_inv(BIT3,BIT2|BIT1|BIT0);//reload issue

	Reg[0x10f3].write_CMI();
	Reg[0x20fa].write(0x20);

	Reg[0x2008].write_CMI();//Dmux/Amux OFF//
	Reg[0x20f9].write_CMI();//Dmux/Amux OFF//

	Reg[0x1025].write_CMI();
	Reg[0x1026].write_CMI();
	Reg[0x1027].write_CMI();
	Reg[0x1028].write_CMI();//Configure GPIO1

	Relay.BIT[K21_GPIO1_TMUA].clr(); // CLOSE AMUX_BUF
	Relay.BIT[K47_TMU].clr();

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=INRUSH_FSW2_Trim_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3/global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3/global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, INRUSH_FSW2_Acc[nSiteIndex]);
	}
    //************INRUSH_FSW2**********	
	//************VOUTSW_UVLO_F**********
	clrAllFlag();
	double VOUTSW_UVLO_F_Target=2.7-0.1*((Reg[0x10f5].CMI&0x18)>>3);//V
	double VOUTSW_UVLO_F_Acc[SITE_MAX]={0},VOUTSW_UVLO_R[SITE_MAX]={0},VOUTSW_UVLO_HYS[SITE_MAX]={0};
	TestNO_target=52;
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	Reg[0x2007].write(0xad);
	Reg[0x2008].write(0x05);
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_PG pin
	
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f7].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(0.9*VOUTSW_UVLO_F_Target);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,1.05*VOUTSW_UVLO_F_Target,0.95*VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
    */
	Fvi45.CH[VOUT_SW].fv(1.1*VOUTSW_UVLO_F_Target);
	delayms(2);
	if(QC_Flag==0)
	{	
		Reg[0x20f7].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_UVLO_F_Target,0.95*VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}	
	if(QC_Flag==1)
	{
		Fvi45.CH[VOUT_SW].fv(1.1*VOUTSW_UVLO_F_Target);
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1e-3*Master_Trim_HighLi[56]+VOUTSW_UVLO_F_Target,1e-3*Master_Trim_LowLi[56]+VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,40,40);
		//sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			//1.05*VOUTSW_UVLO_F_Target,1e-3*Master_Trim_LowLi[56]+VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);

		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f7;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.012;//mV
			global_Target_array[nSiteIndex]=VOUTSW_UVLO_F_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_UVLO_F_Target,0.95*VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
					1.05*VOUTSW_UVLO_F_Target,0.95*VOUTSW_UVLO_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_UVLO_F_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VOUTSW_UVLO_F_Target);//mV
	}

	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Rise
	//sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		//0.95*VOUTSW_UVLO_F_Target,1.2*VOUTSW_UVLO_F_Target,0.002,LESS_THAN,1.5,global_temp,20);
	sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		VOUTSW_UVLO_F_Target+1e-3*Master_Trim_LowLi[61],VOUTSW_UVLO_F_Target+1e-3*Master_Trim_HighLi[61],0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,10,10);

	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_UVLO_R[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_UVLO_HYS[nSiteIndex]=1e3*(VOUTSW_UVLO_R[nSiteIndex]-global_post[nSiteIndex]);//mV
	}
	Fvi45.CH[VOUT_SW].fv(VIN_SET);
    delayms(2);	
	Reg[0x2007].write(0xa0);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VOUTSW_UVLO_F_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_UVLO_F_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, VOUTSW_UVLO_R[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, VOUTSW_UVLO_HYS[nSiteIndex]);

	}
	//************VOUTSW_UVLO_F**********
	//************VOUTSW_BR_F**********
    HBIT_Set=(Reg[0x10f5].CMI>>7)&0x01;
    Value_Set=(Reg[0x10f5].CMI>>5)&0x03;
	double VOUTSW_BR_F_Target=3.1-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
	double VOUTSW_BR_F_Acc[SITE_MAX]={0},VOUTSW_BR_R[SITE_MAX]={0},VOUTSW_BR_HYS[SITE_MAX]={0};
	double VOUTSW_BR_HYS_Set=0,VOUTSW_BR_HYS_Acc[SITE_MAX]={0};
	VOUTSW_BR_HYS_Set=100+50*((Reg[0x10f6].CMI>>2)&0x03);
	TestNO_target=62;
	Reg[0x2007].write(0xaf);
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f6].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(0.9*VOUTSW_BR_F_Target);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,1.05*VOUTSW_BR_F_Target,0.95*VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
    */
	if(QC_Flag==0)
	{	
		Reg[0x20f6].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_BR_F_Target,0.95*VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}
	
	if(QC_Flag==1)
	{
		Fvi45.CH[VOUT_SW].fv(1.05*VOUTSW_BR_F_Target);
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1e-3*Master_Trim_HighLi[66]+VOUTSW_BR_F_Target,1e-3*Master_Trim_LowLi[66]+VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,40,40);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f6;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.012;//mV
			global_Target_array[nSiteIndex]=VOUTSW_BR_F_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_BR_F_Target,0.95*VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
					1.05*VOUTSW_BR_F_Target,0.95*VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_BR_F_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VOUTSW_BR_F_Target);//mV
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Rise
	Reg[0x2007].write(0x8f);//change on 20201216 from 0xaf to 0x8f
    delayms(1);

	Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target);
    delayms(1);
	sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		VOUTSW_BR_F_Target+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[73]),VOUTSW_BR_F_Target+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_HighLi[73]),0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,40,40);
	//sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		//VOUTSW_BR_F_Target,VOUTSW_BR_F_Target+0.2,0.002,LESS_THAN,1.5,global_temp,20,0);

	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_BR_R[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_BR_HYS[nSiteIndex]=1e3*(VOUTSW_BR_R[nSiteIndex]-global_post[nSiteIndex]);//mV
		VOUTSW_BR_HYS_Acc[nSiteIndex]=VOUTSW_BR_HYS[nSiteIndex]-VOUTSW_BR_HYS_Set;
	}

	Fvi45.CH[VOUT_SW].fv(VIN_SET);
	Reg[0x2007].write(0xa0);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VOUTSW_BR_F_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_BR_F_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, VOUTSW_BR_HYS_Set);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, VOUTSW_BR_R[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, VOUTSW_BR_HYS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, VOUTSW_BR_HYS_Acc[nSiteIndex]);

	}
	//************VOUTSW_BR_F**********
	//************VOUTSW_POK_Warn_F**********
    HBIT_Set=(Reg[0x10f5].CMI>>2)&0x01;
    Value_Set=Reg[0x10f5].CMI&0x03;

	double VOUTSW_POK_Warn_F_Target=3.15-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
	double VOUTSW_POK_Warn_F_Acc[SITE_MAX]={0},VOUTSW_POK_Warn_R[SITE_MAX]={0},VOUTSW_POK_Warn_HYS[SITE_MAX]={0};
	double VOUTSW_POK_Warn_HYS_Set=0,VOUTSW_POK_Warn_HYS_Acc[SITE_MAX]={0};
	VOUTSW_POK_Warn_HYS_Set=100+50*(Reg[0x10f6].CMI&0x03);

	TestNO_target=74;
	Reg[0x2007].write(0xae);
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f5].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(0.9*VOUTSW_POK_Warn_F_Target);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,1.05*VOUTSW_POK_Warn_F_Target,0.95*VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
    */
	if(QC_Flag==0)
	{	
		Reg[0x20f5].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_POK_Warn_F_Target,0.95*VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}
	if(QC_Flag==1)
	{
		Fvi45.CH[VOUT_SW].fv(1.05*VOUTSW_POK_Warn_F_Target);
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1e-3*Master_Trim_HighLi[78]+VOUTSW_POK_Warn_F_Target,1e-3*Master_Trim_LowLi[78]+VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,30,30);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f5;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.012;//mV
			global_Target_array[nSiteIndex]=VOUTSW_POK_Warn_F_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,1.05*VOUTSW_POK_Warn_F_Target,0.95*VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
					1.05*VOUTSW_POK_Warn_F_Target,0.95*VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_POK_Warn_F_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VOUTSW_POK_Warn_F_Target);//mV
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Rise
	//sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		//0.9*VOUTSW_POK_Warn_F_Target,1.2*VOUTSW_POK_Warn_F_Target,0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,40,10);
	sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		VOUTSW_POK_Warn_F_Target+1e-3*(VOUTSW_POK_Warn_HYS_Set+Master_Trim_LowLi[85]),VOUTSW_POK_Warn_F_Target+1e-3*(VOUTSW_POK_Warn_HYS_Set+Master_Trim_HighLi[85]),0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,40,20);

	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_POK_Warn_R[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_POK_Warn_HYS[nSiteIndex]=1e3*(VOUTSW_POK_Warn_R[nSiteIndex]-global_post[nSiteIndex]);//mV
		VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]=VOUTSW_POK_Warn_HYS[nSiteIndex]-VOUTSW_POK_Warn_HYS_Set;//mV
	}

	Fvi16.CH[EXT_EN].open();

	Fvi45.CH[VOUT_SW].fv(VIN_SET);
    delayms(2);	
	Reg[0x2007].write(0xa0);
	
	Reg[0x2008].write_CMI();
	Reg[0x101d].write_CMI();
	Reg[0x101e].write_CMI();
	Reg[0x101f].write_CMI();
	Reg[0x1020].write_CMI();//Disable Configure EXT_PG pin
	

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VOUTSW_POK_Warn_F_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_POK_Warn_F_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, VOUTSW_POK_Warn_HYS_Set);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, VOUTSW_POK_Warn_R[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, VOUTSW_POK_Warn_HYS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]);
	}
	//************VOUTSW_POK_Warn_F**********
	//************SW_ON_R**********
	double SW_ON_Rise_Target=2.95-0.1*((Reg[0x10f2].CMI&0xc0)>>6);//V
	double SW_ON_Rise_Acc[SITE_MAX]={0},SW_ON_F[SITE_MAX]={0},SW_ON_HYS[SITE_MAX]={0};

	TestNO_target=86;
	Fvi16.CH[GPIO1].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	Reg[0x20f9].write(0x02);
	Reg[0x20fa].write(0xa0);
	Reg[0x2008].write(0x04);

	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1

	Reg[0x10f7].write(0x04);// enable DMUX
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f8].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VIN_3V3].fv(0.9*SW_ON_Rise_Target);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,0.95*SW_ON_Rise_Target,1.1*SW_ON_Rise_Target,0.002,LESS_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
    */
	if(QC_Flag==0)
	{	
		Reg[0x20f8].write_Data_trimvalue(2,0,zero);
		Fvi45.CH[VIN_3V3].fv(0.9*SW_ON_Rise_Target);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.95*SW_ON_Rise_Target,1.1*SW_ON_Rise_Target,0.002,LESS_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}

	if(QC_Flag==1)
	{
		Fvi45.CH[VIN_3V3].fv(0.9*SW_ON_Rise_Target);
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-3*Master_Trim_LowLi[90]+SW_ON_Rise_Target,1e-3*Master_Trim_HighLi[90]+SW_ON_Rise_Target,0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,40,40);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f8;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.012;//mV
			global_Target_array[nSiteIndex]=SW_ON_Rise_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.95*SW_ON_Rise_Target,1.1*SW_ON_Rise_Target,0.002,LESS_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					0.95*SW_ON_Rise_Target,1.1*SW_ON_Rise_Target,0.002,LESS_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		SW_ON_Rise_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-SW_ON_Rise_Target);//mV
	}

	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Fall
	sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
		SW_ON_Rise_Target-1e-3*Master_Trim_LowLi[95],SW_ON_Rise_Target-1e-3*Master_Trim_HighLi[95],-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,10,10);
	FOR_EACH_SITE(nSiteIndex)
	{
		SW_ON_F[nSiteIndex]=global_temp[nSiteIndex];
		SW_ON_HYS[nSiteIndex]=1e3*(global_post[nSiteIndex]-SW_ON_F[nSiteIndex]);//mV
	}

	Reg[0x10f7].write_CMI();
	Reg[0x1008].write_CMI();// Disable DMUX
	Fvi45.CH[VIN_3V3].fv(VIN_SET);

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=SW_ON_Rise_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, SW_ON_Rise_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, SW_ON_F[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, SW_ON_HYS[nSiteIndex]);

	}
	//************SW_ON_R**********
	//************VIN3P3_OV_Warn_R**********

    Value_Set=(Reg[0x10f2].CMI&0x30)>>4;

	double VIN3P3_OV_Warn_R_Target=0;
	double VIN3P3_OV_Warn_R_Acc[SITE_MAX]={0},VIN3P3_OV_Warn_F[SITE_MAX]={0},VIN3P3_OV_Warn_HYS[SITE_MAX]={0};
	if(Value_Set==0) VIN3P3_OV_Warn_R_Target=3.63;
	else if(Value_Set==1) VIN3P3_OV_Warn_R_Target=3.70;
	else if(Value_Set==2) VIN3P3_OV_Warn_R_Target=3.76;
	else if(Value_Set==3) VIN3P3_OV_Warn_R_Target=3.82;

	TestNO_target=96;
	Reg[0x1008].write(0x05);// Disable DMUX
	//Reg[0x10f2].set_clr_inv(BIT3,BIT2|BIT1|BIT0);
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f4].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(0.9*VIN3P3_OV_Warm_R_Target);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.95*VIN3P3_OV_Warm_R_Target,1.05*VIN3P3_OV_Warm_R_Target,0.002,GREATER_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
    */
	if(QC_Flag==0)
	{	
		Reg[0x20f4].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.95*VIN3P3_OV_Warn_R_Target,1.05*VIN3P3_OV_Warn_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}
	//Fvi45.CH[VIN_3V3].fv(3);
	//Fvi45.CH[VIN_3V3].fv(4);

	if(QC_Flag==1)
	{
		
		Fvi45.CH[VIN_3V3].fv(0.90*VIN3P3_OV_Warn_R_Target);
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-3*Master_Trim_LowLi[100]+VIN3P3_OV_Warn_R_Target,1e-3*Master_Trim_HighLi[100]+VIN3P3_OV_Warn_R_Target,0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,40,20);
		
		//Doesn't work with -30 +30 limit, works with -10 +10 limit.	
		/*
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.92*VIN3P3_OV_Warm_R_Target,1e-3*Master_Trim_HighLi[100]+VIN3P3_OV_Warm_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
		*/
		FOR_EACH_SITE(nSiteIndex)
		{
			global_Pre[nSiteIndex]=global_temp[nSiteIndex];
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}
	}	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f4;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.015;//mV
			global_Target_array[nSiteIndex]=VIN3P3_OV_Warn_R_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,0.95*VIN3P3_OV_Warn_R_Target,1.05*VIN3P3_OV_Warn_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					0.95*VIN3P3_OV_Warn_R_Target,1.05*VIN3P3_OV_Warn_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VIN3P3_OV_Warn_R_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VIN3P3_OV_Warn_R_Target);//mV
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Fall
	Fvi45.CH[VIN_3V3].fv(1.05*VIN3P3_OV_Warn_R_Target);
	delayms(2);	
	sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
		VIN3P3_OV_Warn_R_Target-1e-3*Master_Trim_LowLi[105],VIN3P3_OV_Warn_R_Target-1e-3*Master_Trim_HighLi[105],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,20,20);
	FOR_EACH_SITE(nSiteIndex)
	{
		VIN3P3_OV_Warn_F[nSiteIndex]=global_temp[nSiteIndex];
		VIN3P3_OV_Warn_HYS[nSiteIndex]=1e3*(global_post[nSiteIndex]-VIN3P3_OV_Warn_F[nSiteIndex]);//mV
	}
    
	Fvi45.CH[VIN_3V3].fv(VIN_SET);
	//Reg[0x20f9].write(_0x00);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VIN3P3_OV_Warn_R_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VIN3P3_OV_Warn_R_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, VIN3P3_OV_Warn_F[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, VIN3P3_OV_Warn_HYS[nSiteIndex]);
	}
	
	//************VIN3P3_OV_Warn_R**********
	//************VIN3P3_OV_R**********
	double VIN3P3_OV_R_Target=3.8;
	double VIN3P3_OV_R_Acc[SITE_MAX]={0},VIN3P3_OV_F[SITE_MAX]={0},VIN3P3_OV_HYS[SITE_MAX]={0};
	TestNO_target=106;
	Reg[0x20fa].write(0xa0);
	Reg[0x20f9].write(_0x00);
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f3].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(0.9*VIN3P3_OV_R_Target);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,0.9*VIN3P3_OV_R_Target,1.1*VIN3P3_OV_R_Target,0.002,GREATER_THAN,1.5,global_temp,100);
        Trim_Code_Value[i]=global_temp[0];
	}
	*/
	if(QC_Flag==0)
	{	
		Reg[0x20f3].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,0.95*VIN3P3_OV_R_Target,1.05*VIN3P3_OV_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
	}	
	if(QC_Flag==1)
	{
		Fvi45.CH[VIN_3V3].fv(0.95*VIN3P3_OV_R_Target);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-3*Master_Trim_LowLi[110]+VIN3P3_OV_R_Target,1e-3*Master_Trim_HighLi[110]+VIN3P3_OV_R_Target,0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,20);
	}	
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=global_temp[nSiteIndex];
		global_post[nSiteIndex]=global_temp[nSiteIndex];
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f3;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.015;//mV
			global_Target_array[nSiteIndex]=VIN3P3_OV_R_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.95*VIN3P3_OV_R_Target,1.03*VIN3P3_OV_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					0.95*VIN3P3_OV_R_Target,1.05*VIN3P3_OV_R_Target,0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VIN3P3_OV_R_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VIN3P3_OV_R_Target);//mV
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Fall
	Fvi45.CH[VIN_3V3].fv(1.0*VIN3P3_OV_R_Target);
	delayms(1);	
	sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
		VIN3P3_OV_R_Target-1e-3*Master_Trim_LowLi[115],VIN3P3_OV_R_Target-1e-3*Master_Trim_HighLi[115],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,10,10);
	FOR_EACH_SITE(nSiteIndex)
	{
		VIN3P3_OV_F[nSiteIndex]=global_temp[nSiteIndex];
		VIN3P3_OV_HYS[nSiteIndex]=1e3*(global_post[nSiteIndex]-VIN3P3_OV_F[nSiteIndex]);//mV
	}

	Fvi16.CH[GPIO1].open();
	Fvi45.CH[VIN_3V3].fv(VIN_SET);
	Reg[0x20f9].write_CMI();
	Reg[0x1025].write_CMI();
	Reg[0x2008].write_CMI();
	Reg[0x10f7].write_CMI();

	Reg[0x20fa].write(0x20);
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VIN3P3_OV_R_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VIN3P3_OV_R_Acc[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, VIN3P3_OV_F[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, VIN3P3_OV_HYS[nSiteIndex]);
	}
	//************VIN3P3_OV_R**********
	//************VOUTSW_BR_2nd**********
	double VOUTSW_BR_2nd_Target=3.2-0.25*((0x80&Reg[0x10f5].CMI)>>7)-
		                        0.05*(1+((0x80&Reg[0x10f5].CMI)>>7))*
								((0x60&Reg[0x10f5].CMI)>>5)
								+0.05*((0x0c&Reg[0x10f6].CMI)>>2);
	double VOUTSW_BR_2nd_Acc[SITE_MAX]={0},VOUTSW_BR_2nd_R[SITE_MAX]={0},VOUTSW_BR_2nd_HYS[SITE_MAX]={0};
	TestNO_target=116;
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x10f5].set_clr_inv(BITNONE,BIT4|BIT3|BIT2|BIT1|BIT0);
	Reg[0x1011].write(0x40);
	Reg[0x2007].write(0xa5);
	Reg[0x2008].write(0x05);

	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_EN pin

	/*
	for(j=0;j<4;j++)
	{
		Reg[0x10f6].write_Data_trimvalue(3,2,j);
		for(i=0;i<8;i++)
		{
			Reg[0x10f5].write_Data_trimvalue(7,5,i);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(1.05*VOUTSW_BR_2nd_Target);
			delayms(1);
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				3.4,2.5,-0.005,GREATER_THAN,1.5,global_temp,20);
			Trim_Code_Value[i]=global_temp[0];
		}
		delay(200);

	}
	*/
	/*
	for(i=0;i<8;i++)
	{
		Reg[0x20f0].write_Data_trimvalue(2,0,i);
		delay(200);
		Fvi45.CH[VOUT_SW].fv(1.05*VOUTSW_BR_2nd_Target);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.1*VOUTSW_BR_2nd_Target,0.9*VOUTSW_BR_2nd_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
        Trim_Code_Value[i]=global_temp[0];
	}
	*/
	if(QC_Flag==0)
	{	
		Reg[0x20f0].write_Data_trimvalue(2,0,zero);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_BR_2nd_Target,0.95*VOUTSW_BR_2nd_Target,-0.002,GREATER_THAN,1.5,global_temp,20);

	}	
	if(QC_Flag==1)
	{
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_BR_2nd_Target,0.95*VOUTSW_BR_2nd_Target,-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	}	
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=global_temp[nSiteIndex];
		global_post[nSiteIndex]=global_temp[nSiteIndex];
	}
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x20f0;//--> set trim address
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=8;//--> set table size
		unsigned char code_queue[8] =  {4,5,6,7,0,1,2,3,};
		int ratio_queue[8] = {-4,-3,-2,-1,0,1,2,3};
		print_index=TestNO_target+5;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.01;//mV
			global_Target_array[nSiteIndex]=VOUTSW_BR_2nd_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			1.05*VOUTSW_BR_2nd_Target,0.95*VOUTSW_BR_2nd_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}

		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
					1.05*VOUTSW_BR_2nd_Target,0.95*VOUTSW_BR_2nd_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
						global_post[nSiteIndex]=global_temp[nSiteIndex];
				}
			}
			else
				break;
		}			
		
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	} 
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_BR_2nd_Acc[nSiteIndex]=1e3*(global_post[nSiteIndex]-VOUTSW_BR_2nd_Target);//mV
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x101d].write(_0x00);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(_0x00);//Config EXT_EN pin
	
	Reg[0x10f5].write_CMI();
	Reg[0x1011].write_CMI();
	Reg[0x2007].write(0xa0);
	Reg[0x2008].write(_0x00);
	
	Fvi45.CH[VOUT_SW].fv(VIN_SET);
	Fvi16.CH[EXT_EN].open();
	
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=VOUTSW_BR_2nd_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_BR_2nd_Acc[nSiteIndex]);
	}

	//************VOUTSW_BR_2nd_F**********
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_LDOs_Vout
STT_TESTFUNC Trim_LDOs_Vout(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_LDOs_Vout_params* ours)
{

	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	double LDO_Vout_Final_Acc[LDO_MAX][SITE_MAX]={0};
	double LDO_LP_Vout_Final_Acc[LDO_MAX][SITE_MAX]={0};

	TestNO_target=2;
	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();

	Fvi16.CH[VIN_LDO1].fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);
	Fvi16.CH[VIN_LDO2].fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);

	Fqvi.CH[OUT_LDO1].fi(-2,SITE_ALL,SET_CLAMP,4,-4,4,-4);
	Fqvi.CH[OUT_LDO2].fi(-2,SITE_ALL,SET_CLAMP,4,-4,4,-4);
	delayms(2);
	Reg[0x1093].set_clr_inv(BIT7|BIT6|BIT3|BIT2,BIT4|BIT0);//Normal mode
	Reg[0x1094].set_clr_inv(BITNONE,BIT6|BIT2);
	//Reg[0x10f2].set_clr_inv(BIT3);//reload issue
	if(LDO_Vset[0]>1.1)
		Reg[0x1092].set_clr_inv(BIT1);
	if(LDO_Vset[1]>1.1)
		Reg[0x1092].set_clr_inv(BIT0);
	if(LDO_Vset[0]>1.1&&LDO_Vset[1]>1.1)
		Reg[0x1092].set_clr_inv(BIT1|BIT0);
	/*
	for(i=0;i<32;i++)
	{
		Reg[0x20f5].write_Data_trimvalue(7,3,i);
		Fqvi.CH[GP_OUTLDO[0]].mv(Multi_global_Pre[0]);
        Trim_Code_Value[i]=Multi_global_Pre[0][0];
	}
	*/
	/*
	for(i=0;i<32;i++)
	{
		Reg[0x20f7].write_Data_trimvalue(7,3,i);
		Fqvi.CH[GP_OUTLDO[0]].mv(Multi_global_Pre[1]);
        Trim_Code_Value[i]=Multi_global_Pre[1][0];
	}
	*/

	if(QC_Flag==0)
	{
		Reg[0x20f5].write_Data_trimvalue(7,3,zero);
		Reg[0x20f7].write_Data_trimvalue(7,3,zero);
	}
	/*
	int Data_readback[219]={0}; 
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if(cmi_mask[i].reg_addr==0x10f2) 
		{
			Reg[0x1016].set_clr_inv(BIT7,BIT6);
			delayms(1);
			Fvi45.CH[VBYP].fv(0);
			delayms(1);
		}
		Reg[cmi_mask[i].reg_addr].read(readback);	
		Data_readback[i]=readback[0];

		if(cmi_mask[i].reg_addr==0x10f7)
		{
			Fvi45.CH[VBYP].fv(VIN_SET+1.5);
			delayms(1);
		}
	}
	*/
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(Multi_global_Pre[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];
		}
	}
    //************LDO1/2_Vout**********
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_BitHigh=7,global_BitLow=3,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		int Multi_global_Trim_table_size[LDO_MAX]={32,32};//--> set table size
		unsigned char code_queue_array[LDO_MAX][32]={   { 16,  17, 18, 19, 20, 21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{ 16,  17, 18, 19, 20, 21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
		int ratio_queue_array[LDO_MAX][32] = {  {-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
												{-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
		int Print_Num=8; 
		print_index=TestNO_target+Print_Num-3;
		
		//=========search trim code for each site==========//		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_LDO_Vout_Trim_Addr[nTILE];//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=1.66*LDO_Vset[nTILE]/0.6/1e3;//V
				Multi_global_Target[nTILE][nSiteIndex]=LDO_Vset[nTILE];
			}
		}
		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		//========Measure value=========//
		delayms(2);
		FOR_EACH_TILE(nTILE,LDO_MAX)
			Fqvi.CH[GP_OUTLDO[nTILE]].mv(Multi_global_post[nTILE]);//use measure voltage
		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,LDO_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,LDO_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					//========Measure postvalue=========//
					Fqvi.CH[GP_OUTLDO[nTILE]].mv(global_temp);//use measure voltage
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
					}					
				}
			}
			if(isAllFlagClr_MultiTile(LDO_MAX, Multi_global_retrim_Flag))
				break;
		}
		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
		

	} 
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_Vout_Final_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-LDO_Vset[nTILE])/LDO_Vset[nTILE];
		}
	}

	//power off						
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		int Print_Num=8; 
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=LDO_Vset[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, LDO_Vout_Final_Acc[nTILE][nSiteIndex]);
				
		}
	}
	//************LDO1/2_Vout**********

    //************LDO1/2_Vout_LP**********
	TestNO_target=18;
	if(CMI_Rev_No==520102||CMI_Rev_No==522102||CMI_Rev_No==522104||CMI_Rev_No==521101||CMI_Rev_No==523103)	delayms(100);

	Reg[0x1093].set_clr_inv(BIT7|BIT6|BIT3|BIT2|BIT4|BIT0);
	Reg[0x1012].write(0xf9);
	Reg[0x100a].write(0x06);
	Reg[0x2007].write(0x80);//Add for 90522-102.E5 CMI
	Reg[0x1093].write(0xbb);
	Reg[0x1094].write(_0x00);
	Reg[0x1006].write(0x05);

	/*
	if(1)
	{
		for(int i=0;i<CMI_var.RegisterSize;i++)
			Reg[CMI_Full_address[i]].read();
			Record_allstatus();
		Compare_allstatus("plp_buck",CMI_var.RegisterSize,CMI_Full_address);
	}
    */
	/*
	
	for(i=0;i<32;i++)
	{
		Reg[0x20f6].write_Data_trimvalue(7,3,i);
		Fqvi.CH[GP_OUTLDO[0]].mv(Multi_global_Pre[0]);
        Trim_Code_Value[i]=Multi_global_Pre[0][0];
	}
	*/
	/*
	for(i=0;i<32;i++)
	{
		Reg[0x20f8].write_Data_trimvalue(7,3,i);
		Fqvi.CH[GP_OUTLDO[1]].mv(Multi_global_Pre[1]);
        Trim_Code_Value[i]=Multi_global_Pre[1][0];
	}
	*/
	if(QC_Flag==0)
	{
		Reg[0x20f6].write_Data_trimvalue(7,3,zero);
		Reg[0x20f8].write_Data_trimvalue(7,3,zero);
	}

	delayms(2);
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(Multi_global_Pre[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];
		}
	}
    //************LDO1/2_Vout**********
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_BitHigh=7,global_BitLow=3,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		int Multi_global_Trim_table_size[LDO_MAX]={32,32};//--> set table size
		unsigned char code_queue_array[LDO_MAX][32]={   { 16,  17, 18, 19, 20, 21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{ 16,  17, 18, 19, 20, 21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
		int ratio_queue_array[LDO_MAX][32] = {  {-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
												{-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
		int Print_Num=8; 
		print_index=TestNO_target+Print_Num-3;
		
		//=========search trim code for each site==========//		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_LDO_LP_Vout_Trim_Addr[nTILE];//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=1.66*LDO_Vset[nTILE]/0.6/1e3;//V
				Multi_global_Target[nTILE][nSiteIndex]=LDO_Vset[nTILE];
			}
		}
		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		//========Measure value=========//
		delayms(2);
		FOR_EACH_TILE(nTILE,LDO_MAX)
			Fqvi.CH[GP_OUTLDO[nTILE]].mv(Multi_global_post[nTILE]);//use measure voltage

		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,LDO_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,LDO_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					//========Measure postvalue=========//
					Fqvi.CH[GP_OUTLDO[nTILE]].mv(global_temp);//use measure voltage
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
					}
					
				}
			}
			if(isAllFlagClr_MultiTile(LDO_MAX, Multi_global_retrim_Flag))
				break;
		}
		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_LP_Vout_Final_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-LDO_Vset[nTILE])/LDO_Vset[nTILE];
		}
	}	
	//power off		
	Fqvi.CH[OUT_LDO1].fi(0,SITE_ALL,SET_CLAMP,4,-4,1200,-1200);
	Fqvi.CH[OUT_LDO2].fi(0,SITE_ALL,SET_CLAMP,4,-4,1200,-1200);

	Reg[0x1012].write_CMI();
	Reg[0x100a].write_CMI();
	Reg[0x1093].write_CMI();
	Reg[0x1094].set_clr_inv(BITNONE,BIT6|BIT2);
	Reg[0x1006].write_CMI();
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		int Print_Num=8; 
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=LDO_Vset[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, LDO_LP_Vout_Final_Acc[nTILE][nSiteIndex]);
				
		}
	}
	//************LDO1/2_Vout_LP**********
	return TRUE;
}// END_STT_TESTFUNC

//Write LDO ILIM, POK, OV here

// Hàm đo, trim LDO Current Limit (ILIM)
void Trim_LDO_ILIM(int ldo_idx, double target_ilim) {
    // 1. Kích hoạt LDO
    Enable_LDO(ldo_idx, true);

    // 2. Thử từng mã trim, đo dòng đến khi đạt target
    for (int trim_code = TRIM_ILIM_MIN; trim_code <= TRIM_ILIM_MAX; ++trim_code) {
        Set_LDO_ILIM_Trim(ldo_idx, trim_code); // Gửi mã trim vào LDO
        double measured_ilim = Measure_LDO_ILIM(ldo_idx); // Đo dòng giới hạn

        // 3. So sánh với target từ test matrix
        if (measured_ilim >= target_ilim * 0.98 && measured_ilim <= target_ilim * 1.02) {
            LogResult("LDO_ILIM", ldo_idx, measured_ilim, trim_code);
            break;
        }
    }
    Enable_LDO(ldo_idx, false);
}

// Hàm kiểm tra ngưỡng Power-OK (POK) của LDO
void Trim_LDO_POK(int ldo_idx, double pok_min, double pok_max) {
    Enable_LDO(ldo_idx, true);

    double vout = LDO_VMIN;
    double pok_threshold = 0.0;
    while (vout <= LDO_VMAX) {
        Set_LDO_Output(ldo_idx, vout);
        bool pok_state = Read_LDO_POK(ldo_idx);
        if (pok_state) {
            pok_threshold = vout;
            break;
        }
        vout += 0.005;
    }

    if (pok_threshold >= pok_min && pok_threshold <= pok_max) {
        LogResult("LDO_POK", ldo_idx, pok_threshold, "PASS");
    } else {
        LogResult("LDO_POK", ldo_idx, pok_threshold, "FAIL");
    }
    Enable_LDO(ldo_idx, false);
}

// Hàm kiểm tra, trim ngưỡng Over Voltage (OV) của LDO
void Trim_LDO_OV(int ldo_idx, double ov_min, double ov_max) {
    Enable_LDO(ldo_idx, true);

    double vout = LDO_VNOMINAL;
    double ov_threshold = 0.0;
    while (vout <= LDO_OV_MAX) {
        Set_LDO_Output(ldo_idx, vout);
        bool ov_triggered = Check_LDO_OV(ldo_idx);
        if (ov_triggered) {
            ov_threshold = vout;
            break;
        }
        vout += 0.005;
    }

    if (ov_threshold >= ov_min && ov_threshold <= ov_max) {
        LogResult("LDO_OV", ldo_idx, ov_threshold, "PASS");
    } else {
        LogResult("LDO_OV", ldo_idx, ov_threshold, "FAIL");
    }
    Enable_LDO(ldo_idx, false);
}

// Test Function: Trim_LDO_OSC
STT_TESTFUNC Trim_LDO_OSC(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_LDO_OSC_params* ours)
{
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	
	double LDO_OSC_Trim_Target=1.3;//us:1/Mhz
	double LDO_OSC_Acc[SITE_MAX]={0};
	TestNO_target=2;
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K21_GPIO1_TMUA].set(); // CLOSE AMUX_BUF
	TMU_CH1.stallset(Stall7V,TMUCOUNT,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	delayms(2);
	Reg[0x2095].write(0x07);
	/*
	for(i=0;i<4;i++)
	{
		Reg[0x2091].write_Data_trimvalue(1,0,i);
		delay(200);
		TMU_CH1.arm(1);
		delayms(2);
		TMU_CH1.read();
        Trim_Code_Value[i]=meas_data[1]/1e3;
	}
    */
	if(QC_Flag==0)
	{	
		Reg[0x2091].write_Data_trimvalue(1,0,zero);
	}

	measure_TMUCH1();
	FOR_EACH_SITE(nSiteIndex)
	{
		global_Pre[nSiteIndex]=global_temp[nSiteIndex];
		global_post[nSiteIndex]=global_temp[nSiteIndex];
	}
	delayms(1);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and address==========//
		global_trimReg_addr=0x2091;//--> set trim address
		global_BitHigh=1,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		//global_BitHigh_2nd=4,global_BitLow_2nd=0,global_ShiftBITcount_2nd=0; // copy value if have 2nd trim address
		global_Trim_table_size=4;//--> set table size
		unsigned char code_queue[4] = {1,0,2,3};
		int ratio_queue[4] = {-1,0,1,2};
		print_index=TestNO_target+4;
		
		//=========search trim code for each site==========//
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.86;//Mhz
			global_Target_array[nSiteIndex]=LDO_OSC_Trim_Target;
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, 
				global_Trim_table_size,code_queue, ratio_queue, 
				global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
		delayms(2);
		measure_TMUCH1();
		FOR_EACH_SITE(nSiteIndex)
		{
			global_post[nSiteIndex]=global_temp[nSiteIndex];
		}	
		//===========================================================retrim============================================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				store_Post_value[global_retrim_loop][nSiteIndex]=global_post[nSiteIndex];
				store_Global_trimcode[global_retrim_loop][nSiteIndex]=Global_trimcode[nSiteIndex];
				if (shift_one_trim_code(global_Target_array[nSiteIndex], global_post[nSiteIndex], &global_table_queue_array[nSiteIndex], global_LSB,global_Trim_table_size, code_queue,
					&Global_trimcode[nSiteIndex], &global_count_array[nSiteIndex], &global_trim_tend_array[nSiteIndex], &global_retrim_Flag[nSiteIndex] ,
					global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF, 0.5))
				{
					global_retrim_Flag[nSiteIndex]=true;
					//--write 2nd trim address if has
					//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				}
			}
			if(isOneFlagTrue(global_retrim_Flag))
			{
				//===========================measure post value===================================//
				delayms(2);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
					{
						if (global_retrim_Flag[nSiteIndex]==1)
							global_post[nSiteIndex]=global_temp[nSiteIndex];
					}
				}
			}
			else
				break;
		}			
		//===========================search the best trim code===================================//
		FOR_EACH_SITE(nSiteIndex)
		{	
			for (i=0;i<global_count_array[nSiteIndex]+1;i++)
			{
				store_Post_value_array[i]=store_Post_value[i][nSiteIndex];
				store_Global_trimcode_array[i]=store_Global_trimcode[i][nSiteIndex];
			}
			if (search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex], 
				global_BitHigh, global_BitLow, global_trimReg_addr ))
			{		
				//--write 2nd trim address if has
				//Reg[global_trimReg_addr_2nd].write_Data_trimvalue_singlesite(global_BitHigh_2nd,global_BitLow_2nd,Global_trimcode[nSiteIndex],global_ShiftBITcount_2nd);
				
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		} 
	}  
	FOR_EACH_SITE(nSiteIndex)
	{
		LDO_OSC_Acc[nSiteIndex]=1e2*(global_post[nSiteIndex]-LDO_OSC_Trim_Target)/LDO_OSC_Trim_Target;
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_end++++++++++++++++++++++++++++++++++++++++++++++++++++//
	Reg[0x2095].write(_0x00);

	Reg[0x2008].write_CMI();
	Reg[0x1025].write_CMI();//LDO1 GPIO1
	Reg[0x1031].write_CMI();//LDO2 LPM

	Relay.BIT[K21_GPIO1_TMUA].clr(); // CLOSE AMUX_BUF
	Relay.BIT[K47_TMU].clr();

	FOR_EACH_SITE(nSiteIndex)
	{
		global_Target_array[nSiteIndex]=LDO_OSC_Trim_Target;
		pFunction->LogResult(NSITEINDEX, TestNO_target, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+1, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2, Global_trimcode[nSiteIndex]);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LDO_OSC_Acc[nSiteIndex]);
	}
    //************HF_OSC**********
	Fqvi.CH[OUT_LDO1].fi(0);
	Fqvi.CH[OUT_LDO2].fi(0);
	Fqvi.CH[OUT_LDO1].open();
	Fqvi.CH[OUT_LDO2].open();
	
	Fvi16.CH[VIN_LDO1].fv(0);
	Fvi16.CH[VIN_LDO2].fv(0);
	Fvi16.CH[VIN_LDO1].open();
	Fvi16.CH[VIN_LDO2].open();	

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Bucks_Vout
STT_TESTFUNC Trim_Bucks_Vout(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_Vout_params* ours)
{	
	//FTP Limit value
	double Buck_Vout_Trim_HighLi[201]={0},Buck_Vout_Trim_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Buck_Vout_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_Vout_Trim_LowLi[i+1]);//item 1-48
	}
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Buck_TM_Reset();
	Reg[0x2008].set_clr_inv(BIT0);//disable reload
	//************Buck_Vout**********
	double Buck_Vout_Acc[PMU_MAX][SITE_MAX]={0},Buck_Vout_LPM101[PMU_MAX][SITE_MAX]={0},Buck_Vout_LPM102[PMU_MAX][SITE_MAX]={0},
		   Buck_Vout_LPM101_Acc[PMU_MAX][SITE_MAX]={0},Buck_Vout_LPM102_Acc[PMU_MAX][SITE_MAX]={0};
	TestNO_target=2;
	int Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	delayms(2);
	Fvi45.CH[VBYP]	.fv(6.5);	

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			if(nTILE==1||nTILE==2)
			{
				Fvi45.CH[VOUT_SW]	.fv(5);
				Fvi45.CH[VIN_3V3]	.fv(5);	
			}
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT3|BIT2|BIT0,BIT1);
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x00].set_clr_inv(BITNONE,BIT7|BIT6);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
			

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x30);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x18);
			delayms(2);
			Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
			if(Buck_Vset[nTILE]<1.5)
				Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,0.4,-0.4);


			/*
			for(i=0;i<32;i++)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x03].write_Data_trimvalue(4,0,i);
				delay(200);
				Fvi45.CH[GP_FB_Buck[nTILE]].mv(global_temp);
				Trim_Code_Value[i]=global_temp[0];
			}
			*/
			delayms(1);	
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x03].write_Data_trimvalue(4,0,0x13);		
			}
		}
		delayms(2);
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
		{
			Fvi45.CH[VOUT_SW]	.fv(5);
			Fvi45.CH[VIN_3V3]	.fv(5);	

			Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
			if(Buck_Vset[nTILE]<1.8)
				Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,0.4,-0.4);
			delayms(2);
			Reg[GP_User_Buck_Addr[nTILE]+0x03].set_clr_inv(BIT0);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0);

			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6|BIT3);

			Reg[0x2056].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
			Reg[0x2056].write(readback);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x08);
			delayms(2);
			/*
			
			for(i=0;i<32;i++)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x03].write_Data_trimvalue(4,0,i);
				delay(200);
				Fvi45.CH[GP_FB_Buck[nTILE]].mv(global_temp);
				Trim_Code_Value[i]=global_temp[0];
			}
			*/
			delayms(1);
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x03].write_Data_trimvalue(4,0,0x04);		
			}
		}
	}
	//===========================write default trim code========================//
	delayms(10);
	//========Measure pre value=========//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].mv(Multi_global_Pre[nTILE]);//use measure voltage
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];
		}
	}
	delayms(2);	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=4,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=32;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={32,32,32,32,32};//--> set table size
		unsigned char code_queue_array[PMU_MAX][32]={ { 15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,0},
													{ 15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,0},
													{ 15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,0},
													{ 15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,0},
													{ 15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,0}};

		if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
		{
			for(i=0;i<15;i++)
			{
				code_queue_array[0][i]=i+17;
			}
			code_queue_array[0][15]=1;
			code_queue_array[0][16]=2;
			code_queue_array[0][17]=3;
			code_queue_array[0][18]=4;
			code_queue_array[0][19]=5;
			code_queue_array[0][20]=6;
			code_queue_array[0][21]=16;
			code_queue_array[0][22]=7;
			code_queue_array[0][23]=8;
			code_queue_array[0][24]=9;
			code_queue_array[0][25]=10;
			code_queue_array[0][26]=11;
			code_queue_array[0][27]=12;
			code_queue_array[0][28]=13;
			code_queue_array[0][29]=0;
			code_queue_array[0][30]=14;
			code_queue_array[0][31]=15;
		}
		int ratio_queue_array[PMU_MAX][32] = {  {-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13},
												{-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13},	
												{-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13},	
												{-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13},	
												{-18,	-17,	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
		
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x03;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=Buck_Vset[nTILE]*0.0045;//V
				Multi_global_Target[nTILE][nSiteIndex]=0.995*Buck_Vset[nTILE];
				if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
					Multi_global_Target[nTILE][nSiteIndex]=0.995*Buck_Vset[nTILE];
			}
		}	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		//========Measure post value=========//
		delayms(2);
		FOR_EACH_TILE(nTILE,PMU_MAX)
			Fvi45.CH[GP_FB_Buck[nTILE]].mv(Multi_global_post[nTILE]);//use measure voltage

		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					//========Measure postvalue=========//
					Fvi45.CH[GP_FB_Buck[nTILE]].mv(global_temp);//use measure voltage
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
					}

				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_Vout_Acc[nTILE][nSiteIndex]=100*(Multi_global_post[nTILE][nSiteIndex]-0.995*Buck_Vset[nTILE])/(0.995*Buck_Vset[nTILE]);
			if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
					Buck_Vout_Acc[nTILE][nSiteIndex]=100*(Multi_global_post[nTILE][nSiteIndex]-0.995*Buck_Vset[nTILE])/(0.995*Buck_Vset[nTILE]);
		}
	}
	//==============power off=================//
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET);	
	Fvi45.CH[VBYP]	.fv(VIN_SET+1.5);	

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				//Multi_global_Target[nTILE][nSiteIndex]=Buck_Vset[nTILE];
				Buck_Vout_final[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Vout_Acc[nTILE][nSiteIndex]);
				
			}
		}
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				//Multi_global_Target[nTILE][nSiteIndex]=Buck_Vset[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Vout_Acc[nTILE][nSiteIndex]);		
			}
		}
	}
	//************Buck_Vout**********
	//************Buck_burst_Vout**********
	/*
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x80);//Configure LPM
	*/	
	/*
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_EN pin
	*/
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1

	double Buck_Vout_Burst_Target[PMU_MAX]={104,104,104,104,104};
	double Buck_Vout_Burst_Acc[PMU_MAX][SITE_MAX]={0},Buck_Vout_Burst_Fall[PMU_MAX][SITE_MAX]={0},Buck_Vout_Burst_HYS[PMU_MAX][SITE_MAX]={0};
	TestNO_target=42;
	print_index=TestNO_target+Print_Num-3;
	delayms(5);
	Fvi16.CH[GPIO1].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//Fvi16.CH[LPM].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	Reg[0x2008].set_clr_inv(BIT2|BIT0);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			if(nTILE!=4) Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6|BIT3);
			
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x00].set_clr_inv(BIT7|BIT6);	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xc0|readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2a);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			/*
			if(nTILE==4)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);	
			}
			*/
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
				if(nTILE!=4) FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xf0&readback[nSiteIndex];
				if(nTILE==4) FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xd0&readback[nSiteIndex];
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);		
			}
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		}
	}
	/*
	for(i=0;i<16;i++)
	{
		nTILE=4;
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		readback[0]=(0xf0&readback[0])|i;
		readback[1]=(0xf0&readback[1])|i;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);		
        
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write_Data_trimvalue(3,0,i);
		delay(200);
		//========Measure pre value=========//
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(BIT7|BIT6|BIT5);//Confiure LPM set:11
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT6|BIT5,BIT7);//Confiure EXT_EN set:01		
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.9*Buck_Vset[nTILE],1.2*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
		
		Trim_Code_Value[i]=1e2*global_temp[0]/Buck_Vset[nTILE];
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	}
	*/
	if(0)//Remove this trim on BZ
	{
		delayms(2);
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				//========Measure pre value=========//
				//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(BIT7|BIT6|BIT5);//Confiure LPM set:11
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
				//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT6|BIT5,BIT7);//Confiure EXT_EN set:01
				if(QC_Flag==0)//CTP
				{
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						0.98*Buck_Vset[nTILE],1.10*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
				}
				if(QC_Flag==1)//GNG
				{
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]+Buck_Vout_Trim_LowLi[46+8*nTILE]),1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]+Buck_Vout_Trim_HighLi[46+8*nTILE]),0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,10);
				}
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_Pre[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
					Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
				}
				//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			}
			
		}
		delayms(2);
		
		//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
		if(QC_Flag==0)
		{	
			//=========assign trim table and trim address==========//
			global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
			global_Trim_table_size=16;//--> set table size
			int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
			unsigned char code_queue_array[PMU_MAX][16]={   {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
															{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
															{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
															{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
															{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}};
			int ratio_queue_array[PMU_MAX][16] = {  {-7,	-6,	-5,	-4,	-3,	-2,	-1, 0,	0,	1,	2,	3,	4,	5,	6,	7},
													{-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,  0,	1,	2,	3,	4,	5,	6,	7},
													{-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,  0,	1,	2,	3,	4,	5,	6,	7},
													{-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,  0,	1,	2,	3,	4,	5,	6,	7},
													{-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,  0,	1,	2,	3,	4,	5,	6,	7}};	
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x07;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=1.2;//%
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Vout_Burst_Target[nTILE];	
				}
			}	
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{
						Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
							Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
					}
				}
			}
			//========Measure post value=========//
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					//========Measure pre value=========//
					if(nTILE==4)
					{
						Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
						FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
						Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
					}
					Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*Buck_Vset[nTILE]);
					delayms(1);
					//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(BIT7|BIT6|BIT5);//Confiure LPM set:11
					//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2a);
					//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2a);

					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10

					//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT6|BIT5,BIT7);//Confiure EXT_EN set:01		
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						1.0*Buck_Vset[nTILE],1.1*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
					FOR_EACH_SITE(nSiteIndex)
					{
						Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
					}			
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				}
			}
			//=========================================retrim===========================================//	
			for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
			{
				FOR_EACH_TILE(nTILE,PMU_MAX)
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
				delayms(2);
				FOR_EACH_TILE(nTILE,PMU_MAX)
				{
					if(PMU_ACOT_MODE[nTILE])
					{
						if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
						{
							//========Measure postvalue=========//
							if(nTILE==4)
							{
								Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
								FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
								Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
							}
							Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*Buck_Vset[nTILE]);
							delayms(1);
							//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(BIT7|BIT6|BIT5);//Confiure LPM set:11
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
							//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2a);

							//Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT6|BIT5,BIT7);//Confiure EXT_EN set:01		
							sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
								1.0*Buck_Vset[nTILE],1.1*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
							FOR_EACH_SITE(nSiteIndex)
							{
								if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
									Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
							}

						}
					}
				}
				if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
					break;
			}
				
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
						{
							store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
							store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
						}
						if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
						{		
							//---print internal_post1 and internal post2 and retrim count------------//
							pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
							pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
							pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
						}
					}
				}
			}

		}  
		//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Vout_Burst_Acc[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_Vout_Burst_Target[nTILE];
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Vout_Burst_Target[nTILE];
					pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
					if(QC_Flag==0)
					{
						pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
						pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
					}
					pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Vout_Burst_Acc[nTILE][nSiteIndex]);	
				}
			}
		}
		//Falling
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{

				//========Measure pre value=========//
				if(nTILE==4)
				{
					Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
					FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
					Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
				}
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.2*Buck_Vset[nTILE]);
				delayms(2);
				if(nTILE<2.5)
				{
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]-Buck_Vout_Trim_LowLi[83+2*nTILE]),1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]-Buck_Vout_Trim_HighLi[83+2*nTILE]),-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
				}
				if(nTILE>2.5)
				{
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]-Buck_Vout_Trim_LowLi[83+2*nTILE]),1e-2*Buck_Vset[nTILE]*(Buck_Vout_Burst_Target[nTILE]-Buck_Vout_Trim_HighLi[83+2*nTILE]),-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag,20,20);
				}
				//sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					//1.05*Buck_Vset[nTILE],0.95*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20);

				FOR_EACH_SITE(nSiteIndex)
				{
					Buck_Vout_Burst_Fall[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
					Buck_Vout_Burst_HYS[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_Vout_Burst_Fall[nTILE][nSiteIndex];
				}
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				FOR_EACH_SITE(nSiteIndex)
				{	
					pFunction->LogResult(NSITEINDEX, 82+2*nTILE, Buck_Vout_Burst_Fall[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, 83+2*nTILE, Buck_Vout_Burst_HYS[nTILE][nSiteIndex]);
				}
			}
		}
	}
	//************Buck_burst_Vout**********
	//************Buck_Vout_LP101/102**********
	//LPM_101
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2c);//Confiure GPIO1 set:10
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.08*Buck_Vset[nTILE]);
			delayms(2);
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				1e-2*Buck_Vset[nTILE]*(101+Buck_Vout_Trim_HighLi[93+4*nTILE]),1e-2*Buck_Vset[nTILE]*(101+Buck_Vout_Trim_LowLi[93+4*nTILE]),-0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,20);

			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Vout_LPM101[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
				Buck_Vout_LPM101_Acc[nTILE][nSiteIndex]=Buck_Vout_LPM101[nTILE][nSiteIndex]-101;
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			FOR_EACH_SITE(nSiteIndex)
			{	
				pFunction->LogResult(NSITEINDEX, 92+4*nTILE, Buck_Vout_LPM101[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 93+4*nTILE, Buck_Vout_LPM101_Acc[nTILE][nSiteIndex]);
			}
		}
	}
	delayms(1);
	//LPM_102
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*Buck_Vset[nTILE]);
			delayms(1);
			//sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				//0.98*Buck_Vset[nTILE],1.05*Buck_Vset[nTILE],0.002,LESS_THAN,1.5,global_temp,20);		
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				1e-2*Buck_Vset[nTILE]*(102+Buck_Vout_Trim_LowLi[95+4*nTILE]),1e-2*Buck_Vset[nTILE]*(102+Buck_Vout_Trim_HighLi[95+4*nTILE]),0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);

			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Vout_LPM102[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
				Buck_Vout_LPM102_Acc[nTILE][nSiteIndex]=Buck_Vout_LPM102[nTILE][nSiteIndex]-102;
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x20);//Can not change to 0x00, or it affect the Buck_Ton test.

			FOR_EACH_SITE(nSiteIndex)
			{	
				pFunction->LogResult(NSITEINDEX, 94+4*nTILE, Buck_Vout_LPM102[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 95+4*nTILE, Buck_Vout_LPM102_Acc[nTILE][nSiteIndex]);
			}
		}
	}
   	//************Buck_Vout_LP101/102**********
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Buck12_VoutLP
STT_TESTFUNC Trim_Buck12_VoutLP(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Buck12_VoutLP_params* ours)
{
	double Buck_VoutLP_Target[PMU_MAX]={0};
	double Buck_VoutLP_Acc[PMU_MAX][SITE_MAX]={0};
	int Print_Num=8;
	TestNO_target=2;
	print_index=TestNO_target+Print_Num-3;

	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Fvi45.CH[VOUT_SW]	.fv(5);
		Fvi45.CH[VIN_3V3]	.fv(5);	

		Factory_Mode_Check();
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{

		if(PMU_MultiP_Mode[nTILE])
		{

			Buck_VoutLP_Target[nTILE]=0.99*Buck_Vset[nTILE];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);//Reset from Vout test
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT3,BIT6);
			Reg[0x2056].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x3f&readback[nSiteIndex];
			Reg[0x2056].write(readback);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.1*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
			delayms(2);	
		}
	}
	if(QC_Flag==1)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
			{
				//========Measure postvalue=========//
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x81);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_VoutLP_Target[nTILE]);
				delayms(2);
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					1.05*Buck_VoutLP_Target[nTILE],0.95*Buck_VoutLP_Target[nTILE],-0.002,GREATER_THAN,1.5,global_temp,20);
				
				FOR_EACH_SITE(nSiteIndex)
					Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
				
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);//Confiure GPIO1 set:10
			}
		}
	}
	/*
	nTILE=0;
	for(i=0;i<16;i++)
	{

		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write((Reg[GP_Factory_Buck_Addr[nTILE]+0x07].CMI&0xe0)|i);

		Fvi45.CH[FB1].fv(1.25 * Buck_Vset[nTILE]);
		delayms(1);

		sweep_VI(RESOURCE_FVI45,FB1,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1.1*Buck_VoutLP_Target[nTILE],0.9*Buck_VoutLP_Target[nTILE],-0.002,GREATER_THAN,1.5,global_temp,20);
			
		Trim_Code_Value[i]	=global_temp[0];
	}
	*/
	delayms(2);

	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8}};
		int ratio_queue_array[PMU_MAX][16] = {  {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x07;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;

			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=0.007*Buck_VoutLP_Target[nTILE];//%
				Multi_global_Target[nTILE][nSiteIndex]=Buck_VoutLP_Target[nTILE];	
			}
		}	
		unsigned char code_queue_new[PMU_MAX][16]={0};
		bool table_need_from_max_to_min=true;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x81);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10

				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x08|(0xe0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

				Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0035*Buck_VoutLP_Target[nTILE]);
				delayms(1);
				sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE], code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
					RESOURCE_FVI16, GPIO1, VOLTAGE, LESS_THAN, 1.5, Multi_Global_trimcode[nTILE],  FindToggle, 100); 


				Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_VoutLP_Target[nTILE]);
				delayms(2);
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					1.05*Buck_VoutLP_Target[nTILE],0.95*Buck_VoutLP_Target[nTILE],-0.002,GREATER_THAN,1.5,global_temp,20);

				FOR_EACH_SITE(nSiteIndex)
					Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];

				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);//Confiure GPIO1 set:10

			}
		}
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure postvalue=========//
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x81);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10
						Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_VoutLP_Target[nTILE]);
						delayms(2);
						sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
							1.05*Buck_VoutLP_Target[nTILE],0.95*Buck_VoutLP_Target[nTILE],-0.002,GREATER_THAN,1.5,global_temp,20);
						
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
						}
						
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);//Confiure GPIO1 set:10
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
		{
			Fvi45.CH[VOUT_SW]	.fv(VIN_SET);
			Fvi45.CH[VIN_3V3]	.fv(VIN_SET);	

			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_VoutLP_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_VoutLP_Target[nTILE])/Buck_VoutLP_Target[nTILE];
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_VoutLP_Acc[nTILE][nSiteIndex]);	
			}
		}
	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Buck12_Islope
STT_TESTFUNC Trim_Buck12_Islope(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Buck12_Islope_params* ours)
{
	double Buck_Islope_Target[PMU_MAX]={3.5/(1+(0x01&(Reg[0x2057].CMI>>5))),3.5/(1+(0x01&(Reg[0x2057].CMI>>5))),0,0,0};
	double Buck_Islope_Acc[PMU_MAX][SITE_MAX]={0};
	int Print_Num=8;
	TestNO_target=2;
	print_index=TestNO_target+Print_Num-3;

	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Factory_Mode_Check();
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		}
		Reg[0x1019].write(0x08);
		Reg[0x101a].write(0x80);
		Reg[0x101b].write(_0x00);
		Reg[0x101c].write(_0x00);//Config EXT_PG pin

		Reg[0x2008].write(0x08);
		Fvi16.CH[EXT_PG].fv(1,SITE_ALL,Stall3V3,Stall50uA);
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{

		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x00].write(0x0f);
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6|BIT3);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[0x2056].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
			Reg[0x2056].write(readback);

			Fvi45.CH[GP_FB_Buck[nTILE]].fv(2);
			delayms(2);	
			//========Measure postvalue=========//
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x0e);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
			Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=-1e3*global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
		}
	}	
	/*
	nTILE=1;
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x0e);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);

	for(i=0;i<4;i++)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write((Reg[GP_Factory_Buck_Addr[nTILE]+0x05].CMI&0xcf)|(i<<4));

		Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage	
		Trim_Code_Value[i]=-1e3*global_temp[0];
	}
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
    */
	delayms(2);

	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=5,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=4;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={4,4,4,4,4};//--> set table size
		unsigned char code_queue_array[PMU_MAX][4]={   {0,1,2,3},
														{0,1,2,3},
														{0,1,2,3},
														{0,1,2,3},
														{0,1,2,3}};
		int ratio_queue_array[PMU_MAX][4] = {  {0,1,2,3},
												{0,1,2,3},
												{0,1,2,3},
												{0,1,2,3},
												{0,1,2,3}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x05;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;

				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.6;//uA
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Islope_Target[nTILE];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			//========Measure postvalue=========//
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x0e);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
				Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage
				FOR_EACH_SITE(nSiteIndex)
					Multi_global_post[nTILE][nSiteIndex]=-1e3*global_temp[nSiteIndex];
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
			}
		}
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure postvalue=========//
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x0e);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
						Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=-1e3*global_temp[nSiteIndex];
						}
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
			Fvi45.CH[GP_FB_Buck[nTILE]].open();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Islope_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Islope_Target[nTILE])/Buck_Islope_Target[nTILE];
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Islope_Acc[nTILE][nSiteIndex]);	
			}
		}
	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Buck12_IOS_Ratio
STT_TESTFUNC Trim_Buck12_IOS_Ratio(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Buck12_IOS_Ratio_params* ours)
{
	double Buck_Ioffset_Target[PMU_MAX]={0.25,0.25},Buck_Ratio_Target[PMU_MAX]={100000,100000};
	double Buck_Ioffset_Acc[PMU_MAX][SITE_MAX]={0},Buck_Ratio_Acc[PMU_MAX][SITE_MAX]={0};
	int Print_Num=8;
	TestNO_target=2;
	print_index=TestNO_target+Print_Num-3;

	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Factory_Mode_Check();
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		}
	}
	//************Buck_Ioffset**********

	/*
	nTILE=1;

	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x08|(0xf0&readback[nSiteIndex]);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);
	for(i=0;i<16;i++)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xf0&readback[nSiteIndex])|i;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(readback);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
		delayms(1);
		Fvi16.CH[EXT_PG].mi(global_temp);
		Trim_Code_Value[i]=-1e3*global_temp[0];
	}
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
    */
	delayms(2);
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//

		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {7,	6,	5,	4,	3,	2,	1,	0,	8,	9,	10,	11,	12,	13,	14,	15},
														{7,	6,	5,	4,	3,	2,	1,	0,	8,	9,	10,	11,	12,	13,	14,	15},
														{7,	6,	5,	4,	3,	2,	1,	0,	8,	9,	10,	11,	12,	13,	14,	15},
														{7,	6,	5,	4,	3,	2,	1,	0,	8,	9,	10,	11,	12,	13,	14,	15},
														{7,	6,	5,	4,	3,	2,	1,	0,	8,	9,	10,	11,	12,	13,	14,	15}};
		int ratio_queue_array[PMU_MAX][16] = {  {0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15},
												{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15},
												{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15},
												{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15},
												{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x05;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				if(0)
				{
					FOR_EACH_SITE(nSiteIndex)
					{
						Multi_global_LSB[nTILE][nSiteIndex]=0.0001;//uA
						Multi_global_Target[nTILE][nSiteIndex]=Buck_Ioffset_Target[nTILE];	
						Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
							Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
					}
				}
			}
		}

		unsigned char code_queue_new[PMU_MAX][16]={0};
		bool table_need_from_max_to_min=false;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			//========Measure postvalue=========//
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);

				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x08|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);

				Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x00|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(readback);
				delayms(1);
				sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE], code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
					RESOURCE_FVI16, EXT_PG, CURRENT, LESS_THAN, -Buck_Ioffset_Target[nTILE]/1e3, Multi_Global_trimcode[nTILE],  FindToggle, 200); 

				/*
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[0][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[0][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex],
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5,-999999,-Buck_Ioffset_Target[nTILE]/1e3);
				}
				*/
				delayms(1);
				Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage
				FOR_EACH_SITE(nSiteIndex)
					Multi_global_post[nTILE][nSiteIndex]=-1e3*global_temp[nSiteIndex];

				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			}
		}
		//=========================================retrim===========================================//	
		/*
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure postvalue=========//
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x0e);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
						Fvi16.CH[EXT_PG].mi(global_temp);//use measure voltage
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=-1e3*global_temp[nSiteIndex];
						}
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
		*/
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Ioffset_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Ioffset_Target[nTILE])/Buck_Ioffset_Target[nTILE];
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_Ioffset_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				//pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Ioffset_Acc[nTILE][nSiteIndex]);	
			}
		}
	}
	//************Buck_Ioffset**********
	//************Buck_Ratio**********
	double SW_Measure[2][SITE_MAX]={0};
	double Ratio_force[2]={500,1000};

	TestNO_target=18;
	print_index=TestNO_target+Print_Num-3;

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x08|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);

			Relay.BIT[GP_Buck_SW_Relay[nTILE]].set();
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,1,-1,1500,-1500);

			//Force I=500mA
			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[0]);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
			Fvi16.CH[EXT_PG].mi(I1);
			//Force I=1000mA

			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[1]);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
			Fvi16.CH[EXT_PG].mi(I2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex])/(I1[nSiteIndex]-I2[nSiteIndex]);
				Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];

			}
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
		}
	}

	/*
	nTILE=0;
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);
	
	for(i=0;i<16;i++)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=i|(0xf0&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);
		//Force I=500mA
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[0]);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
		Fvi16.CH[EXT_PG].mi(I1);
		//Force I=1000mA
		
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[1]);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
		Fvi16.CH[EXT_PG].mi(I2);

		Trim_Code_Value[i]=(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex])/(I1[nSiteIndex]-I2[nSiteIndex]);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
	}
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
	*/
	delayms(2);
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {0,1,2,3,4,5,6,7,	8,	9,	10,	11,	12,	13,	14,	15},
														{0,1,2,3,4,5,6,7,	8,	9,	10,	11,	12,	13,	14,	15},
														{0,1,2,3,4,5,6,7,	8,	9,	10,	11,	12,	13,	14,	15},
														{0,1,2,3,4,5,6,7,	8,	9,	10,	11,	12,	13,	14,	15},
														{0,1,2,3,4,5,6,7,	8,	9,	10,	11,	12,	13,	14,	15}};
		int ratio_queue_array[PMU_MAX][16] = {  {-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x01;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.076*Buck_Ratio_Target[nTILE];//
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Ratio_Target[nTILE];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		if(0)
		{
			unsigned char code_queue_new[PMU_MAX][16]={0};
			bool table_need_from_max_to_min=false;
			FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
			{			
				table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			//========Measure postvalue=========//
			if(PMU_MultiP_Mode[nTILE])
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);					
					//Force I=500mA
					delayms(1);
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[0]);
					delayms(2);
					Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
					Fvi16.CH[EXT_PG].mi(I1);
					//Force I=1000mA
					
					delayms(1);
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[1]);
					delayms(2);
					Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
					Fvi16.CH[EXT_PG].mi(I2);
					FOR_EACH_SITE(nSiteIndex)
					{
						Multi_global_post[nTILE][nSiteIndex]=(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex])/(I1[nSiteIndex]-I2[nSiteIndex]);						
					}
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
					
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
				}
			}
		}
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure postvalue=========//
						if(PMU_MultiP_Mode[nTILE])
						{
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1d);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x40);					
							//Force I=500mA
							delayms(1);
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[0]);
							delayms(2);
							Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
							Fvi16.CH[EXT_PG].mi(I1);
							//Force I=1000mA
							
							delayms(1);
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Ratio_force[1]);
							delayms(2);
							Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
							Fvi16.CH[EXT_PG].mi(I2);

							FOR_EACH_SITE(nSiteIndex)
							{
								if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
									Multi_global_post[nTILE][nSiteIndex]=(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex])/(I1[nSiteIndex]-I2[nSiteIndex]);						
							}
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
							
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Reg[0x2008].set_clr_inv(BIT2|BIT0);//Reset to DMUX mode after this test
		Fvi16.CH[EXT_PG].fi(0);
		Fvi16.CH[EXT_PG].open();
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Fqvi.CH[GP_SW_Buck[nTILE]].open();
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Ratio_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Ratio_Target[nTILE])/Buck_Ratio_Target[nTILE];
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Ratio_Acc[nTILE][nSiteIndex]);	
			}
		}
	}
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_Ton
STT_TESTFUNC Trim_Bucks_Ton(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_Ton_params* ours)
{
	double Buck_Ton_Acc[PMU_MAX][SITE_MAX]={0},
		   Buck_Tminon_Acc[PMU_MAX][SITE_MAX]={0},Buck_Tmaxon_Acc[PMU_MAX][SITE_MAX]={0},
		   Buck_Ton_100ns_Acc[PMU_MAX][SITE_MAX]={0},Buck_Toff_100ns_Acc[PMU_MAX][SITE_MAX]={0},Buck_Toff_125ns_Acc[PMU_MAX][SITE_MAX]={0};
	double Buck_Tminon[PMU_MAX][SITE_MAX]={0},Buck_Tmaxon[PMU_MAX][SITE_MAX]={0},
		   Buck_Ton_100ns[PMU_MAX][SITE_MAX]={0},Buck_Toff_100ns[PMU_MAX][SITE_MAX]={0},Buck_Toff_125ns[PMU_MAX][SITE_MAX]={0};
	int Buck_HSB_Trim_Bit_set[PMU_MAX][SITE_MAX]={0};
	double Buck_Ton_0x00_HSB0[PMU_MAX][SITE_MAX]={0},Thson[PMU_MAX][SITE_MAX]={0}, Tswvision[PMU_MAX][SITE_MAX]={0};
	double Toffset_Target[PMU_MAX]={26,29,14.5,14.5,14.5};
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_Ton**********
	/*
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x80);//Configure LPM
	*/	
	/*
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_EN pin
	*/
	/*
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	*/
	TestNO_target=7;
	int Print_Num=18;
	print_index=TestNO_target+Print_Num-13;
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K21_GPIO1_TMUA].set(); // CLOSE AMUX_BUF
	delayms(1);
	TMU_CH1.stallset(Stall7V,TMU1uS,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(0.5,UP);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	delayms(2);
	if(1)
 	{																									
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{									 
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT5|BIT4|BIT1);																					
				Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
							 
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xbf&readback[nSiteIndex];			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
				Reg[0x2008].write(0x2c);		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x84);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7,BIT6|BIT5|BIT4|BIT3|BIT2);//Confiure GPIO1 set:10
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.2);
				delayms(1);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
					Thson[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x82);
				
				delayms(1);
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
					Tswvision[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Toffset[nTILE][nSiteIndex]=Thson[nTILE][nSiteIndex]-Tswvision[nTILE][nSiteIndex]-10.0;
					if(fabs(Toffset[nTILE][nSiteIndex]-Toffset_Target[nTILE])>2)
						Toffset[nTILE][nSiteIndex]=Toffset_Target[nTILE];
					Buck_Ton_Trim_Target[nTILE][nSiteIndex]=1e3*Buck_Vset[nTILE]/VIN_SET/2/0.85-Toffset[nTILE][nSiteIndex];
				}
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write_CMI();
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT5|BIT4|BIT1|BIT0);				
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6|BIT3);
			Reg[0x2056].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x7f&readback[nSiteIndex]);
			Reg[0x2056].write(readback);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x88);	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write((0x04&(~Reg[GP_Factory_Buck_Addr[nTILE]+0x00].CMI))<<5);	
							  
							  
			if(QC_Flag==0)
			{			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x90|(0x0f&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	
			}
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.00*Buck_Vset[nTILE]);
			delayms(1);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.98*Buck_Vset[nTILE]);
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, 2+nTILE, Toffset[nTILE][nSiteIndex]);
			}
		}
	}

	Fvi16.CH[GPIO1].fi(25,SITE_ALL,Stall3V3,Stall50mA);
	//Fvi16.CH[LPM].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	delayms(2);
			
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa2);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			//Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT5|BIT4|BIT1);				
		}
	}
	//Buck_HSB_Trim_Bit_set
	if(QC_Flag==0)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			//========Measure pre value=========//
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x00&readback[nSiteIndex];
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		
				
				measure_TMUCH1();
				FOR_EACH_SITE(nSiteIndex)
				{
					Buck_Ton_0x00_HSB0[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];					
					if(Buck_Ton_Trim_Target[nTILE][nSiteIndex]<(Buck_Ton_0x00_HSB0[nTILE][nSiteIndex]-2))
						 Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]=0x20;			
				}
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			}
			
		}
	}
	delayms(2);	
	//Buck_HSB_Trim_Bit_set
	//ACOT trim table
	/*
	for(i=0;i<16;i++)
	{
		nTILE=3;	
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]|(i<<1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		
		delay(200);
		//========Measure pre value=========//
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
		delayms(1);
		measure_TMUCH1();
		
		Trim_Code_Value[i]=1e6*global_temp[0];
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	}
	*/
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		//========Measure pre value=========//
		if(PMU_ACOT_MODE[nTILE])
		{
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa2);
			//Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x3e&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		
			if(QC_Flag==0)
			{	
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]|0x16|(0x00&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
		}
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
			if(PMU_MultiP_Mode[0])
			{
				TMU_CH1.stallset(Stall7V,TMUCOUNT,1);
				delayms(10);
			}
			TMU_CH1.arm(1);
			delayms(2);
			TMU_CH1.read();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck12_MultiP_Freq[nTILE][nSiteIndex]=meas_data[nSiteIndex+1]/1e3;	
				Buck_Ton_Trim_Target[nTILE][nSiteIndex]=1e3*0.85/Buck12_MultiP_Freq[nTILE][nSiteIndex]/(VIN_SET/(0.98*Buck_Vset[nTILE]));
			}
			if(PMU_MultiP_Mode[1])
			{
				TMU_CH1.stallset(Stall7V,TMU1uS,1);
				delayms(10);
			}
		}
		delayms(1);
		measure_TMUCH1();
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Pre[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
			Multi_global_post[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
		}
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	}
	delayms(2);
	/*	//Multiphase trim table

	for(i=0;i<16;i++)
	{
		nTILE=0;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x0f&readback[nSiteIndex])|i<<4;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
		TMU_CH1.stallset(Stall7V,TMU1uS,1);
		TMU_CH1.switchset(STARTtoA+STOPtoA);
		TMU_CH1.startset(1.0,UP);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		delayms(1);
		measure_TMUCH1();		
		Trim_Code_Value[i]=1e6*global_temp[0];
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	}
	*/
	if(Buck_Ton_ZC_Target_Print)
	{
		printf("\n----------------------Bucks_Ton_Target_set-----------------------------");
		FOR_EACH_SITE(nSiteIndex)	
		{
			printf("\n Buck1_Ton=%3.3fnS", Buck_Ton_Trim_Target[0][nSiteIndex]); 
			printf("\n Buck2_Ton=%3.3fnS", Buck_Ton_Trim_Target[1][nSiteIndex]); 
			printf("\n Buck3_Ton=%3.3fnS", Buck_Ton_Trim_Target[2][nSiteIndex]); 
			printf("\n Buck4_Ton=%3.3fnS", Buck_Ton_Trim_Target[3][nSiteIndex]); 
			printf("\n Buck5_Ton=%3.3fnS", Buck_Ton_Trim_Target[4][nSiteIndex]); 
		}
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=4,global_BitLow=1,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
														{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
		int ratio_queue_array[PMU_MAX][16] = {  {	-11,-10,-9,-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4},
												{	-11,-10,-9,-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4},
												{	-11,-10,-9,-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4},
												{	-11,-10,-9,-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4},
												{	-11,-10,-9,-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x00;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.045*Buck_Ton_Trim_Target[nTILE][nSiteIndex];//nS
				}
			}
			if(PMU_MultiP_Mode[nTILE])
			{
				for(i=0;i<16;i++)
				{
					code_queue_array[nTILE][i]=15-i;
					ratio_queue_array[nTILE][i]=i-6;
				}
			
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x04;//--> set trim address
				Multi_global_BitHigh[nTILE]=7;//--> set trim bits
				Multi_global_BitLow[nTILE]=4;
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.075*Buck_Ton_Trim_Target[nTILE][nSiteIndex];//nS
				}			
			}
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_Ton_Trim_Target[nTILE][nSiteIndex];	
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		//========Measure post value=========//
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);		
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]|(0x3e&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		

				delayms(2);				
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
			}
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
			}
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
			}
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}		
		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				//========Measure post value=========//
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					if(PMU_ACOT_MODE[nTILE])
					{
						Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);		
						FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]|(0x3e&readback[nSiteIndex]);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
						delayms(2);				
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
						delayms(1);
					}
					if(PMU_MultiP_Mode[nTILE])
					{
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
					}
					delayms(2);
					measure_TMUCH1();
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
					}
					delayms(1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x00].Trimmed[nSiteIndex]=Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]|Reg[GP_Factory_Buck_Addr[nTILE]+0x00].Trimmed[nSiteIndex];
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Post Frequency&Target
	/*
	if(QC_Flag==0)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
				TMU_CH1.stallset(Stall7V,TMUCOUNT,1);
				delayms(2);
				TMU_CH1.arm(1);
				delayms(2);
				TMU_CH1.read();
				FOR_EACH_SITE(nSiteIndex)
				{
					Buck12_MultiP_Freq[nTILE][nSiteIndex]=meas_data[nSiteIndex+1]/1e3;	
				}
				TMU_CH1.stallset(Stall7V,TMU1uS,1);
			}
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	*/
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Ton_Measure[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
			Buck_Ton_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Ton_Trim_Target[nTILE][nSiteIndex])/Buck_Ton_Trim_Target[nTILE][nSiteIndex];
		}
	}

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT3,BIT6);//Confiure GPIO1 set:10
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(((Reg[GP_Factory_Buck_Addr[nTILE]+0x00].CMI&0x04)<<5)|Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].CMI);//TM_MINON_BURST: F_YE[7]= F_Y0[2]
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Minscl_PWM[nTILE][nSiteIndex]=1e5*global_temp[nSiteIndex]*Buck12_MultiP_Freq[nTILE][nSiteIndex]*VIN_SET/(0.95*Buck_Vset[nTILE]);
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write_CMI();//TM_MINON_BURST: F_YE[7]= F_Y0[2]
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	//Measure Ton with Offset settings

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x3e&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
			
			for(i=0;i<4;i++)
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x05].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xcf&readback[nSiteIndex])|(i<<4);
				Reg[GP_User_Buck_Addr[nTILE]+0x05].write(readback);	
				delayms(1);
				measure_TMUCH1();
			
				FOR_EACH_SITE(nSiteIndex)
				{
					Buck_FSWO[i][nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];	
					Buck_FSWO_Acc[i][nTILE][nSiteIndex]=1e2*(Buck_FSWO[i][nTILE][nSiteIndex]-Buck_FSWO[0][nTILE][nSiteIndex])/Buck_FSWO[0][nTILE][nSiteIndex];
				}
			}
			Reg[GP_User_Buck_Addr[nTILE]+0x05].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xcf&readback[nSiteIndex]);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].write(readback);				
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	//Tminon
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,DOWN);
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);

			delayms(1);
			//========Measure pre value=========//
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4|BIT2,BIT6|BIT5|BIT3);//Confiure GPIO1 set:10
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Tminon[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				Buck_Tminon_Acc[nTILE][nSiteIndex]=1e2*(Buck_Tminon[nTILE][nSiteIndex]-30)/30;
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	//Tmaxon_3uS
	TMU_CH1.stallset(Stall7V,TMU4mS,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(2,UP);
	TMU_CH1.stopset(2,DOWN);
	TMU_CH1.arm(1);
	delayms(2);
	unsigned char Ton_3us_Value[SITE_MAX]={0},RegF0x00[SITE_MAX]={0};

	Reg[0x2000].read(RegF0x00);
	FOR_EACH_SITE(nSiteIndex)	Ton_3us_Value[nSiteIndex]=0x80|(0x00&RegF0x00[nSiteIndex]);
	Reg[0x2000].write(Ton_3us_Value);

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4|BIT3,BIT6|BIT5|BIT2);//Confiure GPIO1 set:10
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Tmaxon[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
				Buck_Tmaxon_Acc[nTILE][nSiteIndex]=1e2*(Buck_Tmaxon[nTILE][nSiteIndex]-3.5)/3.5;
				/*
				for(i=0;i<50;i++)
				{
					if(fabs(Buck_Tmaxon[nTILE][nSiteIndex]-3)>1)
					{
						TMU_CH1.arm(1,nSiteIndex);
						delay(200);
						TMU_CH1.read(nSiteIndex);
						global_temp[nSiteIndex]=meas_data[nSiteIndex+1]/1e3;	
						Buck_Tmaxon[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
					}
					if(fabs(Buck_Tmaxon[nTILE][nSiteIndex]-3)<1)
						break;
				}
				*/
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	Reg[0x2000].write(RegF0x00);

	//Ton_100nS
	TMU_CH1.stallset(Stall7V,TMU1uS,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.2,UP);
	TMU_CH1.stopset(1.2,DOWN);
	TMU_CH1.arm(1);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xbc);	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Ton_100ns[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				Buck_Ton_100ns_Acc[nTILE][nSiteIndex]=1e2*(Buck_Ton_100ns[nTILE][nSiteIndex]-100)/100;
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);	
		}
	}
	//Toff_100/125nS
	unsigned char RegF0xX7[PMU_MAX][SITE_MAX]={0};

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);

			//100ns
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			//FOR_EACH_SITE(nSiteIndex) RegF0xX7[nTILE][nSiteIndex]=0xaf&(0x80|readback[nSiteIndex]);
			FOR_EACH_SITE(nSiteIndex) RegF0xX7[nTILE][nSiteIndex]=0x2f&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(RegF0xX7[nTILE]);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xbb);	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT4,BIT6|BIT5|BIT3|BIT2);//Confiure GPIO1 set:10
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Toff_100ns[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				Buck_Toff_100ns_Acc[nTILE][nSiteIndex]=1e2*(Buck_Toff_100ns[nTILE][nSiteIndex]-100)/100;
			}
			//125ns
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex) RegF0xX7[nTILE][nSiteIndex]=0xaf&(0x80|readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(RegF0xX7[nTILE]);
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Toff_125ns[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];
				Buck_Toff_125ns_Acc[nTILE][nSiteIndex]=1e2*(Buck_Toff_125ns[nTILE][nSiteIndex]-125)/125;
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		}
	}
	//==============power off=================//

	Fvi16.CH[GPIO1].fi(0);
	Fvi16.CH[GPIO1].open();
	Relay.BIT[K21_GPIO1_TMUA].clr(); // CLOSE AMUX_BUF
	Relay.BIT[K47_TMU].clr();

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=Buck_Ton_Trim_Target[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, (Buck_HSB_Trim_Bit_set[nTILE][nSiteIndex]>>1)|Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Ton_Acc[nTILE][nSiteIndex]);
			if(PMU_ACOT_MODE[nTILE])
			{				
				pFunction->LogResult(NSITEINDEX, TestNO_target+8+Print_Num*nTILE, Buck_Tminon[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+9+Print_Num*nTILE, Buck_Tminon_Acc[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+10+Print_Num*nTILE, Buck_Tmaxon[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+11+Print_Num*nTILE, Buck_Tmaxon_Acc[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+12+Print_Num*nTILE, Buck_Ton_100ns[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+13+Print_Num*nTILE, Buck_Ton_100ns_Acc[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+14+Print_Num*nTILE, Buck_Toff_100ns[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+15+Print_Num*nTILE, Buck_Toff_100ns_Acc[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+16+Print_Num*nTILE, Buck_Toff_125ns[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+17+Print_Num*nTILE, Buck_Toff_125ns_Acc[nTILE][nSiteIndex]);

			}
			
		}			
	}
	//************Buck_Ton**********
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Bucks_Vclamp_Low
STT_TESTFUNC Trim_Bucks_Vclamp_Low(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_Vclamp_Low_params* ours)
{
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_Vclamp**********
	TestNO_target=2;
	double Buck_Vclamp_Instd_P[PMU_MAX][SITE_MAX]={0},Buck_Vclamp_Instd_F[PMU_MAX][SITE_MAX]={0};
	double Buck_Verr_Minstd_P[PMU_MAX][SITE_MAX]={0},Buck_Verr_Minstd_F[PMU_MAX][SITE_MAX]={0};

	double Buck_Vclamp_Instd[PMU_MAX][SITE_MAX]={0};
	double Buck_Verr_Minstd[PMU_MAX][SITE_MAX]={0};

	double Buck_Vclamp_Target[PMU_MAX]={0};
	double Buck_Vclamp_MAX[PMU_MAX]={0};

	double Buck_Vclamp_Acc[PMU_MAX][SITE_MAX]={0};

	double Buck_VFFDC[PMU_MAX][SITE_MAX]={0};
	double Buck_VFFDC_Acc[PMU_MAX][SITE_MAX]={0};

	double Buck_VMeasure_nstd[PMU_MAX][SITE_MAX]={0};
	double Buck_Clamp_nstd[PMU_MAX][SITE_MAX]={0};
	double Buck_Clamp_high_nstd[PMU_MAX][SITE_MAX]={0};
	double Buck_Clamp_high_nstd_Acc[PMU_MAX][SITE_MAX]={0};

	double VswUV=2.7-0.1*((Reg[0x10f5].CMI>>3)&0x03);//V
	double VIN_MINDrop[PMU_MAX]={0};//V
	double VINMIN[PMU_MAX]={0};//V
	double CFF_MAX=7.08;//pF
	double VRipple_Target_MIN=90;//mV
	double VIN_SET_MAX=3.8;//pF

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		VIN_MINDrop[nTILE]=Buck_Vset[nTILE]/(1-0.045*1.6)+0.15;
		
		if(VswUV>VIN_MINDrop[nTILE])
			VINMIN[nTILE]=VswUV;
		else
			VINMIN[nTILE]=VIN_MINDrop[nTILE];
		
		Buck_Vclamp_Target[nTILE]=1.2*VINMIN[nTILE]*VRipple_Target_MIN*(VIN_SET-Buck_Vset[nTILE]-0.15)/(VINMIN[nTILE]-Buck_Vset[nTILE]-0.15)/VIN_SET;
		Buck_Vclamp_MAX[nTILE]=1.2*VINMIN[nTILE]*VRipple_Target_MIN*(VIN_SET_MAX-Buck_Vset[nTILE]-0.15)/(VINMIN[nTILE]-Buck_Vset[nTILE]-0.15)/VIN_SET_MAX;
		
		if(Buck_Vclamp_MAX[nTILE]>200)
			Buck_Vclamp_Target[nTILE]=0.140;
		else if(Buck_Vclamp_MAX[nTILE]>160&&Buck_Vclamp_MAX[nTILE]<200)
			Buck_Vclamp_Target[nTILE]=0.120;
		else			Buck_Vclamp_Target[nTILE]=0.100;
			
	}
	int Print_Num=18;
	print_index=TestNO_target+Print_Num-3;
	double EXT_PG_V1[2]={0},EXT_PG_V2[2]={0};
	delayms(2);
	Relay.BIT[K26_AMUX_Buffer].set(); // CLOSE AMUX_BUF
	delayms(2);
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);//use measure voltage
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x3e&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BITNONE,BIT5|BIT4|BIT3|BIT2);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE]);
		}
	}
	
	/*
	for(i=0;i<16;i++)
	{
		nTILE=1;

		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(i);
		//========Measure pre value=========//
		//Buck_Vclamp_Instd
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
		//V1
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		//V2	
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		Buck_Vclamp_Instd[nTILE][0]=(EXT_PG_V1[0]+EXT_PG_V2[0])/2;
		
		//Buck_Verr_Minstd
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x76);	
		//V1
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);	
		//V2	
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);		
		Buck_Verr_Minstd[nTILE][0]=(EXT_PG_V1[0]+EXT_PG_V2[0])/2;
        Trim_Code_Value[i]=Buck_Vclamp_Instd[nTILE][0]-Buck_Verr_Minstd[nTILE][0];
		
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
	}
	*/
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT1);//Default code2		
			}

			//========Measure pre value=========//
			//Buck_Vclamp_Instd
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
			//V1
			Reg[0x2008].write(0x28);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
			//V2	
			Reg[0x2008].write(0x38);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Vclamp_Instd_P[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				Buck_Vclamp_Instd_F[nTILE][nSiteIndex]=Buck_Vclamp_Instd_P[nTILE][nSiteIndex];
			}	
			//Buck_Verr_Minstd
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x76);	
			//V1
			Reg[0x2008].write(0x28);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V1);	
			//V2	
			Reg[0x2008].write(0x38);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V2);		
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Verr_Minstd_P[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				Buck_Verr_Minstd_F[nTILE][nSiteIndex]=Buck_Verr_Minstd_P[nTILE][nSiteIndex];
				global_temp[nSiteIndex]=Buck_Vclamp_Instd_P[nTILE][nSiteIndex]-Buck_Verr_Minstd_P[nTILE][nSiteIndex];
				Multi_global_Pre[nTILE][nSiteIndex]=global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
			}
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE, BIT0|BIT1);

		}
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		double Vclamp_global_LSB[PMU_MAX]={0.013,0.0145,0.014,0.014,0.011};
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15},
														{0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15},
														{0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15},
														{0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15},
														{0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15}};
		int ratio_queue_array[PMU_MAX][16] = {  {	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x05;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=Vclamp_global_LSB[nTILE];//mV
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Vclamp_Target[nTILE];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		//========Measure post value=========//
		delayms(2);
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			if(PMU_ACOT_MODE[nTILE])
			{
				//========Measure pre value=========//
				//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

				//Buck_Vclamp_Instd
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
				//V1
				Reg[0x2008].write(0x28);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
				//V2	
				Reg[0x2008].write(0x38);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
				FOR_EACH_SITE(nSiteIndex)
				{
					Buck_Vclamp_Instd_F[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				}
				
				//Buck_Verr_Minstd
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x76);	
				//V1
				Reg[0x2008].write(0x28);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V1);	
				//V2	
				Reg[0x2008].write(0x38);
				delayms(1);
				Fvi16.CH[EXT_PG].mv(EXT_PG_V2);		
				FOR_EACH_SITE(nSiteIndex)
				{
					Buck_Verr_Minstd_F[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
					global_temp[nSiteIndex]=Buck_Vclamp_Instd_F[nTILE][nSiteIndex]-Buck_Verr_Minstd_F[nTILE][nSiteIndex];
					Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
				}
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
				//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE, BIT0|BIT1);

			}
		}		
		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure pre value=========//
						//Buck_Vclamp_Instd
						//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
						//V1
						Reg[0x2008].write(0x28);
						delayms(1);
						Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
						//V2	
						Reg[0x2008].write(0x38);
						delayms(1);
						Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
						FOR_EACH_SITE(nSiteIndex)
						{
							Buck_Vclamp_Instd_F[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
						}
						
						//Buck_Verr_Minstd
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x76);	
						//V1
						Reg[0x2008].write(0x28);
						delayms(1);
						Fvi16.CH[EXT_PG].mv(EXT_PG_V1);	
						//V2	
						Reg[0x2008].write(0x38);
						delayms(1);
						Fvi16.CH[EXT_PG].mv(EXT_PG_V2);		
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							{
								Buck_Verr_Minstd_F[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
								global_temp[nSiteIndex]=Buck_Vclamp_Instd_F[nTILE][nSiteIndex]-Buck_Verr_Minstd_F[nTILE][nSiteIndex];						
								Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
							}
						}
						delayms(1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
						//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE, BIT0|BIT1);

					}
				}
				if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
					break;
			}
		}
		
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_Vclamp_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Vclamp_Target[nTILE])/Buck_Vclamp_Target[nTILE];
		}
	}
	//==============power off=================//
	//VFFDC
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{	
		if(PMU_ACOT_MODE[nTILE])
		{
			//========Measure pre value=========//
			//Buck_Vclamp_Instd
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xef&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x35);
			//V1
			Reg[0x2008].write(0x28);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
			//V2	
			Reg[0x2008].write(0x38);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_VFFDC[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				Buck_VFFDC_Acc[nTILE][nSiteIndex]=1e2*(Buck_VFFDC[nTILE][nSiteIndex]-Buck_Vclamp_Instd_P[nTILE][nSiteIndex])/Buck_Vclamp_Instd_P[nTILE][nSiteIndex];
			}	
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE, BIT0|BIT1);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
		}
	}
	//Vclamp_high_nstd
	double Buck_Clamp_high_nstd_target[PMU_MAX]={0.34,0.34,0.34,0.34,0.34};
	//if(CMI_Rev_No==520103||CMI_Rev_No==520100||CMI_Rev_No==522100) Buck_Clamp_high_nstd_target[3]=0.34;//F_0xx6[5:3] can affect the target
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*Buck_Vset[nTILE]);

			//========Measure pre value=========//
			//Buck_Vclamp_Instd
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
			//V1
			Reg[0x2008].write(0x28);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
			//V2	
			Reg[0x2008].write(0x38);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Clamp_nstd[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
			}

			//Buck_Measure_Instd
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x76);	
			//V1
			Reg[0x2008].write(0x28);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
			//V2	
			Reg[0x2008].write(0x38);
			delayms(1);
			Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_VMeasure_nstd[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
				Buck_Clamp_high_nstd[nTILE][nSiteIndex]=Buck_VMeasure_nstd[nTILE][nSiteIndex]-Buck_Clamp_nstd[nTILE][nSiteIndex];
				Buck_Clamp_high_nstd_Acc[nTILE][nSiteIndex]=1e2*(Buck_Clamp_high_nstd[nTILE][nSiteIndex]-Buck_Clamp_high_nstd_target[nTILE])/Buck_Clamp_high_nstd_target[nTILE];
			}
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x75);
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE, BIT0|BIT1);
		}
	}

	Reg[0x2008].write(_0x00);	
	Relay.BIT[K26_AMUX_Buffer].clr(); // CLOSE AMUX_BUF
	delayms(2);
	Fvi16.CH[EXT_PG].open();//use measure voltage

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_Vclamp_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Buck_Vclamp_Instd_P[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Buck_Verr_Minstd_P[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+5+Print_Num*nTILE, Buck_Vclamp_Instd_F[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+6+Print_Num*nTILE, Buck_Verr_Minstd_F[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+7+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+8+Print_Num*nTILE, Buck_Vclamp_Acc[nTILE][nSiteIndex]);
				
				pFunction->LogResult(NSITEINDEX, TestNO_target+12+Print_Num*nTILE, Buck_VFFDC[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+13+Print_Num*nTILE, Buck_VFFDC_Acc[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+14+Print_Num*nTILE, Buck_VMeasure_nstd[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+15+Print_Num*nTILE, Buck_Clamp_nstd[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+16+Print_Num*nTILE, Buck_Clamp_high_nstd[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+17+Print_Num*nTILE, Buck_Clamp_high_nstd_Acc[nTILE][nSiteIndex]);
			}
		}
	}
	//************Buck_Vclamp**********
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_Ripple
STT_TESTFUNC Trim_Bucks_Ripple(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_Ripple_params* ours)
{
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_Ripple**********
	TestNO_target=2;
	int Print_Num=9;
	print_index=TestNO_target+Print_Num-4;

	double Buck_Ripple_Target[PMU_MAX][SITE_MAX]={0};
	double Buck_Ripple_Acc[PMU_MAX][SITE_MAX]={0};
	double Buck_Vripple[PMU_MAX][SITE_MAX]={0};
	double VswUV=2.7-0.1*((Reg[0x10f5].CMI>>3)&0x03);//V
	double VIN_MINDrop[PMU_MAX]={0};//V
	double VINMIN[PMU_MAX]={0};//V
	double Gain_Couple[PMU_MAX]={0};
	double CFF_MAX=7.08;//pF
	double VRipple_Target_MIN=90;//mV
	double Ton_Measure_MIN[PMU_MAX][SITE_MAX]={0};
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		VIN_MINDrop[nTILE]=Buck_Vset[nTILE]/(1-0.045*1.6)+0.15;

		if(VswUV>VIN_MINDrop[nTILE])
			VINMIN[nTILE]=VswUV;
		else
			VINMIN[nTILE]=VIN_MINDrop[nTILE];

		Gain_Couple[nTILE]=0.0073*pow(Buck_Vset[nTILE],6)-0.1077*pow(Buck_Vset[nTILE],5)+0.6194*pow(Buck_Vset[nTILE],4)-
			1.7174*pow(Buck_Vset[nTILE],3)+2.3317*pow(Buck_Vset[nTILE],2)-1.5025*pow(Buck_Vset[nTILE],1)+1.2758*pow(Buck_Vset[nTILE],0);

		FOR_EACH_SITE(nSiteIndex)
		{
			Ton_Measure_MIN[nTILE][nSiteIndex]=(Ton_Measure[nTILE][nSiteIndex]+Toffset[nTILE][nSiteIndex])*VIN_SET/VINMIN[nTILE];
			Buck_Ripple_Target[nTILE][nSiteIndex]=1e3*(VINMIN[nTILE]-Buck_Vset[nTILE]-0.15)*Ton_Measure_MIN[nTILE][nSiteIndex]*Gain_Couple[nTILE]/CFF_MAX/VRipple_Target_MIN;
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				if(Buck_Ripple_Target[nTILE][nSiteIndex]<200)
				{
					Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
					readback[nSiteIndex]=0x20|readback[nSiteIndex];
					Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
				}
			}
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
			delayms(1);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xef&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.02*Buck_Vset[nTILE]);
		}
	}
	/*
	for(i=0;i<16;i++)
	{
		nTILE=4;

		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
		readback[0]=(i<<4)|(0x0f&readback[0]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(readback);	

		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}	
		//========Measure pre value=========//
		//Buck_Ripple_Instd
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x3a);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(2);
		delayms(2);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(global_temp);	

		Trim_Code_Value[i]=1e3*global_temp[0];
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x30);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(2);
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
	*/
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			if(QC_Flag==0)
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x0f&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(readback);	
			}
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
				delayms(2);
			}

			//========Measure pre value=========//
			//Buck_Ripple_Instd
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x3a);

			Fqvi.CH[GP_SW_Buck[nTILE]].fv(2,SITE_ALL,SET_CLAMP,4,-4,40,-40);
			delayms(5);
			Fqvi.CH[GP_SW_Buck[nTILE]].fv(2,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(global_temp);	

			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
			}
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			//Fqvi.CH[GP_SW_Buck[nTILE]].fv(0);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);

			delayms(2);
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}
	}
	delayms(2);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double Ripple_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=7,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},
														{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},
														{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},
														{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},
														{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0}};
		int ratio_queue_array[PMU_MAX][16] = {  {-7,-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,8},
												{-7,-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,8},
												{-7,-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,8},
												{-7,-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,8},
												{-7,-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7,8}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x05;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.525;
					Multi_global_Target[nTILE][nSiteIndex]=2e3/Buck_Ripple_Target[nTILE][nSiteIndex];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		//========Measure post value=========//
		delayms(2);
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].set();
					delayms(2);			
				}
				//========Measure pre value=========//
				//Buck_Ripple_Instd
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x3a);
				delayms(5);			
				Fqvi.CH[GP_SW_Buck[nTILE]].mi(global_temp);	
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_post[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
				}
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
				delayms(2);
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].clr();
				}
			}
		}		
		//=========================================retrim===========================================//
		
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].set();
							delayms(2);			
						}
						
						//========Measure pre value=========//
						//Buck_Ripple_Instd
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x3a);		
						delayms(5);
						Fqvi.CH[GP_SW_Buck[nTILE]].mi(global_temp);	
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=1e3*global_temp[nSiteIndex];
						}
						delayms(1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
						delayms(1);
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].clr();
						}
					}
				}
				if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
					break;
			}
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, 2e3/store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, 2e3/store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_Ripple_Acc[nTILE][nSiteIndex]=1e2*(2e3/Multi_global_post[nTILE][nSiteIndex]-Buck_Ripple_Target[nTILE][nSiteIndex])/Buck_Ripple_Target[nTILE][nSiteIndex];
			Buck_Vripple[nTILE][nSiteIndex]=(VIN_SET-Buck_Vset[nTILE])*Buck_Ton_Trim_Target[nTILE][nSiteIndex]*Gain_Couple[nTILE]/(5.9*2e3/Multi_global_post[nTILE][nSiteIndex]);
		}
	}

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		}
		//Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		//Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}
	//==============power off=================//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_Ripple_Target[nTILE][nSiteIndex];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, 2e3/Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, 2e3/Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Ripple_Acc[nTILE][nSiteIndex]);		
				pFunction->LogResult(NSITEINDEX, TestNO_target+8+Print_Num*nTILE, Buck_Vripple[nTILE][nSiteIndex]);		
			}
		}
	}
    
	delayms(2);

	//************Buck_Ripple**********
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_ILIM
STT_TESTFUNC Trim_Bucks_ILIM(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_ILIM_params* ours)
{
	//FTP Limit value
	double Buck_ILIM_Trim_HighLi[301]={0},Buck_ILIM_Trim_LowLi[301]={0};
    for(i=0;i<300;i++)
	{
		pFunction->GetHiLimit(i,&Buck_ILIM_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_ILIM_Trim_LowLi[i+1]);//item 1-48
	}

	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_HSILIMIT**********
	TestNO_target=2;
	int Print_Num=8; 
	print_index=TestNO_target+Print_Num-3;
	double Buck_HSILIM_Final[PMU_MAX][SITE_MAX]={0};

	double Buck_HSILIM_Acc[PMU_MAX][SITE_MAX]={0};

	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1

	Reg[0x2008].write(0x04);

	Fvi16.CH[GPIO1].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//Fvi16.CH[LPM].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Relay.BIT[GP_Buck_SW_Relay[nTILE]].set();
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,1,-1,1700,-1700);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);


		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xef&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xf7&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
		if(QC_Flag==0)
		{		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x00|(0x0f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);	
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x20);
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		}
	}
	delayms(2);
	/*
	for(i=0;i<16;i++)
	{
		nTILE=1;	
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		if(QC_Flag==0)
		{		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=i<<4|(0x0f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);	
		}
		if(PMU_ACOT_MODE[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x11);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.3*Buck_HSILIM_Target[nTILE]);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
		}
		if(PMU_MultiP_Mode[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9d);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.3*Buck_HSILIM_Target[nTILE]);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
		}	
		delayms(2);
		sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
			-0.6*Buck_HSILIM_Target[nTILE],-1.4*Buck_HSILIM_Target[nTILE],-30,GREATER_THAN,1.5,global_temp,20);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Trim_Code_Value[i]=-global_temp[0];
	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
   */
	if(QC_Flag==1)
	{
		//========Measure pre value=========//
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
			}
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x11);
			}
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9d);
			}
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.2*Buck_HSILIM_Target[nTILE]);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-(1+1e-2*Buck_ILIM_Trim_LowLi[6+8*nTILE])*Buck_HSILIM_Target[nTILE],-(1+1e-2*Buck_ILIM_Trim_HighLi[6+8*nTILE])*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20,0,10,10);
			
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			}
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}					
		}
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={150,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=7,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}};
		int ratio_queue_array[PMU_MAX][16] = {  {	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7}};
		unsigned char code_queue_new[PMU_MAX][16]={0};
		bool table_need_from_max_to_min=false;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x01;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=0.05*Buck_HSILIM_Target[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Target[nTILE];	
			}
		}
		/*
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		*/
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			//nTILE=1;
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
			}
			//========Measure pre value=========//
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x11);
			}
			if(PMU_MultiP_Mode[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9d);
			}			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x0f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);	
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.975*Buck_HSILIM_Target[nTILE]);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
			sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
				RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
				Multi_Global_trimcode[nTILE],  FindToggle, 100); 
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);
            
			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Target[nTILE]);
			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.7*Buck_HSILIM_Target[nTILE]);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-0.9*Buck_HSILIM_Target[nTILE],-1.1*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,100);
			
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			

			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}
		delayms(1);	
		//=========================================retrim===========================================//
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].set();
						delayms(2);
					}
					if(PMU_ACOT_MODE[nTILE])
					{
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x11);
					}
					if(PMU_MultiP_Mode[nTILE])
					{
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9d);
					}					
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.8*Buck_HSILIM_Target[nTILE]);
					delayms(1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
					delayms(1);
					sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
						-0.8*Buck_HSILIM_Target[nTILE],-1.2*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
					
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
					}
					if(PMU_ACOT_MODE[nTILE])
					{	
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
					}
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].clr();
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
					break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_HSILIM_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_HSILIM_Target[nTILE])/Buck_HSILIM_Target[nTILE];
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_HSILIM_Final[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
			Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Target[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_HSILIM_Acc[nTILE][nSiteIndex]);		
		}
	}		
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
	}
	//************Buck_HSILIM**********

	//************Buck_HSILIM_Warn**********
	TestNO_target=42;
	Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_HSILIM_Warn_Acc[PMU_MAX][SITE_MAX]={0};	
	/*
	for(i=0;i<16;i++)
	{
		nTILE=4;

		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].read(readback);
		readback[0]=(i<<4)|(0x0f&readback[0]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x05].write(readback);	

		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}	
		//========Measure pre value=========//
		//Buck_ILIMIT_Instd
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x3a);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(2);
		delayms(2);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(global_temp);	

		Trim_Code_Value[i]=2/global_temp[0];
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(2);
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
	*/
	if(QC_Flag==0)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x00|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	
			}
		}		
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9b);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Warn_Target[nTILE]);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					//-0.8*Buck_HSILIM_Warn_Target[nTILE],-1.2*Buck_HSILIM_Warn_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);	
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-(1e-2*Buck_ILIM_Trim_LowLi[46+8*nTILE])*Buck_HSILIM_Target[nTILE],-(1e-2*Buck_ILIM_Trim_HighLi[46+8*nTILE])*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
			
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}
		}	
	}
	if(QC_Flag==1)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].set();
				}
				delayms(2);
				//========Measure pre value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x12);
				delayms(2);
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Warn_Target[nTILE]);
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
				//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					//-0.8*Buck_HSILIM_Warn_Target[nTILE],-1.2*Buck_HSILIM_Warn_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-(1e-2*Buck_ILIM_Trim_LowLi[46+8*nTILE])*Buck_HSILIM_Target[nTILE],-(1e-2*Buck_ILIM_Trim_HighLi[46+8*nTILE])*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20,GNG_Flag);	
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].clr();
				}
			}
		}
	}
	delayms(2);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8}};
		int ratio_queue_array[PMU_MAX][16] = {  {	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7}};
		unsigned char code_queue_new[PMU_MAX][16]={0};
		bool table_need_from_max_to_min=false;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x04;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=0.05*Buck_HSILIM_Warn_Target[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Warn_Target[nTILE];	
			}
		}
		/*
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		*/
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{

				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].set();
				}

				//========Measure pre value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x12);

				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x07|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	

				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1.0*Buck_HSILIM_Warn_Target[nTILE]);
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
				delayms(1);
				sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
					RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
					Multi_Global_trimcode[nTILE],  FindToggle, 20); 
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);	
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Warn_Target[nTILE]);
				delayms(1);
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.7*Buck_HSILIM_Warn_Target[nTILE]);
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		

				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-0.8*Buck_HSILIM_Warn_Target[nTILE],-1.2*Buck_HSILIM_Warn_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
        			
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);


				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}

				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].clr();
				}
			}
		}	
		
		//=========================================retrim===========================================//
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].set();
						}
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x12);					
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.8*Buck_HSILIM_Warn_Target[nTILE]);
						delayms(1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
						delayms(1);									
						sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
							-0.7*Buck_HSILIM_Warn_Target[nTILE],-1.3*Buck_HSILIM_Warn_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
											
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);					
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);

						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
						}
						
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].clr();
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_HSILIM_Warn_Acc[nTILE][nSiteIndex]=1e2*Multi_global_post[nTILE][nSiteIndex]/Buck_HSILIM_Final[nTILE][nSiteIndex];
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		//Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		//Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_HSILIM_Warn_Acc[nTILE][nSiteIndex]);		
			}
		}
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Warn_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_HSILIM_Warn_Acc[nTILE][nSiteIndex]);		
			}
		}
	}

	delayms(2);
	//************Buck_HSILIM_Warn**********
	//************Buck_HSILIM_Shutdown**********
	TestNO_target=82;
	Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_HSILIM_Shutdown_Acc[PMU_MAX][SITE_MAX]={0};	
	/*
	for(i=0;i<16;i++)
	{
		nTILE=4;

		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
			delayms(2);
		}
		//========Measure pre value=========//
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(i<<4)|(0x0f&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	

		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x13);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Shutdown_Target[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(1);
		sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
			-0.5*Buck_HSILIM_Shutdown_Target[nTILE],-1.5*Buck_HSILIM_Shutdown_Target[nTILE],-20,GREATER_THAN,1.5,global_temp,20);	
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);

		Trim_Code_Value[i]=-global_temp[0];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
	*/
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x9c);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.8*Buck_HSILIM_Shutdown_Target[nTILE]);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
			delayms(1);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-(1e-2*Buck_ILIM_Trim_LowLi[86+8*nTILE])*Buck_HSILIM_Target[nTILE],-(1e-2*Buck_ILIM_Trim_HighLi[86+8*nTILE])*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20,GNG_Flag);	
			
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}
		}
	}
	if(QC_Flag==1)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].set();
					delayms(2);
				}
				//========Measure pre value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x13);
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.8*Buck_HSILIM_Shutdown_Target[nTILE]);
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
				delayms(1);
				//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					//-0.9*Buck_HSILIM_Shutdown_Target[nTILE],-1.1*Buck_HSILIM_Shutdown_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);	
				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-(1e-2*Buck_ILIM_Trim_LowLi[86+8*nTILE])*Buck_HSILIM_Target[nTILE],-(1e-2*Buck_ILIM_Trim_HighLi[86+8*nTILE])*Buck_HSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20,GNG_Flag);	

				
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].clr();
				}
			}
		}
	}	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=7,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={   {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
														{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}};
		int ratio_queue_array[PMU_MAX][16] = {  {	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7},
												{	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,	0,	1,	2,	3,	4,	5,	6,	7}};
		unsigned char code_queue_new[PMU_MAX][16]={0};
		bool table_need_from_max_to_min=false;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x04;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=0.03*Buck_HSILIM_Shutdown_Target[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Shutdown_Target[nTILE];	
			}
		}
		/*
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		*/
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].set();
				}

				//========Measure pre value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x13);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0x0f&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	

				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-Buck_HSILIM_Shutdown_Target[nTILE]);
				delayms(3);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
				delayms(1);
				sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
					RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
					Multi_Global_trimcode[nTILE],  FindToggle, 20); 

				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);	
				delayms(2);
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Shutdown_Target[nTILE]);	
				delayms(1);
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.7*Buck_HSILIM_Shutdown_Target[nTILE]);	
				delayms(1);

				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
						
				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-0.9*Buck_HSILIM_Shutdown_Target[nTILE],-1.1*Buck_HSILIM_Shutdown_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
        			
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);

				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}

				if(nTILE==4)
				{
					Relay.BIT[K41_FQVI5].clr();
				}
			}
		}		
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].set();
						}
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);

						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x13);					
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
					
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);	
						delayms(2);
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_HSILIM_Shutdown_Target[nTILE]);	
						delayms(1);
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.7*Buck_HSILIM_Shutdown_Target[nTILE]);	
						delayms(1);

						sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
							-0.9*Buck_HSILIM_Shutdown_Target[nTILE],-1.1*Buck_HSILIM_Shutdown_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
											
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);					
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);

						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
						}
						
						if(nTILE==4)
						{
							Relay.BIT[K41_FQVI5].clr();
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		//Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		//Fvi45.CH[GP_FB_Buck[nTILE]].open();
		//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
	}
	/*
	//Buck5 bug - can not mux shutdown trigger to GPIO on AZ silicon

	Reg[0x2084].read(readback);
	FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x0f&readback[nSiteIndex];
	Reg[0x2084].write(readback);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		Reg[0x2084].Trimmed[nSiteIndex]=Reg[0x2084].Trimmed[nSiteIndex]&0x0f;	
		Multi_Global_trimcode[4][nSiteIndex]=0;
	}
	*/
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_HSILIM_Shutdown_Acc[nTILE][nSiteIndex]=1e2*Multi_global_post[nTILE][nSiteIndex]/Buck_HSILIM_Final[nTILE][nSiteIndex];	
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_HSILIM_Shutdown_Acc[nTILE][nSiteIndex]);		
			}
		}
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_HSILIM_Shutdown_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_HSILIM_Shutdown_Acc[nTILE][nSiteIndex]);		
			}
		}
	}
	delayms(2);
	//************Buck_HSILIM_Shut**********	
	//************Buck_LSILIMIT**********
	TestNO_target=122;
	Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_LSILIM_Acc[PMU_MAX][SITE_MAX]={0};	
	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		if(QC_Flag==0&&nTILE!=1)
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0x87&readback[nSiteIndex]);
		if(QC_Flag==1&&nTILE!=1)
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0xf7&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);
		if(PMU_MultiP_Mode[nTILE])//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
	/*
	for(i=0;i<8;i++)
	{
		nTILE=4;	
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(i<<4)|(0x87&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);

		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.5*Buck_LSILIM_Target[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(1);	
		sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
			-0.5*Buck_LSILIM_Target[nTILE],-1.5*Buck_LSILIM_Target[nTILE],-20,GREATER_THAN,1.5,global_temp,20);

		Trim_Code_Value[i]=-global_temp[0];

		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(5);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);		
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
	*/
	if(QC_Flag==1)
	{		
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
				delayms(2);
			}
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.2*Buck_LSILIM_Target[nTILE]);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
			delayms(1);	
			//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				//-0.85*Buck_LSILIM_Target[nTILE],-1.15*Buck_LSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-(1+1e-2*Buck_ILIM_Trim_LowLi[126+8*nTILE])*Buck_LSILIM_Target[nTILE],-(1+1e-2*Buck_ILIM_Trim_HighLi[126+8*nTILE])*Buck_LSILIM_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20,GNG_Flag);	

			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}
	}
	delayms(2);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=6,global_BitLow=4,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=8;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={8,8,8,8,8};//--> set table size
		unsigned char code_queue_array[PMU_MAX][8]={    {4,5,6,7,0,1,2,3},
														{4,5,6,7,0,1,2,3},
														{4,5,6,7,0,1,2,3},
														{4,5,6,7,0,1,2,3},
														{4,5,6,7,0,1,2,3}};
		int ratio_queue_array[PMU_MAX][8] = {	{-4,-3,-2,-1,0,1,2,3},
												{-4,-3,-2,-1,0,1,2,3},
												{-4,-3,-2,-1,0,1,2,3},
												{-4,-3,-2,-1,0,1,2,3},
												{-4,-3,-2,-1,0,1,2,3}};	
		unsigned char code_queue_new[PMU_MAX][8]={0};
		bool table_need_from_max_to_min=true;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x08;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=0.1*Buck_LSILIM_Target[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_LSILIM_Target[nTILE];	
			}
		}
		/*
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		*/
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
			}

			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa1);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
			if(nTILE!=1)
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0xb0|(0x87&readback[nSiteIndex]);
			if(nTILE==1)
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x30|(0x87&readback[nSiteIndex]);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);	

			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1.05*Buck_LSILIM_Target[nTILE]);
			delayms(1);
			if(nTILE==4)
			{
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1.0*Buck_LSILIM_Target[nTILE]);
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);

			sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
				RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
				Multi_Global_trimcode[nTILE],  FindToggle, 100); 
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);
			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.2*Buck_LSILIM_Target[nTILE]);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				-0.85*Buck_LSILIM_Target[nTILE],-1.15*Buck_LSILIM_Target[nTILE],-10,GREATER_THAN,1.5,global_temp,20);	
        		
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
			}

			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}	
		//=========================================retrim===========================================//	Disable as AS silicon issue, trim range is not enough
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].set();
					}
					Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
					FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0xf7&readback[nSiteIndex]);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);	
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa1);					
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-0.2*Buck_LSILIM_Target[nTILE]);
					delayms(1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
					delayms(1);									
					sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
						-0.8*Buck_LSILIM_Target[nTILE],-1.2*Buck_LSILIM_Target[nTILE],-10,GREATER_THAN,1.5,global_temp,20);	
										
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);					
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
					if(PMU_ACOT_MODE[nTILE])
					{
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
					}
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
					}
					
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].clr();
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		//Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		//Fvi45.CH[GP_FB_Buck[nTILE]].open();
		//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_LSILIM_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_LSILIM_Target[nTILE])/Buck_LSILIM_Target[nTILE];
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=Buck_LSILIM_Target[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_LSILIM_Acc[nTILE][nSiteIndex]);		
		}
	}
	delayms(2);
	//************Buck_LSILIMIT**********
	//************Buck_ZC**********
	double Buck_Ind[PMU_MAX]={0.47,0.47,0.47,0.47,1.0};
	double ZC_Delay_Target[PMU_MAX]={110,110,90,95,85};

	TestNO_target=167;
	Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_ZC_Acc[PMU_MAX][SITE_MAX]={0};	
	double ZC_Delay[PMU_MAX][SITE_MAX]={0};	
	unsigned char ZC_Trim_Byte_value[PMU_MAX][SITE_MAX]={0};

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);	
	}
	delayms(1);
	//BUCKS ZC Delay
	/*
	int Data_readback[219]={0}; 
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if(cmi_mask[i].reg_addr==0x10f2) 
		{
			Reg[0x1016].set_clr_inv(BIT7,BIT6);
			delayms(1);
			Fvi45.CH[VBYP].fv(0);
			delayms(1);
		}
		Reg[cmi_mask[i].reg_addr].read(readback);	
		Data_readback[i]=readback[0];

		if(cmi_mask[i].reg_addr==0x10f7)
		{
			Fvi45.CH[VBYP].fv(VIN_SET+1.5);
			delayms(1);
		}
	}
	*/
	if(1)
	{
		Relay.BIT[K47_TMU].set();
		Relay.BIT[K21_GPIO1_TMUA].set(); // CLOSE AMUX_BUF
		delayms(1);
		TMU_CH1.stallset(Stall7V,TMU1uS,1);
		TMU_CH1.switchset(STARTtoA+STOPtoA);
		TMU_CH1.startset(0.5,UP);
		TMU_CH1.stopset(2.0,DOWN);
		TMU_CH1.arm(1);
		delayms(2);	
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,1,-1,1200,-1200);
			Fqvi.CH[GP_SW_Buck[nTILE]].open();
			Reg[GP_User_Buck_Addr[nTILE]+0x00].write(0x0f);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x3f&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x08);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x98);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x88);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(ZC_Trim_Byte_value[nTILE]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x7f&(0x1f|readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);

			//Reg[0x2008].write(0x04);// already set in previous item
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			//FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0xdf&(readback[nSiteIndex]);
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
			delayms(1);
			measure_TMUCH1();
			FOR_EACH_SITE(nSiteIndex)
			{
				ZC_Delay[nTILE][nSiteIndex]=1e6*global_temp[nSiteIndex];					
			}
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write_CMI();
			//Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write_CMI();
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)//Re calculate the ZC target
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(ZC_Trim_Byte_value[nTILE]);
			//Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(ZC_Delay[nTILE][nSiteIndex]-ZC_Delay_Target[nTILE])>30)
						ZC_Delay[nTILE][nSiteIndex]=ZC_Delay_Target[nTILE];
				Buck_ZC_Target[nTILE][nSiteIndex]=-(ZC_Delay[nTILE][nSiteIndex]+15.0)*Buck_Vset[nTILE]/Buck_Ind[nTILE];
			}
		}
		Relay.BIT[K47_TMU].clr();
		Relay.BIT[K21_GPIO1_TMUA].clr();	
	}
	if(Buck_Ton_ZC_Target_Print)
	{
		printf("\n----------------------Bucks_ZC_set-----------------------------");
		FOR_EACH_SITE(nSiteIndex)
		{
			printf("\n Buck1_ZC_Target=%4.1fmA", Buck_ZC_Target[0][nSiteIndex]); 
			printf("\n Buck2_ZC_Target=%4.1fmA", Buck_ZC_Target[1][nSiteIndex]); 
			printf("\n Buck3_ZC_Target=%4.1fmA", Buck_ZC_Target[2][nSiteIndex]); 
			printf("\n Buck4_ZC_Target=%4.1fmA", Buck_ZC_Target[3][nSiteIndex]); 
			printf("\n Buck5_ZC_Target=%4.1fmA", Buck_ZC_Target[4][nSiteIndex]); 
		}
		printf("\n--------------------------Target set Done----------------------------------");	
		printf("\n");
		Buck_Ton_ZC_Target_Print=false;
	}

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			//Buck_ZC_Target[nTILE][0]=-((19*pow(Buck_Vset[nTILE],2)-117*Buck_Vset[nTILE])+263)*Buck_Vset[nTILE]/Buck_Ind[nTILE];
			pFunction->LogResult(NSITEINDEX, 162+nTILE, ZC_Delay[nTILE][nSiteIndex]);
		}
	}
	delayms(2);
	/*
	for(i=0;i<32;i++)
	{
		nTILE=2;	
		//========Measure pre value=========//

		
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
			delayms(2);
		}
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0xf7&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);	
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x24);
		}
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=i|(0x20&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);	

		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa2);
		}
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(2.5*Buck_ZC_Target[nTILE][0]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
		delayms(2);
		sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
			2.0*Buck_ZC_Target[nTILE][0],-1.0*Buck_ZC_Target[nTILE][0],50,GREATER_THAN,1.5,global_temp,20);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Trim_Code_Value[i]=global_temp[0];

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		}			
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}
	}
    */
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{

		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		if(nTILE!=1)
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x80|(0xf7&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);	
	}

	if(QC_Flag==1)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
				delayms(2);
			}
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x24);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]-200);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(2);			
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0],Buck_ILIM_Trim_HighLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0],10,GREATER_THAN,1.5,global_temp,20,GNG_Flag,10,10);
		
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
			}
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}
	}
	delayms(2);
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		double ZC_global_LSB[PMU_MAX]={180,160,160,150,60};//mA
		global_BitHigh=4,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=32;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={32,32,32,32,32};//--> set table size
		unsigned char code_queue_array[PMU_MAX][32]={   {31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},
														{31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},
														{31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},
														{31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},
														{31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0}};//Code 31-0, current -900mA to 900mA
		int ratio_queue_array[PMU_MAX][32] = {	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
												{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
												{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
												{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
												{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31}};	
		unsigned char code_queue_new[PMU_MAX][32]={0};
		bool table_need_from_max_to_min=true;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x09;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=ZC_global_LSB[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_ZC_Target[nTILE][nSiteIndex];	
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			//method 2: =========sweep trim code table follow Test matrix==========//
			//--set condition and force target value by device--//	
			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].set();
			}
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x24);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x00|(0x20&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);	
			delayms(2);
			//FOR_EACH_SITE(nSiteIndex) 	Fqvi.CH[GP_SW_Buck[nTILE]].fi(Buck_ZC_Target[nTILE][nSiteIndex],nSiteIndex);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(1.0*Buck_ZC_Target[nTILE][0]);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
			sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
				RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
				Multi_Global_trimcode[nTILE],  FindToggle, 20); 

			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);
			delayms(1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x3f&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]-200);
			delayms(2);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
			delayms(1);
			//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				//3.0*Buck_ZC_Target[nTILE][0],-0.5*Buck_ZC_Target[nTILE][0],10,GREATER_THAN,1.5,global_temp,20);	
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]-200,Buck_ILIM_Trim_HighLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]+100,10,GREATER_THAN,1.5,global_temp,20);
		
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
			}

			if(nTILE==4)
			{
				Relay.BIT[K41_FQVI5].clr();
			}
		}
		
		//=========================================retrim===========================================//
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].set();
						delayms(2);
					}
					Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
					FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x3f&readback[nSiteIndex];
					Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);

					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x24);
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]-200);
					delayms(2);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
					delayms(1);
					//sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
						//3.0*Buck_ZC_Target[nTILE][0],-0.5*Buck_ZC_Target[nTILE][0],10,GREATER_THAN,1.5,global_temp,20);
					sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
						Buck_ILIM_Trim_LowLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]-200,Buck_ILIM_Trim_HighLi[171+8*nTILE]+Buck_ZC_Target[nTILE][0]+100,10,GREATER_THAN,1.5,global_temp,20);
					
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);					
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=global_temp[nSiteIndex];
					}
					if(nTILE==4)
					{
						Relay.BIT[K41_FQVI5].clr();
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}

	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			//Buck_ZC_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_ZC_Target[nTILE][nSiteIndex])/Buck_ZC_Target[nTILE][nSiteIndex];//%
			Buck_ZC_Acc[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_ZC_Target[nTILE][nSiteIndex];//mA
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fqvi.CH[GP_SW_Buck[nTILE]].open();
	}
	//==============power off=================//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=Buck_ZC_Target[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_ZC_Acc[nTILE][nSiteIndex]);		
		}
	}
	delayms(2);
	//************Buck_ZC**********
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_POK
STT_TESTFUNC Trim_Bucks_POK(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_POK_params* ours)
{
	//FTP Limit value
	double Buck_POK_Trim_HighLi[201]={0},Buck_POK_Trim_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Buck_POK_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_POK_Trim_LowLi[i+1]);//item 1-48
	}
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_POK**********
	TestNO_target=2;
	int Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_POK_Target[PMU_MAX]={0},Buck_POK_HYS_Target[PMU_MAX]={0};
	double Buck_POK_Acc[PMU_MAX][SITE_MAX]={0};	

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Buck_POK_Target[nTILE]=94+(2*(Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI>>7)&0x01);
		Buck_POK_HYS_Target[nTILE]=2+(2*(Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI>>6)&0x01);
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);//Need to set Buck LSILIM_SD=0 for the POK/OV/VTH test
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x7f&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);

		if(QC_Flag==0)
		{		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x02].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x20|(0x1f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x02].write(readback);
		}
	}
	//Sweep trim table

	/*
	for(i=0;i<8;i++)
	{
		nTILE=4;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xcf&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x02].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(i<<5)|(0x1f&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x02].write(readback);

		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);
		delayms(1);
		Reg[GP_User_Buck_Addr[nTILE]+0x00].set_clr_inv(BIT3|BIT2|BIT1|BIT0);		
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa8);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			0.85*Buck_Vset[nTILE],1.05*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
		Trim_Code_Value[i]=1e2*global_temp[0]/Buck_Vset[nTILE];
		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
	}
	//Sweep trim table
	*/
	delayms(5);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		//FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xcf&readback[nSiteIndex];
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);
		delayms(1);
		//========Measure pre value=========//
		Reg[GP_User_Buck_Addr[nTILE]+0x00].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
		
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa8);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(1);
		if(QC_Flag==0)
		{
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				0.85*Buck_Vset[nTILE],1.00*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
		}
		if(QC_Flag==1)
		{
			if(nTILE<2.5)
			{
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					(1e-2*(Buck_POK_Trim_LowLi[6+8*nTILE]+Buck_POK_Target[nTILE]))*Buck_Vset[nTILE],(1e-2*(Buck_POK_Trim_HighLi[6+8*nTILE]+Buck_POK_Target[nTILE]))*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,20);
			}
			if(nTILE>2.5)
			{
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					(1e-2*(Buck_POK_Trim_LowLi[6+8*nTILE]+Buck_POK_Target[nTILE]))*Buck_Vset[nTILE],(1e-2*(Buck_POK_Trim_HighLi[6+8*nTILE]+Buck_POK_Target[nTILE]))*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,20);
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Pre[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
	}
	delayms(2);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		
		//=========assign trim table and trim address==========//
		double POK_global_LSB[PMU_MAX]={1.2,1.2,1.2,1.5,0.9};//%
		global_BitHigh=7,global_BitLow=5,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=8;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={8,8,8,8,8};//--> set table size
		unsigned char code_queue_array[PMU_MAX][8]={    {3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4}};
		int ratio_queue_array[PMU_MAX][8] = {	{-2,-1,0,1,2,3,4,5},
												{-2,-1,0,1,2,3,4,5},
												{-2,-1,0,1,2,3,4,5},
												{-2,-1,0,1,2,3,4,5},
												{-2,-1,0,1,2,3,4,5}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x02;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=POK_global_LSB[nTILE];//%
				Multi_global_Target[nTILE][nSiteIndex]=Buck_POK_Target[nTILE];	
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			delayms(2);
			//========Measure post value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa8);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.85*Buck_Vset[nTILE]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
			delayms(1);
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				0.9*Buck_Vset[nTILE],1.00*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
			delayms(1);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.8*Buck_Vset[nTILE]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			}
		}		
		//=========================================retrim===========================================//
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					delayms(1);
					//========Measure pre value=========//
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa8);
					Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.9*Buck_Vset[nTILE]);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
					delayms(1);
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						0.9*Buck_Vset[nTILE],1.00*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
					}
					Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.9*Buck_Vset[nTILE]);				
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
					delayms(2);
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
		
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_Acc[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_POK_Target[nTILE];
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=Buck_POK_Target[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_POK_Acc[nTILE][nSiteIndex]);		
		}
	}
	delayms(2);
	//************Buck_POK**********
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_OV
STT_TESTFUNC Trim_Bucks_OV(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_OV_params* ours)
{
	//FTP Limit value
	double Buck_OV_Trim_HighLi[201]={0},Buck_OV_Trim_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Buck_OV_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_OV_Trim_LowLi[i+1]);//item 1-48
	}
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_OV**********
	TestNO_target=2;
	int Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_OV_Target[PMU_MAX]={114,114,114,114,114},Buck_OV_HYS_Target[PMU_MAX]={4,4,4,4,4};
	double Buck_OV_Acc[PMU_MAX][SITE_MAX]={0};	

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(QC_Flag==0)
		{		
			Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x00|(0xf8&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);
		}
	}
	//Sweep trim table
	/*
	nTILE=0;

	Reg[GP_User_Buck_Addr[nTILE]+0x00].set_clr_inv(BIT3|BIT2|BIT1|BIT0);	
	Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2f);
	for(i=0;i<8;i++)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=i|(0xf8&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE]);
		delayms(2);	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(1);	
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1.05*Buck_Vset[nTILE],1.25*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
		delayms(1);
		Trim_Code_Value[i]=1e2*global_temp[0]/Buck_Vset[nTILE];
	}
	Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
	
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);	
	*/
	//Sweep trim table

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		delayms(2);
		//========Measure pre value=========//
		Reg[GP_User_Buck_Addr[nTILE]+0x00].set_clr_inv(BIT3|BIT2|BIT1|BIT0);

		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2f);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE]);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
		delayms(2);
		if(QC_Flag==0)
		{
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				1.05*Buck_Vset[nTILE],1.25*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
		}
		if(QC_Flag==1)
		{
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				1e-2*(Buck_OV_Trim_LowLi[6+8*nTILE]+Buck_OV_Target[nTILE])*Buck_Vset[nTILE],1e-2*(Buck_OV_Trim_HighLi[6+8*nTILE]+Buck_OV_Target[nTILE])*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,40,20);
			//sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				//1.05*Buck_Vset[nTILE],1.25*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
		}
		delayms(1);
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Pre[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
	}
	delayms(2);	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=2,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=8;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={8,8,8,8,8};//--> set table size
		unsigned char code_queue_array[PMU_MAX][8]={    {3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4},
														{3,2,1,0,7,6,5,4}};
		int ratio_queue_array[PMU_MAX][8] = {	{-3,-2,-1,0,1,2,3,4},
												{-3,-2,-1,0,1,2,3,4},
												{-3,-2,-1,0,1,2,3,4},
												{-3,-2,-1,0,1,2,3,4},
												{-3,-2,-1,0,1,2,3,4}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x08;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=1.8;//%
				Multi_global_Target[nTILE][nSiteIndex]=Buck_OV_Target[nTILE];	
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
					Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			delayms(2);
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2f);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
			delayms(2);			
			sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				1.1*Buck_Vset[nTILE],1.2*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
			delayms(1);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			}
		}		
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
					Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
					if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
						Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
						&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
					{
						Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
						//---write 2nd trim code if has
						//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
				{
					//========Measure pre value=========//
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2f);
					Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
					delayms(2);
					sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
						1.05*Buck_Vset[nTILE],1.2*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20);
					delayms(1);
					Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);				
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
					Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);					
					FOR_EACH_SITE(nSiteIndex)
					{
						if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
						Multi_global_post[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
				{
					store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
					store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
				}
				if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
					Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
				{		
					//---print internal_post1 and internal post2 and retrim count------------//
					pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
					pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
					pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
				}
			}
		}
	}  
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OV_Acc[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_OV_Target[nTILE];
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Multi_global_Target[nTILE][nSiteIndex]=Buck_OV_Target[nTILE];
			pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
			if(QC_Flag==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
			}
			pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_OV_Acc[nTILE][nSiteIndex]);		
		}
	}
	delayms(2);
	//************Buck_OV**********

	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Bucks_VTH_HDUTY
STT_TESTFUNC Trim_Bucks_VTH_HDUTY(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Bucks_VTH_HDUTY_params* ours)
{
	//FTP Limit value
	double Buck_VTH_Trim_HighLi[201]={0},Buck_VTH_Trim_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Buck_VTH_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_VTH_Trim_LowLi[i+1]);//item 1-48
	}
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************Buck_VTH_HDUTY**********
	TestNO_target=2;
	int Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_VTH_HDUTY_Target[PMU_MAX]={125,125,125,125,125};
	double Buck_VTH_HDUTY_Acc[PMU_MAX][SITE_MAX]={0};	

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x40|readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

			if(QC_Flag==0)
			{		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].read(readback);
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x00|(0xf0&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x01].write(readback);
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			delayms(2);
			//========Measure pre value=========//
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa9);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.7*VIN_SET);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
			delayms(2);
			if(QC_Flag==0)
			{
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					0.75*VIN_SET,0.9*VIN_SET,0.002,GREATER_THAN,1.5,global_temp,20);
			}
			if(QC_Flag==1)
			{
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					1e2/(Buck_VTH_HDUTY_Target[nTILE]+Buck_VTH_Trim_HighLi[6+nTILE*8])*VIN_SET,1e2/(Buck_VTH_HDUTY_Target[nTILE]+Buck_VTH_Trim_LowLi[6+nTILE*8])*VIN_SET,0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,40,40);
			}			
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);

			delayms(2);
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.0*Buck_Vset[nTILE]);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=1e2*VIN_SET/global_temp[nSiteIndex];
				Multi_global_post[nTILE][nSiteIndex]=1e2*VIN_SET/global_temp[nSiteIndex];		
			}
		}
	}
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		//double ILIMIT_global_LSB[PMU_MAX]={0.52,0.52,0.52,0.52,0.52};//uA
		global_BitHigh=3,global_BitLow=0,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=16;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={16,16,16,16,16};//--> set table size
		unsigned char code_queue_array[PMU_MAX][16]={ {7,6,5,4,3,2,1,0,15,14,13	,12	,11	,10	,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11	,10	,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11	,10	,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11	,10	,9,8},
														{7,6,5,4,3,2,1,0,15,14,13,12,11	,10	,9,8}};
		int ratio_queue_array[PMU_MAX][16] = {	{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8},
												{-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x01;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=1.5;//%
					Multi_global_Target[nTILE][nSiteIndex]=Buck_VTH_HDUTY_Target[nTILE];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				delayms(2);
				//========Measure post value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa9);
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.7*VIN_SET);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
				delayms(2);		
				sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
					0.77*VIN_SET,0.85*VIN_SET,0.002,GREATER_THAN,1.5,global_temp,20);
				delayms(2);
				Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.7*VIN_SET);
			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
				
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_post[nTILE][nSiteIndex]=1e2*VIN_SET/global_temp[nSiteIndex];		
				}
			}
		}		
		//=========================================retrim===========================================//	
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_ACOT_MODE[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure pre value=========//
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa9);
						Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.7*VIN_SET);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10	
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
						delayms(2);				
						sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
							0.75*VIN_SET,0.85*VIN_SET,0.002,GREATER_THAN,1.5,global_temp,20);
						Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.7*VIN_SET);
					
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
						Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
							Multi_global_post[nTILE][nSiteIndex]=1e2*VIN_SET/global_temp[nSiteIndex];		
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}		
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_VTH_HDUTY_Acc[nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex]-Buck_VTH_HDUTY_Target[nTILE];
		}
	}
	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
			Fvi45.CH[GP_FB_Buck[nTILE]].open();	
		}
	}
	Fvi16.CH[GPIO1]      .open();

	//==============power off=================//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_VTH_HDUTY_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_VTH_HDUTY_Acc[nTILE][nSiteIndex]);		
			}
		}
	}
	delayms(2);
	//************Buck_VTH_HDUTY**********
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Trim_Buck12_Ri
STT_TESTFUNC Trim_Buck12_Ri(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Buck12_Ri_params* ours)
{

	double Buck_Ri_Target[PMU_MAX]={0.2/(1+(0x01&(Reg[0x2057].CMI>>5))),0.2/(1+(0x01&(Reg[0x2057].CMI>>5)))};
	double Buck_Ri_Acc[PMU_MAX][SITE_MAX]={0};

	double SW_Measure[2][SITE_MAX]={0};
	double SW_force[2]={600,1200};

	int Print_Num=8;
	TestNO_target=2;
	print_index=TestNO_target+Print_Num-3;
	
	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Reg[0x1019].write(0x08);
		Reg[0x101a].write(0x80);
		Reg[0x101b].write(_0x00);
		Reg[0x101c].write(_0x00);//Config EXT_PG pin
		
		Reg[0x2008].write(0x08);
		Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);

		Factory_Mode_Check();
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].set();
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,1,-1,1500,-1500);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1f);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
			if(QC_Flag==0)
			{
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x06|(0xf0&readback[nSiteIndex]);
			}
			if(QC_Flag==1)
			{
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfe&readback[nSiteIndex];
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(Reg[GP_Factory_Buck_Addr[nTILE]+0x07].CMI&0x20)|(0x5f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(Reg[GP_Factory_Buck_Addr[nTILE]+0x07].CMI&0x20)|(0x5f&readback[nSiteIndex]);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];
			Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);


			//Force I=500mA
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[0]);
			delayms(5);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
			Fvi16.CH[EXT_PG].mv(V1);
			//Force I=1000mA

			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[1]);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
			Fvi16.CH[EXT_PG].mv(V2);
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Pre[nTILE][nSiteIndex]=1e3*(V2[nSiteIndex]-V1[nSiteIndex])/(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex]);
				Multi_global_post[nTILE][nSiteIndex]=Multi_global_Pre[nTILE][nSiteIndex];

			}
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);

			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
			delayms(2);
		}
	}

	
	/*
	nTILE=1;
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1f);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x20|(0x7f&readback[nSiteIndex]);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
	for(i=0;i<8;i++)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(i<<1)|(0xf0&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);
		

		//Force I=500mA
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[0]);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
		Fvi16.CH[EXT_PG].mv(V1);
		//Force I=1000mA
		
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[1]);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
		Fvi16.CH[EXT_PG].mv(V2);

		Trim_Code_Value[i]=1e3*(V2[0]-V1[0])/(SW_Measure[0][0]-SW_Measure[1][0]);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(1);
	}
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
	Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
	delayms(2);
	*/
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_BitHigh=3,global_BitLow=1,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=8;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={8,8,8,8,8};//--> set table size
		unsigned char code_queue_array[PMU_MAX][8]={   {0,1,2,3,4,5,6,7},
														{0,1,2,3,4,5,6,7},
														{0,1,2,3,4,5,6,7},
														{0,1,2,3,4,5,6,7},
														{0,1,2,3,4,5,6,7}};
		int ratio_queue_array[PMU_MAX][8] = {   {-3,	-2,	-1,	0,	1,	2,	3,	4},
												{-3,	-2,	-1,	0,	1,	2,	3,	4},
												{-3,	-2,	-1,	0,	1,	2,	3,	4},
												{-3,	-2,	-1,	0,	1,	2,	3,	4},
												{-3,	-2,	-1,	0,	1,	2,	3,	4}};	
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x04;//--> set trim address
				Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
				Multi_global_BitLow[nTILE]=global_BitLow;
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_LSB[nTILE][nSiteIndex]=0.11*Buck_Ri_Target[nTILE];//
					Multi_global_Target[nTILE][nSiteIndex]=Buck_Ri_Target[nTILE];	
					Multi_Global_trimcode[nTILE][nSiteIndex]=get_trim_code_LookupTable(Multi_global_Target[nTILE][nSiteIndex],Multi_global_Pre[nTILE][nSiteIndex], Multi_global_LSB[nTILE][nSiteIndex],
						Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], ratio_queue_array[nTILE], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF);
				}
			}
		}
		if(0)
		{
			unsigned char code_queue_new[PMU_MAX][8]={0};
			bool table_need_from_max_to_min=false;
			FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
			{			
				table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			//========Measure postvalue=========//
			if(PMU_MultiP_Mode[nTILE])
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1f);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
					
					Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
					FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xfe&readback[nSiteIndex]);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);	
					
					//Force I=500mA
					delayms(1);
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[0]);
					delayms(2);
					Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
					Fvi16.CH[EXT_PG].mv(V1);
					//Force I=1000mA
					
					delayms(1);
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[1]);
					delayms(2);
					Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
					Fvi16.CH[EXT_PG].mv(V2);
					FOR_EACH_SITE(nSiteIndex)
					{
						Multi_global_post[nTILE][nSiteIndex]=1e3*(V2[nSiteIndex]-V1[nSiteIndex])/(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex]);						
					}
					Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
					
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
					Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
				}
			}
		}
		//=========================================retrim===========================================//	
		
		for(global_retrim_loop=0;global_retrim_loop<global_Trim_table_size;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						//========Measure postvalue=========//
						if(PMU_MultiP_Mode[nTILE])
						{
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x1f);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x10);
							
							Reg[GP_Factory_Buck_Addr[nTILE]+0x04].read(readback);
							FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xfe&readback[nSiteIndex]);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x04].write(readback);						
							//Force I=500mA
							delayms(1);
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[0]);
							delayms(2);
							Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[0]);
							Fvi16.CH[EXT_PG].mv(V1);
							//Force I=1000mA
							
							delayms(1);
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(-SW_force[1]);
							delayms(2);
							Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_Measure[1]);
							Fvi16.CH[EXT_PG].mv(V2);

							FOR_EACH_SITE(nSiteIndex)
							{
								if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
									Multi_global_post[nTILE][nSiteIndex]=1e3*(V2[nSiteIndex]-V1[nSiteIndex])/(SW_Measure[0][nSiteIndex]-SW_Measure[1][nSiteIndex]);						
							}
							Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
							
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
							Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
	}  
	
	if(PMU_MultiP_Mode[0]&&PMU_MultiP_Mode[1])
	{
		Reg[0x2008].set_clr_inv(BIT2|BIT0);//Reset to DMUX mode after this test
		Fvi16.CH[EXT_PG].open();
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Ri_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_Ri_Target[nTILE])/Buck_Ri_Target[nTILE];
			}
		}
	}
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_Ri_Target[nTILE];
				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_Ri_Acc[nTILE][nSiteIndex]);	
			}
		}
	}
	
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Trim_Buck12_IMIN
STT_TESTFUNC Trim_Buck12_IMIN(CFunction *pFunction, int nSiteNo, int nTestFlag, Trim_Buck12_IMIN_params* ours)
{
	TestNO_target=2;
	int Print_Num=8;
	print_index=TestNO_target+Print_Num-3;
	double Buck_IMIN_Acc[PMU_MAX][SITE_MAX]={0},Buck_IMIN_Target[PMU_MAX]={110,110,0,0,0};	
	Fvi16.CH[GPIO1].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
		}
	}	
	/*
	for(i=0;i<4;i++)
	{
		nTILE=1;	
		//========Measure pre value=========//
		Reg[GP_Factory_Buck_Addr[nTILE]+0x06].read(readback);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(i<<3)|(0xe7&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x06].write(readback);	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x97);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x20);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
		delayms(2);
		sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
			-50,-2.0*Buck_IMIN_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);

		Trim_Code_Value[i]=-global_temp[0];

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
	}
	*/
	if(QC_Flag==1)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX) 
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				//========Measure pre value=========//
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x97);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x20);
				
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-20);
				delayms(1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
				delayms(2);
				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-0.5*Buck_IMIN_Target[nTILE],-1.5*Buck_IMIN_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
				
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_Pre[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}			
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
			}
		}
	}
	delayms(2);
	
	//++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		double global_LSB[PMU_MAX]={25,25,0,0,0};//mA
		global_BitHigh=4,global_BitLow=3,global_ShiftBITcount=0;//--> set trim bits
		global_Trim_table_size=4;//--> set table size
		int Multi_global_Trim_table_size[PMU_MAX]={4,4,4,4,4};//--> set table size
		unsigned char code_queue_array[PMU_MAX][4]={{3,2,1,0},
													{3,2,1,0},
													{3,2,1,0},
													{3,2,1,0},
													{3,2,1,0}};
		int ratio_queue_array[PMU_MAX][4] = {	{0,1,2,3},
												{0,1,2,3},
												{0,1,2,3},
												{0,1,2,3},
												{0,1,2,3}};	
		unsigned char code_queue_new[PMU_MAX][4]={0};
		bool table_need_from_max_to_min=false;
		FOR_EACH_TILE(nTILE,PMU_MAX)//rearrangement table 
		{			
			table_code_from_max_to_min(table_need_from_max_to_min, Multi_global_Trim_table_size[nTILE],code_queue_array[nTILE], code_queue_new[nTILE]);
		}

		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Multi_global_trimReg_addr[nTILE]=GP_Factory_Buck_Addr[nTILE]+0x06;//--> set trim address
			Multi_global_BitHigh[nTILE]=global_BitHigh;//--> set trim bits
			Multi_global_BitLow[nTILE]=global_BitLow;
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_LSB[nTILE][nSiteIndex]=global_LSB[nTILE];//mA
				Multi_global_Target[nTILE][nSiteIndex]=Buck_IMIN_Target[nTILE];	
			}
		}
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			if(PMU_MultiP_Mode[nTILE])
			{
				//method 2: =========sweep trim code table follow Test matrix==========//
				//--set condition and force target value by device--//	
				//========Measure pre value=========//
				Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x97);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x20);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x06].read(readback);
				FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x18|(0xe7&readback[nSiteIndex]);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x06].write(readback);	

				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1.0*Buck_IMIN_Target[nTILE]);
				delayms(2);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		
				delayms(1);

				sweep_code(Multi_global_Trim_table_size[nTILE], Multi_global_trimReg_addr[nTILE] , code_queue_new[nTILE], Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], 
					RESOURCE_FVI16,         GPIO1,      VOLTAGE,   GREATER_THAN, 1.5,
					Multi_Global_trimcode[nTILE],  FindToggle, 100); 
				
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(-1);

				delayms(2);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10		

				sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
					-0.5*Buck_IMIN_Target[nTILE],-1.5*Buck_IMIN_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
        			
				Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
				FOR_EACH_SITE(nSiteIndex)
				{
					Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				}
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
				Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			}
		}
		
		//=========================================retrim===========================================//
		/*
		for(global_retrim_loop=0;global_retrim_loop<3;global_retrim_loop++)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					FOR_EACH_SITE(nSiteIndex)
					{	
						Multi_store_Post_value_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_global_post[nTILE][nSiteIndex];
						Multi_store_Global_trimcode_array[global_retrim_loop][nTILE][nSiteIndex]=Multi_Global_trimcode[nTILE][nSiteIndex];
						if (shift_one_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_post[nTILE][nSiteIndex], &Multi_global_table_queue[nTILE][nSiteIndex],
							Multi_global_LSB[nTILE][nSiteIndex],Multi_global_Trim_table_size[nTILE], code_queue_array[nTILE],
							&Multi_Global_trimcode[nTILE][nSiteIndex], &Multi_global_count[nTILE][nSiteIndex], &Multi_global_trim_tend[nTILE][nSiteIndex], &Multi_global_retrim_Flag[nTILE][nSiteIndex] ,
							Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE] ,DIFF, 0.5))
						{
							Multi_global_retrim_Flag[nTILE][nSiteIndex]=true;
							//---write 2nd trim code if has
							//Reg[Multi_global_trimReg_addr_2nd[nTILE]].write_Data_trimvalue_singlesite(Multi_global_BitHigh_2nd[nTILE],Multi_global_BitLow_2nd[nTILE],Multi_Global_trimcode[nTILE][nSiteIndex],global_ShiftBITcount_2nd);
						}
					}
				}
			}
			delayms(2);
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{
				if(PMU_MultiP_Mode[nTILE])
				{
					if(isOneFlagTrue(Multi_global_retrim_Flag[nTILE]))
					{
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x97);
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(-20);
						delayms(2);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1,BIT6|BIT0);//Confiure GPIO1 set:10		
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5|BIT1|BIT0,BIT6);//Confiure GPIO1 set:10	
						delayms(1);
						sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,GPIO1,VOLTAGE,
							-0.5*Buck_IMIN_Target[nTILE],-1.5*Buck_IMIN_Target[nTILE],-5,GREATER_THAN,1.5,global_temp,20);
						
						Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
						Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
						FOR_EACH_SITE(nSiteIndex)
						{
							if (Multi_global_retrim_Flag[nTILE][nSiteIndex]==1)
								Multi_global_post[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
						}
					}
				}
			}
			if(isAllFlagClr_MultiTile(PMU_MAX, Multi_global_retrim_Flag))
				break;
		}
			
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			if(PMU_MultiP_Mode[nTILE])
			{
				FOR_EACH_SITE(nSiteIndex)
				{	
					for (i=0;i<Multi_global_count[nTILE][nSiteIndex]+1;i++)
					{
						store_Post_value_array[i]=Multi_store_Post_value_array[i][nTILE][nSiteIndex];
						store_Global_trimcode_array[i]=Multi_store_Global_trimcode_array[i][nTILE][nSiteIndex];
					}
					if (search_Best_trim_code(Multi_global_Target[nTILE][nSiteIndex], Multi_global_count[nTILE][nSiteIndex], &Multi_global_post[nTILE][nSiteIndex], &Multi_Global_trimcode[nTILE][nSiteIndex], 
						Multi_global_BitHigh[nTILE], Multi_global_BitLow[nTILE], Multi_global_trimReg_addr[nTILE]))
					{		
						//---print internal_post1 and internal post2 and retrim count------------//
						pFunction->LogResult(NSITEINDEX, print_index+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]-1]);
						pFunction->LogResult(NSITEINDEX, print_index+1+Print_Num*nTILE, store_Post_value_array[Multi_global_count[nTILE][nSiteIndex]]);
						pFunction->LogResult(NSITEINDEX, print_index+2+Print_Num*nTILE, Multi_global_count[nTILE][nSiteIndex]);
					}
				}
			}
		}
		*/
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_IMIN_Acc[nTILE][nSiteIndex]=1e2*(Multi_global_post[nTILE][nSiteIndex]-Buck_IMIN_Target[nTILE])/Buck_IMIN_Target[nTILE];
		}
	}

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT0|BIT1);
			Fqvi.CH[GP_SW_Buck[nTILE]].open();
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
			
			FOR_EACH_SITE(nSiteIndex)
			{
				Multi_global_Target[nTILE][nSiteIndex]=Buck_IMIN_Target[nTILE];

				pFunction->LogResult(NSITEINDEX, TestNO_target+Print_Num*nTILE, Multi_global_Target[nTILE][nSiteIndex]);
				if(QC_Flag==0)
				{
					//pFunction->LogResult(NSITEINDEX, TestNO_target+1+Print_Num*nTILE, Multi_global_Pre[nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, TestNO_target+2+Print_Num*nTILE, Multi_Global_trimcode[nTILE][nSiteIndex]);
				}
				pFunction->LogResult(NSITEINDEX, TestNO_target+3+Print_Num*nTILE, Multi_global_post[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, TestNO_target+4+Print_Num*nTILE, Buck_IMIN_Acc[nTILE][nSiteIndex]);		
			}
		}
	}
	delayms(2);
	Fvi16.CH[GPIO1].open();
	//==============power off=================//
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	Fvi45.CH[VOUT_SW]      .fv(0);	
	Fvi45.CH[VIN_3V3]      .fv(0);	
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(2);
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]      .open();
 
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Flash_Check
STT_TESTFUNC Flash_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Flash_Check_params* ours)
{	
	FOR_EACH_SITE(nSiteIndex)
	{
		if(IsSiteActive(nSiteIndex))
			Fail_bin_No[nSiteIndex]=3;
	}
	int flashing_repeat=0;
	int fail_count[SITE_MAX]={0};
	unsigned char data_readback[SITE_MAX]={0};
	int i2c_slave_addr[SITE_MAX]={0};

	//=========================flash IC============================================//
	if (QC_Flag==0)
	{
		if(CMI_Rev_No==300) //htol
			flashing_repeat=20;
		else
			flashing_repeat=1;
		for(int flashcount=0; flashcount<flashing_repeat; flashcount++)
		{
			Erase_Flash(0,300,true);
		}
	}
	delayms(2);	
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	i2c_init(HCTL_MODE,300000,2);
	delayms(5);
	
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex)
	{
		i2c_slave_addr[nSiteIndex]=0x03&readback[nSiteIndex];
		readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	}
	Reg[0x2000].write(readback);
	
	//Enter_Factory(true);
	//==============================check register code===========================
	for(i=0;i<CMI_var.RegisterSize;i++)
	{
		if(cmi_mask[i].reg_addr==0x10f2) 
		{
			Reg[0x1016].set_clr_inv(BIT7,BIT6);
			delayms(1);
			Fvi45.CH[VBYP].fv(0);
			delayms(1);
		}
		
		Reg[cmi_mask[i].reg_addr].read(readback);
		
		FOR_EACH_SITE(nSiteIndex)
		{
			if(cmi_mask[i].reg_addr==0x2000) readback[nSiteIndex]=readback[nSiteIndex]+i2c_slave_addr[nSiteIndex];
		}
		
		if(cmi_mask[i].reg_addr==0x10f7)
		{
			Fvi45.CH[VBYP].fv(VIN_SET+1.5);
			delayms(1);
		}
	
		if(isVMreg(cmi_mask[i].reg_addr))
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				data_readback[nSiteIndex]=readback[nSiteIndex];
				double_unit_reg[1][nSiteIndex][i]=readback[nSiteIndex];
				if(QC_Flag==0)
				{
					if(((data_readback[nSiteIndex]&cmi_mask[i].read_bit)-Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex])!=0)
					{
						fail_count[nSiteIndex]++;
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex]);
						printf("data_readback value = %d\n",data_readback[nSiteIndex]);
						printf("\n");
						//readback[nSiteIndex]=-readback[nSiteIndex];
					}
				}
				if(QC_Flag==1)
				{
					if((cmi_mask[i].reg_addr==0x2040||cmi_mask[i].reg_addr==0x2050)&&(CMI_Rev_No==521101||CMI_Rev_No==523103))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0xff;
					if((cmi_mask[i].reg_addr==0x2044||cmi_mask[i].reg_addr==0x2054)&&(CMI_Rev_No==521101||CMI_Rev_No==523103))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0x01;
					if((cmi_mask[i].reg_addr==0x2046||cmi_mask[i].reg_addr==0x2056)&&(CMI_Rev_No==521101||CMI_Rev_No==523103))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0xe7;
					if((cmi_mask[i].reg_addr==0x2047||cmi_mask[i].reg_addr==0x2057)&&(CMI_Rev_No==521101||CMI_Rev_No==523103))//Buck12 in multiphase-mode: F_0xY0 is different with ACOT mode, no trim bits.
						cmi_mask[i].trim_bit=0xf0;

					if(((data_readback[nSiteIndex]&cmi_mask[i].trim_bit&cmi_mask[i].read_bit)-Reg[cmi_mask[i].reg_addr].CMI)!=0)
					{
						fail_count[nSiteIndex]++;
						printf("reg address = %d\n",cmi_mask[i].reg_addr);
						printf("trimmed value = %d\n",Reg[cmi_mask[i].reg_addr].Trimmed[nSiteIndex]);
						printf("data_readback value = %d\n",data_readback[nSiteIndex]);
						printf("\n");
						//readback[nSiteIndex]=readback[nSiteIndex];
					}
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, i+2, readback[nSiteIndex]);
	}
	//printf("\n");

	FOR_EACH_SITE(nSiteIndex)
		pFunction->LogResult(NSITEINDEX, 1, fail_count[nSiteIndex]);

	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(2);
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]      .open();	


	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Device_Rdson
STT_TESTFUNC Device_Rdson(CFunction *pFunction, int nSiteNo, int nTestFlag, Device_Rdson_params* ours)
{
	double Buck_Rdsn[PMU_MAX][SITE_MAX]={0},Buck_Rdsp[PMU_MAX][SITE_MAX]={0};
	double I1_Measure[PMU_MAX][SITE_MAX]={0},V1_Measure[PMU_MAX][SITE_MAX]={0},I2_Measure[PMU_MAX][SITE_MAX]={0},V2_Measure[PMU_MAX][SITE_MAX]={0};
	FOR_EACH_SITE(nSiteIndex)
	{
		if(IsSiteActive(nSiteIndex))
			Fail_bin_No[nSiteIndex]=4;
	}
	
	Enter_Factory(true);
	delayms(2);
	Disable_Sequence();
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	delayms(2);
	Buck_TM_Reset();
	delayms(2);	
	//************Buck_Rdsp**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{

		if(PMU_MultiP_Mode[nTILE]&&nTILE==1)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x00].set_clr_inv(BITNONE,BIT3|BIT2|BIT1|BIT0);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.08*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);

		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,1,-1,1000,-1000);
		delayms(1);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT1|BIT0);
		delayms(1);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);


		//Rdsp
		Relay.BIT[GP_Buck_SW_Relay[nTILE]].set();
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x10);
		delayms(1);
        Fqvi.CH[GP_SW_Buck[nTILE]].fi(-10);
		delayms(1);

        Fqvi.CH[GP_SW_Buck[nTILE]].mi(I1_Measure[nTILE]);
        Fqvi.CH[GP_SW_Buck[nTILE]].mv(V1_Measure[nTILE]);

		if(nTILE==4)
		{
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-210);
			delayms(1);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-310);
			delayms(1);	
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(-410);
			delayms(1);
		}
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-510);
		delayms(1);

        Fqvi.CH[GP_SW_Buck[nTILE]].mi(I2_Measure[nTILE]);
        Fqvi.CH[GP_SW_Buck[nTILE]].mv(V2_Measure[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_Rdsp[nTILE][nSiteIndex]=1e6*(V2_Measure[nTILE][nSiteIndex]-V1_Measure[nTILE][nSiteIndex])/(I2_Measure[nTILE][nSiteIndex]-I1_Measure[nTILE][nSiteIndex]);
			//WLCSP package B2_Rdsp not accurary due to the sense pin should be C1, now add offset to subtract the internal metal resistor from E1 to C1.
			if(nTILE==1&&package_type==0) Buck_Rdsp[nTILE][nSiteIndex]=Buck_Rdsp[nTILE][nSiteIndex]-4;
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);	
        Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(2);
		Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
		delayms(1);

		//Rdsn	
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x20);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(500);
		delayms(2);
        Fqvi.CH[GP_SW_Buck[nTILE]].mi(I1_Measure[nTILE]);
        Fqvi.CH[GP_SW_Buck[nTILE]].mv(V1_Measure[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_Rdsn[nTILE][nSiteIndex]=1e6*V1_Measure[nTILE][nSiteIndex]/I1_Measure[nTILE][nSiteIndex];
		}

        Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].open();

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}

		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 2*nTILE+2, Buck_Rdsp[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 2*nTILE+3, Buck_Rdsn[nTILE][nSiteIndex]);
		}
		
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}
	Relay.BIT[K48_I2C]		.clr();
	delayms(2);
	Fvi45.CH[VOUT_SW]		.fv(0);
	Fvi45.CH[VIN_3V3]		.fv(0);
	Fvi45.CH[VBYP]		.fv(0);
	delayms(2);
	Fvi45.CH[VOUT_SW]		.open(0);
	Fvi45.CH[VIN_3V3]		.open(0);
	Fvi45.CH[VBYP]		.open(0);

	//==============power off=================//
    //Continue the next test item

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Device_Leakage_ABS
STT_TESTFUNC Device_Leakage_ABS(CFunction *pFunction, int nSiteNo, int nTestFlag, Device_Leakage_ABS_params* ours)
{
	double SW_HS_Leak_ABS[PMU_MAX][SITE_MAX]={0},SW_LS_Leak_ABS[PMU_MAX][SITE_MAX]={0},Buck_Ishut_ABS[PMU_MAX][SITE_MAX]={0},FB_Iout_ABS[PMU_MAX][SITE_MAX]={0};
	double LDO_Idis_ABS[LDO_MAX][SITE_MAX]={0};
	double SDA_HS_Leak_ABS[SITE_MAX]={0},SDA_LS_Leak_ABS[SITE_MAX]={0},SCL_HS_Leak_ABS[SITE_MAX]={0},SCL_LS_Leak_ABS[SITE_MAX]={0};
	double PWRDN0_HS_Leak_ABS[SITE_MAX]={0},PWRDN0_LS_Leak_ABS[SITE_MAX]={0},PWRDN1_HS_Leak_ABS[SITE_MAX]={0},PWRDN1_LS_Leak_ABS[SITE_MAX]={0};
	double GPIO0_HS_Leak_ABS[SITE_MAX]={0},GPIO0_LS_Leak_ABS[SITE_MAX]={0},GPIO1_HS_Leak_ABS[SITE_MAX]={0},GPIO1_LS_Leak_ABS[SITE_MAX]={0};
	double EXTPG_HS_Leak_ABS[SITE_MAX]={0},EXTPG_LS_Leak_ABS[SITE_MAX]={0},EXTEN_HS_Leak_ABS[SITE_MAX]={0},EXTEN_LS_Leak_ABS[SITE_MAX]={0};
	double COREPG_HS_Leak_ABS[SITE_MAX]={0},COREPG_LS_Leak_ABS[SITE_MAX]={0},GATE_HS_Leak_ABS[SITE_MAX]={0},GATE_LS_Leak_ABS[SITE_MAX]={0};
	double LPM_HS_Leak_ABS[SITE_MAX]={0},LPM_LS_Leak_ABS[SITE_MAX]={0},nRESET_HS_Leak_ABS[SITE_MAX]={0},nRESET_LS_Leak_ABS[SITE_MAX]={0};
	double nIRQ_HS_Leak_ABS[SITE_MAX]={0},nIRQ_LS_Leak_ABS[SITE_MAX]={0};

	unsigned char RegF_0xxd_Data[PMU_MAX][SITE_MAX]={0},RegF_0xx9_Data[PMU_MAX][SITE_MAX]={0};

	double ABS_Leak_HighLi[51]={0},ABS_Leak_LowLi[51]={0};
    for(i=0;i<50;i++)
	{
		pFunction->GetHiLimit(i,&ABS_Leak_HighLi[i+1]);
		pFunction->GetLowLimit(i,&ABS_Leak_LowLi[i+1]);//item 1-48
	}
	double check_V[2]={0};
	Enter_Factory(true);
	delayms(2);
	//Fvi16.CH[nIRQ]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//delayms(1);
	//Fvi16.CH[nIRQ]	.mv(check_V);
	//printf("nIRQ_V1 = %3.3f\n",check_V[0]);
	//printf("\n");
	//Fvi16.CH[nIRQ]	.open();
	delayms(1);
	GPIO_Leak_setting();
	delayms(1);
	Disconnect_all_cap();
	delayms(1);
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Reg[0x1012].write(0xff);
	delayms(2);
	Fvi45.CH[VOUT_SW]		.fv(4);
	Fvi45.CH[VIN_3V3]		.fv(4);
	delayms(1);
	//Buck_Idis
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		if(PMU_MultiP_Mode[nTILE])//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].read(RegF_0xxd_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0x40|RegF_0xxd_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(readback);
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(RegF_0xx9_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0xdf&RegF_0xx9_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
		
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(4,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);	
		//LS_Leak
		double LS_Trigger_ABS_Current[PMU_MAX]={80,80,80,80,30};
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(4.3,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(5);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(4.3,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_LS_Leak_ABS[nTILE]);
		delayms(1);
		
		for(i=0;i<10;i++)
		{
			Fqvi.CH[GP_SW_Buck[nTILE]].fv(4.3-0.005*i);
			delayms(2);
			Fqvi.CH[GP_SW_Buck[nTILE]].mi(i_meas);
			FOR_EACH_SITE(nSiteIndex)
			{
				if((1e3*i_meas[0]<(ABS_Leak_HighLi[2+2*nTILE]-10))&&Flag[nSiteIndex]==0)
				{
					SW_LS_Leak_ABS[nTILE][nSiteIndex]=i_meas[nSiteIndex];
					Flag[nSiteIndex]=1;
				}
			}
			if(isAllFlagSet(false))
				break;	
		}	
		clrAllFlag();
		
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Fqvi.CH[GP_SW_Buck[nTILE]].open();
		
		//Buck_Ishut
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(4,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(4,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);		
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_Ishut_ABS[nTILE]);
		
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(RegF_0xxd_Data[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(RegF_0xx9_Data[nTILE]);	

		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].clr();
		}	
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 2*nTILE+2, 1e3*SW_LS_Leak_ABS[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 2*nTILE+3, 1e3*Buck_Ishut_ABS[nTILE][nSiteIndex]);
		}
	}
	
	Fvi16.CH[VIN_LDO1] .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[VIN_LDO2] .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Relay.BIT[K14_OUTLDO_Cap] .set();

	Reg[0x1040].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Maks the ILIMWarn fault
	Reg[0x1050].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Maks the ILIMWarn fault
	Reg[0x1060].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Maks the ILIMWarn fault
	Reg[0x1070].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Maks the ILIMWarn fault
	Reg[0x1080].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Maks the ILIMWarn fault
	Reg[0x1091].set_clr_inv(BIT1|BIT0);//Maks the ILIMWarn fault

	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);//Need configure 0x80 to remove the IRQ trigger
	Reg[0x2002].write(0x04);
	Reg[0x2003].write(0x01);
	delayms(1);
	
	Reg[0x1040].read(readback);
	//printf("B1_fault = %d\n",readback[0]);
	Reg[0x1050].read(readback);
	//printf("B2_fault = %d\n",readback[0]);
	Reg[0x1060].read(readback);
	//printf("B3_fault = %d\n",readback[0]);
	Reg[0x1070].read(readback);
	//printf("B4_fault = %d\n",readback[0]);
	Reg[0x1080].read(readback);
	//printf("B5_fault = %d\n",readback[0]);
	Reg[0x1090].read(readback);
	//printf("LDO12_fault = %d\n",readback[0]);
	Reg[0x1007].read(readback);
	//printf("Master_fault = %d\n",readback[0]);
	//printf("\n");
	
	Relay.BIT[K48_I2C]		.clr();
	Fvi45.CH[VBYP]      .fv(0);	

	Fvi16.CH[SDA].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[SCL].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[nRESET].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[nIRQ].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[CORE_PG].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[LPM].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[PWRDN0].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[PWRDN1].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[EXT_EN].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[EXT_PG].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GPIO0].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GPIO1].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GATE].fv(-0.3,SITE_ALL,Stall3V3,Stall50uA);
	delayms(2);
	Fvi16.CH[SDA].mi(SDA_HS_Leak_ABS);
	Fvi16.CH[SCL].mi(SCL_HS_Leak_ABS);
	Fvi16.CH[nRESET].mi(nRESET_HS_Leak_ABS);
	Fvi16.CH[nIRQ].mi(nIRQ_HS_Leak_ABS);

	Fvi16.CH[CORE_PG].mi(COREPG_HS_Leak_ABS);
	Fvi16.CH[LPM].mi(LPM_HS_Leak_ABS);
	Fvi16.CH[PWRDN0].mi(PWRDN0_HS_Leak_ABS);
	Fvi16.CH[PWRDN1].mi(PWRDN1_HS_Leak_ABS);

	Fvi16.CH[EXT_EN].mi(EXTEN_HS_Leak_ABS);
	Fvi16.CH[EXT_PG].mi(EXTPG_HS_Leak_ABS);
	Fvi16.CH[GPIO0].mi(GPIO0_HS_Leak_ABS);
	Fvi16.CH[GPIO1].mi(GPIO1_HS_Leak_ABS);
	Fvi16.CH[GATE].mi(GATE_HS_Leak_ABS);

	TestNO_target=12;

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, 1e3*SDA_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3*SCL_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, 1e3*nRESET_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3*nIRQ_HS_Leak_ABS[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*COREPG_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*LPM_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*PWRDN0_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*PWRDN1_HS_Leak_ABS[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+8, 1e3*EXTEN_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, 1e3*EXTPG_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, 1e3*GPIO0_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, 1e3*GPIO1_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GATE_HS_Leak_ABS[nSiteIndex]);

	}
	Fvi16.CH[SDA].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[SCL].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[nRESET].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[nIRQ].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[CORE_PG].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[LPM].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[PWRDN0].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[PWRDN1].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[EXT_EN].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[EXT_PG].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[GPIO0].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[GPIO1].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	Fvi16.CH[GATE].fv(3.6,SITE_ALL,Stall12V,Stall50uA);
	delayms(2);
	Fvi16.CH[SDA].mi(SDA_LS_Leak_ABS);
	Fvi16.CH[SCL].mi(SCL_LS_Leak_ABS);
	Fvi16.CH[nRESET].mi(nRESET_LS_Leak_ABS);
	Fvi16.CH[nIRQ].mi(nIRQ_LS_Leak_ABS);
	Fvi16.CH[CORE_PG].mi(COREPG_LS_Leak_ABS);
	Fvi16.CH[LPM].mi(LPM_LS_Leak_ABS);

	Fvi16.CH[PWRDN0].mi(PWRDN0_LS_Leak_ABS);
	Fvi16.CH[PWRDN1].mi(PWRDN1_LS_Leak_ABS);
	Fvi16.CH[EXT_EN].mi(EXTEN_LS_Leak_ABS);
	Fvi16.CH[EXT_PG].mi(EXTPG_LS_Leak_ABS);
	Fvi16.CH[GPIO0].mi(GPIO0_LS_Leak_ABS);
	Fvi16.CH[GPIO1].mi(GPIO1_LS_Leak_ABS);
	Fvi16.CH[GATE].mi(GATE_LS_Leak_ABS);
	Fvi16.CH[SDA].fv(0);
	Fvi16.CH[SCL].fv(0);
	Fvi16.CH[nRESET].fv(0);
	Fvi16.CH[nIRQ].fv(0);
	Fvi16.CH[CORE_PG].fv(0);
	Fvi16.CH[LPM].fv(0);
	
	Fvi16.CH[PWRDN0].fv(0);
	Fvi16.CH[PWRDN1].fv(0);
	Fvi16.CH[EXT_EN].fv(0);
	Fvi16.CH[EXT_PG].fv(0);
	Fvi16.CH[GPIO0].fv(0);
	Fvi16.CH[GPIO1].fv(0);
	Fvi16.CH[GATE].fv(0);
	
	Fvi16.CH[SDA].open();
	Fvi16.CH[SCL].open();
	Fvi16.CH[nRESET].open();
	Fvi16.CH[nIRQ].open();
	Fvi16.CH[CORE_PG].open();
	Fvi16.CH[LPM].open();
	Fvi16.CH[PWRDN0].open();
	Fvi16.CH[PWRDN1].open();
	Fvi16.CH[EXT_EN].open();
	Fvi16.CH[EXT_PG].open();
	Fvi16.CH[GPIO0].open();
	Fvi16.CH[GPIO1].open();
	Fvi16.CH[GATE].open();
	delayms(1);
	Relay.BIT[K48_I2C]		.clr();
	delayms(2);
	Fvi16.CH[VIN_LDO1]. fv(0);
	Fvi16.CH[VIN_LDO2]. fv(0);
	Relay.BIT[K14_OUTLDO_Cap].clr();
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	delayms(3);
	Fvi16.CH[VIN_LDO1]. open();
	Fvi16.CH[VIN_LDO2]. open();
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]      .open();
	TestNO_target=25;	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, 1e3*SDA_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3*SCL_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, 1e3*nRESET_HS_Leak_ABS[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3*nIRQ_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*COREPG_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*LPM_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*PWRDN0_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*PWRDN1_HS_Leak_ABS[nSiteIndex]);
		
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, 1e3*EXTEN_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, 1e3*EXTPG_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, 1e3*GPIO0_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, 1e3*GPIO1_HS_Leak_ABS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GATE_HS_Leak_ABS[nSiteIndex]);		
	}
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Device_Leakage
STT_TESTFUNC Device_Leakage(CFunction *pFunction, int nSiteNo, int nTestFlag, Device_Leakage_params* ours)
{
	double SW_HS_Leak[PMU_MAX][SITE_MAX]={0},SW_LS_Leak[PMU_MAX][SITE_MAX]={0},Buck_Ishut[PMU_MAX][SITE_MAX]={0},FB_Iout[PMU_MAX][SITE_MAX]={0};
	double LDO_Idis[LDO_MAX][SITE_MAX]={0};
	double SDA_HS_Leak[SITE_MAX]={0},SDA_LS_Leak[SITE_MAX]={0},SCL_HS_Leak[SITE_MAX]={0},SCL_LS_Leak[SITE_MAX]={0};
	double PWRDN0_HS_Leak[SITE_MAX]={0},PWRDN0_LS_Leak[SITE_MAX]={0},PWRDN1_HS_Leak[SITE_MAX]={0},PWRDN1_LS_Leak[SITE_MAX]={0};
	double GPIO0_HS_Leak[SITE_MAX]={0},GPIO0_LS_Leak[SITE_MAX]={0},GPIO1_HS_Leak[SITE_MAX]={0},GPIO1_LS_Leak[SITE_MAX]={0};
	double EXTPG_HS_Leak[SITE_MAX]={0},EXTPG_LS_Leak[SITE_MAX]={0},EXTEN_HS_Leak[SITE_MAX]={0},EXTEN_LS_Leak[SITE_MAX]={0};
	double COREPG_HS_Leak[SITE_MAX]={0},COREPG_LS_Leak[SITE_MAX]={0},GATE_HS_Leak[SITE_MAX]={0},GATE_LS_Leak[SITE_MAX]={0};
	double LPM_HS_Leak[SITE_MAX]={0},LPM_LS_Leak[SITE_MAX]={0},nRESET_HS_Leak[SITE_MAX]={0},nRESET_LS_Leak[SITE_MAX]={0};
	double nIRQ_HS_Leak[SITE_MAX]={0},nIRQ_LS_Leak[SITE_MAX]={0};
	unsigned char RegF_0xxd_Data[PMU_MAX][SITE_MAX]={0},RegF_0xx9_Data[PMU_MAX][SITE_MAX]={0};
	//Disconnect_all_cap();
	Enter_Factory(true);
	delayms(1);
	Disable_Sequence();
	delayms(1);
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Buck_TM_Reset();
	//Buck_Idis
	Reg[0x100a].set_clr_inv(BIT3);
	Reg[0x100b].set_clr_inv(BIT3);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		if(PMU_MultiP_Mode[nTILE])//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].read(RegF_0xxd_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0x40|RegF_0xxd_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(RegF_0xx9_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0xdf&RegF_0xx9_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);

		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET+0.3);
		delayms(2);	
		//LS_Leak
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(VIN_SET+0.3,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(VIN_SET+0.3,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_LS_Leak[nTILE]);
		//HS_Leak
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_HS_Leak[nTILE]);

		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Fqvi.CH[GP_SW_Buck[nTILE]].open();

		//Buck_Ishut
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);		
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_Ishut[nTILE]);

		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(RegF_0xxd_Data[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(RegF_0xx9_Data[nTILE]);	

		//FB_Iout
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT7);

		if(nTILE==4)
		{	
			Reg[0x1016].write(0x80);
			Reg[0x1018].write(0xe0);
			Reg[0x1019].write(0x0d);
			Reg[0x101d].write(0x0d);
			Reg[0x1021].write(0x0d);
			Reg[0x1025].write(0x0d);
			Reg[0x1029].write(0x0d);	
			Reg[0x102d].write(0x0d);
			Reg[0x1031].write(0x0d);
			SDASCL_Connect_VI();
		}
		delayms(1);		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);		
		Fvi45.CH[GP_FB_Buck[nTILE]].mi(FB_Iout[nTILE]);
		if(nTILE==4)
		{
			SDASCL_Connect_I2C();
			Relay.BIT[K41_FQVI5].clr();
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);

		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 4*nTILE+2, 1e3*SW_LS_Leak[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+3, 1e3*SW_HS_Leak[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+4, 1e3*Buck_Ishut[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+5, 1e3*FB_Iout[nTILE][nSiteIndex]);
		}

	}	
	//LDO_Idis
	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();

	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall5mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall5mA);
	Reg[0x1093].set_clr_inv(BITNONE,BIT7|BIT3);
	Reg[0x1094].set_clr_inv(BIT7|BIT3);

	Fqvi.CH[OUT_LDO1].fv(0,SITE_ALL,SET_CLAMP,4,-4,40,-40);
	Fqvi.CH[OUT_LDO2].fv(0,SITE_ALL,SET_CLAMP,4,-4,40,-40);
    delayms(2);
	Fqvi.CH[OUT_LDO1].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.004,-0.004);
	Fqvi.CH[OUT_LDO2].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.004,-0.004);
    delayms(2);
	Fqvi.CH[OUT_LDO1].mi(LDO_Idis[0]);
	Fqvi.CH[OUT_LDO2].mi(LDO_Idis[1]);

	Fqvi.CH[OUT_LDO1].open();
	Fqvi.CH[OUT_LDO2].open();

	Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();

	Fvi16.CH[VIN_LDO1]	.fv(0);
	Fvi16.CH[VIN_LDO2]	.fv(0);
	Fvi16.CH[VIN_LDO1]	.open();
	Fvi16.CH[VIN_LDO2]	.open();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 22, 1e3*LDO_Idis[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 23, 1e3*LDO_Idis[1][nSiteIndex]);
	}	

	delayms(2);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);//Need configure 0x80 to remove the IRQ trigger
	Reg[0x20fa].write(0x20);
	Reg[0x2002].write(0x04);
	Reg[0x2003].write(0x01);

	GPIO_Leak_setting();
	Fvi45.CH[VBYP]		.fv(0);
	SDASCL_Connect_VI();
	delayms(2);	
	Fvi16.CH[SDA].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[SCL].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[nRESET].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[nIRQ].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[CORE_PG].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[LPM].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[PWRDN0].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[PWRDN1].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[EXT_EN].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[EXT_PG].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GPIO0].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GPIO1].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[GATE].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	delayms(2);
	Fvi16.CH[SDA].mi(SDA_HS_Leak);
	Fvi16.CH[SCL].mi(SCL_HS_Leak);
	Fvi16.CH[nRESET].mi(nRESET_HS_Leak);
	Fvi16.CH[nIRQ].mi(nIRQ_HS_Leak);
	Fvi16.CH[CORE_PG].mi(COREPG_HS_Leak);
	Fvi16.CH[LPM].mi(LPM_HS_Leak);
	Fvi16.CH[PWRDN0].mi(PWRDN0_HS_Leak);
	Fvi16.CH[PWRDN1].mi(PWRDN1_HS_Leak);
	Fvi16.CH[EXT_EN].mi(EXTEN_HS_Leak);
	Fvi16.CH[EXT_PG].mi(EXTPG_HS_Leak);
	Fvi16.CH[GPIO0].mi(GPIO0_HS_Leak);
	Fvi16.CH[GPIO1].mi(GPIO1_HS_Leak);
	Fvi16.CH[GATE].mi(GATE_HS_Leak);

	TestNO_target=24;

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, 1e3*SDA_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3*SCL_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, 1e3*nRESET_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3*nIRQ_HS_Leak[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*COREPG_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*LPM_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*PWRDN0_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*PWRDN1_HS_Leak[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+8, 1e3*EXTEN_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, 1e3*EXTPG_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, 1e3*GPIO0_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, 1e3*GPIO1_HS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GATE_HS_Leak[nSiteIndex]);

	}
	Fvi16.CH[SDA].fv(VIN_SET);
	Fvi16.CH[SCL].fv(VIN_SET);
	Fvi16.CH[nRESET].fv(VIN_SET);
	Fvi16.CH[nIRQ].fv(VIN_SET);
	Fvi16.CH[CORE_PG].fv(VIN_SET);
	Fvi16.CH[LPM].fv(VIN_SET);
	
	Fvi16.CH[PWRDN0].fv(VIN_SET);
	Fvi16.CH[PWRDN1].fv(VIN_SET);
	Fvi16.CH[EXT_EN].fv(VIN_SET);
	Fvi16.CH[EXT_PG].fv(VIN_SET);
	Fvi16.CH[GPIO0].fv(VIN_SET);
	Fvi16.CH[GPIO1].fv(VIN_SET);
	Fvi16.CH[GATE].fv(VIN_SET);
	delayms(2);
	Fvi16.CH[SDA].mi(SDA_LS_Leak);
	Fvi16.CH[SCL].mi(SCL_LS_Leak);
	Fvi16.CH[nRESET].mi(nRESET_LS_Leak);
	Fvi16.CH[nIRQ].mi(nIRQ_LS_Leak);
	Fvi16.CH[CORE_PG].mi(COREPG_LS_Leak);
	Fvi16.CH[LPM].mi(LPM_LS_Leak);
	Fvi16.CH[PWRDN0].mi(PWRDN0_LS_Leak);
	Fvi16.CH[PWRDN1].mi(PWRDN1_LS_Leak);
	Fvi16.CH[EXT_EN].mi(EXTEN_LS_Leak);
	Fvi16.CH[EXT_PG].mi(EXTPG_LS_Leak);
	Fvi16.CH[GPIO0].mi(GPIO0_LS_Leak);
	Fvi16.CH[GPIO1].mi(GPIO1_LS_Leak);
	Fvi16.CH[GATE].mi(GATE_LS_Leak);

	Fvi16.CH[SDA].fv(0);
	Fvi16.CH[SCL].fv(0);
	Fvi16.CH[nRESET].fv(0);
	Fvi16.CH[nIRQ].fv(0);
	Fvi16.CH[CORE_PG].fv(0);
	Fvi16.CH[LPM].fv(0);
	
	Fvi16.CH[PWRDN0].fv(0);
	Fvi16.CH[PWRDN1].fv(0);
	Fvi16.CH[EXT_EN].fv(0);
	Fvi16.CH[EXT_PG].fv(0);
	Fvi16.CH[GPIO0].fv(0);
	Fvi16.CH[GPIO1].fv(0);
	Fvi16.CH[GATE].fv(0);
	
	Fvi16.CH[SDA].open();
	Fvi16.CH[SCL].open();
	Fvi16.CH[nRESET].open();
	Fvi16.CH[nIRQ].open();
	Fvi16.CH[CORE_PG].open();
	Fvi16.CH[LPM].open();
	Fvi16.CH[PWRDN0].open();
	Fvi16.CH[PWRDN1].open();
	Fvi16.CH[EXT_EN].open();
	Fvi16.CH[EXT_PG].open();
	Fvi16.CH[GPIO0].open();
	Fvi16.CH[GPIO1].open();
	Fvi16.CH[GATE].open();
	delayms(1);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}

	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(5);
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();

	TestNO_target=37;	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, 1e3*SDA_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, 1e3*SCL_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, 1e3*nRESET_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, 1e3*nIRQ_LS_Leak[nSiteIndex]);
		
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*COREPG_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*LPM_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*PWRDN0_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*PWRDN1_LS_Leak[nSiteIndex]);
		
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, 1e3*EXTEN_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, 1e3*EXTPG_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, 1e3*GPIO0_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, 1e3*GPIO1_LS_Leak[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GATE_LS_Leak[nSiteIndex]);		
	}
	

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Master_Isupply
STT_TESTFUNC Master_Isupply(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_Isupply_params* ours)
{
	double I_PWRDIS[SITE_MAX]={0},INLDO_Current[LDO_MAX][SITE_MAX]={0},I_PWRDIS_State[SITE_MAX]={0};
	double VOUTSW_Current[SITE_MAX]={0},VIN3V3_Current[SITE_MAX]={0},Buck_VIN_Current[SITE_MAX]={0};
	double Buck_IQ_Normal[PMU_MAX][SITE_MAX]={0},Buck_IQ_LPM[PMU_MAX][SITE_MAX]={0},Buck_IQ_Shutdown[PMU_MAX][SITE_MAX]={0};
	double LDO_Off_CP_Normal[LDO_MAX][SITE_MAX]={0},LDO_On_CP_Normal[LDO_MAX][SITE_MAX]={0},LDO_Off_CP_LPM[LDO_MAX][SITE_MAX]={0},LDO_On_CP_LPM[LDO_MAX][SITE_MAX]={0},LDO_Shutdown[LDO_MAX][SITE_MAX]={0};
	double LDO12_On_CP_Normal[SITE_MAX]={0},LDO12_On_CP_LPM[SITE_MAX]={0};

	double Buck_off[PMU_MAX][SITE_MAX]={0},Buck_IQSTD_LPM1[PMU_MAX][SITE_MAX]={0},Buck_IQSTD_LPM0_Save1[PMU_MAX][SITE_MAX]={0},Buck_IQSTD_LPM0_Save0[PMU_MAX][SITE_MAX]={0};
	double Buck_std[SITE_MAX]={0};
	Relay.BIT[K14_OUTLDO_Cap].set();
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	delayms(2);
	Relay.BIT[K48_I2C].set();	
	//debug	
	//i2c_init(HCTL_MODE,300000,2);
	//debug	
	delayms(2);
	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Isupply_check_setting();

	Fvi45.CH[VBYP]		.fv(0);
	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 2, Factory_Mode_Flag[nSiteIndex]);
	}
	//************I_PWRDIS**********

	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi16.CH[SDA]	.fv(0,SITE_ALL,Stall12V,Stall5mA);
	Fvi16.CH[SCL]	.fv(0,SITE_ALL,Stall12V,Stall5mA);
	delayms(10);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(10);
	Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
	Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
	FOR_EACH_SITE(nSiteIndex)
		I_PWRDIS[nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex];
	for(i=0;i<30;i++)
	{
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
			i_meas[nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex];
			if(fabs(1e3*i_meas[nSiteIndex]-28)<2&&Flag[nSiteIndex]==0)
			{
				I_PWRDIS[nSiteIndex]=i_meas[nSiteIndex];
				Flag[nSiteIndex]=1;
			}	
		}
		if(isAllFlagSet(false))
			break;
	}	
	clrAllFlag();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 3, 1e3*VOUTSW_Current[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 4, 1e3*VIN3V3_Current[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 5, 1e3*I_PWRDIS[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	SDASCL_Connect_I2C();
	delayms(2);

	//************Buck_IQ_Normal**********
	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(1);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x01);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].read(readback);

		delayms(2);
		SDASCL_Connect_VI();
		delayms(10);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		for(i=0;i<30;i++)
		{
			Fvi45.CH[VOUT_SW]	.mi(i_meas);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(1e3*i_meas[nSiteIndex]-60)<10&&Flag[nSiteIndex]==0)
				{
					VOUTSW_Current[nSiteIndex]=i_meas[nSiteIndex];
					Flag[nSiteIndex]=1;
				}	
			}
			if(isAllFlagSet(false))
				break;
		}	
		clrAllFlag();

		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
			Buck_IQ_Normal[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-I_PWRDIS[nSiteIndex];
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);

		SDASCL_Connect_I2C();
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(_0x00);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
				pFunction->LogResult(NSITEINDEX, 6+nTILE, 1e3*Buck_IQ_Normal[nTILE][nSiteIndex]);
		}
	}				
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	delayms(1);	

	//************Buck_IQ_LPM**********

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xc0|readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);	
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x01);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].read(readback);	

		delayms(2);
		SDASCL_Connect_VI();
		delayms(2);
		Fvi45.CH[VBYP]		.fv(0);
		delayms(10);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(10);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		for(i=0;i<30;i++)
		{
			Fvi45.CH[VOUT_SW]	.mi(i_meas);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(1e3*i_meas[nSiteIndex]-21)<5&&Flag[nSiteIndex]==0)
				{
					VOUTSW_Current[nSiteIndex]=i_meas[nSiteIndex];
					Flag[nSiteIndex]=1;
				}	
			}
			if(isAllFlagSet(false))
				break;
		}	
		clrAllFlag();
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
			Buck_IQ_LPM[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-I_PWRDIS[nSiteIndex];
		
		SDASCL_Connect_I2C();
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);	
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(_0x00);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
				pFunction->LogResult(NSITEINDEX, 11+nTILE, 1e3*Buck_IQ_LPM[nTILE][nSiteIndex]);
		}
	}

	//************Buck_IQ_Shutdown**********
	SDASCL_Connect_VI();
	delayms(2);
	Fvi45.CH[VBYP]		.fv(0);
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(2);
	for(i=0;i<20;i++)
	{
		Fvi45.CH[VOUT_SW]	.mi(I1);
		Fvi45.CH[VIN_3V3]	.mi(I2);
		delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(fabs(1e3*I1[nSiteIndex]+1e3*I2[nSiteIndex]-1e3*I_PWRDIS[nSiteIndex])<3)
			{
				Flag[nSiteIndex]=1;
				VOUTSW_Current[nSiteIndex]=I1[nSiteIndex];
				VIN3V3_Current[nSiteIndex]=I2[nSiteIndex];
			}	
		}
		if(isAllFlagSet(false))
			break;
	}	
	clrAllFlag();
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
			Buck_IQ_Shutdown[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-I_PWRDIS[nSiteIndex];	
		delayms(1);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
				pFunction->LogResult(NSITEINDEX, 16+nTILE, 1e3*Buck_IQ_Shutdown[nTILE][nSiteIndex]);			
		}

	}
	SDASCL_Connect_I2C();
	delayms(5);
	//************LDO_Off_CP_Normal**********
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);

	//More settings for different cmi file
	Reg[0x1095].write(0xfc);//set SST as 102 CMI or maximal for 103 cmi
	Reg[0x10f7].set_clr_inv(BITNONE,BIT3|BIT2);//Need to set bit3/2 to 0 for multi-phase cmi

	//Set the OUT=1.5V for Charge Pump mode
	if(LDO_Vset[0]<1.25)
	{
		Reg[0x1096].write(0.9/(0.005*(1+((Reg[0x1092].CMI&0x08)>>3))));//Disable the fault
		Reg[0x1097].write(0.9/(0.005*(1+((Reg[0x1092].CMI&0x08)>>3))));//Disable the fault
	}
	if(LDO_Vset[1]<1.25)
	{
		Reg[0x1098].write(0.9/(0.005*(1+((Reg[0x1092].CMI&0x04)>>2))));//Disable the fault
		Reg[0x1099].write(0.9/(0.005*(1+((Reg[0x1092].CMI&0x04)>>2))));//Disable the fault
	}
	//More settings for different cmi file

	Reg[0x1091].write(0xff);//Disable the fault
	Reg[0x1092].set_clr_inv(BITNONE,BIT1|BIT0);	
	delayms(1);
	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{
		delayms(2);
		if(nTILE==0)	Reg[0x1093].write(0xc4);
		if(nTILE==1)	Reg[0x1093].write(0x4c);
		SDASCL_Connect_VI();
		delayms(10);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		Fvi16.CH[GP_INLDO[nTILE]]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50uA);
		delayms(20);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi16.CH[GP_INLDO[nTILE]].mi(INLDO_Current[nTILE]);

		FOR_EACH_SITE(nSiteIndex)
			LDO_Off_CP_Normal[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[nTILE][nSiteIndex]-I_PWRDIS[nSiteIndex];
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);	

		SDASCL_Connect_I2C();
		delayms(5);		
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 21+nTILE, 1e3*LDO_Off_CP_Normal[nTILE][nSiteIndex]);
		}
	}
	
	printf("LDO1 Off CP Normal = %3.6f\n",INLDO_Current[0][0]);
	printf("LDO2 Off CP Normal = %3.6f\n",INLDO_Current[1][0]);
	delayms(1);
	Reg[0x1092].write_CMI();
	Reg[0x1093].write(0x44);
	delayms(2);

	//************LDO_On_CP_Normal**********
	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);

	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{

		if(nTILE==0)
		{
			Reg[0x1092].set_clr_inv(BIT1,BIT0);
			Reg[0x1093].write(0xc4);
		}
		if(nTILE==1)
		{
			Reg[0x1092].set_clr_inv(BIT0,BIT1);
			Reg[0x1093].write(0x4c);
		}
		SDASCL_Connect_VI();
		delayms(10);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		Fvi16.CH[GP_INLDO[nTILE]]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50uA);
		delayms(20);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi16.CH[GP_INLDO[nTILE]].mi(INLDO_Current[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			LDO_On_CP_Normal[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[nTILE][nSiteIndex]-I_PWRDIS[nSiteIndex];
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		SDASCL_Connect_I2C();
		delayms(5);		
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 23+nTILE, 1e3*LDO_On_CP_Normal[nTILE][nSiteIndex]);
		}
	}
	printf("LDO1 On CP Normal = %3.6f\n",INLDO_Current[0][0]);
	printf("LDO2 On CP Normal = %3.6f\n",INLDO_Current[1][0]);

	Reg[0x1093].write(0x44);
	Relay.BIT[K14_OUTLDO_Cap].clr();
	if(CMI_Rev_No==300)//For HTOL setting only
	{
		Reg[0x1098].write(0xbe);
		Reg[0x1099].write(0xbe);
	}
	//************LDO_Off_CP_LPM**********
	SDASCL_Connect_I2C();
	delayms(2);
	Reg[0x100a].write(0xfe);
	Reg[0x1006].write(0x05);
	Reg[0x1092].set_clr_inv(BITNONE,BIT1|BIT0);	
	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{
		delayms(2);
		if(nTILE==0)	Reg[0x1093].write(0xd4);
		if(nTILE==1)	Reg[0x1093].write(0x4d);
		SDASCL_Connect_VI();
		delayms(5);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi16.CH[GP_INLDO[nTILE]].mi(INLDO_Current[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			LDO_Off_CP_LPM[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[nTILE][nSiteIndex]-I_PWRDIS[nSiteIndex];	
		SDASCL_Connect_I2C();
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(1);			
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 25+nTILE, 1e3*LDO_Off_CP_LPM[nTILE][nSiteIndex]);
		}
	}
	printf("LDO1 Off CP LPM = %3.6f\n",INLDO_Current[0][0]);
	printf("LDO2 Off CP LPM = %3.6f\n",INLDO_Current[1][0]);
	//************LDO_On_CP_LPM**********
	delayms(5);
	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{
		if(nTILE==0)
		{
			Reg[0x1092].set_clr_inv(BIT1,BIT0);
			Reg[0x1093].write(0xd4);
		}
		if(nTILE==1)
		{
			Reg[0x1092].set_clr_inv(BIT0,BIT1);
			Reg[0x1093].write(0x4d);
		}
		delayms(10);
		SDASCL_Connect_VI();
		delayms(5);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi16.CH[GP_INLDO[nTILE]].mi(INLDO_Current[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			LDO_On_CP_LPM[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[nTILE][nSiteIndex]-I_PWRDIS[nSiteIndex];
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		SDASCL_Connect_I2C();
		delayms(2);		
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 27+nTILE, 1e3*LDO_On_CP_LPM[nTILE][nSiteIndex]);
		}
	}
	printf("LDO1 On CP LPM = %3.6f\n",INLDO_Current[0][0]);
	printf("LDO2 On CP LPM = %3.6f\n",INLDO_Current[1][0]);
	Reg[0x1092].write_CMI();
	Reg[0x1093].write(0x44);	
	Reg[0x100a].write(_0x00);
	Reg[0x1095].write_CMI();
	//************LDO_Shutdown**********
	Reg[0x1006].write(0x04);
	delayms(1);
	SDASCL_Connect_VI();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(5);
	for(i=0;i<20;i++)
	{
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current,nSiteIndex);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current,nSiteIndex);
		
		if(fabs(1e3*VOUTSW_Current[nSiteIndex]+1e3*VIN3V3_Current[nSiteIndex]-1e3*I_PWRDIS[nSiteIndex])>3)
		{
			delayms(2);
		}
		if(fabs(1e3*VOUTSW_Current[nSiteIndex]+1e3*VIN3V3_Current[nSiteIndex]-1e3*I_PWRDIS[nSiteIndex])<3)
			break;
	}
	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi16.CH[GP_INLDO[nTILE]].mi(INLDO_Current[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			LDO_Shutdown[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[nTILE][nSiteIndex]-I_PWRDIS[nSiteIndex];
		
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 29+nTILE, 1e3*LDO_Shutdown[nTILE][nSiteIndex]);
		}
	}
	

	//************LDO12_CP_On_Normal**********
	Relay.BIT[K14_OUTLDO_Cap].set();
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	SDASCL_Connect_I2C();
	delayms(2);	
	Reg[0x1006].write_CMI();
	Reg[0x1092].set_clr_inv(BIT1|BIT0);
	Reg[0x1093].write(0xcc);
	SDASCL_Connect_VI();
	delayms(15);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	delayms(25);
	Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
	Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
	Fvi16.CH[GP_INLDO[0]].mi(INLDO_Current[0]);
	Fvi16.CH[GP_INLDO[1]].mi(INLDO_Current[1]);
	FOR_EACH_SITE(nSiteIndex)
		LDO12_On_CP_Normal[nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[0][nSiteIndex]+INLDO_Current[1][nSiteIndex]-I_PWRDIS[nSiteIndex];
				
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 31, 1e3*LDO12_On_CP_Normal[nSiteIndex]);
	}
	//************LDO12_CP_On_LPM**********
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	SDASCL_Connect_I2C();
	delayms(2);	
	Reg[0x1093].write(0x44);
	delayms(1);
	Reg[0x100a].write(0xfe);
	delayms(2);
	Reg[0x1006].write(0x05);
	delayms(2);
	Reg[0x1093].write(0xdd);
	delayms(10);
	SDASCL_Connect_VI();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	delayms(20);
	Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
	Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
	Fvi16.CH[GP_INLDO[0]].mi(INLDO_Current[0]);
	Fvi16.CH[GP_INLDO[1]].mi(INLDO_Current[1]);
	FOR_EACH_SITE(nSiteIndex)
		LDO12_On_CP_LPM[nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+INLDO_Current[0][nSiteIndex]+INLDO_Current[1][nSiteIndex]-I_PWRDIS[nSiteIndex];
				
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 32, 1e3*LDO12_On_CP_LPM[nSiteIndex]);
	}
	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);
	SDASCL_Connect_I2C();
	delayms(2);	
	Relay.BIT[K14_OUTLDO_Cap].clr();
	Fvi16.CH[VIN_LDO1]	.fv(0);
	Fvi16.CH[VIN_LDO2]	.fv(0);
	Fvi16.CH[VIN_LDO1]	.open();
	Fvi16.CH[VIN_LDO2]	.open();
	
	//************I_PWRDIS_State**********
	
	Reg[0x10f3].write(0x01);
	Reg[0x10f7].write(0x0e);

	Fvi16.CH[GPIO0].fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);

	Reg[0x10f1].write(0x01);
	SDASCL_Connect_VI();
	delayms(2);	
	Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	delayms(2);
	Fvi45.CH[VIN_3V3]	.mi(I_PWRDIS_State);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 33, 1e3*I_PWRDIS_State[nSiteIndex]);
	}
	Fvi16.CH[GPIO0].fi(0);
	Fvi16.CH[GPIO0].open();
		
	//Re-Boot the device for Buck_Iq check
	Fvi16.CH[SDA]	.open();
	Fvi16.CH[SCL]	.open();
	delayms(2);

	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VBYP]		.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	delayms(5);
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	i2c_init(HCTL_MODE,300000,2);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 34, Factory_Mode_Flag[nSiteIndex]);
	}
	
	//Re-Boot the device for Buck_Iq check
	//************Buck_Iqstd_LPM**********
	
	Buck_TM_Reset();
	delayms(2);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(2);

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{	
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{	
		//Buck_off, LPM=1;
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xc0|readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(2);
		SDASCL_Connect_VI();
		delayms(5);
		Fvi45.CH[VBYP]		.fv(0);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]]	.mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Buck_off[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex];
				pFunction->LogResult(NSITEINDEX, 4*nTILE+35, 1e3*Buck_off[nTILE][nSiteIndex]);				
			}
		}
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		delayms(2);
		SDASCL_Connect_I2C();
		delayms(5);
		//Save_Iqq=1,LPM=1;
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		//FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x10|readback[nSiteIndex]);
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
		delayms(1);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
		delayms(2);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x40);
		delayms(1);
		SDASCL_Connect_VI();
		delayms(2);
		Fvi45.CH[VBYP]		.fv(0);
		delayms(5);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		for(i=0;i<30;i++)
		{
			Fvi45.CH[VOUT_SW]	.mi(i_meas);
			delayms(2);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(1e3*i_meas[nSiteIndex]-21)<5&&Flag[nSiteIndex]==0)
				{
                    VOUTSW_Current[nSiteIndex]=i_meas[nSiteIndex];
					Flag[nSiteIndex]=1;
				}	
			}
			if(isAllFlagSet(false))
				break;
		}	
		clrAllFlag();
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]]	.mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Buck_IQSTD_LPM1[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-Buck_off[nTILE][nSiteIndex];
				pFunction->LogResult(NSITEINDEX, 4*nTILE+36, 1e3*Buck_IQSTD_LPM1[nTILE][nSiteIndex]);	
			}
		}
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		delayms(2);
		SDASCL_Connect_I2C();
		delayms(2);
		//Save_Iqq=1,LPM=0;
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);	
		Fvi45.CH[VBYP]		.fv(0);
		SDASCL_Connect_VI();
		delayms(2);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);	
		delayms(2);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);

		for(i=0;i<30;i++)
		{
			Fvi45.CH[VOUT_SW]	.mi(i_meas);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(1e3*i_meas[nSiteIndex]-61)<5&&Flag[nSiteIndex]==0)
				{
                    VOUTSW_Current[nSiteIndex]=i_meas[nSiteIndex];
					Flag[nSiteIndex]=1;
				}	
			}
			if(isAllFlagSet(false))
				break;
		}	
		clrAllFlag();

		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]]	.mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Buck_IQSTD_LPM0_Save1[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-Buck_off[nTILE][nSiteIndex];
				pFunction->LogResult(NSITEINDEX, 4*nTILE+37, 1e3*Buck_IQSTD_LPM0_Save1[nTILE][nSiteIndex]);	
			}
		}
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		delayms(2);
		SDASCL_Connect_I2C();
		delayms(2);
		//Save_Iqq=0,LPM=0;
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		//FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xef&readback[nSiteIndex]);
		//Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x20);

		SDASCL_Connect_VI();
		delayms(5);
		Fvi45.CH[VBYP]		.fv(0);
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		
		delayms(5);
		Fvi45.CH[VOUT_SW]	.mi(VOUTSW_Current);
		Fvi45.CH[VIN_3V3]	.mi(VIN3V3_Current);
		Fvi45.CH[GP_VIN_Buck[nTILE]]	.mi(Buck_VIN_Current);
		FOR_EACH_SITE(nSiteIndex)
		{
			if(PMU_ACOT_MODE[nTILE])
			{
				Buck_IQSTD_LPM0_Save0[nTILE][nSiteIndex]=VOUTSW_Current[nSiteIndex]+VIN3V3_Current[nSiteIndex]+Buck_VIN_Current[nSiteIndex]-Buck_off[nTILE][nSiteIndex];
				pFunction->LogResult(NSITEINDEX, 4*nTILE+38, 1e3*Buck_IQSTD_LPM0_Save0[nTILE][nSiteIndex]);		
			}
		}
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		delayms(2);
		SDASCL_Connect_I2C();
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2,BIT1|BIT0);
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
		Fvi45.CH[GP_VIN_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	//************Buck_Iqstd_LPM**********
	Relay.BIT[K48_I2C]		.clr();
	delayms(2);
	Fvi45.CH[VBYP]		.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	delayms(2);
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();

	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Master_GPIO
STT_TESTFUNC Master_GPIO(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_GPIO_params* ours)
{
	double I1_Measure[SITE_MAX]={0},I2_Measure[SITE_MAX]={0},V_Measure[SITE_MAX]={0};
	double SCL_PU_R_VFB5[4][SITE_MAX]={0},SDA_PU_R_VFB5[4][SITE_MAX]={0},SCL_PU_R_VOUTSW[4][SITE_MAX]={0},SDA_PU_R_VOUTSW[4][SITE_MAX]={0},SDA_Leak[SITE_MAX]={0};

	double nRESET_VOL_GPIONOD[SITE_MAX]={0},nRESET_RPUP_GPIONOD_OPT1[SITE_MAX]={0},nRESET_RPUP_GPIONOD_OPT2[SITE_MAX]={0},
		   nRESET_RPUP_GPIONOD_OPT3[SITE_MAX]={0},nRESET_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},nRESET_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},nRESET_VOL_GPIOCM_VIO[SITE_MAX]={0},nRESET_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double nIRQ_VOL_GPIONOD[SITE_MAX]={0},nIRQ_RPUP_GPIONOD_OPT1[SITE_MAX]={0},nIRQ_RPUP_GPIONOD_OPT2[SITE_MAX]={0},
		   nIRQ_RPUP_GPIONOD_OPT3[SITE_MAX]={0},nIRQ_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},nIRQ_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},nIRQ_VOL_GPIOCM_VIO[SITE_MAX]={0},nIRQ_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double CorePG_VOL_GPIONOD[SITE_MAX]={0},CorePG_RPUP_GPIONOD_OPT1[SITE_MAX]={0},CorePG_RPUP_GPIONOD_OPT2[SITE_MAX]={0},
		   CorePG_RPUP_GPIONOD_OPT3[SITE_MAX]={0},CorePG_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},CorePG_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},CorePG_VOL_GPIOCM_VIO[SITE_MAX]={0},CorePG_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double LPM_VIH_GPIO_VOUTSW[SITE_MAX]={0},LPM_VIL_GPIO_VOUTSW[SITE_MAX]={0},LPM_HYS_GPIO_VOUTSW[SITE_MAX]={0},LPM_VIH_GPIO_MVIO[SITE_MAX]={0},LPM_VIL_GPIO_MVIO[SITE_MAX]={0},LPM_HYS_GPIO_MVIO[SITE_MAX]={0},
		   LPM_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},LPM_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},LPM_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   LPM_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},LPM_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},LPM_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   LPM_RPUP_GPIONOD_OPT1[SITE_MAX]={0},LPM_RPUP_GPIONOD_OPT2[SITE_MAX]={0},LPM_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   LPM_VOL_GPIONOD[SITE_MAX]={0},LPM_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},LPM_VOL_GPIOCM_VIO[SITE_MAX]={0},LPM_VOL_GPIO_LED[SITE_MAX]={0},LPM_Rdis_GPBLD[SITE_MAX]={0},
		   LPM_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},LPM_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double PWRDN0_VIH_GPIO_VOUTSW[SITE_MAX]={0},PWRDN0_VIL_GPIO_VOUTSW[SITE_MAX]={0},PWRDN0_HYS_GPIO_VOUTSW[SITE_MAX]={0},PWRDN0_VIH_GPIO_MVIO[SITE_MAX]={0},PWRDN0_VIL_GPIO_MVIO[SITE_MAX]={0},PWRDN0_HYS_GPIO_MVIO[SITE_MAX]={0},
	       PWRDN0_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},PWRDN0_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},PWRDN0_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   PWRDN0_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},PWRDN0_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},PWRDN0_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   PWRDN0_RPUP_GPIONOD_OPT1[SITE_MAX]={0},PWRDN0_RPUP_GPIONOD_OPT2[SITE_MAX]={0},PWRDN0_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   PWRDN0_VOL_GPIONOD[SITE_MAX]={0},PWRDN0_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},PWRDN0_VOL_GPIOCM_VIO[SITE_MAX]={0},PWRDN0_VOL_GPIO_LED[SITE_MAX]={0},PWRDN0_Rdis_GPBLD[SITE_MAX]={0},
		   PWRDN0_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},PWRDN0_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double PWRDN1_VIH_GPIO_VOUTSW[SITE_MAX]={0},PWRDN1_VIL_GPIO_VOUTSW[SITE_MAX]={0},PWRDN1_HYS_GPIO_VOUTSW[SITE_MAX]={0},PWRDN1_VIH_GPIO_MVIO[SITE_MAX]={0},PWRDN1_VIL_GPIO_MVIO[SITE_MAX]={0},PWRDN1_HYS_GPIO_MVIO[SITE_MAX]={0},
	       PWRDN1_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},PWRDN1_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},PWRDN1_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   PWRDN1_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},PWRDN1_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},PWRDN1_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   PWRDN1_RPUP_GPIONOD_OPT1[SITE_MAX]={0},PWRDN1_RPUP_GPIONOD_OPT2[SITE_MAX]={0},PWRDN1_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   PWRDN1_VOL_GPIONOD[SITE_MAX]={0},PWRDN1_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},PWRDN1_VOL_GPIOCM_VIO[SITE_MAX]={0},PWRDN1_VOL_GPIO_LED[SITE_MAX]={0},PWRDN1_Rdis_GPBLD[SITE_MAX]={0},
		   PWRDN1_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},PWRDN1_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double EXTEN_VIH_GPIO_VOUTSW[SITE_MAX]={0},EXTEN_VIL_GPIO_VOUTSW[SITE_MAX]={0},EXTEN_HYS_GPIO_VOUTSW[SITE_MAX]={0},EXTEN_VIH_GPIO_MVIO[SITE_MAX]={0},EXTEN_VIL_GPIO_MVIO[SITE_MAX]={0},EXTEN_HYS_GPIO_MVIO[SITE_MAX]={0},
	       EXTEN_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},EXTEN_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},EXTEN_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   EXTEN_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},EXTEN_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},EXTEN_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   EXTEN_RPUP_GPIONOD_OPT1[SITE_MAX]={0},EXTEN_RPUP_GPIONOD_OPT2[SITE_MAX]={0},EXTEN_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   EXTEN_VOL_GPIONOD[SITE_MAX]={0},EXTEN_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},EXTEN_VOL_GPIOCM_VIO[SITE_MAX]={0},EXTEN_VOL_GPIO_LED[SITE_MAX]={0},EXTEN_Rdis_GPBLD[SITE_MAX]={0},
		   EXTEN_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},EXTEN_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double EXTPG_VIH_GPIO_VOUTSW[SITE_MAX]={0},EXTPG_VIL_GPIO_VOUTSW[SITE_MAX]={0},EXTPG_HYS_GPIO_VOUTSW[SITE_MAX]={0},EXTPG_VIH_GPIO_MVIO[SITE_MAX]={0},EXTPG_VIL_GPIO_MVIO[SITE_MAX]={0},EXTPG_HYS_GPIO_MVIO[SITE_MAX]={0},
	       EXTPG_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},EXTPG_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},EXTPG_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   EXTPG_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},EXTPG_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},EXTPG_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   EXTPG_RPUP_GPIONOD_OPT1[SITE_MAX]={0},EXTPG_RPUP_GPIONOD_OPT2[SITE_MAX]={0},EXTPG_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   EXTPG_VOL_GPIONOD[SITE_MAX]={0},EXTPG_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},EXTPG_VOL_GPIOCM_VIO[SITE_MAX]={0},EXTPG_VOL_GPIO_LED[SITE_MAX]={0},EXTPG_Rdis_GPBLD[SITE_MAX]={0},
		   EXTPG_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},EXTPG_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double GPIO0_VIH_GPIO_VOUTSW[SITE_MAX]={0},GPIO0_VIL_GPIO_VOUTSW[SITE_MAX]={0},GPIO0_HYS_GPIO_VOUTSW[SITE_MAX]={0},GPIO0_VIH_GPIO_MVIO[SITE_MAX]={0},GPIO0_VIL_GPIO_MVIO[SITE_MAX]={0},GPIO0_HYS_GPIO_MVIO[SITE_MAX]={0},
	       GPIO0_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},GPIO0_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},GPIO0_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   GPIO0_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},GPIO0_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},GPIO0_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   GPIO0_RPUP_GPIONOD_OPT1[SITE_MAX]={0},GPIO0_RPUP_GPIONOD_OPT2[SITE_MAX]={0},GPIO0_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   GPIO0_VOL_GPIONOD[SITE_MAX]={0},GPIO0_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},GPIO0_VOL_GPIOCM_VIO[SITE_MAX]={0},GPIO0_VOL_GPIO_LED[SITE_MAX]={0},GPIO0_Rdis_GPBLD[SITE_MAX]={0},
		   GPIO0_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},GPIO0_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double GPIO1_VIH_GPIO_VOUTSW[SITE_MAX]={0},GPIO1_VIL_GPIO_VOUTSW[SITE_MAX]={0},GPIO1_HYS_GPIO_VOUTSW[SITE_MAX]={0},GPIO1_VIH_GPIO_MVIO[SITE_MAX]={0},GPIO1_VIL_GPIO_MVIO[SITE_MAX]={0},GPIO1_HYS_GPIO_MVIO[SITE_MAX]={0},
	       GPIO1_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},GPIO1_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},GPIO1_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   GPIO1_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},GPIO1_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},GPIO1_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0},	   
		   GPIO1_RPUP_GPIONOD_OPT1[SITE_MAX]={0},GPIO1_RPUP_GPIONOD_OPT2[SITE_MAX]={0},GPIO1_RPUP_GPIONOD_OPT3[SITE_MAX]={0},
		   GPIO1_VOL_GPIONOD[SITE_MAX]={0},GPIO1_VOL_GPIOCM_VOUTSW[SITE_MAX]={0},GPIO1_VOL_GPIOCM_VIO[SITE_MAX]={0},GPIO1_VOL_GPIO_LED[SITE_MAX]={0},GPIO1_Rdis_GPBLD[SITE_MAX]={0},
		   GPIO1_VOH_GPIOCM_VOUTSW[SITE_MAX]={0},GPIO1_VOH_GPIOCM_VIO[SITE_MAX]={0};
	double GATE_VIH_GPIO_VOUTSW[SITE_MAX]={0},GATE_VIL_GPIO_VOUTSW[SITE_MAX]={0},GATE_HYS_GPIO_VOUTSW[SITE_MAX]={0},GATE_VIH_GPIO_MVIO[SITE_MAX]={0},GATE_VIL_GPIO_MVIO[SITE_MAX]={0},GATE_HYS_GPIO_MVIO[SITE_MAX]={0},
	       GATE_RPUP_GPIO_VOUTSW_OPT1[SITE_MAX]={0},GATE_RPUP_GPIO_VOUTSW_OPT2[SITE_MAX]={0},GATE_RPUP_GPIO_VOUTSW_OPT3[SITE_MAX]={0},
		   GATE_RPUP_GPIO_MVIO_OPT1[SITE_MAX]={0},GATE_RPUP_GPIO_MVIO_OPT2[SITE_MAX]={0},GATE_RPUP_GPIO_MVIO_OPT3[SITE_MAX]={0};   

	double VIHPWRDIS[SITE_MAX]={0};
	double VILPWRDIS[SITE_MAX]={0},PWRDIS_HYS[SITE_MAX]={0},RPDPWRDIS[4][SITE_MAX]={0};
	double limit_adjust=0, V_FBB5_VIO[SITE_MAX]={0},V_VOUTSW[SITE_MAX]={0};

	unsigned char PU_R_Settiing_1[4]={0x40,0x50,0x60,0x70},PU_R_Settiing_2[4]={0x80,0x90,0xa0,0xb0};
    //FTP Limit value
	double Master_Sub1_HighLi[301]={0},Master_Sub1_LowLi[301]={0};
    for(i=0;i<300;i++)
	{
		pFunction->GetHiLimit(i,&Master_Sub1_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Master_Sub1_LowLi[i+1]);//item 1-48
	}
	Connect_all_cap();
	delayms(1);
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	delayms(1);
	Fvi45.CH[VOUT_SW]		.fv(2.6);
	Fvi45.CH[VIN_3V3]		.fv(2.6);
	delayms(1);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
	delayms(2);
	Relay.BIT[K48_I2C]		.set();
	delayms(5);
	Reg[0x1012].write(0xff);
	Reg[0x1086].write(0xff);//Add for 90522-101 cmi on 12/24/2020, some units fail on SDA/SCL pull-up resistor check
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	//mask IRQ nRESET
	Reg[0x1037].write(0x7f);
	Reg[0x1040].write(0xff);
	Reg[0x1050].write(0xff);
	Reg[0x1060].write(0xff);
	Reg[0x1070].write(0xff);
	Reg[0x1080].write(0xff);
	Reg[0x1091].write(0xff);
	//mask IRQ nRESET
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
	delayms(1);
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Fvi16.CH[nIRQ]	.fi(0,SITE_ALL,Stall3V3,Stall5mA);
	Fvi16.CH[nRESET].fi(0,SITE_ALL,Stall3V3,Stall5mA);
	delayms(2);
	Reg[0x1085].write(0x45);
	Reg[0x1016].write(0x40);
	Reg[0x1081].write(0x78);
	Reg[0x1082].write(0x78);
	Reg[0x1083].write(0x03);
	Reg[0x1084].write(_0x00);
	Reg[0x1085].write(0x45);
	delayms(1);
	Fvi45.CH[FB5]		.fv(1.8,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VBYP]		.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	delayms(2);
	Fvi45.CH[FB5]		.mv(V_FBB5_VIO);
	Fvi45.CH[VOUT_SW]	.mv(V_VOUTSW);

	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 2, Factory_Mode_Flag[nSiteIndex]);
	}
	//************SDASCL_Check**********
	for(i=0;i<4;i++)
	{
		Reg[0x1016].write(PU_R_Settiing_1[i]|0x08);
		Reg[0x1016].write(PU_R_Settiing_1[i]|0x08);
		Reg[0x1016].write(PU_R_Settiing_1[i]|0x08);
		delayms(2);
		Relay.BIT[K48_I2C]		.clr();
		delayms(2);
		if(i==0)
		{
			Fvi16.CH[SDA]	.fv(0,SITE_ALL,Stall12V,Stall50uA);
			Fvi16.CH[SCL]	.fv(0,SITE_ALL,Stall12V,Stall50uA);
		}
		if(i==1)
		{
			Fvi16.CH[SDA]	.fv(0,SITE_ALL,Stall12V,Stall5mA);
			Fvi16.CH[SCL]	.fv(0,SITE_ALL,Stall12V,Stall5mA);
		}
		if(i==2||i==3)
		{
			Fvi16.CH[SDA]	.fv(0,SITE_ALL,Stall12V,Stall500uA);
			Fvi16.CH[SCL]	.fv(0,SITE_ALL,Stall12V,Stall500uA);
		}
		delayms(5);
		Fvi16.CH[SDA]	.mi(I1_Measure);
		Fvi16.CH[SCL]	.mi(I2_Measure);
		
		FOR_EACH_SITE(nSiteIndex)
		{
			SCL_PU_R_VFB5[i][nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
			SDA_PU_R_VFB5[i][nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I2_Measure[nSiteIndex]);
			if(i==0&&SCL_PU_R_VFB5[i][nSiteIndex]>999)
				SCL_PU_R_VFB5[i][nSiteIndex]=99999;
			if(i==0&&SDA_PU_R_VFB5[i][nSiteIndex]>999)
				SDA_PU_R_VFB5[i][nSiteIndex]=99999;
		}	
		SDASCL_Connect_I2C();
		//i2c_init(HCTL_MODE,300000,2);
		//delayms(2);
		Reg[0x1016].write(PU_R_Settiing_2[i]|0x08);
		Reg[0x1016].write(PU_R_Settiing_2[i]|0x08);
		Reg[0x1016].write(PU_R_Settiing_2[i]|0x08);
		delayms(2);
		SDASCL_Connect_VI();
		delayms(2);
		Fvi16.CH[SDA]	.mi(I1_Measure);
		Fvi16.CH[SCL]	.mi(I2_Measure);
		FOR_EACH_SITE(nSiteIndex)
		{
			SCL_PU_R_VOUTSW[i][nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
			SDA_PU_R_VOUTSW[i][nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I2_Measure[nSiteIndex]);
			if(i==0&&SCL_PU_R_VOUTSW[i][nSiteIndex]>999)
				SCL_PU_R_VOUTSW[i][nSiteIndex]=99999;
			if(i==0&&SDA_PU_R_VOUTSW[i][nSiteIndex]>999)
				SDA_PU_R_VOUTSW[i][nSiteIndex]=99999;

		}	
		SDASCL_Connect_I2C();
		delayms(1);
		//i2c_init(HCTL_MODE,300000,2);
		//delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 3+i*4, SCL_PU_R_VFB5[i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4+i*4, SDA_PU_R_VFB5[i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 5+i*4, SCL_PU_R_VOUTSW[i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 6+i*4, SDA_PU_R_VOUTSW[i][nSiteIndex]);
		}
	}
	//************SDASCL_Check**********
	//************nRESET_Check**********
	TestNO_target=19;
	//nRESET_VOL_GPIONOD
	Reg[0x1016].write(0x48);
	Reg[0x1018].write(_0x00);
	delayms(2);

	delayms(1);
	Fvi16.CH[nRESET]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[nRESET]	.mv(nRESET_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, 1e3*nRESET_VOL_GPIONOD[nSiteIndex]);
	}
	//nRESET_RPUP_GPIONOD
	Fvi16.CH[nRESET]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(2);
	Reg[0x1016].write(0x40);
	delayms(1);
	Fvi16.CH[nRESET]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nRESET_RPUP_GPIONOD_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(nRESET_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			nRESET_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1016].write(0x42);
	delayms(1);
	Fvi16.CH[nRESET]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		nRESET_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1016].write(0x43);
	delayms(1);
	Fvi16.CH[nRESET]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nRESET_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1016].write(0x40);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, nRESET_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, nRESET_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, nRESET_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//nRESET_VOL_GPIOCM_VOUTSW
	Reg[0x1018].write(0x20);
	Reg[0x1016].write(0x4c);
	Fvi16.CH[nRESET]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[nRESET]	.mv(nRESET_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*nRESET_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//nRESET_VOH_GPIOCM_VOUTSW
	Reg[0x1016].write(0x44);
	Fvi16.CH[nRESET]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[nRESET]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nRESET_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*nRESET_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//nRESET_VOL_GPIOCM_VIO
	Reg[0x1018].write(_0x00);
	Reg[0x1016].write(0x4c);
	Fvi16.CH[nRESET]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[nRESET]	.mv(nRESET_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*nRESET_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//nRESET_VOH_GPIOCM_VIO
	Reg[0x1016].write(0x44);
	Fvi16.CH[nRESET]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[nRESET]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nRESET_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*nRESET_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	Reg[0x1016].write(0x40);

	Fvi16.CH[nRESET]	.fi(0);
	Fvi16.CH[nRESET]	.open();
	//************nRESET_Check**********
	//************nIRQ_Check**********
	TestNO_target=27;
	Reg[0x1018].write(_0x00);
	Reg[0x1017].write(0x08);
	Fvi16.CH[nIRQ]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(2);
	Fvi16.CH[nIRQ]	.mv(nIRQ_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, 1e3*nIRQ_VOL_GPIONOD[nSiteIndex]);
	}
	//nIRQ_RPUP_GPIONOD
	Fvi16.CH[nIRQ]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(2);
	Reg[0x1017].write(_0x00);
	delayms(1);
	Fvi16.CH[nIRQ]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nIRQ_RPUP_GPIONOD_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(nIRQ_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			nIRQ_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1017].write(0x02);
	delayms(1);
	Fvi16.CH[nIRQ]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		nIRQ_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1017].write(0x03);
	delayms(1);
	Fvi16.CH[nIRQ]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nIRQ_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1017].write(_0x00);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, nIRQ_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, nIRQ_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, nIRQ_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//nIRQ_VOL_GPIOCM_VOUTSW
	Reg[0x1018].write(0x40);
	Reg[0x1017].write(0x0c);
	Fvi16.CH[nIRQ]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[nIRQ]	.mv(nIRQ_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*nIRQ_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//nIRQ_VOH_GPIOCM_VOUTSW
	Reg[0x1017].write(0x04);
	Fvi16.CH[nIRQ]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[nIRQ]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nIRQ_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*nIRQ_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//nIRQ_VOL_GPIOCM_VIO
	Reg[0x1018].write(_0x00);
	Reg[0x1017].write(0x0c);
	Fvi16.CH[nIRQ]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[nIRQ]	.mv(nIRQ_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*nIRQ_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//nIRQ_VOH_GPIOCM_VIO
	Reg[0x1017].write(0x04);
	Fvi16.CH[nIRQ]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[nIRQ]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		nIRQ_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*nIRQ_VOH_GPIOCM_VIO[nSiteIndex]);
	}

	Fvi16.CH[nIRQ]	.fi(0);
	Fvi16.CH[nIRQ]	.open();
	//************nIRQ_Check**********
	//************CorePG_Check**********
	TestNO_target=35;

	//CorePG_VOL_GPIONOD
	Reg[0x1017].write(0x80);
	Reg[0x1018].write(_0x00);
	Fvi16.CH[CORE_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mv(CorePG_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, 1e3*CorePG_VOL_GPIONOD[nSiteIndex]);
	}
	//CorePG_RPUP_GPIONOD
	Fvi16.CH[CORE_PG]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(2);
	Reg[0x1017].write(_0x00);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		CorePG_RPUP_GPIONOD_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(CorePG_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			CorePG_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1017].write(0x20);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		CorePG_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1017].write(0x30);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		CorePG_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1017].write(_0x00);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, CorePG_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, CorePG_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, CorePG_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//CorePG_VOL_GPIOCM_VOUTSW
	Reg[0x1018].write(0x80);
	Reg[0x1017].write(0xc0);
	Fvi16.CH[CORE_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mv(CorePG_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, 1e3*CorePG_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//CorePG_VOH_GPIOCM_VOUTSW
	Reg[0x1017].write(0x40);
	Fvi16.CH[CORE_PG]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		CorePG_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, 1e3*CorePG_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//CorePG_VOL_GPIOCM_VIO
	Reg[0x1018].write(_0x00);
	Reg[0x1017].write(0xc0);
	Fvi16.CH[CORE_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mv(CorePG_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, 1e3*CorePG_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//CorePG_VOH_GPIOCM_VIO
	Reg[0x1017].write(0x40);
	Fvi16.CH[CORE_PG]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[CORE_PG]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		CorePG_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 1e3*CorePG_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	Reg[0x1017].write_CMI();
	Reg[0x1016].write(0x40);

	Fvi16.CH[CORE_PG]	.fi(0);
	Fvi16.CH[CORE_PG]	.open();
	//************CorePG_Check**********
	//************LPM_Check**********
	TestNO_target=43;
    double Vstep=0.005;
	//LPM_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x08);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x1031].write(0x0c);
	Reg[0x1032].write(0x80);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x08);

	
	//LPM_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);

	//VIH
	Fvi16.CH[LPM]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		LPM_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[LPM]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		LPM_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		LPM_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(LPM_VIH_GPIO_VOUTSW[nSiteIndex]-LPM_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//LPM_VIHL_GPIO_MVIO

	Reg[0x1031].write(0x08);
	//VIH
	Fvi16.CH[LPM]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		LPM_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[LPM]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		LPM_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		LPM_HYS_GPIO_MVIO[nSiteIndex]=1e3*(LPM_VIH_GPIO_MVIO[nSiteIndex]-LPM_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LPM_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LPM_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, LPM_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//LPM_RPUP_GPIO_VOUTSW
	Reg[0x1031].write(0x0c);
	Reg[0x1034].write(0x08);
	Fvi16.CH[LPM]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(LPM_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			LPM_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1034].write(0x48);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1034].write(0x68);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, LPM_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, LPM_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, LPM_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//LPM_RPUP_GPIO_MVIO

	Reg[0x1031].write(0x08);
	Reg[0x1034].write(0x08);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(LPM_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			LPM_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1034].write(0x48);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1034].write(0x68);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, LPM_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, LPM_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, LPM_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}
	
	//LPM_VOL_GPIONOD
	Reg[0x1031].write(0x0d);
	Reg[0x1032].write(0x0b);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x08);
	Fvi16.CH[LPM]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(LPM_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*LPM_VOL_GPIONOD[nSiteIndex]);
	}
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);

	//LPM_RPUP_GPIONOD
	Fvi16.CH[LPM]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x1034].write(0x08);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(LPM_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			LPM_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1034].write(0x48);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1034].write(0x68);
	delayms(1);
	Fvi16.CH[LPM]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, LPM_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, LPM_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, LPM_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//LPM_VOL_GPIOCM_VOUTSW
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(0x2b);

	Fvi16.CH[LPM]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(LPM_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*LPM_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//LPM_VOH_GPIOCM_VOUTSW
	Reg[0x1032].write(0x0b);
	Fvi16.CH[LPM]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*LPM_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//LPM_VOL_GPIOCM_VIO
	Reg[0x1031].write(0x0b);
	Reg[0x1032].write(0x2b);
	Fvi16.CH[LPM]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(LPM_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*LPM_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//LPM_VOH_GPIOCM_VIO
	Reg[0x1032].write(0x0b);
	Fvi16.CH[LPM]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*LPM_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//LPM_RDIS_GPBLD
	Reg[0x1031].write(0x09);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x88);
	Fvi16.CH[LPM]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[LPM]	.mv(V_Measure);
	Fvi16.CH[LPM]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, LPM_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[LPM]	.fi(0);
	Reg[0x1034].write(0x08);
	//LPM_VOL_GPIOLED
	Reg[0x1031].write(0x0d);
	Reg[0x1032].write(0x2b);
	Fvi16.CH[LPM]	.fi(30);
	delayms(1);
	Fvi16.CH[LPM]	.mv(LPM_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*LPM_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[LPM]	.fi(0);
	Fvi16.CH[LPM]	.open();
	//************LPM_Check**********
	//************PWRDN0_Check**********
	TestNO_target=65;
    Vstep=0.005;
	//PWRDN0_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x09);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x1029].write(0x0c);
	Reg[0x102a].write(0x80);
	Reg[0x102b].write(_0x00);
	Reg[0x102c].write(0x08);

	
	//PWRDN0_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0);

	//VIH
	Fvi16.CH[PWRDN0]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		PWRDN0_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[PWRDN0]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		PWRDN0_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN0_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(PWRDN0_VIH_GPIO_VOUTSW[nSiteIndex]-PWRDN0_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, PWRDN0_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, PWRDN0_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, PWRDN0_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//PWRDN0_VIHL_GPIO_MVIO

	Reg[0x1029].write(0x08);
	//VIH
	Fvi16.CH[PWRDN0]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		PWRDN0_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[PWRDN0]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		PWRDN0_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN0_HYS_GPIO_MVIO[nSiteIndex]=1e3*(PWRDN0_VIH_GPIO_MVIO[nSiteIndex]-PWRDN0_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, PWRDN0_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, PWRDN0_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, PWRDN0_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//PWRDN0_RPUP_GPIO_VOUTSW
	Reg[0x1029].write(0x0c);
	Reg[0x102c].write(0x08);
	Fvi16.CH[PWRDN0]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			PWRDN0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x102c].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x102c].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, PWRDN0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, PWRDN0_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, PWRDN0_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//PWRDN0_RPUP_GPIO_MVIO

	Reg[0x1029].write(0x08);
	Reg[0x102c].write(0x08);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			PWRDN0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x102c].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x102c].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, PWRDN0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, PWRDN0_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, PWRDN0_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}
	
	//PWRDN0_VOL_GPIONOD
	Reg[0x1029].write(0x0d);
	Reg[0x102a].write(0x0b);
	Reg[0x102b].write(_0x00);
	Reg[0x102c].write(0x08);
	Fvi16.CH[PWRDN0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(PWRDN0_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*PWRDN0_VOL_GPIONOD[nSiteIndex]);
	}
	//PWRDN0_RPUP_GPIONOD
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);
	Fvi16.CH[PWRDN0]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x102c].write(0x08);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN0_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			PWRDN0_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x102c].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x102c].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, PWRDN0_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, PWRDN0_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, PWRDN0_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//PWRDN0_VOL_GPIOCM_VOUTSW
	Reg[0x1029].write(0x0f);
	Reg[0x102a].write(0x2b);

	Fvi16.CH[PWRDN0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(PWRDN0_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*PWRDN0_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//PWRDN0_VOH_GPIOCM_VOUTSW
	Reg[0x102a].write(0x0b);
	Fvi16.CH[PWRDN0]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*PWRDN0_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//PWRDN0_VOL_GPIOCM_VIO
	Reg[0x1029].write(0x0b);
	Reg[0x102a].write(0x2b);
	Fvi16.CH[PWRDN0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(PWRDN0_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*PWRDN0_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//PWRDN0_VOH_GPIOCM_VIO
	Reg[0x102a].write(0x0b);
	Fvi16.CH[PWRDN0]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*PWRDN0_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//PWRDN0_RDIS_GPBLD
	Reg[0x1029].write(0x09);
	Reg[0x102a].write(_0x00);
	Reg[0x102b].write(_0x00);
	Reg[0x102c].write(0x88);
	Fvi16.CH[PWRDN0]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(V_Measure);
	Fvi16.CH[PWRDN0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, PWRDN0_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[PWRDN0]	.fi(0);
	Reg[0x102c].write(0x08);

	//PWRDN0_VOL_GPIOLED
	Reg[0x1029].write(0x0d);
	Reg[0x102a].write(0x2b);
	Fvi16.CH[PWRDN0]	.fi(25);
	delayms(1);
	Fvi16.CH[PWRDN0]	.mv(PWRDN0_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*PWRDN0_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[PWRDN0]	.fi(0);
	Fvi16.CH[PWRDN0]	.open();
	//************PWRDN0_Check**********
	//************PWRDN1_Check**********
	TestNO_target=87;
    Vstep=0.005;
	//PWRDN1_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0a);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x102d].write(0x0c);
	Reg[0x102e].write(0x80);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x08);

	
	//PWRDN1_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0);

	//VIH
	Fvi16.CH[PWRDN1]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		PWRDN1_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[PWRDN1]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		PWRDN1_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN1_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(PWRDN1_VIH_GPIO_VOUTSW[nSiteIndex]-PWRDN1_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, PWRDN1_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, PWRDN1_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, PWRDN1_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//PWRDN1_VIHL_GPIO_MVIO

	Reg[0x102d].write(0x08);
	//VIH
	Fvi16.CH[PWRDN1]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		PWRDN1_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[PWRDN1]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		PWRDN1_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN1_HYS_GPIO_MVIO[nSiteIndex]=1e3*(PWRDN1_VIH_GPIO_MVIO[nSiteIndex]-PWRDN1_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, PWRDN1_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, PWRDN1_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, PWRDN1_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//PWRDN1_RPUP_GPIO_VOUTSW
	Reg[0x102d].write(0x0c);
	Reg[0x1030].write(0x08);
	Fvi16.CH[PWRDN1]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			PWRDN1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1030].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1030].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, PWRDN1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, PWRDN1_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, PWRDN1_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//PWRDN1_RPUP_GPIO_MVIO

	Reg[0x102d].write(0x08);
	Reg[0x1030].write(0x08);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			PWRDN1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1030].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}

	Reg[0x1030].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, PWRDN1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, PWRDN1_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, PWRDN1_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}

	//PWRDN1_VOL_GPIONOD
	Reg[0x102d].write(0x0d);
	Reg[0x102e].write(0x0b);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x08);
	Fvi16.CH[PWRDN1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(PWRDN1_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*PWRDN1_VOL_GPIONOD[nSiteIndex]);
	}

	//PWRDN1_RPUP_GPIONOD
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);
	Fvi16.CH[PWRDN1]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x1030].write(0x08);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(PWRDN1_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			PWRDN1_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1030].write(0x48);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1030].write(0x68);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, PWRDN1_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, PWRDN1_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, PWRDN1_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//PWRDN1_VOL_GPIOCM_VOUTSW
	Reg[0x102d].write(0x0f);
	Reg[0x102e].write(0x2b);

	Fvi16.CH[PWRDN1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(PWRDN1_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*PWRDN1_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//PWRDN1_VOH_GPIOCM_VOUTSW
	Reg[0x102e].write(0x0b);
	Fvi16.CH[PWRDN1]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*PWRDN1_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//PWRDN1_VOL_GPIOCM_VIO
	Reg[0x102d].write(0x0b);
	Reg[0x102e].write(0x2b);
	Fvi16.CH[PWRDN1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(PWRDN1_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*PWRDN1_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//PWRDN1_VOH_GPIOCM_VIO
	Reg[0x102e].write(0x0b);
	Fvi16.CH[PWRDN1]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*PWRDN1_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//PWRDN1_RDIS_GPBLD
	Reg[0x102d].write(0x09);
	Reg[0x102e].write(_0x00);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x88);
	Fvi16.CH[PWRDN1]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(V_Measure);
	Fvi16.CH[PWRDN1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, PWRDN1_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[PWRDN1]	.fi(0);
	Reg[0x1030].write(0x08);

	//PWRDN1_VOL_GPIOLED
	Reg[0x102d].write(0x0d);
	Reg[0x102e].write(0x2b);
	Fvi16.CH[PWRDN1]	.fi(30);
	delayms(1);
	Fvi16.CH[PWRDN1]	.mv(PWRDN1_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*PWRDN1_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[PWRDN1]	.fi(0);
	Fvi16.CH[PWRDN1]	.open();
	//************PWRDN1_Check**********
	//************EXTEN_Check**********
	TestNO_target=109;
    Vstep=0.005;
	//EXTEN_VOL_GPIONOD
	Reg[0x102d].write(0x0b);
	Reg[0x102e].write(0x0b);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x08);

	Reg[0x101d].write(0x0c);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	
	//EXTEN_VIHL_GPIOM_VOUTSW
	Fvi16.CH[PWRDN1]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);

	//VIH
	Fvi16.CH[EXT_EN]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall50mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,PWRDN1,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		EXTEN_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[EXT_EN]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,PWRDN1,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		EXTEN_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		EXTEN_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(EXTEN_VIH_GPIO_VOUTSW[nSiteIndex]-EXTEN_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, EXTEN_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, EXTEN_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, EXTEN_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//EXTEN_VIHL_GPIO_MVIO

	Reg[0x101d].write(0x08);
	//VIH
	Fvi16.CH[EXT_EN]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,PWRDN1,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		EXTEN_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[EXT_EN]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,PWRDN1,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		EXTEN_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		EXTEN_HYS_GPIO_MVIO[nSiteIndex]=1e3*(EXTEN_VIH_GPIO_MVIO[nSiteIndex]-EXTEN_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, EXTEN_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, EXTEN_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, EXTEN_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[PWRDN1]	.open();
	//EXTEN_RPUP_GPIO_VOUTSW
	Reg[0x101d].write(0x0c);
	Reg[0x1020].write(0x08);
	Fvi16.CH[EXT_EN]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTEN_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			EXTEN_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1020].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1020].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, EXTEN_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, EXTEN_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, EXTEN_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//EXTEN_RPUP_GPIO_MVIO

	Reg[0x101d].write(0x08);
	Reg[0x1020].write(0x08);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTEN_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			EXTEN_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1020].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1020].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, EXTEN_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, EXTEN_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, EXTEN_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}
	
	//EXTEN_VOL_GPIONOD
	Reg[0x101d].write(0x0d);
	Reg[0x101e].write(0x0a);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	Fvi16.CH[EXT_EN]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(EXTEN_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*EXTEN_VOL_GPIONOD[nSiteIndex]);
	}
	Reg[0x102d].write(0x08);
	Reg[0x102e].write(0x80);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x48);
	//EXTEN_RPUP_GPIONOD
	Fvi16.CH[EXT_EN]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x1020].write(0x08);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTEN_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			EXTEN_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1020].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1020].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, EXTEN_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, EXTEN_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, EXTEN_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//EXTEN_VOL_GPIOCM_VOUTSW
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(0x2a);

	Fvi16.CH[EXT_EN]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(EXTEN_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*EXTEN_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//EXTEN_VOH_GPIOCM_VOUTSW
	Reg[0x101e].write(0x0a);
	Fvi16.CH[EXT_EN]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*EXTEN_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//EXTEN_VOL_GPIOCM_VIO
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x2a);
	Fvi16.CH[EXT_EN]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(EXTEN_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*EXTEN_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//EXTEN_VOH_GPIOCM_VIO
	Reg[0x101e].write(0x0a);
	Fvi16.CH[EXT_EN]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*EXTEN_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//EXTEN_RDIS_GPBLD
	Reg[0x101d].write(0x09);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x88);
	Fvi16.CH[EXT_EN]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(V_Measure);
	Fvi16.CH[EXT_EN]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, EXTEN_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[EXT_EN]	.fi(0);
	Reg[0x1020].write(0x08);

	//EXTEN_VOL_GPIOLED
	Reg[0x101d].write(0x0d);
	Reg[0x101e].write(0x2a);
	Fvi16.CH[EXT_EN]	.fi(30);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(EXTEN_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*EXTEN_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[EXT_EN]	.fi(0);
	Fvi16.CH[EXT_EN]	.open();
	//************EXTEN_Check**********
	//************EXTPG_Check**********
	TestNO_target=131;
    Vstep=0.005;
	//EXTPG_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0c);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x1019].write(0x0c);
	Reg[0x101a].write(0x80);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);

	
	//EXTPG_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);

	//VIH
	Fvi16.CH[EXT_PG]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		EXTPG_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[EXT_PG]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		EXTPG_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		EXTPG_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(EXTPG_VIH_GPIO_VOUTSW[nSiteIndex]-EXTPG_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, EXTPG_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, EXTPG_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, EXTPG_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//EXTPG_VIHL_GPIO_MVIO

	Reg[0x1019].write(0x08);
	//VIH
	Fvi16.CH[EXT_PG]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		EXTPG_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[EXT_PG]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		EXTPG_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		EXTPG_HYS_GPIO_MVIO[nSiteIndex]=1e3*(EXTPG_VIH_GPIO_MVIO[nSiteIndex]-EXTPG_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, EXTPG_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, EXTPG_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, EXTPG_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//EXTPG_RPUP_GPIO_VOUTSW
	Reg[0x1019].write(0x0c);
	Reg[0x101c].write(0x08);
	Fvi16.CH[EXT_PG]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTPG_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			EXTPG_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x101c].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x101c].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, EXTPG_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, EXTPG_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, EXTPG_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//EXTPG_RPUP_GPIO_MVIO

	Reg[0x1019].write(0x08);
	Reg[0x101c].write(0x08);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTPG_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			EXTPG_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x101c].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x101c].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, EXTPG_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, EXTPG_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, EXTPG_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}

	//EXTPG_VOL_GPIONOD
	Reg[0x1019].write(0x0d);
	Reg[0x101a].write(0x0b);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x48);
	Fvi16.CH[EXT_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(EXTPG_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*EXTPG_VOL_GPIONOD[nSiteIndex]);
	}
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);

	//EXTPG_RPUP_GPIONOD
	Fvi16.CH[EXT_PG]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x101c].write(0x08);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(EXTPG_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			EXTPG_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x101c].write(0x48);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x101c].write(0x68);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, EXTPG_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, EXTPG_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, EXTPG_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//EXTPG_VOL_GPIOCM_VOUTSW
	Reg[0x1019].write(0x0f);
	Reg[0x101a].write(0x2b);

	Fvi16.CH[EXT_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(EXTPG_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*EXTPG_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//EXTPG_VOH_GPIOCM_VOUTSW
	Reg[0x101a].write(0x0b);
	Fvi16.CH[EXT_PG]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*EXTPG_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//EXTPG_VOL_GPIOCM_VIO
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x2b);
	Fvi16.CH[EXT_PG]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(EXTPG_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*EXTPG_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//EXTPG_VOH_GPIOCM_VIO
	Reg[0x101a].write(0x0b);
	Fvi16.CH[EXT_PG]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*EXTPG_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//EXTPG_RDIS_GPBLD
	Reg[0x1019].write(0x09);
	Reg[0x101a].write(_0x00);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x88);
	Fvi16.CH[EXT_PG]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(V_Measure);
	Fvi16.CH[EXT_PG]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, EXTPG_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[EXT_PG]	.fi(0);
	Reg[0x101c].write(0x08);

	//EXTPG_VOL_GPIOLED
	Reg[0x1019].write(0x0d);
	Reg[0x101a].write(0x2b);
	Fvi16.CH[EXT_PG]	.fi(30);
	delayms(1);
	Fvi16.CH[EXT_PG]	.mv(EXTPG_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*EXTPG_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[EXT_PG]	.fi(0);
	Fvi16.CH[EXT_PG]	.open();
	//************EXTPG_Check**********
	//************GPIO0_Check**********
	
	TestNO_target=153;
    Vstep=0.005;
	//GPIO0_VOL_GPIONOD
	Reg[0x10f7].write(0x0c);

	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0e);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x1021].write(0x0c);
	Reg[0x1022].write(0x80);
	Reg[0x1023].write(_0x00);
	Reg[0x1024].write(0x08);

	
	//GPIO0_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0);

	//VIH
	Fvi16.CH[GPIO0]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		GPIO0_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[GPIO0]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GPIO0_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		GPIO0_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(GPIO0_VIH_GPIO_VOUTSW[nSiteIndex]-GPIO0_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, GPIO0_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, GPIO0_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, GPIO0_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//GPIO0_VIHL_GPIO_MVIO
	Reg[0x1021].write(0x08);
	//VIH
	Fvi16.CH[GPIO0]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		GPIO0_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[GPIO0]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GPIO0_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		GPIO0_HYS_GPIO_MVIO[nSiteIndex]=1e3*(GPIO0_VIH_GPIO_MVIO[nSiteIndex]-GPIO0_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, GPIO0_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, GPIO0_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, GPIO0_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//GPIO0_RPUP_GPIO_VOUTSW
	Reg[0x1021].write(0x0c);
	Reg[0x1024].write(0x08);
	Fvi16.CH[GPIO0]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			GPIO0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1024].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1024].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, GPIO0_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, GPIO0_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, GPIO0_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//GPIO0_RPUP_GPIO_MVIO

	Reg[0x1021].write(0x08);
	Reg[0x1024].write(0x08);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			GPIO0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1024].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1024].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, GPIO0_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, GPIO0_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, GPIO0_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}
	
	//GPIO0_VOL_GPIONOD
	Reg[0x1021].write(0x0d);
	Reg[0x1022].write(0x0b);
	Reg[0x1023].write(_0x00);
	Reg[0x1024].write(0x08);
	Fvi16.CH[GPIO0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(GPIO0_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GPIO0_VOL_GPIONOD[nSiteIndex]);
	}
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);
	//GPIO0_RPUP_GPIONOD
	Fvi16.CH[GPIO0]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x1024].write(0x08);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO0_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			GPIO0_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1024].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1024].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, GPIO0_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, GPIO0_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, GPIO0_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//GPIO0_VOL_GPIOCM_VOUTSW
	Reg[0x1021].write(0x0f);
	Reg[0x1022].write(0x2b);

	Fvi16.CH[GPIO0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(GPIO0_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*GPIO0_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//GPIO0_VOH_GPIOCM_VOUTSW
	Reg[0x1022].write(0x0b);
	Fvi16.CH[GPIO0]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*GPIO0_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//GPIO0_VOL_GPIOCM_VIO
	Reg[0x1021].write(0x0b);
	Reg[0x1022].write(0x2b);
	Fvi16.CH[GPIO0]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(GPIO0_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*GPIO0_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//GPIO0_VOH_GPIOCM_VIO
	Reg[0x1022].write(0x0b);
	Fvi16.CH[GPIO0]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*GPIO0_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//GPIO0_RDIS_GPBLD
	Reg[0x1021].write(0x09);
	Reg[0x1022].write(_0x00);
	Reg[0x1023].write(_0x00);
	Reg[0x1024].write(0x88);
	Fvi16.CH[GPIO0]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(V_Measure);
	Fvi16.CH[GPIO0]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, GPIO0_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[GPIO0]	.fi(0);
	Reg[0x1024].write(0x08);

	//GPIO0_VOL_GPIOLED
	Reg[0x1021].write(0x0d);
	Reg[0x1022].write(0x2b);
	Fvi16.CH[GPIO0]	.fi(30);
	delayms(1);
	Fvi16.CH[GPIO0]	.mv(GPIO0_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*GPIO0_VOL_GPIO_LED[nSiteIndex]);
	}
    
	Fvi16.CH[GPIO0]	.fi(0);
	Fvi16.CH[GPIO0]	.open();
	//************GPIO0_Check**********
	//************GPIO1_Check**********
	TestNO_target=175;
    Vstep=0.005;
	//GPIO1_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0f);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x1025].write(0x0c);
	Reg[0x1026].write(0x80);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x08);

	
	//GPIO1_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0);

	//VIH
	Fvi16.CH[GPIO1]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		GPIO1_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[GPIO1]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GPIO1_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		GPIO1_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(GPIO1_VIH_GPIO_VOUTSW[nSiteIndex]-GPIO1_VIL_GPIO_VOUTSW[nSiteIndex]);
	}


	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, GPIO1_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, GPIO1_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, GPIO1_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//GPIO1_VIHL_GPIO_MVIO
	Reg[0x1025].write(0x08);
	//VIH
	Fvi16.CH[GPIO1]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		GPIO1_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
	//VIL
	Fvi16.CH[GPIO1]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GPIO1_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		GPIO1_HYS_GPIO_MVIO[nSiteIndex]=1e3*(GPIO1_VIH_GPIO_MVIO[nSiteIndex]-GPIO1_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, GPIO1_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, GPIO1_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, GPIO1_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//GPIO1_RPUP_GPIO_VOUTSW
	Reg[0x1025].write(0x0c);
	Reg[0x1028].write(0x08);
	Fvi16.CH[GPIO1]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);

	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			GPIO1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1028].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1028].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, GPIO1_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, GPIO1_RPUP_GPIO_VOUTSW_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, GPIO1_RPUP_GPIO_VOUTSW_OPT3[nSiteIndex]);
	}
	//GPIO1_RPUP_GPIO_MVIO

	Reg[0x1025].write(0x08);
	Reg[0x1028].write(0x08);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			GPIO1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1028].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_MVIO_OPT2[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1028].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIO_MVIO_OPT3[nSiteIndex]=-V_FBB5_VIO[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, GPIO1_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+10, GPIO1_RPUP_GPIO_MVIO_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, GPIO1_RPUP_GPIO_MVIO_OPT3[nSiteIndex]);
	}

	//GPIO1_VOL_GPIONOD

	Reg[0x1025].write(0x0d);
	Reg[0x1026].write(0x0b);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x08);
	Fvi16.CH[GPIO1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(GPIO1_VOL_GPIONOD);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, 1e3*GPIO1_VOL_GPIONOD[nSiteIndex]);
	}
	//GPIO1_RPUP_GPIONOD

	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x48);

	Fvi16.CH[GPIO1]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	Reg[0x1028].write(0x08);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIONOD_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GPIO1_RPUP_GPIONOD_OPT1[nSiteIndex]>4000)
			GPIO1_RPUP_GPIONOD_OPT1[nSiteIndex]=99999;
	}
	Reg[0x1028].write(0x48);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIONOD_OPT2[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	Reg[0x1028].write(0x68);
	delayms(1);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_RPUP_GPIONOD_OPT3[nSiteIndex]=-V_VOUTSW[nSiteIndex]/I1_Measure[nSiteIndex];
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+13, GPIO1_RPUP_GPIONOD_OPT1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+14, GPIO1_RPUP_GPIONOD_OPT2[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+15, GPIO1_RPUP_GPIONOD_OPT3[nSiteIndex]);
	}
	//GPIO1_VOL_GPIOCM_VOUTSW
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(0x2b);

	Fvi16.CH[GPIO1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(GPIO1_VOL_GPIOCM_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+16, 1e3*GPIO1_VOL_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//GPIO1_VOH_GPIOCM_VOUTSW
	Reg[0x1026].write(0x0b);
	Fvi16.CH[GPIO1]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_VOH_GPIOCM_VOUTSW[nSiteIndex]=V_VOUTSW[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+17, 1e3*GPIO1_VOH_GPIOCM_VOUTSW[nSiteIndex]);
	}
	//GPIO1_VOL_GPIOCM_VIO
	Reg[0x1025].write(0x0b);
	Reg[0x1026].write(0x2b);
	Fvi16.CH[GPIO1]	.fi(1,SITE_ALL,Stall1V1,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(GPIO1_VOL_GPIOCM_VIO);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+18, 1e3*GPIO1_VOL_GPIOCM_VIO[nSiteIndex]);
	}
	//GPIO1_VOH_GPIOCM_VIO
	Reg[0x1026].write(0x0b);
	Fvi16.CH[GPIO1]	.fi(-1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(V_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_VOH_GPIOCM_VIO[nSiteIndex]=V_FBB5_VIO[nSiteIndex]-V_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+19, 1e3*GPIO1_VOH_GPIOCM_VIO[nSiteIndex]);
	}
	//GPIO1_RDIS_GPBLD
	Reg[0x1025].write(0x09);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x88);
	Fvi16.CH[GPIO1]	.fv(0.1,SITE_ALL,Stall1V1,Stall50mA);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(V_Measure);
	Fvi16.CH[GPIO1]	.mi(I1_Measure);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_Rdis_GPBLD[nSiteIndex]=1e3*V_Measure[nSiteIndex]/I1_Measure[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target+20, GPIO1_Rdis_GPBLD[nSiteIndex]);
	}
	Fvi16.CH[GPIO1]	.fi(0);
	Reg[0x1028].write(0x08);

	//GPIO1_VOL_GPIOLED
	Reg[0x1025].write(0x0d);
	Reg[0x1026].write(0x2b);
	Fvi16.CH[GPIO1]	.fi(30);
	delayms(1);
	Fvi16.CH[GPIO1]	.mv(GPIO1_VOL_GPIO_LED);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+21, 1e3*GPIO1_VOL_GPIO_LED[nSiteIndex]);
	}

	Fvi16.CH[GPIO1]	.fi(0);
	Fvi16.CH[GPIO1]	.open();

	//************GPIO1_Check**********
	//************GATE_Check**********
	TestNO_target=197;
    Vstep=0.005;
	//GATE_VOL_GPIONOD
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0d);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);

	Reg[0x10f3].write(0x01);
	Reg[0x10f2].write(0x05);
	
	//GATE_VIHL_GPIOM_VOUTSW
	Fvi16.CH[EXT_EN]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//VIH
	Fvi16.CH[GATE]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1,SITE_ALL,Stall3V3,Stall5mA);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GATE,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target],Master_Sub1_HighLi[TestNO_target],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex) 
		GATE_VIH_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
	delayms(1);
	//VIL
	Fvi16.CH[GATE]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
	delayms(1);

	sweep_VI(RESOURCE_FVI16,GATE,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+1],Master_Sub1_LowLi[TestNO_target+1],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GATE_VIL_GPIO_VOUTSW[nSiteIndex]=global_temp[nSiteIndex];
		GATE_HYS_GPIO_VOUTSW[nSiteIndex]=1e3*(GATE_VIH_GPIO_VOUTSW[nSiteIndex]-GATE_VIL_GPIO_VOUTSW[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, GATE_VIH_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, GATE_VIL_GPIO_VOUTSW[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, GATE_HYS_GPIO_VOUTSW[nSiteIndex]);

	}
	//GATE_VIHL_GPIO_MVIO
	Reg[0x10f2].write(0x04);
	//VIH
	Fvi16.CH[GATE]	.fv(Master_Sub1_LowLi[TestNO_target+3]-0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GATE,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_LowLi[TestNO_target+3],Master_Sub1_HighLi[TestNO_target+3],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex) 
		GATE_VIH_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];

	delayms(1);
	//VIL
	Fvi16.CH[GATE]	.fv(Master_Sub1_HighLi[TestNO_target+4]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GATE,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+4],Master_Sub1_LowLi[TestNO_target+4],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{	
		GATE_VIL_GPIO_MVIO[nSiteIndex]=global_temp[nSiteIndex];
		GATE_HYS_GPIO_MVIO[nSiteIndex]=1e3*(GATE_VIH_GPIO_MVIO[nSiteIndex]-GATE_VIL_GPIO_MVIO[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, GATE_VIH_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, GATE_VIL_GPIO_MVIO[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, GATE_HYS_GPIO_MVIO[nSiteIndex]);	
	}
	Fvi16.CH[EXT_EN]	.open();
	//GATE_RPUP_GPIO_VOUTSW
	Reg[0x10f2].write(0x05);

	Fvi16.CH[GATE]	.fv(0,SITE_ALL,Stall1V1,Stall50uA);
	delayms(1);
	Fvi16.CH[GATE]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GATE_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GATE_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]>4000)
			GATE_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]=99999;
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, GATE_RPUP_GPIO_VOUTSW_OPT1[nSiteIndex]);
	}
	//GATE_RPUP_GPIO_MVIO
	
	Reg[0x10f2].write(0x04);
	delayms(1);
	Fvi16.CH[GATE]	.mi(I1_Measure);
	FOR_EACH_SITE(nSiteIndex)
	{
		GATE_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=V_FBB5_VIO[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
		if(GATE_RPUP_GPIO_MVIO_OPT1[nSiteIndex]>4000)
			GATE_RPUP_GPIO_MVIO_OPT1[nSiteIndex]=99999;
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, GATE_RPUP_GPIO_MVIO_OPT1[nSiteIndex]);
	}
	Fvi16.CH[GATE]	.fi(0);
	Fvi16.CH[GATE]	.open();
	Fvi45.CH[VOUT_SW]		.fv(0);
	Fvi45.CH[VIN_3V3]		.fv(0);
	delayms(2);

	//************GATE_Check**********
	//************PWRDIS_Check**********
	/*
	TestNO_target=205;
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);

	Reg[0x1006].set_clr_inv(BIT2);//Add for 90520-103.E4B PWRDIS VIL
	Reg[0x10f3].write(0x01);
	Reg[0x10f2].write(0x08);
	Reg[0x10f7].write(0x02);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(_0x00);	
	Reg[0x1016].write(0x40);
	Reg[0x1081].write(0x78);
	Reg[0x1082].write(0x78);
	Reg[0x1083].write(0x03);
	Reg[0x1084].write(_0x00);
	Reg[0x1085].write(0x45);
	Reg[0x1085].read(readback);

	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, Factory_Mode_Flag[nSiteIndex]);
	}	
	delayms(1);
	Fvi45.CH[FB5]		.fv(1.8);
	Fvi45.CH[VBYP]		.fv(0);
	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, Factory_Mode_Flag[nSiteIndex]);
	}	
	Fvi16.CH[EXT_EN]	.fi(0,SITE_ALL,Stall3V3,Stall50uA);
	*/
	TestNO_target=205;
    Fvi45.CH[VBYP]        .fv(VIN_SET+1.5);
    Fvi45.CH[VOUT_SW]        .fv(VIN_SET);
    Fvi45.CH[VIN_3V3]        .fv(VIN_SET);
    delayms(2);
	Reg[0x10f2].write(0x08);//i2c_addr change back to 104 cmi set after configure it
	Reg[0x2000].read(readback);
    FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
    Reg[0x2000].write(readback); 

    Reg[0x1006].set_clr_inv(BIT2);//Add for 90520-103.E4B PWRDIS VIL
    Reg[0x10f3].write(0x01);
    Reg[0x10f7].write(0x02);
    Reg[0x1012].write(0xff);
    Reg[0x2007].write(0x80);
    Reg[0x1016].write(0x40);
    Reg[0x1081].write(0x78);
    Reg[0x1082].write(0x78);
    Reg[0x1083].write(0x03);
    Reg[0x1084].write(_0x00);
    Reg[0x1085].write(0x45);

    delayms(1);
    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
    {
        pFunction->LogResult(NSITEINDEX, TestNO_target, Factory_Mode_Flag[nSiteIndex]);
    }    
    delayms(1);
    Fvi45.CH[FB5]        .fv(1.8);
    Fvi45.CH[VBYP]        .fv(0);
    delayms(1);
    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
    {
        pFunction->LogResult(NSITEINDEX, TestNO_target+1, Factory_Mode_Flag[nSiteIndex]);
    }    
    Fvi16.CH[EXT_EN]    .fi(0,SITE_ALL,Stall3V3,Stall50uA);
	/*
	Reg[0x1043].set_clr_inv(BITNONE,BIT0);
	Reg[0x1053].set_clr_inv(BITNONE,BIT0);
	Reg[0x1063].set_clr_inv(BITNONE,BIT0);
	Reg[0x1073].set_clr_inv(BITNONE,BIT0);
	Reg[0x101d].write(0xcf);
	delayms(1);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(_0x00);
	Reg[0x1012].write(0xf7);
	Reg[0x1083].set_clr_inv(BITNONE,BIT0);
	*/
	
	Reg[0x10f7].write(0x0e);
    Reg[0x101d].write(0x0b);
    delayms(1);
    Reg[0x101e].write(0x0e);
    Reg[0x101f].write(_0x00);
    Reg[0x1020].write(0x08);
    Reg[0x1021].write(0x0c);
    Reg[0x1022].write(0x80);
    Reg[0x1023].write(_0x00);
    Reg[0x1024].write(0x08);
    delayms(2);
	Fvi16.CH[GPIO0]	.fv(0,SITE_ALL,Stall3V3,Stall50mA);
	delayms(1);
	/*
	Fvi16.CH[GPIO0]	.fv(1.35);
	delayms(1);
	Fvi16.CH[EXT_EN]	.mv(V_Measure);
	//printf("EXT_EN_Voltage1 = %3.3f\n",V_Measure[0]);
	delayms(1);
	Reg[0x1006].write(0x05);
	delayms(5);
	//VIHPWRDIS
	Fvi16.CH[EXT_EN]	.mv(V_Measure);

	FOR_EACH_SITE(nSiteIndex)
	{
		if(fabs(V_Measure[nSiteIndex]-VIN_SET)<0.1||fabs(V_Measure[nSiteIndex]-1.8)<0.1)
			VIHPWRDIS[nSiteIndex]=1;
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VIHPWRDIS[nSiteIndex]);
	}
	*/
	
	//VIHPWRDIS
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
        Master_Sub1_LowLi[TestNO_target+2],Master_Sub1_HighLi[TestNO_target+2],Vstep,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
    FOR_EACH_SITE(nSiteIndex)
    {
		VIHPWRDIS[nSiteIndex]=global_temp[nSiteIndex];
        pFunction->LogResult(NSITEINDEX, TestNO_target+2, VIHPWRDIS[nSiteIndex]);
    }
	

	//VILPWRDIS
	Fvi16.CH[GPIO0]	.fv(Master_Sub1_HighLi[TestNO_target+3]+0.1);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Master_Sub1_HighLi[TestNO_target+3],Master_Sub1_LowLi[TestNO_target+3],-Vstep,LESS_THAN,1.5,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex)
	{
		VILPWRDIS[nSiteIndex]=global_temp[nSiteIndex];
		PWRDIS_HYS[nSiteIndex]=1e3*(VIHPWRDIS[nSiteIndex]-VILPWRDIS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VILPWRDIS[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, PWRDIS_HYS[nSiteIndex]);

	}
	//RPDPWRDIS
	unsigned char RPD_Set[4]={0x02,0x06,0x0a,0x0e};
	Fvi16.CH[GPIO0]	.fv(VIN_SET,SITE_ALL,Stall3V3,Stall500uA);
	delayms(1);
	for(i=0;i<4;i++)
	{
		Reg[0x10f7].write(RPD_Set[i]);
		delayms(1);

		Fvi16.CH[GPIO0]	.mi(I1_Measure);
		FOR_EACH_SITE(nSiteIndex)
		{
			RPDPWRDIS[i][nSiteIndex]=V_VOUTSW[nSiteIndex]/fabs(I1_Measure[nSiteIndex]);
			if(RPDPWRDIS[i][nSiteIndex]>4000)
				RPDPWRDIS[i][nSiteIndex]=99999;
			pFunction->LogResult(NSITEINDEX, TestNO_target+5+i, RPDPWRDIS[i][nSiteIndex]);
		}
	}
	Fvi16.CH[GPIO0]	.fi(0);
	Fvi16.CH[GPIO0]	.open(0);
	Fvi16.CH[EXT_EN].open(0);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(5);	

	//************PWRDIS_Check**********
	//************VIN_3V3_UVLO**********
	TestNO_target=214;	
	Relay.BIT[K28_VIN_Cap]		.clr();
	double VIN_3V3_UVLO_R[SITE_MAX]={0},VIN_3V3_UVLO_F[SITE_MAX]={0},VIN_3V3_UVLO_HYS[SITE_MAX]={0};
	Fvi45.CH[VBYP]      .fv(1.2,SITE_ALL,SET_CLAMP,2,-2,40,-40);
	delayms(2);
	Fvi45.CH[VBYP]      .fv(1.2,SITE_ALL,SET_CLAMP,2,-2,0.4,-0.4);
	Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target]-0.2);
	Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target]-0.2);	
	delayms(2);
	if(GNG_Flag==0)
	{
		//Rise
		Vstart=Master_Sub1_LowLi[TestNO_target]-0.2;
		Vstop=Master_Sub1_HighLi[TestNO_target]+0.1;
		Vstep=0.005;
		Vforce=Vstart;
		for(i=0;i<(Vstop-Vstart)/Vstep;i++)
		{
			Fvi45.CH[VOUT_SW]	.fv(Vforce);
			Fvi45.CH[VIN_3V3]	.fv(Vforce);	
			delay(100);
			Fvi45.CH[VBYP]      .mi(I1_Measure);
			FOR_EACH_SITE(nSiteIndex)
			{
				if((fabs(1e3*I1_Measure[nSiteIndex]-120)<50)&&(Flag[nSiteIndex]==0))
				{
					VIN_3V3_UVLO_R[nSiteIndex]=Vforce-Vstep/2;
					Flag[nSiteIndex]=1;
				}
				if((fabs(1e3*I1_Measure[nSiteIndex]-120)>50)&&(Flag[nSiteIndex]==0))
				{
					VIN_3V3_UVLO_R[nSiteIndex]=Vforce;
				}
			}	
			Vforce+=Vstep;
			if(isAllFlagSet(false))
				break;	
		}
		clrAllFlag();
		//Fall Use GNG to check for sample test program to save test time temprory
		/*
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET);
		Fvi45.CH[VIN_3V3]	.fv(VIN_SET);	
		delayms(5);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.05);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.05);	
		delayms(5);	
		Vstart=Master_Sub1_HighLi[TestNO_target+1]+0.05;
		Vstop=Master_Sub1_LowLi[TestNO_target+1]-0.1;
		Vstep=-0.01;
		Vforce=Vstart;
		for(i=0;i<(Vstop-Vstart)/Vstep;i++)
		{
			Fvi45.CH[VOUT_SW]	.fv(Vforce);
			Fvi45.CH[VIN_3V3]	.fv(Vforce);	
			delayms(50);
			Fvi45.CH[VBYP]      .mi(I1_Measure);
			//printf("Current_VBYP = %3.3f\n",1e3*I1_Measure[0]);
			//printf("Voltage_VIN = %3.3f\n",Vforce);
			FOR_EACH_SITE(nSiteIndex)
			{
				if((fabs(1e3*I1_Measure[nSiteIndex]-240)<50)&&(Flag[nSiteIndex]==0))
				{
					VIN_3V3_UVLO_F[nSiteIndex]=Vforce-Vstep/2;
					VIN_3V3_UVLO_HYS[nSiteIndex]=1e3*(VIN_3V3_UVLO_R[nSiteIndex]-VIN_3V3_UVLO_F[nSiteIndex]);
					Flag[nSiteIndex]=1;
				}
				if((fabs(1e3*I1_Measure[nSiteIndex]-240)>50)&&(Flag[nSiteIndex]==0))
				{
					VIN_3V3_UVLO_F[nSiteIndex]=Vforce;
					VIN_3V3_UVLO_HYS[nSiteIndex]=1e3*(VIN_3V3_UVLO_R[nSiteIndex]-VIN_3V3_UVLO_F[nSiteIndex]);
				}
			}	
			Vforce+=Vstep;
			if(isAllFlagSet(false))
				break;	
		}
		clrAllFlag();
		*/
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);	
		delayms(1);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target+1]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target+1]);	
		delayms(1);
		Fvi45.CH[VBYP]      .mi(I1_Measure);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target+1]-0.1);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target+1]-0.1);	
		delayms(1);	
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target+1]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target+1]);	
		delayms(1);
		Fvi45.CH[VBYP]      .mi(I2_Measure);
		FOR_EACH_SITE(nSiteIndex)
		{
			if((fabs(1e3*I1_Measure[nSiteIndex]-120)<50)&&(fabs(1e3*I2_Measure[nSiteIndex]-240)<50))
			{
				VIN_3V3_UVLO_F[nSiteIndex]=(Master_Sub1_LowLi[TestNO_target+1]+Master_Sub1_HighLi[TestNO_target+1])/2;
				VIN_3V3_UVLO_HYS[nSiteIndex]=1e3*(VIN_3V3_UVLO_R[nSiteIndex]-VIN_3V3_UVLO_F[nSiteIndex]);
			}
			else
			{
                VIN_3V3_UVLO_F[nSiteIndex]=9999;
				VIN_3V3_UVLO_HYS[nSiteIndex]=9999;
			}
		}
	}

	if(GNG_Flag==1)
	{
		//Rise
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target]-0.1);	
		delayms(2);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target]);	
		delayms(2);
		Fvi45.CH[VBYP]      .mi(I1_Measure);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target]);	
		delayms(1);
		Fvi45.CH[VBYP]      .mi(I2_Measure);
		FOR_EACH_SITE(nSiteIndex)
		{
			if((fabs(1e3*I1_Measure[nSiteIndex]-240)<50)&&(fabs(1e3*I2_Measure[nSiteIndex]-120)<50))
				VIN_3V3_UVLO_R[nSiteIndex]=(Master_Sub1_LowLi[TestNO_target]+Master_Sub1_HighLi[TestNO_target])/2;
			else
                VIN_3V3_UVLO_R[nSiteIndex]=9999;
		}		
		//Fall
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target+1]+0.1);	
		delayms(1);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_HighLi[TestNO_target+1]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_HighLi[TestNO_target+1]);	
		delayms(1);
		Fvi45.CH[VBYP]      .mi(I1_Measure);
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target+1]+0.1);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target+1]+0.1);	
		delayms(1);	
		Fvi45.CH[VOUT_SW]	.fv(Master_Sub1_LowLi[TestNO_target+1]);
		Fvi45.CH[VIN_3V3]	.fv(Master_Sub1_LowLi[TestNO_target+1]);	
		delayms(1);
		Fvi45.CH[VBYP]      .mi(I2_Measure);
		FOR_EACH_SITE(nSiteIndex)
		{
			if((fabs(1e3*I1_Measure[nSiteIndex]-120)<50)&&(fabs(1e3*I2_Measure[nSiteIndex]-240)<50))
			{
				VIN_3V3_UVLO_F[nSiteIndex]=(Master_Sub1_LowLi[TestNO_target+1]+Master_Sub1_HighLi[TestNO_target+1])/2;
				VIN_3V3_UVLO_HYS[nSiteIndex]=1e3*(VIN_3V3_UVLO_R[nSiteIndex]-VIN_3V3_UVLO_F[nSiteIndex]);
			}
			else
			{
                VIN_3V3_UVLO_F[nSiteIndex]=9999;
				VIN_3V3_UVLO_HYS[nSiteIndex]=9999;
			}
		}		
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, VIN_3V3_UVLO_R[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VIN_3V3_UVLO_F[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VIN_3V3_UVLO_HYS[nSiteIndex]);
	}
    
	//************VIN_3V3_UVLO**********
	Relay.BIT[K48_I2C].clr();	
	Relay.BIT[K28_VIN_Cap]		.set();
	delayms(2);	
	Fvi45.CH[VBYP]      .fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[FB5]		.fv(0);
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();
	Fvi45.CH[FB5]		.open();
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Master_Timer
STT_TESTFUNC Master_Timer(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_Timer_params* ours)
{	
	double LPM_Exit_LPM_Delay_Target=0;
	double LPM_Exit_LPM_Delay_0[SITE_MAX]={0},LPM_Exit_LPM_Delay_CMI[SITE_MAX]={0}; 
	double LPM_Exit_LPM_Delay[SITE_MAX]={0},LPM_Exit_LPM_Delay_Acc[SITE_MAX]={0}; 


	double LDO_OnDelay_Target[LDO_MAX]={0},LDO_OffDelay_Target[LDO_MAX]={0};
	double LDO_OnDelay_0[LDO_MAX][SITE_MAX]={0}, LDO_OffDelay_0[LDO_MAX][SITE_MAX]={0};
	double LDO_OnDelay_CMI[LDO_MAX][SITE_MAX]={0}, LDO_OffDelay_CMI[LDO_MAX][SITE_MAX]={0};
	double LDO_OnDelay[LDO_MAX][SITE_MAX]={0}, LDO_OffDelay[LDO_MAX][SITE_MAX]={0};
	double LDO_OnDelay_Acc[LDO_MAX][SITE_MAX]={0},LDO_OffDelay_Acc[LDO_MAX][SITE_MAX]={0};
	double LDO_SST[8][LDO_MAX][SITE_MAX]={0};
   	
	double Buck_OnDelay_Target[PMU_MAX]={0},Buck_OffDelay_Target[PMU_MAX]={0};
	double Buck_OnDelay_0[PMU_MAX][SITE_MAX]={0}, Buck_OffDelay_0[PMU_MAX][SITE_MAX]={0};
	double Buck_OnDelay_CMI[PMU_MAX][SITE_MAX]={0}, Buck_OffDelay_CMI[PMU_MAX][SITE_MAX]={0};
	double Buck_OnDelay[PMU_MAX][SITE_MAX]={0}, Buck_OffDelay[PMU_MAX][SITE_MAX]={0};
	double Buck_OnDelay_Acc[PMU_MAX][SITE_MAX]={0},Buck_OffDelay_Acc[PMU_MAX][SITE_MAX]={0};
	

	double LPM_On_Delay_LDO_Target[LDO_MAX]={0},LPM_On_Delay_LDO[LDO_MAX][SITE_MAX]={0},LPM_On_Delay_LDO_Acc[LDO_MAX][SITE_MAX]={0};
	double LPM_On_Delay_LDO_0[LDO_MAX][SITE_MAX]={0},LPM_On_Delay_LDO_CMI[LDO_MAX][SITE_MAX]={0}; 
	double LPM_On_Delay_Buck_Target[PMU_MAX]={0},LPM_On_Delay_Buck[PMU_MAX][SITE_MAX]={0},LPM_On_Delay_Buck_Acc[PMU_MAX][SITE_MAX]={0};
	double LPM_On_Delay_Buck_0[PMU_MAX][SITE_MAX]={0},LPM_On_Delay_Buck_CMI[PMU_MAX][SITE_MAX]={0}; 
	//resource_init();
	//Connect_all_cap();//enable if run this function only
    Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(1);
    Fvi16.CH[VIN_LDO1]      .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[VIN_LDO2]      .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	delayms(3);
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K48_I2C].set();
	//delayms(5);	
	//i2c_init(HCTL_MODE,300000,2);//enable if run this function only
	delayms(3);//enable if run this function only
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x20fa].write(0x20);
	delayms(1);
	Fvi45.CH[FB5]           .fv(1.8, SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************LPM_ExitLPM_Delay**********
	LPM_Exit_LPM_Delay_Target=10*(Reg[0x1014].CMI&0x3f);
	if(LPM_Exit_LPM_Delay_Target==0) LPM_Exit_LPM_Delay_Target=400;
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
	Relay.BIT[K18_EXTEN_TMUA].set();
	Relay.BIT[K25_LPM_TMUB].set();
	delayms(1);
	TMU_CH1.stallset(Stall7V,TMU4mS,10);
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,DOWN);

	//Maks fault
	/*
	Reg[0x1040].set_clr_inv(BIT3);
	Reg[0x1050].set_clr_inv(BIT3);
	Reg[0x1060].set_clr_inv(BIT3);
	Reg[0x1070].set_clr_inv(BIT3);
	Reg[0x1080].set_clr_inv(BIT3);
	*/
	Reg[0x1040].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1050].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1060].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1070].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1080].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1045].set_clr_inv(BIT2);
	Reg[0x1055].set_clr_inv(BIT2);
	Reg[0x1065].set_clr_inv(BIT2);
	Reg[0x1075].set_clr_inv(BIT2);
	Reg[0x1085].set_clr_inv(BIT2);	
	Reg[0x1091].write(_0x00);
	//Maks fault
	Reg[0x2058].set_clr_inv(BITNONE,BIT7);
	Reg[0x100a].write(0xfa);
	Reg[0x100d].write(_0x00);
	//Add per new TM
	Reg[0x101a].write(0x80);
	Reg[0x1028].write(_0x00);//Set it to 0 for multiphase cmi
	Reg[0x1019].write(0x09);
	Reg[0x1021].write(0x09);
	Reg[0x1025].write(0x09);
	Reg[0x1029].write(0x09);
	Reg[0x102d].write(0x09);
	//Add per new TM
	Reg[0x101d].write(0xff);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(_0x00);
	Reg[0x1031].write(0x77);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(_0x00);
	Reg[0x1081].write(0x78);
	Reg[0x1082].write(0x78);
	Reg[0x1083].write(0xca);
	Reg[0x1084].write(_0x00);
	Reg[0x1085].write(_0x00);
	Reg[0x1086].write(_0x00);
	Reg[0x1093].write(0x99);
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);

	Reg[0x1010].write(_0x00);
	Reg[0x1011].write(0x80);
	Reg[0x1012].write(0xf1);
	Reg[0x1013].write(_0x00);
	Reg[0x1014].write(_0x00);
	Reg[0x1015].write(_0x00);
	Reg[0x2007].write(_0x00);
	Reg[0x1006].write(0x05);
	delayms(5);
	if(CMI_Rev_No==520100||CMI_Rev_No==522100) 	delayms(45);

	//LPM_ExitLPM_Delay0
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1006].write(0x04);
	delayms(1);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LPM_Exit_LPM_Delay_0[nSiteIndex]=meas_data[nSiteIndex+1];

	delayms(1);
	Reg[0x1006].write(0x05);
	//LPM_ExitLPM_Delay_CMI
	Reg[0x1014].write_CMI();
	if(Reg[0x1014].CMI==0) Reg[0x1014].write(0x3f);
	delayms(1);

	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1006].write(0x04);
	delayms(1);
	TMU_CH1.readdata();

	FOR_EACH_SITE(nSiteIndex)
	{	
		LPM_Exit_LPM_Delay_CMI[nSiteIndex]=meas_data[nSiteIndex+1];
		LPM_Exit_LPM_Delay[nSiteIndex]=LPM_Exit_LPM_Delay_CMI[nSiteIndex]-LPM_Exit_LPM_Delay_0[nSiteIndex];
		LPM_Exit_LPM_Delay_Acc[nSiteIndex]=100*(LPM_Exit_LPM_Delay[nSiteIndex]-LPM_Exit_LPM_Delay_Target)/LPM_Exit_LPM_Delay_Target;
	}
	delayms(1);
	Reg[0x1014].write(_0x00);

	Relay.BIT[K18_EXTEN_TMUA].clr();
	Relay.BIT[K25_LPM_TMUB].clr();
	TestNO_target=2;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_Exit_LPM_Delay_Target);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_Exit_LPM_Delay_0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_Exit_LPM_Delay_CMI[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LPM_Exit_LPM_Delay[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LPM_Exit_LPM_Delay_Acc[nSiteIndex]);
	}	
	//************LPM_ExitLPM_Delay**********
	
	//************LDO_Delay**********	

	FOR_EACH_TILE(nTILE,LDO_MAX) 
	{
		LDO_OnDelay_Target[nTILE]=((0xf0&Reg[0x109a+nTILE].CMI)>>4)*100;
		LDO_OffDelay_Target[nTILE]=(0x0f&Reg[0x109a+nTILE].CMI)*50;

		if(nTILE==0) LPM_On_Delay_LDO_Target[nTILE]=((0xf0&Reg[0x109c].CMI)>>4)*100;
		if(nTILE==1) LPM_On_Delay_LDO_Target[nTILE]=((0x0f&Reg[0x109c].CMI)>>0)*100;

		if(LDO_OnDelay_Target[nTILE]==0) LDO_OnDelay_Target[nTILE]=100;
		if(LDO_OffDelay_Target[nTILE]==0) LDO_OffDelay_Target[nTILE]=50;
		if(LPM_On_Delay_LDO_Target[nTILE]==0) LPM_On_Delay_LDO_Target[nTILE]=100;

	}
	
	Fvi45.CH[FB1].fv(Buck_Vset[0], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
	delayms(1);
	Relay.BIT[K19_EXTEN_TMUB].set();
	Relay.BIT[K21_GPIO1_TMUA].set();
	delayms(1);	
	TMU_CH1.stallset(Stall7V,TMU4mS,10);
	TMU_CH1.switchset(STARTtoA+STOPtoB);

	Reg[0x2008].write(0x04);
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x80);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);
	Reg[0x1095].write_CMI();
	Reg[0x2095].write(0x06);
	Reg[0x204d].write(0x8a);
	Reg[0x204f].write(0x40);
	Reg[0x1043].set_clr_inv(BIT6|BIT5|BIT4);
	Reg[0x1046].write(0xff);
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);	
	Reg[0x109d].set_clr_inv(BITNONE,BIT0);//New adding for BZ
	Reg[0x1044].set_clr_inv(BIT2|BIT1|BIT0);//New adding for BZ
	Reg[0x1012].write(0xf5);

	delayms(2);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_0[0][nSiteIndex]=meas_data[nSiteIndex+1];

	Reg[0x1012].write(0xf5);
	delayms(1);
	Reg[0x1012].write(0x71);
	delayms(1);
	//LDO_off_delay_0
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf5);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OffDelay_0[0][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x109a].read(readback);

	//LDO_on off_delay_cmi		
	Reg[0x109a].write_CMI();
	if((Reg[0x109a].CMI&0x0f)==0x00)
		Reg[0x109a].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
	if((Reg[0x109a].CMI&0xf0)==0x00)	
		Reg[0x109a].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
	if(Reg[0x109a].CMI==0x00)
		Reg[0x109a].set_clr_inv(BIT0|BIT4,BIT1|BIT2|BIT3|BIT5|BIT6|BIT7);	
	

	Reg[0x109a].read(readback);

	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];

	//LDO_off_delay_cmi	
	/*
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf5);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
	LDO_OffDelay_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];
	*/
	
	if((readback[0]&0x0f)<7)
	{
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf5);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];
	}
	if((readback[0]&0x0f)==7)
	{
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf5);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];
		if(LDO_OffDelay_CMI[0][0]>6000)
		{
			TMU_CH1.switchset(STARTtoB+STOPtoA);
			Reg[0x1012].write(0x71);
			delayms(1);
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf5);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
				LDO_OffDelay_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];
			if(LDO_OffDelay_CMI[0][0]>6000)
				LDO_OffDelay_CMI[0][0]=0;
		}
	}
	if((readback[0]&0x0f)>7)
	{	
		TMU_CH1.switchset(STARTtoB+STOPtoA);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf5);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][nSiteIndex]=-meas_data[nSiteIndex+1];
		}
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	FOR_EACH_SITE(nSiteIndex)
	{
		

		LDO_OnDelay[0][nSiteIndex]=LDO_OnDelay_0[0][nSiteIndex]-LDO_OnDelay_CMI[0][nSiteIndex];
		LDO_OnDelay_Acc[0][nSiteIndex]=100*(LDO_OnDelay[0][nSiteIndex]-LDO_OnDelay_Target[0])/LDO_OnDelay_Target[0];

		LDO_OffDelay[0][nSiteIndex]=LDO_OffDelay_0[0][nSiteIndex]-LDO_OffDelay_CMI[0][nSiteIndex];
		LDO_OffDelay_Acc[0][nSiteIndex]=100*(LDO_OffDelay[0][nSiteIndex]-LDO_OffDelay_Target[0])/LDO_OffDelay_Target[0];

	}
	TestNO_target=7;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LDO_OnDelay_0[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LDO_OnDelay_CMI[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LDO_OnDelay_Target[0]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LDO_OnDelay[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LDO_OnDelay_Acc[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, LDO_OffDelay_0[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, LDO_OffDelay_CMI[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, LDO_OffDelay_Target[0]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, LDO_OffDelay[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, LDO_OffDelay_Acc[0][nSiteIndex]);
	}
	//New adding for bz	
	//LDO1
	
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);

	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);
	Reg[0x109c].write(_0x00);
	Reg[0x109d].set_clr_inv(BIT0);
	Reg[0x1012].write(0xf5);	
	delayms(5);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LPM_On_Delay_LDO_0[0][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x1012].write(0xf5);	
	delayms(5);
	
	Reg[0x109c].write_CMI();
	if((Reg[0x109c].CMI&0xf0)==0x00)
		Reg[0x109c].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
	
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_On_Delay_LDO_CMI[0][nSiteIndex]=meas_data[nSiteIndex+1];
		LPM_On_Delay_LDO[0][nSiteIndex]=LPM_On_Delay_LDO_0[0][nSiteIndex]-LPM_On_Delay_LDO_CMI[0][nSiteIndex];
		LPM_On_Delay_LDO_Acc[0][nSiteIndex]=100*(LPM_On_Delay_LDO[0][nSiteIndex]-LPM_On_Delay_LDO_Target[0])/LPM_On_Delay_LDO_Target[0];
		
	}
	Reg[0x1012].write(0xf5);	
	Reg[0x109d].set_clr_inv(BITNONE,BIT0);
	TestNO_target=17;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_On_Delay_LDO_0[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_On_Delay_LDO_CMI[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_On_Delay_LDO_Target[0]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LPM_On_Delay_LDO[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LPM_On_Delay_LDO_Acc[0][nSiteIndex]);
	}
	//New adding for bz
	
	//LDO_SST		
	Reg[0x109a].write(_0x00);
	Reg[0x2095].write(0x05);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	for(i=0;i<8;i++)
	{
		Reg[0x1012].write(0xf5);	
		Reg[0x1095].write((0x03&Reg[0x1095].CMI)|(i<<5));    
		delayms(1);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delay(300);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_SST[i][0][nSiteIndex]=0.9*(LDO_OnDelay_0[0][nSiteIndex]-meas_data[nSiteIndex+1]);
			pFunction->LogResult(NSITEINDEX, 22+i, LDO_SST[i][0][nSiteIndex]);
		}
	}

	//Relay.BIT[K19_EXTEN_TMUB].clr();
	Relay.BIT[K21_GPIO1_TMUA].clr();

	//Relay.BIT[K19_EXTEN_TMUB].set();

	//LDO_on_delay_0
	Relay.BIT[K24_LPM_TMUA].set();
	Reg[0x2095].write(0x0f);
	Reg[0x1012].write(0xf3);
	delayms(2);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_0[1][nSiteIndex]=meas_data[nSiteIndex+1];

	//LDO_off_delay_0
	Reg[0x1012].write(0xf3);
	delayms(1);
	Reg[0x1012].write(0x71);
	delayms(1);
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf3);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OffDelay_0[1][nSiteIndex]=meas_data[nSiteIndex+1];


	
	//LDO_on_delay_cmi			
	Reg[0x109b].write_CMI();
	if((Reg[0x109b].CMI&0x0f)==0x00)
		Reg[0x109b].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
	if((Reg[0x109b].CMI&0xf0)==0x00)	
		Reg[0x109b].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
	if(Reg[0x109b].CMI==0x00)
		Reg[0x109b].set_clr_inv(BIT0|BIT4,BIT1|BIT2|BIT3|BIT5|BIT6|BIT7);	
	
	Reg[0x109b].read(readback);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_CMI[1][nSiteIndex]=meas_data[nSiteIndex+1];
	
	/*
	//LDO_off_delay_cmi		
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf3);
	delay(300);
	TMU_CH1.readdata();
	*/

	if((readback[0]&0x0f)<7)
	{
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf3);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][nSiteIndex]=meas_data[nSiteIndex+1];
	}
	if((readback[0]&0x0f)==7)
	{
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf3);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][nSiteIndex]=meas_data[nSiteIndex+1];
		if(LDO_OffDelay_CMI[1][0]>6000)
		{
			TMU_CH1.switchset(STARTtoB+STOPtoA);
			Reg[0x1012].write(0x71);
			delayms(1);
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf3);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
				LDO_OffDelay_CMI[1][nSiteIndex]=meas_data[nSiteIndex+1];
			if(LDO_OffDelay_CMI[1][0]>6000)
				LDO_OffDelay_CMI[1][0]=0;
		}
	}
	if((readback[0]&0x0f)>7)
	{	
		TMU_CH1.switchset(STARTtoB+STOPtoA);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0xf3);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][nSiteIndex]=-meas_data[nSiteIndex+1];
		}
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	FOR_EACH_SITE(nSiteIndex)
	{
		

		LDO_OnDelay[1][nSiteIndex]=LDO_OnDelay_0[1][nSiteIndex]-LDO_OnDelay_CMI[1][nSiteIndex];
		LDO_OnDelay_Acc[1][nSiteIndex]=100*(LDO_OnDelay[1][nSiteIndex]-LDO_OnDelay_Target[1])/LDO_OnDelay_Target[1];

		LDO_OffDelay[1][nSiteIndex]=LDO_OffDelay_0[1][nSiteIndex]-LDO_OffDelay_CMI[1][nSiteIndex];
		LDO_OffDelay_Acc[1][nSiteIndex]=100*(LDO_OffDelay[1][nSiteIndex]-LDO_OffDelay_Target[1])/LDO_OffDelay_Target[1];

	}
	TestNO_target=30;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LDO_OnDelay_0[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LDO_OnDelay_CMI[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LDO_OnDelay_Target[1]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LDO_OnDelay[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LDO_OnDelay_Acc[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, LDO_OffDelay_0[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, LDO_OffDelay_CMI[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, LDO_OffDelay_Target[1]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+8, LDO_OffDelay[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+9, LDO_OffDelay_Acc[1][nSiteIndex]);
	}
	////
	//LDO2
	
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);
	Reg[0x109c].write(_0x00);
	Reg[0x109d].set_clr_inv(BIT0);
	delayms(2);
	Reg[0x1012].write(0xf3);	
	delayms(5);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);	
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LPM_On_Delay_LDO_0[1][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x1012].write(0xf3);	
	delayms(5);
	Reg[0x109c].write_CMI();
	if((Reg[0x109c].CMI&0x0f)==0x00)
		Reg[0x109c].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
	
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);	
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_On_Delay_LDO_CMI[1][nSiteIndex]=meas_data[nSiteIndex+1];
		LPM_On_Delay_LDO[1][nSiteIndex]=LPM_On_Delay_LDO_0[1][nSiteIndex]-LPM_On_Delay_LDO_CMI[1][nSiteIndex];
		LPM_On_Delay_LDO_Acc[1][nSiteIndex]=100*(LPM_On_Delay_LDO[1][nSiteIndex]-LPM_On_Delay_LDO_Target[1])/LPM_On_Delay_LDO_Target[1];
		
	}
	Reg[0x109d].set_clr_inv(BITNONE,BIT0);
	TestNO_target=40;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_On_Delay_LDO_0[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_On_Delay_LDO_CMI[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_On_Delay_LDO_Target[1]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LPM_On_Delay_LDO[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LPM_On_Delay_LDO_Acc[1][nSiteIndex]);
	}
	//New adding for bz
	////
	//LDO_SST		
	Reg[0x109b].write(_0x00);
	Reg[0x2095].write(0x0e);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	for(i=0;i<8;i++)
	{
		Reg[0x1012].write(0xf3);	
		Reg[0x1095].write((0x03&Reg[0x1095].CMI)|(i<<2));    
		delayms(1);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delay(300);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_SST[i][1][nSiteIndex]=0.9*(LDO_OnDelay_0[1][nSiteIndex]-meas_data[nSiteIndex+1]);
			pFunction->LogResult(NSITEINDEX, 45+i, LDO_SST[i][1][nSiteIndex]);
		}
	}
	Relay.BIT[K19_EXTEN_TMUB].clr();
	Relay.BIT[K24_LPM_TMUA].clr();
	//************LDO_SST**********
	//************Buck_Delay**********
	
	int Trigger_Set0[PMU_MAX]={0xf3,0xf3,0xf3,0xf3,0xfb},Trigger_Set1[PMU_MAX]={0x71,0xb1,0xd1,0xe1,0xf1};
	Relay.BIT[K18_EXTEN_TMUA].set();
	Relay.BIT[K25_LPM_TMUB].set();
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if (((0xf0&Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI)>>4)==0)
			Buck_OnDelay_Target[nTILE]=100;
		else
			Buck_OnDelay_Target[nTILE]=((0xf0&Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI)>>4)*100;

		if ((0x07&Reg[GP_User_Buck_Addr[nTILE]+0x04].CMI)==0)
			Buck_OffDelay_Target[nTILE]=50;
		else
			Buck_OffDelay_Target[nTILE]=(0x07&Reg[GP_User_Buck_Addr[nTILE]+0x04].CMI)*50;

		if (((0x0f&Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI))==0)
			LPM_On_Delay_Buck_Target[nTILE]=100;
		else
			LPM_On_Delay_Buck_Target[nTILE]=((0x0f&Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI))*100;

	}
	Fvi45.CH[FB1].fv(Buck_Vset[0], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
	delayms(1);
	Reg[0x109b].write(0xff);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(nTILE==0||nTILE==1)
		{
			Reg[0x2058].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];//B1&2
			Reg[0x2058].write(readback);
			
		}
		if(nTILE==2||nTILE==4)
		{
			Reg[0x2067].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];//B3&5
			Reg[0x2067].write(readback);
		}
		if(nTILE==3)
		{
			Reg[0x2077].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];//B4
			Reg[0x2077].write(readback);
		}
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x09);
		if(nTILE!=0)
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x40);
		Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BITNONE,BIT7|BIT6|BIT5|BIT4);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT2|BIT1|BIT0);


		delayms(1);
		//Buck_on_delay_0
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			
			Buck_OnDelay_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
	
		//Buck_off_delay_0
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)		
			Buck_OffDelay_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];

		Reg[GP_User_Buck_Addr[nTILE]+0x06].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x04].write_CMI();
		if((Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI&0xf0)==0x00)	
			Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
		if((Reg[GP_User_Buck_Addr[nTILE]+0x04].CMI&0x07)==0x00)
			Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT0,BIT1|BIT2);
		//Buck_on_delay_cmi
		delayms(1);
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)	
			Buck_OnDelay_CMI[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
		
		//Buck_off_delay_cmi
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OffDelay_CMI[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OnDelay[nTILE][nSiteIndex]=Buck_OnDelay_0[nTILE][nSiteIndex]-Buck_OnDelay_CMI[nTILE][nSiteIndex];
			Buck_OnDelay_Acc[nTILE][nSiteIndex]=100*(Buck_OnDelay[nTILE][nSiteIndex]-Buck_OnDelay_Target[nTILE])/Buck_OnDelay_Target[nTILE];
			
			Buck_OffDelay[nTILE][nSiteIndex]=Buck_OffDelay_0[nTILE][nSiteIndex]-Buck_OffDelay_CMI[nTILE][nSiteIndex];
			Buck_OffDelay_Acc[nTILE][nSiteIndex]=100*(Buck_OffDelay[nTILE][nSiteIndex]-Buck_OffDelay_Target[nTILE])/Buck_OffDelay_Target[nTILE];

			pFunction->LogResult(NSITEINDEX, 15*nTILE+53, Buck_OnDelay_0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+54, Buck_OnDelay_CMI[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+55, Buck_OnDelay_Target[nTILE]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+56, Buck_OnDelay[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+57, Buck_OnDelay_Acc[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+58, Buck_OffDelay_0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+59, Buck_OffDelay_CMI[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+60, Buck_OffDelay_Target[nTILE]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+61, Buck_OffDelay[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+62, Buck_OffDelay_Acc[nTILE][nSiteIndex]);

		}
		//New adding for BZ
		
		if(nTILE==0||nTILE==1)
		{
			Reg[0x2058].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|readback[nSiteIndex];//B1&2
			Reg[0x2058].write(readback);
			
		}
		if(nTILE==2||nTILE==4)
		{
			Reg[0x2067].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x20|readback[nSiteIndex];//B3&5
			Reg[0x2067].write(readback);		
		}
		if(nTILE==3)
		{
			Reg[0x2077].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x20|readback[nSiteIndex];//B4
			Reg[0x2077].write(readback);
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x06].write(_0x00);	
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(5);
		//Buck_on_delay_0
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			
			LPM_On_Delay_Buck_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
		
		Reg[0x1012].write(Trigger_Set0[nTILE]);	
		Reg[GP_User_Buck_Addr[nTILE]+0x06].write_CMI();
		if((Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI&0x0f)==0x00)	
			Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BIT0,BIT3|BIT2|BIT1);
		
		//Buck_on_delay_cmi		
		delayms(1);
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LPM_On_Delay_Buck_CMI[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
			LPM_On_Delay_Buck[nTILE][nSiteIndex]=LPM_On_Delay_Buck_0[nTILE][nSiteIndex]-LPM_On_Delay_Buck_CMI[nTILE][nSiteIndex];
			LPM_On_Delay_Buck_Acc[nTILE][nSiteIndex]=100*(LPM_On_Delay_Buck[nTILE][nSiteIndex]-LPM_On_Delay_Buck_Target[nTILE])/LPM_On_Delay_Buck_Target[nTILE];		
	
			pFunction->LogResult(NSITEINDEX, 15*nTILE+63, LPM_On_Delay_Buck_0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+64, LPM_On_Delay_Buck_CMI[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+65, LPM_On_Delay_Buck_Target[nTILE]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+66, LPM_On_Delay_Buck[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 15*nTILE+67, LPM_On_Delay_Buck_Acc[nTILE][nSiteIndex]);	
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x06].write_CMI();
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0); 
		Fvi45.CH[GP_FB_Buck[nTILE]].open(); 
		//New adding for BZ	
		if(nTILE!=4)
		{
			Fvi45.CH[GP_FB_Buck[nTILE+1]].fv(1.00*Buck_Vset[nTILE+1], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
			delayms(1);
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		if(nTILE!=0)
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
	}	
  	//************Power_off**********
	Relay.BIT[K47_TMU].clr();
	Relay.BIT[K18_EXTEN_TMUA].clr();
	Relay.BIT[K25_LPM_TMUB].clr();
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VBYP]      .fv(0);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[FB5]		.fv(0);

	Fvi16.CH[VIN_LDO1]  .open();
	Fvi16.CH[VIN_LDO2]  .open();
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();
	Fvi45.CH[FB5]		.open();
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Master_Option_Check
STT_TESTFUNC Master_Option_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_Option_Check_params* ours)
{
	int HBIT_Set=0,Value_Set=0;	
	double Master_Trim_HighLi[201]={0},Master_Trim_LowLi[201]={0};
	double Trigger_V1[SITE_MAX]={0},Trigger_V2[SITE_MAX]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Master_Trim_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Master_Trim_LowLi[i+1]);//item 1-48
	}

	Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	//debug	
	//i2c_init(HCTL_MODE,300000,2);
	//delayms(2);
	//debug	
	Reg[0x10f2].set_clr_inv(BIT3,BIT2|BIT1|BIT0);// Disable DMUX
	Reg[0x2007].write(0xa0);
	Reg[0x20fa].write(0x20);
	Disable_Sequence();

	delayms(2);
	Reg[0x1019].write(0x08);
	Reg[0x101a].write(0x80);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(_0x00);//Config EXT_PG pin
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************VOUTSW_UVLO_F**********
	TestNO_target=1;

	double VOUTSW_UVLO_F_Target=2.7-0.1*((Reg[0x10f5].CMI&0x18)>>3);//V
	double VOUTSW_UVLO_F_Acc[SITE_MAX]={0},VOUTSW_UVLO_R[SITE_MAX]={0},VOUTSW_UVLO_HYS[SITE_MAX]={0};
	double VOUTSW_UVLO_Fall_Meas[SITE_MAX]={0},VOUTSW_UVLO_Rise_Meas[SITE_MAX]={0},VOUTSW_UVLO_HYS_Meas[SITE_MAX]={0};
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x2007].write(0xad);
	Reg[0x2008].write(0x05);
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);//Config EXT_PG pin
	
	Fvi45.CH[VOUT_SW].fv(1.1*VOUTSW_UVLO_F_Target);
	delayms(2);
	// Measure other settings
	for(i=0;i<4;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xe7&Reg[0x10f5].CMI)|(i<<3);
		Reg[0x10f5].write(readback);
		VOUTSW_UVLO_F_Target=2.7-0.1*((readback[0]&0x18)>>3);
		delayms(1);
		if(GNG_Flag==0)
		{
			//Falling
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.05);
			delay(20);
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				Master_Trim_HighLi[3*i+2],Master_Trim_LowLi[3*i+2],-0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
				VOUTSW_UVLO_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];

			//Rising
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.05);
			delay(20);		
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.04);
			delay(20);			
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.03);
			delay(20);			
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.02);
			delayms(1);
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
			Master_Trim_LowLi[3*i+3],Master_Trim_HighLi[3*i+3],0.002,LESS_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_UVLO_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_UVLO_HYS_Meas[nSiteIndex]=1e3*(VOUTSW_UVLO_Rise_Meas[nSiteIndex]-VOUTSW_UVLO_Fall_Meas[nSiteIndex]);//mV
			}
		}
		if(GNG_Flag==1)
		{
			//Fall
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]+0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+2]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+2]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VOUTSW_UVLO_Fall_Meas[nSiteIndex]=(Master_Trim_HighLi[3*i+2]+Master_Trim_LowLi[3*i+2])/2;		   
				}
				else
				{
					VOUTSW_UVLO_Fall_Meas[nSiteIndex]=99999;
				}
			}
			//Rise
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.05);
			delay(20);		
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.04);
			delay(20);			
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.03);
			delay(20);			
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.02);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]-0.01);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[3*i+3]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[3*i+3]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VOUTSW_UVLO_Rise_Meas[nSiteIndex]=(Master_Trim_HighLi[3*i+3]+Master_Trim_LowLi[3*i+3])/2;	
					VOUTSW_UVLO_HYS_Meas[nSiteIndex]=(Master_Trim_HighLi[3*i+4]+Master_Trim_LowLi[3*i+4])/2;
				}
				else
				{
					VOUTSW_UVLO_Rise_Meas[nSiteIndex]=99999;
					VOUTSW_UVLO_HYS_Meas[nSiteIndex]=0;
				}
			}			
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+3*i+1, VOUTSW_UVLO_Fall_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+3*i+2, VOUTSW_UVLO_Rise_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+3*i+3, VOUTSW_UVLO_HYS_Meas[nSiteIndex]);
		}
	}
	Reg[0x10f5].write_CMI();

	Fvi45.CH[VOUT_SW].fv(VIN_SET);
    delayms(2);	
	Reg[0x2007].write(0xa0);

	//************VOUTSW_UVLO_F**********

	//************VOUTSW_BR_F**********
	TestNO_target=14;

	double VOUTSW_BR_F_Target=0;
	double VOUTSW_BR_HYS_Set=0,VOUTSW_BR_HYS_Acc[SITE_MAX]={0},VOUTSW_BR_HYS_Target=0;
	double VOUTSW_BR_Fall_Meas[SITE_MAX]={0},VOUTSW_BR_F_Acc[SITE_MAX]={0},VOUTSW_BR_Rise_Meas[SITE_MAX]={0},VOUTSW_BR_HYS_Meas[SITE_MAX]={0};
	VOUTSW_BR_HYS_Set=100+50*((Reg[0x10f6].CMI>>2)&0x03);

	//Falling option check, Hys follow the cmi 
	for(i=0;i<8;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x1f&Reg[0x10f5].CMI)|(i<<5);
		Reg[0x10f5].write(readback);
		HBIT_Set=(readback[0]>>7)&0x01;
		Value_Set=(readback[0]>>5)&0x03;
		
		VOUTSW_BR_F_Target=3.1-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
		delayms(1);
		Reg[0x2007].write(0xaf);
		if(GNG_Flag==0)
		{
			//falling
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.05);
			delayms(1);	
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				Master_Trim_HighLi[4*i+14],Master_Trim_LowLi[4*i+14],-0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
				VOUTSW_BR_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
			
			//Rising
			Reg[0x2007].write(0x8f);//change on 20201216 from 0xaf to 0x8f
			delayms(1);
			
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.1);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.08);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.06);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.04);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.02);
			delay(200);
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17]),Master_Trim_HighLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_HighLi[4*i+17]),0.002,LESS_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_BR_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_BR_HYS_Meas[nSiteIndex]=1e3*(VOUTSW_BR_Rise_Meas[nSiteIndex]-VOUTSW_BR_Fall_Meas[nSiteIndex]);//mV
				VOUTSW_BR_HYS_Acc[nSiteIndex]=VOUTSW_BR_HYS_Meas[nSiteIndex]-VOUTSW_BR_HYS_Set;//mV			
			}
		}
		if(GNG_Flag==1)
		{
			//Fall
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VOUTSW_BR_Fall_Meas[nSiteIndex]=(Master_Trim_HighLi[4*i+14]+Master_Trim_LowLi[4*i+14])/2;		   
				}
				else
				{
					VOUTSW_BR_Fall_Meas[nSiteIndex]=99999;
				}
			}
			//Rise	
			Reg[0x2007].write(0x8f);//change on 20201216 from 0xaf to 0x8f
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.1);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.08);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.06);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.04);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17])-0.02);
			delay(200);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_LowLi[4*i+17]));
			delay(200);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[4*i+14]+1e-3*(VOUTSW_BR_HYS_Set+Master_Trim_HighLi[4*i+17]));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VOUTSW_BR_Rise_Meas[nSiteIndex]=VOUTSW_BR_F_Target+1e-3*VOUTSW_BR_HYS_Set;	
					VOUTSW_BR_HYS_Meas[nSiteIndex]=VOUTSW_BR_HYS_Set;
					VOUTSW_BR_HYS_Acc[nSiteIndex]=0;
				}
				else
				{
					VOUTSW_BR_Rise_Meas[nSiteIndex]=99999;
					VOUTSW_BR_HYS_Meas[nSiteIndex]=99999;
					VOUTSW_BR_HYS_Acc[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+4*i+0, VOUTSW_BR_Fall_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4*i+1, VOUTSW_BR_Rise_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4*i+2, VOUTSW_BR_HYS_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+4*i+3, VOUTSW_BR_HYS_Acc[nSiteIndex]);
		}
	}
	//Falling follow the cmi, Hys option check 
	TestNO_target=46;
	Reg[0x10f5].write_CMI();
	HBIT_Set=(Reg[0x10f5].CMI>>7)&0x01;
	Value_Set=(Reg[0x10f5].CMI>>5)&0x03;
	VOUTSW_BR_F_Target=3.1-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, VOUTSW_BR_F_Target);
	}
	for(i=0;i<4;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xf3&Reg[0x10f6].CMI)|(i<<2);
		Reg[0x10f6].write(readback);
		VOUTSW_BR_HYS_Set=100+i*50;
		delayms(1);
		Reg[0x2007].write(0xaf);
		if(GNG_Flag==0)
		{
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.05);
			delay(20);

			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target,1e-3*Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_BR_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_BR_F_Acc[nSiteIndex]=1e3*(VOUTSW_BR_Fall_Meas[nSiteIndex]-VOUTSW_BR_F_Target);
			}
			Reg[0x2007].write(0x8f);//change on 20201216 from 0xaf to 0x8f
			delayms(1);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.08);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.06);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.04);
			delay(20);

			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set),VOUTSW_BR_F_Target+1e-3*(Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set),0.002,LESS_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_BR_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_BR_HYS_Meas[nSiteIndex]=1e3*(VOUTSW_BR_Rise_Meas[nSiteIndex]-VOUTSW_BR_Fall_Meas[nSiteIndex]);//mV
			}
		}
		if(GNG_Flag==1)
		{
			//Fall
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target+0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_F_Target);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VOUTSW_BR_F_Acc[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target+1]+Master_Trim_LowLi[2*i+TestNO_target+1])/2;		   
				}
				else
				{
					VOUTSW_BR_F_Acc[nSiteIndex]=99999;
				}
			}
			//Rise	
			Reg[0x2007].write(0x8f);//change on 20201216 from 0xaf to 0x8f
			delay(200);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.08);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.06);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set)-0.04);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_BR_F_Target+1e-3*(Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_BR_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VOUTSW_BR_HYS_Meas[nSiteIndex]=VOUTSW_BR_HYS_Set;
				}
				else
				{
					VOUTSW_BR_HYS_Meas[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+1, VOUTSW_BR_F_Acc[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+2, VOUTSW_BR_HYS_Meas[nSiteIndex]);
		}
	}
	Fvi45.CH[VOUT_SW].fv(VIN_SET);
	Reg[0x2007].write(0xa0);
	//************VOUTSW_BR_F**********
	//************VOUTSW_UV_Warn_F**********
	TestNO_target=55;

	double VOUTSW_POK_Warn_HYS_Set=0,VOUTSW_POK_Warn_F_Target=0,VOUTSW_POK_Warn_HYS_Target=0;
	double VOUTSW_POK_Warn_Fall_Meas[SITE_MAX]={0},VOUTSW_POK_Warn_F_Acc[SITE_MAX]={0},VOUTSW_POK_Warn_Rise_Meas[SITE_MAX]={0},VOUTSW_POK_Warn_HYS_Meas[SITE_MAX]={0},VOUTSW_POK_Warn_HYS_Acc[SITE_MAX]={0};
	VOUTSW_POK_Warn_HYS_Set=100+50*(Reg[0x10f6].CMI&0x03);
	Reg[0x2007].write(0xae);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, VOUTSW_POK_Warn_HYS_Set);
	}	
	//Falling option check, Hys follow the cmi 
	for(i=0;i<8;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xe8&Reg[0x10f5].CMI)|(i<<0);
		Reg[0x10f5].write(readback);
		HBIT_Set=(readback[0]>>2)&0x01;
		Value_Set=readback[0]&0x03;
		VOUTSW_POK_Warn_F_Target=3.15-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
		delayms(1);
		if(GNG_Flag==0)
		{
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.05);
			delay(20);

			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				Master_Trim_HighLi[TestNO_target+2*i+1],Master_Trim_LowLi[TestNO_target+2*i+1],-0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
				VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
		
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.02);
			delay(20);
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set),
				VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_HighLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set),0.002,LESS_THAN,1.5,global_temp,20);		
			
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_POK_Warn_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]=1e3*(VOUTSW_POK_Warn_Rise_Meas[nSiteIndex]-VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]);//mV
				VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]=VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]-VOUTSW_POK_Warn_HYS_Set;
			}		
		}
		if(GNG_Flag==1)
		{
			//Fall
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]+0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_HighLi[TestNO_target+2*i+1]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(Master_Trim_LowLi[TestNO_target+2*i+1]);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target+1]+Master_Trim_LowLi[2*i+TestNO_target+1])/2;		   
				}
				else
				{
					VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]=99999;
				}
			}
			//Rise	
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set)-0.02);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_HighLi[TestNO_target+2*i+2]+VOUTSW_POK_Warn_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target+2]+Master_Trim_LowLi[2*i+TestNO_target+2])/2;;
				}
				else
				{
					VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+1, VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+2, VOUTSW_POK_Warn_HYS_Acc[nSiteIndex]);
		}
	}

	//Falling follow the cmi, Hys option check 

	TestNO_target=72;	
	Reg[0x10f5].write_CMI();
	Reg[0x2007].write(0xae);
	HBIT_Set=(Reg[0x10f5].CMI>>2)&0x01;
	Value_Set=Reg[0x10f5].CMI&0x03;
	VOUTSW_POK_Warn_F_Target=3.15-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, VOUTSW_POK_Warn_F_Target);
	}	

	for(i=0;i<4;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xfc&Reg[0x10f6].CMI)|(i<<0);
		Reg[0x10f6].write(readback);
		VOUTSW_POK_Warn_HYS_Set=100+i*50;

		if(GNG_Flag==0)
		{
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.05);
			delay(20);
			
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target,
				1e-3*Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target,-0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_POK_Warn_F_Acc[nSiteIndex]=1e3*(VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]-VOUTSW_POK_Warn_F_Target);
			}
			delayms(1);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.08);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.06);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.04);
			delay(20);
			
			sweep_VI(RESOURCE_FVI45,VOUT_SW,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set),
				VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set),0.002,LESS_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
			{
				VOUTSW_POK_Warn_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]=1e3*(VOUTSW_POK_Warn_Rise_Meas[nSiteIndex]-VOUTSW_POK_Warn_Fall_Meas[nSiteIndex]);//mV
			}
		}
		if(GNG_Flag==1)
		{
			//Fall
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.2);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target+0.05);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(1e-3*Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_F_Target);
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VOUTSW_POK_Warn_F_Acc[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target+1]+Master_Trim_LowLi[2*i+TestNO_target+1])/2;		   
				}
				else
				{
					VOUTSW_POK_Warn_F_Acc[nSiteIndex]=99999;
				}
			}
			//Rise	
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.08);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.06);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set)-0.04);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_LowLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VOUT_SW].fv(VOUTSW_POK_Warn_F_Target+1e-3*(Master_Trim_HighLi[2*i+TestNO_target+1]+VOUTSW_POK_Warn_HYS_Set));
			delay(20);
			Fvi16.CH[EXT_EN].mv(Trigger_V2);
			
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]=VOUTSW_POK_Warn_HYS_Set;
				}
				else
				{
					VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+1, VOUTSW_POK_Warn_F_Acc[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+2, VOUTSW_POK_Warn_HYS_Meas[nSiteIndex]);
		}
	}	
	Fvi16.CH[EXT_EN].open();
	Fvi45.CH[VOUT_SW].fv(VIN_SET);
    delayms(2);	
	Reg[0x2007].write(0xa0);	
	Reg[0x2008].write_CMI();
	Reg[0x101d].write_CMI();
	Reg[0x101e].write_CMI();
	Reg[0x101f].write_CMI();
	Reg[0x1020].write_CMI();//Disable Configure EXT_PG pin
	//************VOUTSW_UV_Warn_F**********
	//************SW_ON_R**********
	TestNO_target=81;

	double SW_ON_Rise_Target=2.95-0.1*((Reg[0x10f2].CMI&0xc0)>>6);//V
	double SW_ON_Rise_Meas[SITE_MAX]={0},SW_ON_Fall_Meas[SITE_MAX]={0},SW_ON_HYS_Meas[SITE_MAX]={0};
	Fvi16.CH[GPIO1].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x20f9].write(0x02);
	Reg[0x20fa].write(0xa0);
	Reg[0x2008].write(0x04);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	Reg[0x10f7].write(0x04);// enable DMUX
	// Measure other settings
	for(i=0;i<4;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&Reg[0x10f2].CMI)|(i<<6);
		Reg[0x10f2].write(readback);
		SW_ON_Rise_Target=2.95-0.1*((readback[0]&0xc0)>>6);
		delayms(1);

		if(GNG_Flag==0)
		{
			//Rising
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.08);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.06);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.04);
			delay(20);
			sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Master_Trim_LowLi[TestNO_target+2*i],Master_Trim_HighLi[TestNO_target+2*i],0.002,LESS_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
				SW_ON_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];
			//Falling
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.2);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.05);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.02);
			delay(20);
			sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1],
				Master_Trim_LowLi[TestNO_target+2*i]-1e-3*Master_Trim_HighLi[TestNO_target+2*i+1],-0.002,GREATER_THAN,1.5,global_temp,20);	
			FOR_EACH_SITE(nSiteIndex)
			{
				SW_ON_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
				SW_ON_HYS_Meas[nSiteIndex]=1e3*(SW_ON_Rise_Meas[nSiteIndex]-SW_ON_Fall_Meas[nSiteIndex]);//mV
			}
		}
		if(GNG_Flag==1)
		{
			//Rise
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.2);
			delay(200);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.1);
			delay(200);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.05);
			delay(200);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.02);
			delay(200);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]);
			delayms(1);
			Fvi16.CH[GPIO1].mv(Trigger_V1);
			delay(200);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]);
			delay(200);
			Fvi16.CH[GPIO1].mv(Trigger_V2);		
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					SW_ON_Rise_Meas[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target]+Master_Trim_LowLi[2*i+TestNO_target])/2;		   
				}
				else
				{
					SW_ON_Rise_Meas[nSiteIndex]=99999;
				}
			}
			//Falling
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.2);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.05);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.02);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]);
			delay(20);
			Fvi16.CH[GPIO1].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-1e-3*Master_Trim_HighLi[TestNO_target+2*i+1]);
			delay(20);
			Fvi16.CH[GPIO1].mv(Trigger_V2);

			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					SW_ON_HYS_Meas[nSiteIndex]=(Master_Trim_HighLi[2*i+1+TestNO_target]+Master_Trim_LowLi[2*i+1+TestNO_target])/2;		   
				}
				else
				{
					SW_ON_HYS_Meas[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+0, SW_ON_Rise_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+1, SW_ON_HYS_Meas[nSiteIndex]);
		}
	}
	Reg[0x10f2].write_CMI();
	Reg[0x10f7].write_CMI();
	Reg[0x1008].write_CMI();// Disable DMUX
	Fvi45.CH[VIN_3V3].fv(VIN_SET);
	//************SW_ON_R**********	
	//************VIN3P3_OV_Warn_R**********
	TestNO_target=89;

    Value_Set=(Reg[0x10f2].CMI&0x30)>>4;
	double VIN3P3_OV_Warn_R_Target=0;	
	double VIN3P3_OV_Warn_R_Acc[SITE_MAX]={0},VIN3P3_OV_Warn_F[SITE_MAX]={0},VIN3P3_OV_Warn_HYS[SITE_MAX]={0};
	double VIN3P3_OV_Warn_Rise_Meas[SITE_MAX]={0},VIN3P3_OV_Warn_Fall_Meas[SITE_MAX]={0},VIN3P3_OV_Warn_HYS_Meas[SITE_MAX]={0};

	Reg[0x1008].write(0x05);// Disable DMUX
	// Measure other settings
	for(i=0;i<3;i++)
	{
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xcf&Reg[0x10f2].CMI)|(i<<4);
		Reg[0x10f2].write(readback);
		Value_Set=(readback[0]&0x30)>>4;
		if(i==0)
			VIN3P3_OV_Warn_R_Target=3.63;
		else if(i==1)
			VIN3P3_OV_Warn_R_Target=3.70;
		else if(i==2)
			VIN3P3_OV_Warn_R_Target=3.76;

		if(GNG_Flag==0)
		{
			//Rising
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.08);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.06);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.04);
			delay(20);

			sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Master_Trim_LowLi[TestNO_target+2*i],Master_Trim_HighLi[TestNO_target+2*i],0.002,GREATER_THAN,1.5,global_temp,20);
			FOR_EACH_SITE(nSiteIndex)
				VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]=global_temp[nSiteIndex];

			//Falling
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.2);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.05);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.02);
			delay(20);
			sweep_VI(RESOURCE_FVI45,VIN_3V3,VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
				Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1],
				Master_Trim_LowLi[TestNO_target+2*i]-1e-3*Master_Trim_HighLi[TestNO_target+2*i+1],-0.002,LESS_THAN,1.5,global_temp,20);	
			FOR_EACH_SITE(nSiteIndex)
			{
				VIN3P3_OV_Warn_Fall_Meas[nSiteIndex]=global_temp[nSiteIndex];
				VIN3P3_OV_Warn_HYS_Meas[nSiteIndex]=1e3*(VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]-VIN3P3_OV_Warn_Fall_Meas[nSiteIndex]);//mV
			}
		}
		if(GNG_Flag==1)
		{
			//Rise
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.2);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.05);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-0.02);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]);
			delayms(1);
			Fvi16.CH[GPIO1].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]);
			delay(20);
			Fvi16.CH[GPIO1].mv(Trigger_V2);		
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]>1.5))
				{
					VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]=(Master_Trim_HighLi[2*i+TestNO_target]+Master_Trim_LowLi[2*i+TestNO_target])/2;		   
				}
				else
				{
					VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]=99999;
				}
			}
			//Falling
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.05);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.02);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]+0.01);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+2*i]-1e-3*Master_Trim_LowLi[TestNO_target+2*i+1]);
			delayms(1);
			Fvi16.CH[GPIO1].mv(Trigger_V1);
			delay(20);
			Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target+2*i]-1e-3*Master_Trim_HighLi[TestNO_target+2*i+1]);
			delayms(1);
			Fvi16.CH[GPIO1].mv(Trigger_V2);	
			FOR_EACH_SITE(nSiteIndex)
			{
				if((Trigger_V1[nSiteIndex]>1.5)&&(Trigger_V2[nSiteIndex]<1.5))
				{
					VIN3P3_OV_Warn_HYS_Meas[nSiteIndex]=(Master_Trim_HighLi[2*i+1+TestNO_target]+Master_Trim_LowLi[2*i+1+TestNO_target])/2;		   
				}
				else
				{
					VIN3P3_OV_Warn_HYS_Meas[nSiteIndex]=99999;
				}
			}
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+0, VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+2*i+1, VIN3P3_OV_Warn_HYS_Meas[nSiteIndex]);
		}
	}	
	//Disable check
	Reg[0x10f2].set_clr_inv(BIT5|BIT4);
	//Rise
	Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target]-0.1);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target]-0.05);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target]);
	delay(200);
	Fvi16.CH[GPIO1].mv(Trigger_V1);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+4]);
	delay(200);
	Fvi16.CH[GPIO1].mv(Trigger_V2);		
	FOR_EACH_SITE(nSiteIndex)
	{
		if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]<1.5))
		{
			VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]=1;		   
		}
		else
		{
			VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]=99999;
		}
	}
	//Fall
	Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+4]-1e-3*Master_Trim_LowLi[TestNO_target+5]+0.1);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+4]-1e-3*Master_Trim_LowLi[TestNO_target+5]+0.05);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+4]-1e-3*Master_Trim_LowLi[TestNO_target+5]+0.02);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_HighLi[TestNO_target+4]-1e-3*Master_Trim_LowLi[TestNO_target+5]);
	delay(200);
	Fvi16.CH[GPIO1].mv(Trigger_V1);
	delay(200);
	Fvi45.CH[VIN_3V3].fv(Master_Trim_LowLi[TestNO_target]-1e-3*Master_Trim_HighLi[TestNO_target+5]);
	delay(200);
	Fvi16.CH[GPIO1].mv(Trigger_V2);		
	FOR_EACH_SITE(nSiteIndex)
	{
		if((Trigger_V1[nSiteIndex]<1.5)&&(Trigger_V2[nSiteIndex]<1.5))
		{
			VIN3P3_OV_Warn_Fall_Meas[nSiteIndex]=1;		   
		}
		else
		{
			VIN3P3_OV_Warn_Fall_Meas[nSiteIndex]=99999;
		}
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, VIN3P3_OV_Warn_Rise_Meas[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, VIN3P3_OV_Warn_Fall_Meas[nSiteIndex]);
	}
	Fvi45.CH[VIN_3V3].fv(VIN_SET);	
	//************Power_off**********
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	Fvi16.CH[EXT_EN].open();
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VBYP]      .fv(0);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	delayms(2);
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]      .open();
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: LDO
STT_TESTFUNC LDO(CFunction *pFunction, int nSiteNo, int nTestFlag, LDO_params* ours)
{
	double LDO_Vdrop_0P8[LDO_MAX][SITE_MAX]={0}, LDO_Vdrop_1P8[LDO_MAX][SITE_MAX]={0};
	double LDO_LoadReg[LDO_MAX][SITE_MAX]={0}, LDO_LineReg[LDO_MAX][SITE_MAX]={0};
	double LDO_Rpd[LDO_MAX][SITE_MAX]={0}, LDO_Ipd[LDO_MAX][SITE_MAX]={0};

	double LDO_SST[LDO_MAX][SITE_MAX][8]={0};

	double V1_Measure[LDO_MAX][SITE_MAX]={0},V2_Measure[LDO_MAX][SITE_MAX]={0},V3_Measure[LDO_MAX][SITE_MAX]={0};
	double I_Measure[LDO_MAX][SITE_MAX]={0};

	int LDO_Vset_0P8_Data[LDO_MAX]={0};
	LDO_Vset_0P8_Data[0]=0.2/(0.005*(1+((Reg[0x1092].CMI&0x08)>>3)));
	LDO_Vset_0P8_Data[1]=0.2/(0.005*(1+((Reg[0x1092].CMI&0x04)>>2)));

    Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	i2c_init(HCTL_MODE,300000,2);
	delayms(2);
	Reg[0x1012].write(0xff);
	if(CMI_Rev_No==520102||CMI_Rev_No==522102||CMI_Rev_No==522104||CMI_Rev_No==521101||CMI_Rev_No==523103)
	{
		Reg[0x1012].write(_0x00);
		delayms(50);
	}
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	

	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();	

	delayms(2);

	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall50mA);

	
  	//************LDO_Drop**********
	double LDO_Drop_Load[LDO_MAX]={400,150};
	TestNO_target=2;	

	Relay.BIT[K12_OUTLDO1_Floating].set(); 
	Relay.BIT[K15_OUTLDO2_Floating].set();
    delayms(2);
	Reg[0x1093].set_clr_inv(BIT7|BIT3|BIT6|BIT2,BIT4|BIT0);//Normal mode, turn off LP mode
    Reg[0x1094].set_clr_inv(BITNONE,BIT6|BIT2);//Pldn_LPM_En=0
    Reg[0x10f2].write(0x30);

    //MEASURE VDROPOUT AT 0.8V
    Reg[0x1092].set_clr_inv(BITNONE,BIT1|BIT0);//Range default CMI & Clear Charge pumps       

	Reg[0x1096].write(LDO_Vset_0P8_Data[0]);
	Reg[0x1097].write(LDO_Vset_0P8_Data[0]);
	Reg[0x1098].write(LDO_Vset_0P8_Data[1]);
	Reg[0x1099].write(LDO_Vset_0P8_Data[1]);

	Fvi16.CH[VIN_LDO1].fv(0.7);
	Fvi16.CH[VIN_LDO2].fv(0.7);
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,500,-500);
		delayms(1);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-LDO_Drop_Load[nTILE]);
		delayms(2);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(LDO_Vdrop_0P8[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0);
	}
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, -1e3*LDO_Vdrop_0P8[nTILE][nSiteIndex]);			
	} 
    //MEASURE VDROPOUT AT 1.8V
    Reg[0x1092].set_clr_inv(BIT3|BIT2|BIT1|BIT0);//Range=1 & Enable Charge pumps
	Reg[0x1096].write(0x78);
	Reg[0x1097].write(0x78);
	Reg[0x1098].write(0x78);
	Reg[0x1099].write(0x78);

	Fvi16.CH[VIN_LDO1].fv(1.7);
	Fvi16.CH[VIN_LDO2].fv(1.7);
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		delayms(1);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-LDO_Drop_Load[nTILE]);
		delayms(2);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(LDO_Vdrop_1P8[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0);
	}
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, TestNO_target+2+nTILE, -1e3*LDO_Vdrop_1P8[nTILE][nSiteIndex]);
	} 
	Relay.BIT[K12_OUTLDO1_Floating].clr(); 
	Relay.BIT[K15_OUTLDO2_Floating].clr();

	Fvi16.CH[VIN_LDO1].fv(VIN_SET);
	Fvi16.CH[VIN_LDO2].fv(VIN_SET);
	Reg[0x1096].write_CMI();
	Reg[0x1097].write_CMI();
	Reg[0x1098].write_CMI();
	Reg[0x1099].write_CMI();
    //************LDO_Drop**********
  	//************LDO_LoadReg**********
	double LDO_Reg_Load[LDO_MAX]={400,250};
	TestNO_target=6;
	Reg[0x1091].write(0xff);//Disable the fault

	if(LDO_Vset[0]>1.1)
		Reg[0x1092].set_clr_inv(BIT1);
	if(LDO_Vset[1]>1.1)
		Reg[0x1092].set_clr_inv(BIT0);
	if(LDO_Vset[0]>1.1&&LDO_Vset[1]>1.1)
		Reg[0x1092].set_clr_inv(BIT1|BIT0);
    delayms(2);
	//Load 1mA
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(1);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-1);
		delayms(2);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(V1_Measure[nTILE]);
	}
	Relay.BIT[K12_OUTLDO1_Floating].set(); 
	delayms(2);
	Relay.BIT[K15_OUTLDO2_Floating].set();
	delayms(10);
	//floating Load 1mA
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(V2_Measure[nTILE]);
	} 
	//floating Load 400/250mA
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		if(LDO_Vset[nTILE]>1.3)
			Fqvi.CH[GP_OUTLDO[nTILE]].fi(-1,SITE_ALL,SET_CLAMP,2,-2,800,-800);
		else
			Fqvi.CH[GP_OUTLDO[nTILE]].fi(-1,SITE_ALL,SET_CLAMP,7,-7,800,-800);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.1*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.2*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.3*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.5*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.6*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.8*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-0.9*LDO_Reg_Load[nTILE]);
		delay(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-LDO_Reg_Load[nTILE]);
		delayms(5);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(V3_Measure[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0);
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_LoadReg[nTILE][nSiteIndex]=1e2*(V2_Measure[nTILE][nSiteIndex]-V3_Measure[nTILE][nSiteIndex])/V1_Measure[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, LDO_LoadReg[nTILE][nSiteIndex]);			
		}
	} 
   	Relay.BIT[K12_OUTLDO1_Floating].clr();
	Relay.BIT[K15_OUTLDO2_Floating].clr();
  	//************LDO_LoadReg**********
  	//************LDO_LineReg**********
	double LDO_Reg_Line[LDO_MAX]={1,10};
	TestNO_target=8;	
	Reg[0x1096].write(LDO_Vset_0P8_Data[0]);
	Reg[0x1097].write(LDO_Vset_0P8_Data[0]);
	Reg[0x1098].write(LDO_Vset_0P8_Data[1]);
	Reg[0x1099].write(LDO_Vset_0P8_Data[1]);
    Reg[0x1092].set_clr_inv(BITNONE,BIT1|BIT0);//Range default CMI & Clear Charge pumps       

	Fvi16.CH[VIN_LDO1].fv(1.2);
	Fvi16.CH[VIN_LDO2].fv(1.2);
	//VINLDO=1.2
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,40,-40);
		delayms(1);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-LDO_Reg_Line[nTILE]);
		delayms(2);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(V1_Measure[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0);
	} 
	//VINLDO=3.6
	Fvi16.CH[VIN_LDO1].fv(VIN_SET+0.3);
	Fvi16.CH[VIN_LDO2].fv(VIN_SET+0.3);
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(-LDO_Reg_Line[nTILE]);
		delayms(2);
		Fqvi.CH[GP_OUTLDO[nTILE]].mv(V2_Measure[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fi(0);
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_LineReg[nTILE][nSiteIndex]=1e2*(V1_Measure[nTILE][nSiteIndex]-V2_Measure[nTILE][nSiteIndex])/V1_Measure[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, LDO_LineReg[nTILE][nSiteIndex]);			
		}
	} 
	Fvi16.CH[VIN_LDO1].fv(VIN_SET);
	Fvi16.CH[VIN_LDO2].fv(VIN_SET);
	//Reg[0x1092].write_CMI();      
	Reg[0x1096].write_CMI();
	Reg[0x1097].write_CMI();
	Reg[0x1098].write_CMI();
	Reg[0x1099].write_CMI();
	Reg[0x10f2].write_CMI();
  	//************LDO_LineReg**********
	
  	//************LDO_Rpd**********
	TestNO_target=10;	
	Reg[0x2095].write(0xc0);      
	Reg[0x1093].set_clr_inv(BITNONE,BIT6|BIT2);   
    Reg[0x1094].set_clr_inv(BITNONE,BIT7|BIT3);   
	Reg[0x10f2].set_clr_inv(BIT3);//Add this reg
	delayms(1);
	Fqvi.CH[OUT_LDO1].fv(0,SITE_ALL,SET_CLAMP,2,-2,40,-40);
	Fqvi.CH[OUT_LDO2].fv(0,SITE_ALL,SET_CLAMP,2,-2,40,-40);
	delayms(1);
	Fqvi.CH[OUT_LDO1].fv(0.1);
	Fqvi.CH[OUT_LDO2].fv(0.1);
	delayms(10);
	Fqvi.CH[OUT_LDO1].mv(V1_Measure[0]);
	Fqvi.CH[OUT_LDO2].mv(V1_Measure[1]);
	delayms(1);
	Fqvi.CH[OUT_LDO1].mi(I_Measure[0]);
	Fqvi.CH[OUT_LDO2].mi(I_Measure[1]);
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_Rpd[nTILE][nSiteIndex]=1e3*V1_Measure[nTILE][nSiteIndex]/I_Measure[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, LDO_Rpd[nTILE][nSiteIndex]);					
		}
	}
	Fqvi.CH[OUT_LDO1].fi(0);
	Fqvi.CH[OUT_LDO2].fi(0);

	Reg[0x2095].write(_0x00);      
	Reg[0x10f2].write_CMI();
  	//************LDO_Rpd**********

	//************LDO_Ipd**********

	TestNO_target=12;	
    Reg[0x1093].set_clr_inv(BIT7|BIT6|BIT3|BIT2);     
    Reg[0x1094].set_clr_inv(BIT6|BIT2);     
	Reg[0x2095].write(_0x00);      
	
	Fqvi.CH[OUT_LDO1].fv(LDO_Vset[0]+0.01,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fqvi.CH[OUT_LDO2].fv(LDO_Vset[1]+0.01,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	delayms(10);
	Fqvi.CH[OUT_LDO1].mi(LDO_Ipd[0]);
	Fqvi.CH[OUT_LDO2].mi(LDO_Ipd[1]);
    FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, LDO_Ipd[nTILE][nSiteIndex]);					
		}
	}
	Fqvi.CH[OUT_LDO1].fi(0);
	Fqvi.CH[OUT_LDO2].fi(0);
	Fqvi.CH[OUT_LDO1].open();
	Fqvi.CH[OUT_LDO2].open();
	Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();

  	//************LDO_Ipd**********
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi16.CH[VIN_LDO1].fv(0);
	Fvi16.CH[VIN_LDO2].fv(0);
	Fvi45.CH[VIN_LDO1]	.open();
	Fvi45.CH[VIN_LDO2]	.open();
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Buck_Sub1
STT_TESTFUNC Buck_Sub1(CFunction *pFunction, int nSiteNo, int nTestFlag, Buck_Sub1_params* ours)
{
	double EXT_PG_V1[SITE_MAX]={0},EXT_PG_V2[SITE_MAX]={0};
	double Buck_EA0P5V[PMU_MAX][SITE_MAX]={0},Buck_ICONSTN_1uA[PMU_MAX][SITE_MAX]={0},Buck_SST_0[PMU_MAX][SITE_MAX][8]={0},Buck_SST_1[PMU_MAX][SITE_MAX][8]={0};
	double Buck_Rpd[PMU_MAX][SITE_MAX]={0},Buck_Ipd[PMU_MAX][SITE_MAX]={0};
	double V_Measure[SITE_MAX]={0},I_Measure[SITE_MAX]={0};
    int B5_AUX_POK[SITE_MAX]={0},Data_read[4][SITE_MAX]={0};

	//Connect_all_cap();//enable if run this function only
    Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	i2c_init(HCTL_MODE,300000,2);//enable if run this function only
	delayms(2);//enable if run this function only

	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	
	Buck_TM_Reset();

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
  	//************Buck_EA0P5V**********
	TestNO_target=2;	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall1V1,Stall50uA);//use measure voltage
	Relay.BIT[K26_AMUX_Buffer].set(); // CLOSE AMUX_BUF
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.02*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x31);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x3f&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);	
	}
	delayms(5);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BITNONE,BIT5|BIT4|BIT2);	
		Reg[0x2008].write(0x28);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V1);
		Reg[0x2008].write(0x38);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(EXT_PG_V2);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_EA0P5V[nTILE][nSiteIndex]=(EXT_PG_V1[nSiteIndex]+EXT_PG_V2[nSiteIndex])/2;
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);

		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, Buck_EA0P5V[nTILE][nSiteIndex]);
		}	
	}
	Relay.BIT[K26_AMUX_Buffer].clr(); // CLOSE AMUX_BUF
	
	delayms(2);
  	//************Buck_EA0P5V**********
  	//************Buck_ICONSTN_1uA**********
	TestNO_target=7;	
	Reg[0x1019].write(0x08);
	Reg[0x101a].write(0x08);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(_0x00);
	Reg[0x2008].write(0x08);
	Fvi16.CH[EXT_PG].fv(0,SITE_ALL,Stall1V1,Stall50uA);//use measure voltage
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x39);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BITNONE,BIT5|BIT4|BIT2);	
		delayms(1);
		Fvi16.CH[EXT_PG].mi(Buck_ICONSTN_1uA[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, -1e3*Buck_ICONSTN_1uA[nTILE][nSiteIndex]);
		}
	}
	Fvi16.CH[EXT_PG].fi(0);
	Fvi16.CH[EXT_PG].open();

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();			
	}
  	//************Buck_ICONSTN_1uA**********
  	//************Buck_SST**********
	TestNO_target=12;	
	Reg[0x2008].write(0x04);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K21_GPIO1_TMUA].set();
	delayms(2);
	TMU_CH1.stallset(Stall7V,TMU4mS,5);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_User_Buck_Addr[nTILE]+0x08].set_clr_inv(BITNONE,BIT3);
	}

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xae);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7,BIT6|BIT5);//Confiure GPIO1 set:10
		//SST_0
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6|BIT1);//Confiure GPIO1 set:10
		delayms(1);				
		for(i=0;i<8;i++)
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x03].write((0x01|(0x8f&Reg[GP_User_Buck_Addr[nTILE]+0x03].CMI))|(i<<4));//SST set	
			delayms(2);				
			TMU_CH1.waitTRIG();
			delayms(1);	
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);//buck on
			delay(300);	
			TMU_CH1.read();
			FOR_EACH_SITE(nSiteIndex)
				Buck_SST_0[nTILE][nSiteIndex][i]=meas_data[nSiteIndex+1];			
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);//buck off
			FOR_EACH_SITE(nSiteIndex)
				pFunction->LogResult(NSITEINDEX, TestNO_target+8*nTILE+i, 0.8*Buck_SST_0[nTILE][nSiteIndex][i]);
		}
		//SST_1
		/*
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT1,BIT6);//Confiure GPIO1 set:10
		for(i=0;i<8;i++)
		{
			Reg[GP_User_Buck_Addr[nTILE]+0x03].write((0x01|(0x8f&Reg[GP_User_Buck_Addr[nTILE]+0x03].CMI))|(i<<4));//SST set	
			delayms(1);	
			TMU_CH1.waitTRIG();
			delayms(1);	
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);//buck on
			delay(100);	
			TMU_CH1.read();
			FOR_EACH_SITE(nSiteIndex)
				Buck_SST_1[nTILE][nSiteIndex][i]=meas_data[nSiteIndex+1];			
			Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);//buck off
			FOR_EACH_SITE(nSiteIndex)
				pFunction->LogResult(NSITEINDEX, TestNO_target+16*nTILE+i+8, 0.8*Buck_SST_1[nTILE][nSiteIndex][i]);
		}
		*/
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fi(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	Relay.BIT[K21_GPIO1_TMUA].clr();
	Relay.BIT[K47_TMU].clr();
  	//************Buck_SST**********
  	//************Buck_Rpd**********
	TestNO_target=52;	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);//buck off
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT7|BIT6|BIT5);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.1,SITE_ALL,SET_CLAMP,2,-2,40,-40);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].mv(V_Measure);
		Fvi45.CH[GP_FB_Buck[nTILE]].mi(I_Measure);
		FOR_EACH_SITE(nSiteIndex)
			Buck_Rpd[nTILE][nSiteIndex]=1e3*V_Measure[nSiteIndex]/I_Measure[nSiteIndex];	
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, Buck_Rpd[nTILE][nSiteIndex]);
		//Reg[GP_User_Buck_Addr[nTILE]+0x01].write_CMI();
	}
  	//************Buck_Rpd**********
  	//************Buck_Ipd**********
	TestNO_target=57;	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0,BIT1);//buck on
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT6,BIT7|BIT5);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.5,SITE_ALL,SET_CLAMP,2,-2,40,-40);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].mi(Buck_Ipd[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, TestNO_target+nTILE, Buck_Ipd[nTILE][nSiteIndex]);
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);//buck off
		Reg[GP_User_Buck_Addr[nTILE]+0x04].write_CMI();
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();	
	}
  	//************Buck_Ipd**********
	//************B5_AUX_POK**********
	TestNO_target=62;	
	Reg[0x1016].write(0x40);
	Reg[0x1081].write(0x78);
	Reg[0x1082].write(0x78);
	Reg[0x1083].write(0x03);
	Reg[0x1084].write(_0x00);
	Reg[0x1085].write(0x45);	
	Reg[0x1084].set_clr_inv(BIT7|BIT3);
	Reg[0x1012].set_clr_inv(BIT3|BIT0);

	Fvi45.CH[FB5]		.fv(1.8,SITE_ALL,SET_CLAMP,7,-7,200,-200);
	Fvi45.CH[VBYP]		.fv(0);

	//FB5=1.6V
	Fvi45.CH[FB5]		.fv(1.6);
    delayms(2);
	Reg[0x1004].write(0x55);
	Reg[0x1004].read(readback);
	FOR_EACH_SITE(nSiteIndex) Data_read[0][nSiteIndex]=readback[nSiteIndex];
	Reg[0x1004].write(0xaa);
	Reg[0x1004].read(readback);
	FOR_EACH_SITE(nSiteIndex) Data_read[1][nSiteIndex]=readback[nSiteIndex];
	//FB5=2.2V
	Fvi45.CH[FB5]		.fv(2.2);
	delayms(2);
	Reg[0x1004].write(0x55);
	Reg[0x1004].read(readback);
	FOR_EACH_SITE(nSiteIndex) Data_read[2][nSiteIndex]=readback[nSiteIndex];

	Reg[0x1004].write(0xaa);
	Reg[0x1004].read(readback);
	FOR_EACH_SITE(nSiteIndex)
	{
		Data_read[3][nSiteIndex]=readback[nSiteIndex];
			if((Data_read[0][nSiteIndex]==0x55)&&(Data_read[1][nSiteIndex]==0xaa)&&(Data_read[2][nSiteIndex]==0x55)&&(Data_read[3][nSiteIndex]==0xaa))
			B5_AUX_POK[nSiteIndex]=1;
	}
	Fvi45.CH[FB5]		.fi(0);
	Fvi45.CH[FB5]		.open();
	FOR_EACH_SITE(nSiteIndex)
		pFunction->LogResult(NSITEINDEX, TestNO_target, B5_AUX_POK[nSiteIndex]);

  	//************Buck_Ipd**********
  	//************Power_off**********
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		
	}	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Buck_Sub2
STT_TESTFUNC Buck_Sub2(CFunction *pFunction, int nSiteNo, int nTestFlag, Buck_Sub2_params* ours)
{
	double Buck_POK0_R[PMU_MAX][SITE_MAX]={0},Buck_POK1_R[PMU_MAX][SITE_MAX]={0}, Buck_POK_F[PMU_MAX][SITE_MAX]={0};
	double Buck_POK_HYS0[PMU_MAX][SITE_MAX]={0},Buck_POK_HYS1[PMU_MAX][SITE_MAX]={0};

	double Buck_OV_R[PMU_MAX][SITE_MAX]={0},Buck_OV_F[PMU_MAX][SITE_MAX]={0},Buck_OV_HYS[PMU_MAX][SITE_MAX]={0};

    //FTP Limit value
	double Buck_Sub2_HighLi[101]={0},Buck_Sub2_LowLi[101]={0};
    for(i=0;i<100;i++)
	{
		pFunction->GetHiLimit(i,&Buck_Sub2_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Buck_Sub2_LowLi[i+1]);//item 1-48
	}
	//************Power_on**********
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	delayms(1);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	i2c_init(HCTL_MODE,300000,2);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
	Disable_Sequence();
    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
	Buck_TM_Reset();

	Fvi16.CH[GPIO1]		.fi(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x2008].write(0x04);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);

		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0xfc);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT6);
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x3f&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xef&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x07].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xf7&readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x08].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(readback);//Need to set Buck LSILIM_SD=0 for the POK/OV/VTH test
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=0x7f&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);
	}
	delayms(2);
	//************Buck_POK**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		if(Buck_Vset[nTILE]<1.5)
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,40,-40);

		delayms(1);
		//========Measure pre value=========//
		Reg[GP_User_Buck_Addr[nTILE]+0x00].write(0x0f);
		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xa8);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);


		//Rising: POK=0,Hys=0
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x05);//Set POK=0,HYS=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*6+2]*Buck_Vset[nTILE]);
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	

		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*6+2]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*6+2]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK0_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling: POK=0,Hys=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+2]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+2]-Buck_Sub2_LowLi[nTILE*6+3])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+2]-Buck_Sub2_HighLi[nTILE*6+3])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS0[nTILE][nSiteIndex]=Buck_POK0_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		//Falling: POK=0,Hys=1
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x45);//Set POK=0,HYS=1

		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+2]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+2]-Buck_Sub2_LowLi[nTILE*6+4])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+2]-Buck_Sub2_HighLi[nTILE*6+4])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS1[nTILE][nSiteIndex]=Buck_POK0_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*6+2, Buck_POK0_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+3, Buck_POK_HYS0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+4, Buck_POK_HYS1[nTILE][nSiteIndex]);
		}
		//Rising: POK=1,Hys=0
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x85);//Set POK=1,HYS=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*6+5]*Buck_Vset[nTILE]);
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	
		
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*6+5]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*6+5]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK1_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling: POK=1,Hys=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+5]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+5]-Buck_Sub2_LowLi[nTILE*6+6])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+5]-Buck_Sub2_HighLi[nTILE*6+6])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS0[nTILE][nSiteIndex]=Buck_POK1_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		//Falling: POK=1,Hys=1
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0xc5);//Set POK=1,HYS=1
		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+5]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+5]-Buck_Sub2_LowLi[nTILE*6+7])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+5]-Buck_Sub2_HighLi[nTILE*6+7])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS1[nTILE][nSiteIndex]=Buck_POK1_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*6+5, Buck_POK1_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+6, Buck_POK_HYS0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+7, Buck_POK_HYS1[nTILE][nSiteIndex]);
		}
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0xfc);
	}
	//************Buck_POK**********
	//************Buck_OV**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x2f);		
		//Rising
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*3+32]*Buck_Vset[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	
		delayms(1);		
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*3+32]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*3+32]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,10,10);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OV_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*3+33]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_HighLi[nTILE*3+33]*Buck_Vset[nTILE],1e-2*Buck_Sub2_LowLi[nTILE*3+33]*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OV_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_OV_HYS[nTILE][nSiteIndex]=Buck_OV_R[nTILE][nSiteIndex]-Buck_OV_F[nTILE][nSiteIndex];
		}

		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*3+32, Buck_OV_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*3+33, Buck_OV_F[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*3+34, Buck_OV_HYS[nTILE][nSiteIndex]);
		}
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);
	}
	//************Buck_OV**********

	//************Buck_POK_LPM**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xc0|readback[nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
		Reg[GP_User_Buck_Addr[nTILE]+0x00].write(0x0f);		
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0xe8);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(_0x00);
		//Rising: POK=0,Hys=0
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x05);//Set POK=0,HYS=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*6+47]*Buck_Vset[nTILE]);
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	

		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*6+47]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*6+47]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,20,20);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK0_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling: POK=0,Hys=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+47]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+47]-Buck_Sub2_LowLi[nTILE*6+48])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+47]-Buck_Sub2_HighLi[nTILE*6+48])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS0[nTILE][nSiteIndex]=Buck_POK0_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		//Falling: POK=0,Hys=1
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x45);//Set POK=0,HYS=1

		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+47]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+47]-Buck_Sub2_LowLi[nTILE*6+49])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+47]-Buck_Sub2_HighLi[nTILE*6+49])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS1[nTILE][nSiteIndex]=Buck_POK0_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*6+47, Buck_POK0_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+48, Buck_POK_HYS0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+49, Buck_POK_HYS1[nTILE][nSiteIndex]);
		}
		//Rising: POK=1,Hys=0
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0x85);//Set POK=1,HYS=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*6+50]*Buck_Vset[nTILE]);
		delayms(2);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	
		
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*6+50]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*6+50]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK1_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling: POK=1,Hys=0
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+50]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+50]-Buck_Sub2_LowLi[nTILE*6+51])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+50]-Buck_Sub2_HighLi[nTILE*6+51])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS0[nTILE][nSiteIndex]=Buck_POK1_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		//Falling: POK=1,Hys=1
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write((Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0x3c)|0xc5);//Set POK=1,HYS=1
		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*6+50]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*(Buck_Sub2_HighLi[nTILE*6+50]-Buck_Sub2_LowLi[nTILE*6+52])*Buck_Vset[nTILE],1e-2*(Buck_Sub2_LowLi[nTILE*6+50]-Buck_Sub2_HighLi[nTILE*6+52])*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_POK_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_POK_HYS1[nTILE][nSiteIndex]=Buck_POK1_R[nTILE][nSiteIndex]-Buck_POK_F[nTILE][nSiteIndex];
		}
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*6+50, Buck_POK1_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+51, Buck_POK_HYS0[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*6+52, Buck_POK_HYS1[nTILE][nSiteIndex]);
		}
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(Reg[GP_User_Buck_Addr[nTILE]+0x05].CMI&0xfc);
	}
	//************Buck_POK_LPM**********
	//************Buck_OV_LPM**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x6f);		
		//Rising
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT2|BIT0,BIT1);
		delayms(1);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.95*1e-2*Buck_Sub2_LowLi[nTILE*3+77]*Buck_Vset[nTILE]);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].set_clr_inv(BIT7|BIT5,BIT6);//Confiure GPIO1 set:10	
		delayms(1);		
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_LowLi[nTILE*3+77]*Buck_Vset[nTILE],1e-2*Buck_Sub2_HighLi[nTILE*3+77]*Buck_Vset[nTILE],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag,10,10);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OV_R[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
		}
		//Falling
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*1e-2*Buck_Sub2_HighLi[nTILE*3+78]*Buck_Vset[nTILE]);
		delayms(1);
		sweep_VI(RESOURCE_FVI45,GP_FB_Buck[nTILE],VOLTAGE,RESOURCE_FVI16,GPIO1,VOLTAGE,
			1e-2*Buck_Sub2_HighLi[nTILE*3+78]*Buck_Vset[nTILE],1e-2*Buck_Sub2_LowLi[nTILE*3+78]*Buck_Vset[nTILE],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OV_F[nTILE][nSiteIndex]=1e2*global_temp[nSiteIndex]/Buck_Vset[nTILE];
			Buck_OV_HYS[nTILE][nSiteIndex]=Buck_OV_R[nTILE][nSiteIndex]-Buck_OV_F[nTILE][nSiteIndex];
		}

		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, nTILE*3+77, Buck_OV_R[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*3+78, Buck_OV_F[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, nTILE*3+79, Buck_OV_HYS[nTILE][nSiteIndex]);
		}
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0|BIT1);
	}
	//************Buck_OV_LPM**********

	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	Fvi16.CH[GPIO1]		.open();
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(2);
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Buck_Sub3
STT_TESTFUNC Buck_Sub3(CFunction *pFunction, int nSiteNo, int nTestFlag, Buck_Sub3_params* ours)
{

	double Vramsum_min[SITE_MAX]={0}, Vcomp_clamp[SITE_MAX]={0}, Vcc_Vsm_Delta[SITE_MAX]={0};
	double Buck_Minscl[PMU_MAX][SITE_MAX]={0};
	double I_AVIN1[SITE_MAX]={0}, I_AVIN2[SITE_MAX]={0}, I_VINB12[SITE_MAX]={0}, I_VINB3[SITE_MAX]={0}; 
	double Iq_temp1[SITE_MAX]={0}, Iq_temp2[SITE_MAX]={0}, Iq_temp3[SITE_MAX]={0};	
	double Iq_LPM[SITE_MAX]={0}, Iq_LPM_Font[SITE_MAX];

	if(PMU_MultiP_Mode[0]||PMU_MultiP_Mode[1])
	{		
		//************Power_on**********
		Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);	
		delayms(2);
		Relay.BIT[K48_I2C].set();
		delayms(2);
		Reg[0x2000].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
		Reg[0x2000].write(readback);
		Reg[0x1012].write(0xff);
		Reg[0x2007].write(0x80);
		Reg[0x20fa].write(0x20);
		Disable_Sequence();
		Buck_TM_Reset();
		Factory_Mode_Check();
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
		}
		Relay.BIT[K26_AMUX_Buffer].set(); // CLOSE AMUX_BUF
		delayms(2);
		Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);//use measure voltage
		//************Vcomp_clamp**********
		Fvi45.CH[VIN_B12].fv(VIN_SET, SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB1].fv(1.05*Buck_Vset[0], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		delayms(2);
		Reg[0x2056].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
		Reg[0x2056].write(readback);
		Reg[0x1044].set_clr_inv(BITNONE,BIT3|BIT6);

		Reg[0x1019].write(0x08);
		Reg[0x101a].write(0x80);
		Reg[0x101b].write(_0x00);
		Reg[0x101c].write(_0x00);//Config EXT_PG pin

		Reg[0x2008].write(0x08);
		Reg[0x1045].set_clr_inv(BIT3|BIT2|BIT0,BIT1);
		Reg[0x1055].set_clr_inv(BIT3|BIT2|BIT0,BIT1);
		Reg[0x204d].write(0x0c);
		Reg[0x204f].write(0x10);	
		delayms(1);
		Fvi16.CH[EXT_PG].mv(Vcomp_clamp);
		FOR_EACH_SITE(nSiteIndex)
		{
			//pFunction->LogResult(NSITEINDEX, 2, Vcomp_clamp[nSiteIndex]);
		}
		//************Vcomp_clamp**********
		//************Vramsum_min**********
		Reg[0x2056].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
		Reg[0x2056].write(readback);
		Reg[0x1044].set_clr_inv(BITNONE,BIT3|BIT6);
		Reg[0x204d].write(0x1f);
		delayms(1);
		Fvi16.CH[EXT_PG].mv(Vramsum_min);
		FOR_EACH_SITE(nSiteIndex)
		{
			Vcc_Vsm_Delta[nSiteIndex]=Vramsum_min[nSiteIndex]-Vcomp_clamp[nSiteIndex];
			//pFunction->LogResult(NSITEINDEX, 3, Vramsum_min[nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 4, Vcc_Vsm_Delta[nSiteIndex]);
		}
		//************Vramsum_min**********
		Relay.BIT[K26_AMUX_Buffer].clr(); // open AMUX_BUF
		Fvi16.CH[EXT_PG].open();//open VI
		delayms(2);
		Reg[0x2008].write(_0x00);
		Fvi45.CH[FB1].fi(0);
		Fvi45.CH[FB1].open();
		Disconnect_all_cap();
		//************B1_IQ_LPM**********
		Fvi45.CH[FB3].fv(0.9*Buck_Vset[2], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB3].fv(1.03*Buck_Vset[2], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB1].fv(0.9*Buck_Vset[0], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB2].fv(0.9*Buck_Vset[0], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB1].fv(1.03*Buck_Vset[0], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[FB2].fv(1.03*Buck_Vset[0], SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		delayms(1);	
		Reg[0x2056].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x7f&readback[nSiteIndex]);
		Reg[0x2056].write(readback);

		Reg[0x1044].set_clr_inv(BITNONE,BIT3|BIT6);
		Reg[0x1054].set_clr_inv(BITNONE,BIT3|BIT6);
		Reg[0x1040].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
		Reg[0x1050].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
		Reg[0x1045].set_clr_inv(BIT2,BIT0|BIT1);
		Reg[0x1055].set_clr_inv(BIT2,BIT0|BIT1);
		Reg[0x1060].write(0x0f);
		Reg[0x1063].set_clr_inv(BIT0);
		Reg[0x1065].set_clr_inv(BIT2|BIT0,BIT1);
		delayms(2);	
		SDASCL_Connect_VI();
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 4, -4);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 4, -4);
		delayms(15);
		for(i=0;i<50;i++)
		{
			Fvi45.CH[VOUT_SW]	.mi(I_AVIN2);
			delayms(2);
			FOR_EACH_SITE(nSiteIndex)
			{
				if(fabs(1e3*I_AVIN2[nSiteIndex]-160)<20&&Flag[nSiteIndex]==0)
				{
					Flag[nSiteIndex]=1;
				}	
			}
			if(isAllFlagSet(false))
				break;
		}	
		clrAllFlag();
		
		Fvi45.CH[VIN_B12].mi(I_VINB12);
		Fvi45.CH[VIN_B3].mi(I_VINB3);
		Fvi45.CH[VIN_3V3].mi(I_AVIN1);
		Fvi45.CH[VOUT_SW].mi(I_AVIN2);
		FOR_EACH_SITE(nSiteIndex)
			Iq_temp1[nSiteIndex]=I_AVIN1[nSiteIndex]+I_AVIN2[nSiteIndex]+I_VINB12[nSiteIndex]+I_VINB3[nSiteIndex];
		SDASCL_Connect_I2C();


		Reg[0x1045].set_clr_inv(BIT2|BIT0,BIT1);
		Reg[0x1055].set_clr_inv(BIT2|BIT0,BIT1);		
		Reg[0x2040].set_clr_inv(BIT6|BIT7,BIT2|BIT0);
		Reg[0x2050].set_clr_inv(BIT6|BIT7,BIT2|BIT0);		
		Reg[0x2046].set_clr_inv(BIT2);
		Reg[0x2056].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x04|(0x7f&readback[nSiteIndex]);
		Reg[0x2056].write(readback);

		//Reg[0x2056].set_clr_inv(BIT2);				
		SDASCL_Connect_VI();
		delayms(15);
		Fvi45.CH[VIN_B12].mi(I_VINB12);
		Fvi45.CH[VIN_B3].mi(I_VINB3);
		Fvi45.CH[VIN_3V3].mi(I_AVIN1);
		Fvi45.CH[VOUT_SW].mi(I_AVIN2);
		FOR_EACH_SITE(nSiteIndex)
		{	
			Iq_temp2[nSiteIndex]=I_AVIN1[nSiteIndex]+I_AVIN2[nSiteIndex]+I_VINB12[nSiteIndex]+I_VINB3[nSiteIndex];
			Iq_LPM[nSiteIndex]=Iq_temp2[nSiteIndex]-Iq_temp1[nSiteIndex];
		}
		SDASCL_Connect_I2C();
		Reg[0x1044].set_clr_inv(BIT3,BIT6);
		Reg[0x1054].set_clr_inv(BIT3,BIT6);

		SDASCL_Connect_VI();
		delayms(20);
		Fvi45.CH[VIN_B12].mi(I_VINB12);
		Fvi45.CH[VIN_B3].mi(I_VINB3);
		Fvi45.CH[VIN_3V3].mi(I_AVIN1);
		Fvi45.CH[VOUT_SW].mi(I_AVIN2);
		FOR_EACH_SITE(nSiteIndex)
		{	
			Iq_temp3[nSiteIndex]=I_AVIN1[nSiteIndex]+I_AVIN2[nSiteIndex]+I_VINB12[nSiteIndex]+I_VINB3[nSiteIndex];
			Iq_LPM_Font[nSiteIndex]=Iq_temp3[nSiteIndex]-Iq_temp1[nSiteIndex];
		}
		Fvi16.CH[SDA]	.open();
		Fvi16.CH[SCL]	.open();
		Fvi45.CH[FB1].fi(0);
		Fvi45.CH[FB1].open();
		Fvi45.CH[FB2].fi(0);
		Fvi45.CH[FB2].open();
		Fvi45.CH[FB3].fi(0);
		Fvi45.CH[FB3].open();
		Fvi45.CH[VIN_B12].fv(0);
		Fvi45.CH[VIN_B12].open();	
		Fvi45.CH[VBYP]      .fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);	
		Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[VIN_3V3]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
																																		  
			pFunction->LogResult(NSITEINDEX, 11, 1e3*Iq_LPM[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 12, 1e3*Iq_LPM_Font[nSiteIndex]);
																						
		}
	}
	//************Buck_Freq**********
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_Minscl[nTILE][nSiteIndex]=1e-1*Ton_Measure[nTILE][nSiteIndex]*Buck12_MultiP_Freq[nTILE][nSiteIndex]*VIN_SET/(0.95*Buck_Vset[nTILE]);
				pFunction->LogResult(NSITEINDEX, 5+3*nTILE, Buck12_MultiP_Freq[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 6+3*nTILE, Buck_Minscl[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 7+3*nTILE, Buck_Minscl_PWM[nTILE][nSiteIndex]);
			}
		}
	}
	//************Buck_Freq**********

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Buck_Sub4
STT_TESTFUNC Buck_Sub4(CFunction *pFunction, int nSiteNo, int nTestFlag, Buck_Sub4_params* ours)
{
	//FTP Limit value
	double EXTPG_VIH_FB5_1P6[SITE_MAX]={0},EXTPG_VIL_FB5_1P6[SITE_MAX]={0},EXTPG_HYS_FB5_1P6[SITE_MAX]={0};
	double EXTEN_VIH_FB5_1P6[SITE_MAX]={0},EXTEN_VIL_FB5_1P6[SITE_MAX]={0},EXTEN_HYS_FB5_1P6[SITE_MAX]={0};
	double GPIO0_VIH_FB5_1P6[SITE_MAX]={0},GPIO0_VIL_FB5_1P6[SITE_MAX]={0},GPIO0_HYS_FB5_1P6[SITE_MAX]={0};
	double GPIO1_VIH_FB5_1P6[SITE_MAX]={0},GPIO1_VIL_FB5_1P6[SITE_MAX]={0},GPIO1_HYS_FB5_1P6[SITE_MAX]={0};
	double PWRDN0_VIH_FB5_1P6[SITE_MAX]={0},PWRDN0_VIL_FB5_1P6[SITE_MAX]={0},PWRDN0_HYS_FB5_1P6[SITE_MAX]={0};
	double PWRDN1_VIH_FB5_1P6[SITE_MAX]={0},PWRDN1_VIL_FB5_1P6[SITE_MAX]={0},PWRDN1_HYS_FB5_1P6[SITE_MAX]={0};
	double LPM_VIH_FB5_1P6[SITE_MAX]={0},LPM_VIL_FB5_1P6[SITE_MAX]={0},LPM_HYS_FB5_1P6[SITE_MAX]={0};
	double EXTPG_VIH_FB5_2P0[SITE_MAX]={0},EXTPG_VIL_FB5_2P0[SITE_MAX]={0},EXTPG_HYS_FB5_2P0[SITE_MAX]={0};
	double EXTEN_VIH_FB5_2P0[SITE_MAX]={0},EXTEN_VIL_FB5_2P0[SITE_MAX]={0},EXTEN_HYS_FB5_2P0[SITE_MAX]={0};
	double GPIO0_VIH_FB5_2P0[SITE_MAX]={0},GPIO0_VIL_FB5_2P0[SITE_MAX]={0},GPIO0_HYS_FB5_2P0[SITE_MAX]={0};
	double GPIO1_VIH_FB5_2P0[SITE_MAX]={0},GPIO1_VIL_FB5_2P0[SITE_MAX]={0},GPIO1_HYS_FB5_2P0[SITE_MAX]={0};
	double PWRDN0_VIH_FB5_2P0[SITE_MAX]={0},PWRDN0_VIL_FB5_2P0[SITE_MAX]={0},PWRDN0_HYS_FB5_2P0[SITE_MAX]={0};
	double PWRDN1_VIH_FB5_2P0[SITE_MAX]={0},PWRDN1_VIL_FB5_2P0[SITE_MAX]={0},PWRDN1_HYS_FB5_2P0[SITE_MAX]={0};
	double LPM_VIH_FB5_2P0[SITE_MAX]={0},LPM_VIL_FB5_2P0[SITE_MAX]={0},LPM_HYS_FB5_2P0[SITE_MAX]={0};

	double VOUTSW_EXTPG_VIH_3P64[SITE_MAX]={0},VOUTSW_EXTPG_VIL_3P64[SITE_MAX]={0},VOUTSW_EXTPG_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_EXTEN_VIH_3P64[SITE_MAX]={0},VOUTSW_EXTEN_VIL_3P64[SITE_MAX]={0},VOUTSW_EXTEN_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_GPIO0_VIH_3P64[SITE_MAX]={0},VOUTSW_GPIO0_VIL_3P64[SITE_MAX]={0},VOUTSW_GPIO0_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_GPIO1_VIH_3P64[SITE_MAX]={0},VOUTSW_GPIO1_VIL_3P64[SITE_MAX]={0},VOUTSW_GPIO1_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_PWRDN0_VIH_3P64[SITE_MAX]={0},VOUTSW_PWRDN0_VIL_3P64[SITE_MAX]={0},VOUTSW_PWRDN0_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_PWRDN1_VIH_3P64[SITE_MAX]={0},VOUTSW_PWRDN1_VIL_3P64[SITE_MAX]={0},VOUTSW_PWRDN1_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_LPM_VIH_3P64[SITE_MAX]={0},VOUTSW_LPM_VIL_3P64[SITE_MAX]={0},VOUTSW_LPM_HYS_3P64[SITE_MAX]={0};
	double VOUTSW_EXTPG_VIH_2P65[SITE_MAX]={0},VOUTSW_EXTPG_VIL_2P65[SITE_MAX]={0},VOUTSW_EXTPG_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_EXTEN_VIH_2P65[SITE_MAX]={0},VOUTSW_EXTEN_VIL_2P65[SITE_MAX]={0},VOUTSW_EXTEN_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_GPIO0_VIH_2P65[SITE_MAX]={0},VOUTSW_GPIO0_VIL_2P65[SITE_MAX]={0},VOUTSW_GPIO0_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_GPIO1_VIH_2P65[SITE_MAX]={0},VOUTSW_GPIO1_VIL_2P65[SITE_MAX]={0},VOUTSW_GPIO1_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_PWRDN0_VIH_2P65[SITE_MAX]={0},VOUTSW_PWRDN0_VIL_2P65[SITE_MAX]={0},VOUTSW_PWRDN0_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_PWRDN1_VIH_2P65[SITE_MAX]={0},VOUTSW_PWRDN1_VIL_2P65[SITE_MAX]={0},VOUTSW_PWRDN1_HYS_2P65[SITE_MAX]={0};
	double VOUTSW_LPM_VIH_2P65[SITE_MAX]={0},VOUTSW_LPM_VIL_2P65[SITE_MAX]={0},VOUTSW_LPM_HYS_2P65[SITE_MAX]={0};

	double Add_HighLi[201]={0},Add_LowLi[201]={0};
    for(i=0;i<200;i++)
	{
		pFunction->GetHiLimit(i,&Add_HighLi[i+1]);
		pFunction->GetLowLimit(i,&Add_LowLi[i+1]);//item 1-48
	}
	//************Power_on**********
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);	
	delayms(1);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	//i2c_init(HCTL_MODE,300000,2);
	//delayms(2);
	Reg[0x10f2].write(0xc8); //Put it here to avoid reload
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);

	Reg[0x1012].write(0xff);
	Reg[0x2007].write(_0x00);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	
	Additional_Settings();
	Fvi45.CH[VBYP]		.fv(0);
	//*****VFB5 ref to AGND EXTPG*****
	TestNO_target=2;
	Reg[0x1019].write(0x08);
	Reg[0x101a].write(0x80);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	Fvi16.CH[EXT_PG].fv(0,SITE_ALL,Stall3V3,Stall50mA);
	Reg[0x101d].write(0x0b);
	Reg[0x101e].write(0x0c);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	Reg[0x1081].write(_0x00);
	Reg[0x1082].write(_0x00);
	Reg[0x1084].write(0x80);
	Reg[0x1085].write(0x05);
	
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V
	Fvi45.CH[FB5].fv(1.6);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		EXTPG_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];

	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		EXTPG_HYS_FB5_1P6[nSiteIndex]=1e3*(EXTPG_VIH_FB5_1P6[nSiteIndex]-EXTPG_VIL_FB5_1P6[nSiteIndex]);
	}

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, EXTPG_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, EXTPG_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, EXTPG_HYS_FB5_1P6[nSiteIndex]);
	}
	//FB5=2.0V
	Fvi45.CH[FB5].fv(2.0);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		EXTPG_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTPG_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		EXTPG_HYS_FB5_2P0[nSiteIndex]=1e3*(EXTPG_VIH_FB5_2P0[nSiteIndex]-EXTPG_VIL_FB5_2P0[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, EXTPG_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, EXTPG_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, EXTPG_HYS_FB5_2P0[nSiteIndex]);
	}
	//*****VOUTSW input to EXT_PG*****		
	TestNO_target=131;
	Reg[0x10f5].write(0xff);		
	Reg[0x1019].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_EXTPG_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_EXTPG_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_EXTPG_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_EXTPG_VIH_3P64[nSiteIndex]-VOUTSW_EXTPG_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_EXTPG_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_EXTPG_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_EXTPG_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_EXTPG_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_PG,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_EXTPG_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_EXTPG_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_EXTPG_VIH_2P65[nSiteIndex]-VOUTSW_EXTPG_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_EXTPG_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_EXTPG_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_EXTPG_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x1019].write(0x0f);
	
	//*****VFB5 ref to AGND EXT_EN*****
	TestNO_target=8;
	Reg[0x101d].write(0x08);
	Reg[0x101e].write(0x80);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	Fvi16.CH[EXT_EN].fv(0,SITE_ALL,Stall3V3,Stall50mA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x0b);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	//FB5=1.6V	
	Fvi45.CH[FB5].fv(1.6);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		EXTEN_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		EXTEN_HYS_FB5_1P6[nSiteIndex]=1e3*(EXTEN_VIH_FB5_1P6[nSiteIndex]-EXTEN_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, EXTEN_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, EXTEN_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, EXTEN_HYS_FB5_1P6[nSiteIndex]);
	}
	//FB5=2.0V	
	Fvi45.CH[FB5].fv(2.0);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		EXTEN_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		EXTEN_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		EXTEN_HYS_FB5_2P0[nSiteIndex]=1e3*(EXTEN_VIH_FB5_2P0[nSiteIndex]-EXTEN_VIL_FB5_2P0[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, EXTEN_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, EXTEN_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, EXTEN_HYS_FB5_2P0[nSiteIndex]);
	}

	//*****VOUTSW input to EXT_EN*****		
	TestNO_target=137;
	Reg[0x10f5].write(0xff);		
	Reg[0x101d].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_EXTEN_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_EXTEN_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_EXTEN_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_EXTEN_VIH_3P64[nSiteIndex]-VOUTSW_EXTEN_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_EXTEN_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_EXTEN_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_EXTEN_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_EXTEN_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,EXT_EN,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_EXTEN_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_EXTEN_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_EXTEN_VIH_2P65[nSiteIndex]-VOUTSW_EXTEN_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_EXTEN_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_EXTEN_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_EXTEN_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x101d].write(0x0f);

	//Reset EXT_EN setup 
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	Fvi16.CH[EXT_EN].fi(0);
	Fvi16.CH[EXT_EN].open();

	//*****VFB5 ref to AGND GPIO_0*****
	TestNO_target=14;
	Reg[0x1021].write(0x08);
	Reg[0x1022].write(0x80);
	Reg[0x1023].write(_0x00);
	Reg[0x1024].write(0x08);
	Fvi16.CH[GPIO0].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x0e);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V	
	Fvi45.CH[FB5].fv(1.6);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		GPIO0_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);

	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		GPIO0_HYS_FB5_1P6[nSiteIndex]=1e3*(GPIO0_VIH_FB5_1P6[nSiteIndex]-GPIO0_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, GPIO0_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, GPIO0_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, GPIO0_HYS_FB5_1P6[nSiteIndex]);
	}	
	//FB5=2.0V	
	Fvi45.CH[FB5].fv(2.0);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[11],Add_HighLi[11],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		GPIO0_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[12],Add_LowLi[12],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO0_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		GPIO0_HYS_FB5_2P0[nSiteIndex]=1e3*(GPIO0_VIH_FB5_2P0[nSiteIndex]-GPIO0_VIL_FB5_2P0[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, GPIO0_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, GPIO0_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, GPIO0_HYS_FB5_2P0[nSiteIndex]);
	}
	
	//*****VOUTSW input to GPIO0*****		
	TestNO_target=143;
	Reg[0x10f5].write(0xff);		
	Reg[0x1021].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_GPIO0_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_GPIO0_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_GPIO0_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_GPIO0_VIH_3P64[nSiteIndex]-VOUTSW_GPIO0_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_GPIO0_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_GPIO0_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_GPIO0_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_GPIO0_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_GPIO0_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_GPIO0_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_GPIO0_VIH_2P65[nSiteIndex]-VOUTSW_GPIO0_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_GPIO0_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_GPIO0_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_GPIO0_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x1021].write(0x0f);



	//Reset GPIO0 setup 
	Reg[0x1021].write(0x0f);
	Reg[0x1022].write(_0x00);
	Reg[0x1023].write(_0x00);
	Reg[0x1024].write(0x08);
	Fvi16.CH[GPIO0].fi(0);
	Fvi16.CH[GPIO0].open();

	//*****VFB5 ref to AGND GPIO_1*****
	TestNO_target=20;
	Reg[0x1025].write(0x08);
	Reg[0x1026].write(0x80);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x08);
	Fvi16.CH[GPIO1].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x0f);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V	

	Fvi45.CH[FB5].fv(1.6);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		GPIO1_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		GPIO1_HYS_FB5_1P6[nSiteIndex]=1e3*(GPIO1_VIH_FB5_1P6[nSiteIndex]-GPIO1_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, GPIO1_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, GPIO1_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, GPIO1_HYS_FB5_1P6[nSiteIndex]);
	}	
	//FB5=2.0V	
	Fvi45.CH[FB5].fv(2.0);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		GPIO1_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		GPIO1_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		GPIO1_HYS_FB5_2P0[nSiteIndex]=1e3*(GPIO1_VIH_FB5_2P0[nSiteIndex]-GPIO1_VIL_FB5_2P0[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, GPIO1_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, GPIO1_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, GPIO1_HYS_FB5_2P0[nSiteIndex]);
	}


		//*****VOUTSW input to GPIO1*****		
	TestNO_target=149;
	Reg[0x10f5].write(0xff);		
	Reg[0x1025].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_GPIO1_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_GPIO1_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_GPIO1_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_GPIO1_VIH_3P64[nSiteIndex]-VOUTSW_GPIO1_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_GPIO1_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_GPIO1_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_GPIO1_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_GPIO1_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,GPIO1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_GPIO1_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_GPIO1_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_GPIO1_VIH_2P65[nSiteIndex]-VOUTSW_GPIO1_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_GPIO1_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_GPIO1_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_GPIO1_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x1025].write(0x0f);
	//Reset GPIO1 setup 
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x08);
	Fvi16.CH[GPIO1].fi(0);
	Fvi16.CH[GPIO1].open();

	//*****VFB5 ref to AGND PWRDN_0*****
	TestNO_target=26;

	Reg[0x1029].write(0x08);
	Reg[0x102a].write(0x80);
	Reg[0x102b].write(_0x00);
	Reg[0x102c].write(0x08);
	Fvi16.CH[PWRDN0].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x09);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V	

	Fvi45.CH[FB5].fv(1.6);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		PWRDN0_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN0_HYS_FB5_1P6[nSiteIndex]=1e3*(PWRDN0_VIH_FB5_1P6[nSiteIndex]-PWRDN0_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, PWRDN0_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, PWRDN0_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, PWRDN0_HYS_FB5_1P6[nSiteIndex]);
	}	
	//FB5=2.0V	
	
	Fvi45.CH[FB5].fv(2.0);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		PWRDN0_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN0_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN0_HYS_FB5_2P0[nSiteIndex]=1e3*(PWRDN0_VIH_FB5_2P0[nSiteIndex]-PWRDN0_VIL_FB5_2P0[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, PWRDN0_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, PWRDN0_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, PWRDN0_HYS_FB5_2P0[nSiteIndex]);
	}

		//*****VOUTSW input to PWRDN0*****		
	TestNO_target=155;
	Reg[0x10f5].write(0xff);		
	Reg[0x1029].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_PWRDN0_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_PWRDN0_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_PWRDN0_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_PWRDN0_VIH_3P64[nSiteIndex]-VOUTSW_PWRDN0_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_PWRDN0_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_PWRDN0_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_PWRDN0_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_PWRDN0_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN0,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_PWRDN0_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_PWRDN0_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_PWRDN0_VIH_2P65[nSiteIndex]-VOUTSW_PWRDN0_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_PWRDN0_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_PWRDN0_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_PWRDN0_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x1029].write(0x0f);
	//Reset PWRDN_0 setup 
	Reg[0x1029].write(0x0f);
	Reg[0x102a].write(_0x00);
	Reg[0x102b].write(_0x00);
	Reg[0x102c].write(0x08);
	Fvi16.CH[PWRDN0].fi(0);
	Fvi16.CH[PWRDN0].open();

	//*****VFB5 ref to AGND PWRDN_1*****
	TestNO_target=32;

	Reg[0x102d].write(0x08);
	Reg[0x102e].write(0x80);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x08);
	Fvi16.CH[PWRDN1].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x0a);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V	
	Fvi45.CH[FB5].fv(1.6);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		PWRDN1_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN1_HYS_FB5_1P6[nSiteIndex]=1e3*(PWRDN1_VIH_FB5_1P6[nSiteIndex]-PWRDN1_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, PWRDN1_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, PWRDN1_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, PWRDN1_HYS_FB5_1P6[nSiteIndex]);
	}	
	//FB5=2.0V	
	
	Fvi45.CH[FB5].fv(2.0);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		PWRDN1_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		PWRDN1_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		PWRDN1_HYS_FB5_2P0[nSiteIndex]=1e3*(PWRDN1_VIH_FB5_2P0[nSiteIndex]-PWRDN1_VIL_FB5_2P0[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, PWRDN1_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, PWRDN1_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, PWRDN1_HYS_FB5_2P0[nSiteIndex]);
	}
			//*****VOUTSW input to PWRDN1*****		
	TestNO_target=161;
	Reg[0x10f5].write(0xff);		
	Reg[0x1029].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_PWRDN1_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_PWRDN1_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_PWRDN1_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_PWRDN1_VIH_3P64[nSiteIndex]-VOUTSW_PWRDN1_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_PWRDN1_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_PWRDN1_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_PWRDN1_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_PWRDN1_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,PWRDN1,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_PWRDN1_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_PWRDN1_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_PWRDN1_VIH_2P65[nSiteIndex]-VOUTSW_PWRDN1_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_PWRDN1_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_PWRDN1_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_PWRDN1_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x102d].write(0x0f);
	//Reset PWRDN_1 setup 
	Reg[0x102d].write(0x0f);
	Reg[0x102e].write(_0x00);
	Reg[0x102f].write(_0x00);
	Reg[0x1030].write(0x08);
	Fvi16.CH[PWRDN1].fi(0);
	Fvi16.CH[PWRDN1].open();
	//*****VFB5 ref to AGND LPM*****
	TestNO_target=38;

	Reg[0x1031].write(0x08);
	Reg[0x1032].write(0x80);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x08);
	Fvi16.CH[LPM].fv(0,SITE_ALL,Stall3V3,Stall50uA);
	Reg[0x1019].write(0x0b);
	Reg[0x101a].write(0x08);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Fvi16.CH[EXT_PG].fi(0,SITE_ALL,Stall3V3,Stall50uA);
	//FB5=1.6V	
	Fvi45.CH[FB5].fv(1.6);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)	
		LPM_VIH_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_VIL_FB5_1P6[nSiteIndex]=global_temp[nSiteIndex];
		LPM_HYS_FB5_1P6[nSiteIndex]=1e3*(LPM_VIH_FB5_1P6[nSiteIndex]-LPM_VIL_FB5_1P6[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_VIH_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_VIL_FB5_1P6[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_HYS_FB5_1P6[nSiteIndex]);
	}	
	//FB5=2.0V	
	Fvi45.CH[FB5].fv(2.0);
	delayms(2);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		LPM_VIH_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		LPM_VIL_FB5_2P0[nSiteIndex]=global_temp[nSiteIndex];
		LPM_HYS_FB5_2P0[nSiteIndex]=1e3*(LPM_VIH_FB5_2P0[nSiteIndex]-LPM_VIL_FB5_2P0[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, LPM_VIH_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, LPM_VIL_FB5_2P0[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, LPM_HYS_FB5_2P0[nSiteIndex]);
	}


	//*****VOUTSW input to LPM*****		
	TestNO_target=167;
	Reg[0x10f5].write(0xff);		
	Reg[0x1031].write(0x0c);	
	Reg[0x1019].read(readback);
	Fvi45.CH[VOUT_SW]		.fv(3.64);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_LPM_VIH_3P64[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+1],Add_LowLi[TestNO_target+1],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_LPM_VIL_3P64[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_LPM_HYS_3P64[nSiteIndex]=1e3*(VOUTSW_LPM_VIH_3P64[nSiteIndex]-VOUTSW_LPM_VIL_3P64[nSiteIndex]);
	}
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+0, VOUTSW_LPM_VIH_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VOUTSW_LPM_VIL_3P64[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VOUTSW_LPM_HYS_3P64[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(2.65);
	delayms(1);
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_LowLi[TestNO_target+3],Add_HighLi[TestNO_target+3],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
		VOUTSW_LPM_VIH_2P65[nSiteIndex]=global_temp[nSiteIndex];
	
	sweep_VI(RESOURCE_FVI16,LPM,VOLTAGE,RESOURCE_FVI16,EXT_PG,VOLTAGE,
		Add_HighLi[TestNO_target+4],Add_LowLi[TestNO_target+4],-0.002,LESS_THAN,0.2,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		VOUTSW_LPM_VIL_2P65[nSiteIndex]=global_temp[nSiteIndex];
		VOUTSW_LPM_HYS_2P65[nSiteIndex]=1e3*(VOUTSW_LPM_VIH_2P65[nSiteIndex]-VOUTSW_LPM_VIL_2P65[nSiteIndex]);
	}
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, VOUTSW_LPM_VIH_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, VOUTSW_LPM_VIL_2P65[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, VOUTSW_LPM_HYS_2P65[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	delayms(1);
	Reg[0x10f5].write_CMI();		
	Reg[0x1031].write(0x0f);



	Fvi45.CH[FB5].fv(0);
	Fvi45.CH[FB5].open();

	//Reset LPM setup 
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x08);
	//Reset EXT_PG setup 
	Reg[0x1019].write(0x0f);
	Reg[0x101a].write(_0x00);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	//Reset B5
	Reg[0x1081].write_CMI();
	Reg[0x1082].write_CMI();
	Reg[0x1085].write(_0x00);

	Fvi16.CH[LPM].fi(0);
	Fvi16.CH[LPM].open();

	//*****FBB5_AUX_POK threshold*****
	TestNO_target=44;

	double Aux_Pok_R[SITE_MAX]={0}, Aux_Pok_F[SITE_MAX]={0}, Aux_Pok_Hys[SITE_MAX]={0};
	Reg[0x1019].write(0x08);
	Reg[0x101a].write(0x80);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	Fvi16.CH[EXT_PG].fv(2,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(0x0c);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	
	Reg[0x1084].write(0x88);
	
	sweep_VI(RESOURCE_FVI45,FB5,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_LowLi[TestNO_target],Add_HighLi[TestNO_target],0.002,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
	delayms(2);
	FOR_EACH_SITE(nSiteIndex)
	{
		Aux_Pok_R[nSiteIndex]=global_temp[nSiteIndex];
		pFunction->LogResult(NSITEINDEX, TestNO_target, Aux_Pok_R[nSiteIndex]);
	}
	
	sweep_VI(RESOURCE_FVI45,FB5,VOLTAGE,RESOURCE_FVI16,EXT_EN,VOLTAGE,
		Add_HighLi[TestNO_target]-1e-3*Add_LowLi[TestNO_target+1],Add_LowLi[TestNO_target]-1e-3*Add_HighLi[TestNO_target+1],-0.002,LESS_THAN,1.5,global_temp,20,GNG_Flag);
	FOR_EACH_SITE(nSiteIndex)
	{
		Aux_Pok_F[nSiteIndex]=global_temp[nSiteIndex];
		Aux_Pok_Hys[nSiteIndex]=1e3*(Aux_Pok_R[nSiteIndex]-Aux_Pok_F[nSiteIndex]);	
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, Aux_Pok_Hys[nSiteIndex]);
	}
	
	Reg[0x1019].write(0x0f);
	Reg[0x101a].write(_0x00);
	Reg[0x101b].write(_0x00);
	Reg[0x101c].write(0x08);
	
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	
	Reg[0x1084].write(0x80);
	Fvi16.CH[EXT_PG].fi(0);
	Fvi16.CH[EXT_PG].open();	
	Fvi16.CH[EXT_EN].fi(0);
	Fvi16.CH[EXT_EN].open();	
	//*****Active mode supply current VIN*****

	double I_VOUTSW[SITE_MAX]={0},I_VIN[SITE_MAX]={0},I_VINB[PMU_MAX][SITE_MAX]={0},I_VINLDO[LDO_MAX][SITE_MAX]={0},I_VOUTB[PMU_MAX][SITE_MAX]={0},I_Active1[SITE_MAX]={0},I_Active2[SITE_MAX]={0};
	double I_LPM1[SITE_MAX]={0}, I_LPM2[SITE_MAX]={0}, I_LPM3[SITE_MAX]={0};
	

	Relay.BIT[K14_OUTLDO_Cap].set();
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX)
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(3.3,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	Fvi16.CH[VIN_LDO1].fv(3.3);
	Fvi16.CH[VIN_LDO2].fv(3.3);
	
	FOR_EACH_TILE(nTILE,PMU_MAX)
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	delayms(2);


	Reg[0x1016].read(readback);
	Reg[0x1043].set_clr_inv(BITNONE,BIT0);
	Reg[0x1053].set_clr_inv(BITNONE,BIT0);
	Reg[0x1063].set_clr_inv(BITNONE,BIT0);
	Reg[0x1073].set_clr_inv(BITNONE,BIT0);
	Reg[0x1083].set_clr_inv(BITNONE,BIT0);
	Reg[0x1093].write(0x88);
	Reg[0x1085].set_clr_inv(BIT2);
	
	
	delayms(2);
	Reg[0x1036].write(_0x00);
	Reg[0x1012].write(_0x00);
	delayms(100);
	//Reg[0x1012].write(0x1f);
	//Reg[0x1016].write(0xa0);
	SDASCL_Connect_VI();
	delayms(50);
	Fvi45.CH[GP_VIN_Buck[nTILE]].fv(3.3,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(2);
	//VIN3V3 Input
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	Fvi45.CH[VIN_3V3]		.mi(I_VIN);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.mi(I_VOUTB[nTILE]);
	}

	FOR_EACH_TILE(nTILE,LDO_MAX)
		Fvi16.CH[GP_INLDO[nTILE]]		.mi(I_VINLDO[nTILE]);


	FOR_EACH_SITE(nSiteIndex)
	{
		I_Active1[nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[0][nSiteIndex]+I_VINB[2][nSiteIndex]+I_VINB[3][nSiteIndex]+I_VINB[4][nSiteIndex]
		+I_VINLDO[0][nSiteIndex]+I_VINLDO[1][nSiteIndex];
		pFunction->LogResult(NSITEINDEX, 46, 1e3*I_Active1[nSiteIndex]);
	}
	printf("Ioutsw = %3.3f\n",I_VOUTSW[0]);
	printf("Ivin = %3.3f\n",I_VIN[0]);


	//VOUTSW Input
	SDASCL_Connect_I2C();
	Reg[0x10f2].write(0xc0);
	Reg[0x10f2].read(readback);
	delayms(10);
	
	SDASCL_Connect_VI();
	delayms(30);
	
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	Fvi45.CH[VIN_3V3]		.mi(I_VIN);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.mi(I_VOUTB[nTILE]);
	}
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
		Fvi16.CH[GP_INLDO[nTILE]]		.mi(I_VINLDO[nTILE]);
	
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_Active2[nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[0][nSiteIndex]+I_VINB[2][nSiteIndex]+I_VINB[3][nSiteIndex]+I_VINB[4][nSiteIndex]
			+I_VINLDO[0][nSiteIndex]+I_VINLDO[1][nSiteIndex];
		pFunction->LogResult(NSITEINDEX, 47, 1e3*I_Active2[nSiteIndex]);
	}


	SDASCL_Connect_I2C();
	//Reg[0x10f2].write_CMI();
	Reg[0x1016].read(readback);


	


	
	//*****DPS3, LPM1 Supply Current*****
	Fvi45.CH[VBYP]      .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);	
	delayms(2);
	Reg[0x1016].read(readback);
	Reg[0x1012].read(readback);
	//Reg[0x1012].write(0x01);
	Reg[0x1043].set_clr_inv(BIT0);
	Reg[0x1053].set_clr_inv(BIT0);
	Reg[0x1063].set_clr_inv(BIT0);
	Reg[0x1073].set_clr_inv(BIT0);
	Reg[0x1083].set_clr_inv(BIT0);
	Reg[0x1093].write(0xdd);


	Reg[0x1043].set_clr_inv(BITNONE,BIT0);
	Reg[0x1053].set_clr_inv(BITNONE,BIT0);
	Reg[0x1063].set_clr_inv(BITNONE,BIT0);
	Reg[0x1073].set_clr_inv(BITNONE,BIT0);
	Reg[0x1083].set_clr_inv(BITNONE,BIT0);
	Reg[0x1093].write(0x44);

	Reg[0x2040].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
	Reg[0x2040].write(readback);
	Reg[0x2050].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
	Reg[0x2050].write(readback);
	Reg[0x2060].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
	Reg[0x2060].write(readback);
	Reg[0x2070].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
	Reg[0x2070].write(readback);
	Reg[0x2080].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|(0x3f&readback[nSiteIndex]);
	Reg[0x2080].write(readback);
	Reg[0x100a].write(0xee);
	Reg[0x100b].write(0x28);
	Reg[0x100c].write(_0x00);
	Reg[0x100a].read(readback);
	Reg[0x2007].read(readback);
	Fvi45.CH[VBYP].fv(0);
	Reg[0x2007].read(readback);



	Reg[0x1093].write(0x99);
	Reg[0x1006].write(0x05);
	Reg[0x100a].read(readback);
	Reg[0x1040].read(readback);
	Reg[0x1050].read(readback);
	Reg[0x1060].read(readback);
	Reg[0x1070].read(readback);
	Reg[0x1080].read(readback);
	Reg[0x1090].read(readback);
	//Fvi45.CH[FB4].fv(0);

	Reg[0x1093].read(readback);
	//Reg[0x1036].write(0x7f);
	//Reg[0x1012].write(_0x00);

	SDASCL_Connect_VI();
	delayms(20);
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	Fvi45.CH[VIN_3V3]		.mi(I_VIN);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.mi(I_VOUTB[nTILE]);
	}
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
		Fvi16.CH[GP_INLDO[nTILE]]		.mi(I_VINLDO[nTILE]);
	
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LPM1[nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[0][nSiteIndex]+I_VINB[2][nSiteIndex]+I_VINB[3][nSiteIndex]+I_VINB[4][nSiteIndex]
			+I_VINLDO[0][nSiteIndex]+I_VINLDO[1][nSiteIndex];
		pFunction->LogResult(NSITEINDEX, 48, 1e3*I_LPM1[nSiteIndex]);
	}
	
	
	//*****DPS4, LPM2 Supply Current*****
	
	SDASCL_Connect_I2C();
	Reg[0x1006].write(0x04);
	delayms(2);
	Reg[0x1006].write(0x06);
	Reg[0x100a].read(readback);
	Reg[0x100b].read(readback);
	Reg[0x1040].read(readback);
	Reg[0x1050].read(readback);
	Reg[0x1060].read(readback);
	Reg[0x1070].read(readback);
	Reg[0x1080].read(readback);
	Reg[0x1090].read(readback);
	

	Reg[0x1093].read(readback);
	
	SDASCL_Connect_VI();
	delayms(20);
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	Fvi45.CH[VIN_3V3]		.mi(I_VIN);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.mi(I_VOUTB[nTILE]);
	}
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
		Fvi16.CH[GP_INLDO[nTILE]]		.mi(I_VINLDO[nTILE]);
	
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LPM2[nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[0][nSiteIndex]+I_VINB[2][nSiteIndex]+I_VINB[3][nSiteIndex]+I_VINB[4][nSiteIndex]
			+I_VINLDO[0][nSiteIndex]+I_VINLDO[1][nSiteIndex];
		pFunction->LogResult(NSITEINDEX, 49, 1e3*I_LPM2[nSiteIndex]);
	}

	//*****LPM3 Supply Current*****

	SDASCL_Connect_I2C();
	Reg[0x1006].write(0x04);
	delayms(2);
	Reg[0x1006].write(0x07);
	Reg[0x1006].read(readback);
	Reg[0x100a].read(readback);
	Reg[0x100b].read(readback);
	Reg[0x100c].read(readback);
	Reg[0x1040].read(readback);
	Reg[0x1050].read(readback);
	Reg[0x1060].read(readback);
	Reg[0x1070].read(readback);
	Reg[0x1080].read(readback);
	Reg[0x1090].read(readback);


	Reg[0x1093].read(readback);

	
	SDASCL_Connect_VI();
	delayms(20);
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	Fvi45.CH[VIN_3V3]		.mi(I_VIN);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]]			.mi(I_VOUTB[nTILE]);
	}
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
		Fvi16.CH[GP_INLDO[nTILE]]		.mi(I_VINLDO[nTILE]);
	
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LPM3[nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[0][nSiteIndex]+I_VINB[2][nSiteIndex]+I_VINB[3][nSiteIndex]+I_VINB[4][nSiteIndex]
			+I_VINLDO[0][nSiteIndex]+I_VINLDO[1][nSiteIndex];
		pFunction->LogResult(NSITEINDEX, 50, 1e3*I_LPM3[nSiteIndex]);
	}
	

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fvi45.CH[GP_INLDO[nTILE]].fv(0);
		Fvi45.CH[GP_INLDO[nTILE]].open();
	}
	
	Relay.BIT[K14_OUTLDO_Cap].clr();
	

	
	SDASCL_Connect_I2C();
	Fvi45.CH[VBYP]      .fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);	
	Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	Fvi45.CH[VIN_3V3]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	

	//*****Bucks Bypass mode*****
	double VoutB_3p3[PMU_MAX][SITE_MAX]={0},VoutB_2p4[PMU_MAX][SITE_MAX]={0};
	
	//************Power_on**********
	Enter_Factory(true);
	delayms(1);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(_0x00);
	Additional_Settings();
	Fvi45.CH[VBYP]		.fv(0);
	delayms(2);
		
	Reg[0x2047].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
	Reg[0x2047].write(readback);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(nTILE==4)
		{		
			Relay.BIT[K41_FQVI5].set();
		}
		delayms(1);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(3.3,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(1);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x01|readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x05);

		Fqvi.CH[GP_SW_Buck[nTILE]].fi(-10,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mv(VoutB_3p3[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, 51+2*nTILE, VoutB_3p3[nTILE][nSiteIndex]);
		
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(2.4,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].mv(VoutB_2p4[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
			pFunction->LogResult(NSITEINDEX, 52+2*nTILE, VoutB_2p4[nTILE][nSiteIndex]);

		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(_0x00);
		if(nTILE==4)
		{		
			Relay.BIT[K41_FQVI5].clr();
		}
		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Fqvi.CH[GP_SW_Buck[nTILE]].open();
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}	
	//*****Bucks ILIM in Bypass mode*****
	double ILIM_BP[PMU_MAX][SITE_MAX]={0},ILIMWARN_BP[PMU_MAX][SITE_MAX]={0},ILIMSHUT_BP[PMU_MAX][SITE_MAX]={0};
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);
	Reg[0x2008].set_clr_inv(BIT2);
	Fvi16.CH[EXT_EN].fi(0,SITE_ALL,Stall3V3,Stall50uA);

	if(0)
	{
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{
			Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
			Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x05);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x40);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x01);
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write(0x20);
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].set();
			if(nTILE==4)
			{		
				Relay.BIT[K41_FQVI5].set();
			}
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0,SITE_ALL,SET_CLAMP,2,-2,1500,-1500);
			
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				-(1+1e-2*Add_LowLi[62+4*nTILE])*Buck_HSILIM_Target[nTILE],-(1+1e-2*Add_HighLi[62+4*nTILE])*Buck_HSILIM_Target[nTILE],-20,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{	
				ILIM_BP[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				pFunction->LogResult(NSITEINDEX, 61+4*nTILE, Buck_HSILIM_Target[nTILE]);
				pFunction->LogResult(NSITEINDEX, 62+4*nTILE, 1e2*ILIM_BP[nTILE][nSiteIndex]/Buck_HSILIM_Target[nTILE]-100);
			}
			
			//ILIM WARN Bypass
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x02);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				-1e-2*Add_LowLi[63+4*nTILE]*Buck_HSILIM_Target[nTILE],-1e-2*Add_HighLi[63+4*nTILE]*Buck_HSILIM_Target[nTILE],-20,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{	
				ILIMWARN_BP[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				//pFunction->LogResult(NSITEINDEX, 63+4*nTILE, 1e2*ILIMWARN_BP[nTILE][nSiteIndex]/Buck_HSILIM_Target[nTILE]);
			}
			
			//ILIM SHUT Bypass
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x03);
			sweep_VI(RESOURCE_FQVI,GP_SW_Buck[nTILE],CURRENT,RESOURCE_FVI16,EXT_EN,VOLTAGE,
				-1e-2*Add_LowLi[64+4*nTILE]*Buck_HSILIM_Target[nTILE],-1e-2*Add_HighLi[64+4*nTILE]*Buck_HSILIM_Target[nTILE],-20,GREATER_THAN,1.5,global_temp,20,GNG_Flag);
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			delayms(1);
			FOR_EACH_SITE(nSiteIndex)
			{	
				ILIMSHUT_BP[nTILE][nSiteIndex]=-global_temp[nSiteIndex];
				//pFunction->LogResult(NSITEINDEX, 64+4*nTILE, 1e2*ILIMSHUT_BP[nTILE][nSiteIndex]/Buck_HSILIM_Target[nTILE]);
			}
			Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
			Fqvi.CH[GP_SW_Buck[nTILE]].open();
			Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
			Fvi45.CH[GP_VIN_Buck[nTILE]].open();
			Relay.BIT[GP_Buck_SW_Relay[nTILE]].clr();
			if(nTILE==4)
			{		
				Relay.BIT[K41_FQVI5].clr();
			}
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write_CMI();
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write_CMI();
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0e].write_CMI();
			Reg[GP_User_Buck_Addr[nTILE]+0x05].write(_0x00);
		}
	}

	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x08);
	
	Fvi16.CH[EXT_EN].open();
	
	//*****Bucks IQQ Shutdown in Bypass mode*****
	double Iqq_Buck[PMU_MAX][SITE_MAX]={0}, Iqq_BuckShut[PMU_MAX][SITE_MAX]={0};
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);

	SDASCL_Connect_VI();
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(10);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
		Fvi45.CH[VIN_3V3]		.mi(I_VIN);
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Iqq_BuckShut[nTILE][nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, 81+nTILE, 1e3*Iqq_BuckShut[nTILE][nSiteIndex]);
		}	
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}

	Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(10);
		SDASCL_Connect_I2C();
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x05);
		SDASCL_Connect_VI();
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(10);
		Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
		Fvi45.CH[VIN_3V3]		.mi(I_VIN);
		Fvi45.CH[GP_VIN_Buck[nTILE]]		.mi(I_VINB[nTILE]);
		FOR_EACH_SITE(nSiteIndex)
		{
			Iqq_Buck[nTILE][nSiteIndex]=I_VOUTSW[nSiteIndex]+I_VIN[nSiteIndex]+I_VINB[nTILE][nSiteIndex];
			Iqq_Buck[nTILE][nSiteIndex]=Iqq_Buck[nTILE][nSiteIndex]-Iqq_BuckShut[nTILE][nSiteIndex];
			pFunction->LogResult(NSITEINDEX, 86+nTILE, 1e3*Iqq_Buck[nTILE][nSiteIndex]);
		}
		SDASCL_Connect_I2C();
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(_0x00);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}	
	Reg[0x2047].write_CMI();	
	DISP=1;
	starttest(DISP);
	Fvi45.CH[VOUT_SW]		.mi(I_VOUTSW);
	savetesttime("Test",DISP);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfe&readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
	}
	
	
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(PMU_ACOT_MODE[nTILE])
		{
			FOR_EACH_SITE(nSiteIndex)
			{	
				for(i=0;i<4;i++)
				{
					pFunction->LogResult(NSITEINDEX, 91+nTILE*8+2*i, Buck_FSWO[i][nTILE][nSiteIndex]);
					pFunction->LogResult(NSITEINDEX, 92+nTILE*8+2*i, Buck_FSWO_Acc[i][nTILE][nSiteIndex]);

				}			
			}
		}	
	}

	
	/*
	//Power off
	Fvi45.CH[VBYP]      .fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);	
	Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	Fvi45.CH[VIN_3V3]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	delayms(2);
	*/
	
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: INRUSH
STT_TESTFUNC INRUSH(CFunction *pFunction, int nSiteNo, int nTestFlag, INRUSH_params* ours)
{
	double INRUSH_FSW1[8][SITE_MAX]={0},INRUSH_FSW1_IGATE[8][SITE_MAX]={0},INRUSH_FSW2[2][SITE_MAX]={0},INRUSH_FSW2_IGATE[2][SITE_MAX]={0};

	//************Power_on**********
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);	
	delayms(1);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	i2c_init(HCTL_MODE,300000,2);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
	Disable_Sequence();

    Factory_Mode_Check();
    FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	
	//************INRUSH_FSW1**********
	Relay.BIT[K47_TMU].set(); 
	Relay.BIT[K21_GPIO1_TMUA].set(); 
	Fvi16.CH[GATE]		.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	delayms(1);
	Fvi16.CH[GATE]		.fv(VIN_SET,SITE_ALL,Stall3V3,Stall50uA);

	TMU_CH1.stallset(Stall7V,TMUCOUNT,1);
	TMU_CH1.switchset(STARTtoA+STOPtoA);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	delayms(2);
	
	Reg[0x10f2].write(0x08);
	Reg[0x20f9].write(0x06);
	Reg[0x20fa].write(0xa0);
	Reg[0x2008].write(0x05);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);//Configure GPIO1
	for(i=0;i<8;i++)
	{
		Reg[0x10f3].write(0x04|(i<<5));
		delayms(1);
		Fvi16.CH[GATE]		.mi(INRUSH_FSW1_IGATE[i]);
		TMU_CH1.read();
		FOR_EACH_SITE(nSiteIndex)
		{
			INRUSH_FSW1[i][nSiteIndex]=meas_data[nSiteIndex+1];	
			pFunction->LogResult(NSITEINDEX, 2+i*2, INRUSH_FSW1[i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 3+i*2, -1e3*INRUSH_FSW1_IGATE[i][nSiteIndex]);
		}
	}
	//************INRUSH_FSW1**********
	//************INRUSH_FSW2**********
	Reg[0x20f9].write(0x01);
	//Set0
	Reg[0x10f3].set_clr_inv(BITNONE,BIT4|BIT3|BIT2|BIT1|BIT0);
	delayms(1);
	Fvi16.CH[GATE]		.mi(INRUSH_FSW2_IGATE[0]);

	TMU_CH1.read();
	FOR_EACH_SITE(nSiteIndex)
	{
		INRUSH_FSW2[0][nSiteIndex]=meas_data[nSiteIndex+1];	
		pFunction->LogResult(NSITEINDEX, 18, INRUSH_FSW2[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 19, -1e3*INRUSH_FSW2_IGATE[0][nSiteIndex]);

	}
	//Set1
	Reg[0x10f3].set_clr_inv(BIT4,BIT3|BIT2|BIT1|BIT0);
	delayms(1);
	Fvi16.CH[GATE]		.mi(INRUSH_FSW2_IGATE[1]);

	TMU_CH1.read();
	FOR_EACH_SITE(nSiteIndex)
	{
		INRUSH_FSW2[1][nSiteIndex]=meas_data[nSiteIndex+1];	
		pFunction->LogResult(NSITEINDEX, 20, INRUSH_FSW2[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 21, -1e3*INRUSH_FSW2_IGATE[1][nSiteIndex]);
	}
	//************INRUSH_FSW2**********
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi16.CH[GATE]		.fi(0);
	Fvi16.CH[GATE]		.open(0);
	Relay.BIT[K47_TMU].clr(); 
	Relay.BIT[K21_GPIO1_TMUA].clr(); 
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	

	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Vstress_Check
STT_TESTFUNC Vstress_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Vstress_Check_params* ours)
{
	double I_Buck[4][SITE_MAX]={0},I_VIN[2][SITE_MAX]={0};
	double I_HS_3P3_All[2][SITE_MAX]={0},I_HS_3P3_Buck[2][SITE_MAX]={0};
	double I_HS_5P28_All[SITE_MAX]={0},I_HS_5P28_Buck[SITE_MAX]={0};
	double I_HS_delta_All[SITE_MAX]={0},I_HS_delta_Buck[SITE_MAX]={0};
	double I_LS_3P3_All[2][SITE_MAX]={0},I_LS_3P3_Buck[2][SITE_MAX]={0};
	double I_LS_5P28_All[SITE_MAX]={0},I_LS_5P28_Buck[SITE_MAX]={0};
	double I_LS_delta_All[SITE_MAX]={0},I_LS_delta_Buck[SITE_MAX]={0};
	double Vstress_Voltage=1.6*VIN_SET;
	Disconnect_all_cap();
	Enter_Factory(true);	
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET, SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	}
	Isupply_check_setting();
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0xa0);
	Reg[0x2008].write(0x01);
	Reg[0x20fa].write(0x20);
	Fvi45.CH[VBYP]		.fv(0);
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Reg[GP_User_Buck_Addr[nTILE]+0x05].write(0x05);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xc0|readback[nSiteIndex];
		Reg[GP_Factory_Buck_Addr[nTILE]+0x00].write(readback);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x50);
	}
	SDASCL_Connect_VI();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	Fvi45.CH[VIN_B12]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	Fvi45.CH[VIN_B3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	Fvi45.CH[VIN_B4]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	Fvi45.CH[VIN_B5]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);

	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_HS_3P3_All[0][nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_HS_3P3_Buck[0][nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
	}
	for(i=0;i<10;i++)
	{
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET+i*(Vstress_Voltage-VIN_SET)/10);
		delayms(1);
	}
	Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B12]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B3]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B4]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B5]	.fv(Vstress_Voltage);
	Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage,SITE_ALL,SET_CLAMP,7,-7,4,-4);

	delayms(100);
	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_HS_5P28_All[nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_HS_5P28_Buck[nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
	}
	for(i=0;i<10;i++)
	{
		Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage-i*(Vstress_Voltage-VIN_SET)/10);
	}
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET);
	Fvi45.CH[VIN_B12]	.fv(VIN_SET);
	Fvi45.CH[VIN_B3]	.fv(VIN_SET);
	Fvi45.CH[VIN_B4]	.fv(VIN_SET);
	Fvi45.CH[VIN_B5]	.fv(VIN_SET);
	delayms(10);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	delayms(10);
	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_HS_3P3_All[1][nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_HS_3P3_Buck[1][nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_HS_delta_All[nSiteIndex]=I_HS_3P3_All[1][nSiteIndex]-I_HS_3P3_All[0][nSiteIndex];
		I_HS_delta_Buck[nSiteIndex]=I_HS_3P3_Buck[1][nSiteIndex]-I_HS_3P3_Buck[0][nSiteIndex];
	}
	TestNO_target=2;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, I_HS_3P3_All[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, I_HS_3P3_Buck[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, I_HS_5P28_All[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, I_HS_5P28_Buck[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, I_HS_3P3_All[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, I_HS_3P3_Buck[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, I_HS_delta_All[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, I_HS_delta_Buck[nSiteIndex]);
	}
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	SDASCL_Connect_I2C();
	delayms(10);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x60);
	}
	SDASCL_Connect_VI();
	delayms(10);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	}
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	}
	delayms(10);
	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);		
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LS_3P3_All[0][nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_LS_3P3_Buck[0][nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
	}
/*
	for(i=0;i<50;i++)
	{
		delayms(10);
		Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
		trim_table_value[i]=1e3*I_VIN[0][0];
	}
	*/
	for(i=0;i<10;i++)
	{
		Fvi45.CH[VOUT_SW]	.fv(VIN_SET+i*(Vstress_Voltage-VIN_SET)/10);
		delayms(1);
	}
	Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B12]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B3]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B4]	.fv(Vstress_Voltage);
	Fvi45.CH[VIN_B5]	.fv(Vstress_Voltage);
	Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	delayms(100);
	

	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LS_5P28_All[nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_LS_5P28_Buck[nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
	}
	for(i=0;i<10;i++)
	{
		Fvi45.CH[VOUT_SW]	.fv(Vstress_Voltage-i*(Vstress_Voltage-VIN_SET)/10);
		delayms(1);
	}
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	Fvi45.CH[VIN_3V3]	.fv(VIN_SET);
	Fvi45.CH[VIN_B12]	.fv(VIN_SET);
	Fvi45.CH[VIN_B3]	.fv(VIN_SET);
	Fvi45.CH[VIN_B4]	.fv(VIN_SET);
	Fvi45.CH[VIN_B5]	.fv(VIN_SET);
	delayms(10);
	Fvi45.CH[VOUT_SW]	.fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	delayms(10);
	/*
	for(i=0;i<50;i++)
	{
		delayms(20);
		Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
		trim_table_value[i]=1e3*I_VIN[0][0];
	}
	*/
	Fvi45.CH[VOUT_SW].mi(I_VIN[0]);
	Fvi45.CH[VIN_3V3].mi(I_VIN[1]);
	Fvi45.CH[VIN_B12].mi(I_Buck[0]);
	Fvi45.CH[VIN_B3].mi(I_Buck[1]);
	Fvi45.CH[VIN_B4].mi(I_Buck[2]);
	Fvi45.CH[VIN_B5].mi(I_Buck[3]);
	FOR_EACH_SITE(nSiteIndex)
	{
		I_LS_3P3_All[1][nSiteIndex]=1e3*(I_VIN[0][nSiteIndex]+I_VIN[1][nSiteIndex]+I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_LS_3P3_Buck[1][nSiteIndex]=1e3*(I_Buck[0][nSiteIndex]+I_Buck[1][nSiteIndex]+I_Buck[2][nSiteIndex]+I_Buck[3][nSiteIndex]);
		I_LS_delta_All[nSiteIndex]=I_LS_3P3_All[1][nSiteIndex]-I_LS_3P3_All[0][nSiteIndex];
		I_LS_delta_Buck[nSiteIndex]=I_LS_3P3_Buck[1][nSiteIndex]-I_LS_3P3_Buck[0][nSiteIndex];
	}
	//Power off
	Fvi16.CH[SDA]	.open();
	Fvi16.CH[SCL]	.open();
	delayms(2);
	Relay.BIT[K48_I2C]		.set();
	delayms(3);
	SDASCL_Connect_I2C();
	delayms(2);
	Factory_Mode_Check();
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}
	Relay.BIT[K48_I2C]		.clr();
	delayms(2);
	Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-400);
	Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-400);
	delayms(2);
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]	.open();
	TestNO_target=10;
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target, I_LS_3P3_All[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, I_LS_3P3_Buck[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, I_LS_5P28_All[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, I_LS_5P28_Buck[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+4, I_LS_3P3_All[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+5, I_LS_3P3_Buck[1][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, I_LS_delta_All[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+7, I_LS_delta_Buck[nSiteIndex]);
		//pFunction->LogResult(NSITEINDEX, TestNO_target+8, Factory_Mode_Flag[nSiteIndex]);
	}	
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Power_Sequence_Check
STT_TESTFUNC Power_Sequence_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Power_Sequence_Check_params* ours)
{
	double Buck_Vout[PMU_MAX][SITE_MAX]={0};
	double Buck_Vout_status[PMU_MAX][SITE_MAX]={0};
	double LDO_Vout[LDO_MAX][SITE_MAX]={0};
	double LDO_Vout_status[LDO_MAX][SITE_MAX]={0};
	//RC relay
	//if(1&&(CMI_Rev_No==520103||CMI_Rev_No==520100||CMI_Rev_No==522100))//90522102.E5B cmi has cmi set issue that can not pass the power on check
	if(0)//90522102.E5B cmi has cmi set issue that can not pass the power on check
	{
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);
			Fqvi.CH[GP_SW_Buck[nTILE]].fv(0);
			Fvi45.CH[GP_FB_Buck[nTILE]].open();
			Fqvi.CH[GP_SW_Buck[nTILE]].open();
		}
		Connect_all_cap();
		Relay.BIT[K1_B1_RC].set();
		Relay.BIT[K4_B2_RC].set();
		Relay.BIT[K6_B3_RC].set();
		Relay.BIT[K8_B4_RC].set();
		Relay.BIT[K10_B5_RC].set();
		Relay.BIT[K42_FQVI6].set();
		Relay.BIT[K43_FQVI7].set();	
		delayms(2);
		//Normal sequence
		Fvi45.CH[VBYP]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[VOUT_SW]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
		Fvi45.CH[VIN_3V3]		.fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);	
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
		Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
		//Add for 90522102.E5B cmi		
		if(1)
		{
			Fvi45.CH[VBYP]        .fv(VIN_SET+1.5);
			//delayms(2);
			Relay.BIT[K48_I2C]		.set();
			delayms(2);
			Reg[0x2000].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
			Reg[0x2000].write(readback);
			//i2c_init(HCTL_MODE,300000,2);
			//delayms(2);
			//delayms(150);//Add this for BGA part		
			Reg[0x2079].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];
			Reg[0x2079].write(readback);
		
			if(CMI_Rev_No==522103||CMI_Rev_No==520103||CMI_Rev_No==520102||CMI_Rev_No==522102||CMI_Rev_No==522101||CMI_Rev_No==520101)
			{
				Reg[0x2069].read(readback);
				FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];
				Reg[0x2069].write(readback);
			}
			delayms(2);
			Fvi45.CH[VBYP]        .fv(0);
			delayms(2);
			Relay.BIT[K48_I2C].clr();
		}	
		//Add for 90522102.E5B cmi
		Fvi45.CH[VIN_B12].fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 200, -200);
		Fvi45.CH[VIN_B3].fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 200, -200);
		Fvi45.CH[VIN_B4].fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 200, -200);
		Fvi45.CH[VIN_B5].fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 200, -200);
		Fvi16.CH[VIN_LDO1].fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
		Fvi16.CH[VIN_LDO2].fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			Fvi45.CH[GP_FB_Buck[nTILE]].fi(0.0,SITE_ALL,SET_CLAMP,7,-7,0.001,-0.001);
		}
		
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			Fqvi.CH[GP_OUTLDO[nTILE]].fi(0.0,SITE_ALL,SET_CLAMP,7,-7,0.001,-0.001);
		}
		delayms(150);//delay for FBB pins rise up, no less than 120ms
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			Fvi45.CH[GP_FB_Buck[nTILE]].mv(Buck_Vout[nTILE]);		
			FOR_EACH_SITE(nSiteIndex)
			{	
				if(CMI_Rev_No==520103||CMI_Rev_No==520101||CMI_Rev_No==522101||CMI_Rev_No==522104||CMI_Rev_No==522103)//normal check
				{
					if(fabs((Buck_Vout[nTILE][nSiteIndex]-Buck_Vset[nTILE]))<0.15)
						Buck_Vout_status[nTILE][nSiteIndex]=1;
				}
				if(CMI_Rev_No==520102||CMI_Rev_No==522102||CMI_Rev_No==522104)//Disable_B5 in the cmi
				{
					if(nTILE==4)
					{
						if(fabs((Buck_Vout[nTILE][nSiteIndex]-0))<0.15)
							Buck_Vout_status[nTILE][nSiteIndex]=1;
					}
					if(nTILE!=4)
					{
						if(fabs((Buck_Vout[nTILE][nSiteIndex]-Buck_Vset[nTILE]))<0.15)
							Buck_Vout_status[nTILE][nSiteIndex]=1;
					}

				}
				if(CMI_Rev_No==520100||CMI_Rev_No==522100)//B1/2multiphase, B2 no need to check
				{
					if(nTILE==1) Buck_Vout_status[nTILE][nSiteIndex]=1;
					if(nTILE!=1)
					{
						if(fabs((Buck_Vout[nTILE][nSiteIndex]-Buck_Vset[nTILE]))<0.15)
							Buck_Vout_status[nTILE][nSiteIndex]=1;
					}
				}
			}

			Fvi45.CH[GP_FB_Buck[nTILE]].open();
		}
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			Fqvi.CH[GP_OUTLDO[nTILE]].mv(LDO_Vout[nTILE]);
			FOR_EACH_SITE(nSiteIndex)
			{	
				if(fabs((LDO_Vout[nTILE][nSiteIndex]-LDO_Vset[nTILE]))<0.1)
					LDO_Vout_status[nTILE][nSiteIndex]=1;
				if(CMI_Rev_No==520101||CMI_Rev_No==522101) //DIS LDO2
				{
					if(nTILE==1) LDO_Vout_status[nTILE][nSiteIndex]=1;
				}
				
			}
			

		}
		
		FOR_EACH_TILE(nTILE,PMU_MAX)
		{	
			Fvi45.CH[GP_FB_Buck[nTILE]].open();
			Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
			Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		}
		FOR_EACH_TILE(nTILE,LDO_MAX)
		{
			Fqvi.CH[GP_OUTLDO[nTILE]].open();
			Fvi16.CH[GP_INLDO[nTILE]].fv(0);
			Fvi16.CH[GP_INLDO[nTILE]].open(0);
		}
		Relay.BIT[K1_B1_RC].clr();
		Relay.BIT[K4_B2_RC].clr();
		Relay.BIT[K6_B3_RC].clr();
		Relay.BIT[K8_B4_RC].clr();
		Relay.BIT[K10_B5_RC].clr();
		Relay.BIT[K42_FQVI6].clr();
		Relay.BIT[K43_FQVI7].clr();	
	    Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	    Fvi45.CH[VBYP]		.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	    Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
		delayms(5);
		Fvi45.CH[VBYP]		.open();
		Fvi45.CH[VOUT_SW]		.open();
		Fvi45.CH[VIN_3V3]		.open();
		FOR_EACH_SITE(nSiteIndex)
		{
			FOR_EACH_TILE(nTILE,PMU_MAX)
			{			
				pFunction->LogResult(NSITEINDEX, 1+2*nTILE, Buck_Vout[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 2+2*nTILE, Buck_Vout_status[nTILE][nSiteIndex]);		
			}
			FOR_EACH_TILE(nTILE,LDO_MAX)
			{
				pFunction->LogResult(NSITEINDEX, 11+2*nTILE, LDO_Vout[nTILE][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, 12+2*nTILE, LDO_Vout_status[nTILE][nSiteIndex]);			
			}
		}		
	}
		
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Device_Leakage_Recheck
STT_TESTFUNC Device_Leakage_Recheck(CFunction *pFunction, int nSiteNo, int nTestFlag, Device_Leakage_Recheck_params* ours)
{
	double SW_HS_Leak[PMU_MAX][SITE_MAX]={0},SW_LS_Leak[PMU_MAX][SITE_MAX]={0},Buck_Ishut[PMU_MAX][SITE_MAX]={0},FB_Iout[PMU_MAX][SITE_MAX]={0};
	double LDO_Idis[LDO_MAX][SITE_MAX]={0};
	unsigned char RegF_0xxd_Data[PMU_MAX][SITE_MAX]={0},RegF_0xx9_Data[PMU_MAX][SITE_MAX]={0};
	Disconnect_all_cap();
	delayms(2);
	Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(2);
	Relay.BIT[K48_I2C].set();
	delayms(2);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(0x80);
	Reg[0x20fa].write(0x20);
    Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	delayms(2);
	Disable_Sequence();
	Buck_TM_Reset();
	//Buck_Idis
	Reg[0x100a].set_clr_inv(BIT3);
	Reg[0x100b].set_clr_inv(BIT3);
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(nTILE==4)
		{
			Relay.BIT[K41_FQVI5].set();
		}
		if(PMU_MultiP_Mode[nTILE])//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
		delayms(2);
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].read(RegF_0xxd_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0x40|RegF_0xxd_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(readback);

		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].read(RegF_0xx9_Data[nTILE]);
		FOR_EACH_SITE(nSiteIndex)	readback[nSiteIndex]=(0xdf&RegF_0xx9_Data[nTILE][nSiteIndex]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(readback);

		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET+0.3);
		delayms(2);	
		//LS_Leak
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(VIN_SET+0.3,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(VIN_SET+0.3,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_LS_Leak[nTILE]);
		//HS_Leak
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fqvi.CH[GP_SW_Buck[nTILE]].fv(0,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(1);
		Fqvi.CH[GP_SW_Buck[nTILE]].mi(SW_HS_Leak[nTILE]);

		Fqvi.CH[GP_SW_Buck[nTILE]].fi(0);
		Fqvi.CH[GP_SW_Buck[nTILE]].open();

		//Buck_Ishut
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);		
		Fvi45.CH[GP_VIN_Buck[nTILE]].mi(Buck_Ishut[nTILE]);

		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(VIN_SET,SITE_ALL,SET_CLAMP,7,-7,40,-40);	
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(RegF_0xxd_Data[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x09].write(RegF_0xx9_Data[nTILE]);	

		//FB_Iout
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BIT0);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT7);

		if(nTILE==4)
		{	
			Reg[0x1016].write(0x80);
			Reg[0x1018].write(0xe0);
			Reg[0x1019].write(0x0d);
			Reg[0x101d].write(0x0d);
			Reg[0x1021].write(0x0d);
			Reg[0x1025].write(0x0d);
			Reg[0x1029].write(0x0d);	
			Reg[0x102d].write(0x0d);
			Reg[0x1031].write(0x0d);
			SDASCL_Connect_VI();
		}
		delayms(1);		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(2);		
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(5);		
		Fvi45.CH[GP_FB_Buck[nTILE]].mi(FB_Iout[nTILE]);
		if(nTILE==4)
		{
			SDASCL_Connect_I2C();
			Relay.BIT[K41_FQVI5].clr();
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x05].set_clr_inv(BITNONE,BIT0);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 4*nTILE+2, 1e3*SW_LS_Leak[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+3, 1e3*SW_HS_Leak[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+4, 1e3*Buck_Ishut[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 4*nTILE+5, 1e3*FB_Iout[nTILE][nSiteIndex]);
		}
	}	
	//LDO_Idis
	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();

	Fvi16.CH[VIN_LDO1]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall5mA);
	Fvi16.CH[VIN_LDO2]	.fv(VIN_SET,SITE_ALL,Stall12V,Stall5mA);
	Reg[0x1093].set_clr_inv(BITNONE,BIT7|BIT3);
	Reg[0x1094].set_clr_inv(BIT7|BIT3);

	Fqvi.CH[OUT_LDO1].fv(0,SITE_ALL,SET_CLAMP,4,-4,40,-40);
	Fqvi.CH[OUT_LDO2].fv(0,SITE_ALL,SET_CLAMP,4,-4,40,-40);
    delayms(2);
	Fqvi.CH[OUT_LDO1].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
	Fqvi.CH[OUT_LDO2].fv(0,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
    delayms(2);
	Fqvi.CH[OUT_LDO1].mi(LDO_Idis[0]);
	Fqvi.CH[OUT_LDO2].mi(LDO_Idis[1]);

	Fqvi.CH[OUT_LDO1].open();
	Fqvi.CH[OUT_LDO2].open();

	Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();

	Fvi16.CH[VIN_LDO1]	.fv(0);
	Fvi16.CH[VIN_LDO2]	.fv(0);
	Fvi16.CH[VIN_LDO1]	.open();
	Fvi16.CH[VIN_LDO2]	.open();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 22, 1e3*LDO_Idis[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 23, 1e3*LDO_Idis[1][nSiteIndex]);
	}							 		
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[VBYP]      .fv(0);	
	delayms(2);		
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]      .open();
	return TRUE;
}// END_STT_TESTFUNC
// Test Function: Digital_Check
STT_TESTFUNC Digital_Check(CFunction *pFunction, int nSiteNo, int nTestFlag, Digital_Check_params* ours)
{
	double SDA_VOL[SITE_MAX]={0};
	int I2C_Print_Flag=0;
	int I2C_Capture_Flag=0;

	int I2C_Check_P[2][SITE_MAX]={0};
	int I2C_Check_P1[SITE_MAX]={0};
	int I2C_Check_P2[SITE_MAX]={0};
	int Scan_test1_P[SITE_MAX]={0},Scan_test2_P[SITE_MAX]={0};
	int Scan_check_fail_line[SITE_MAX]={0};
    //Scan test
	if(I2C_Check_Flag==1)
	{
	   Connect_all_cap();
	   delayms(2);
	   //Enter_Factory();
	   Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	   Fvi45.CH[VOUT_SW]		.fv(VIN_SET-0.3);
	   Fvi45.CH[VIN_3V3]		.fv(VIN_SET-0.3);
	   delayms(2);
	   Relay.BIT[K48_I2C].set();	
	   delayms(2);
	   Reg[0x2000].read(readback);
	   FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	   Reg[0x2000].write(readback);
	   Reg[0x1012].write(0xff);
	   Reg[0x2007].write(0x80);
	   Reg[0x20fa].write(0x20);
	   delayms(1); 
	   //Enter Scan mode
	   Factory_Mode_Check();
	   Reg[0x1084].write(_0x00);
	   delayms(2);
	   Reg[0x2007].write(0x10);
	   
	   Relay.BIT[K48_I2C].clr();
	   Relay.BIT[K45_ScanOut].set();
	   Relay.BIT[K46_ScanI2C].set();
       
	   SetRelayStatus(0, 0x0c00, sErrorMsg,nErrorMsgLen);//Close DDD K10/K11 for both sites
	   delayms(5);
	   //pattern1
	   nResult = RunPatternTillDoneBySite(sSITE,Scan_file1,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		   &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);
	   Scan_check_fail_line[0]=naFirstFailedCycle[0]+1;//Print out the 1st fail line
       
	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("Scan_file1 has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
		   Scan_test1_P[0]=-1;Scan_test1_P[1]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("Scan_file1 function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
			   printf("site1 FirstFailedCycle is %d\n", naFirstFailedCycle[0]);
		   }
		   Scan_test1_P[0]=naPatternPassed[0]; Scan_test1_P[1]=naPatternPassed[1];
	   }

	   //Set capture option	  
	   if(I2C_Print_Flag==1)
	   {
		   nResult = SetCaptureOption("CaptureOnStartCycle", 40000, sErrorMsg, nErrorMsgLen);
		   if(nResult < 0)
		   {
			   printf ("SetCaptureOption has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
			   output1[1]=-12;	output1[2]=-12;
		   }
		   else
		   {
			   printf("SetCaptureOption function is successful.\n");
		   } 
	       delayms(2);
	   }	  
      //pattern2
	   delayms(15); 
	   nResult = RunPatternTillDoneBySite(sSITE,Scan_file2,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		  &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);
	   Scan_check_fail_line[1]=naFirstFailedCycle[0]+1;//Print out the 1st fail line
       
	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("Scan_file2 has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
           Scan_test2_P[0]=-1;Scan_test2_P[1]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("Scan_file2 function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
			   printf("site1 FirstFailedCycle is %d\n", naFirstFailedCycle[0]);
		   }
		   Scan_test2_P[0]=naPatternPassed[0]; Scan_test2_P[1]=naPatternPassed[1];
	   }
	   //Get capture data
	   if(I2C_Print_Flag==1)
	   {
		   nResult = GetCapturedDataAllBySite(0, Scan_sPinGrpList, sCaptureDataFile, 800, naAcquiredCycles[0], sErrorMsg, nErrorMsgLen);
		   if(nResult < 0)
		   {
			   printf ("GetCapturedDataAllBySite has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
			   output1[1]=-12;	output1[2]=-12;
		   }
		   else
		   {
			   printf("GetCapturedDataAllBySite function is successful.\n");
		   }
	   }	  
	   delayms(2);
	   Relay.BIT[K45_ScanOut].clr();
	   Relay.BIT[K46_ScanI2C].clr();
	   SetRelayStatus(0, 0x0000, sErrorMsg,nErrorMsgLen);//open DDD K0-K11
	   Fvi45.CH[VOUT_SW]	.fv(0);
	   Fvi45.CH[VBYP]		.fv(0);
	   Fvi45.CH[VIN_3V3]	.fv(0);

	   delayms(50);
	   //Scan_test
	   //I2C_test
	   
	   Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	   Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	   Fvi45.CH[VIN_3V3]		.fv(VIN_SET);
	   delayms(2);
	   Relay.BIT[K48_I2C].set();
	   delayms(2);
	   Reg[0x2000].read(readback);
	   FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	   Reg[0x2000].write(readback);
	   Reg[0x1012].write(0xff);
	   Reg[0x2007].write(0x80);
	   Reg[0x20fa].write(0x20);
	   Reg[0x1016].write(0x40);
	   Reg[0x1081].write(0x78);
	   Reg[0x1082].write(0x78);
	   Reg[0x1083].write(0x03);
	   Reg[0x1084].write(_0x00);
	   Reg[0x1085].write(0x45);	
	   Fvi45.CH[FB5]		.fv(1.8,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	   delayms(2);
       
	   Relay.BIT[K48_I2C].clr();
	   Relay.BIT[K46_ScanI2C].set();
	   SetRelayStatus(0, 0x0c00, sErrorMsg,nErrorMsgLen);//Close DDD K10/K11 for both sites

	   //VIH_VOUTWS
	   delayms(2);
	   nResult=SetVDriveLoByPinGrp(Scan_sPinGrpList, 0.4, sErrorMsg,nErrorMsgLen);// set Driver High to 1.8v for all 5 pins  
	   nResult=SetVDriveHiByPinGrp(Scan_sPinGrpList, 1.8, sErrorMsg,nErrorMsgLen);// set Driver High to 3.3v for all 5 pins
	   delayms(5);
	   nResult = RunPatternTillDoneBySite(sSITE,sPattern_W55,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		 &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);

	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("RunPatternTillDone has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
           output1[1]=-1;output1[2]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("RunPatternTillDone function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
		   }
		   output1[1]=naPatternPassed[0]; output1[2]=naPatternPassed[1];
	   }

	   Relay.BIT[K48_I2C].set();
	   delayms(2);
	   Relay.BIT[K46_ScanI2C].clr();
	   delayms(5);
	   Reg[0x1004].read(readback);
       if(readback[0]==0X55)
         I2C_Check_P1[0]=1;
       if(readback[1]==0X55)
		I2C_Check_P1[1]=1;
	   Relay.BIT[K48_I2C].clr();
	   delayms(2);
	   Relay.BIT[K46_ScanI2C].set();
	   delayms(2);
	   nResult = RunPatternTillDoneBySite(sSITE,sPattern_WAA,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		  &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);
	   
	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("RunPatternTillDone has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
          output1[1]=-1;output1[2]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("RunPatternTillDone function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
		   }
		  output1[1]=naPatternPassed[0]; output1[2]=naPatternPassed[1];
	   }
	   Relay.BIT[K48_I2C].set();
	   Relay.BIT[K46_ScanI2C].clr();

       delayms(2);
	   Reg[0x1004].read(readback);
	
	   if(readback[0]==0XAA)
		  I2C_Check_P2[0]=1;
	   if(readback[1]==0XAA)
		  I2C_Check_P2[1]=1;
       if(I2C_Check_P1[0]&&I2C_Check_P2[0])
          I2C_Check_P[0][0]=1;
	   if(I2C_Check_P1[1]&&I2C_Check_P2[1])
		  I2C_Check_P[0][1]=1;

	   //VIH_FBB5
	   //Fvi45.CH[VBYP].fv(0); //issue can not force 0V or the 2nd check fail
	   delayms(1);
	   Relay.BIT[K48_I2C].clr();
	   delayms(2);
	   Relay.BIT[K46_ScanI2C].set();
	   delayms(5);
	   I2C_Check_P1[0]=0;
	   I2C_Check_P1[1]=0;
	   I2C_Check_P2[0]=0;
	   I2C_Check_P2[1]=0;

	   nResult=SetVDriveLoByPinGrp(Scan_sPinGrpList, 0.4, sErrorMsg,nErrorMsgLen);// set Driver High to 0.4v for all 5 pins
	   nResult=SetVDriveHiByPinGrp(Scan_sPinGrpList, 1.05, sErrorMsg,nErrorMsgLen);// set Driver High to 1.05v for all 5 pins
	   delayms(5);
	   nResult = RunPatternTillDoneBySite(sSITE,sPattern_W55,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		   &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);
	   
	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("RunPatternTillDone has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
           output1[1]=-1;output1[2]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("RunPatternTillDone function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
		   }
		   output1[1]=naPatternPassed[0]; output1[2]=naPatternPassed[1];
	   }
	   
	   Relay.BIT[K48_I2C].set();
	   delayms(2);
	   Relay.BIT[K46_ScanI2C].clr();
	   delayms(5);
	   Reg[0x1004].read(readback);
       if(readback[0]==0X55)
		   I2C_Check_P1[0]=1;
       if(readback[1]==0X55)
		   I2C_Check_P1[1]=1;
	   Relay.BIT[K48_I2C].clr();
	   delayms(2);
	   Relay.BIT[K46_ScanI2C].set();
	   delayms(2);
	   nResult = RunPatternTillDoneBySite(sSITE,sPattern_WAA,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
		   &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);
	   
	   if(nResult < 0)
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf ("RunPatternTillDone has error and ErrorMsg is:\n");
			   printf (sErrorMsg);
			   printf("\n");
		   }
		   output1[1]=-1;output1[2]=-1;
	   }
	   else
	   {
		   if(I2C_Print_Flag==1)
		   {
			   printf("RunPatternTillDone function is successful.\n");
			   printf("site1 Pattern finished? %d\n", naPatternFinished[0]);
			   printf("site2 Pattern finished? %d\n", naPatternFinished[1]);
			   printf("site1 Pattern Passed? %d\n", naPatternPassed[0]);
			   printf("site2 Pattern Passed? %d\n", naPatternPassed[1]);
			   printf("site1 Finished Pattern Length is %d\n", naTotalCycles[0]);
			   printf("site2 Finished Pattern Length is %d\n", naTotalCycles[1]);
		   }
		   output1[1]=naPatternPassed[0]; output1[2]=naPatternPassed[1];
	   }
	   Relay.BIT[K48_I2C].set();
	   Relay.BIT[K46_ScanI2C].clr();
	   
       delayms(2);
	   Reg[0x1004].read(readback);
	   
	   if(readback[0]==0XAA)
		   I2C_Check_P2[0]=1;
	   if(readback[1]==0XAA)
		   I2C_Check_P2[1]=1;
       if(I2C_Check_P1[0]&&I2C_Check_P2[0])
		   I2C_Check_P[1][0]=1;
	   if(I2C_Check_P1[1]&&I2C_Check_P2[1])
		   I2C_Check_P[1][1]=1;
	   //************SDA_Voltage_Output***************
	   nResult=SetVDriveLoByPinGrp(Scan_sPinGrpList, 0, sErrorMsg,nErrorMsgLen);// set Driver High to 1.8v for all 5 pins  
	   nResult=SetVDriveHiByPinGrp(Scan_sPinGrpList, VIN_SET, sErrorMsg,nErrorMsgLen);// set Driver High to 3.3v for all 5 pins 
	   delayms(5);
	   //Output_low	
	   //VOL
	   Fvi16.CH[SDA].open();
	   Fvi16.CH[SCL].open();

	   Fvi45.CH[VBYP].fv(VIN_SET+1.5);
	   delayms(2);
	   Relay.BIT[K48_I2C].clr();
	   Relay.BIT[K46_ScanI2C].set();
	   delayms(2);
       nResult = RunPatternTillDoneBySite(sSITE,sPattern_W4A,&naPatternFinished[0],nPatternFinishedLen,&naPatternPassed[0],nPatternPassedLen,&naAcquiredCycles[0],nAcquiredCyclesLen,
         &naFirstFailedCycle[0],nFirstFailedCycleLen,sFailedPinList,nFailedPinListLen,&naTotalCycles[0],nTotalCyclesLen,sErrorMsg,nErrorMsgLen);	 

	   Relay.BIT[K46_ScanI2C].clr();
	   SetRelayStatus(0, 0x0000, sErrorMsg,nErrorMsgLen);//open DDD K0-K11
	   delayms(2);

	   Fvi16.CH[SDA].fi(5,SITE_ALL,Stall3V3,Stall50mA);
	   delayms(2);
	   Fvi16.CH[SDA].mv(SDA_VOL);

	   Fvi16.CH[SDA].fi(0,SITE_ALL,Stall3V3,Stall50mA);
	   delayms(2);
	   Fvi16.CH[SDA].open();
 
	   Fvi45.CH[VOUT_SW]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	   Fvi45.CH[VBYP]		.fv(0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
	   Fvi45.CH[VIN_3V3]	.fv(0,SITE_ALL,SET_CLAMP,7,-7,400,-400);
	   Fvi45.CH[FB5]		.fv(0);
	   Fvi45.CH[FB5]		.open();
	   delayms(2);
	   Fvi45.CH[VOUT_SW]	.open();
	   Fvi45.CH[VIN_3V3]	.open();
	   Fvi45.CH[VBYP]      .open();
	   //************I2C_Check***************
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 2, Scan_test1_P[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 3, Scan_test2_P[nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 4, I2C_Check_P[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 5, I2C_Check_P[1][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 6, 1e3*SDA_VOL[nSiteIndex]);
		}
	}

	return TRUE;
}// END_STT_TESTFUNC


// Test Function: Master_Timer_sub1
STT_TESTFUNC Master_Timer_sub1(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_Timer_sub1_params* ours)
{
	double LPM_Exit_LPM_Delay_Target[64]={0};
	double LPM_Exit_LPM_Delay_0[SITE_MAX]={0},LPM_Exit_LPM_Delay_CMI[64][SITE_MAX]={0}; 
	double LPM_Exit_LPM_Delay[64][SITE_MAX]={0},LPM_Exit_LPM_Delay_Acc[64][SITE_MAX]={0}; 


	double LDO_OnDelay_Target[LDO_MAX][16]={0},LDO_OffDelay_Target[LDO_MAX][16]={0};
	double LDO_OnDelay_0[LDO_MAX][SITE_MAX]={0}, LDO_OffDelay_0[LDO_MAX][SITE_MAX]={0};
	double LDO_OnDelay_CMI[LDO_MAX][16][SITE_MAX]={0}, LDO_OffDelay_CMI[LDO_MAX][16][SITE_MAX]={0};
	double LDO_OnDelay[LDO_MAX][16][SITE_MAX]={0}, LDO_OffDelay[LDO_MAX][16][SITE_MAX]={0};
	double LDO_OnDelay_Acc[LDO_MAX][16][SITE_MAX]={0},LDO_OffDelay_Acc[LDO_MAX][16][SITE_MAX]={0};
	double LDO_SST[8][LDO_MAX][SITE_MAX]={0};
   	
	double Buck_OnDelay_Target[PMU_MAX][16]={0},Buck_OffDelay_Target[PMU_MAX][8]={0};
	double Buck_OnDelay_0[PMU_MAX][SITE_MAX]={0}, Buck_OffDelay_0[PMU_MAX][SITE_MAX]={0};
	double Buck_OnDelay_CMI[PMU_MAX][16][SITE_MAX]={0}, Buck_OffDelay_CMI[PMU_MAX][8][SITE_MAX]={0};
	double Buck_OnDelay[PMU_MAX][16][SITE_MAX]={0}, Buck_OffDelay[PMU_MAX][8][SITE_MAX]={0};
	double Buck_OnDelay_Acc[PMU_MAX][16][SITE_MAX]={0},Buck_OffDelay_Acc[PMU_MAX][8][SITE_MAX]={0};
	

	double LPM_On_Delay_LDO_Target[LDO_MAX][16]={0},LPM_On_Delay_LDO[LDO_MAX][16][SITE_MAX]={0},LPM_On_Delay_LDO_Acc[LDO_MAX][16][SITE_MAX]={0};
	double LPM_On_Delay_LDO_0[LDO_MAX][SITE_MAX]={0},LPM_On_Delay_LDO_CMI[LDO_MAX][16][SITE_MAX]={0}; 
	double LPM_On_Delay_Buck_Target[PMU_MAX][16]={0},LPM_On_Delay_Buck[PMU_MAX][16][SITE_MAX]={0},LPM_On_Delay_Buck_Acc[PMU_MAX][16][SITE_MAX]={0};
	double LPM_On_Delay_Buck_0[PMU_MAX][SITE_MAX]={0},LPM_On_Delay_Buck_CMI[PMU_MAX][16][SITE_MAX]={0}; 
	//resource_init();
	//Connect_all_cap();//enable if run this function only
    Fvi45.CH[VBYP]		    .fv(VIN_SET+1.5,SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
    Fvi45.CH[VOUT_SW]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET,SITE_ALL, SET_CLAMP, 7, -7, 400, -400);
	delayms(1);
    Fvi16.CH[VIN_LDO1]      .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	Fvi16.CH[VIN_LDO2]      .fv(VIN_SET,SITE_ALL,Stall3V3,Stall50mA);
	delayms(3);
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K48_I2C].set();
	//delayms(5);	
	//i2c_init(HCTL_MODE,300000,2);//enable if run this function only
	delayms(3);//enable if run this function only
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x20fa].write(0x20);
	delayms(1);
	Fvi45.CH[FB5]           .fv(1.8, SITE_ALL, SET_CLAMP, 7, -7, 40, -40);
	delayms(1);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}
	//************LPM_ExitLPM_Delay**********
	
	FOR_EACH_TILE(nTILE,PMU_MAX) 
	{
		if(PMU_MultiP_Mode[nTILE]&&nTILE==0)//Set B1/2 to ACT mode if B1/2 multi-phase mode=1
		{
			Reg[0x2047].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];
			Reg[0x2047].write(readback);	
		}
	}
	Relay.BIT[K18_EXTEN_TMUA].set();
	Relay.BIT[K25_LPM_TMUB].set();
	delayms(1);
	TMU_CH1.stallset(Stall7V,TMU4mS,10);
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,DOWN);

	//Maks fault
	/*
	Reg[0x1040].set_clr_inv(BIT3);
	Reg[0x1050].set_clr_inv(BIT3);
	Reg[0x1060].set_clr_inv(BIT3);
	Reg[0x1070].set_clr_inv(BIT3);
	Reg[0x1080].set_clr_inv(BIT3);
	*/
	Reg[0x1040].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1050].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1060].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1070].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1080].set_clr_inv(BIT3|BIT2|BIT1|BIT0);
	Reg[0x1045].set_clr_inv(BIT2);
	Reg[0x1055].set_clr_inv(BIT2);
	Reg[0x1065].set_clr_inv(BIT2);
	Reg[0x1075].set_clr_inv(BIT2);
	Reg[0x1085].set_clr_inv(BIT2);	
	Reg[0x1091].write(_0x00);
	//Maks fault
	Reg[0x2058].set_clr_inv(BITNONE,BIT7);
	Reg[0x100a].write(0xfa);
	Reg[0x100d].write(_0x00);
	//Add per new TM
	Reg[0x101a].write(0x80);
	Reg[0x1028].write(_0x00);//Set it to 0 for multiphase cmi
	Reg[0x1019].write(0x09);
	Reg[0x1021].write(0x09);
	Reg[0x1025].write(0x09);
	Reg[0x1029].write(0x09);
	Reg[0x102d].write(0x09);
	//Add per new TM
	Reg[0x101d].write(0xff);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(_0x00);
	Reg[0x1031].write(0x77);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(_0x00);
	Reg[0x1081].write(0x78);
	Reg[0x1082].write(0x78);
	Reg[0x1083].write(0xca);
	Reg[0x1084].write(_0x00);
	Reg[0x1085].write(_0x00);
	Reg[0x1086].write(_0x00);
	Reg[0x1093].write(0x99);
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);

	Reg[0x1010].write(_0x00);
	Reg[0x1011].write(0x80);
	Reg[0x1012].write(0xf1);
	Reg[0x1013].write(_0x00);
	Reg[0x1014].write(_0x00);
	Reg[0x1015].write(_0x00);
	Reg[0x2007].write(_0x00);
	Reg[0x1006].write(0x05);
	delayms(5);
	if(CMI_Rev_No==520100||CMI_Rev_No==522100) 	delayms(45);
	
	
		
		
	
		//LPM_ExitLPM_Delay0
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1006].write(0x04);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LPM_Exit_LPM_Delay_0[nSiteIndex]=meas_data[nSiteIndex+1];
		
		delayms(1);
	for(i=0;i<64;i++)
	{

		Reg[0x1006].write(0x05);
		Reg[0x1014].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xc0&readback[nSiteIndex])|i;
		Reg[0x1014].write(readback);
		LPM_Exit_LPM_Delay_Target[i]=10*(readback[0]&0x3f);
		if(LPM_Exit_LPM_Delay_Target[i]==0) LPM_Exit_LPM_Delay_Target[i]=630;
		//else if(LPM_Exit_LPM_Delay_Target[i]>400) LPM_Exit_LPM_Delay_Target[i]=400;

		//LPM_ExitLPM_Delay_CMI
		if((readback[0]&0x3f)==0) Reg[0x1014].write(0x3f);
		Reg[0x1014].read(readback);
		delayms(1);
		
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1006].write(0x04);
		delayms(1);
		TMU_CH1.readdata();
		
		FOR_EACH_SITE(nSiteIndex)
		{	
			LPM_Exit_LPM_Delay_CMI[i][nSiteIndex]=meas_data[nSiteIndex+1];
			LPM_Exit_LPM_Delay[i][nSiteIndex]=LPM_Exit_LPM_Delay_CMI[i][nSiteIndex]-LPM_Exit_LPM_Delay_0[nSiteIndex];
			LPM_Exit_LPM_Delay_Acc[i][nSiteIndex]=100*(LPM_Exit_LPM_Delay[i][nSiteIndex]-LPM_Exit_LPM_Delay_Target[i])/LPM_Exit_LPM_Delay_Target[i];
		}
		delayms(1);
		
		
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 1+i*3, LPM_Exit_LPM_Delay_Target[i]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_Exit_LPM_Delay_0[nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+2, LPM_Exit_LPM_Delay_CMI[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 2+i*3, LPM_Exit_LPM_Delay[i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 3+i*3, LPM_Exit_LPM_Delay_Acc[i][nSiteIndex]);
		}	
	}
	
	Reg[0x1014].write(_0x00);
	
	Relay.BIT[K18_EXTEN_TMUA].clr();
		Relay.BIT[K25_LPM_TMUB].clr();


	//************LPM_ExitLPM_Delay**********
	


	//************LDO_Delay**********	

	
	
	Fvi45.CH[FB1].fv(Buck_Vset[0], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
	delayms(1);
	Relay.BIT[K19_EXTEN_TMUB].set();
	Relay.BIT[K21_GPIO1_TMUA].set();
	delayms(1);	
	TMU_CH1.stallset(Stall7V,TMU4mS,10);
	TMU_CH1.switchset(STARTtoA+STOPtoB);

	Reg[0x2008].write(0x04);
	Reg[0x1031].write(0x0f);
	Reg[0x1032].write(_0x00);
	Reg[0x1033].write(_0x00);
	Reg[0x1034].write(0x80);
	Reg[0x1025].write(0x0f);
	Reg[0x1026].write(_0x00);
	Reg[0x1027].write(_0x00);
	Reg[0x1028].write(0x80);
	Reg[0x101d].write(0x0f);
	Reg[0x101e].write(_0x00);
	Reg[0x101f].write(_0x00);
	Reg[0x1020].write(0x80);
	Reg[0x1095].write_CMI();
	Reg[0x2095].write(0x06);
	Reg[0x204d].write(0x8a);
	Reg[0x204f].write(0x40);
	Reg[0x1043].set_clr_inv(BIT6|BIT5|BIT4);
	Reg[0x1046].write(0xff);
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);	
	Reg[0x109d].set_clr_inv(BITNONE,BIT0);//New adding for BZ
	Reg[0x1044].set_clr_inv(BIT2|BIT1|BIT0);//New adding for BZ
	Reg[0x1012].write(0xf5);

	delayms(2);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_0[0][nSiteIndex]=meas_data[nSiteIndex+1];

	Reg[0x1012].write(0xf5);
	delayms(1);
	Reg[0x1012].write(0x71);
	delayms(1);
	//LDO_off_delay_0
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf5);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OffDelay_0[0][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x109a].read(readback);




	for(i=0;i<16;i++)
	{	
	
		Reg[0x109a].read(readback);
		FOR_EACH_SITE(nSiteIndex) 
		{
			readback[nSiteIndex]=(0x0f&readback[nSiteIndex])|i<<4;
			readback[nSiteIndex]=(0xf0&readback[nSiteIndex])|i;
		}
		Reg[0x109a].write(readback);
		LDO_OnDelay_Target[0][i]=((0xf0&readback[0])>>4)*100;
		LDO_OffDelay_Target[0][i]=(0x0f&readback[0])*50;
		if(LDO_OnDelay_Target[0][i]==0) LDO_OnDelay_Target[0][i]=100;
		if(LDO_OffDelay_Target[0][i]==0) LDO_OffDelay_Target[0][i]=50;

		
	
		//LDO_on off_delay_cmi		
		Reg[0x109a].read(readback);
		if((readback[0]&0x0f)==0x00)
			Reg[0x109a].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
		if((readback[0]&0xf0)==0x00)	
			Reg[0x109a].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
		if(readback[0]==0x00)
			Reg[0x109a].set_clr_inv(BIT0|BIT4,BIT1|BIT2|BIT3|BIT5|BIT6|BIT7);	

		Reg[0x109a].read(readback);
		TMU_CH1.switchset(STARTtoA+STOPtoB);
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OnDelay_CMI[0][i][nSiteIndex]=meas_data[nSiteIndex+1];

		//LDO_off_delay_cmi	
		
		if(i<7)
		{
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf5);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][i][nSiteIndex]=meas_data[nSiteIndex+1];
		}
		if(i==7)
		{
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf5);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][i][nSiteIndex]=meas_data[nSiteIndex+1];
			if(LDO_OffDelay_CMI[0][i][0]>6000)
			{
				TMU_CH1.switchset(STARTtoB+STOPtoA);
				Reg[0x1012].write(0x71);
				delayms(1);
				TMU_CH1.startset(1.5,DOWN);
				TMU_CH1.stopset(1.5,DOWN);
				TMU_CH1.arm(1);
				TMU_CH1.waitTRIG();
				delayms(1);
				Reg[0x1012].write(0xf5);
				delay(300);
				TMU_CH1.readdata();
				FOR_EACH_SITE(nSiteIndex)
				LDO_OffDelay_CMI[0][i][nSiteIndex]=meas_data[nSiteIndex+1];
				if(LDO_OffDelay_CMI[0][i][0]>6000)
					LDO_OffDelay_CMI[0][i][0]=0;
			}
		}
		if(i>7)
		{	
			TMU_CH1.switchset(STARTtoB+STOPtoA);
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf5);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[0][i][nSiteIndex]=-meas_data[nSiteIndex+1];
		}
		

		
		
		

		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_OnDelay[0][i][nSiteIndex]=LDO_OnDelay_0[0][nSiteIndex]-LDO_OnDelay_CMI[0][i][nSiteIndex];
			LDO_OnDelay_Acc[0][i][nSiteIndex]=100*(LDO_OnDelay[0][i][nSiteIndex]-LDO_OnDelay_Target[0][i])/LDO_OnDelay_Target[0][i];

			LDO_OffDelay[0][i][nSiteIndex]=LDO_OffDelay_0[0][nSiteIndex]-LDO_OffDelay_CMI[0][i][nSiteIndex];
			LDO_OffDelay_Acc[0][i][nSiteIndex]=100*(LDO_OffDelay[0][i][nSiteIndex]-LDO_OffDelay_Target[0][i])/LDO_OffDelay_Target[0][i];
		}
		TestNO_target=193;
		FOR_EACH_SITE(nSiteIndex)
		{
			//pFunction->LogResult(NSITEINDEX, TestNO_target+0, LDO_OnDelay_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+1, LDO_OnDelay_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6, LDO_OnDelay_Target[0][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+1, LDO_OnDelay[0][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+2, LDO_OnDelay_Acc[0][i][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+5, LDO_OffDelay_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+6, LDO_OffDelay_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+3, LDO_OffDelay_Target[0][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+4, LDO_OffDelay[0][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+5, LDO_OffDelay_Acc[0][i][nSiteIndex]);
		}

	}
	//New adding for bz	
	//LDO1
	
		//if(nTILE==1) LPM_On_Delay_LDO_Target[nTILE]=((0x0f&Reg[0x109c].CMI)>>0)*100;
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);

	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);
	Reg[0x109c].write(_0x00);
	Reg[0x109d].set_clr_inv(BIT0);
	Reg[0x1012].write(0xf5);	
	delayms(5);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LPM_On_Delay_LDO_0[0][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x1012].write(0xf5);	
	delayms(5);
	
	for(i=0;i<16;i++)
	{
		
		delayms(2);
		Reg[0x109c].read(readback);
		FOR_EACH_SITE(nSiteIndex) 
			readback[nSiteIndex]=(0x0f&readback[nSiteIndex])|i<<4;
		Reg[0x109c].write(readback);
		LPM_On_Delay_LDO_Target[0][i]=((0xf0&readback[0])>>4)*100;
		if(LPM_On_Delay_LDO_Target[0][i]==0) LPM_On_Delay_LDO_Target[0][i]=100;
		if((readback[0]&0xf0)==0x00)
			Reg[0x109c].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
		
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LPM_On_Delay_LDO_CMI[0][i][nSiteIndex]=meas_data[nSiteIndex+1];
			LPM_On_Delay_LDO[0][i][nSiteIndex]=LPM_On_Delay_LDO_0[0][nSiteIndex]-LPM_On_Delay_LDO_CMI[0][i][nSiteIndex];
			LPM_On_Delay_LDO_Acc[0][i][nSiteIndex]=100*(LPM_On_Delay_LDO[0][i][nSiteIndex]-LPM_On_Delay_LDO_Target[0][i])/LPM_On_Delay_LDO_Target[0][i];
			
		}
		Reg[0x1012].write(0xf5);
		TestNO_target=289;
		FOR_EACH_SITE(nSiteIndex)
		{
			
			//pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_On_Delay_LDO_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_On_Delay_LDO_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3, LPM_On_Delay_LDO_Target[0][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3+1, LPM_On_Delay_LDO[0][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3+2, LPM_On_Delay_LDO_Acc[0][i][nSiteIndex]);
			
		}
	}
		
		Reg[0x109d].set_clr_inv(BITNONE,BIT0);
	//New adding for bz
	
	
	//LDO_SST		
	Reg[0x109a].write(_0x00);
	Reg[0x2095].write(0x05);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	for(i=0;i<8;i++)
	{
		Reg[0x1012].write(0xf5);	
		Reg[0x1095].write((0x03&Reg[0x1095].CMI)|(i<<5));    
		delayms(1);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delay(300);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_SST[i][0][nSiteIndex]=0.9*(LDO_OnDelay_0[0][nSiteIndex]-meas_data[nSiteIndex+1]);
			//pFunction->LogResult(NSITEINDEX, 22+i, LDO_SST[i][0][nSiteIndex]);
		}
	}
	

	//Relay.BIT[K19_EXTEN_TMUB].clr();
	Relay.BIT[K21_GPIO1_TMUA].clr();

	//Relay.BIT[K19_EXTEN_TMUB].set();

	//LDO_on_delay_0
	Relay.BIT[K24_LPM_TMUA].set();
	Reg[0x2095].write(0x0f);
	Reg[0x1012].write(0xf3);
	delayms(2);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OnDelay_0[1][nSiteIndex]=meas_data[nSiteIndex+1];

	//LDO_off_delay_0
	Reg[0x1012].write(0xf3);
	delayms(1);
	Reg[0x1012].write(0x71);
	delayms(1);
	TMU_CH1.startset(1.5,DOWN);
	TMU_CH1.stopset(1.5,DOWN);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0xf3);
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LDO_OffDelay_0[1][nSiteIndex]=meas_data[nSiteIndex+1];


	for(i=0;i<16;i++)
	{	
	
		Reg[0x109b].read(readback);
		FOR_EACH_SITE(nSiteIndex) 
		{
			readback[nSiteIndex]=(0x0f&readback[nSiteIndex])|i<<4;
			readback[nSiteIndex]=(0xf0&readback[nSiteIndex])|i;
		}
		Reg[0x109b].write(readback);
		LDO_OnDelay_Target[1][i]=((0xf0&readback[0])>>4)*100;
		LDO_OffDelay_Target[1][i]=(0x0f&readback[0])*50;
		if(LDO_OnDelay_Target[1][i]==0) LDO_OnDelay_Target[1][i]=100;
		if(LDO_OffDelay_Target[1][i]==0) LDO_OffDelay_Target[1][i]=50;

		
	
		//LDO_on off_delay_cmi		
		Reg[0x109b].read(readback);
		if((readback[0]&0x0f)==0x00)
			Reg[0x109b].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
		if((readback[0]&0xf0)==0x00)	
			Reg[0x109b].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
		if(readback[0]==0x00)
			Reg[0x109b].set_clr_inv(BIT0|BIT4,BIT1|BIT2|BIT3|BIT5|BIT6|BIT7);	

		Reg[0x109b].read(readback);
		TMU_CH1.switchset(STARTtoA+STOPtoB);
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			LDO_OnDelay_CMI[1][i][nSiteIndex]=meas_data[nSiteIndex+1];

		//LDO_off_delay_cmi	
		
		if(i<7)
		{
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf3);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][i][nSiteIndex]=meas_data[nSiteIndex+1];
		}
		if(i==7)
		{
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf3);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][i][nSiteIndex]=meas_data[nSiteIndex+1];
			if(LDO_OffDelay_CMI[1][i][0]>6000)
			{
				TMU_CH1.switchset(STARTtoB+STOPtoA);
				Reg[0x1012].write(0x71);
				delayms(1);
				TMU_CH1.startset(1.5,DOWN);
				TMU_CH1.stopset(1.5,DOWN);
				TMU_CH1.arm(1);
				TMU_CH1.waitTRIG();
				delayms(1);
				Reg[0x1012].write(0xf3);
				delay(300);
				TMU_CH1.readdata();
				FOR_EACH_SITE(nSiteIndex)
				LDO_OffDelay_CMI[1][i][nSiteIndex]=meas_data[nSiteIndex+1];
				if(LDO_OffDelay_CMI[1][i][0]>6000)
					LDO_OffDelay_CMI[1][i][0]=0;
			}
		}
		if(i>7)
		{	
			TMU_CH1.switchset(STARTtoB+STOPtoA);
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(0xf3);
			delay(300);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			LDO_OffDelay_CMI[1][i][nSiteIndex]=-meas_data[nSiteIndex+1];
		}
		

		
		
		

		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_OnDelay[1][i][nSiteIndex]=LDO_OnDelay_0[1][nSiteIndex]-LDO_OnDelay_CMI[1][i][nSiteIndex];
			LDO_OnDelay_Acc[1][i][nSiteIndex]=100*(LDO_OnDelay[1][i][nSiteIndex]-LDO_OnDelay_Target[1][i])/LDO_OnDelay_Target[1][i];

			LDO_OffDelay[1][i][nSiteIndex]=LDO_OffDelay_0[1][nSiteIndex]-LDO_OffDelay_CMI[1][i][nSiteIndex];
			LDO_OffDelay_Acc[1][i][nSiteIndex]=100*(LDO_OffDelay[1][i][nSiteIndex]-LDO_OffDelay_Target[1][i])/LDO_OffDelay_Target[1][i];
		}
		TestNO_target=337;
		FOR_EACH_SITE(nSiteIndex)
		{
			//pFunction->LogResult(NSITEINDEX, TestNO_target+0, LDO_OnDelay_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+1, LDO_OnDelay_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6, LDO_OnDelay_Target[1][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+1, LDO_OnDelay[1][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+2, LDO_OnDelay_Acc[1][i][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+5, LDO_OffDelay_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+6, LDO_OffDelay_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+3, LDO_OffDelay_Target[1][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+4, LDO_OffDelay[1][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*6+5, LDO_OffDelay_Acc[1][i][nSiteIndex]);
		}

	}
	TMU_CH1.switchset(STARTtoA+STOPtoB);
	delayms(1);


	////
	//LDO2
	
	Reg[0x109a].write(_0x00);
	Reg[0x109b].write(_0x00);
	Reg[0x109c].write(_0x00);
	Reg[0x109d].set_clr_inv(BIT0);
	delayms(2);
	Reg[0x1012].write(0xf3);	
	delayms(5);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	TMU_CH1.arm(1);
	TMU_CH1.waitTRIG();
	delayms(1);
	Reg[0x1012].write(0x71);	
	delay(300);
	TMU_CH1.readdata();
	FOR_EACH_SITE(nSiteIndex)
		LPM_On_Delay_LDO_0[1][nSiteIndex]=meas_data[nSiteIndex+1];
	Reg[0x1012].write(0xf3);	
	delayms(5);

	for(i=0;i<16;i++)
	{
		
		delayms(2);
		Reg[0x109c].read(readback);
		FOR_EACH_SITE(nSiteIndex) 
			readback[nSiteIndex]=(0xf0&readback[nSiteIndex])|i;
		Reg[0x109c].write(readback);
		LPM_On_Delay_LDO_Target[1][i]=(0x0f&readback[0])*100;
		if(LPM_On_Delay_LDO_Target[1][i]==0) LPM_On_Delay_LDO_Target[1][i]=100;
		if((readback[0]&0x0f)==0x00)
			Reg[0x109c].set_clr_inv(BIT0,BIT1|BIT2|BIT3);
		
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LPM_On_Delay_LDO_CMI[1][i][nSiteIndex]=meas_data[nSiteIndex+1];
			LPM_On_Delay_LDO[1][i][nSiteIndex]=LPM_On_Delay_LDO_0[1][nSiteIndex]-LPM_On_Delay_LDO_CMI[1][i][nSiteIndex];
			LPM_On_Delay_LDO_Acc[1][i][nSiteIndex]=100*(LPM_On_Delay_LDO[1][i][nSiteIndex]-LPM_On_Delay_LDO_Target[1][i])/LPM_On_Delay_LDO_Target[1][i];
			
		}
		Reg[0x1012].write(0xf3);
		TestNO_target=433;
		FOR_EACH_SITE(nSiteIndex)
		{
			
			//pFunction->LogResult(NSITEINDEX, TestNO_target+0, LPM_On_Delay_LDO_0[0][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, TestNO_target+1, LPM_On_Delay_LDO_CMI[0][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3, LPM_On_Delay_LDO_Target[1][i]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3+1, LPM_On_Delay_LDO[1][i][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+i*3+2, LPM_On_Delay_LDO_Acc[1][i][nSiteIndex]);
			
		}
	}
	
		Reg[0x109d].set_clr_inv(BITNONE,BIT0);
	
		
	//New adding for bz
	////

	
	//LDO_SST		
	Reg[0x109b].write(_0x00);
	Reg[0x2095].write(0x0e);
	TMU_CH1.startset(1.5,UP);
	TMU_CH1.stopset(1.5,UP);
	for(i=0;i<8;i++)
	{
		Reg[0x1012].write(0xf3);	
		Reg[0x1095].write((0x03&Reg[0x1095].CMI)|(i<<2));    
		delayms(1);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delay(300);
		Reg[0x1012].write(0x71);
		delay(300);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			LDO_SST[i][1][nSiteIndex]=0.9*(LDO_OnDelay_0[1][nSiteIndex]-meas_data[nSiteIndex+1]);
			//pFunction->LogResult(NSITEINDEX, 45+i, LDO_SST[i][1][nSiteIndex]);
		}
	}
	
	Relay.BIT[K19_EXTEN_TMUB].clr();
	Relay.BIT[K24_LPM_TMUA].clr();
	
	//************LDO_SST**********


	//************Buck_Delay**********
	
	int Trigger_Set0[PMU_MAX]={0xf3,0xf3,0xf3,0xf3,0xfb},Trigger_Set1[PMU_MAX]={0x71,0xb1,0xd1,0xe1,0xf1};
	Relay.BIT[K18_EXTEN_TMUA].set();
	Relay.BIT[K25_LPM_TMUB].set();
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		

	

		

	}
	Fvi45.CH[FB1].fv(Buck_Vset[0], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
	delayms(1);
	Reg[0x109b].write(0xff);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		if(nTILE==0||nTILE==1)
		{
			Reg[0x2058].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x7f&readback[nSiteIndex];//B1&2
			Reg[0x2058].write(readback);
			
		}
		if(nTILE==2||nTILE==4)
		{
			Reg[0x2067].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];//B3&5
			Reg[0x2067].write(readback);
		}
		if(nTILE==3)
		{
			Reg[0x2077].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xdf&readback[nSiteIndex];//B4
			Reg[0x2077].write(readback);
		}
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(0x09);
		if(nTILE!=0)
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(0x40);
		Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BITNONE,BIT7|BIT6|BIT5|BIT4);
		Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BITNONE,BIT2|BIT1|BIT0);


		delayms(1);
		//Buck_on_delay_0
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			
			Buck_OnDelay_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
	
		//Buck_off_delay_0
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)		
			Buck_OffDelay_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];

		//Reg[GP_User_Buck_Addr[nTILE]+0x06].write_CMI();
		//Reg[GP_User_Buck_Addr[nTILE]+0x04].write_CMI();
		//if((Reg[GP_User_Buck_Addr[nTILE]+0x06].CMI&0xf0)==0x00)	
		//	Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
		//if((Reg[GP_User_Buck_Addr[nTILE]+0x04].CMI&0x07)==0x00)
			//Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT0,BIT1|BIT2);


		for(i=0;i<16;i++)
		{	
			Reg[0x1012].write(Trigger_Set0[nTILE]);
			Reg[GP_User_Buck_Addr[nTILE]+0x06].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0x0f&readback[nSiteIndex])|(i<<4);
			Reg[GP_User_Buck_Addr[nTILE]+0x06].write(readback);
			if(i==0)
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BIT4,BIT5|BIT6|BIT7);
				Buck_OnDelay_Target[nTILE][i]=100;
			}
			else
			Buck_OnDelay_Target[nTILE][i]=((0xf0&readback[0])>>4)*100;


			//Buck_on_delay_cmi
			delayms(1);
			TMU_CH1.startset(1.5,UP);
			TMU_CH1.stopset(1.5,UP);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(Trigger_Set1[nTILE]);
			delayms(1);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)	
			{
				Buck_OnDelay_CMI[nTILE][i][nSiteIndex]=meas_data[nSiteIndex+1];
				Buck_OnDelay[nTILE][i][nSiteIndex]=Buck_OnDelay_0[nTILE][nSiteIndex]-Buck_OnDelay_CMI[nTILE][i][nSiteIndex];
				Buck_OnDelay_Acc[nTILE][i][nSiteIndex]=100*(Buck_OnDelay[nTILE][i][nSiteIndex]-Buck_OnDelay_Target[nTILE][i])/Buck_OnDelay_Target[nTILE][i];
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+481, Buck_OnDelay_Target[nTILE][i]);
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+482, Buck_OnDelay[nTILE][i][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+483, Buck_OnDelay_Acc[nTILE][i][nSiteIndex]);
			}
			
		}
		/*
		//Buck_on_delay_cmi
		delayms(1);
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)	
			Buck_OnDelay_CMI[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
			*/
		for(i=0;i<8;i++)
		{	
			Reg[0x1012].write(Trigger_Set1[nTILE]);
			Reg[GP_User_Buck_Addr[nTILE]+0x04].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xf8&readback[nSiteIndex])|(i);
			Reg[GP_User_Buck_Addr[nTILE]+0x04].write(readback);
			if(i==0)
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x04].set_clr_inv(BIT0,BIT1|BIT2);
				Buck_OffDelay_Target[nTILE][i]=50;;
			}		
			else
			Buck_OffDelay_Target[nTILE][i]=(0x07&readback[0])*50;
			
			
			delayms(1);
			TMU_CH1.startset(1.5,DOWN);
			TMU_CH1.stopset(1.5,DOWN);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(Trigger_Set0[nTILE]);
			delayms(1);
			TMU_CH1.readdata();
			FOR_EACH_SITE(nSiteIndex)
			{
				Buck_OffDelay_CMI[nTILE][i][nSiteIndex]=meas_data[nSiteIndex+1];
				Buck_OffDelay[nTILE][i][nSiteIndex]=Buck_OffDelay_0[nTILE][nSiteIndex]-Buck_OffDelay_CMI[nTILE][i][nSiteIndex];
				Buck_OffDelay_Acc[nTILE][i][nSiteIndex]=100*(Buck_OffDelay[nTILE][i][nSiteIndex]-Buck_OffDelay_Target[nTILE][i])/Buck_OffDelay_Target[nTILE][i];
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+529, Buck_OffDelay_Target[nTILE][i]);
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+530, Buck_OffDelay[nTILE][i][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, i*3+72*nTILE+531, Buck_OffDelay_Acc[nTILE][i][nSiteIndex]);
			}
		}
		/*
		//Buck_off_delay_cmi
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		delayms(1);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
		{
			Buck_OffDelay_CMI[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
		}
		*/
		FOR_EACH_SITE(nSiteIndex)
		{
			
			

			
			//pFunction->LogResult(NSITEINDEX, 15*nTILE+53, Buck_OnDelay_0[nTILE][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 15*nTILE+54, Buck_OnDelay_CMI[nTILE][nSiteIndex]);
			
			//pFunction->LogResult(NSITEINDEX, 15*nTILE+58, Buck_OffDelay_0[nTILE][nSiteIndex]);
			//pFunction->LogResult(NSITEINDEX, 15*nTILE+59, Buck_OffDelay_CMI[nTILE][nSiteIndex]);
			
			

		}
		//New adding for BZ
		
		if(nTILE==0||nTILE==1)
		{
			Reg[0x2058].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x80|readback[nSiteIndex];//B1&2
			Reg[0x2058].write(readback);
			
		}
		if(nTILE==2||nTILE==4)
		{
			Reg[0x2067].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x20|readback[nSiteIndex];//B3&5
			Reg[0x2067].write(readback);		
		}
		if(nTILE==3)
		{
			Reg[0x2077].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x20|readback[nSiteIndex];//B4
			Reg[0x2077].write(readback);
		}
		Reg[GP_User_Buck_Addr[nTILE]+0x06].write(_0x00);	
		Reg[0x1012].write(Trigger_Set0[nTILE]);
		delayms(5);
		//Buck_on_delay_0
		TMU_CH1.startset(1.5,UP);
		TMU_CH1.stopset(1.5,UP);
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Reg[0x1012].write(Trigger_Set1[nTILE]);
		
		delayms(1);
		TMU_CH1.readdata();
		FOR_EACH_SITE(nSiteIndex)
			
			LPM_On_Delay_Buck_0[nTILE][nSiteIndex]=meas_data[nSiteIndex+1];
		
		for(i=0;i<16;i++)
		{
			Reg[0x1012].write(Trigger_Set0[nTILE]);	
			Reg[GP_User_Buck_Addr[nTILE]+0x06].read(readback);
			FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xf0&readback[nSiteIndex])|(i);
			Reg[GP_User_Buck_Addr[nTILE]+0x06].write(readback);
			if(i==0)
			{
				Reg[GP_User_Buck_Addr[nTILE]+0x06].set_clr_inv(BIT0,BIT3|BIT2|BIT1);
				LPM_On_Delay_Buck_Target[nTILE][i]=100;
			}
			else
			LPM_On_Delay_Buck_Target[nTILE][i]=((0x0f&readback[0]))*100;
				
			
			//Buck_on_delay_cmi		
			delayms(1);
			TMU_CH1.startset(1.5,UP);
			TMU_CH1.stopset(1.5,UP);
			TMU_CH1.arm(1);
			TMU_CH1.waitTRIG();
			delayms(1);
			Reg[0x1012].write(Trigger_Set1[nTILE]);
			delayms(1);
			TMU_CH1.readdata();
			
			FOR_EACH_SITE(nSiteIndex)
			{
				LPM_On_Delay_Buck_CMI[nTILE][i][nSiteIndex]=meas_data[nSiteIndex+1];
				LPM_On_Delay_Buck[nTILE][i][nSiteIndex]=LPM_On_Delay_Buck_0[nTILE][nSiteIndex]-LPM_On_Delay_Buck_CMI[nTILE][i][nSiteIndex];
				LPM_On_Delay_Buck_Acc[nTILE][i][nSiteIndex]=100*(LPM_On_Delay_Buck[nTILE][i][nSiteIndex]-LPM_On_Delay_Buck_Target[nTILE][i])/LPM_On_Delay_Buck_Target[nTILE][i];		
				
				//pFunction->LogResult(NSITEINDEX, 15*nTILE+63, LPM_On_Delay_Buck_0[nSiteIndex]);
				//pFunction->LogResult(NSITEINDEX, 15*nTILE+64, LPM_On_Delay_Buck_CMI[nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, i*3+48*nTILE+841, LPM_On_Delay_Buck_Target[nTILE][i]);
				pFunction->LogResult(NSITEINDEX, i*3+48*nTILE+842, LPM_On_Delay_Buck[nTILE][i][nSiteIndex]);
				pFunction->LogResult(NSITEINDEX, i*3+48*nTILE+843, LPM_On_Delay_Buck_Acc[nTILE][i][nSiteIndex]);	
				
			}
	
		}

		
		Reg[GP_User_Buck_Addr[nTILE]+0x06].write_CMI();
		Fvi45.CH[GP_FB_Buck[nTILE]].fi(0); 
		Fvi45.CH[GP_FB_Buck[nTILE]].open(); 
		//New adding for BZ	
		if(nTILE!=4)
		{
			Fvi45.CH[GP_FB_Buck[nTILE+1]].fv(1.00*Buck_Vset[nTILE+1], SITE_ALL, SET_CLAMP, 4, -4, 5, -5); 
			delayms(1);
		}
		Reg[GP_Factory_Buck_Addr[nTILE]+0x0d].write(_0x00);
		if(nTILE!=0)
			Reg[GP_Factory_Buck_Addr[nTILE]+0x0f].write(_0x00);
	}	
  	//************Power_off**********
	Relay.BIT[K47_TMU].clr();
	Relay.BIT[K18_EXTEN_TMUA].clr();
	Relay.BIT[K25_LPM_TMUB].clr();
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VBYP]      .fv(0);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[FB5]		.fv(0);

	Fvi16.CH[VIN_LDO1]  .open();
	Fvi16.CH[VIN_LDO2]  .open();
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();
	Fvi45.CH[FB5]		.open();
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Master_Timer_sub2
STT_TESTFUNC Master_Timer_sub2(CFunction *pFunction, int nSiteNo, int nTestFlag, Master_Timer_sub2_params* ours)
{
	double BrownOut_Timer[16][SITE_MAX]={0},BrownOut_Timer_Target[16]={0};
	Fvi45.CH[VBYP]		.fv(VIN_SET+1.5);
	Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	Fvi45.CH[VIN_3V3]		.fv(VIN_SET);	
	delayms(1);
	Relay.BIT[K47_TMU].set();
	Relay.BIT[K48_I2C].set();
	Relay.BIT[K18_EXTEN_TMUA].set();
	Relay.BIT[K22_GPIO1_TMUB].set();
	delayms(2);
	//i2c_init(HCTL_MODE,300000,2);
	//delayms(2);
	delayms(2);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(_0x00);
	Factory_Mode_Check();
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, Factory_Mode_Flag[nSiteIndex]);
	}	
	Additional_Settings();
	Reg[0x2008].write(0x2c);
	Reg[0x2007].write(0x0f);
	Reg[0x206d].write(0x09);
	Reg[0x206f].write(0x80);
	Fvi45.CH[VBYP]		.fv(0);
	Reg[0x1066].write(_0x00);
	Reg[0x1011].set_clr_inv(BIT7,BIT6);
	
	Reg[0x1043].set_clr_inv(BITNONE,BIT0);
	Reg[0x1053].set_clr_inv(BITNONE,BIT0);
	Reg[0x1063].set_clr_inv(BITNONE,BIT0);
	Reg[0x1073].set_clr_inv(BITNONE,BIT0);
	Reg[0x1083].set_clr_inv(BITNONE,BIT0);
	Reg[0x1093].write(0x88);
	
	Relay.BIT[K14_OUTLDO_Cap].set();
	delayms(2);
	FOR_EACH_TILE(nTILE,PMU_MAX)
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(3.3,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	Fvi16.CH[VIN_LDO1].fv(3.3);
	Fvi16.CH[VIN_LDO2].fv(3.3);
	
	FOR_EACH_TILE(nTILE,PMU_MAX)
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(1.05*Buck_Vset[nTILE],SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
	delayms(2);
	Reg[0x1012].write(_0x00);
	
	Reg[0x10f5].read(readback);
	int HBIT_Set=0,Value_Set=0; 
	double VOUTSW_BR_F_Target=0;
	HBIT_Set=(readback[0]>>7)&0x01;
	Value_Set=(readback[0]>>5)&0x03;
	
		VOUTSW_BR_F_Target=3.1-0.25*HBIT_Set-0.05*(1+HBIT_Set)*Value_Set;
	for(i=0;i<16;i++)
	{	
		Reg[0x1011].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=(0xc3&readback[nSiteIndex])|(i<<2);//B4
		Reg[0x1011].write(readback);
		delayms(2);
		BrownOut_Timer_Target[i]=100+i*50;
		if(i>8) BrownOut_Timer_Target[i]=500;
		
		
		TMU_CH1.stallset(Stall7V,TMU4mS,10);
		TMU_CH1.switchset(STARTtoA+STOPtoB);
		TMU_CH1.startset(1.5,DOWN);
		TMU_CH1.stopset(1.5,DOWN);
		
		
		TMU_CH1.arm(1);
		TMU_CH1.waitTRIG();
		delayms(1);
		Fvi45.CH[VOUT_SW]		.fv(VOUTSW_BR_F_Target-0.03); 
		delayms(1);
		TMU_CH1.readdata();
		Fvi45.CH[VOUT_SW]		.fv(VIN_SET);
	}
	
	//************Power_off**********
	Relay.BIT[K47_TMU].clr();
	Relay.BIT[K18_EXTEN_TMUA].clr();
	Relay.BIT[K22_GPIO1_TMUB].clr();
	Relay.BIT[K48_I2C].clr();
	delayms(2);
	Fvi45.CH[VBYP]      .fv(0);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);		
	Fvi45.CH[FB5]		.fv(0);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
	}

	Fvi16.CH[VIN_LDO1]  .open();
	Fvi16.CH[VIN_LDO2]  .open();
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]      .open();
	Fvi45.CH[FB5]		.open();
	return TRUE;
}// END_STT_TESTFUNC

// Test Function: Potential_checks
STT_TESTFUNC Potential_checks(CFunction *pFunction, int nSiteNo, int nTestFlag, Potential_checks_params* ours)
{
	//************Power_on**********
	Enter_Factory(true);
	delayms(1);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	Reg[0x1012].write(0xff);
	Reg[0x2007].write(_0x00);
	Additional_Settings();
	Fvi45.CH[VBYP]		.fv(0);
	delayms(2);

	//*****LDO Operating range*****
	double OUTLDO[LDO_MAX][SITE_MAX]={0};
	Relay.BIT[K14_OUTLDO_Cap].set();
	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();
	delayms(2);
	Fvi16.CH[VIN_LDO1].fv(3.3);
	Fvi16.CH[VIN_LDO2].fv(3.3);
	Reg[0x1093].write(0xcc);
	Reg[0x10f5].write(0xff);
	
	Fqvi.CH[OUT_LDO1].fi(-10,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
	Fqvi.CH[OUT_LDO2].fi(-10,SITE_ALL,SET_CLAMP,4,-4,0.04,-0.04);
	delayms(2);
	
	Reg[0x10f5].read(readback);
	Fvi45.CH[VOUT_SW].fv(3.64);
	Fvi16.CH[VIN_LDO1].fv(3.64);
	Fvi16.CH[VIN_LDO2].fv(3.64);
	delayms(2);
	Fqvi.CH[OUT_LDO1].mv(OUTLDO[0]);
	Fqvi.CH[OUT_LDO2].mv(OUTLDO[1]);
	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 1, OUTLDO[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 2, OUTLDO[1][nSiteIndex]);	
	}
	if(LDO_Vset[0]>2) 
	{
		Reg[0x1096].write(0x8c);
		Reg[0x1097].write(0x8c);
		Reg[0x1092].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x08|readback[nSiteIndex];
		Reg[0x1092].write(readback);
	}

	if(LDO_Vset[1]>2) 
	{
		Reg[0x1098].write(0x8c);
		Reg[0x1099].write(0x8c);
		Reg[0x1092].read(readback);
		FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0x04|readback[nSiteIndex];
		Reg[0x1092].write(readback);
	}

	Fvi45.CH[VOUT_SW].fv(2.5);
	Fvi16.CH[VIN_LDO1].fv(2.5);
	Fvi16.CH[VIN_LDO2].fv(2.5);
	delayms(2);
	Fqvi.CH[OUT_LDO1].mv(OUTLDO[0]);
	Fqvi.CH[OUT_LDO2].mv(OUTLDO[1]);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 3, OUTLDO[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 4, OUTLDO[1][nSiteIndex]);	
	}
	
	//VinLDO Operating range
	Reg[0x1092].write(_0x00);
	Reg[0x1096].write(_0x00);
	Reg[0x1097].write(_0x00);
	Reg[0x1098].write(_0x00);
	Reg[0x1099].write(_0x00);
	
	Fvi45.CH[VOUT_SW].fv(VIN_SET);
	Fvi16.CH[VIN_LDO1].fv(1.0);
	Fvi16.CH[VIN_LDO2].fv(1.0);
	
	delayms(2);
	Fqvi.CH[OUT_LDO1].mv(OUTLDO[0]);
	Fqvi.CH[OUT_LDO2].mv(OUTLDO[1]);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 5, OUTLDO[0][nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, 6, OUTLDO[1][nSiteIndex]);	
	}
	
	Fvi16.CH[VIN_LDO1].fv(0);
	Fvi16.CH[VIN_LDO1].fv(0);
	Fvi16.CH[VIN_LDO1].open();
	Fvi16.CH[VIN_LDO1].open();

	Fqvi.CH[OUT_LDO1].fi(0);
	Fqvi.CH[OUT_LDO2].fi(0);
	Fqvi.CH[OUT_LDO1].open();
	Fqvi.CH[OUT_LDO2].open();

	Fvi45.CH[VBYP]      .fv(0);	
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);	

	Fvi45.CH[VBYP]      .open();	
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();	

	Relay.BIT[K14_OUTLDO_Cap].clr();
	Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();
	SDASCL_Connect_VI();
	Fvi16.CH[SDA].fv(0);
	Fvi16.CH[SCL].fv(0);


	delayms(2);

	//*****VIN3V3 to AGND*****
	double I_VIN[SITE_MAX]={0},I_VOUTSW[SITE_MAX]={0},I_Vbyp[SITE_MAX]={0};
	
	Fvi45.CH[VIN_3V3].fv(4.0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	Fvi45.CH[VOUT_SW].fv(0.0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	delayms(100);
	Fvi45.CH[VIN_3V3].mi(I_VIN);
	//Fvi45.CH[VOUT_SW].mi(I_VOUTSW);
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 7, 1e3*I_VIN[nSiteIndex]);
	}
	
	Fvi45.CH[VOUT_SW].fv(4.0);
	delayms(100);
	//Fvi45.CH[VIN_3V3].mi(I_VIN);
	Fvi45.CH[VOUT_SW].mi(I_VOUTSW);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 8, 1e3*I_VOUTSW[nSiteIndex]);
	}

	Fvi45.CH[VBYP].fv(3.63,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
	delayms(20);
	//Fvi45.CH[VIN_3V3].mi(I_VIN);
	//Fvi45.CH[VOUT_SW].mi(I_VOUTSW);
	Fvi45.CH[VOUT_SW].mi(I_Vbyp);

	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, 9, 1e3*I_Vbyp[nSiteIndex]);
	}
	SDASCL_Connect_I2C();
	delayms(2);
	Fvi45.CH[VBYP]		.fv(0);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);	
	//Fvi45.CH[VOUT_SW]	.open();
	//Fvi45.CH[VIN_3V3]	.open();
	//Fvi45.CH[VBYP]		.open();
	delayms(100);

	//*****FBx to AGND*****
	double IOutBucks[PMU_MAX][SITE_MAX]={0};
	Enter_Factory(true);
	delayms(1);
	Reg[0x2000].read(readback);
	FOR_EACH_SITE(nSiteIndex) readback[nSiteIndex]=0xfc&readback[nSiteIndex];
	Reg[0x2000].write(readback);
	//Reg[0x1012].write(0xff);
	//Reg[0x2007].write(_0x00);
	Fvi45.CH[VBYP]		.fv(0);
	Factory_Mode_Check();
	delayms(100);
	Fvi45.CH[VOUT_SW]	.fv(4.0);
	FOR_EACH_TILE(nTILE,PMU_MAX)
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(4.0,SITE_ALL,SET_CLAMP,7,-7,4,-4);
	delayms(2);
	Reg[0x1044].write(0x80);
	Reg[0x1054].write(0x80);
	Reg[0x1064].write(0x80);
	Reg[0x1074].write(0x80);
	Reg[0x1084].write(0x80);
	//Reg[0x1094].write(0x88);
	Reg[0x1043].write(0x01);
	Reg[0x1053].write(0x01);
	Reg[0x1063].write(0x01);
	Reg[0x1073].write(0x01);
	Reg[0x1083].write(0x01);
	//Reg[0x1093].write(0x44);
	Reg[0x1045].write(_0x00);
	Reg[0x1055].write(_0x00);
	Reg[0x1065].write(_0x00);
	Reg[0x1075].write(_0x00);
	Reg[0x1085].write(_0x00);
	//Reg[0x1095].write(0x00);
	Reg[0x1043].read(readback);
	SDASCL_Connect_VI();
	delayms(100);
	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(4.0,SITE_ALL,SET_CLAMP,7,-7,40,-40);
		delayms(10);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(4.0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(2);
		Fvi45.CH[GP_FB_Buck[nTILE]].mi(IOutBucks[nTILE]);
		Fvi45.CH[GP_FB_Buck[nTILE]].fv(0.0,SITE_ALL,SET_CLAMP,7,-7,0.4,-0.4);
		delayms(2);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 10+nTILE, 1e3*IOutBucks[nTILE][nSiteIndex]);
		}
	}

	FOR_EACH_TILE(nTILE,PMU_MAX)
	{
		Fvi45.CH[GP_VIN_Buck[nTILE]].fv(0.0,SITE_ALL,SET_CLAMP,7,-7,4,-4);
		Fvi45.CH[GP_VIN_Buck[nTILE]].open();
		Fvi45.CH[GP_FB_Buck[nTILE]].open();
	}
	delayms(2);

	//*****OUTLDO to AGND*****
	double I_VinLDO[LDO_MAX][SITE_MAX]={0}, I_OutLDO[LDO_MAX][SITE_MAX]={0};
	Relay.BIT[K14_OUTLDO_Cap].set();
	Relay.BIT[K42_FQVI6].set();
	Relay.BIT[K43_FQVI7].set();
	SDASCL_Connect_I2C();
	delayms(2);
	Reg[0x1094].write(0x88);
	Reg[0x1093].write(0x44);
	SDASCL_Connect_VI();
	delayms(2);
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fvi16.CH[GP_INLDO[nTILE]].fv(4.3);
		delayms(20);
		Fvi16.CH[GP_INLDO[nTILE]].mi(I_VinLDO[nTILE]);
		Fqvi.CH[GP_OUTLDO[nTILE]].fv(4.3,SITE_ALL,SET_CLAMP,7,-7,0.04,-0.04);
		delayms(200);
		Fqvi.CH[GP_OUTLDO[nTILE]].mi(I_OutLDO[nTILE]);
		Fvi16.CH[GP_INLDO[nTILE]].fv(0);
		Fqvi.CH[GP_OUTLDO[nTILE]].fv(0);
		FOR_EACH_SITE(nSiteIndex)
		{
			pFunction->LogResult(NSITEINDEX, 15+nTILE*2, 1e3*I_VinLDO[nTILE][nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, 16+nTILE*2, 1e3*I_OutLDO[nTILE][nSiteIndex]);
		}
		
	}
	
	FOR_EACH_TILE(nTILE,LDO_MAX)
	{
		Fvi16.CH[GP_INLDO[nTILE]].open();
		Fqvi.CH[GP_OUTLDO[nTILE]].open();
	}
	
	SDASCL_Connect_I2C();
	Fvi45.CH[VBYP]		.fv(0);
	Fvi45.CH[VOUT_SW]	.fv(0);
	Fvi45.CH[VIN_3V3]	.fv(0);	
	Fvi45.CH[VOUT_SW]	.open();
	Fvi45.CH[VIN_3V3]	.open();
	Fvi45.CH[VBYP]		.open();
	delayms(2);
	Relay.BIT[K14_OUTLDO_Cap].clr();
	Relay.BIT[K42_FQVI6].clr();
	Relay.BIT[K43_FQVI7].clr();
	delayms(2);
	return TRUE;


}// END_STT_TESTFUNC
