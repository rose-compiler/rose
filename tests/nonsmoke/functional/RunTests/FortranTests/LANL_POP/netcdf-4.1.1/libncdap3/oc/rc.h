/*
 * rc.h
 *
 *  Created on: Mar 5, 2009
 *      Author: rikki
 */

#ifndef RC_H_
#define RC_H_

struct OCproxy {
	char *host;
	int port;
	char *user;
	char *password;
};

extern int dods_compress;
extern int dods_verify;
extern struct OCproxy *pstructProxy;
extern char *cook;
extern char *userName;
extern char *password;

extern int credentials_in_url(const char *url);
extern int extract_credentials(const char *url, char **name, char **pw, char **result_url);
extern int set_credentials(const char *name, const char *pw);
extern int read_dodsrc(char *in_file_name);
extern int write_dodsrc(char *out_file_name);

extern int set_user_password(CURL* curl, const char *userC,
		const char *passwordC);
extern int set_proxy(CURL* curl, struct OCproxy *pstructProxy);
extern int set_cookies(CURL* curl, const char *cook);
extern int set_verify(CURL* curl);
extern int set_compression(CURL* curl);

#endif /* RC_H_ */
