/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2005 Thomas Merz and PDFlib GmbH. All rights reserved. |
 +---------------------------------------------------------------------------+
 |                                                                           |
 |    This software is subject to the PDFlib license. It is NOT in the       |
 |    public domain. Extended versions and commercial licenses are           |
 |    available, please check http://www.pdflib.com.                         |
 |                                                                           |
 *---------------------------------------------------------------------------*/

/* $Id: pdflib_java.c,v 1.1 2005/10/11 17:18:08 vuduc2 Exp $
 *
 * synch'd with pdflib.h 1.232
 *
 * JNI wrapper code for the PDFlib Java binding
 *
 */

#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#ifdef OS390
#define IBM_MVS
#define NEEDSIEEE754
#define NEEDSLONGLONG
#endif

#if defined __ILEC400__ && !defined AS400
#define AS400
#endif

#if defined(OS390) || defined(MVS) || defined(AS400)
#undef PDF_FEATURE_EBCDIC
#define PDF_FEATURE_EBCDIC
#endif

#include <jni.h>

#ifdef OS390
#include <jni_convert.h>
#endif


#include "pdflib.h"


#ifndef OS390

#ifndef int2ll
#define int2ll(a)	((jlong) (a))
#endif

#ifndef ll2int
#define ll2int(a)	((int) (a))
#endif

#ifndef todouble
#define todouble(a)     ((jdouble) (a))
#endif

#ifndef dbl2nat
#define dbl2nat(a)      ((double) (a))
#endif

#endif /* !OS390 */


#define NULL_INT		NULL
#define PDF_ENV			jlong
#define PDF_ENV2PTR(env)	*((PDF **) &(env))



/* Compilers which are not strictly ANSI conforming can set PDF_VOLATILE
 * to an empty value.
 */
#ifndef PDF_VOLATILE
#define PDF_VOLATILE    volatile
#endif

/* thread-specific data */
typedef struct {
    jint	jdkversion;
    jmethodID	MID_String_getBytes;	/* cached method identifier */
} pdf_wrapper_data;

/* This is used in the wrapper functions for thread-safe exception handling */
#define EXC_STUFF pdf_wrapper_data * PDF_VOLATILE ex

/* Exception handling */

#define PDF_ISNULL(j)	(j == (PDF_ENV) 0)


#define PDF_JAVA_SANITY_CHECK_VOID(j) 				\
    if (PDF_ISNULL(j)) {					\
	jthrow(jenv, "Must not call any PDFlib method after delete()", \
		0, "pdflib_java.c");				\
	return;							\
    }

#define PDF_JAVA_SANITY_CHECK(j) 				\
    if (PDF_ISNULL(j)) {					\
	jthrow(jenv, "Must not call any PDFlib method after delete()",	\
		0, "pdflib_java.c");				\
	return _jresult;					\
    }

/* {{{ jthrow */
static void
jthrow(JNIEnv *jenv, const char *msg, int errnum, const char *apiname)
{
    jclass PDFlibException;
    jstring jmsg, japiname;
    jmethodID cid;
    jthrowable e;
    char *classstring;

#ifndef PDFLIB_EBCDIC
    classstring = "com/pdflib/PDFlibException";
    jmsg = (*jenv)->NewStringUTF(jenv, msg);
    japiname = (*jenv)->NewStringUTF(jenv, apiname);
#endif /* PDFLIB_EBCDIC */

    PDFlibException = (*jenv)->FindClass(jenv, classstring);
    if (PDFlibException == NULL_INT) {
        return;  /* Exception thrown */
    }

    /* Get method ID for PDFlibException(String, int, String) constructor */
#ifndef PDFLIB_EBCDIC
    cid = (*jenv)->GetMethodID(jenv, PDFlibException, "<init>",
        "(Ljava/lang/String;ILjava/lang/String;)V");
#endif /* PDFLIB_EBCDIC */
    if (cid == NULL_INT) {
        return;  /* Exception thrown */
    }

    e = (*jenv)->NewObject(jenv, PDFlibException, cid, jmsg, errnum, japiname);
    (*jenv)->Throw(jenv, e);
}
/* }}} */

#define pdf_catch	PDF_CATCH(p) { 					\
		    jthrow(jenv, PDF_get_errmsg(p), PDF_get_errnum(p), \
		                            PDF_get_apiname(p)); \
		}

#define PDF_BYTES  1
#define PDF_UTF8   2
#define PDF_UTF16  3

/* {{{ ConvertJavaString
 *
 * Converts a Java Unicode string to a C string.
 *
 * key:
 */
/*
 * PDF_BYTES
 * Converts to Latin-1 encoded in a null terminated byte array.
 * An exception will be thrown if the Java string contains at least
 * one Unicode character > U+00FF.
 * Array must be freed by 'free'.
 * lenP can be NULL.
 *
 * PDF_UTF8
 * Converts to UTF-8 with BOM  encoded in a null terminated byte array.
 * BOM will be discarded if Java string contains only Unicode character< U+0080.
 * Array will be freed by PDFlib at the end of next API function.
 * lenP can be NULL.
 */
/*
 * PDF_UTF16
 * Converts to a byte array keeping UTF-16 which can contain nulls.
 * Array must be freed by JNI method 'ReleaseStringChars'.
 * lenP must be specified.
 *
 */

static char *
ConvertJavaString(PDF *p, JNIEnv *jenv, jstring string, int key, int *lenP)
{
    const jchar *unistring;
    char *result = NULL;
    int len;

    if (lenP)
        *lenP = 0;

    if (!string)
        return NULL;

    len = (int) (*jenv)->GetStringLength(jenv, string);

    if (len == 0)
        return NULL;

    unistring = (*jenv)->GetStringChars(jenv, string, NULL);

    if (!unistring)
    {
        pdf_throw(p, "Java", "ConvertJavaString",
            "JNI internal string allocation failed");
    }

    switch (key)
    {
        case PDF_BYTES:
        {
            int i;

            result = (char *) malloc((size_t)(len + 1));
            if (!result)
            {
                pdf_throw(p, "Java", "ConvertJavaString",
                    "JNI internal string allocation failed");
            }

            /* pick the low-order bytes only */
            for (i = 0; i < len; i++)
            {
                if (unistring[i] > 0xFF)
                {
                    char buf[128];

                    sprintf(buf, "High Unicode character '\\u%04X' is not "
                            "supported is this character string", unistring[i]);
                    pdf_throw(p, "Java", "ConvertJavaString", buf);
                }
                result[i] = (char) unistring[i];
            }
            result[i] = '\0';

           (*jenv)->ReleaseStringChars(jenv, string, unistring);


        }
        break;

        case PDF_UTF8:
        {
            /* Convert to UTF-8 */
            result =
	    (char *) PDF_utf16_to_utf8(p, (char *) unistring, 2 * len, &len);

           (*jenv)->ReleaseStringChars(jenv, string, unistring);
        }
        break;

        case PDF_UTF16:
        {
            *lenP = 2 * len;
            result = (char *) unistring;
        }
        break;
    }

    return result;
}
/* }}} */

#define ReleaseCStringChars(chars) \
    if (chars) free(chars);

