/*********************************************************************
  MODULE:	QC MENU & TESTS

  FILE:		QCTests.c

  DATE:		01/25/07

  ANALYSIS:	 qc tests

  CALLED BY:
		enhanced_tests


  *************************************************************************/
#include "crc.h"
#include "menu.h"
#include "keyboard.h"
#include "i2c.h"
#include "screen.h"
#include "printer.h"
#include "uart.h"
#include "message.h"
#include "nuc.h"
#include <string.h>
	typedef struct qcmeas QCMEAS;
	struct qcmeas
	{
		float meas;
		float result;
		char actstr[12];
	};

QCMEAS qcmeas[6];


static void qc_1_strip(void);
static void	qc_2_strips(void);
static void	qc_hmpao(void);
static void	qc_mag3(void);
static	void display_qc_1_strip(void);
static	void display_qc_2_strips(void);
static	void display_qc_hmpao(void);
static	void display_qc_mag3(void);
static short format_result(short index,char *buffer);
static void display_result(short index, short msg_num, short line);
static  void pr_qc_1_strip(char prtype);
static  void pr_qc_2_strips(char prtype);
static void print_actstr(short index, short msg_num, char prtype);
static void print_result(short index, short msg_num, char prtype);
static void print_result_head(char prtype);
static   void pr_qc_mag3(char prtype);
static  void pr_qc_hmpao(char prtype);
static void pr_1_radiopharm(short msg_num,char prtype);
static bool print_qc(short type);
static void pr_radiopharm(char prtype);
static void save_qc_meas(short nmeas);

enum QCTESTS
{
    QC_1_STRIP,
    QC_2_STRIPS,
    QC_HMPAO,
    QC_MAG3,

};

/*static const char *report_header[] =
 -     {
 -         "Single Strip / Single Solvent",
 -         "Two Strips / Two Solvents",
 -         "Tc99m HMPAO (Exametazine)",
 -         "Tc99m MAG3"
 -     };*/


static const short report_header[] =
    {
        QC_61,
        QC_62,
        QC_63,
        QC_64,
    };

    extern CURRENT current;

    void qc_menu(void)
	{

        short iret;

        iret = display_menu(QC_MENU);
        if(home_set())
            return;

        switch (iret)
        {

        case -1:  //MENU button
            return;
        case 1:
            qc_1_strip();
            break;
        case 2:
            qc_2_strips();
            break;
        case 3:
            qc_hmpao();
            break;
        case 4:
		    qc_mag3();
		    break;
		}
	}




    static void qc_1_strip(void)
    {
		short nmeas;
        float sum;
        short tc_index;
        NUCDATA  nucdata;
        char nucname[8];

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

		/* nuclide is Tc */
        //get Tc99m index
        strcpy(nucname,"Tc99m");
        tc_index = NuclideData_getIndexFromName("Tc99m");
		NuclideData_getNuclide(tc_index,&nucdata);

		for(;;)
		{
			for(nmeas = 0; nmeas < 2; nmeas++)
			{
				erase_screen();
				if(nmeas ==0)
                    //display_text(0,8,"Place TOP of Strip",0,SMALL,NORMAL);
                    display_small_message(QC_1,8,0,NORMAL);
				else
                    //display_text(0,8,"Place BOTTOM of Strip",0,SMALL,NORMAL);
                    display_small_message(QC_2,8,0,NORMAL);
    			//display_text(0,16,"in Chamber",0,SMALL,NORMAL);
                display_small_message(QC_3,16,0,NORMAL);

				contmsg();
                if(home_set())
                    return;

				erase_screen();
                measure_activity(nucname);
                if(home_set())
                    return;

				save_qc_meas(nmeas);


			} /* end nmeas loop */


			/* calculate results */
			sum = qcmeas[0].meas + qcmeas[1].meas;
			qcmeas[0].result = 100. * qcmeas[0].meas / sum;
			qcmeas[1].result = 100. * qcmeas[1].meas / sum;

			/* display summary */
			display_qc_1_strip();
            if(home_set())
                return;

	        //print report -- return if no more tests
            if(!print_qc(QC_1_STRIP) || home_set())
				return;
		}


	}



