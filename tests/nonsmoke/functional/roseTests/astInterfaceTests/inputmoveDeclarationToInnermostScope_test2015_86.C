
int strcmp(char*,char*);
void strcpy (char*,char*);
int strstr (char*,char*);
char* strrchr(char*,char);
int isdigit(char);
int strlen(char*);

const unsigned long NULL = 0L;

const int MAXLINE = 42;

void foobar()
   {
     char rgdmpname[MAXLINE], cname[MAXLINE], arg1[16];
     char msg[MAXLINE], tmpstr[MAXLINE], tmpstr_lower[MAXLINE];

     char** argv;
     int i;
     int ierr;

     char* ptr;
     bool noerrflag;

     if (1)
        {     
          if (!strcmp(tmpstr_lower,"abc")) {
               strcpy(rgdmpname, argv[i+1]);
               i++;
            }
            else if (!strcmp(tmpstr_lower,"abc")) {

               strcpy(rgdmpname, argv[i+1]);
               i++;
            }
            else if (!strcmp(tmpstr_lower,"abc")) {

            }
            else if (!strcmp(tmpstr_lower,"abc")) {

            }
            else if (    (strstr(tmpstr_lower,"abc") != NULL)
                      || (strstr(tmpstr_lower,"abc") != NULL) ) {

            }
            else if ( ((ptr = strrchr(tmpstr,'-')) != NULL) &&
                      (isdigit((int)(ptr[1]))) ) {
               *ptr = '\0';
               strcpy(rgdmpname,tmpstr);
            }
            else if (    (isdigit((int)(tmpstr[0])))
                      && (isdigit((int)(tmpstr[strlen(tmpstr)-1]))) ) {

            }
            else {
            }
            if (ierr != 0) {
               rgdmpname[0] = '\0' ;
            }

         } else if (!strcmp(argv[i],"abc")) {

            noerrflag = true;

         }
   }
