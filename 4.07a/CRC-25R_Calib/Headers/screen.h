/***********************************************************
  MODULE:  Screen Header

  FILE:   screen.h

  DATE:   09/06/06

**********************************************************************/


void write_screen_command(uchar commad);
void write_screen_data(uchar data, bool cmd_flag);
uchar read_screen_byte(bool cmd_flag);
void read_screen(short plane);
void write_screen(short plane);
void erase_screen(void);
void screen_set(void);
void initialize_screen(void);
void set_cursor(ushort address);
void set_plane(short plane);
void reverse_plane(void);

ushort get_screen_address(short xpos, short ypos, short plane);
void draw_point(short xpos, short ypos, short plane, bool draw_flag);
void draw_horiz_line(short xpos0, short xpos1, short ypos, short plane,
    bool draw_flag);
void draw_vert_line(short xpos, short ypos0, short ypos1, short plane,
    bool draw_flag);
    
void display_text(short xpos0, short ypos0, char *string,short plane,
			short fontsize, short rflag);
void draw_box(short xpos0, short ypos0, short xpos1, short ypos1,
    short plane, short fill_flag);
void erase_lines(short ypos, short nlines, short plane);   
void get_ch_array(uchar ch, short fontsize, uchar *ch_array);
void readcg(uchar ch, uchar cgdata[]);
void rev_outline(short n, short m, short xpos, short ypos, short plane, short rflag);
void unrev(short xpos, short ypos, char *chrev, short plane, short fontsize);
void erase_text_lines(short ypos, short fontsize, short plane);
void erase_text(short xpos, short ypos,short plane,
			short length,short fontsize);
void erase_box(short xpos0, short ypos0,
		short xpos1, short ypos1, short plane);
void erase_text_line(short ypos, short fontsize, short plane);
void write_special(char *spchar,short xpos, short ypos, short fontsize, short plane);

short inpint(short xpos0, short ypos0,short fontsize, short plane,
	short length,short minval, short maxval, char ch0);
float inpflt(short xpos0, short ypos0,short fontsize, short plane,
	short length,float minval, float maxval);
bool numstr_in(short xpos0, short ypos0,char *strng,
			   short fontsize, short plane,
			   short length,short er_length,
			   bool disp_flag, bool md_flag);
short  time_date_input(short xpos0, short ypos0,char *strng,
                    short fontsize, short plane);
bool decode_time_date(bool meas_flag, struct tm *dt);
float inpfl_u(short xpos0,short ypos0,short length,short *unit,
				  short type);			   
short inpalnum(short xpos0,short ypos0,char *strng,short length,bool alflag);
void contmsg(void);
void key_pc_continue(void);
bool yesorno(void);
short choose(short min, short max);
void ackmsg(void);

bool date_in(short xpos, short ypos,struct tm *dt);
bool time_in(short xpos, short ypos, struct tm *dt);
void disp_date_time(struct tm *dt);

void screen_print(short ystart,short yend);
uchar get_scrmem(short plane, short addr);
void screen_print_pcl(void);
void screen_print_eps(void);

enum draw_types
{
    DRAW,
    ERASE_FLAG,
};

enum boxtypes
{
    EMPTY_BOX,
    FILLED_BOX,
    SOLID_BOX,
};

//font sizes
enum   
{
    SMALL,
	MEDIUM,
	BIG,
	VBIG,
    LOGO,
};

typedef struct font FONT;
struct font
{
	short		x;
	short		y;
	short		nbytes;
	short		delx;
	short		dely;
	char		keep[7];
    uchar       rev_mask;        
};

/* Normal and Reversed text */
enum
{
	NORMAL,
	REV,
};