static	void pr_qc_1_strip(char prtype)
	{


		//print_actstr(0,"Top    of Strip: ",prtype);
		//print_actstr(1,"Bottom of Strip: ",prtype);
        //print_actstr(0,QC_31,prtype);
        //print_actstr(1,QC_32,prtype);
        print_actstr(0,L_TOP_OF_STRIP2,prtype);    // "Top of Strip:"
        print_actstr(1,L_BOTTOM_OF_STRIP2,prtype);    // "Bottom of Strip:"

        print_result_head(prtype);

        //print_result(0,"Top    / (Top + Bottom): ",prtype);
		//print_result(1,"Bottom / (Top + Bottom): ",prtype);
        //print_result(0,QC_33,prtype);
		print_result(0,L_TOP_TOP_BOTTOM,prtype);    // "Top    / (Top + Bottom): "
        //print_result(1,QC_34,prtype);
		print_result(1,L_BOTTOM_TOP_BOTTOM,prtype);    // "Bottom / (Top + Bottom): "

		formfeed(prtype);

	}


static	void display_qc_1_strip(void)
	{

		erase_screen();
		//display_text(48,0,"QC TEST",0,SMALL,NORMAL);
		//display_text(12,8,"Single Strip",0,SMALL,NORMAL);
        display_small_message(QC_4,0,0,NORMAL);
        display_small_message(QC_5,8,0,NORMAL);

        //display_result(0,"Top   /(Top+Bottom)",2);
		//display_result(1,"Bottom/(Top+Bottom)",4);
        display_result(0,QC_6,2);
        display_result(1,QC_7,4);

        contmsg();

	}

static void pr_radiopharm(char prtype)
    {

        //strcpy(str1,"Radiopharmaceutical: ____________");
        pr_1_radiopharm(L_QC_RADIOPHARMACEUTICAL,prtype);    // "Radiopharmaceutical: _________"

        //strcpy(str1,"Lot #: __________________________");
        pr_1_radiopharm(L_QC_LOT,prtype);    // "Lot #: _______________________"

        //strcpy(str1,"Kit #: __________________________");
        pr_1_radiopharm(L_QC_KIT,prtype);    // "Kit #: _______________________"

        feed(1,prtype);

    }

//static void pr_1_radiopharm(char *str,char prtype)
static void pr_1_radiopharm(short msg_num,char prtype)
    {
        char strng[90];
		char message[50];

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[1],str,strlen(str));
        //get_message(msg_num,&strng[1]);
		get_amulet_message(msg_num,message);
		strncpy(&strng[1],message,strlen(message));
        pr_write(strng);

    }

static void	qc_2_strips(void)

	{

		short nmeas;
        float sum;
        short tc_index;
        char nucname[8];
        NUCDATA  nucdata;

        // Removed: System is read at the beginning
		//EE_READ(syst, &current.system);

		/* nuclide is Tc */
        //get Tc99m index
        strcpy(nucname,"Tc99m");
        tc_index = NuclideData_getIndexFromName("Tc99m");
		NuclideData_getNuclide(tc_index,&nucdata);

		for(;;)
		{
			for(nmeas = 0; nmeas < 4; nmeas++)
			{
				erase_screen();
				if(nmeas == 0 || nmeas == 2)
                    //display_text(0,0,"Place TOP of",0,SMALL,0);
                    display_small_message(QC_8,8,0,NORMAL);
				else
                    //display_text(0,8,"Place BOTTOM of",0,SMALL,0);
                    display_small_message(QC_9,8,0,NORMAL);
				if(nmeas == 0 || nmeas == 1)
                    //display_text(0,16,"Strip A in Chamber",0,SMALL,0);
                    display_small_message(QC_10,16,0,NORMAL);
				else
                    //display_text(0,16,"Strip B in Chamber",0,SMALL,0);
                    display_small_message(QC_11,16,0,NORMAL);
				contmsg();
                if(home_set())
                    return;

				erase_screen();
				measure_activity(nucname);

                if(home_set())
                    return;
                save_qc_meas(nmeas);



			} /* end nmeas loop */


			/* calculate results */
			sum = qcmeas[0].meas + qcmeas[1].meas;
			qcmeas[0].result = 100. * qcmeas[0].meas / sum;
			sum = qcmeas[2].meas + qcmeas[3].meas;
			qcmeas[1].result = 100. * qcmeas[3].meas / sum;
			qcmeas[2].result = 100. - (qcmeas[0].result + qcmeas[1].result);

			/* display summary */
			display_qc_2_strips();
            if(home_set())
                return;

	        //print report -- break if no more tests
            if(!print_qc(QC_2_STRIPS)|| home_set())
				return;
		}


	}