#define ReleaseJavaStringChars(jenv, string, chars) \
    if (chars) (*jenv)->ReleaseStringChars(jenv, string, (jchar *) chars);


/* {{{ GetNewStringUTF */
static jstring
GetNewStringUTF(JNIEnv *jenv, char *cstring)
{
    jstring _jresult = 0;
    char *_result = cstring;


    /* Creating new string object by converting UTF-8 to UTF-16 */
    _jresult = (jstring)(*jenv)->NewStringUTF(jenv, _result);


    return _jresult;
}
/* }}} */

/* {{{ PDF_activate_item */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1activate_1item(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jid)
{
    PDF *p;
    int id;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	id = (int )jid;

	PDF_activate_item(p, id);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_add_bookmark */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1add_1bookmark(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jint jparent, jint jopen)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    int parent;
    int open;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);
        parent = (int )jparent;
        open = (int )jopen;

        _result = (int )PDF_add_bookmark2(p, text, len1, parent, open);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_add_launchlink */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1launchlink(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jfilename)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE filename = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        filename = ConvertJavaString(p, jenv, jfilename, PDF_BYTES, NULL);

        PDF_add_launchlink(p, llx, lly, urx, ury, filename);
    } pdf_catch;

    ReleaseCStringChars(filename);
}
/* }}} */

/* {{{ PDF_add_locallink */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1locallink(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jint jpage, jstring joptlist)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    int page;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
	page = (int )jpage;
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        PDF_add_locallink(p, llx, lly, urx, ury, page, optlist);
    } pdf_catch;

}
/* }}} */

/* {{{ PDF_add_nameddest */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1nameddest(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jname, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE name = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        name = ConvertJavaString(p, jenv, jname, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_add_nameddest(p, name, len1, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jname, name);
}
/* }}} */

/* {{{ PDF_add_note */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1note(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jcontents, jstring jtitle, jstring jicon, jint jopen)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE contents = NULL;
    char * PDF_VOLATILE title = NULL;
    char * PDF_VOLATILE icon = NULL;
    int open;
    int lenc, lent;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        contents = ConvertJavaString(p, jenv, jcontents, PDF_UTF16, &lenc);
        title = ConvertJavaString(p, jenv, jtitle, PDF_UTF16, &lent);
        icon = ConvertJavaString(p, jenv, jicon, PDF_BYTES, NULL);
	open = (int )jopen;

        PDF_add_note2(p, llx, lly, urx, ury, contents, lenc, title, lent,
            icon, open);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jcontents, contents);
    ReleaseJavaStringChars(jenv, jtitle, title);
    ReleaseCStringChars(icon);
}
/* }}} */

/* {{{ PDF_add_pdflink */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1pdflink(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jfilename, jint jpage, jstring joptlist)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE filename = NULL;
    int page;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        filename = ConvertJavaString(p, jenv, jfilename, PDF_BYTES, NULL);
	page = (int )jpage;
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_add_pdflink(p, llx, lly, urx, ury, filename, page, optlist);
    } pdf_catch;

    ReleaseCStringChars(filename);
}
/* }}} */

/* {{{ PDF_add_thumbnail */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1thumbnail(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jimage)
{
    PDF *p;
    int image;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	image = (int )jimage;

	PDF_add_thumbnail(p, image);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_add_weblink */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1add_1weblink(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jurl)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char *PDF_VOLATILE url = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        url = ConvertJavaString(p, jenv, jurl, PDF_BYTES, NULL);

	PDF_add_weblink(p, llx, lly, urx, ury, url);
    } pdf_catch;

    ReleaseCStringChars(url);
}
/* }}} */

/* {{{ PDF_arc */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1arc(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy, jdouble jr, jdouble jalpha,
    jdouble jbeta)
{
    PDF *p;
    double x;
    double y;
    double r;
    double alpha;
    double beta;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        r = dbl2nat(jr);
        alpha = dbl2nat(jalpha);
        beta = dbl2nat(jbeta);

	PDF_arc(p, x, y, r, alpha, beta);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_arcn */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1arcn(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy, jdouble jr, jdouble jalpha,
    jdouble jbeta)
{
    PDF *p;
    double x;
    double y;
    double r;
    double alpha;
    double beta;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        r = dbl2nat(jr);
        alpha = dbl2nat(jalpha);
        beta = dbl2nat(jbeta);

	PDF_arcn(p, x, y, r, alpha, beta);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_attach_file */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1attach_1file(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jfilename, jstring jdescription, jstring jauthor,
    jstring jmimetype, jstring jicon)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE filename = NULL;
    char * PDF_VOLATILE description = NULL;
    char * PDF_VOLATILE author = NULL;
    char * PDF_VOLATILE mimetype = NULL;
    char * PDF_VOLATILE icon = NULL;
    int flen, dlen, alen;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &flen);
        description =ConvertJavaString(p, jenv, jdescription, PDF_UTF16, &dlen);
        author = ConvertJavaString(p, jenv, jauthor, PDF_UTF16, &alen);
        mimetype = ConvertJavaString(p, jenv, jmimetype, PDF_BYTES, NULL);
        icon = ConvertJavaString(p, jenv, jicon, PDF_BYTES, NULL);

	PDF_attach_file2(p, llx, lly, urx, ury, filename, flen,
	     description, dlen, author, alen, mimetype, icon);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfilename, filename);
    ReleaseJavaStringChars(jenv, jdescription, description);
    ReleaseJavaStringChars(jenv, jauthor, author);
    ReleaseCStringChars(mimetype);
    ReleaseCStringChars(icon);
}
/* }}} */

/* {{{ PDF_begin_document */
JNIEXPORT int JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1document(JNIEnv *jenv, jclass jcls,
	PDF_ENV jp, jstring jfilename, jstring joptlist)
{
    jint _jresult = -1;
    int PDF_VOLATILE _result = -1;
    PDF *p;
    char * PDF_VOLATILE filename = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        _result = (int )PDF_begin_document(p, filename, len1, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfilename, filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_begin_font */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1font(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfontname, jdouble ja, jdouble jb, jdouble jc,
    jdouble jd, jdouble je, jdouble jf, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE fontname = NULL;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        fontname = ConvertJavaString(p, jenv, jfontname, PDF_UTF16, &len1);
        a = dbl2nat(ja);
        b = dbl2nat(jb);
        c = dbl2nat(jc);
        d = dbl2nat(jd);
        e = dbl2nat(je);
        f = dbl2nat(jf);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_begin_font(p, fontname, len1, a, b, c, d, e, f, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfontname, fontname);
}
/* }}} */

/* {{{ PDF_begin_glyph */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1glyph(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jglyphname, jdouble jwx, jdouble jllx, jdouble jlly,
    jdouble jurx, jdouble jury)
{
    PDF *p;
    char * PDF_VOLATILE glyphname = NULL;
    double wx;
    double llx;
    double lly;
    double urx;
    double ury;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        glyphname = ConvertJavaString(p, jenv, jglyphname, PDF_BYTES, NULL);
        wx = dbl2nat(jwx);
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);

	PDF_begin_glyph(p, glyphname, wx, llx, lly, urx, ury);
    } pdf_catch;

    ReleaseCStringChars(glyphname);
}
/* }}} */

