

void measure_Trim_VBG()
{

	//===========================VINTQ===================================//
	Reg[0x10F3].set_clr_inv(BIT4,BIT5);	// DISABLE internal buffer
	Reg[0x10F2].write(0x09);	// VINTQ to GPIO9
	Reg[0x10f2].read(readback);
	delayms(2);
	Fvi16.CH[GPIO9].mv(VINTQ_1);		// VINTQ_1
	
	Reg[0x10F3].set_clr_inv(BIT5|BIT4,BIT6);	// Swap buffer to cancel offset
	Reg[0x10F2].write(0x08);	// VGB_VREG to GPIO9
	delayms(2);
	Fvi16.CH[GPIO9].mv(V1);		// VBG_VREG_1
	Reg[0x10F3].set_clr_inv(BIT5|BIT4|BIT6);	// Swap buffer to cancel offset
	Reg[0x10F2].write(0x08);	// VGB_VREG to GPIO9
	delayms(2);
	Fvi16.CH[GPIO9].mv(V2);		// VBG_VREG_1
	
	FOR_EACH_SITE(nSiteIndex)
	{
		global_temp[nSiteIndex]=VINTQ_1[nSiteIndex] - (V1[nSiteIndex]+V2[nSiteIndex])/2;   // VBG_VREG
	}
}