static	void pr_qc_2_strips(char prtype)
	{

		char strng[90];
		char message[40];


		//print_actstr(0,"Top    of Strip A: ",prtype);
		//print_actstr(1,"Bottom of Strip A: ",prtype);
		//print_actstr(2,"Top    of Strip B: ",prtype);
		//print_actstr(3,"Bottom of Strip B: ",prtype);
        //print_actstr(0,QC_35,prtype);
        //print_actstr(1,QC_36,prtype);
        //print_actstr(2,QC_37,prtype);
        //print_actstr(3,QC_38,prtype);
        print_actstr(0,L_TOP_OF_STRIP_A2,prtype);    // "Top of Strip A:"
        print_actstr(1,L_BOTTOM_OF_STRIP_A2,prtype);    // "Bottom of Strip A:"
        print_actstr(2,L_TOP_OF_STRIP_B2,prtype);    // "Top of Strip B:"
        print_actstr(3,L_BOTTOM_OF_STRIP_B2,prtype);    // "Bottom of Strip B:"

        print_result_head(prtype);

		//print_result(0,"Strip A: Free          = ",prtype);
		//print_result(1,"Strip B: R / H         = ",prtype);
		//print_result(2,"Purity: 100 -(F + R/H) = ",prtype);
        //print_result(0,QC_39,prtype);
        //print_result(1,QC_40,prtype);
        //print_result(2,QC_41,prtype);
        print_result(0,L_STRIP_A_FREE,prtype);    // "Strip A: Free          = "
        print_result(1,L_STRIP_B_R_H,prtype);    // "Strip B: R / H         = "
        print_result(2,L_PURITY_F_R_H,prtype);    // "Purity: 100 -(F + R/H) = "

		feed(1,prtype);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"F = Free",8);
        //get_message(QC_42,&strng[1]);
		get_amulet_message(L_F_FREE,message);    // "F = Free"
		strncpy(&strng[1],message,strlen(message));
        pr_write(strng);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"R = Reduced",11);
        //get_message(QC_43,&strng[1]);
		get_amulet_message(L_R_REDUCED,message);    // "R = Reduced"
		strncpy(&strng[1],message,strlen(message));
        pr_write(strng);

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"H = Hydrolized",14);
        //get_message(QC_44,&strng[1]);
		get_amulet_message(L_H_HYDROLIZED,message);    // "H = Hydrolized"
		strncpy(&strng[1],message,strlen(message));
        pr_write(strng);

		formfeed(prtype);

	}

static	void display_qc_2_strips(void)
	{

		erase_screen();
		//display_text(0,0,"QC TEST   Two Strips",0,SMALL,NORMAL);
        display_small_message(QC_12,0,0,NORMAL);

		//display_result(0,"Free Tc99m",1);
		//display_result(1,"Reduced/Hydrolized",3);
		//display_result(2,"Purity",5);
        display_result(0,QC_13,1);
        display_result(1,QC_14,3);
        display_result(2,QC_15,5);

		contmsg();

	}