/* {{{ PDF_begin_item */
JNIEXPORT int JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1item(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
    jstring jtag, jstring joptlist)
{
    jint _jresult = -1;
    int PDF_VOLATILE _result = -1;
    PDF *p;
    char * PDF_VOLATILE tag = NULL;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        tag = ConvertJavaString(p, jenv, jtag, PDF_UTF8, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        _result = (int )PDF_begin_item(p, tag, optlist);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_begin_layer */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1layer(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jlayer)
{
    PDF *p;
    int layer;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	layer = (int )jlayer;

	PDF_begin_layer(p, layer);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_begin_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1page(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jwidth, jdouble jheight)
{
    PDF *p;
    double width;
    double height;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);
        PDF_begin_page(p, width, height);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_begin_page_ext */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1page_1ext(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jwidth, jdouble jheight, jstring joptlist)
{
    PDF *p;
    double width;
    double height;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        PDF_begin_page_ext(p, width, height, optlist);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_begin_pattern */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1pattern(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jwidth, jdouble jheight, jdouble jxstep, jdouble jystep,
    jint jpainttype)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    double width;
    double height;
    double xstep;
    double ystep;
    int painttype;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);
        xstep = dbl2nat(jxstep);
        ystep = dbl2nat(jystep);
	painttype = (int)jpainttype;

	_result = (int )PDF_begin_pattern(p, width, height, xstep,
					ystep, painttype);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_begin_template */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1begin_1template(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jwidth, jdouble jheight)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    double width;
    double height;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);

	_result = (int )PDF_begin_template(p, width, height);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_boot */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1boot(JNIEnv *jenv, jclass jcls)
{
    /* throws nothing from within the library */
    PDF_boot();
}
/* }}} */

/* {{{ PDF_circle */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1circle(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy, jdouble jr)
{
    PDF *p;
    double x;
    double y;
    double r;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        r = dbl2nat(jr);
	PDF_circle(p,x,y,r);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_clip */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1clip(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_clip(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_close */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1close(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_close(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_close_image */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1close_1image(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jimage)
{
    PDF *p;
    int image;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	image = (int )jimage;

	PDF_close_image(p, image);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_closepath */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1closepath(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_closepath(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_closepath_fill_stroke */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1closepath_1fill_1stroke(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_closepath_fill_stroke(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_closepath_stroke */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1closepath_1stroke(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_closepath_stroke(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_close_pdi */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1close_1pdi(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jdoc)
{
    PDF *p;
    int doc;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	doc = (int )jdoc;

	PDF_close_pdi(p, doc);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_close_pdi_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1close_1pdi_1page(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage)
{
    PDF *p;
    int page;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	page = (int )jpage;

	PDF_close_pdi_page(p, page);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_concat */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1concat(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
	jdouble a, jdouble b, jdouble c, jdouble d, jdouble e, jdouble f)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        PDF_concat(p, dbl2nat(a), dbl2nat(b), dbl2nat(c), dbl2nat(d),
                                dbl2nat(e), dbl2nat(f));
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_continue_text */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1continue_1text(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext)
{
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);

	PDF_continue_text2(p, text, len1);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);
}
/* }}} */

/* {{{ PDF_create_action */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1create_1action(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtype, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE type = NULL;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        type = ConvertJavaString(p, jenv, jtype, PDF_BYTES, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        _result = (int) PDF_create_action(p, type, optlist);
    } pdf_catch;

    ReleaseCStringChars(type);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_create_annotation */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1create_1annotation(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jtype, jstring joptlist)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE type = NULL;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        type = ConvertJavaString(p, jenv, jtype, PDF_BYTES, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        PDF_create_annotation(p, llx, lly, urx, ury, type, optlist);
    } pdf_catch;

    ReleaseCStringChars(type);
}
/* }}} */

/* {{{ PDF_create_bookmark */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1create_1bookmark(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jstring joptlist)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        _result = (int )PDF_create_bookmark(p, text, len1, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_create_field */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1create_1field(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp,  jdouble jllx, jdouble jlly, jdouble jurx, jdouble jury,
    jstring jname, jstring jtype, jstring joptlist)
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE name = NULL;
    char * PDF_VOLATILE type = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        name = ConvertJavaString(p, jenv, jname, PDF_UTF16, &len1);
        type = ConvertJavaString(p, jenv, jtype, PDF_BYTES, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        PDF_create_field(p, llx, lly, urx, ury, name, len1, type, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jname, name);
    ReleaseCStringChars(type);
}
/* }}} */

/* {{{ PDF_create_fieldgroup */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1create_1fieldgroup(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jname, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE name = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        name = ConvertJavaString(p, jenv, jname, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        PDF_create_fieldgroup(p, name, len1, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jname, name);
}
/* }}} */

/* {{{ PDF_create_gstate */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1create_1gstate(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring joptlist)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = PDF_create_gstate(p, optlist);
    } pdf_catch;


    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_create_pvf */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1create_1pvf(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfilename, jbyteArray jdata, jstring joptlist)
{
    PDF *p;
    char *  PDF_VOLATILE filename = NULL;
    char *  PDF_VOLATILE data = NULL;
    char *  PDF_VOLATILE optlist = NULL;
    char *  PDF_VOLATILE optlist_long = NULL;
    char *copyopt = " copy";
    int len1;
    size_t dlen;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &len1);
	dlen = (*jenv)->GetArrayLength(jenv, jdata);
	data = (char *)
	    (*jenv)->GetByteArrayElements(jenv, jdata, (jboolean *) NULL);

        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	/*
	 * We must set the copy option since we must Release the data
	 * in this function, but it will be needed longer.
	 * Therefore we append the "copy" option to the supplied option list.
	 */
	if (optlist)
	{
	    optlist_long = (char *) malloc(strlen(optlist) + strlen(copyopt)+1);
	    if (optlist_long == NULL)
	    {
		jthrow(jenv, "Out of memory in PDF_create_pvf", 0,
		    "pdflib_java.c");
		return;
	    }
	    strcpy(optlist_long, optlist);
	    strcat(optlist_long, copyopt);
	}
	else
	{
	    optlist_long = copyopt;
	}

	PDF_create_pvf(p, filename, len1, data, dlen, optlist_long);

    } pdf_catch;

    if (optlist)
	free(optlist_long);		/* NOT optlist, but optlist_long! */

    ReleaseJavaStringChars(jenv, jfilename, filename);
    (*jenv)->ReleaseByteArrayElements(jenv, jdata, (jbyte*) data, JNI_ABORT);
}
/* }}} */

/* {{{ PDF_create_textflow */
JNIEXPORT int JNICALL
Java_com_pdflib_pdflib_PDF_1create_1textflow(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    char *  PDF_VOLATILE text = NULL;
    char *  PDF_VOLATILE optlist = NULL;
    int len;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        _result = (int) PDF_create_textflow(p, text, len, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_curveto */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1curveto(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx1, jdouble jy1, jdouble jx2, jdouble jy2,
    jdouble jx3, jdouble jy3)
{
    PDF *p;
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x1 = dbl2nat(jx1);
        y1 = dbl2nat(jy1);
        x2 = dbl2nat(jx2);
        y2 = dbl2nat(jy2);
        x3 = dbl2nat(jx3);
        y3 = dbl2nat(jy3);

	PDF_curveto(p, x1, y1, x2, y2, x3, y3);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_define_layer */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1define_1layer(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jname, jstring joptlist)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE name = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        name = ConvertJavaString(p, jenv, jname, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = PDF_define_layer(p, name, len1, optlist);
    } pdf_catch;

    ReleaseCStringChars(name);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_delete */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1delete(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;
    EXC_STUFF;

    /* Different sanity check here since PDF_delete() may be called multiply,
     * e.g., once from the finalizer and once explicitly
     */

    if (PDF_ISNULL(jp))
        return;

    p = PDF_ENV2PTR(jp);


    ex = (pdf_wrapper_data *) (PDF_get_opaque(p));
    free(ex);

    PDF_delete(p);
}
/* }}} */

/* {{{ PDF_delete_pvf */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1delete_1pvf(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfilename)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE filename = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &len1);
	_result = (int )PDF_delete_pvf(p, filename, len1);

    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfilename, filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_delete_textflow */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1delete_1textflow(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jtextflow)
{
    PDF *p;
    int textflow;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        textflow = (int )jtextflow;

        PDF_delete_textflow(p, textflow);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_encoding_set_char */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1encoding_1set_1char(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jencoding, jint jslot, jstring jglyphname, jint juv)
{
    PDF *p;
    char * PDF_VOLATILE encoding = NULL;
    int slot;
    char * PDF_VOLATILE glyphname = NULL;
    int uv;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        encoding = ConvertJavaString(p, jenv, jencoding, PDF_BYTES, NULL);
	slot = (int)jslot;
        glyphname = ConvertJavaString(p, jenv, jglyphname, PDF_BYTES, NULL);
	uv = (int)juv;

	PDF_encoding_set_char(p, encoding, slot, glyphname, uv);
    } pdf_catch;

    ReleaseCStringChars(encoding);
    ReleaseCStringChars(glyphname);
}
/* }}} */

/* {{{ PDF_end_document */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1document(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
    jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        PDF_end_document(p, optlist);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_font */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1font(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_font(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_glyph */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1glyph(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_glyph(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_item */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1item(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jid)
{
    PDF *p;
    int id;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        id = (int )jid;

        PDF_end_item(p, id);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_layer */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1layer(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_layer(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1page(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_page(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_page_ext */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1page_1ext(JNIEnv *jenv, jclass jcls,
	PDF_ENV jp, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        PDF_end_page_ext(p, optlist);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_pattern */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1pattern(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_pattern(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_end_template */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1end_1template(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_end_template(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_endpath */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1endpath(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_endpath(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_fill */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1fill(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_fill(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_fill_imageblock */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1fill_1imageblock(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage, jstring jblockname, jint jimage, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE blockname = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int page = (int) jpage;
    int image = (int) jimage;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        blockname = ConvertJavaString(p, jenv, jblockname, PDF_UTF8, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
	_result = (int )PDF_fill_imageblock(p, page, blockname, image, optlist);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_fill_pdfblock */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1fill_1pdfblock(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage, jstring jblockname, jint jcontents,
    jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE blockname = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int page = (int) jpage;
    int contents = (int) jcontents;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        blockname = ConvertJavaString(p, jenv, jblockname, PDF_UTF8, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
	_result =
	    (int) PDF_fill_pdfblock(p, page, blockname, contents, optlist);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_fill_textblock */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1fill_1textblock(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage, jstring jblockname, jstring jtext, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    int page = (int) jpage;
    char * PDF_VOLATILE blockname = NULL;
    char * PDF_VOLATILE text = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int tlen;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        blockname = ConvertJavaString(p, jenv, jblockname, PDF_UTF8, NULL);
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &tlen);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
	_result = (int )PDF_fill_textblock(p, page, blockname,
			    text, tlen, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_fill_stroke */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1fill_1stroke(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_fill_stroke(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_findfont */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1findfont(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfontname, jstring jencoding, jint jembed)
{
    jint _jresult = -1;
    int PDF_VOLATILE _result = -1;
    PDF *p;
    char * PDF_VOLATILE fontname = NULL;
    char * PDF_VOLATILE encoding = NULL;
    int embed;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        fontname = ConvertJavaString(p, jenv, jfontname, PDF_UTF8, NULL);
        encoding = ConvertJavaString(p, jenv, jencoding, PDF_BYTES, NULL);
	embed = (int )jembed;

	_result = (int )PDF_findfont(p, fontname, encoding, embed);
    } pdf_catch;

    ReleaseCStringChars(encoding);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_fit_image */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1fit_1image(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jimage, jdouble jx, jdouble jy, jstring joptlist)
{
    PDF *p;
    int image;
    double x;
    double y;
    char *  PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
	image = (int)jimage;
        x = dbl2nat(jx);
        y = dbl2nat(jy);
	PDF_fit_image(p, image, x, y, optlist);
    } pdf_catch;

}
/* }}} */

/* {{{ PDF_fit_pdi_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1fit_1pdi_1page(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage, jdouble jx, jdouble jy, jstring joptlist)
{
    PDF *p;
    int page;
    double x;
    double y;
    char *  PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	page = (int) jpage;
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_fit_pdi_page(p, page, x, y, optlist);
    } pdf_catch;

}
/* }}} */

/* {{{ PDF_fit_textflow */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1fit_1textflow(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jtextflow, jdouble jllx, jdouble jlly, jdouble jurx,
    jdouble jury, jstring joptlist)
{
    PDF *p;
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    int textflow;
    double llx;
    double lly;
    double urx;
    double ury;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        textflow = (int )jtextflow;
        llx = dbl2nat(jllx);
        lly = dbl2nat(jlly);
        urx = dbl2nat(jurx);
        ury = dbl2nat(jury);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        _result = (char *)
		PDF_fit_textflow(p, textflow, llx, lly, urx, ury, optlist);
    } pdf_catch;

    if(_result)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_fit_textline */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1fit_1textline(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jdouble jx, jdouble jy, jstring joptlist)
{
    PDF *p;
    char *  PDF_VOLATILE text = NULL;
    double x;
    double y;
    char *  PDF_VOLATILE optlist = NULL;
    int len;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len);
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_fit_textline(p, text, len, x, y, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);
}
/* }}} */

/* {{{ PDF_get_apiname */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1get_1apiname(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    PDF *p;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        _result = (char *)PDF_get_apiname(p);
    } pdf_catch;

    if(_result)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_buffer */
JNIEXPORT jbyteArray JNICALL
Java_com_pdflib_pdflib_PDF_1get_1buffer(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    jbyteArray PDF_VOLATILE _jresult = 0;
    const unsigned char *buffer;
    PDF *p;
    long size;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        buffer = (const unsigned char *) PDF_get_buffer(p, &size);
        _jresult = (*jenv)->NewByteArray(jenv, (jsize) size);

        if (_jresult == (jbyteArray) NULL) {
            pdf_throw(p, "Java", "get_buffer",
                "Couldn't allocate PDF output buffer");
        } else {

            (*jenv)->SetByteArrayRegion(jenv, _jresult,
                        0, (jsize) size, (jbyte *) buffer);
        }
    } pdf_catch;

    return _jresult;
}
/* }}} */

/* {{{ PDF_get_errnum */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1get_1errnum(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result = 0;
    PDF *p;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        _result = PDF_get_errnum(p);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_errmsg */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1get_1errmsg(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    PDF *p;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        _result = (char *)PDF_get_errmsg(p);
    } pdf_catch;

    if(_result)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_parameter */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1get_1parameter(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jdouble jmodifier)
{
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    char * PDF_VOLATILE key = NULL;
    double modifier;
    PDF *p;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        key =  ConvertJavaString(p, jenv, jkey, PDF_BYTES, NULL);
        modifier = dbl2nat(jmodifier);

	_result = (char *) PDF_get_parameter(p, key, modifier);
    } pdf_catch;

    ReleaseCStringChars(key);

    if(_result)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_value */
JNIEXPORT jdouble JNICALL
Java_com_pdflib_pdflib_PDF_1get_1value(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jdouble jvaluex)
{
    jdouble _jresult = todouble(0);
    double PDF_VOLATILE _result;
    PDF *p;
    char *  PDF_VOLATILE key = NULL;
    double value;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        key =  ConvertJavaString(p, jenv, jkey, PDF_BYTES, NULL);
        value = dbl2nat(jvaluex);

	_result = PDF_get_value(p, key, value);
    } pdf_catch;

    ReleaseCStringChars(key);

    _jresult = todouble(_result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_pdi_parameter */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1get_1pdi_1parameter(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jint jdoc, jint jpage, jint jreserved)
{
    PDF *p;
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    char * PDF_VOLATILE key = NULL;
    int doc;
    int page;
    int reserved;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	doc = (int )jdoc;
	page = (int )jpage;
	reserved = (int )jreserved;
        key = ConvertJavaString(p, jenv, jkey, PDF_UTF8, NULL);

	_result = (char *)
	    PDF_get_pdi_parameter(p, key, doc, page, reserved, (int *)0);
    } pdf_catch;

    if(_result != NULL)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_get_pdi_value */
JNIEXPORT jdouble JNICALL
Java_com_pdflib_pdflib_PDF_1get_1pdi_1value(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jint jdoc, jint jpage, jint jreserved)
{
    PDF *p;
    jdouble _jresult = todouble(0);
    jdouble PDF_VOLATILE _result = todouble(0);
    char * PDF_VOLATILE key = NULL;
    int doc;
    int page;
    int reserved;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	doc = (int )jdoc;
	page = (int )jpage;
	reserved = (int )jreserved;
        key = ConvertJavaString(p, jenv, jkey, PDF_UTF8, NULL);

        _result = todouble(PDF_get_pdi_value(p, key, doc, page, reserved));
    } pdf_catch;

    _jresult = _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_info_textflow */
JNIEXPORT double JNICALL
Java_com_pdflib_pdflib_PDF_1info_1textflow(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jtextflow, jstring jkeyword)
{
    PDF *p;
    jdouble _jresult = todouble(0);
    jdouble PDF_VOLATILE _result = todouble(0);
    int textflow;
    char *  PDF_VOLATILE keyword = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        textflow = (int )jtextflow;
        keyword =  ConvertJavaString(p, jenv, jkeyword, PDF_BYTES, NULL);

        _result = PDF_info_textflow(p, textflow, keyword);
    } pdf_catch;

    ReleaseCStringChars(keyword);

    _jresult = _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_initgraphics */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1initgraphics(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_initgraphics(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_lineto */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1lineto(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy)
{
    PDF *p;
    double x;
    double y;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);

	PDF_lineto(p, x, y);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_load_font */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1load_1font(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfontname, jstring jencoding, jstring joptlist)
{
    jint _jresult = -1;
    int PDF_VOLATILE _result = -1;
    PDF *p;
    char * PDF_VOLATILE fontname = NULL;
    char * PDF_VOLATILE encoding = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        fontname = ConvertJavaString(p, jenv, jfontname, PDF_UTF16, &len1);
        encoding = ConvertJavaString(p, jenv, jencoding, PDF_BYTES, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_load_font(p,fontname, len1, encoding, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfontname, fontname);
    ReleaseCStringChars(encoding);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_load_iccprofile */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1load_1iccprofile(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jprofilename, jstring joptlist)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE profilename = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        profilename =
            ConvertJavaString(p, jenv, jprofilename, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_load_iccprofile(p, profilename, len1, optlist);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jprofilename, profilename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_load_image */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1load_1image(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jimagetype, jstring jfilename, jstring joptlist)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE imagetype = NULL;
    char * PDF_VOLATILE filename = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        imagetype = ConvertJavaString(p, jenv, jimagetype, PDF_BYTES, NULL);
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_load_image(p, imagetype, filename, len1, optlist);
    } pdf_catch;

    ReleaseCStringChars(imagetype);
    ReleaseJavaStringChars(jenv, jfilename, filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_makespotcolor */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1makespotcolor(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jspotname)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE spotname = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        spotname = ConvertJavaString(p, jenv, jspotname, PDF_BYTES, NULL);
	_result = (int )PDF_makespotcolor(p, spotname, 0);
    } pdf_catch;

    ReleaseCStringChars(spotname);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_moveto */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1moveto(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy)
{
    PDF *p;
    double x;
    double y;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);

	PDF_moveto(p, x, y);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_new */
JNIEXPORT jlong JNICALL
Java_com_pdflib_pdflib_PDF_1new(JNIEnv *jenv, jclass jcls)
{
    jlong _jresult;
    PDF * PDF_VOLATILE p = (PDF *) 0;
    EXC_STUFF;
    char jdkname[64];
    jclass stringClass;

    _jresult = int2ll(0);

    if ((ex = malloc(sizeof(pdf_wrapper_data))) == NULL) {
        jthrow(jenv, "Couldn't initialize PDFlib (malloc)", 0, "pdflib_java.c");
        return _jresult;
    }

    p = (PDF *)PDF_new2(NULL, NULL, NULL, NULL, (void *) ex);
    if (p == (PDF *)0) {
        jthrow(jenv, "Couldn't initialize PDF object due to memory shortage",
                        0, "pdflib_java.c");
        return _jresult;
    }

    PDF_TRY(p) {
        ex->jdkversion = (*jenv)->GetVersion(jenv);

        if (((*jenv)->ExceptionOccurred(jenv)) != NULL_INT) {
            (*jenv)->ExceptionDescribe(jenv);
            return _jresult;
        }

        sprintf(jdkname, "JDK %d.%d",
                (int) ((ex->jdkversion & 0xFF0000) >> 16),
                (int) (ex->jdkversion & 0xFF));
        PDF_set_parameter(p, "binding", jdkname);
        PDF_set_parameter(p, "objorient", "true");

        PDF_set_parameter(p, "textformat", "auto2");
        PDF_set_parameter(p, "hypertextformat", "auto2");
        PDF_set_parameter(p, "hypertextencoding", "");

/* "java/lang/String */
#define PDF_java_lang_String \
        "\x6a\x61\x76\x61\x2f\x6c\x61\x6e\x67\x2f\x53\x74\x72\x69\x6e\x67"
#define PDF_getBytes    "\x67\x65\x74\x42\x79\x74\x65\x73"      /* "getBytes" */
#define PDF_sig         "\x28\x29\x5B\x42"                      /* "()[B" */

        stringClass = (*jenv)->FindClass(jenv, PDF_java_lang_String);

        if (stringClass == NULL_INT) {
            (*jenv)->ExceptionDescribe(jenv);
            jthrow(jenv,
                "Couldn't initialize PDFlib (FindClass)", 0, "pdflib_java.c");

            _jresult = int2ll(0);

            return _jresult;
        }

        ex->MID_String_getBytes =
            (*jenv)->GetMethodID(jenv, stringClass, PDF_getBytes, PDF_sig);

        if (ex->MID_String_getBytes == NULL_INT) {
            (*jenv)->ExceptionDescribe(jenv);
            jthrow(jenv,
                "Couldn't initialize PDFlib (GetMethodID)", 0, "pdflib_java.c");
            return _jresult;
        }

    } pdf_catch;

    *(PDF **)&_jresult = p;

    return _jresult;
}
/* }}} */

/* {{{ PDF_open_CCITT */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1CCITT(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfilename, jint jwidth, jint jheight, jint jBitReverse,
    jint jK, jint jBlackIs1)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE filename = NULL;
    int width;
    int height;
    int BitReverse;
    int K;
    int BlackIs1;

    PDF_JAVA_SANITY_CHECK(jp);

    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_BYTES, NULL);
	width = (int )jwidth;
	height = (int )jheight;
	BitReverse = (int )jBitReverse;
	K = (int )jK;
	BlackIs1 = (int )jBlackIs1;

	_result = (int )PDF_open_CCITT(p, filename, width, height, BitReverse,
					    K, BlackIs1);
    } pdf_catch;

    ReleaseCStringChars(filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_open_file */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1file(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfilename)
{
    jint _jresult = -1;
    int PDF_VOLATILE _result = -1;
    PDF *p;
    char * PDF_VOLATILE filename = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_BYTES, NULL);
        _result = (int )PDF_open_file(p, filename);
    } pdf_catch;

    ReleaseCStringChars(filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_open_image */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1image(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jimagetype, jstring jsource, jbyteArray jdata,
    jlong jlength, jint jwidth, jint jheight, jint jcomponents,
    jint jbpc, jstring jparams)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE imagetype = NULL;
    char * PDF_VOLATILE source = NULL;
    char * PDF_VOLATILE data = NULL;
    long length;
    int width;
    int height;
    int components;
    int bpc;
    char * PDF_VOLATILE params = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	data = (char *)
	    (*jenv)->GetByteArrayElements(jenv, jdata, (jboolean *) NULL);
        imagetype = ConvertJavaString(p, jenv, jimagetype, PDF_BYTES, NULL);
        source = ConvertJavaString(p, jenv, jsource, PDF_BYTES, NULL);

	length = (long )jlength;

	width = (int )jwidth;
	height = (int )jheight;
	components = (int )jcomponents;
	bpc = (int )jbpc;
        params = ConvertJavaString(p, jenv, jparams, PDF_BYTES, NULL);

	_result = (int )PDF_open_image(p, imagetype, source, data, length,
		     width, height, components, bpc, params);
    } pdf_catch;

    ReleaseCStringChars(imagetype);
    ReleaseCStringChars(source);
    (*jenv)->ReleaseByteArrayElements(jenv, jdata, (jbyte*) data, JNI_ABORT);
    ReleaseCStringChars(params);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_open_image_file */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1image_1file(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jimagetype, jstring jfilename, jstring jstringparam,
    jint jintparam)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE imagetype = NULL;
    char * PDF_VOLATILE filename = NULL;
    char * PDF_VOLATILE stringparam = NULL;
    int intparam;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        imagetype = ConvertJavaString(p, jenv, jimagetype, PDF_BYTES, NULL);
        filename = ConvertJavaString(p, jenv, jfilename, PDF_BYTES, NULL);
        stringparam = ConvertJavaString(p, jenv, jstringparam, PDF_BYTES, NULL);
	intparam = (int )jintparam;

	_result = (int )PDF_open_image_file(p, imagetype, filename,
				    stringparam, intparam);
    } pdf_catch;

    ReleaseCStringChars(imagetype);
    ReleaseCStringChars(filename);
    ReleaseCStringChars(stringparam);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_open_pdi */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1pdi(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfilename, jstring joptlist, jint jreserved)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE filename = NULL;
    char * PDF_VOLATILE optlist = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        filename = ConvertJavaString(p, jenv, jfilename, PDF_UTF16, &len1);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

        _result = (int )PDF_open_pdi(p, filename, optlist, len1);

    } pdf_catch;

    ReleaseJavaStringChars(jenv, jfilename, filename);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_open_pdi_page */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1open_1pdi_1page(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jdoc, jint jpagenumber, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    int doc;
    int pagenumber;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	doc = (int )jdoc;
	pagenumber = (int )jpagenumber;
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_open_pdi_page(p, doc, pagenumber, optlist);
    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_place_image */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1place_1image(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jimage, jdouble jx, jdouble jy, jdouble jscale)
{
    PDF *p;
    int image;
    double x;
    double y;
    double scale;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	image = (int )jimage;
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        scale = dbl2nat(jscale);

	PDF_place_image(p, image, x, y, scale);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_place_pdi_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1place_1pdi_1page(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jpage, jdouble jx, jdouble jy, jdouble jsx, jdouble jsy)
{
    PDF *p;
    int page;
    double x;
    double y;
    double sx;
    double sy;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	page = (int )jpage;
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        sx = dbl2nat(jsx);
        sy = dbl2nat(jsy);

	PDF_place_pdi_page(p, page, x, y, sx, sy);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_process_pdi */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1process_1pdi(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jdoc, jint jpage, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    int doc;
    int page;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	doc = (int )jdoc;
	page = (int )jpage;
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_process_pdi(p, doc, page, optlist);

    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_rect */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1rect(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy, jdouble jwidth, jdouble jheight)
{
    PDF *p;
    double x;
    double y;
    double width;
    double height;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);


    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);

	PDF_rect(p, x, y, width, height);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_restore */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1restore(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_restore(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_resume_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1resume_1page(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
    jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        PDF_resume_page(p, optlist);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_rotate */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1rotate(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jphi)
{
    PDF *p;
    double phi;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);
    phi = dbl2nat(jphi);

    PDF_TRY(p) {     PDF_rotate(p, phi);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_save */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1save(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_save(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_scale */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1scale(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jsx, jdouble jsy)
{
    PDF *p;
    double sx;
    double sy;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        sx = dbl2nat(jsx);
        sy = dbl2nat(jsy);

	PDF_scale(p, sx, sy);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_set_border_color */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1border_1color(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jred, jdouble jgreen, jdouble jblue)
{
    PDF *p;
    double red;
    double green;
    double blue;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);
    red = dbl2nat(jred);
    green = dbl2nat(jgreen);
    blue = dbl2nat(jblue);

    PDF_TRY(p) {     PDF_set_border_color(p, red, green, blue);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_set_border_dash */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1border_1dash(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jb, jdouble jw)
{
    PDF *p;
    double b;
    double w;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);
    b = dbl2nat(jb);
    w = dbl2nat(jw);

    PDF_TRY(p) {     PDF_set_border_dash(p, b, w);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_set_border_style */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1border_1style(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jstyle, jdouble jwidth)
{
    PDF *p;
    char * PDF_VOLATILE style = NULL;
    double width;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        style = ConvertJavaString(p, jenv, jstyle, PDF_BYTES, NULL);
        width = dbl2nat(jwidth);

	PDF_set_border_style(p, style, width);
    } pdf_catch;

    ReleaseCStringChars(style);
}
/* }}} */

/* {{{ PDF_set_gstate */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1gstate(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jgstate)
{
    PDF *p;
    int gstate;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	gstate = (int)jgstate;

	PDF_set_gstate(p, gstate);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_set_info */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1info(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jstring jvaluex)
{
    PDF *p;
    char * PDF_VOLATILE key = NULL;
    char * PDF_VOLATILE value = NULL;
    int len2;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        key = ConvertJavaString(p, jenv, jkey, PDF_UTF8, NULL);
        value = ConvertJavaString(p, jenv, jvaluex, PDF_UTF16, &len2);

	PDF_set_info2(p, key, value, len2);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jvaluex, value);
}
/* }}} */

/* {{{ PDF_set_layer_dependency */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1layer_1dependency(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtype, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE type = NULL;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        type = ConvertJavaString(p, jenv, jtype, PDF_BYTES, NULL);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_set_layer_dependency(p, type, optlist);
    } pdf_catch;

    ReleaseCStringChars(type);
}

/* }}} */

/* {{{ PDF_set_parameter */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1parameter(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jstring jvaluex)
{
    PDF *p;
    char * PDF_VOLATILE key = NULL;
    char * PDF_VOLATILE value = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        key = ConvertJavaString(p, jenv, jkey, PDF_BYTES, NULL);
        value = ConvertJavaString(p, jenv, jvaluex, PDF_UTF8, NULL);

        PDF_set_parameter(p, key, value);

    } pdf_catch;

    ReleaseCStringChars(key);
}
/* }}} */

/* {{{ PDF_set_text_pos */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1text_1pos(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jx, jdouble jy)
{
    PDF *p;
    double x;
    double y;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        x = dbl2nat(jx);
        y = dbl2nat(jy);

	PDF_set_text_pos(p, x, y);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_set_value */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1set_1value(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jkey, jdouble jvaluex)
{
    PDF *p;
    char *  PDF_VOLATILE key = NULL;
    double value;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        key =  ConvertJavaString(p, jenv, jkey, PDF_BYTES, NULL);
        value = dbl2nat(jvaluex);

	PDF_set_value(p, key, value);
    } pdf_catch;

    ReleaseCStringChars(key);
}
/* }}} */

/* {{{ PDF_setcolor */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setcolor(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jfstype, jstring jcolorspace, jdouble jc1, jdouble jc2,
    jdouble jc3, jdouble jc4)
{
    PDF *p;
    char * PDF_VOLATILE fstype = NULL;
    char * PDF_VOLATILE colorspace = NULL;
    double c1;
    double c2;
    double c3;
    double c4;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);
    c1 = dbl2nat(jc1);
    c2 = dbl2nat(jc2);
    c3 = dbl2nat(jc3);
    c4 = dbl2nat(jc4);

    PDF_TRY(p) {
        fstype = ConvertJavaString(p, jenv, jfstype, PDF_BYTES, NULL);
        colorspace = ConvertJavaString(p, jenv, jcolorspace, PDF_BYTES, NULL);
	PDF_setcolor(p, fstype, colorspace, c1, c2, c3, c4);
    } pdf_catch;

    ReleaseCStringChars(fstype);
    ReleaseCStringChars(colorspace);
}
/* }}} */

/* {{{ PDF_setdash */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setdash(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jb, jdouble jw)
{
    PDF *p;
    double b;
    double w;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        b = dbl2nat(jb);
        w = dbl2nat(jw);

	PDF_setdash(p, b, w);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setdashpattern */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setdashpattern(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	PDF_setdashpattern(p, optlist);
    } pdf_catch;

}
/* }}} */

/* {{{ PDF_setflat */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setflat(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jflatness)
{
    PDF *p;
    double flatness;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        flatness = dbl2nat(jflatness);

	PDF_setflat(p, flatness);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setfont */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setfont(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jfont, jdouble jfontsize)
{
    PDF *p;
    int font;
    double fontsize;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	font = (int )jfont;
        fontsize = dbl2nat(jfontsize);
	PDF_setfont(p, font, fontsize);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setlinecap */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setlinecap(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jlinecap)
{
    PDF *p;
    int linecap;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	linecap = (int )jlinecap;

	PDF_setlinecap(p, linecap);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setlinejoin */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setlinejoin(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jlinejoin)
{
    PDF *p;
    int linejoin;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	linejoin = (int )jlinejoin;

	PDF_setlinejoin(p, linejoin);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setlinewidth */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setlinewidth(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jwidth)
{
    PDF *p;
    double width;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        width = dbl2nat(jwidth);

	PDF_setlinewidth(p, width);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setmatrix */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setmatrix(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble ja, jdouble jb, jdouble jc, jdouble jd,
    jdouble je, jdouble jf)
{
    PDF *p;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        a = dbl2nat(ja);
        b = dbl2nat(jb);
        c = dbl2nat(jc);
        d = dbl2nat(jd);
        e = dbl2nat(je);
        f = dbl2nat(jf);

	PDF_setmatrix(p, a, b, c, d, e, f);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setmiterlimit */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setmiterlimit(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jmiter)
{
    PDF *p;
    double miter;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        miter = dbl2nat(jmiter);

	PDF_setmiterlimit(p, miter);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_setpolydash */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1setpolydash(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jfloatArray jdasharray)
{
    PDF *p;
    float *carray;
    jfloat* javaarray;
    int i;
    jsize PDF_VOLATILE length;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	length = (*jenv)->GetArrayLength(jenv, jdasharray);
	carray = (float *) malloc(sizeof(float) * length);
	if (carray == NULL)
	{
	    jthrow(jenv, "Out of memory in PDF_set_polydash", 0,
	    	"pdflib_java.c");
	    return;
	}

	javaarray = (*jenv)->GetFloatArrayElements(jenv, jdasharray, 0);

	for(i=0; i < length; i++)
            carray[i] = (float) javaarray[i];

	PDF_setpolydash(p, carray, length);
    } pdf_catch;

    (*jenv)->ReleaseFloatArrayElements(jenv, jdasharray, javaarray, 0);
    free(carray);
}
/* }}} */

/* {{{ PDF_shading */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1shading(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
    jstring jshtype, jdouble jx0, jdouble jy0, jdouble jx1, jdouble jy1,
    jdouble jc1, jdouble jc2, jdouble jc3, jdouble jc4, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE shtype = NULL;
    char * PDF_VOLATILE optlist = NULL;
    double x0, y0, x1, y1, c1, c2, c3, c4;


    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        shtype = ConvertJavaString(p, jenv, jshtype, PDF_BYTES, NULL);
        x0 = dbl2nat(jx0);
        y0 = dbl2nat(jy0);
        x1 = dbl2nat(jx1);
        y1 = dbl2nat(jy1);
        c1 = dbl2nat(jc1);
        c2 = dbl2nat(jc2);
        c3 = dbl2nat(jc3);
        c4 = dbl2nat(jc4);
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_shading(p, shtype, x0, y0, x1, y1, c1, c2, c3, c4,
					optlist);

    } pdf_catch;

    ReleaseCStringChars(shtype);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_shading_pattern */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1shading_1pattern(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jshading, jstring joptlist)
{
    PDF *p;
    jint _jresult = 0;
    int PDF_VOLATILE _result = -1;
    char * PDF_VOLATILE optlist = NULL;
    int shading;


    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	shading = (int)jshading;
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);

	_result = (int )PDF_shading_pattern(p, shading, optlist);

    } pdf_catch;

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_shfill */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1shfill(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jint jshading)
{
    PDF *p;
    int shading;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
	shading = (int )jshading;

	PDF_shfill(p, shading);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_show */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1show(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext)
{
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);

    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);

	PDF_show2(p, text, len1);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);
}
/* }}} */

/* {{{ PDF_show_boxed */
JNIEXPORT jint JNICALL
Java_com_pdflib_pdflib_PDF_1show_1boxed(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jdouble jleft, jdouble jtop, jdouble jwidth,
    jdouble jheight, jstring jhmode, jstring jfeature)
{
    jint _jresult = 0;
    int PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    double left;
    double top;
    double width;
    double height;
    char * PDF_VOLATILE hmode = NULL;
    char * PDF_VOLATILE feature = NULL;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);
        left = dbl2nat(jleft);
        top = dbl2nat(jtop);
        width = dbl2nat(jwidth);
        height = dbl2nat(jheight);
        hmode = ConvertJavaString(p, jenv, jhmode, PDF_BYTES, NULL);
        feature = ConvertJavaString(p, jenv, jfeature, PDF_BYTES, NULL);

	_result = PDF_show_boxed2(p, text, len1, left, top, width, height,
                                  hmode, feature);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);
    ReleaseCStringChars(hmode);
    ReleaseCStringChars(feature);

    _jresult = (jint) _result;
    return _jresult;
}
/* }}} */

/* {{{ PDF_show_xy */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1show_1xy(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jdouble jx, jdouble jy)
{
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    double x;
    double y;
    int len1;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);
        x = dbl2nat(jx);
        y = dbl2nat(jy);

	PDF_show_xy2(p, text, len1,  x, y);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);
}
/* }}} */

/* {{{ PDF_shutdown */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1shutdown(JNIEnv *jenv, jclass jcls)
{
    /* throws nothing */
    PDF_shutdown();
}
/* }}} */

/* {{{ PDF_skew */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1skew(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jalpha, jdouble jbeta)
{
    PDF *p;
    double alpha;
    double beta;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        alpha = dbl2nat(jalpha);
        beta = dbl2nat(jbeta);

	PDF_skew(p, alpha, beta);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_stringwidth */
JNIEXPORT jdouble JNICALL
Java_com_pdflib_pdflib_PDF_1stringwidth(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jtext, jint jfont, jdouble jfontsize)
{
    jdouble _jresult = todouble(0);
    double PDF_VOLATILE _result;
    PDF *p;
    char * PDF_VOLATILE text = NULL;
    int font;
    double fontsize;
    int len1;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        text = ConvertJavaString(p, jenv, jtext, PDF_UTF16, &len1);
	font = (int )jfont;
        fontsize = dbl2nat(jfontsize);

	_result = PDF_stringwidth2(p, text, len1, font, fontsize);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jtext, text);

    _jresult = todouble(_result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_stroke */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1stroke(JNIEnv *jenv, jclass jcls, PDF_ENV jp)
{
    PDF *p;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {     PDF_stroke(p);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_suspend_page */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1suspend_1page(JNIEnv *jenv, jclass jcls, PDF_ENV jp,
    jstring joptlist)
{
    PDF *p;
    char * PDF_VOLATILE optlist = NULL;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        optlist = ConvertJavaString(p, jenv, joptlist, PDF_UTF8, NULL);
        PDF_suspend_page(p, optlist);
    } pdf_catch;
}
/* }}} */

/* {{{ PDF_translate */
JNIEXPORT void JNICALL
Java_com_pdflib_pdflib_PDF_1translate(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jdouble jtx, jdouble jty)
{
    PDF *p;
    double tx;
    double ty;

    PDF_JAVA_SANITY_CHECK_VOID(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        tx = dbl2nat(jtx);
        ty = dbl2nat(jty);

	PDF_translate(p, tx, ty);
    } pdf_catch;
}
/* }}} */

#if 0
/* {{{ PDF_utf16_to_utf8 */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1utf16_1to_1utf8(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jutf16string)
{
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    char * PDF_VOLATILE utf16string = NULL;
    PDF *p;
    int len;
    int reslen;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        utf16string = ConvertJavaString(p, jenv, jutf16string, PDF_UTF16, &len);

        _result = (char *) PDF_utf16_to_utf8(p, utf16string, len, &reslen);
    } pdf_catch;

    ReleaseJavaStringChars(jenv, jutf16string, utf16string);

    if(_result)
        _jresult = (jstring)GetNewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */

/* {{{ PDF_utf8_to_utf16 */
JNIEXPORT jstring JNICALL
Java_com_pdflib_pdflib_PDF_1utf8_1to_1utf16(JNIEnv *jenv, jclass jcls,
    PDF_ENV jp, jstring jutf8string, jstring jformat)
{
    jstring PDF_VOLATILE _jresult = 0;
    char * PDF_VOLATILE _result = NULL;
    char * PDF_VOLATILE utf8string = NULL;
    char * PDF_VOLATILE format = NULL;
    PDF *p;
    int len;
    int reslen;

    PDF_JAVA_SANITY_CHECK(jp);
    p = PDF_ENV2PTR(jp);

    PDF_TRY(p) {
        utf8string = ConvertJavaString(p, jenv, jutf8string, PDF_BYTES, NULL);
        format = ConvertJavaString(p, jenv, jformat, PDF_BYTES, NULL);

        _result = (char *) PDF_utf8_to_utf16(p, utf8string, format, &reslen);
    } pdf_catch;

    ReleaseCStringChars(utf8string);
    ReleaseCStringChars(format);

    if(_result)
        _jresult = (jstring)(*jenv)->NewStringUTF(jenv, _result);
    return _jresult;
}
/* }}} */
#endif


/*
 * vim600: sw=4 fdm=marker
 */