bool Trim_VBG(CFunction *pFunction,int TestNO_target)
{ 

	double VAOQ_1[SITE_MAX],VAOQ[SITE_MAX],VAOQ_2[SITE_MAX];
	double VINTQ_1[SITE_MAX], VINTQ[SITE_MAX];
	double VREG[SITE_MAX];


	Fvi16.CH[GPIO9]	    .fi(0,SITE_ALL,Stall3V3,Stall50uA); // use to measure voltage
	delayms(2);	
	Reg[0x100A].write(0xAA);
	delayms(2);
	Reg[0x100A].read(readback);
	//===========================VAOQ===================================//
	Reg[0x10FD].write(0xA0); // Force ON BFET
	Reg[0x10FD].read(readback);
	delayms(1);	
	Relay.BIT[K19_GPIO9_AMUX].set(); // Turn on GPIO9 AMUX buffer
	delayms(2);
	Reg[0x10F3].set_clr_inv(BIT4|BIT5,BIT6);
	Reg[0x10F2].write(0x06);	// VAOQ to GPIO9
	Reg[0x10F2].read(readback);
	delayms(2);
	Fvi16.CH[GPIO9].mv(VAOQ_1);		// VAOQ_1
	Reg[0x10F3].set_clr_inv(BIT6|BIT5|BIT4);	// Swap buffer to cancel offset
	delayms(2);	
	Fvi16.CH[GPIO9].mv(VAOQ_2);		// VAOQ_2
//	delayms(2);	

	FOR_EACH_SITE(nSiteIndex)
	{
		VAOQ[nSiteIndex] = 3*(VAOQ_1[nSiteIndex]+VAOQ_2[nSiteIndex])/2; // VAOQ is divided by 3 and meansured in GPIO9 to ensure that INTERNAL BUFFER is used 
		pFunction->LogResult(NSITEINDEX, TestNO_target, VAOQ[nSiteIndex]);
		VAOQ_global[nSiteIndex]=VAOQ[nSiteIndex];
	}

		//===========================VAOD===================================//
//	Reg[0x10C0].write(0x80); // Force ON BFET
//	delayms(2);	
	Reg[0x10F2].write(0x05);	// VAOD to GPIO9
	delayms(2);
	Fvi16.CH[GPIO9].mv(VAOD);		// VAOQ_1
//	Reg[0x10F3].set_clr_inv(BIT6|BIT4);	// Swap buffer to cancel offset
//	delayms(10);	
//	Fvi16.CH[GPIO9].mv(VAOQ_2);		// VAOQ_2
//	delayms(2);	

	FOR_EACH_SITE(nSiteIndex)
	{
		VAOD[nSiteIndex] = 3*VAOD[nSiteIndex]; // VAOQ is divided by 3 and meansured in GPIO9 to ensure that INTERNAL BUFFER is used 
	}


	//===========================VINTQ===================================//
	Reg[0x10F3].set_clr_inv(BIT4,BIT5|BIT6);
	Reg[0x10F2].write(0x09);	// VINTQ to GPIO9
	delayms(2);
	Fvi16.CH[GPIO9].mv(VINTQ_1);		// VINTQ_1
//	Reg[0x10F3].set_clr_inv(BIT4,BIT6); // En AMUX & Dis AMUX BUF
//	delayms(2);	
//	Fvi16.CH[GPIO9].mv(VINTQ_2);		// VINTQ_2

	FOR_EACH_SITE(nSiteIndex)
	{
		VINTQ[nSiteIndex] = (VINTQ_1[nSiteIndex]) ;
		pFunction->LogResult(NSITEINDEX, TestNO_target+1, VINTQ[nSiteIndex]);
	}
	
	//===========================VBG_VREG===================================//
	Reg[0x100A].read(readback);
	Reg[0x10F0].read(readback);
	//=========variable define==========//
	int print_index=TestNO_target+8; 
	double VBG_Target = 1.206; //TM changed from 1.2 to 1.206 on 12/3

	FOR_EACH_SITE(nSiteIndex)  // copy target avoid QA fail
	{
		global_Target_array[nSiteIndex]=VBG_Target;	
	}

	//===========================write default trim code========================//
	if(QC_Flag==0)
	{	
		Reg[0x1040].set_clr_inv(BITNONE,BIT7|BIT6|BIT5|BIT4|BIT3);		
	}
	delayms(2);

	//===========================measure pre value===================================//
	measure_Trim_VBG();
	

	FOR_EACH_SITE(nSiteIndex)
	{
		VREG[nSiteIndex]=V1[nSiteIndex];
		global_Pre[nSiteIndex]=global_temp[nSiteIndex];
		global_post[nSiteIndex]=global_temp[nSiteIndex];
	}
	//Read trim code for QA print
	Reg[0x1040].read(trimcode);
	FOR_EACH_SITE(nSiteIndex)
		trimcode[nSiteIndex]=(trimcode[nSiteIndex]&0xf8)>>3;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_start++++++++++++++++++++++++++++++++++++++++++++++++++++//
	if(QC_Flag==0)
	{	
		//=========assign trim table and trim address==========//
		global_Trim_table_size=32;//--> set table size
		unsigned char code_queue[32] = {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
		int ratio_queue[32] =        {-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
		
		global_trimReg_addr=0x1040;//--> set trim address
		global_BitHigh=7,global_BitLow=3;//--> set trim bits
		FOR_EACH_SITE(nSiteIndex)
		{			
			global_LSB=0.003;//mV
			global_Target_array[nSiteIndex]=VBG_Target;
			global_Pre[nSiteIndex]=global_Pre[nSiteIndex];
		}
		Reg[0x1040].read(readback);
		FOR_EACH_SITE(nSiteIndex)
		{	
			Global_trimcode[nSiteIndex]=get_trim_code_LookupTable(global_Target_array[nSiteIndex],global_Pre[nSiteIndex], global_LSB, global_Trim_table_size, code_queue, ratio_queue, 
																	  global_BitHigh, global_BitLow, global_trimReg_addr ,DIFF);
		}
	
		//===========================measure post value===================================//
		measure_Trim_VBG();
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
				}
			}
			if((global_retrim_Flag[SITE1]==1)||(global_retrim_Flag[SITE2]==1))
			{
				//===========================measure post value===================================//
				measure_Trim_VBG();//measure value
				FOR_EACH_SITE(nSiteIndex)
				{
					if (global_retrim_Flag[nSiteIndex]==1)
					{
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
				//---print internal_post1 and internal post2 and retrim count------------//
				pFunction->LogResult(NSITEINDEX, print_index, store_Post_value_array[global_count_array[nSiteIndex]-1]);
				pFunction->LogResult(NSITEINDEX, print_index+1, store_Post_value_array[global_count_array[nSiteIndex]]);
				pFunction->LogResult(NSITEINDEX, print_index+2, global_count_array[nSiteIndex]);
			}
		}
	}  
	//++++++++++++++++++++++++++++++++++++++++++++++++++++ Trim_flow_end+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
	//power off
//	Reg[0x10F3].write_CMI();// set to default buffer setting 
//	Reg[0x10F2].write_CMI();// set to default AMUX  
//		Reg[0x1040].read(readback);
//	Relay.BIT[K19_GPIO9_AMUX].clr(); // Turn off GPIO9 AMUX buffer
	delayms(2);	
	FOR_EACH_SITE(nSiteIndex)
	{
		pFunction->LogResult(NSITEINDEX, TestNO_target+2, VREG[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+3, global_Target_array[nSiteIndex]);
		if(QC_Flag==0)
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+4, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+5, Global_trimcode[nSiteIndex]);
			if(global_count_array[nSiteIndex]==0)
			{
				pFunction->LogResult(NSITEINDEX, TestNO_target+8, 0);
				pFunction->LogResult(NSITEINDEX, TestNO_target+9, 0);
				pFunction->LogResult(NSITEINDEX, TestNO_target+10, 0);
			}
			global_count_array[nSiteIndex]=0;
		}
		else
		{
			pFunction->LogResult(NSITEINDEX, TestNO_target+4, global_Pre[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+5, trimcode[nSiteIndex]);
			pFunction->LogResult(NSITEINDEX, TestNO_target+8, 0);
			pFunction->LogResult(NSITEINDEX, TestNO_target+9, 0);
			pFunction->LogResult(NSITEINDEX, TestNO_target+10, 0);
		}
		pFunction->LogResult(NSITEINDEX, TestNO_target+6, global_post[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+11, VINTQ_1[nSiteIndex]);
		pFunction->LogResult(NSITEINDEX, TestNO_target+12, V1[nSiteIndex]);

		pFunction->LogResult(NSITEINDEX, TestNO_target+7, 100*((global_post[nSiteIndex]-global_Target_array[nSiteIndex])/global_Target_array[nSiteIndex]));
//		pFunction->LogResult(NSITEINDEX, TestNO_target+10, VINTQ[nSiteIndex]-global_post[nSiteIndex]); // VBANDGAP
	}

	return true;
}

//=========================== Measure VPTAT_OFS ===========================//
void measure_Trim_VPTAT_OFS()
{
    double VN1[SITE_MAX], VN2[SITE_MAX], VP1[SITE_MAX], VP2[SITE_MAX];
    double VN[SITE_MAX], VP[SITE_MAX];

    // Enable AMUX + AMUX BUF
    Reg[0x10F3].set_clr_inv(BIT4|BIT5, BITNONE); // F3[5:4] = 0b11

    // Force trim code = 0
    Reg[0x105B].set_clr_inv(BITNONE, BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);

    // Bring VN to GPIO9
    Reg[0x10F2].write(0x0F);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VN1);

    // Swap buffer to cancel offset
    Reg[0x10F3].set_clr_inv(BIT6, BITNONE);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VN2);

    FOR_EACH_SITE(nSiteIndex)
        VN[nSiteIndex] = (VN1[nSiteIndex] + VN2[nSiteIndex]) / 2;

    // Bring VP to GPIO9
    Reg[0x10F2].write(0x0E);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VP1);

    // Swap buffer
    Reg[0x10F3].set_clr_inv(BIT6, BITNONE);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VP2);

    FOR_EACH_SITE(nSiteIndex)
        VP[nSiteIndex] = (VP1[nSiteIndex] + VP2[nSiteIndex]) / 2;

    FOR_EACH_SITE(nSiteIndex)
        global_temp[nSiteIndex] = VP[nSiteIndex] - VN[nSiteIndex];
}