static void	qc_hmpao(void)
	{


		short nmeas;
		float sum;
        short tc_index;
        char nucname[8];
        NUCDATA  nucdata;

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

		/* nuclide is Tc */
        //get Tc99m index
        strcpy(nucname,"Tc99m");
        tc_index = NuclideData_getIndexFromName("Tc99m");
        NuclideData_getNuclide(tc_index,&nucdata);

		for(;;)
		{
			for(nmeas = 0; nmeas < 6; nmeas++)
			{
				erase_screen();
				if(nmeas == 0 || nmeas == 2 || nmeas == 4)
                    //display_text(0,8,"Place TOP of",0,SMALL,NORMAL);
                    display_small_message(QC_8,8,0,NORMAL);
				else
                    //display_text(0,8,"Place BOTTOM of",0,SMALL,NORMAL);
                    display_small_message(QC_9,8,0,NORMAL);
				if(nmeas == 0 || nmeas == 1)
                    //display_text(0,16,"Strip A in Chamber",0,SMALL,NORMAL);
                    display_small_message(QC_10,16,0,NORMAL);
				else if(nmeas == 2 || nmeas == 3)
                    //display_text(0,16,"Strip B in Chamber",0,SMALL,NORMAL);
                    display_small_message(QC_11,16,0,NORMAL);
				else
                    //display_text(0,16,"Strip C in Chamber",0,SMALL,NORMAL);
                    display_small_message(QC_16,16,0,NORMAL);
				contmsg();
                if(home_set())
                    return;

				erase_screen();

                measure_activity(nucname);
                if(home_set())
                    return;

				save_qc_meas(nmeas);



			} /* end nmeas loop */


			/* calculate results */
			sum = qcmeas[0].meas + qcmeas[1].meas;
			qcmeas[0].result = 100. * qcmeas[0].meas / sum;
			sum = qcmeas[2].meas + qcmeas[3].meas;
			qcmeas[1].result = 100. * qcmeas[3].meas / sum;
			sum = qcmeas[4].meas + qcmeas[5].meas;
			qcmeas[2].result = 100. * qcmeas[4].meas / sum -
				   qcmeas[0].result;

			/* display summary */
			display_qc_hmpao();
            if(home_set())
                return;

	        //print report -- break if no more tests
            if(!print_qc(QC_HMPAO) || home_set())
				return;
		}


	}



static	void pr_qc_hmpao(char prtype)
	{

/*		print_actstr(0,"Top    of Strip A: ",prtype);
 - 		print_actstr(1,"Bottom of Strip A: ",prtype);
 - 		print_actstr(2,"Top    of Strip B: ",prtype);
 - 		print_actstr(3,"Bottom of Strip B: ",prtype);
 - 		print_actstr(4,"Top    of Strip C: ",prtype);
 - 		print_actstr(5,"Bottom of Strip C: ",prtype);*/
/*        print_actstr(0,QC_35,prtype);
 -         print_actstr(1,QC_36,prtype);
 -         print_actstr(2,QC_37,prtype);
 -         print_actstr(3,QC_38,prtype);
 -         print_actstr(4,QC_45,prtype);
 -         print_actstr(5,QC_46,prtype);*/
        print_actstr(0,L_TOP_OF_STRIP_A2,prtype);    // "Top of Strip A:"
        print_actstr(1,L_BOTTOM_OF_STRIP_A2,prtype);    // "Bottom of Strip A:"
        print_actstr(2,L_TOP_OF_STRIP_B2,prtype);    // "Top of Strip B:"
        print_actstr(3,L_BOTTOM_OF_STRIP_B2,prtype);    // "Bottom of Strip B:"
        print_actstr(4,L_TOP_OF_STRIP_C2,prtype);    // "Top of Strip C:"
        print_actstr(5,L_BOTTOM_OF_STRIP_C2,prtype);    // "Bottom of Strip C:"


        print_result_head(prtype);

		//print_result(0,"Free Tc99m              :",prtype);
		//print_result(1,"Hydrolized/Reduced Tc99m:",prtype);
		//print_result(2,"Tc99m HMPAO             :",prtype);
/*        print_result(0,QC_47,prtype);
 -         print_result(1,QC_48,prtype);
 -         print_result(2,QC_49,prtype);*/
        print_result(0,L_FREE_TC99M,prtype);    // "Free Tc99m              :"
        print_result(1,L_HYDROLIZED_REDUCED_TC99M,prtype);    // "Hydrolized/Reduced Tc99m:"
        print_result(2,L_TC99M_HMPAO,prtype);    // "Tc99m HMPAO             :"

		formfeed(prtype);

	}

