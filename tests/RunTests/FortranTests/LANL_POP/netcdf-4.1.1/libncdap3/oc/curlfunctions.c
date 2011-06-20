/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */


#include "ocinternal.h"
#include "ocdebug.h"
#include "ocdata.h"
#include "occontent.h"

#include "rc.h"

/* This is called with arguments while the other functions in this file are
 * used with global values read from the.dodsrc file.
 */
int
set_user_password(CURL* curl, const char *userC, const char *passwordC)
{
	CURLcode cstat;
	int iUserPassSize = strlen(userC) + strlen(passwordC) + 2;
	char *userPassword = malloc(sizeof(char) * iUserPassSize);
	if (!userPassword) {
		oc_log(LOGERR,
		      "Failed to allocate memory for the username and/or password.\n");
		return OC_ENOMEM;
	}
	strncpy(userPassword, userC, iUserPassSize);
	strncat(userPassword, ":", iUserPassSize);
	strncat(userPassword, passwordC, iUserPassSize);
	cstat = curl_easy_setopt(curl, CURLOPT_USERPWD, userPassword);
	if (cstat != CURLE_OK) {
		free(userPassword);
		return OC_ECURL;
	}

	cstat = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long) CURLAUTH_ANY);
	if (cstat != CURLE_OK) {
		free(userPassword);
		return OC_ECURL;
	}

	free(userPassword);
	return OC_NOERR;
}

int
set_proxy(CURL* curl, struct OCproxy *pstructProxy)
{
	CURLcode cstat;
	cstat = curl_easy_setopt(curl, CURLOPT_PROXY, pstructProxy->host);
	if (cstat != CURLE_OK)
		return OC_ECURL;

	cstat = curl_easy_setopt(curl, CURLOPT_PROXYPORT, pstructProxy->port);
	if (cstat != CURLE_OK)
		return OC_ECURL;

	if (pstructProxy->user) {
		int userPassSize = strlen(pstructProxy->user) + strlen(
				pstructProxy->password) + 2;
		char *userPassword = malloc(sizeof(char) * userPassSize);
		if (!userPassword) {
			oc_log(LOGERR, "Out of Memory\n");
			return OC_ENOMEM;
		}
		strncpy(userPassword, pstructProxy->user, userPassSize);
		strncat(userPassword, ":", userPassSize);
		strncat(userPassword, pstructProxy->password, userPassSize);
		cstat = curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, userPassword);
		if (cstat != CURLE_OK) {
			free(userPassword);
			return OC_ECURL;
		}

#ifdef CURLOPT_PROXYAUTH
		cstat = curl_easy_setopt(curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
		if(cstat != CURLE_OK) goto fail;
#endif
		free(userPassword);
	}

	return OC_NOERR;
}

int
set_cookies(CURL* curl, const char *cook)
{
	CURLcode cstat;
	cstat = curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cook);
	if (cstat != CURLE_OK)
		return OC_ECURL;

	cstat = curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);
	if (cstat != CURLE_OK)
		return OC_ECURL;

	return OC_NOERR;
}

int
set_verify(CURL* curl)
{
	CURLcode cstat;
	cstat = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	if (cstat != CURLE_OK)
		return OC_ECURL;
	cstat = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	if (cstat != CURLE_OK)
		return OC_ECURL;

	return OC_NOERR;
}

int
set_compression(CURL* curl)
{
#ifdef CURLOPT_ENCODING
	CURLcode cstat;
	cstat = curl_easy_setopt(curl, CURLOPT_ENCODING, 'deflate, gzip');
	if(cstat != CURLE_OK)
		return OC_ECURL;
#endif
	return OC_NOERR;
}

