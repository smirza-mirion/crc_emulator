#pragma once
/*********************************************************************
  MODULE:	DEFINITION FILE   for Printing

  FILE:		printer.h

  DATE:		01/24/07


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
	USB_EPS_LABEL_PRINTER = -3,
    USB_EPS_PRINTER,
    USB_PRINTER,
    SLIP_PRINTER,
	ROLL_PRINTER,
	OKI_PRINTER,
    LX_PRINTER,
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
void lininit_db(char *strng, bool flag, char printer);
void linupdate_db(char *strng, bool large, bool bold);
void lininit3(char *strng, bool flag, char printer);
void printer_init(void);
void feed(short num,char printer);
void feed_db(short num,char printer);
void feed2(short num,char printer);
bool insert_paper(short type, bool rel_flag);
bool start_printer(char printer, short mode, bool flag, short type);
void print_accuracy_test(short prtype);
void print_constancy_test(short printer);
void prdate(short printer);
void prdatesec(short printer);
void prdate_language(short printer);
void prdate_languagesec(short printer);
void prdate_measure(short printer, time_t *measuredon);
void prdate_measuresec(short printer, time_t *measuredon);
void prdate_measure_language(short printer, time_t *measuredon);
void prdate_measure_languagesec(short printer, time_t *measuredon);
void dosig(char prtype);
bool print_again(void);
void prline(void);
bool prticket(short mode);
void pr_meas_date(char printer);
void pr_head_dt(char printer);
void prhead(char printer);
void prheadsec(char printer);
void prhead_language(char printer);
void prhead_languagesec(char printer);
void prhead_language_db(char printer);
void prhead_testtime(char printer, time_t testtime);
void prhead_testtimesec(char printer, time_t testtime);
void prhead_testtime_language(char printer, time_t testtime);
void prhead_testtime_languagesec(char printer, time_t testtime);
void color(short n, char printer);
void pr_write(char *strng);
void pr_write2(char *strng, char *header);
void pr_write2sec(char *strng, char *header);
void pr_write2_language(char *strng, char *header);
void pr_write2_languagesec(char *strng, char *header);
void pr_write2_language_db(char *strng, char *header);
void pr_write2_language_dbsec(char *strng, char *header);
void pr_write3(char *strng, char *header);
void pr_write3_language(char *strng, char *header);
void pr_set_linecnt(int line);

void usb_write (char *cData);
void usb_write_num (char *cData, short num);
void pcl_reset(void);
void pcl_bold(bool on);
//void pcl_cpi(char *cpi_str);
void pcl_cpi(char *cpi_str,bool save);
void pcl_expanded(void);
//void pcl_height(char *ht_str);
void pcl_height(char *ht_str,bool save);
void pcl_vertcial(char *vt_str);
void pcl_symbol(char *sym_str);
void pcl_horizontal(char *hz_str);
//void pcl_line_spacing(char *l_str);
void pcl_line_spacing(char *ls_str,bool save);

void eps_reset(bool p_flag);
void eps_cpi(short n);
void eps_horiz(float fin);
void eps_vert(bool abs_flag,float fin);
void eps_bold(float horiz, char *str);
void eps_dbl(bool on);

void change_paper(void);
void set_act_line(char printer);
void pr_mu(char *str, char printer);
short bytes_remaining(void);
void pr_error(void);
void lab_head(char printer, char *title, short t_len, time_t testtime);
void well_head(char printer, char *title, short t_len, time_t testtime);
void well_head2(char printer, char *title1, short t_len1, char *title2, short t_len2, time_t testtime);
void rawheader(char printer, char *title, time_t testtime);
void rawheadersec(char printer, char *title, time_t testtime);
void rawheader_language(char printer, char *title, time_t testtime);
void rawheader_languagesec(char printer, char *title, time_t testtime);
void rawheader2(char printer, char *title1, char *title2, time_t testtime);
void graphics_test(void);

//void g_clear(short height, short leftpixel);
//void g_line(short x0, short y0, short x1, short y1);
//void g_string(short x0, short y0, char *printstring, short fontsize);
//void g_send_hp(void);
//void g_send_okidata(void);
//void g_send_epson(void);
//void g_send_roll(void);
void g_resetlinespacing(char prtype);
void g_print(char prtype, ulong *data, float startROIeV, float endROIeV, AUTOCAL *autoCal);
void insertconst(char *line, int columnstart, int justify, int lengthlimit, char *datastring);
void insertconst3(char *line, int columnstart, int justify, int lengthlimit, char *datastring);
void insertstring(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, char *datastring);
void insertstring3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, char *datastring);
void insertfloat(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, float datafloat);
void insertfloat3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, float datafloat);
void insertint(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, int dataint);
void insertint3(char *line, int columnstart, int justify, int lengthlimit, char *formatstring, int dataint);
void inserttime_t(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly);
void inserttime_tsec(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly);
void inserttime_t_language(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly);
void inserttime_t3_language(char *line, int columnstart, int justify, int lengthlimit, time_t datatime, bool dateonly);