static	void display_qc_hmpao(void)
	{

		erase_screen();

        //display_text(24,0,"QC TEST  HMPAO",0,SMALL,NORMAL);
        display_small_message(QC_17,0,0,NORMAL);

		//display_result(0,"Free Tc99m",1);
		//display_result(1,"Hydrolized/Reduced",3);
		//display_result(2,"Tc99m HMPAO",5);
        display_result(0,QC_18,1);
        display_result(1,QC_19,3);
        display_result(2,QC_20,5);

		contmsg();

	}

    static void	qc_mag3(void)

	{


		short nmeas;
		float sum;
        short tc_index;
        NUCDATA nucdata;
        char nucname[8];

        // Removed: System is read at the beginning
        //EE_READ(syst, &current.system);

		/* nuclide is Tc */
        //get Tc99m index
        strcpy(nucname,"Tc99m");
        tc_index = NuclideData_getIndexFromName("Tc99m");
        NuclideData_getNuclide(tc_index,&nucdata);

		for(;;)
		{
			for(nmeas = 0; nmeas < 3; nmeas++)
			{
				erase_screen();
				switch(nmeas)
				{
				case 0:
					//display_text(0,8,"Place Fraction #1",0,SMALL,NORMAL);
                    display_small_message(QC_21,8,0,NORMAL);
					break;
				case 1:
					//display_text(0,8,"Place Fraction #2",0,SMALL,NORMAL);
                    display_small_message(QC_22,8,0,NORMAL);
					break;
				case 2:
					//display_text(0,8,"Place Cartridge",0,SMALL,NORMAL);
                    display_small_message(QC_23,8,0,NORMAL);
					break;
				}
				//display_text(0,16,"in Chamber",0,SMALL,NORMAL);
                display_small_message(QC_3,16,0,NORMAL);

				contmsg();
                if(home_set())
                    return;

				erase_screen();
                measure_activity(nucname);
                if(home_set())
                    return;

				save_qc_meas(nmeas);



			} /* end nmeas loop */


			/* calculate results */
			sum = qcmeas[0].meas + qcmeas[1].meas + qcmeas[2].meas;
			qcmeas[0].result = 100. * qcmeas[2].meas / sum;
			qcmeas[1].result = 100. * qcmeas[0].meas / sum;
			qcmeas[2].result = 100. * qcmeas[1].meas / sum;

			/* display summary */
			display_qc_mag3();
            if(home_set())
                return;

	        //print report -- break if no more tests
            if(!print_qc(QC_MAG3)|| home_set())
				return;

		}

	}


static	void pr_qc_mag3(char prtype)
	{


		//print_actstr(0,"Fraction #1      : ",prtype);
		//print_actstr(1,"Fraction #2      : ",prtype);
		//print_actstr(2,"Cartridge        : ",prtype);
/*        print_actstr(0,QC_50,prtype);
 -         print_actstr(1,QC_51,prtype);
 -         print_actstr(2,QC_52,prtype);*/
        print_actstr(0,L_FRACTION_12,prtype);    // "Fraction #1      : "
        print_actstr(1,L_FRACTION_22,prtype);    // "Fraction #2      : "
        print_actstr(2,L_CARTRIDGE2,prtype);    // "Cartridge        : "

        print_result_head(prtype);

		//print_result(0,"Non-elutable Tc99m       :",prtype);
		//print_result(1,"Hydrophilic Tc Impurities:",prtype);
		//print_result(2,"Tc99m Mertiatide         :",prtype);
/*        print_result(0,QC_53,prtype);
 -         print_result(1,QC_54,prtype);
 -         print_result(2,QC_55,prtype);*/
        print_result(0,L_NON_ELUTABLE2,prtype);    // "Non-elutable Tc99m       :"
        print_result(1,L_HYDROPHILIC_TC_IMPURITIES,prtype);    // "Hydrophilic Tc Impurities:"
        print_result(2,L_TC_MERTIATIDE,prtype);    // "Tc99m Mertiatide         :"

		formfeed(prtype);

	}

static	void display_qc_mag3(void)
	{

        erase_screen();
		//display_text(18,0,"QC TEST  MAG3",0,SMALL,NORMAL);
        display_small_message(QC_24,0,0,NORMAL);

		//display_result(0,"Non-elutable Tc99m",1);
		//display_result(1,"Hydrophilic Impure",3);
		//display_result(2,"Tc99m Mertiatide",5);
        display_result(0,QC_25,1);
        display_result(1,QC_26,3);
        display_result(2,QC_27,5);

		contmsg();

	}

void Amulet_printOneStrip(void){
	char prtype;
	char strng[90];
	char message[52];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		prhead_languagesec(prtype);

		feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"QUALITY CONTROL TEST",20);
		//get_message(QC_56,&strng[2]);
		get_amulet_message(L_QUALITY_CONTROL_TEST,message);    // "QUALITY CONTROL TEST"
		strncpy(&strng[2],message,strlen(message));
		
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);

		lininit(strng,TRUE,prtype);
		//get_message(report_header[QC_1_STRIP],&strng[2]);
		get_amulet_message(L_SINGLE_STRIP_SINGLE_SOLVENT,message);    // "Single Strip / Single Solvent"
		strncpy(&strng[2],message,strlen(message));
		pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		feed(1,prtype);

		pr_radiopharm(prtype);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"Measured Values",15);
		//get_message(QC_57,&strng[1]);
		get_amulet_message(L_MEASURED_VALUES,message);    // "Measured Values"
		strncpy(&strng[1],message,strlen(message));
		pr_write(strng);

		pr_qc_1_strip(prtype);
	}
}