//=========================== Measure VPTAT ===========================//
void measure_Trim_VPTAT()
{
    double VPTAT_1[SITE_MAX], VPTAT_2[SITE_MAX];

    // Enable AMUX + AMUX BUF
    Reg[0x10F3].set_clr_inv(BIT4|BIT5, BITNONE); // F3[5:4] = 0b11

    // Bring VPTAT to GPIO9
    Reg[0x10F2].write(0x0A);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VPTAT_1);

    // Swap buffer
    Reg[0x10F3].set_clr_inv(BIT6, BITNONE);
    delayms(2);
    Fvi16.CH[GPIO9].mv(VPTAT_2);

    FOR_EACH_SITE(nSiteIndex)
        global_temp[nSiteIndex] = (VPTAT_1[nSiteIndex] + VPTAT_2[nSiteIndex]) / 2;
}

//=========================== Trim VPTAT ===========================//
bool Trim_VPTAT(CFunction *pFunction, int TestNO_target)
{
    double VPTAT_OFS[SITE_MAX];
    double VPTAT[SITE_MAX];
    double VPTAT_Target = 1.2; 

    // Đo VPTAT_OFS (VP - VN)
    measure_Trim_VPTAT_OFS();
    FOR_EACH_SITE(nSiteIndex)
    {
        VPTAT_OFS[nSiteIndex] = global_temp[nSiteIndex];
        pFunction->LogResult(NSITEINDEX, TestNO_target, VPTAT_OFS[nSiteIndex]);
    }

    // Ghi mã trim ban đầu (0)
    if(QC_Flag == 0)
        Reg[0x105B].set_clr_inv(BITNONE, BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);

    // Đo lại VPTAT (sau offset)
    measure_Trim_VPTAT();
    FOR_EACH_SITE(nSiteIndex)
    {
        VPTAT[nSiteIndex] = global_temp[nSiteIndex];
        global_Pre[nSiteIndex] = VPTAT[nSiteIndex];
    }

    // Cấu hình bảng tra
    global_Trim_table_size = 64;
    global_trimReg_addr = 0x105B;
    global_BitHigh = 5;
    global_BitLow = 0;
    double LSB = 0.003; 
    unsigned char code_queue[64];
    int ratio_queue[64];
    for (int i = 0; i < 64; ++i) { code_queue[i] = i; ratio_queue[i] = i; }

    FOR_EACH_SITE(nSiteIndex)
    {
        global_Target_array[nSiteIndex] = VPTAT_Target;
        Global_trimcode[nSiteIndex] = get_trim_code_LookupTable(global_Target_array[nSiteIndex], global_Pre[nSiteIndex], LSB, global_Trim_table_size,
                                                                 code_queue, ratio_queue, global_BitHigh, global_BitLow, global_trimReg_addr, DIFF);
    }

    // Đo lại VPTAT sau trim
    measure_Trim_VPTAT();
    FOR_EACH_SITE(nSiteIndex)
        global_post[nSiteIndex] = global_temp[nSiteIndex];

    // Tìm mã trim tối ưu nếu cần
    FOR_EACH_SITE(nSiteIndex)
    {
        search_Best_trim_code(global_Target_array[nSiteIndex], global_count_array[nSiteIndex], &global_post[nSiteIndex], &Global_trimcode[nSiteIndex],
                               global_BitHigh, global_BitLow, global_trimReg_addr);
    }

    // Log kết quả
    FOR_EACH_SITE(nSiteIndex)
    {
        pFunction->LogResult(NSITEINDEX, TestNO_target + 1, global_post[nSiteIndex]);
        pFunction->LogResult(NSITEINDEX, TestNO_target + 2, global_Pre[nSiteIndex]);
        pFunction->LogResult(NSITEINDEX, TestNO_target + 3, Global_trimcode[nSiteIndex]);
        pFunction->LogResult(NSITEINDEX, TestNO_target + 4, global_Target_array[nSiteIndex]);
        pFunction->LogResult(NSITEINDEX, TestNO_target + 5, 100 * (global_post[nSiteIndex] - global_Target_array[nSiteIndex]) / global_Target_array[nSiteIndex]);
    }

    return true;
}
