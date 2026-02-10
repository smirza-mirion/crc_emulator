
  /* definition file for Lab Tests in Well */
  /* file: labtests.h */
  /* date: 06/01/04 */


float enter_hematocrit(void);
float dilution_factor(void);
float aliquot_vol(void);
float urine_vol(void);
float enter_wt(void);
void disp_plas_res(char * ,float , float );
bool pr_plas_res(char * title,float,float,char *,short );
void send_plas_res(char * ,float , float,char *);
void disp_counts(char *,char *,bool ,char *line);
void lab_head(char ,short ,char *,short);
bool pr_counts(char *,char *,bool ,char *,char *,short);
void pr_patdat(short);
void send_counts(char *,char *,bool ,char *,char *);
void send_feed(short);
void send_lab_head(char *,short);




		