void Amulet_printTwoStrip(void){
	char prtype;
	char strng[90];
	char message[50];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		prhead_languagesec(prtype);

		feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"QUALITY CONTROL TEST",20);
		//get_message(QC_56,&strng[2]);
		get_amulet_message(L_QUALITY_CONTROL_TEST,message);    // "QUALITY CONTROL TEST"
		strncpy(&strng[2],message,strlen(message));
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);

		lininit(strng,TRUE,prtype);
		//get_message(report_header[QC_2_STRIPS],&strng[2]);
		get_amulet_message(L_TWO_STRIPS_TWO_SOLVENTS,message);    // "Two Strips / Two Solvents"
		strncpy(&strng[2],message,strlen(message));
		pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		feed(1,prtype);

		pr_radiopharm(prtype);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"Measured Values",15);
		//get_message(QC_57,&strng[1]);
		get_amulet_message(L_MEASURED_VALUES,message);    // "Measured Values"
		strncpy(&strng[1],message,strlen(message));
		pr_write(strng);

		pr_qc_2_strips(prtype);
	}
}

void Amulet_printHMPAO(void){
	char prtype;
	char strng[90];
	char message[40];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		prhead_languagesec(prtype);

		feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"QUALITY CONTROL TEST",20);
		//get_message(QC_56,&strng[2]);
		get_amulet_message(L_QUALITY_CONTROL_TEST,message);    // "QUALITY CONTROL TEST"
		strncpy(&strng[2],message,strlen(message));
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);

		lininit(strng,TRUE,prtype);
		//get_message(report_header[QC_HMPAO],&strng[2]);
		strcpy(message,"Tc99m HMPAO (Exametazine)");
		strncpy(&strng[2],message,strlen(message));
		pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		feed(1,prtype);

		pr_radiopharm(prtype);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"Measured Values",15);
		//get_message(QC_57,&strng[1]);
		get_amulet_message(L_MEASURED_VALUES,message);    // "Measured Values"
		strncpy(&strng[1],message,strlen(message));
		pr_write(strng);

		pr_qc_hmpao(prtype);
	}
}

void Amulet_printMAG3(void){
	char prtype;
	char strng[90];
	char message[40];

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		prhead_languagesec(prtype);

		feed(1,prtype);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[2],"QUALITY CONTROL TEST",20);
		//get_message(QC_56,&strng[2]);
		get_amulet_message(L_QUALITY_CONTROL_TEST,message);    // "QUALITY CONTROL TEST"
		strncpy(&strng[2],message,strlen(message));
		if(prtype == USB_EPS_PRINTER) eps_bold(0.5,strng);
		else pr_write(strng);

		lininit(strng,TRUE,prtype);
		//get_message(report_header[QC_MAG3],&strng[2]);
		strcpy(message,"Tc99m MAG3");
		strncpy(&strng[2],message,strlen(message));
		pr_write(strng);
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		feed(1,prtype);

		pr_radiopharm(prtype);

		lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],"Measured Values",15);
		//get_message(QC_57,&strng[1]);
		get_amulet_message(L_MEASURED_VALUES,message);    // "Measured Values"
		strncpy(&strng[1],message,strlen(message));
		pr_write(strng);

		pr_qc_mag3(prtype);
	}
}

