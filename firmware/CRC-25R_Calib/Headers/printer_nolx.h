#pragma once
/*********************************************************************
  MODULE:	DEFINITION FILE   for Printing

  FILE:		printer.h

  DATE:		12/18/06


  *************************************************************************/

enum
{
	PR_TICKET,
	PR_LINE,
};

#define ESC 		0x1b
#define	MU_PRT    	0xe6
#define PLUS_MINUS	0xf1
#define DIV_PRT		0xf6
#define PAPER_IN_ALL	0
#define PAPER_IN        2
#define PAPER_OUT	1		//end of sheet
#define NO_PAPER	3
#define NO_PRINTER  (char)0xff
#define NOT_CONNECTED 0x10

enum
{
	TICKET,
	PAPER,
};

enum
{
    USB_EPS_PRINTER = -2,    
    USB_PRINTER,
    SLIP_PRINTER,
	ROLL_PRINTER,
	OKI_PRINTER,
	NONE_PRINTER,
};


void font35(void);
void font17(void);
void font42(void);
void font21(void);
void pr_reset(void);
void font6e(bool on);
void font85e(bool on);
void fontnlq(bool on);
void font171(void);
void bold(bool on);
bool check_oki(short type);
bool check_usb_printer(void);
void formfeed(char printer);
void lininit(char *strng, bool flag, char printer);
void printer_init(void);
void feed(short num,char printer);
bool insert_paper(short type, bool rel_flag);
bool start_printer(char printer, short mode, bool flag, short type);
void print_accuracy_test(short prtype);
void print_constancy_test(short printer);
void prdate(short printer);
void prdatesec(short printer);
void prdate_measure(short printer, time_t *measuredon);
void prdate_measuresec(short printer, time_t *measuredon);
void dosig(char prtype);
bool print_again(void);
void prline(void);
bool prticket(short mode);
void pr_meas_date(char printer);
void pr_head_dt(char printer);
void prhead(char printer);
void prheadsec(char printer);
void color(short n, char printer);
void pr_write(char *strng);

void usb_write (char *cData);
void usb_write_num (char *cData, short num);
void pcl_reset(void);
void pcl_bold(bool on);
void pcl_cpi(char *cpi_str);
void pcl_expanded(void);
void pcl_height(char *ht_str);
void pcl_vertcial(char *vt_str);
void pcl_symbol(char *sym_str);
void pcl_horizontal(char *hz_str);

void eps_reset(bool p_flag);
void eps_cpi(short n);
void eps_horiz(float fin);
void eps_vert(bool abs_flag,float fin);
void eps_bold(float horiz, char *str);


void change_paper(void);
void set_act_line(char printer);
void pr_mu(char *str, char printer);
short bytes_remaining(void);
void pr_error(void);