//prints reports, returns TRUE if do another test, else returns FALSE
static bool print_qc(short type)
    {

        char prtype;
        char strng[90];
        bool ynret;

        //EE_READ(print[0],(uchar *)&prtype);
        prtype = current.printer;

        do
        {

            if (!start_printer(prtype,1,FALSE,PAPER))
                break;


            prhead_language(prtype);

            feed(1,prtype);
            if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
                bold(TRUE);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[2],"QUALITY CONTROL TEST",20);
            get_message(QC_56,&strng[2]);
            if(prtype == USB_EPS_PRINTER)
                eps_bold(0.5,strng);
            else
                pr_write(strng);

            lininit(strng,TRUE,prtype);
            get_message(report_header[type],&strng[2]);
            pr_write(strng);
            if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER)
                bold(FALSE);
            feed(1,prtype);

            pr_radiopharm(prtype);

            lininit(strng,TRUE,prtype);
            //strncpy(&strng[1],"Measured Values",15);
            get_message(QC_57,&strng[1]);
            pr_write(strng);

            switch(type)
            {
            case  QC_1_STRIP:
                pr_qc_1_strip(prtype);
                break;
            case QC_2_STRIPS:
                pr_qc_2_strips(prtype);
                break;
            case QC_HMPAO:
                pr_qc_hmpao(prtype);
                break;
            case QC_MAG3:
                pr_qc_mag3(prtype);
                break;
            }

            erase_screen();
            ynret = print_again();
            if(home_set())
                return FALSE;
        }while(ynret);

        erase_screen();
        //display_text(8,12,"Do Another",0,MEDIUM,NORMAL);
        //display_text(8,24,"Test? Y or N",0,MEDIUM,NORMAL);
        display_medium_message(QC_28,12,0,NORMAL);
        display_medium_message(QC_29,24,0,NORMAL);
        return(yesorno());


    }
    extern MEASUREMENT measurement[];
static void save_qc_meas(short nmeas)
    {
        short ch_num = current.main_chamber;
        //float act;
        //char kun;

	// Changed:
        //act = measurement[ch_num].act;
        //kun = measurement[ch_num].kun;
        //qcmeas[nmeas].meas = act / unitfact[kun - 1];
	qcmeas[nmeas].meas = measurement[ch_num].act0;
        // Locked: CI
        //format_activity(qcmeas[nmeas].meas,current.system,&qcmeas[nmeas].actstr[0]);
        format_activity(qcmeas[nmeas].meas, CI, &qcmeas[nmeas].actstr[0]);

    }

static short format_result(short index, char *buffer)
    {
        short nc;

        nc = sprintf(buffer,"%5.1f",qcmeas[index].result);

        return nc;
    }

//static void display_result(short index, char *str, short line)
static void display_result(short index, short msg_num, short line)
    {

       short nc;
       short ypos;
       char buffer[10];

       ypos = 8 * line;
       //display_text(0,ypos,str,0,SMALL,NORMAL);
       display_small_message(msg_num,ypos,0,NORMAL);

       nc = format_result(index,buffer);
       ypos = 8 * (line + 1);
       display_text(6 * (12 - nc),ypos,buffer,0,SMALL,NORMAL);
       display_text(72,ypos,"%",0,SMALL,NORMAL);


    }

//static void print_result(short index, char *str, char prtype)
static void print_result(short index, short msg_num, char prtype)
    {

        char strng[90];
        char buffer[10];
        short nc;
		char message[50];

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[1],str,strlen(str));
        //get_message(msg_num,&strng[1]);
		get_amulet_message(msg_num,message);
		strncpy(&strng[1],message,strlen(message));
        nc = format_result(index,buffer);
        strncpy(&strng[32 - nc],buffer,nc);
        strng[32] = '%';
        pr_write(strng);

    }

//static void print_actstr(short index, char *str, char prtype)
static void print_actstr(short index, short msg_num, char prtype)
    {

        char strng[90];
		char message[50];

        lininit(strng,TRUE,prtype);
		//strncpy(&strng[1],str,strlen(str));
        //get_message(msg_num,&strng[1]);
		get_amulet_message(msg_num,message);
		strncpy(&strng[1],message,strlen(message));
		strncpy(&strng[24],&qcmeas[index].actstr[0],9);
        pr_mu(&strng[30],prtype);
		//strncpy(&strng[20],&qcmeas[index].actstr[0],9);
        //pr_mu(&strng[26],prtype);
        pr_write(strng);

    }

static void print_result_head(char prtype)
    {
        char strng[90];
		char message[50];

        feed(1,prtype);

        lininit(strng,TRUE,prtype);
        //strncpy(&strng[1],"Results",7);
        //get_message(QC_30,&strng[1]);
		get_amulet_message(L_RESULTS,message);    // "Results"
		strncpy(&strng[1],message,strlen(message));
        pr_write(strng);

    }
