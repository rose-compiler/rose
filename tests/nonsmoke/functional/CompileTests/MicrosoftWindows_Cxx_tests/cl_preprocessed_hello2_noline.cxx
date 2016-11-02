
// DQ: added
// typedef unsigned long size_t;
#define __cdecl
#define __w64



// #line 1 "hello2.cxx"
class type_info;
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\climits"

#pragma once


// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 





// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














// #line 38 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 42 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




















    

// #line 65 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 67 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"














#pragma once































































































// #line 112 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"




// #line 117 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"




// #line 122 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"
// #line 123 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"





















// #line 145 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"



// #line 149 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"



































































































































































































































































































































































































































































































































































































































































                                                




                                                






































































































































// #line 1 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"


#pragma once








// #line 13 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"
// #line 14 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"




// DQ: removed
// typedef __w64 unsigned int   size_t;
// #line 21 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

// #line 23 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"












// #line 40 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"


namespace vc_attributes
{
// #line 45 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

enum YesNoMaybe
{
	
	No = 0x0fff0001,
	Maybe = 0x0fff0010,
	Yes = 0x0fff0100
};

typedef enum YesNoMaybe YesNoMaybe;

enum AccessType
{
	NoAccess = 0,
	Read = 1,
	Write = 2,
	ReadWrite = 3
};

typedef enum AccessType AccessType;



[repeatable]
[source_annotation_attribute( Parameter )]
struct PreAttribute
{

	PreAttribute();
// #line 75 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

	unsigned int Deref;
	YesNoMaybe Valid;
	YesNoMaybe Null;
	YesNoMaybe Tainted;
	AccessType Access;
	size_t ValidElementsConst;
	size_t ValidBytesConst;
	const wchar_t* ValidElements;
	const wchar_t* ValidBytes;
	const wchar_t* ValidElementsLength;
	const wchar_t* ValidBytesLength;
	size_t WritableElementsConst;
	size_t WritableBytesConst;
	const wchar_t* WritableElements;
	const wchar_t* WritableBytes;
	const wchar_t* WritableElementsLength;
	const wchar_t* WritableBytesLength;
	size_t ElementSizeConst;
	const wchar_t* ElementSize;
	YesNoMaybe NullTerminated;
	const wchar_t* Condition;
};

[repeatable]
[source_annotation_attribute( Parameter|ReturnValue )]
struct PostAttribute
{

	PostAttribute();
// #line 106 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

	unsigned int Deref;
	YesNoMaybe Valid;
	YesNoMaybe Null;
	YesNoMaybe Tainted;
	AccessType Access;
	size_t ValidElementsConst;
	size_t ValidBytesConst;
	const wchar_t* ValidElements;
	const wchar_t* ValidBytes;
	const wchar_t* ValidElementsLength;
	const wchar_t* ValidBytesLength;
	size_t WritableElementsConst;
	size_t WritableBytesConst;
	const wchar_t* WritableElements;
	const wchar_t* WritableBytes;
	const wchar_t* WritableElementsLength;
	const wchar_t* WritableBytesLength;
	size_t ElementSizeConst;
	const wchar_t* ElementSize;
	YesNoMaybe NullTerminated;
	YesNoMaybe MustCheck;
	const wchar_t* Condition;
};

[source_annotation_attribute( Parameter )]
struct FormatStringAttribute
{

	FormatStringAttribute();
// #line 137 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

	const wchar_t* Style;
	const wchar_t* UnformattedAlternative;
};

[repeatable]
[source_annotation_attribute( ReturnValue )]
struct InvalidCheckAttribute
{

	InvalidCheckAttribute();
// #line 149 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

	long Value;
};

[source_annotation_attribute( Method )]
struct SuccessAttribute
{

	SuccessAttribute();
// #line 159 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

	const wchar_t* Condition;
};

[repeatable]
[source_annotation_attribute( Parameter )]
struct PreBoundAttribute
{

	PreBoundAttribute();
// #line 170 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"
	unsigned int Deref;
};

[repeatable]
[source_annotation_attribute( Parameter|ReturnValue )]
struct PostBoundAttribute
{

	PostBoundAttribute();
// #line 180 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"
	unsigned int Deref;
};

[repeatable]
[source_annotation_attribute( Parameter )]
struct PreRangeAttribute
{

	PreRangeAttribute();
// #line 190 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"
	unsigned int Deref;
	const char* MinVal;
	const char* MaxVal;
};

[repeatable]
[source_annotation_attribute( Parameter|ReturnValue )]
struct PostRangeAttribute
{

	PostRangeAttribute();
// #line 202 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"
	unsigned int Deref;
	const char* MinVal;
	const char* MaxVal;
};

// #line 208 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"


};  
// #line 212 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"






















typedef ::vc_attributes::YesNoMaybe SA_YesNoMaybe;
const ::vc_attributes::YesNoMaybe SA_Yes = ::vc_attributes::Yes;
const ::vc_attributes::YesNoMaybe SA_No = ::vc_attributes::No;
const ::vc_attributes::YesNoMaybe SA_Maybe = ::vc_attributes::Maybe;

typedef ::vc_attributes::AccessType SA_AccessType;
const ::vc_attributes::AccessType SA_NoAccess = ::vc_attributes::NoAccess;
const ::vc_attributes::AccessType SA_Read = ::vc_attributes::Read;
const ::vc_attributes::AccessType SA_Write = ::vc_attributes::Write;
const ::vc_attributes::AccessType SA_ReadWrite = ::vc_attributes::ReadWrite;


typedef ::vc_attributes::PreAttribute          SA_Pre;
typedef ::vc_attributes::PostAttribute         SA_Post;
typedef ::vc_attributes::FormatStringAttribute SA_FormatString;
typedef ::vc_attributes::InvalidCheckAttribute SA_InvalidCheck; 
typedef ::vc_attributes::SuccessAttribute      SA_Success;
typedef ::vc_attributes::PreBoundAttribute     SA_PreBound;
typedef ::vc_attributes::PostBoundAttribute    SA_PostBound;
typedef ::vc_attributes::PreRangeAttribute     SA_PreRange;
typedef ::vc_attributes::PostRangeAttribute    SA_PostRange;
// #line 256 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"















// #line 272 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

// #line 274 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"




















// #line 295 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"


// #line 298 "c:\\program files (x86)\\microsoft visual studio 9.0\\vc\\include\\codeanalysis\\sourceannotations.h"

// #line 933 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"













































































































































































































































































































































// #line 1267 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"



















































































































































































































// #line 1479 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"
extern "C" {




// #line 1485 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"




























































































































































































































// #line 1706 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
// #line 1740 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"



































































































































































































































    
    
// #line 1970 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"






// #line 1977 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"
// #line 1978 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"


}
// #line 1982 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\sal.h"



// #line 69 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"























































































#pragma pack(push,8)
// #line 158 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"













#pragma once
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"






// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"









#pragma pack(push,8)
// #line 34 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"


extern "C" {
// #line 38 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"








// #line 47 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"





typedef __w64 unsigned int   uintptr_t;
// #line 54 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"

// #line 56 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"





typedef char *  va_list;
// #line 63 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"

// #line 65 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"





// #line 71 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"











// #line 83 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"


// #line 86 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"













// #line 100 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"












































// #line 145 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"


}
// #line 149 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"


#pragma pack(pop)
// #line 153 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"

// #line 155 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\vadefs.h"
// #line 160 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


extern "C" {
// #line 164 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 170 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 175 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 180 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 188 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 195 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 197 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 198 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 205 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 207 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 208 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"













// #line 222 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 223 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 229 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 237 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 239 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 241 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 249 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 250 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 255 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 257 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 258 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 263 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 265 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 266 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


 

// #line 271 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
  
 // #line 273 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 274 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"










// #line 285 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 286 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 293 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 294 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

















// #line 312 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 317 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 324 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 330 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 337 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 338 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 344 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 351 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 352 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 359 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 360 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 365 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


// #line 368 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 370 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 371 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 372 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 378 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"










// #line 389 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 391 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 392 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 393 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"










// #line 404 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 411 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 412 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 420 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 422 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 423 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



 
  
 



// #line 433 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 434 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


 
  
  
 



// #line 444 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 445 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


 
  
   
  

// #line 453 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
 



// #line 458 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 459 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





// #line 465 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 470 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 472 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 473 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"












typedef size_t rsize_t;

// #line 488 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 489 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





typedef __w64 int            intptr_t;
// #line 496 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 498 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














typedef __w64 int            ptrdiff_t;
// #line 514 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 516 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







typedef unsigned short wint_t;
typedef unsigned short wctype_t;

// #line 527 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





















__declspec(deprecated("This name was supported during some Whidbey pre-releases. Instead, use the standard name errno_t.")) typedef int errcode;


// #line 552 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

typedef int errno_t;
// #line 555 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


typedef __w64 long __time32_t;   

// #line 560 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


typedef __int64 __time64_t;     

// #line 565 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"





typedef __time64_t time_t;      
// #line 572 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 574 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 582 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 583 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 588 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 590 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 591 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 596 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 598 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 599 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 606 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 607 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 612 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 616 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"











// #line 628 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 633 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"




// #line 638 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 640 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 641 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"







// #line 649 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


































































































































































// #line 812 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 813 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"









































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 1879 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"















































































































































// #line 2023 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"
// #line 2024 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct * pthreadlocinfo;
typedef struct threadmbcinfostruct * pthreadmbcinfo;
struct __lc_time_data;

typedef struct localeinfo_struct
{
    pthreadlocinfo locinfo;
    pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;


typedef struct tagLC_ID {
        unsigned short wLanguage;
        unsigned short wCountry;
        unsigned short wCodePage;
} LC_ID, *LPLC_ID;

// #line 2045 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


typedef struct threadlocaleinfostruct {
        int refcount;
        unsigned int lc_codepage;
        unsigned int lc_collate_cp;
        unsigned long lc_handle[6]; 
        LC_ID lc_id[6];
        struct {
            char *locale;
            wchar_t *wlocale;
            int *refcount;
            int *wrefcount;
        } lc_category[6];
        int lc_clike;
        int mb_cur_max;
        int * lconv_intl_refcount;
        int * lconv_num_refcount;
        int * lconv_mon_refcount;
        struct lconv * lconv;
        int * ctype1_refcount;
        unsigned short * ctype1;
        const unsigned short * pctype;
        const unsigned char * pclmap;
        const unsigned char * pcumap;
        struct __lc_time_data * lc_time_curr;
} threadlocinfo;

// #line 2074 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


}
// #line 2078 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 2082 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 2084 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 2088 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 2090 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 2094 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 2096 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"






// #line 2103 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"



// #line 2107 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"


#pragma pack(pop)
// #line 2111 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 2113 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"

// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


#pragma pack(push,8)
// #line 11 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"









































// #line 53 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

// #line 55 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
// #line 56 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

		


		




		

 
  

 

// #line 73 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 
  
 // #line 77 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 
  
 // #line 81 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
// #line 85 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
// #line 89 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
// #line 93 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"



// #line 97 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


	

// #line 102 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
		
	// #line 104 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"







// #line 112 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"













// #line 126 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"




// #line 131 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"




// #line 136 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

// #line 138 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
// #line 139 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"



// #line 143 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"











// #line 155 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"








// #line 164 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"






 









// #line 181 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 // #line 183 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 




// #line 190 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 
 

 // #line 195 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 









 










 









 

 









 









 




 





 
 

 
















// #line 281 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"











 

 

 

 










 










 










 










 









// #line 353 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"











// #line 365 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"













#pragma once































































































// #line 111 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"



#pragma comment(lib,"libcpmt")
// #line 116 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"
// #line 117 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"

// #line 119 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"

// #line 121 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"

// #line 123 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\use_ansi.h"
// #line 367 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
// #line 371 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"




// #line 376 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 











 
  

// #line 394 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
   
  // #line 396 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
 // #line 397 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"






 











 

   


// #line 421 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
    
   // #line 423 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 // #line 425 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
  

// #line 431 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
   
  // #line 433 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
 // #line 434 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
  

// #line 440 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
   
  // #line 442 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
 // #line 443 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


 
  

// #line 449 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
   
  // #line 451 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
 // #line 452 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 

// #line 456 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 
   


     
   // #line 463 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
 // #line 464 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


    
// #line 468 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"


  



		

 
  
  
  




  
  
  

  







   
   
   
  // #line 500 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

  
  
  
  

 












// #line 520 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

 

 
namespace std {
typedef bool _Bool;
}
 // #line 528 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

		





		






typedef __int64 _Longlong;
typedef unsigned __int64 _ULonglong;

		


 
  
 // #line 551 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"





		
		






 
namespace std {
		


#pragma warning(push)
#pragma warning(disable:4412)
class  _Lockit
	{	
public:
  

    













// #line 591 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
    explicit __thiscall _Lockit();	
	explicit __thiscall _Lockit(int);	
	__thiscall ~_Lockit();	
    // #line 595 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

    static  void __cdecl _Lockit_ctor(int);
    static  void __cdecl _Lockit_dtor(int);

private:
    static  void __cdecl _Lockit_ctor(_Lockit *);
    static  void __cdecl _Lockit_ctor(_Lockit *, int);
    static  void __cdecl _Lockit_dtor(_Lockit *);

	 _Lockit(const _Lockit&);				
	_Lockit&  operator=(const _Lockit&);	

	int _Locktype;

  












// #line 623 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

	};



























































































// #line 717 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

class  _Mutex
	{	
public:

  
    
    
















// #line 742 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
    __thiscall _Mutex();
	__thiscall ~_Mutex();
	void __thiscall _Lock();
	void __thiscall _Unlock();
    // #line 747 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

private:
    static  void __cdecl _Mutex_ctor(_Mutex *);
    static  void __cdecl _Mutex_dtor(_Mutex *);
    static  void __cdecl _Mutex_Lock(_Mutex *);
    static  void __cdecl _Mutex_Unlock(_Mutex *);

	 _Mutex(const _Mutex&);				
	_Mutex&  operator=(const _Mutex&);	
	void *_Mtx;

  







// #line 767 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

	};

class  _Init_locks
	{	
public:

 
    
    








// #line 786 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"
    __thiscall _Init_locks();
	__thiscall ~_Init_locks();
    // #line 789 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

private:
    static  void __cdecl _Init_locks_ctor(_Init_locks *);
    static  void __cdecl _Init_locks_dtor(_Init_locks *);

 







// #line 803 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

	};
#pragma warning(pop)
}
 // #line 808 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"





		

 void __cdecl _Atexit(void (__cdecl *)(void));

typedef int _Mbstatet;






  
  
 #pragma pack(pop)
// #line 828 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"

// #line 830 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\yvals.h"






// #line 6 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\climits"

 #pragma warning(disable: 4514)

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 20 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"
















// #line 37 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"









































// #line 79 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"






// #line 86 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"
// #line 87 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"







// #line 95 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"
// #line 96 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"
































// #line 129 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\limits.h"
// #line 10 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\climits"
// #line 11 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\climits"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"

#pragma once










 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"





#pragma pack(push,8)
// #line 30 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


extern "C" {
// #line 34 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

























struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;

// #line 72 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"










// #line 83 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"













// #line 97 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"





















// #line 119 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"







// #line 127 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
// #line 128 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"




 FILE * __cdecl __iob_func(void);
// #line 134 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"










// #line 145 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"



typedef __int64 fpos_t;




// #line 154 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
// #line 155 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


// #line 158 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"






// #line 165 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
























[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _filbuf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File );
  int __cdecl _flsbuf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);




[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _fsopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ShFlag);
// #line 197 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

 void __cdecl clearerr([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  errno_t __cdecl clearerr_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File );
// #line 202 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl fclose([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fcloseall(void);




[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _fdopen([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _FileHandle, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode);
// #line 210 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl feof([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl ferror([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] FILE * _File);
  int __cdecl fflush([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl fgetc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fgetchar(void);
  int __cdecl fgetpos([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File , [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] fpos_t * _Pos);
  char * __cdecl fgets([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _MaxCount, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);




[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _fileno([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] FILE * _File);
// #line 224 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"




// #line 229 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  char * __cdecl _tempnam([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _DirName, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _FilePrefix);



// #line 235 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

  int __cdecl _flushall(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "fopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  FILE * __cdecl fopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode);

  errno_t __cdecl fopen_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] FILE ** _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode);
// #line 241 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl fprintf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);

  int __cdecl fprintf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
// #line 245 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl fputc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fputchar([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch);
  int __cdecl fputs([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  size_t __cdecl fread([SA_Pre(Null=SA_No,WritableBytes="\n@""_ElementSize*_Count")] [SA_Pre(Deref=1,Valid=SA_No)] void * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ElementSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  size_t __cdecl fread_s([SA_Pre(Null=SA_No,WritableBytes="\n@""_ElementSize*_Count")] [SA_Pre(Deref=1,Valid=SA_No)] void * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ElementSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
// #line 252 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "freopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  FILE * __cdecl freopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  errno_t __cdecl freopen_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] FILE ** _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _OldFile);
// #line 256 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "fscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl fscanf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _fscanf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
#pragma warning(push)
#pragma warning(disable:6530)

  int __cdecl fscanf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, ...);
// #line 263 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _fscanf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl fsetpos([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const fpos_t * _Pos);
  int __cdecl fseek([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Offset, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Origin);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long __cdecl ftell([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  int __cdecl _fseeki64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Offset, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Origin);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _ftelli64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  size_t __cdecl fwrite([SA_Pre(Null=SA_No,ValidElements="\n@""_Size*_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl getc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl getchar(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _getmaxstdio(void);

 char * __cdecl gets_s([SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _Size);
// #line 278 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { template <size_t _Size> inline char * __cdecl gets_s(char (&_Buffer)[_Size]) { return gets_s(_Buffer, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "gets_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl gets([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Null=SA_No,WritableElementsConst=((size_t)-1))] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Buffer);
[returnvalue:SA_Post(MustCheck=SA_Yes)] int __cdecl _getw([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);


 void __cdecl perror([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _ErrMsg);
// #line 285 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _pclose([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _popen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Command, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Mode);
  int __cdecl printf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);

  int __cdecl printf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
// #line 291 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl putc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl putchar([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch);
  int __cdecl puts([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str);
  int __cdecl _putw([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Word, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);


[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl remove([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl rename([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _OldFilename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _NewFilename);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _unlink([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename);

[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_unlink" ". See online help for details."))  int __cdecl unlink([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename);
// #line 303 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
// #line 304 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
 void __cdecl rewind([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _rmtmp(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "scanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl scanf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_scanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _scanf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);

  int __cdecl scanf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, ...);
// #line 311 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _scanf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
__declspec(deprecated("This function or variable may be unsafe. Consider using " "setvbuf" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void __cdecl setbuf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_Maybe,WritableElementsConst=512)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Post(ValidElementsConst=0)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buffer);
  int __cdecl _setmaxstdio([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Max);
  unsigned int __cdecl _set_output_format([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Format);
  unsigned int __cdecl _get_output_format(void);
  int __cdecl setvbuf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
  int __cdecl _snprintf_s([SA_Pre(Null=SA_No,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
extern "C++" { __pragma(warning(push)); __pragma(warning(disable: 4793)); template <size_t _Size> inline int __cdecl _snprintf_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...) { va_list _ArgList; ( _ArgList = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) ); return _vsnprintf_s(_Dest, _Size, _MaxCount, _Format, _ArgList); } __pragma(warning(pop)); }

  int __cdecl sprintf_s([SA_Pre(Null=SA_No,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
// #line 322 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { __pragma(warning(push)); __pragma(warning(disable: 4793)); template <size_t _Size> inline int __cdecl sprintf_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...) { va_list _ArgList; ( _ArgList = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) ); return vsprintf_s(_Dest, _Size, _Format, _ArgList); } __pragma(warning(pop)); }
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "sscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl sscanf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_sscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _sscanf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);

  int __cdecl sscanf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, ...);
// #line 329 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _sscanf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snscanf([SA_Pre(Null=SA_No,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snscanf_l([SA_Pre(Null=SA_No,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _snscanf_s([SA_Pre(Null=SA_No,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, ...);
  int __cdecl _snscanf_s_l([SA_Pre(Null=SA_No,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "tmpfile_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  FILE * __cdecl tmpfile(void);

  errno_t __cdecl tmpfile_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No)] [SA_Post(Deref=2,Valid=SA_Yes)] FILE ** _File);
  errno_t __cdecl tmpnam_s([SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _Size);
// #line 339 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl tmpnam_s([SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Buf)[_Size]) { return tmpnam_s(_Buf, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "tmpnam_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl tmpnam([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Buffer);
  int __cdecl ungetc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl vfprintf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);

  int __cdecl vfprintf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
// #line 346 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl vprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);

  int __cdecl vprintf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
// #line 350 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "vsnprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl vsnprintf([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);

  int __cdecl vsnprintf_s([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
// #line 354 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _vsnprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
extern "C++" { template <size_t _Size> inline int __cdecl _vsnprintf_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _Args) { return _vsnprintf_s(_Dest, _Size, _MaxCount, _Format, _Args); } }
#pragma warning(push)
#pragma warning(disable:4793)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_snprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snprintf([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...); __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vsnprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _vsnprintf([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _Args);
#pragma warning(pop)

 int __cdecl vsprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
// #line 363 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { template <size_t _Size> inline int __cdecl vsprintf_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _Args) { return vsprintf_s(_Dest, _Size, _Format, _Args); } }
#pragma warning(push)
#pragma warning(disable:4793)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "sprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl sprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...); __declspec(deprecated("This function or variable may be unsafe. Consider using " "vsprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl vsprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _Args);
#pragma warning(pop)
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
  int __cdecl _snprintf_c([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
  int __cdecl _vsnprintf_c([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char *_DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);

  int __cdecl _fprintf_p([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
  int __cdecl _printf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
  int __cdecl _sprintf_p([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
  int __cdecl _vfprintf_p([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
  int __cdecl _vprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
  int __cdecl _vsprintf_p([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, va_list _ArgList);
 int __cdecl _set_printf_count_output([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value);
 int __cdecl _get_printf_count_output();

  int __cdecl _printf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _printf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _printf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vprintf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

  int __cdecl _fprintf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _fprintf_p_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _fprintf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vfprintf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vfprintf_p_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vfprintf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_sprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _sprintf_l([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _sprintf_p_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _sprintf_s_l([SA_Pre(Null=SA_No,WritableBytes="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vsprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _vsprintf_l([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t, va_list _ArgList);
  int __cdecl _vsprintf_p_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char* _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale,  va_list _ArgList);
  int __cdecl _vsprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

  int __cdecl _scprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _scprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vscprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vscprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snprintf_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _snprintf_c_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _snprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vsnprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _vsnprintf_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vsnprintf_c_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, const char *, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vsnprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const char* _Format,[SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);







// #line 423 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"




[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _wfsopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ShFlag);
// #line 429 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

  wint_t __cdecl fgetwc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wint_t __cdecl _fputwchar([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl getwc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wint_t __cdecl putwchar([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch);
  wint_t __cdecl ungetwc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);

  wchar_t * __cdecl fgetws([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl fputws([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wchar_t * __cdecl _getws_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline wchar_t * __cdecl _getws_s(wchar_t (&_String)[_Size]) { return _getws_s(_String, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_getws_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _getws([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String);
  int __cdecl _putws([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str);

  int __cdecl fwprintf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);

  int __cdecl fwprintf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
// #line 451 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl wprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);

  int __cdecl wprintf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
// #line 455 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scwprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl vfwprintf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);

  int __cdecl vfwprintf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
// #line 460 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl vwprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);

  int __cdecl vwprintf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
// #line 464 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


 int __cdecl swprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
// #line 468 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { __pragma(warning(push)); __pragma(warning(disable: 4793)); template <size_t _Size> inline int __cdecl swprintf_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...) { va_list _ArgList; ( _ArgList = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) ); return vswprintf_s(_Dest, _Size, _Format, _ArgList); } __pragma(warning(pop)); }

 int __cdecl vswprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
// #line 472 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
extern "C++" { template <size_t _Size> inline int __cdecl vswprintf_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _Args) { return vswprintf_s(_Dest, _Size, _Format, _Args); } }

  int __cdecl _swprintf_c([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _vswprintf_c([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);

  int __cdecl _snwprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
extern "C++" { __pragma(warning(push)); __pragma(warning(disable: 4793)); template <size_t _Size> inline int __cdecl _snwprintf_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...) { va_list _ArgList; ( _ArgList = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) ); return _vsnwprintf_s(_Dest, _Size, _Count, _Format, _ArgList); } __pragma(warning(pop)); }
  int __cdecl _vsnwprintf_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
extern "C++" { template <size_t _Size> inline int __cdecl _vsnwprintf_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _Args) { return _vsnwprintf_s(_Dest, _Size, _Count, _Format, _Args); } }
#pragma warning(push)
#pragma warning(disable:4793)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_snwprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snwprintf([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...); __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vsnwprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _vsnwprintf([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _Args);
#pragma warning(pop)

  int __cdecl _fwprintf_p([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _wprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _vfwprintf_p([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
  int __cdecl _vwprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
  int __cdecl _swprintf_p([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _vswprintf_p([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scwprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscwprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);

  int __cdecl _wprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _wprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _wprintf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vwprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vwprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vwprintf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

  int __cdecl _fwprintf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _fwprintf_p_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _fwprintf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vfwprintf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vfwprintf_p_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vfwprintf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

  int __cdecl _swprintf_c_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _swprintf_p_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _swprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vswprintf_c_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vswprintf_p_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vswprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scwprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _scwprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscwprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snwprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snwprintf_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _snwprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vsnwprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _vsnwprintf_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
  int __cdecl _vsnwprintf_s_l([SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);










// #line 535 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


#pragma warning(push)
#pragma warning(disable:4141 4996 4793)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_swprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS."))  int __cdecl _swprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...); __declspec(deprecated("This function or variable may be unsafe. Consider using " "vswprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS."))  int __cdecl _vswprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _Args);
__declspec(deprecated("This function or variable may be unsafe. Consider using " "__swprintf_l_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS."))  int __cdecl __swprintf_l( wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, _locale_t _Plocinfo, ...); __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vswprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS."))  int __cdecl __vswprintf_l( wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, _locale_t _Plocinfo, va_list _Args);
#pragma warning(pop)


// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"












#pragma once







// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"










// #line 33 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"

#pragma warning( push )
#pragma warning( disable : 4793 4412 )
static __inline int swprintf(wchar_t * _String, size_t _Count, const wchar_t * _Format, ...)
{
    va_list _Arglist;
    int _Ret;
    ( _Arglist = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) );
    _Ret = _vswprintf_c_l(_String, _Count, _Format, 0, _Arglist);
    ( _Arglist = (va_list)0 );
    return _Ret;
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4412 )
static __inline int __cdecl vswprintf(wchar_t * _String, size_t _Count, const wchar_t * _Format, va_list _Ap)
{
    return _vswprintf_c_l(_String, _Count, _Format, 0, _Ap);
}
#pragma warning( pop )


// #line 57 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"

#pragma warning( push )
#pragma warning( disable : 4793 4412 )
static __inline int _swprintf_l(wchar_t * _String, size_t _Count, const wchar_t * _Format, _locale_t _Plocinfo, ...)
{
    va_list _Arglist;
    int _Ret;
    ( _Arglist = (va_list)( &reinterpret_cast<const char &>(_Plocinfo) ) + ( (sizeof(_Plocinfo) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) );
    _Ret = _vswprintf_c_l(_String, _Count, _Format, _Plocinfo, _Arglist);
    ( _Arglist = (va_list)0 );
    return _Ret;
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4412 )
static __inline int __cdecl _vswprintf_l(wchar_t * _String, size_t _Count, const wchar_t * _Format, _locale_t _Plocinfo, va_list _Ap)
{
    return _vswprintf_c_l(_String, _Count, _Format, _Plocinfo, _Ap);
}
#pragma warning( pop )


#pragma warning( push )
#pragma warning( disable : 4996 )

#pragma warning( push )
#pragma warning( disable : 4793 4141 )
extern "C++" __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "swprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __inline int swprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _String, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...)
{
    va_list _Arglist;
    ( _Arglist = (va_list)( &reinterpret_cast<const char &>(_Format) ) + ( (sizeof(_Format) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) );
    int _Ret = _vswprintf(_String, _Format, _Arglist);
    ( _Arglist = (va_list)0 );
    return _Ret;
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4141 )
extern "C++" __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "vswprintf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __inline int __cdecl vswprintf([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _String, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _Ap)
{
    return _vswprintf(_String, _Format, _Ap);
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4793 4141 )
extern "C++" __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_swprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __inline int _swprintf_l([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _String, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, _locale_t _Plocinfo, ...)
{
    va_list _Arglist;
    ( _Arglist = (va_list)( &reinterpret_cast<const char &>(_Plocinfo) ) + ( (sizeof(_Plocinfo) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) );
    int _Ret = __vswprintf_l(_String, _Format, _Plocinfo, _Arglist);
    ( _Arglist = (va_list)0 );
    return _Ret;
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4141 )
extern "C++" __declspec(deprecated("swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_vswprintf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) __inline int __cdecl _vswprintf_l([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _String, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, _locale_t _Plocinfo, va_list _Ap)
{
    return __vswprintf_l(_String, _Format, _Plocinfo, _Ap);
}
#pragma warning( pop )

#pragma warning( pop )

// #line 126 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"

// #line 128 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"
// #line 129 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\swprintf.inl"

// #line 545 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
// #line 546 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"













// #line 560 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  wchar_t * __cdecl _wtempnam([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Directory, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _FilePrefix);



// #line 566 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscwprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, va_list _ArgList);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _vscwprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "fwscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl fwscanf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fwscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _fwscanf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);

  int __cdecl fwscanf_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, ...);
// #line 574 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _fwscanf_s_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "swscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl swscanf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_swscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _swscanf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);

  int __cdecl swscanf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, ...);
// #line 580 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _swscanf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snwscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snwscanf([SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_snwscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _snwscanf_l([SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _snwscanf_s([SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, ...);
  int __cdecl _snwscanf_s_l([SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "wscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl wscanf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _wscanf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);

  int __cdecl wscanf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, ...);
// #line 590 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
  int __cdecl _wscanf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf_s")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
#pragma warning(pop)

[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _wfdopen([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _FileHandle , [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wfopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  FILE * __cdecl _wfopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode);
  errno_t __cdecl _wfopen_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] FILE ** _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wfreopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  FILE * __cdecl _wfreopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _OldFile);
  errno_t __cdecl _wfreopen_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] FILE ** _File, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _OldFile);



 void __cdecl _wperror([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ErrMsg);
// #line 603 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  FILE * __cdecl _wpopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Command, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Mode);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wremove([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename);
  errno_t __cdecl _wtmpnam_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wtmpnam_s([SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size]) { return _wtmpnam_s(_Buffer, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wtmpnam_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wtmpnam([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Buffer);

  wint_t __cdecl _fgetwc_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wint_t __cdecl _fputwc_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  wint_t __cdecl _ungetwc_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);






// #line 619 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"
inline wint_t __cdecl getwchar()
        {return (fgetwc((&__iob_func()[0]))); }   
inline wint_t __cdecl putwchar(wchar_t _C)
        {return (fputwc(_C, (&__iob_func()[1]))); }       
// #line 624 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"










// #line 635 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


// #line 638 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


// #line 641 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"















// #line 657 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"












 void __cdecl _lock_file([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
 void __cdecl _unlock_file([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);



// #line 675 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


  int __cdecl _fclose_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fflush_nolock([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  size_t __cdecl _fread_nolock([SA_Pre(Null=SA_No,WritableBytes="\n@""_ElementSize*_Count")] [SA_Pre(Deref=1,Valid=SA_No)] void * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ElementSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  size_t __cdecl _fread_nolock_s([SA_Pre(Null=SA_No,WritableBytes="\n@""_ElementSize*_Count")] [SA_Pre(Deref=1,Valid=SA_No)] void * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ElementSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fseek_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Offset, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Origin);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long __cdecl _ftell_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _fseeki64_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Offset, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Origin);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _ftelli64_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  size_t __cdecl _fwrite_nolock([SA_Pre(Null=SA_No,ValidBytes="\n@""_Size*_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
  int __cdecl _ungetc_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);












// #line 700 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"











// #line 712 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_tempnam" ". See online help for details."))  char * __cdecl tempnam([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Directory, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _FilePrefix);



// #line 718 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fcloseall" ". See online help for details."))  int __cdecl fcloseall(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fdopen" ". See online help for details."))  FILE * __cdecl fdopen([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _FileHandle, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Format);
 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fgetchar" ". See online help for details."))  int __cdecl fgetchar(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fileno" ". See online help for details."))  int __cdecl fileno([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] FILE * _File);
 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_flushall" ". See online help for details."))  int __cdecl flushall(void);
 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fputchar" ". See online help for details."))  int __cdecl fputchar([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_getw" ". See online help for details."))  int __cdecl getw([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_putw" ". See online help for details."))  int __cdecl putw([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] FILE * _File);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_rmtmp" ". See online help for details."))  int __cdecl rmtmp(void);

// #line 730 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


}
// #line 734 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"


#pragma pack(pop)
// #line 738 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

// #line 740 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdio.h"

// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"


 
 
 
 
 
 
 

 
 
 
 





typedef FILE _iobuf;


 
namespace std {
using ::_iobuf;

using ::size_t; using ::fpos_t; using ::FILE;
using ::clearerr; using ::fclose; using ::feof;
using ::ferror; using ::fflush; using ::fgetc;
using ::fgetpos; using ::fgets; using ::fopen;
using ::fprintf; using ::fputc; using ::fputs;
using ::fread; using ::freopen; using ::fscanf;
using ::fseek; using ::fsetpos; using ::ftell;
using ::fwrite; using ::getc; using ::getchar;
using ::gets; using ::perror;
using ::putc; using ::putchar;
using ::printf; using ::puts; using ::remove;
using ::rename; using ::rewind; using ::scanf;
using ::setbuf; using ::setvbuf; using ::sprintf;
using ::sscanf; using ::tmpfile; using ::tmpnam;
using ::ungetc; using ::vfprintf; using ::vprintf;
using ::vsprintf;


}
 // #line 61 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"
// #line 62 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"

// #line 64 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdio"





// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"

#pragma once










 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
















#pragma once
// #line 19 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 24 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"







#pragma pack(push,8)
// #line 33 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


extern "C" {
// #line 37 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"



















typedef int (__cdecl * _onexit_t)(void);



// #line 61 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"



// #line 65 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"




// #line 70 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


// #line 73 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"






typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct _ldiv_t {
        long quot;
        long rem;
} ldiv_t;


// #line 91 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"










#pragma pack(4)
typedef struct {
    unsigned char ld[10];
} _LDOUBLE;
#pragma pack()













// #line 120 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

typedef struct {
        double x;
} _CRT_DOUBLE;

typedef struct {
    float f;
} _CRT_FLOAT;





typedef struct {
        


        long double x;
} _LONGDOUBLE;



#pragma pack(4)
typedef struct {
    unsigned char ld12[12];
} _LDBL12;
#pragma pack()


// #line 150 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"














// #line 165 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

 extern int __mb_cur_max;



// #line 171 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
 int __cdecl ___mb_cur_max_func(void);
 int __cdecl ___mb_cur_max_l_func(_locale_t);
// #line 174 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"





































typedef void (__cdecl *_purecall_handler)(void); 


 _purecall_handler __cdecl _set_purecall_handler([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _purecall_handler _Handler);
 _purecall_handler __cdecl _get_purecall_handler();
// #line 217 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


extern "C++"
{




// #line 226 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"







// #line 234 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
}
// #line 236 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"



typedef void (__cdecl *_invalid_parameter_handler)(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t); 


 _invalid_parameter_handler __cdecl _set_invalid_parameter_handler([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _invalid_parameter_handler _Handler);
 _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);
// #line 245 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


extern "C++"
{




// #line 254 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"






// #line 261 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
}
// #line 263 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"




 extern int * __cdecl _errno(void);


errno_t __cdecl _set_errno([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value);
errno_t __cdecl _get_errno([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _Value);
// #line 273 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

 unsigned long * __cdecl __doserrno(void);


errno_t __cdecl _set_doserrno([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Value);
errno_t __cdecl _get_doserrno([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] unsigned long * _Value);


 __declspec(deprecated("This function or variable may be unsafe. Consider using " "strerror" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char ** __cdecl __sys_errlist(void);


 __declspec(deprecated("This function or variable may be unsafe. Consider using " "strerror" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) int * __cdecl __sys_nerr(void);













// #line 299 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


 extern int __argc;          
 extern char ** __argv;      
 extern wchar_t ** __wargv;  







// #line 312 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"





 extern char ** _environ;    
 extern wchar_t ** _wenviron;    
// #line 320 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_pgmptr" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  extern char * _pgmptr;      
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_wpgmptr" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  extern wchar_t * _wpgmptr;  














// #line 338 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

errno_t __cdecl _get_pgmptr([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _Value);
errno_t __cdecl _get_wpgmptr([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Pre(Deref=2,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _Value);



__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_fmode" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  extern int _fmode;          



// #line 349 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

 errno_t __cdecl _set_fmode([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode);
 errno_t __cdecl _get_fmode([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PMode);





// #line 358 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper( _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];

}
// #line 365 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
// #line 366 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"





 __declspec(noreturn) void __cdecl exit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Code);
 __declspec(noreturn) void __cdecl _exit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Code);
 void __cdecl abort(void);
// #line 375 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

 unsigned int __cdecl _set_abort_behavior([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Flags, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Mask);



        int     __cdecl abs([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _X);
        long    __cdecl labs([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _X);
// #line 383 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

        __int64    __cdecl _abs64(__int64);













// #line 399 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"












// #line 412 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
        int    __cdecl atexit(void (__cdecl *)(void));
// #line 414 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


[returnvalue:SA_Post(MustCheck=SA_Yes)]  double  __cdecl atof([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_String);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double  __cdecl _atof_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
// #line 419 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int    __cdecl atoi([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl _atoi_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl atol([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl _atol_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);



[returnvalue:SA_Post(MustCheck=SA_Yes)]  void * __cdecl bsearch_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const void * _Key, [SA_Pre(Null=SA_No,ValidBytes="\n@""_NumOfElements * _SizeOfElements")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Base, 
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _NumOfElements, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeOfElements,
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int (__cdecl * _PtFuncCompare)(void *, const void *, const void *), void * _Context);
// #line 430 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  void * __cdecl bsearch([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const void * _Key, [SA_Pre(Null=SA_No,ValidBytes="\n@""_NumOfElements * _SizeOfElements")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Base, 
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NumOfElements, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeOfElements,
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int (__cdecl * _PtFuncCompare)(const void *, const void *));


 void __cdecl qsort_s([SA_Pre(Null=SA_No,WritableBytes="\n@""_NumOfElements* _SizeOfElements")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Base, 
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _NumOfElements, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeOfElements,
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int (__cdecl * _PtFuncCompare)(void *, const void *, const void *), void *_Context);
// #line 439 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
 void __cdecl qsort([SA_Pre(Null=SA_No,WritableBytes="\n@""_NumOfElements * _SizeOfElements")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Base, 
	[SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NumOfElements, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeOfElements, 
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int (__cdecl * _PtFuncCompare)(const void *, const void *));
// #line 443 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned short __cdecl _byteswap_ushort([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned short _Short);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned long  __cdecl _byteswap_ulong ([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Long);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned __int64 __cdecl _byteswap_uint64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Int64);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  div_t  __cdecl div([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Numerator, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Denominator);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __declspec(deprecated("This function or variable may be unsafe. Consider using " "_dupenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl getenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _VarName);

  errno_t __cdecl getenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _ReturnSize, [SA_Pre(Null=SA_No,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _DstSize, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _VarName);
// #line 451 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl getenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _ReturnSize, char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _VarName) { return getenv_s(_ReturnSize, _Dest, _Size, _VarName); } }



// #line 456 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  errno_t __cdecl _dupenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElements="*_PBufferSizeInBytes")] [SA_Post(Deref=2,Valid=SA_Yes)] char **_PBuffer, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PBufferSizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _VarName);



// #line 462 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  errno_t __cdecl _itoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value, [SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _itoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value, char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _itoa_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_itoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _itoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
  errno_t __cdecl _i64toa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Val, [SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_i64toa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl _i64toa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
  errno_t __cdecl _ui64toa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ui64toa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl _ui64toa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _atoi64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _atoi64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _strtoi64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64 __cdecl _strtoi64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned __int64 __cdecl _strtoui64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned __int64 __cdecl _strtoui64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _String, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int  _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  ldiv_t __cdecl ldiv([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Numerator, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Denominator);

extern "C++"
{
    inline ldiv_t  div([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _A1, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _A2)
    {
        return ldiv(_A1, _A2);
    }
}
// #line 486 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
  errno_t __cdecl _ltoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Val, [SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _ltoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Value, char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _ltoa_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _ltoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl mblen([SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl _mblen_l([SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl _mbstrlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl _mbstrlen_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl _mbstrnlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl _mbstrnlen_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
 int    __cdecl mbtowc([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstCh, [SA_Pre(Null=SA_Maybe,ValidBytes="_SrcSizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _SrcCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SrcSizeInBytes);
 int    __cdecl _mbtowc_l([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstCh, [SA_Pre(Null=SA_Maybe,ValidBytes="_SrcSizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _SrcCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SrcSizeInBytes, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  errno_t __cdecl mbstowcs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="*_PtNumOfCharConverted")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _SrcBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount );
extern "C++" { template <size_t _Size> inline errno_t __cdecl mbstowcs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount) { return mbstowcs_s(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "mbstowcs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl mbstowcs([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);

  errno_t __cdecl _mbstowcs_s_l([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="*_PtNumOfCharConverted")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,ValidElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _SrcBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _mbstowcs_s_l([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale) { return _mbstowcs_s_l(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_mbstowcs_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl _mbstowcs_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl rand(void);


// #line 509 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  int    __cdecl _set_error_mode([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode);

 void   __cdecl srand([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Seed);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl strtod([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl _strtod_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl strtol([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix );
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl _strtol_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char **_EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned long __cdecl strtoul([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned long __cdecl _strtoul_l(const char * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char **_EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);


 int __cdecl system([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Command);
// #line 523 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
  errno_t __cdecl _ultoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Val, [SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _ultoa_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Value, char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _ultoa_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _ultoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "wctomb_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) int    __cdecl wctomb([SA_Pre(Null=SA_Maybe,WritableBytesConst=5)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _MbCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _WCh);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wctomb_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) int    __cdecl _wctomb_l([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _MbCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _WCh, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

  errno_t __cdecl wctomb_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _SizeConverted, [SA_Pre(Null=SA_Maybe,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="*_SizeConverted")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _MbCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _WCh);
// #line 531 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
  errno_t __cdecl _wctomb_s_l([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _SizeConverted, [SA_Pre(Null=SA_Maybe,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _MbCh, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _WCh, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  errno_t __cdecl wcstombs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableBytes="_DstSizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="*_PtNumOfCharConverted")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCountInBytes);
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcstombs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount) { return wcstombs_s(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcstombs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl wcstombs([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
  errno_t __cdecl _wcstombs_s_l([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableBytes="_DstSizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="*_PtNumOfCharConverted")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCountInBytes, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcstombs_s_l([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _PtNumOfCharConverted, [SA_Pre(Null=SA_Maybe,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale) { return _wcstombs_s_l(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcstombs_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl _wcstombs_l([SA_Pre(Null=SA_No,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

























// #line 564 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


































// #line 599 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="\n@""_NumOfElements* _SizeOfElements")]   __declspec(noalias) __declspec(restrict)    void * __cdecl calloc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NumOfElements, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeOfElements);
                     __declspec(noalias)                                                                             void   __cdecl free([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Memory);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_Size")]   __declspec(noalias) __declspec(restrict)                              void * __cdecl malloc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_NewSize")]  __declspec(noalias) __declspec(restrict)                           void * __cdecl realloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NewSize);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="\n@""_Count*_Size")]  __declspec(noalias) __declspec(restrict)                       void * __cdecl _recalloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
                     __declspec(noalias)                                                                             void   __cdecl _aligned_free([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Memory);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_Size")]  __declspec(noalias) __declspec(restrict)                              void * __cdecl _aligned_malloc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_Size")]  __declspec(noalias) __declspec(restrict)                              void * __cdecl _aligned_offset_malloc([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Offset);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_NewSize")]  __declspec(noalias) __declspec(restrict)                              void * __cdecl _aligned_realloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NewSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="\n@""_Count*_Size")]  __declspec(noalias) __declspec(restrict)                       void * __cdecl _aligned_recalloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_NewSize")]  __declspec(noalias) __declspec(restrict)                              void * __cdecl _aligned_offset_realloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NewSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Offset);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="\n@""_Count*_Size")]  __declspec(noalias) __declspec(restrict)                       void * __cdecl _aligned_offset_recalloc([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Offset);
[returnvalue:SA_Post(MustCheck=SA_Yes)]                                                   size_t __cdecl _aligned_msize([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Alignment, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Offset);


















// #line 631 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

// #line 633 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"





  errno_t __cdecl _itow_s ([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _itow_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value, wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _itow_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_itow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _itow([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
  errno_t __cdecl _ltow_s ([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Val, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _ltow_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Value, wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _ltow_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _ltow([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
  errno_t __cdecl _ultow_s ([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Val, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _ultow_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Value, wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix) { return _ultow_s(_Value, _Dest, _Size, _Radix); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _ultow([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Value, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl wcstod([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl _wcstod_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl wcstol([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long   __cdecl _wcstol_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t **_EndPtr, int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned long __cdecl wcstoul([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned long __cdecl _wcstoul_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t **_EndPtr, int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wdupenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) wchar_t * __cdecl _wgetenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _VarName);
  errno_t __cdecl _wgetenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _ReturnSize, [SA_Pre(Null=SA_No,WritableElements="_DstSizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _VarName);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wgetenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _ReturnSize, wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _VarName) { return _wgetenv_s(_ReturnSize, _Dest, _Size, _VarName); } }




// #line 661 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  errno_t __cdecl _wdupenv_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElements="*_BufferSizeInWords")] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t **_Buffer, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t *_BufferSizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_VarName);



// #line 667 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"



 int __cdecl _wsystem([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Command);
// #line 672 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl _wtof([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  double __cdecl _wtof_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wtoi([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wtoi_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long __cdecl _wtol([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  long __cdecl _wtol_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

  errno_t __cdecl _i64tow_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Val, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_i64tow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) wchar_t * __cdecl _i64tow([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
  errno_t __cdecl _ui64tow_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ui64tow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) wchar_t * __cdecl _ui64tow([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64   __cdecl _wtoi64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64   __cdecl _wtoi64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64   __cdecl _wcstoi64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __int64   __cdecl _wcstoi64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned __int64  __cdecl _wcstoui64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  unsigned __int64  __cdecl _wcstoui64_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str , [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _EndPtr, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);


// #line 692 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"














// #line 707 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  char * __cdecl _fullpath([SA_Pre(Null=SA_Maybe,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _FullPath, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Path, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes);





// #line 715 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  errno_t __cdecl _ecvt_s([SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDights, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _ecvt_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Value, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign) { return _ecvt_s(_Dest, _Size, _Value, _NumOfDigits, _PtDec, _PtSign); } }
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ecvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl _ecvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
  errno_t __cdecl _fcvt_s([SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _fcvt_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Value, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign) { return _fcvt_s(_Dest, _Size, _Value, _NumOfDigits, _PtDec, _PtSign); } }
[returnvalue:SA_Post(MustCheck=SA_Yes)]  __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl _fcvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
 errno_t __cdecl _gcvt_s([SA_Pre(Null=SA_No,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _gcvt_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Value, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits) { return _gcvt_s(_Dest, _Size, _Value, _NumOfDigits); } }
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_gcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) char * __cdecl _gcvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atodbl([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _CRT_DOUBLE * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atoldbl([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _LDOUBLE * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atoflt([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _CRT_FLOAT * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atodbl_l([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _CRT_DOUBLE * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atoldbl_l([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _LDOUBLE * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _atoflt_l([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] _CRT_FLOAT * _Result, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned long __cdecl _lrotl([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned long __cdecl _lrotr([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
  errno_t   __cdecl _makepath_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _PathResult, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename,
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Ext);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _makepath_s(char (&_Path)[_Size], [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Ext) { return _makepath_s(_Path, _Size, _Drive, _Dir, _Filename, _Ext); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_makepath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void __cdecl _makepath([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Path, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Ext);












// #line 751 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"












// #line 764 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
        _onexit_t __cdecl _onexit([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _onexit_t _Func);
// #line 766 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
        





#pragma warning (push)
#pragma warning (disable:6540) 
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl _putenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _EnvString);
  errno_t __cdecl _putenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Name, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Value);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned int __cdecl _rotl([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned __int64 __cdecl _rotl64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned int __cdecl _rotr([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] unsigned __int64 __cdecl _rotr64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned __int64 _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Shift);
#pragma warning (pop)

 errno_t __cdecl _searchenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _EnvVar, [SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _ResultPath, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _searchenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _EnvVar, char (&_ResultPath)[_Size]) { return _searchenv_s(_Filename, _EnvVar, _ResultPath, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_searchenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void __cdecl _searchenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _EnvVar, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_ResultPath);

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_splitpath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void   __cdecl _splitpath([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _FullPath, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Drive, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dir, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Filename, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Ext);
  errno_t  __cdecl _splitpath_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _FullPath, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_DriveSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Drive, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DriveSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_DirSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dir, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DirSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_FilenameSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _FilenameSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_ExtSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Ext, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ExtSize);
extern "C++" { template <size_t _DriveSize, size_t _DirSize, size_t _NameSize, size_t _ExtSize> inline errno_t __cdecl _splitpath_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const char *_Dest, char (&_Drive)[_DriveSize], char (&_Dir)[_DirSize], char (&_Name)[_NameSize], char (&_Ext)[_ExtSize]) { return _splitpath_s(_Dest, _Drive, _DriveSize, _Dir, _DirSize, _Name, _NameSize, _Ext, _ExtSize); } }

 void   __cdecl _swab([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Post(ValidElements="_SizeInBytes")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf1, [SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Post(ValidElements="_SizeInBytes")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf2, int _SizeInBytes);








// #line 804 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  wchar_t * __cdecl _wfullpath([SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _FullPath, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Path, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);



// #line 810 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

  errno_t __cdecl _wmakepath_s([SA_Pre(Null=SA_No,WritableElements="_SIZE")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _PathResult, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SIZE, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename,
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Ext);        
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wmakepath_s(wchar_t (&_ResultPath)[_Size], [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Ext) { return _wmakepath_s(_ResultPath, _Size, _Drive, _Dir, _Filename, _Ext); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wmakepath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void __cdecl _wmakepath([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_ResultPath, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Drive, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Dir, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Ext);




[returnvalue:SA_Post(MustCheck=SA_Yes)]  int    __cdecl _wputenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _EnvString);
  errno_t __cdecl _wputenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Name, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Value);
 errno_t __cdecl _wsearchenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _EnvVar, [SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _ResultPath, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wsearchenv_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _EnvVar, wchar_t (&_ResultPath)[_Size]) { return _wsearchenv_s(_Filename, _EnvVar, _ResultPath, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsearchenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void __cdecl _wsearchenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _EnvVar, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_ResultPath);
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsplitpath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  void   __cdecl _wsplitpath([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _FullPath, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Drive, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dir, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Filename, [SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Ext);
 errno_t __cdecl _wsplitpath_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _FullPath, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_DriveSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Drive, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DriveSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_DirSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dir, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DirSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_FilenameSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _FilenameSize, 
		[SA_Pre(Null=SA_Maybe,WritableElements="_ExtSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Ext, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _ExtSize);
extern "C++" { template <size_t _DriveSize, size_t _DirSize, size_t _NameSize, size_t _ExtSize> inline errno_t __cdecl _wsplitpath_s([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const wchar_t *_Path, wchar_t (&_Drive)[_DriveSize], wchar_t (&_Dir)[_DirSize], wchar_t (&_Name)[_NameSize], wchar_t (&_Ext)[_ExtSize]) { return _wsplitpath_s(_Path, _Drive, _DriveSize, _Dir, _DirSize, _Name, _NameSize, _Ext, _ExtSize); } }


// #line 834 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


__declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "SetErrorMode" " instead. See online help for details."))  void __cdecl _seterrormode([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode);
__declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "Beep" " instead. See online help for details."))  void __cdecl _beep([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned _Frequency, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned _Duration);
__declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "Sleep" " instead. See online help for details."))  void __cdecl _sleep([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Duration);

// #line 841 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"
















#pragma warning(push)
#pragma warning(disable: 4141)  
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_ecvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ecvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl ecvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_fcvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl fcvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtDec, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _PtSign);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_gcvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))		 char * __cdecl gcvt([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] double _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _NumOfDigits, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_itoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_itoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))		 char * __cdecl itoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_ltoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))		 char * __cdecl ltoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] long _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_putenv" ". See online help for details."))  int    __cdecl putenv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _EnvString);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_swab" ". See online help for details."))										 void   __cdecl swab([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf1,[SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _SizeInBytes);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_ultoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))	 char * __cdecl ultoa([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _Val, [SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dstbuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Radix);
#pragma warning(pop)
_onexit_t __cdecl onexit([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _onexit_t _Func);

// #line 871 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

// #line 873 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


}

// #line 878 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"


#pragma pack(pop)
// #line 882 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

// #line 884 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdlib.h"

// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"


 

namespace std {

using ::size_t; using ::div_t; using ::ldiv_t;

using ::abort; using ::abs; using ::atexit;
using ::atof; using ::atoi; using ::atol;
using ::bsearch; using ::calloc; using ::div;
using ::exit; using ::free; using ::getenv;
using ::labs; using ::ldiv; using ::malloc;
using ::mblen; using ::mbstowcs; using ::mbtowc;
using ::qsort; using ::rand; using ::realloc;
using ::srand; using ::strtod; using ::strtol;
using ::strtoul; using ::system;
using ::wcstombs; using ::wctomb;


}
 // #line 37 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"
// #line 38 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"

// #line 40 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstdlib"





// #line 9 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

#pragma once




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"

#pragma once










 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"


extern "C" {
// #line 27 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"




// #line 32 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
















 void *  __cdecl _memccpy( [SA_Pre(Null=SA_Maybe,WritableBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const void *  __cdecl memchr( [SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf , [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _memicmp([SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf1, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _memicmp_l([SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf1, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] int     __cdecl memcmp([SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf1, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
         void *  __cdecl memcpy([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_Size")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);

 errno_t  __cdecl memcpy_s([SA_Pre(Null=SA_Maybe,WritableBytes="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_MaxCount")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _DstSize, [SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 57 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
        void *  __cdecl memset([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_Size")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);



__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_memccpy" ". See online help for details."))  void * __cdecl memccpy([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] void * _Dst, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_memicmp" ". See online help for details."))  int __cdecl memicmp([SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf1, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Buf2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
// #line 64 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

// #line 66 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

  errno_t __cdecl _strset_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_DstSize")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _DstSize, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strset_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value) { return _strset_s(_Dest, _Size, _Value); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strset_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Value);

  errno_t __cdecl strcpy_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src);
// #line 73 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl strcpy_s([SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source) { return strcpy_s(_Dest, _Size, _Source); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "strcpy_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl strcpy([SA_Pre(Null=SA_No,WritableElementsLength="_Source")] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source);

  errno_t __cdecl strcat_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src);
// #line 78 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl strcat_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source) { return strcat_s(_Dest, _Size, _Source); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "strcat_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl strcat([SA_Pre(Null=SA_No,WritableElementsLength="_Source")] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] int     __cdecl strcmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2);
        [returnvalue:SA_Post(MustCheck=SA_Yes)] size_t  __cdecl strlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t  __cdecl strnlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);

[returnvalue:SA_Post(MustCheck=SA_Yes)] static __inline size_t  __cdecl strnlen_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount)
{
    return (_Str==0) ? 0 : strnlen(_Str, _MaxCount);
}
// #line 89 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

  errno_t __cdecl memmove_s([SA_Pre(Null=SA_Maybe,WritableBytes="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_MaxCount")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _DstSize, [SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 92 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"



// #line 96 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
  void *  __cdecl memmove([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_Size")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
// #line 98 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"




// #line 103 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  char *  __cdecl _strdup([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src);



// #line 109 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  const char *  __cdecl strchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _stricmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strcmpi([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _stricmp_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl strcoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const  char * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strcoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _stricoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _stricoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strncoll  ([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strncoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strnicoll ([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strnicoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t  __cdecl strcspn([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "_strerror_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char *  __cdecl _strerror([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _ErrMsg);
  errno_t __cdecl _strerror_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _ErrMsg);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strerror_s(char (&_Buffer)[_Size], [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _ErrorMessage) { return _strerror_s(_Buffer, _Size, _ErrorMessage); } }
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "strerror_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char *  __cdecl strerror([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int);

  errno_t __cdecl strerror_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ErrNum);
// #line 130 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl strerror_s(char (&_Buffer)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ErrorMessage) { return strerror_s(_Buffer, _Size, _ErrorMessage); } }
  errno_t __cdecl _strlwr_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strlwr_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_String)[_Size]) { return _strlwr_s(_String, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strlwr_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strlwr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_String);
  errno_t __cdecl _strlwr_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strlwr_s_l([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_String)[_Size], [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale) { return _strlwr_s_l(_String, _Size, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strlwr_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strlwr_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

  errno_t __cdecl strncat_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 140 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl strncat_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count) { return strncat_s(_Dest, _Size, _Source, _Count); } }
#pragma warning(push)
#pragma warning(disable:6059)

__declspec(deprecated("This function or variable may be unsafe. Consider using " "strncat_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl strncat([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count);
#pragma warning(pop)


// #line 149 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl strncmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
// #line 151 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strnicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _strnicmp_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

  errno_t __cdecl strncpy_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 156 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl strncpy_s(char (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count) { return strncpy_s(_Dest, _Size, _Source, _Count); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "strncpy_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl strncpy([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] char *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count);
  errno_t __cdecl _strnset_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strnset_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count) { return _strnset_s(_Dest, _Size, _Val, _Count); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strnset_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strnset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const char *  __cdecl strpbrk([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const char *  __cdecl strrchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch);
 char *  __cdecl _strrev([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t  __cdecl strspn([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const char *  __cdecl strstr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _SubStr);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "strtok_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char *  __cdecl strtok([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Delim);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  char *  __cdecl strtok_s([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Delim, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char ** _Context);
// #line 170 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
  errno_t __cdecl _strupr_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strupr_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_String)[_Size]) { return _strupr_s(_String, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strupr_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strupr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_String);
  errno_t __cdecl _strupr_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _strupr_s_l([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] char (&_String)[_Size], _locale_t _Locale) { return _strupr_s_l(_String, _Size, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_strupr_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  char * __cdecl _strupr_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  size_t  __cdecl strxfrm ([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] char * _Dst, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
  size_t  __cdecl _strxfrm_l([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] char * _Dst, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);


extern "C++" {


[returnvalue:SA_Post(MustCheck=SA_Yes)] inline char * __cdecl strchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch)
	{ return (char*)strchr((const char*)_Str, _Ch); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline char * __cdecl strpbrk([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Control)
	{ return (char*)strpbrk((const char*)_Str, _Control); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline char * __cdecl strrchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Ch)
	{ return (char*)strrchr((const char*)_Str, _Ch); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline char * __cdecl strstr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _SubStr)
	{ return (char*)strstr((const char*)_Str, _SubStr); }
// #line 192 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"


[returnvalue:SA_Post(MustCheck=SA_Yes)] inline void * __cdecl memchr([SA_Pre(Null=SA_Maybe,ValidBytes="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Pv, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
	{ return (void*)memchr((const void*)_Pv, _C, _N); }
// #line 197 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
}
// #line 199 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"






// #line 206 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strdup" ". See online help for details."))  char * __cdecl strdup([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src);



// #line 212 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"


[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strcmpi" ". See online help for details."))  int __cdecl strcmpi([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_stricmp" ". See online help for details."))  int __cdecl stricmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str2);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strlwr" ". See online help for details."))  char * __cdecl strlwr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strnicmp" ". See online help for details."))  int __cdecl strnicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strnset" ". See online help for details."))  char * __cdecl strnset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strrev" ". See online help for details."))  char * __cdecl strrev([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strset" ". See online help for details."))         char * __cdecl strset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Val);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_strupr" ". See online help for details."))  char * __cdecl strupr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Str);

// #line 224 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"









// #line 234 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  wchar_t * __cdecl _wcsdup([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str);



// #line 240 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"


  errno_t __cdecl wcscat_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src);
// #line 244 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcscat_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source) { return wcscat_s(_Dest, _Size, _Source); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcscat_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl wcscat([SA_Pre(Null=SA_No,WritableElementsLength="_Source")] [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const wchar_t * __cdecl wcschr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, wchar_t _Ch);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl wcscmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);

  errno_t __cdecl wcscpy_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src);
// #line 251 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcscpy_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source) { return wcscpy_s(_Dest, _Size, _Source); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcscpy_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl wcscpy([SA_Pre(Null=SA_No,WritableElementsLength="_Source")] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl wcscspn([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl wcslen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl wcsnlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);

[returnvalue:SA_Post(MustCheck=SA_Yes)] static __inline size_t __cdecl wcsnlen_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount)
{
    return (_Src == 0) ? 0 : wcsnlen(_Src, _MaxCount);
}
// #line 262 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

  errno_t __cdecl wcsncat_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 265 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcsncat_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count) { return wcsncat_s(_Dest, _Size, _Source, _Count); } }
#pragma warning(push)
#pragma warning(disable:6059)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcsncat_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl wcsncat([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count);
#pragma warning(pop)
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl wcsncmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);

  errno_t __cdecl wcsncpy_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 274 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcsncpy_s(wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count) { return wcsncpy_s(_Dest, _Size, _Source, _Count); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcsncpy_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl wcsncpy([SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] wchar_t *_Dest, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Source, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const wchar_t * __cdecl wcspbrk([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const wchar_t * __cdecl wcsrchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t __cdecl wcsspn([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Control);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  const wchar_t * __cdecl wcsstr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _SubStr);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "wcstok_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl wcstok([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Delim);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  wchar_t * __cdecl wcstok_s([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Delim, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t ** _Context);
// #line 284 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcserror_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcserror([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ErrNum);
  errno_t __cdecl _wcserror_s([SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ErrNum);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcserror_s(wchar_t (&_Buffer)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Error) { return _wcserror_s(_Buffer, _Size, _Error); } }
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "__wcserror_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl __wcserror([SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str);
  errno_t __cdecl __wcserror_s([SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buffer, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ErrMsg);
extern "C++" { template <size_t _Size> inline errno_t __cdecl __wcserror_s(wchar_t (&_Buffer)[_Size], [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ErrorMessage) { return __wcserror_s(_Buffer, _Size, _ErrorMessage); } }

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsicmp_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsnicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsnicmp_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  errno_t __cdecl _wcsnset_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcsnset_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Dst)[_Size], wchar_t _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount) { return _wcsnset_s(_Dst, _Size, _Val, _MaxCount); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcsnset_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcsnset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Str, wchar_t _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
 wchar_t * __cdecl _wcsrev([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str);
  errno_t __cdecl _wcsset_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Value);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcsset_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Str)[_Size], wchar_t _Val) { return _wcsset_s(_Str, _Size, _Val); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcsset_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcsset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Str, wchar_t _Val);

  errno_t __cdecl _wcslwr_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcslwr_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_String)[_Size]) { return _wcslwr_s(_String, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcslwr_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcslwr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String);
  errno_t __cdecl _wcslwr_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcslwr_s_l([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_String)[_Size], [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale) { return _wcslwr_s_l(_String, _Size, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcslwr_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcslwr_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  errno_t __cdecl _wcsupr_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcsupr_s([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_String)[_Size]) { return _wcsupr_s(_String, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcsupr_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcsupr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String);
  errno_t __cdecl _wcsupr_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcsupr_s_l([SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_String)[_Size], [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale) { return _wcsupr_s_l(_String, _Size, _Locale); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcsupr_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wcsupr_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
  size_t __cdecl wcsxfrm([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] wchar_t * _Dst, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
  size_t __cdecl _wcsxfrm_l([SA_Pre(Null=SA_Maybe,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Maybe)] wchar_t * _Dst, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl wcscoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcscoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsicoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsicoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Str2, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsncoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsncoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsnicoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wcsnicoll_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);




extern "C++" {
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline wchar_t * __cdecl wcschr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_Str, wchar_t _Ch)
        {return ((wchar_t *)wcschr((const wchar_t *)_Str, _Ch)); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline wchar_t * __cdecl wcspbrk([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_Control)
        {return ((wchar_t *)wcspbrk((const wchar_t *)_Str, _Control)); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline wchar_t * __cdecl wcsrchr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch)
        {return ((wchar_t *)wcsrchr((const wchar_t *)_Str, _Ch)); }
[returnvalue:SA_Post(MustCheck=SA_Yes)] inline wchar_t * __cdecl wcsstr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_Str, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_SubStr)
        {return ((wchar_t *)wcsstr((const wchar_t *)_Str, _SubStr)); }
}
// #line 340 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
// #line 341 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"






// #line 348 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsdup" ". See online help for details."))  wchar_t * __cdecl wcsdup([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str);



// #line 354 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"





[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsicmp" ". See online help for details."))  int __cdecl wcsicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsnicmp" ". See online help for details."))  int __cdecl wcsnicmp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsnset" ". See online help for details."))  wchar_t * __cdecl wcsnset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Val, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _MaxCount);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsrev" ". See online help for details."))  wchar_t * __cdecl wcsrev([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsset" ". See online help for details."))  wchar_t * __cdecl wcsset([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str, wchar_t _Val);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcslwr" ". See online help for details."))  wchar_t * __cdecl wcslwr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str);
__declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsupr" ". See online help for details."))  wchar_t * __cdecl wcsupr([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Str);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: " "_wcsicoll" ". See online help for details."))  int __cdecl wcsicoll([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str1, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Str2);

// #line 369 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"


// #line 372 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"



}
// #line 377 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"

// #line 379 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\string.h"
// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"

 
  
namespace std {
using ::size_t; using ::memchr; using ::memcmp;

using ::memcpy; using ::memmove; using ::memset;
using ::strcat; using ::strchr; using ::strcmp;
using ::strcoll; using ::strcpy; using ::strcspn;
using ::strerror; using ::strlen; using ::strncat;
using ::strncmp; using ::strncpy; using ::strpbrk;
using ::strrchr; using ::strspn; using ::strstr;
using ::strtok; using ::strxfrm;

}
  // #line 31 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"
 // #line 32 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"

// #line 34 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstring"





// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

#pragma once




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

#pragma once







// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"

#pragma once










 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"


extern "C" {
// #line 27 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"




























// #line 56 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"









// #line 66 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"


 extern unsigned long  __cdecl __threadid(void);

 extern uintptr_t __cdecl __threadhandle(void);
// #line 72 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"



}
// #line 77 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"

// #line 79 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stddef.h"
// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"


 
namespace std {
using ::ptrdiff_t; using ::size_t;
}
 // #line 21 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"
// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"

// #line 24 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"
// #line 25 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cstddef"





// #line 11 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

namespace std {

		
 

 
 
 
 
 

 
 

 



 
 

 
 

 

















// #line 60 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

 

		
 

 

		




 
 

 
 



// #line 82 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"
  
 // #line 84 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

 
  
  
  
 // #line 90 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

		
enum _Uninitialized
	{	
	_Noinit};
}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 100 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"

// #line 102 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"
// #line 103 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstddef"





// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 13 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"










namespace std {

  


  



  



}

 

 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"













#pragma once
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"








#pragma pack(push,8)
// #line 28 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"







typedef void (__cdecl *terminate_function)();
typedef void (__cdecl *terminate_handler)();
typedef void (__cdecl *unexpected_function)();
typedef void (__cdecl *unexpected_handler)();





// #line 45 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"








struct _EXCEPTION_POINTERS;

typedef void (__cdecl *_se_translator_function)(unsigned int, struct _EXCEPTION_POINTERS*);
// #line 57 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"

 __declspec(noreturn) void __cdecl terminate(void);
 void __cdecl unexpected(void);

 int __cdecl _is_exception_typeof([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const type_info &_Type, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] struct _EXCEPTION_POINTERS * _ExceptionPtr);



 terminate_function __cdecl set_terminate([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] terminate_function _NewPtFunc);
extern "C"  terminate_function __cdecl _get_terminate(void);
 unexpected_function __cdecl set_unexpected([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] unexpected_function _NewPtFunc);
extern "C"  unexpected_function __cdecl _get_unexpected(void);
// #line 70 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"



 _se_translator_function __cdecl _set_se_translator([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _se_translator_function _NewPtFunc);
// #line 75 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"
 bool __cdecl __uncaught_exception();















#pragma pack(pop)
// #line 93 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"
// #line 94 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"
// #line 95 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\eh.h"
// #line 41 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"
 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"






#pragma pack(push,8)
// #line 31 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"


extern "C" {
// #line 35 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"







// #line 43 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"





// #line 49 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"













typedef struct _heapinfo {
        int * _pentry;
        size_t _size;
        int _useflag;
        } _HEAPINFO;

// #line 69 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"




extern __declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_amblksiz" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.")) unsigned int _amblksiz;



// #line 78 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"



































// #line 114 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

































// #line 148 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

 int     __cdecl _resetstkoflw (void);



 unsigned long __cdecl _set_malloc_crt_max_wait([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned long _NewValue);








// #line 163 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_NewSize")]  void *  __cdecl _expand([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NewSize);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t  __cdecl _msize([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] void * _Memory);




// #line 171 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void *          __cdecl _alloca([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  size_t  __cdecl _get_sbh_threshold(void);
 int     __cdecl _set_sbh_threshold([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _NewValue);
 errno_t __cdecl _set_amblksiz([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Value);
 errno_t __cdecl _get_amblksiz([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Value);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _heapadd([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] void * _Memory, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _heapchk(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int     __cdecl _heapmin(void);
 int     __cdecl _heapset([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] unsigned int _Fill);
  int     __cdecl _heapwalk([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] _HEAPINFO * _EntryInfo);
 size_t  __cdecl _heapused(size_t * _Used, size_t * _Commit);

 intptr_t __cdecl _get_heap_handle(void);











// #line 197 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

typedef char __static_assert_t[ (sizeof(unsigned int) <= 8) ];


#pragma warning(push)
#pragma warning(disable:6540)
__inline void *_MarkAllocaS([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)]  void *_Ptr, unsigned int _Marker)
{
    if (_Ptr)
    {
        *((unsigned int*)_Ptr) = _Marker;
        _Ptr = (char*)_Ptr + 8;
    }
    return _Ptr;
}
#pragma warning(pop)
// #line 214 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"








// #line 223 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"






// #line 230 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"








__declspec(noalias) __inline void __cdecl _freea([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Memory)
{
    unsigned int _Marker;
    if (_Memory)
    {
        _Memory = (char*)_Memory - 8;
        _Marker = *(unsigned int *)_Memory;
        if (_Marker == 0xDDDD)
        {
            free(_Memory);
        }





// #line 255 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"
    }
}
// #line 258 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"
// #line 259 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"




// #line 264 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

// #line 266 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"






















}
// #line 290 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"


#pragma pack(pop)
// #line 294 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"

// #line 296 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\malloc.h"
// #line 42 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

 

// #line 46 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

 


































typedef const char *__exString;
extern "C" [returnvalue:SA_Post(MustCheck=SA_Yes)] size_t __cdecl strlen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char *);

extern "C"  errno_t __cdecl strcpy_s([SA_Pre(Null=SA_No,WritableElements="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Src);




// #line 91 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

 namespace std {

class  exception
	{	
public:


































































































     exception();
     exception(const char *const&);
     exception(const char *const&, int);
     exception(const exception&);
    exception&  operator=(const exception&);
    virtual  ~exception();
    virtual const char *  what() const;
// #line 203 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"
private:
	const char *_m_what;
	int _m_doFree;
	};

using ::set_terminate; using ::terminate_handler; using ::terminate; using ::set_unexpected; using ::unexpected_handler; using ::unexpected;

typedef void (__cdecl *_Prhand)(const exception&);




// #line 216 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"
 bool __cdecl uncaught_exception();
// #line 218 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

}

 












































































































// #line 331 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"


namespace std {


// #line 337 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

		
class bad_exception : public exception
	{	
public:
	 bad_exception(const char *_Message = "bad exception")
		throw ()
		: exception(_Message)
		{	
		}

	virtual  ~bad_exception() throw ()
		{	
		}

 





// #line 359 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

	};


static const char * _bad_alloc_Message = "bad allocation";

		
class bad_alloc : public exception
	{	
public:
	 bad_alloc(const char *_Message) throw ()
		: exception(_Message)
		{	
		}

	 bad_alloc() throw ()
		: exception(_bad_alloc_Message, 1)
		{	
		}

	virtual  ~bad_alloc() throw ()
		{	
		}

 





// #line 390 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

	};


}









// #line 405 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"








 #pragma warning(pop)
 #pragma pack(pop)
// #line 416 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"

// #line 418 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"
// #line 419 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\exception"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"

#pragma once





// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

#pragma once






 #pragma pack(push,8)
 #pragma warning(push,3)
 
// #line 13 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

  








// #line 24 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

namespace std {

		
 




typedef void (__cdecl * new_handler) ();
// #line 35 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"
 // #line 36 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

 
struct nothrow_t
	{	
	};

extern const nothrow_t nothrow;	
 // #line 44 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

		
 new_handler __cdecl set_new_handler([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] new_handler)
	throw ();	
}

		
void __cdecl operator delete(void *) throw ();
#pragma warning (suppress: 4985)
[returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void *__cdecl operator new(size_t _Size) throw (...);

 
  
inline void *__cdecl operator new(size_t, void *_Where) throw ()
	{	
	return (_Where);
	}

inline void __cdecl operator delete(void *, void *) throw ()
	{	
	}
 // #line 66 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

 
  
inline void *__cdecl operator new[](size_t, void *_Where) throw ()
	{	
	return (_Where);
	}

inline void __cdecl operator delete[](void *, void *) throw ()
	{	
	}
 // #line 78 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

void __cdecl operator delete[](void *) throw ();	

[returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void *__cdecl operator new[](size_t _Size)
	throw (...);	

 
  
[returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_Size")] void *__cdecl operator new(size_t _Size, const std::nothrow_t&)
	throw ();

[returnvalue:SA_Post(Null=SA_Maybe,WritableBytes="_Size")] void *__cdecl operator new[](size_t _Size, const std::nothrow_t&)
	throw ();	

void __cdecl operator delete(void *, const std::nothrow_t&)
	throw ();	

void __cdecl operator delete[](void *, const std::nothrow_t&)
	throw ();	
 // #line 98 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"


 
using std::new_handler;
 // #line 103 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"


 
 #pragma warning(pop)
 #pragma pack(pop)
// #line 109 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"

// #line 111 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"
// #line 112 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\new"





// #line 9 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

#pragma once




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

#pragma once





// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"

#pragma once










 // #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


















#pragma once
// #line 21 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 27 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


#pragma pack(push,8)
// #line 31 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


extern "C" {
// #line 35 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





















































typedef unsigned long _fsize_t; 

// #line 91 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"



struct _wfinddata32_t {
        unsigned    attrib;
        __time32_t  time_create;    
        __time32_t  time_access;    
        __time32_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

struct _wfinddata32i64_t {
        unsigned    attrib;
        __time32_t  time_create;    
        __time32_t  time_access;    
        __time32_t  time_write;
        __int64     size;
        wchar_t     name[260];
};

struct _wfinddata64i32_t {
        unsigned    attrib;
        __time64_t  time_create;    
        __time64_t  time_access;    
        __time64_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

struct _wfinddata64_t {
        unsigned    attrib;
        __time64_t  time_create;    
        __time64_t  time_access;    
        __time64_t  time_write;
        __int64     size;
        wchar_t     name[260];
};



















// #line 149 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


// #line 152 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"






























// #line 183 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 185 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 186 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

 const unsigned short * __cdecl __pctype_func(void);

 extern const unsigned short *_pctype;


// #line 193 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 194 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 195 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





 extern const unsigned short _wctype[];
// #line 202 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

 const wctype_t * __cdecl __pwctype_func(void);

 extern const wctype_t *_pwctype;


// #line 209 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 210 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 211 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"







                                
















[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswalpha([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswalpha_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswupper([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswupper_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswlower([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswlower_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswdigit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswdigit_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswxdigit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswxdigit_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswspace([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswspace_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswpunct([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswpunct_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswalnum([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswalnum_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswprint([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswprint_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswgraph([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswgraph_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswcntrl([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswcntrl_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswascii([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl isleadbyte([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isleadbyte_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl towupper([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _towupper_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl towlower([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _towlower_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale); 
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iswctype([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wctype_t _Type);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswctype_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wctype_t _Type, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __iswcsymf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswcsymf_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __iswcsym([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iswcsym_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

__declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "iswctype" " instead. See online help for details."))  int __cdecl is_wctype([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wctype_t _Type);


// #line 277 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"












// #line 290 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,NullTerminated=SA_Yes)] [returnvalue:SA_Post(Deref=1,Valid=SA_Yes)]  wchar_t * __cdecl _wgetcwd([SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _SizeInWords);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,NullTerminated=SA_Yes)] [returnvalue:SA_Post(Deref=1,Valid=SA_Yes)]  wchar_t * __cdecl _wgetdcwd([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Drive, [SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _SizeInWords);
[returnvalue:SA_Post(MustCheck=SA_Yes)] [returnvalue:SA_Post(Null=SA_Maybe,NullTerminated=SA_Yes)] [returnvalue:SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * __cdecl _wgetdcwd_nolock([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Drive, [SA_Pre(Null=SA_Maybe,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t * _DstBuf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _SizeInWords);





// #line 300 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wchdir([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Path);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wmkdir([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Path);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wrmdir([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Path);


// #line 307 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"



[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _waccess([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _AccessMode);
  errno_t __cdecl _waccess_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _AccessMode);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wchmod([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _wcreat([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _PermissionMode);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  intptr_t __cdecl _wfindfirst32([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata32_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wfindnext32([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] intptr_t _FindHandle, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata32_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wunlink([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wrename([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _OldFilename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _NewFilename);
 errno_t __cdecl _wmktemp_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _TemplateName, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wmktemp_s(wchar_t (&_TemplateName)[_Size]) { return _wmktemp_s(_TemplateName, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wmktemp_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wmktemp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_TemplateName);

[returnvalue:SA_Post(MustCheck=SA_Yes)]  intptr_t __cdecl _wfindfirst32i64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata32i64_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  intptr_t __cdecl _wfindfirst64i32([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata64i32_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  intptr_t __cdecl _wfindfirst64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata64_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wfindnext32i64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] intptr_t _FindHandle, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata32i64_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wfindnext64i32([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] intptr_t _FindHandle, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata64i32_t * _FindData);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _wfindnext64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] intptr_t _FindHandle, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _wfinddata64_t * _FindData);

  errno_t __cdecl _wsopen_s([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] int * _FileHandle, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _OpenFlag, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ShareFlag, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _PermissionFlag);






// #line 337 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


extern "C++" __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _wopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _OpenFlag, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _PermissionMode = 0);
extern "C++" __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsopen_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _wsopen([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _OpenFlag, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _ShareFlag, int _PermissionMode = 0);

// #line 343 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


// #line 346 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





  wchar_t * __cdecl _wsetlocale([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Category, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Locale);


// #line 355 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





 intptr_t __cdecl _wexecl([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wexecle([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wexeclp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wexeclpe([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wexecv([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList);
 intptr_t __cdecl _wexecve([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList,
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _Env);
 intptr_t __cdecl _wexecvp([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList);
 intptr_t __cdecl _wexecvpe([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList, 
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _Env);
 intptr_t __cdecl _wspawnl([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wspawnle([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wspawnlp([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wspawnlpe([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _ArgList, ...);
 intptr_t __cdecl _wspawnv([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList);
 intptr_t __cdecl _wspawnve([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList,
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _Env);
 intptr_t __cdecl _wspawnvp([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList);
 intptr_t __cdecl _wspawnvpe([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Mode, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Filename, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _ArgList,
        [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * const * _Env);






// #line 387 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


































// #line 422 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


























// #line 449 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 451 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"







typedef unsigned short _ino_t;      


typedef unsigned short ino_t;
// #line 463 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 465 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


typedef unsigned int _dev_t;        


typedef unsigned int dev_t;
// #line 472 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 474 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


typedef long _off_t;                


typedef long off_t;
// #line 481 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 483 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"



struct _stat32 {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        _off_t     st_size;
        __time32_t st_atime;
        __time32_t st_mtime;
        __time32_t st_ctime;
        };



struct stat {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        _off_t     st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };

// #line 517 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

struct _stat32i64 {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        __int64    st_size;
        __time32_t st_atime;
        __time32_t st_mtime;
        __time32_t st_ctime;
        };

struct _stat64i32 {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        _off_t     st_size;
        __time64_t st_atime;
        __time64_t st_mtime;
        __time64_t st_ctime;
        };

struct _stat64 {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        __int64    st_size;
        __time64_t st_atime;
        __time64_t st_mtime;
        __time64_t st_ctime;
        };























// #line 583 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"



// #line 587 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"





 int __cdecl _wstat32([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Name, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _stat32 * _Stat);

 int __cdecl _wstat32i64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Name, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _stat32i64 * _Stat);
 int __cdecl _wstat64i32([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Name, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _stat64i32 * _Stat);
 int __cdecl _wstat64([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _Name, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] struct _stat64 * _Stat);


// #line 600 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 602 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"










  errno_t __cdecl _cgetws_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="*_SizeRead")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buffer, size_t _SizeInWords, [SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _SizeRead);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _cgetws_s([SA_Post(Deref=1,Null=SA_No,ValidElements="*_Buffer")] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size], size_t * _Size) { return _cgetws_s(_Buffer, _Size, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_cgetws_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _cgetws([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Buffer);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _getwch(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _getwche(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _putwch(wchar_t _WCh);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  wint_t __cdecl _ungetwch(wint_t _WCh);
  int __cdecl _cputws([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t * _String);
  int __cdecl _cwprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _cwprintf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_cwscanf_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _cwscanf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_cwscanf_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  int __cdecl _cwscanf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _cwscanf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, ...);
  int __cdecl _cwscanf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="scanf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
  int __cdecl _vcwprintf([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t *_Format, va_list _ArgList);
  int __cdecl _vcwprintf_s([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t *_Format, va_list _ArgList);

  int __cdecl _cwprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, ...);
  int __cdecl _vcwprintf_p([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t*  _Format, va_list _ArgList);

 int __cdecl _cwprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 int __cdecl _cwprintf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 int __cdecl _vcwprintf_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t *_Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
 int __cdecl _vcwprintf_s_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);
 int __cdecl _cwprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, ...);
 int __cdecl _vcwprintf_p_l([SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale, va_list _ArgList);

 wint_t __cdecl _putwch_nolock(wchar_t _WCh);
[returnvalue:SA_Post(MustCheck=SA_Yes)] wint_t __cdecl _getwch_nolock(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] wint_t __cdecl _getwche_nolock(void);
 wint_t __cdecl _ungetwch_nolock(wint_t _WCh);






// #line 650 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


// #line 653 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"






























































































































































































































































































































// #line 972 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"






















































































































































struct tm {
        int tm_sec;     
        int tm_min;     
        int tm_hour;    
        int tm_mday;    
        int tm_mon;     
        int tm_year;    
        int tm_wday;    
        int tm_yday;    
        int tm_isdst;   
        };

// #line 1135 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"




 
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wasctime_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wasctime([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm * _Tm);
 errno_t __cdecl _wasctime_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElementsConst=26)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm * _Tm);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wasctime_s([SA_Post(Deref=1,Null=SA_No,ValidElementsConst=26)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm * _Time) { return _wasctime_s(_Buffer, _Size, _Time); } }

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wctime32_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wctime32([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time32_t *_Time);
 errno_t __cdecl _wctime32_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElementsConst=26)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t* _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time32_t * _Time);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wctime32_s([SA_Post(Deref=1,Null=SA_No,ValidElementsConst=26)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time32_t * _Time) { return _wctime32_s(_Buffer, _Size, _Time); } }

 size_t __cdecl wcsftime([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t * _Format,  [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm * _Tm);
 size_t __cdecl _wcsftime_l([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_FormatString(Style="printf")] const wchar_t *_Format, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm *_Tm, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);

 errno_t __cdecl _wstrdate_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElementsConst=9)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wstrdate_s([SA_Post(Deref=1,Null=SA_No,ValidElementsConst=9)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size]) { return _wstrdate_s(_Buffer, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wstrdate_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wstrdate([SA_Pre(Null=SA_No,WritableElementsConst=9)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Buffer);

 errno_t __cdecl _wstrtime_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElementsConst=9)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wstrtime_s([SA_Post(Deref=1,Null=SA_No,ValidElementsConst=9)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size]) { return _wstrtime_s(_Buffer, _Size); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wstrtime_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wstrtime([SA_Pre(Null=SA_No,WritableElementsConst=9)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Buffer);

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wctime64_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  wchar_t * __cdecl _wctime64([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time64_t * _Time);
 errno_t __cdecl _wctime64_s([SA_Pre(Null=SA_No,WritableElements="_SizeInWords")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElementsConst=26)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t* _Buf, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInWords, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time64_t *_Time);
extern "C++" { template <size_t _Size> inline errno_t __cdecl _wctime64_s([SA_Post(Deref=1,Null=SA_No,ValidElementsConst=26)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Buffer)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const __time64_t * _Time) { return _wctime64_s(_Buffer, _Size, _Time); } }


// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"













#pragma once
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"







// #line 24 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"





#pragma warning(push)
#pragma warning(disable:4996)















static __inline wchar_t * __cdecl _wctime(const time_t * _Time)
{
#pragma warning( push )
#pragma warning( disable : 4996 )
    return _wctime64(_Time);
#pragma warning( pop )
}

static __inline errno_t __cdecl _wctime_s(wchar_t *_Buffer, size_t _SizeInWords, const time_t * _Time)
{
    return _wctime64_s(_Buffer, _SizeInWords, _Time);
}
// #line 59 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"

#pragma warning(pop)

// #line 63 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"
// #line 64 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wtime.inl"
// #line 1165 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 1166 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


// #line 1169 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


typedef int mbstate_t;
typedef wchar_t _Wint_t;

 wint_t __cdecl btowc(int);
 size_t __cdecl mbrlen([SA_Pre(Null=SA_Maybe,ValidBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _Ch, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes,
                              [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
 size_t __cdecl mbrtowc([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _DstCh, [SA_Pre(Null=SA_Maybe,ValidBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] const char * _SrcCh,
                               [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
 errno_t __cdecl mbsrtowcs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t* _Retval, [SA_Pre(Null=SA_Maybe,WritableElements="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_Maybe)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] const char ** _PSrc, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
extern "C++" { template <size_t _Size> inline errno_t __cdecl mbsrtowcs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Retval, [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] wchar_t (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_Maybe)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] const char ** _PSource, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State) { return mbsrtowcs_s(_Retval, _Dest, _Size, _PSource, _Count, _State); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "mbsrtowcs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl mbsrtowcs([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_Maybe)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] const char ** _PSrc, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] mbstate_t * _State);

 errno_t __cdecl wcrtomb_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Retval, [SA_Pre(Null=SA_Maybe,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst,
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Ch, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcrtomb_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Retval, [SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Source, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State) { return wcrtomb_s(_Retval, _Dest, _Size, _Source, _State); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcrtomb_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl wcrtomb([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _Source, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
 errno_t __cdecl wcsrtombs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Retval, [SA_Pre(Null=SA_Maybe,WritableBytes="_SizeInBytes")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="*_Retval")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _Dst,
        [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _SizeInBytes, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] const wchar_t ** _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
extern "C++" { template <size_t _Size> inline errno_t __cdecl wcsrtombs_s([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] size_t * _Retval, [SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char (&_Dest)[_Size], [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] const wchar_t ** _PSrc, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State) { return wcsrtombs_s(_Retval, _Dest, _Size, _PSrc, _Count, _State); } }
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcsrtombs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."))  size_t __cdecl wcsrtombs([SA_Pre(Null=SA_Maybe)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=2,Valid=SA_Yes)] [SA_Post(Deref=1,Null=SA_No,NullTerminated=SA_Yes)] [SA_Post(Deref=2,Valid=SA_Yes)] const wchar_t ** _PSource, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Count, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] mbstate_t * _State);
 int __cdecl wctob([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wint_t _WCh);






// #line 1199 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
  void *  __cdecl memmove([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_Size")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);
// #line 1201 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
 void *  __cdecl memcpy([SA_Pre(Null=SA_Maybe,WritableBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_Size")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_Maybe,ValidBytes="_Size")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Size);

 errno_t __cdecl memcpy_s([SA_Pre(Null=SA_Maybe,WritableBytes="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_MaxCount")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _DstSize, [SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
 errno_t __cdecl memmove_s([SA_Pre(Null=SA_Maybe,WritableBytes="_DstSize")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidBytes="_MaxCount")] [SA_Post(Deref=1,Valid=SA_Yes)] void * _Dst, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _DstSize, [SA_Pre(Null=SA_Maybe,ValidBytes="_MaxCount")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const void * _Src, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _MaxCount);
// #line 1206 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
__inline int __cdecl fwide([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] FILE * _F, int _M)
        {(void)_F; return (_M); }
__inline int __cdecl mbsinit([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const mbstate_t *_P)
        {return (_P == 0 || *_P == 0); }
__inline const wchar_t * __cdecl wmemchr([SA_Pre(Null=SA_No,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        {for (; 0 < _N; ++_S, --_N)
                if (*_S == _C)
                        return (const wchar_t *)(_S);
        return (0); }
__inline int __cdecl wmemcmp([SA_Pre(Null=SA_No,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S1, [SA_Pre(Null=SA_No,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        {for (; 0 < _N; ++_S1, ++_S2, --_N)
                if (*_S1 != *_S2)
                        return (*_S1 < *_S2 ? -1 : +1);
        return (0); }

__inline  wchar_t * __cdecl wmemcpy([SA_Pre(Null=SA_Maybe,WritableElements="_N")] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t *_S1, [SA_Pre(Null=SA_Maybe,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        {
#pragma warning( push )
#pragma warning( disable : 4996 6386 )
            return (wchar_t *)memcpy(_S1, _S2, _N*sizeof(wchar_t));
#pragma warning( pop )
        }


__inline errno_t __cdecl wmemcpy_s([SA_Pre(Null=SA_Maybe,WritableElements="_N1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_N")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_S1, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _N1, [SA_Pre(Null=SA_Maybe,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S2, rsize_t _N)
        {
#pragma warning( push )
#pragma warning( disable : 6386 )
            return memcpy_s(_S1, _N1*sizeof(wchar_t), _S2, _N*sizeof(wchar_t));
        }
#pragma warning( pop )
// #line 1238 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

__inline  wchar_t * __cdecl wmemmove([SA_Pre(Null=SA_Maybe,WritableElements="_N")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_N")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_S1, [SA_Pre(Null=SA_Maybe,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        {
#pragma warning( push )
#pragma warning( disable : 4996 6386 )
#pragma warning( disable : 6387)
			
            return (wchar_t *)memmove(_S1, _S2, _N*sizeof(wchar_t));
#pragma warning( pop )
        }


__inline errno_t __cdecl wmemmove_s([SA_Pre(Null=SA_Maybe,WritableElements="_N1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_N")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_S1, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _N1, [SA_Pre(Null=SA_Maybe,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const wchar_t *_S2, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] rsize_t _N)
        {
#pragma warning( push )
#pragma warning( disable : 6386 )
            return memmove_s(_S1, _N1*sizeof(wchar_t), _S2, _N*sizeof(wchar_t));
#pragma warning( pop )
        }
// #line 1258 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

__inline wchar_t * __cdecl wmemset([SA_Pre(Null=SA_No,WritableElements="_N")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_N")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_S, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        {
            wchar_t *_Su = _S;
            for (; 0 < _N; ++_Su, --_N)
            {
                *_Su = _C;
            }
            return (_S);
        }


extern "C++" {
inline wchar_t * __cdecl wmemchr([SA_Pre(Null=SA_No,ValidElements="_N")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_S, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] wchar_t _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _N)
        { return (wchar_t *)wmemchr((const wchar_t *)_S, _C, _N); }
}
// #line 1275 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"
// #line 1276 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


}       
// #line 1280 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"


#pragma pack(pop)
// #line 1284 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 1286 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\wchar.h"

// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"

typedef mbstate_t _Mbstatet;




typedef mbstate_t _Mbstatet;

 
namespace std {
using ::_Mbstatet;

using ::mbstate_t; using ::size_t;
using ::tm; using ::wint_t;


using ::btowc; using ::fgetwc; using ::fgetws; using ::fputwc;
using ::fputws; using ::fwide; using ::fwprintf;
using ::fwscanf; using ::getwc; using ::getwchar;
using ::mbrlen; using ::mbrtowc; using ::mbsrtowcs;
using ::mbsinit; using ::putwc; using ::putwchar;
using ::swprintf; using ::swscanf; using ::ungetwc;
using ::vfwprintf; using ::vswprintf; using ::vwprintf;
using ::wcrtomb; using ::wprintf; using ::wscanf;
using ::wcsrtombs; using ::wcstol; using ::wcscat;
using ::wcschr; using ::wcscmp; using ::wcscoll;
using ::wcscpy; using ::wcscspn; using ::wcslen;
using ::wcsncat; using ::wcsncmp; using ::wcsncpy;
using ::wcspbrk; using ::wcsrchr; using ::wcsspn;

using ::wcstod; using ::wcstoul; using ::wcsstr;

using ::wcstok; using ::wcsxfrm; using ::wctob;
using ::wmemchr; using ::wmemcmp; using ::wmemcpy;
using ::wmemmove; using ::wmemset; using ::wcsftime;


}
 // #line 54 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"
// #line 55 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"

// #line 57 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\cwchar"





// #line 9 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"













#pragma once
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"





#pragma pack(push,8)
// #line 25 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"












extern "C" {
// #line 39 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

 





typedef void *_HFILE; 


















typedef int (__cdecl * _CRT_REPORT_HOOK)(int, char *, int *);
typedef int (__cdecl * _CRT_REPORT_HOOKW)(int, wchar_t *, int *);



// #line 71 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"




// #line 76 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"




 





 










typedef int (__cdecl * _CRT_ALLOC_HOOK)(int, void *, size_t, int, long, const unsigned char *, int);


// #line 101 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


// #line 104 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

 





































 














typedef void (__cdecl * _CRT_DUMP_CLIENT)(void *, size_t);


// #line 162 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


// #line 165 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

struct _CrtMemBlockHeader;
typedef struct _CrtMemState
{
        struct _CrtMemBlockHeader * pBlockHeader;
        size_t lCounts[5];
        size_t lSizes[5];
        size_t lHighWaterCount;
        size_t lTotalCount;
} _CrtMemState;


 















 













// #line 208 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"



// #line 212 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"



// #line 216 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"



// #line 220 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


















































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 1103 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


}



extern "C++" {



 







 
#pragma warning(suppress: 4985)
[returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void * __cdecl operator new[](size_t _Size);


[returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void * __cdecl operator new(
        size_t _Size,
        int,
        const char *,
        int
        );

#pragma warning(suppress: 4985)
[returnvalue:SA_Post(Null=SA_No,WritableBytes="_Size")] void * __cdecl operator new[](
        size_t _Size,
        int,
        const char *,
        int
        );




void __cdecl operator delete[](void *);

inline void __cdecl operator delete(void * _P, int, const char *, int)
        { ::operator delete(_P); }
inline void __cdecl operator delete[](void * _P, int, const char *, int)
        { ::operator delete[](_P); }
// #line 1151 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"
// #line 1152 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


























































// #line 1211 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

}

// #line 1215 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

// #line 1217 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"


#pragma pack(pop)
// #line 1221 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

// #line 1223 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdbg.h"

// #line 11 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"
namespace std {

		
 




typedef long streamoff;
typedef int streamsize;
 // #line 27 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

 

// #line 31 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"


// #line 34 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"
  
 // #line 36 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"





extern   fpos_t _Fpz;
extern   const streamoff _BADOFF;
// #line 44 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

		
template<class _Statetype>
	class fpos
	{	
	typedef fpos<_Statetype> _Myt;

public:
	 fpos(streamoff _Off = 0)
		: _Myoff(_Off), _Fpos(0), _Mystate(_Stz)
		{	
		}

	 fpos(_Statetype _State, fpos_t _Fileposition)
		: _Myoff(0), _Fpos(_Fileposition), _Mystate(_State)
		{	
		}

	_Statetype  state() const
		{	
		return (_Mystate);
		}

	void  state(_Statetype _State)
		{	
		_Mystate = _State;
		}

	fpos_t  seekpos() const
		{	
		return (_Fpos);
		}

	 operator streamoff() const
		{	
		return ((streamoff)(_Myoff + ((long)(_Fpos))));
		}

	streamoff  operator-(const _Myt& _Right) const
		{	
		return ((streamoff)*this - (streamoff)_Right);
		}

	_Myt&  operator+=(streamoff _Off)
		{	
		_Myoff += _Off;
		return (*this);
		}

	_Myt&  operator-=(streamoff _Off)
		{	
		_Myoff -= _Off;
		return (*this);
		}

	_Myt  operator+(streamoff _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp += _Off);
		}

	_Myt  operator-(streamoff _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp -= _Off);
		}

	bool  operator==(const _Myt& _Right) const
		{	
		return ((streamoff)*this == (streamoff)_Right);
		}

	bool  operator!=(const _Myt& _Right) const
		{	
		return (!(*this == _Right));
		}

private:
	 static const _Statetype _Stz;	
	streamoff _Myoff;	
	fpos_t _Fpos;	
	_Statetype _Mystate;	
	};

	
template<class _Statetype>
	 const _Statetype fpos<_Statetype>::_Stz = _Statetype();

 

 
 

typedef fpos<_Mbstatet> streampos;
typedef streampos wstreampos;

		
struct _Unsecure_char_traits_tag
	{
	};
struct _Secure_char_traits_tag
	{
	};

		
		
		
		
		
struct _Char_traits_base
	{
	typedef _Unsecure_char_traits_tag _Secure_char_traits;
	};

		
		
		
template<class _Elem>
	struct char_traits:
		public _Char_traits_base
	{	
	typedef _Elem char_type;
	typedef long int_type;
	typedef streampos pos_type;
	typedef streamoff off_type;
	typedef _Mbstatet state_type;

	static void __cdecl assign(_Elem& _Left, const _Elem& _Right)
		{	
		_Left = _Right;
		}

	static bool __cdecl eq(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left == _Right);
		}

	static bool __cdecl lt(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left < _Right);
		}

	static int __cdecl compare(const _Elem *_First1,
		const _Elem *_First2, size_t _Count)
		{	


		for (; 0 < _Count; --_Count, ++_First1, ++_First2)
			if (!eq(*_First1, *_First2))
				return (lt(*_First1, *_First2) ? -1 : +1);
		return (0);
		}

	static size_t __cdecl length(const _Elem *_First)
		{	

		size_t _Count;
		for (_Count = 0; !eq(*_First, _Elem()); ++_First)
			++_Count;
		return (_Count);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl copy(_Elem *_First1,
		const _Elem *_First2, size_t _Count)
		{	
		
		return _Copy_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Copy_s(_Elem *_First1, size_t _Dest_size,
		const _Elem *_First2, size_t _Count)
		{	


		{ if (!(_Dest_size >= _Count)) { ((void)0); ::_invalid_parameter_noinfo(); return (0); } };
		_Elem *_Next = _First1;
		for (; 0 < _Count; --_Count, ++_Next, ++_First2)
			assign(*_Next, *_First2);
		return (_First1);
		}

	static const _Elem *__cdecl find(const _Elem *_First,
		size_t _Count, const _Elem& _Ch)
		{	

		for (; 0 < _Count; --_Count, ++_First)
			if (eq(*_First, _Ch))
				return (_First);
		return (0);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl move(_Elem *_First1,
		const _Elem *_First2, size_t _Count)
		{	
		
		return _Move_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Move_s(_Elem *_First1, size_t _Dest_size,
		const _Elem *_First2, size_t _Count)
		{	


		{ if (!(_Dest_size >= _Count)) { ((void)0); ::_invalid_parameter_noinfo(); return (0); } };
		_Elem *_Next = _First1;
		if (_First2 < _Next && _Next < _First2 + _Count)
			for (_Next += _Count, _First2 += _Count; 0 < _Count; --_Count)
				assign(*--_Next, *--_First2);
		else
			for (; 0 < _Count; --_Count, ++_Next, ++_First2)
				assign(*_Next, *_First2);
		return (_First1);
		}

	static _Elem *__cdecl assign(_Elem *_First,
		size_t _Count, _Elem _Ch)
		{	

		_Elem *_Next = _First;
		for (; 0 < _Count; --_Count, ++_Next)
			assign(*_Next, _Ch);
		return (_First);
		}

	static _Elem __cdecl to_char_type(const int_type& _Meta)
		{	
		return ((_Elem)_Meta);
		}

	static int_type __cdecl to_int_type(const _Elem& _Ch)
		{	
		return ((int_type)_Ch);
		}

	static bool __cdecl eq_int_type(const int_type& _Left,
		const int_type& _Right)
		{	
		return (_Left == _Right);
		}

	static int_type __cdecl eof()
		{	
		return ((int_type)(-1));
		}

	static int_type __cdecl not_eof(const int_type& _Meta)
		{	
		return (_Meta != eof() ? (int_type)_Meta : (int_type)!eof());
		}
	};

		
template<> struct  char_traits<wchar_t>:
	public _Char_traits_base
	{	
	typedef wchar_t _Elem;
	typedef _Elem char_type;	
	typedef wint_t int_type;
	typedef streampos pos_type;
	typedef streamoff off_type;
	typedef _Mbstatet state_type;

	static void __cdecl assign(_Elem& _Left, const _Elem& _Right)
		{	
		_Left = _Right;
		}

	static bool __cdecl eq(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left == _Right);
		}

	static bool __cdecl lt(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left < _Right);
		}

	static int __cdecl compare(const _Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	


		return (::wmemcmp(_First1, _First2, _Count));
		}

	static size_t __cdecl length(const _Elem *_First)
		{	

		return (::wcslen(_First));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl copy(_Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	
		
		return _Copy_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Copy_s(_Elem *_First1, size_t _Size_in_words, const _Elem *_First2,
		size_t _Count)
		{	


		::wmemcpy_s((_First1), (_Size_in_words), (_First2), (_Count));
		return _First1;
		}

	static const _Elem *__cdecl find(const _Elem *_First, size_t _Count,
		const _Elem& _Ch)
		{	

		return ((const _Elem *)::wmemchr(_First, _Ch, _Count));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl move(_Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	
		
		return _Move_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Move_s(_Elem *_First1, size_t _Size_in_words, const _Elem *_First2,
		size_t _Count)
		{	


		::wmemmove_s((_First1), (_Size_in_words), (_First2), (_Count));
		return (_Elem *)_First1;
		}

	static _Elem *__cdecl assign(_Elem *_First, size_t _Count, _Elem _Ch)
		{	

		return ((_Elem *)::wmemset(_First, _Ch, _Count));
		}

	static _Elem __cdecl to_char_type(const int_type& _Meta)
		{	
		return (_Meta);
		}

	static int_type __cdecl to_int_type(const _Elem& _Ch)
		{	
		return (_Ch);
		}

	static bool __cdecl eq_int_type(const int_type& _Left,
		const int_type& _Right)
		{	
		return (_Left == _Right);
		}

	static int_type __cdecl eof()
		{	
		return ((wint_t)(0xFFFF));
		}

	static int_type __cdecl not_eof(const int_type& _Meta)
		{	
		return (_Meta != eof() ? _Meta : !eof());
		}
	};


		
template<> struct  char_traits<char>:
	public _Char_traits_base
	{	
	typedef char _Elem;
	typedef _Elem char_type;
	typedef int int_type;
	typedef streampos pos_type;
	typedef streamoff off_type;
	typedef _Mbstatet state_type;

	static void __cdecl assign(_Elem& _Left, const _Elem& _Right)
		{	
		_Left = _Right;
		}

	static bool __cdecl eq(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left == _Right);
		}

	static bool __cdecl lt(const _Elem& _Left, const _Elem& _Right)
		{	
		return (_Left < _Right);
		}

	static int __cdecl compare(const _Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	


		return (::memcmp(_First1, _First2, _Count));
		}

	static size_t __cdecl length(const _Elem *_First)
		{	

		return (::strlen(_First));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl copy(_Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	
		
		return _Copy_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Copy_s(_Elem *_First1, size_t _Size_in_bytes, const _Elem *_First2,
		size_t _Count)
		{	


		::memcpy_s((_First1), (_Size_in_bytes), (_First2), (_Count));
		return _First1;
		}

	static const _Elem *__cdecl find(const _Elem *_First, size_t _Count,
		const _Elem& _Ch)
		{	

		return ((const _Elem *)::memchr(_First, _Ch, _Count));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	static _Elem *__cdecl move(_Elem *_First1, const _Elem *_First2,
		size_t _Count)
		{	
		
		return _Move_s(_First1, _Count, _First2, _Count);
		}

	static _Elem *__cdecl _Move_s(_Elem *_First1, size_t _Size_in_bytes, const _Elem *_First2,
		size_t _Count)
		{	


		::memmove_s((_First1), (_Size_in_bytes), (_First2), (_Count));
		return _First1;
		}

	static _Elem *__cdecl assign(_Elem *_First, size_t _Count, _Elem _Ch)
		{	

		return ((_Elem *)::memset(_First, _Ch, _Count));
		}

	static _Elem __cdecl to_char_type(const int_type& _Meta)
		{	
		return ((_Elem)_Meta);
		}

	static int_type __cdecl to_int_type(const _Elem& _Ch)
		{	
		return ((unsigned char)_Ch);
		}

	static bool __cdecl eq_int_type(const int_type& _Left,
		const int_type& _Right)
		{	
		return (_Left == _Right);
		}

	static int_type __cdecl eof()
		{	
		return ((-1));
		}

	static int_type __cdecl not_eof(const int_type& _Meta)
		{	
		return (_Meta != eof() ? _Meta : !eof());
		}
	};

		
	template <class _Traits>
	class _Inherits_from_char_traits_base
	{
		typedef char _True;
		class _False { char _Dummy[2]; };
		static _True _Inherits(_Char_traits_base);
		static _False _Inherits(...);
		static _Traits _Make_traits();

	public:
		enum { _Exists = sizeof(_Inherits(_Make_traits())) == sizeof(_True) };
	};

	template <class _Traits, bool _Inherits_from_char_traits_base>
	class _Char_traits_category_helper
	{
	public:
		typedef _Unsecure_char_traits_tag _Secure_char_traits;
	};

	
	
	template <class _Elem>
	class _Char_traits_category_helper< char_traits<_Elem> , true>
	{
	public:
		typedef _Secure_char_traits_tag _Secure_char_traits;
	};

	template <class _Traits>
	class _Char_traits_category_helper<_Traits, true>
	{
	public:
		typedef typename _Traits::_Secure_char_traits _Secure_char_traits;
	};

	template <class _Traits>
	class _Char_traits_category
	{
	public:
		typedef typename _Char_traits_category_helper<_Traits, _Inherits_from_char_traits_base<_Traits>::_Exists>::_Secure_char_traits _Secure_char_traits;
	};

	
	
	template<class _Traits> 
	inline
	typename _Char_traits_category<_Traits>::_Secure_char_traits _Char_traits_cat()
	{
		typename _Char_traits_category<_Traits>::_Secure_char_traits _Secure;
		return (_Secure);
	}

	namespace _Traits_helper
	{

		
	template<class _Traits>
	inline
	typename _Traits::char_type *copy_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count)
	{
		return copy_s<_Traits>(_First1, _Size, _First2, _Count, _Char_traits_cat<_Traits>());
	}

	template<class _Traits>
	inline
	typename _Traits::char_type *copy_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count, _Secure_char_traits_tag)
	{
		return _Traits::_Copy_s(_First1, _Size, _First2, _Count);
	}

	
	template<class _Traits>
	inline
	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	typename _Traits::char_type *copy_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count, _Unsecure_char_traits_tag)
	{
		return _Traits::copy(_First1, _First2, _Count);
	}

		
	template<class _Traits>
	inline
	typename _Traits::char_type *move_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count)
	{
		return move_s<_Traits>(_First1, _Size, _First2, _Count, _Char_traits_cat<_Traits>());
	}

	template<class _Traits>
	inline
	typename _Traits::char_type *move_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count, _Secure_char_traits_tag)
	{
		return _Traits::_Move_s(_First1, _Size, _First2, _Count);
	}

	
	template<class _Traits>
	inline
	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	typename _Traits::char_type *move_s(typename _Traits::char_type *_First1, size_t _Size,
		const typename _Traits::char_type *_First2, size_t _Count, _Unsecure_char_traits_tag)
	{
		return _Traits::move(_First1, _First2, _Count);
	}

	} 

		
template<class _Ty>
	class allocator;
class ios_base;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_ios;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class istreambuf_iterator;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class ostreambuf_iterator;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_streambuf;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_istream;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_ostream;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_iostream;
template<class _Elem,
	class _Traits = char_traits<_Elem>,
	class _Alloc = allocator<_Elem> >
	class basic_stringbuf;
template<class _Elem,
	class _Traits = char_traits<_Elem>,
	class _Alloc = allocator<_Elem> >
	class basic_istringstream;
template<class _Elem,
	class _Traits = char_traits<_Elem>,
	class _Alloc = allocator<_Elem> >
	class basic_ostringstream;
template<class _Elem,
	class _Traits = char_traits<_Elem>,
	class _Alloc = allocator<_Elem> >
	class basic_stringstream;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_filebuf;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_ifstream;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_ofstream;
template<class _Elem,
	class _Traits = char_traits<_Elem> >
	class basic_fstream;

 








// #line 703 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

		
typedef basic_ios<char, char_traits<char> > ios;
typedef basic_streambuf<char, char_traits<char> > streambuf;
typedef basic_istream<char, char_traits<char> > istream;
typedef basic_ostream<char, char_traits<char> > ostream;
typedef basic_iostream<char, char_traits<char> > iostream;
typedef basic_stringbuf<char, char_traits<char>,
	allocator<char> > stringbuf;
typedef basic_istringstream<char, char_traits<char>,
	allocator<char> > istringstream;
typedef basic_ostringstream<char, char_traits<char>,
	allocator<char> > ostringstream;
typedef basic_stringstream<char, char_traits<char>,
	allocator<char> > stringstream;
typedef basic_filebuf<char, char_traits<char> > filebuf;
typedef basic_ifstream<char, char_traits<char> > ifstream;
typedef basic_ofstream<char, char_traits<char> > ofstream;
typedef basic_fstream<char, char_traits<char> > fstream;

		
typedef basic_ios<wchar_t, char_traits<wchar_t> > wios;
typedef basic_streambuf<wchar_t, char_traits<wchar_t> >
	wstreambuf;
typedef basic_istream<wchar_t, char_traits<wchar_t> > wistream;
typedef basic_ostream<wchar_t, char_traits<wchar_t> > wostream;
typedef basic_iostream<wchar_t, char_traits<wchar_t> > wiostream;
typedef basic_stringbuf<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wstringbuf;
typedef basic_istringstream<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wistringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wostringstream;
typedef basic_stringstream<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wstringstream;
typedef basic_filebuf<wchar_t, char_traits<wchar_t> > wfilebuf;
typedef basic_ifstream<wchar_t, char_traits<wchar_t> > wifstream;
typedef basic_ofstream<wchar_t, char_traits<wchar_t> > wofstream;
typedef basic_fstream<wchar_t, char_traits<wchar_t> > wfstream;



 










// #line 757 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

}


 #pragma warning(pop)
 #pragma pack(pop)
// #line 764 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"

// #line 766 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"
// #line 767 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iosfwd"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"

namespace std {
		
template<class _Ty> inline
	void swap(_Ty& _Left, _Ty& _Right)
	{	
	if (&_Left != &_Right)
		{	
		_Ty _Tmp = _Left;

		_Left = _Right;
		_Right = _Tmp;
		}
	}

		
template<class _Ty1,
	class _Ty2> struct pair
	{	
	typedef pair<_Ty1, _Ty2> _Myt;
	typedef _Ty1 first_type;
	typedef _Ty2 second_type;

	pair()
		: first(_Ty1()), second(_Ty2())
		{	
		}

	pair(const _Ty1& _Val1, const _Ty2& _Val2)
		: first(_Val1), second(_Val2)
		{	
		}

	template<class _Other1,
		class _Other2>
		pair(const pair<_Other1, _Other2>& _Right)
		: first(_Right.first), second(_Right.second)
		{	
		}

	void swap(_Myt& _Right)
		{	
		if (this != &_Right)
			{	
			std::swap(first, _Right.first);
			std::swap(second, _Right.second);
			}
		}

	_Ty1 first;	
	_Ty2 second;	
	};

		
template<class _Ty1,
	class _Ty2> inline
	bool operator==(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (_Left.first == _Right.first && _Left.second == _Right.second);
	}

template<class _Ty1,
	class _Ty2> inline
	bool operator!=(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (!(_Left == _Right));
	}

template<class _Ty1,
	class _Ty2> inline
	bool operator<(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (_Left.first < _Right.first ||
		!(_Right.first < _Left.first) && _Left.second < _Right.second);
	}

template<class _Ty1,
	class _Ty2> inline
	bool operator>(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (_Right < _Left);
	}

template<class _Ty1,
	class _Ty2> inline
	bool operator<=(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (!(_Right < _Left));
	}

template<class _Ty1,
	class _Ty2> inline
	bool operator>=(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	
	return (!(_Left < _Right));
	}

template<class _Ty1,
	class _Ty2> inline
	pair<_Ty1, _Ty2> make_pair(_Ty1 _Val1, _Ty2 _Val2)
	{	
	return (pair<_Ty1, _Ty2>(_Val1, _Val2));
	}

template<class _Ty1,
	class _Ty2> inline
	void swap(pair<_Ty1, _Ty2>& _Left, pair<_Ty1, _Ty2>& _Right)
	{	
	_Left.swap(_Right);
	}

		
	namespace rel_ops
		{	
template<class _Ty> inline
	bool  operator!=(const _Ty& _Left, const _Ty& _Right)
	{	
	return (!(_Left == _Right));
	}

template<class _Ty> inline
	bool operator>(const _Ty& _Left, const _Ty& _Right)
	{	
	return (_Right < _Left);
	}

template<class _Ty> inline
	bool operator<=(const _Ty& _Left, const _Ty& _Right)
	{	
	return (!(_Right < _Left));
	}

template<class _Ty> inline
	bool operator>=(const _Ty& _Left, const _Ty& _Right)
	{	
	return (!(_Left < _Right));
	}
		}
}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 161 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"

// #line 163 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"
// #line 164 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\utility"























// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

namespace std {

		
struct _Unchecked_iterator_tag
	{
	};
struct _Range_checked_iterator_tag
	{
	};

		
struct _Unchanged_checked_iterator_base_type_tag
	{
	};

struct _Different_checked_iterator_base_type_tag
	{
	};

		
struct _Undefined_inner_type_tag
	{
	};

		
struct _Undefined_move_tag
	{
	};

struct _Swap_move_tag
	{
	};


class _Iterator_base_universal
	{	
	public:
	typedef _Unchecked_iterator_tag _Checked_iterator_category;
	typedef _Unchanged_checked_iterator_base_type_tag _Checked_iterator_base_type;
	typedef _Undefined_inner_type_tag _Inner_type;
	};


		


		


 

































































































































































































































































































// #line 355 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"




class _Container_base_aux;

class _Aux_cont
	{
	public:
	explicit _Aux_cont(const _Container_base_aux * const _Pcont)
		: _Mycontainer(_Pcont) { }

	const _Container_base_aux * _Getcont() const
		{
		return _Mycontainer;
		}

	private:
	const _Container_base_aux * _Mycontainer;
	};

class _Container_base_aux
	{	
	public:
	_Container_base_aux()
		{
		
		}

	_Container_base_aux(const _Container_base_aux&)
		{
		
		}

	_Container_base_aux& operator=(const _Container_base_aux&)
		{
		
		return *this;
		}

	void _Swap_aux(_Container_base_aux& _Right)
		{
		std::swap(_Myownedaux, _Right._Myownedaux);
		std::swap(*_Myownedaux, *_Right._Myownedaux);
		}

	~_Container_base_aux()
		{
		
		}

	protected:
	friend class _Iterator_base_aux;

	_Aux_cont * _Myownedaux;
	};

template<class _Alloc>
	class _Container_base_aux_alloc_real
		: public _Container_base_aux
	{ 
	protected:
	explicit _Container_base_aux_alloc_real(_Alloc _Al)
		: _Alaux(_Al)
		{
		_Myownedaux = new (_Alaux.allocate(1)) _Aux_cont(this);
		}

	_Container_base_aux_alloc_real(const _Container_base_aux_alloc_real& _Right)
		: _Alaux(_Right._Alaux)
		{
		_Myownedaux = new (_Alaux.allocate(1)) _Aux_cont(this);
		}

	_Container_base_aux_alloc_real& operator=(const _Container_base_aux_alloc_real&)
		{
		
		return *this;
		}

	~_Container_base_aux_alloc_real()
		{
		_Myownedaux->~_Aux_cont();

		_Alaux.deallocate(_Myownedaux, 1);
		}

	typename _Alloc::template rebind<_Aux_cont>::other _Alaux; 
	};

class _Iterator_base_aux : public _Iterator_base_universal
	{	
	public:
	_Iterator_base_aux()
		: _Myaux(0) { }

	void _Set_container(const _Container_base_aux * const _Pcont)
		{
		_Myaux = _Pcont->_Myownedaux;
		}

	const _Container_base_aux * _Getmycont() const
		{	
		return _Myaux ? _Myaux->_Getcont() : 0;
		}

	bool _Same_container(const _Iterator_base_aux& _Other) const
		{	
		return _Myaux == _Other._Myaux;
		}

	bool _Has_container() const
		{	
		return _Myaux != 0;
		}

	private:
	const _Aux_cont * _Myaux;
	};


		
 
 
 

  

































































class  _Container_base_secure
	{	
	};











class _Iterator_base_secure : public _Iterator_base_universal
	{	
	public:
	
	 _Iterator_base_secure()
		: _Mycont(0)
		{
		}

	const _Container_base_secure *  _Getmycont() const
		{	
		return _Mycont;
		}

	bool  _Same_container(const _Iterator_base_secure& _Other) const
		{	
		return _Mycont == _Other._Mycont;
		}

	bool  _Has_container() const
		{	
		return _Mycont != 0;
		}

	const _Container_base_secure *_Mycont;
	};

   


typedef _Container_base_aux _Container_base;
typedef _Iterator_base_aux _Iterator_base;

   













// #line 609 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
  // #line 610 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
 // #line 611 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"


template<class _Alloc>
	class _Container_base_aux_alloc_empty
		: public _Container_base
	{ 
	protected:
	explicit _Container_base_aux_alloc_empty(_Alloc) { }

	_Container_base_aux_alloc_empty(const _Container_base_aux_alloc_empty&) { }

	_Container_base_aux_alloc_empty& operator=(const _Container_base_aux_alloc_empty&)
		{
		return *this;
		}

	~_Container_base_aux_alloc_empty() { }
	};


	
	



// #line 637 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"




		
struct input_iterator_tag
	{	
	};

struct output_iterator_tag
	{	
	};

struct forward_iterator_tag
	: public input_iterator_tag
	{	
	};

struct bidirectional_iterator_tag
	: public forward_iterator_tag
	{	
	};

struct random_access_iterator_tag
	: public bidirectional_iterator_tag
	{	
	};

struct _Int_iterator_tag
	{	
	};

struct _Float_iterator_tag
	{	
	};

		
struct _Nonscalar_ptr_iterator_tag
	{	
	};
struct _Scalar_ptr_iterator_tag
	{	
	};

		
template<class _Category,
	class _Ty,
	class _Diff = ptrdiff_t,
	class _Pointer = _Ty *,
	class _Reference = _Ty&>
		struct iterator
			: public _Iterator_base_universal

	{	
	typedef _Category iterator_category;
	typedef _Ty value_type;
	typedef _Diff difference_type;
	typedef _Diff distance_type;	
	typedef _Pointer pointer;
	typedef _Reference reference;
	};

template<class _Category,
	class _Ty,
	class _Diff = ptrdiff_t,
	class _Pointer = _Ty *,
	class _Reference = _Ty&,
	class _Base_class = _Iterator_base>
		struct _Iterator_with_base
			: public _Base_class

	{	
	typedef _Category iterator_category;
	typedef _Ty value_type;
	typedef _Diff difference_type;
	typedef _Diff distance_type;	
	typedef _Pointer pointer;
	typedef _Reference reference;
	};

template<class _Ty,
	class _Diff,
	class _Pointer,
	class _Reference>
	struct _Bidit
		: public _Iterator_with_base<bidirectional_iterator_tag,
			_Ty, _Diff, _Pointer, _Reference>
	{	
	};

template<class _Ty,
	class _Diff,
	class _Pointer,
	class _Reference>
	struct _Ranit
		: public _Iterator_with_base<random_access_iterator_tag,
			_Ty, _Diff, _Pointer, _Reference>
	{	
	};

template<class _Ty,
	class _Diff,
	class _Pointer,
	class _Reference,
	class _Base_class>
	struct _Ranit_base
		: public _Base_class

	{	
	typedef random_access_iterator_tag iterator_category;
	typedef _Ty value_type;
	typedef _Diff difference_type;
	typedef _Diff distance_type;	
	typedef _Pointer pointer;
	typedef _Reference reference;
	};

struct _Outit
	: public iterator<output_iterator_tag, void, void,
		void, void>
	{	
	};

		
template<class _Iter>
	struct iterator_traits
	{	
	typedef typename _Iter::iterator_category iterator_category;
	typedef typename _Iter::value_type value_type;
	typedef typename _Iter::difference_type difference_type;
	typedef difference_type distance_type;	
	typedef typename _Iter::pointer pointer;
	typedef typename _Iter::reference reference;
	};

template<class _Ty>
	struct iterator_traits<_Ty *>
	{	
	typedef random_access_iterator_tag iterator_category;
	typedef _Ty value_type;
	typedef ptrdiff_t difference_type;
	typedef ptrdiff_t distance_type;	
	typedef _Ty *pointer;
	typedef _Ty& reference;
	};

template<class _Ty>
	struct iterator_traits<const _Ty *>
	{	
	typedef random_access_iterator_tag iterator_category;
	typedef _Ty value_type;
	typedef ptrdiff_t difference_type;
	typedef ptrdiff_t distance_type;	
	typedef const _Ty *pointer;
	typedef const _Ty& reference;
	};

template<> struct iterator_traits<_Bool>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef _Bool value_type;
	typedef _Bool difference_type;
	typedef _Bool distance_type;
	typedef _Bool * pointer;
	typedef _Bool& reference;
	};

template<> struct iterator_traits<char>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef char value_type;
	typedef char difference_type;
	typedef char distance_type;
	typedef char * pointer;
	typedef char& reference;
	};

template<> struct iterator_traits<signed char>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef signed char value_type;
	typedef signed char difference_type;
	typedef signed char distance_type;
	typedef signed char * pointer;
	typedef signed char& reference;
	};

template<> struct iterator_traits<unsigned char>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef unsigned char value_type;
	typedef unsigned char difference_type;
	typedef unsigned char distance_type;
	typedef unsigned char * pointer;
	typedef unsigned char& reference;
	};

 
template<> struct iterator_traits<wchar_t>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef wchar_t value_type;
	typedef wchar_t difference_type;
	typedef wchar_t distance_type;
	typedef wchar_t * pointer;
	typedef wchar_t& reference;
	};
 // #line 845 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

template<> struct iterator_traits<short>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef short value_type;
	typedef short difference_type;
	typedef short distance_type;
	typedef short * pointer;
	typedef short& reference;
	};

template<> struct iterator_traits<unsigned short>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef unsigned short value_type;
	typedef unsigned short difference_type;
	typedef unsigned short distance_type;
	typedef unsigned short * pointer;
	typedef unsigned short& reference;
	};

template<> struct iterator_traits<int>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef int value_type;
	typedef int difference_type;
	typedef int distance_type;	
	typedef int * pointer;
	typedef int& reference;
	};

template<> struct iterator_traits<unsigned int>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef unsigned int value_type;
	typedef unsigned int difference_type;
	typedef unsigned int distance_type;	
	typedef unsigned int * pointer;
	typedef unsigned int& reference;
	};

template<> struct iterator_traits<long>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef long value_type;
	typedef long difference_type;
	typedef long distance_type;	
	typedef long * pointer;
	typedef long& reference;
	};

template<> struct iterator_traits<unsigned long>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef unsigned long value_type;
	typedef unsigned long difference_type;
	typedef unsigned long distance_type;	
	typedef unsigned long * pointer;
	typedef unsigned long& reference;
	};

template<> struct iterator_traits<double>
	{
	typedef _Float_iterator_tag iterator_category;
	typedef double value_type;
	typedef double difference_type;
	typedef double distance_type;	
	typedef double * pointer;
	typedef double& reference;
	};

template<> struct iterator_traits<float>
	{
	typedef _Float_iterator_tag iterator_category;
	typedef float value_type;
	typedef float difference_type;
	typedef float distance_type;	
	typedef float * pointer;
	typedef float& reference;
	};

#if 0
// DQ: removed
template<> struct iterator_traits<__int64>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef __int64 value_type;
	typedef __int64 difference_type;
	typedef __int64 distance_type;	
	typedef __int64 * pointer;
	typedef __int64& reference;
	};

template<> struct iterator_traits<unsigned __int64>
	{	
	typedef _Int_iterator_tag iterator_category;
	typedef unsigned __int64 value_type;
	typedef unsigned __int64 difference_type;
	typedef unsigned __int64 distance_type;	
	typedef unsigned __int64 * pointer;
	typedef unsigned __int64& reference;
	};
 // #line 947 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
#endif
		
template<class _Iter> inline
	typename iterator_traits<_Iter>::iterator_category
		__cdecl _Iter_cat(const _Iter&)
	{	
	typename iterator_traits<_Iter>::iterator_category _Cat;
	return (_Cat);
	}

		
		
		
template<class _Cat1, class _Cat2>
	class _Iter_random_helper
	{
	public:
		typedef forward_iterator_tag _Iter_random_cat;
	};

template<>
	class _Iter_random_helper<random_access_iterator_tag, random_access_iterator_tag>
	{
	public:
		typedef random_access_iterator_tag _Iter_random_cat;
	};

template<class _Cat1, class _Cat2, class _Cat3>
	class _Iter_random_helper3
	{
	public:
		typedef forward_iterator_tag _Iter_random_cat;
	};

template<>
	class _Iter_random_helper3<random_access_iterator_tag, random_access_iterator_tag, random_access_iterator_tag>
	{
	public:
		typedef random_access_iterator_tag _Iter_random_cat;
	};

template<class _Iter1, class _Iter2> inline
	typename _Iter_random_helper<
		typename iterator_traits<_Iter1>::iterator_category,
		typename iterator_traits<_Iter2>::iterator_category>::_Iter_random_cat
		__cdecl _Iter_random(const _Iter1&, const _Iter2&)
	{	
	typename _Iter_random_helper<
		iterator_traits<_Iter1>::iterator_category, 
		iterator_traits<_Iter2>::iterator_category>::_Iter_random_cat _Cat;
	return (_Cat);
	}

template<class _Iter1, class _Iter2, class _Iter3> inline
	typename _Iter_random_helper3<
		typename iterator_traits<_Iter1>::iterator_category,
		typename iterator_traits<_Iter2>::iterator_category,
		typename iterator_traits<_Iter3>::iterator_category>::_Iter_random_cat
		__cdecl _Iter_random(const _Iter1&, const _Iter2&, const _Iter3&)
	{	
	typename _Iter_random_helper3<
		iterator_traits<_Iter1>::iterator_category, 
		iterator_traits<_Iter2>::iterator_category,
		iterator_traits<_Iter3>::iterator_category>::_Iter_random_cat _Cat;
	return (_Cat);
	}

		
		
	template <bool _Cond, class _Ty1, class _Ty2>
	class _If
	{
	public:
		typedef _Ty2 _Result;
	};

	template <class _Ty1, class _Ty2>
	class _If<true, _Ty1, _Ty2>
	{
	public:
		typedef _Ty1 _Result;
	};

		
	template <bool _Secure_validation>
	class _Secure_validation_helper
	{
	public:
		typedef _Unchecked_iterator_tag _Checked_iterator_category;
	};

	template <>
	class _Secure_validation_helper<true>
	{
	public:
		typedef _Range_checked_iterator_tag _Checked_iterator_category;
	};


		
	template <class _Iter, bool _Inherits_from_iterator_base>
	class _Checked_iterator_category_helper
	{
	public:
		typedef _Unchecked_iterator_tag _Checked_cat;
	};

	template <class _Iter>
	class _Checked_iterator_category_helper<_Iter, true>
	{
	public:
		typedef typename _Iter::_Checked_iterator_category _Checked_cat;
	};

	template <class _Iter>
	class _Checked_iterator_category
	{
	public:
		typedef typename _Checked_iterator_category_helper<_Iter, __is_base_of(_Iterator_base_universal, _Iter)>::_Checked_cat _Checked_cat;
	};

	template<class _Iter> 
	inline
	typename _Checked_iterator_category<_Iter>::_Checked_cat _Checked_cat(const _Iter&)
	{
		typename _Checked_iterator_category<_Iter>::_Checked_cat _Cat;
		return (_Cat);
	}

		
	template <class _Iter, bool _Inherits_from_iterator_base>
	class _Checked_iterator_base_helper2
	{
	public:
		typedef _Unchanged_checked_iterator_base_type_tag _Checked_iterator_base_type;
	};

	template <class _Iter>
	class _Checked_iterator_base_helper2<_Iter, true>
	{
	public:
		typedef typename _Iter::_Checked_iterator_base_type _Checked_iterator_base_type;
	};

	template <class _Iter, class _Base_type>
	class _Checked_iterator_base_helper1
	{
	public:
		typedef _Different_checked_iterator_base_type_tag _Base_type_tag;
		typedef _Base_type _Checked_iterator_base_type;
	};

	template <class _Iter>
	class _Checked_iterator_base_helper1<_Iter, _Unchanged_checked_iterator_base_type_tag>
	{
	public:
		typedef _Unchanged_checked_iterator_base_type_tag _Base_type_tag;
		typedef _Iter _Checked_iterator_base_type;
	};

	template <class _Iter>
	class _Checked_iterator_base_helper
	{
	public:
		typedef _Checked_iterator_base_helper2<_Iter, __is_base_of(_Iterator_base_universal, _Iter)> _Base_helper2;
		typedef _Checked_iterator_base_helper1<_Iter, typename  _Base_helper2::_Checked_iterator_base_type > _Base_helper1;

		typedef typename _Base_helper1::_Base_type_tag _Checked_iterator_base_type_tag;
		typedef typename _Base_helper1::_Checked_iterator_base_type _Checked_iterator_base_type;
	};

	template<class _Iter, class _Base_tag>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(const _Iter &_It, _Base_tag)
	{
		return _It._Checked_iterator_base();
	}

	template<class _Iter>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(const _Iter &_It, _Unchanged_checked_iterator_base_type_tag)
	{
		return _It;
	}

	template<class _Iter, class _Base_tag>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(_Iter &_It, _Base_tag)
	{
		return _It._Checked_iterator_base();
	}

	template<class _Iter>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(_Iter &_It, _Unchanged_checked_iterator_base_type_tag)
	{
		return _It;
	}

	template<class _Iter>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(const _Iter &_It)
	{
		typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type_tag _Base_tag;
		return _Checked_base(_It, _Base_tag);
	}

	template<class _Iter>
	inline
	typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type
		__cdecl _Checked_base(_Iter &_It)
	{
		typename _Checked_iterator_base_helper<_Iter>::_Checked_iterator_base_type_tag _Base_tag;
		return _Checked_base(_It, _Base_tag);
	}

		

	template<class _DstIter, class _BaseIter>
	inline
	void __cdecl _Checked_assign_from_base(_DstIter &_Dest, const _BaseIter &_Src)
	{
		_Dest._Checked_iterator_assign_from_base(_Src);
	}

	template<class _Iter>
	inline
	void __cdecl _Checked_assign_from_base(_Iter &_Dest, const _Iter &_Src)
	{
		_Dest = _Src;
	}

		
 
 
 // #line 1188 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	
	template <class _Value>
	class _Move_operation_category
	{
	public:
		typedef _Undefined_move_tag _Move_cat;
	};

	template<class _Iter>
	inline
		typename _Move_operation_category<typename iterator_traits<_Iter>::value_type>::_Move_cat _Move_cat(const _Iter&)
	{
		typename _Move_operation_category<typename iterator_traits<_Iter>::value_type>::_Move_cat _Cat;
		return (_Cat);
	}

		
template<class _T1, class _T2, class _Checked_Cat1, class _Checked_Cat2>
struct _Ptr_cat_with_checked_cat_helper
	{
	typedef _Nonscalar_ptr_iterator_tag _Ptr_cat;
	};

template<class _T1,	class _T2>
struct _Ptr_cat_helper
	{
	typedef typename _Ptr_cat_with_checked_cat_helper<_T1, _T2, 
		typename _Checked_iterator_category<_T1>::_Checked_cat,
		typename _Checked_iterator_category<_T2>::_Checked_cat>::_Ptr_cat _Ptr_cat;
	};

template<class _T1,	class _T2, class _Checked_Cat1>
struct _Ptr_cat_with_checked_cat_helper<_T1, _T2, _Checked_Cat1, _Range_checked_iterator_tag>
	{
	typedef typename _Ptr_cat_helper<_T1, typename _T2::_Inner_type>::_Ptr_cat _Ptr_cat;
	};

template<class _T1,	class _T2, class _Checked_Cat2>
struct _Ptr_cat_with_checked_cat_helper<_T1, _T2, _Range_checked_iterator_tag, _Checked_Cat2>
	{
	typedef typename _Ptr_cat_helper<typename _T1::_Inner_type, _T2>::_Ptr_cat _Ptr_cat;
	};

template<class _T1,	class _T2>
struct _Ptr_cat_with_checked_cat_helper<_T1, _T2, _Range_checked_iterator_tag, _Range_checked_iterator_tag>
	{
	typedef typename _Ptr_cat_helper<typename _T1::_Inner_type, typename _T2::_Inner_type>::_Ptr_cat _Ptr_cat;
	};

template<class _T1>
struct _Ptr_cat_helper<_T1, _Undefined_inner_type_tag>
	{
	typedef _Nonscalar_ptr_iterator_tag _Ptr_cat;
	};

template<class _T2>
struct _Ptr_cat_helper<_Undefined_inner_type_tag, _T2>
	{
	typedef _Nonscalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<_Undefined_inner_type_tag, _Undefined_inner_type_tag>
	{
	typedef _Nonscalar_ptr_iterator_tag _Ptr_cat;
	};

		
template<>
struct _Ptr_cat_helper<_Bool *, _Bool *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const _Bool *, _Bool *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<char *, char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const char *, char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<signed char *, signed char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const signed char *, signed char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<unsigned char *, unsigned char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const unsigned char *, unsigned char *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

 

template<>
struct _Ptr_cat_helper<wchar_t *, wchar_t *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const wchar_t *, wchar_t *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

 // #line 1320 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
template<>
struct _Ptr_cat_helper<short *, short *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const short *, short *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<unsigned short *, unsigned short *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const unsigned short *, unsigned short *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<int *, int *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const int *, int *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<unsigned int *, unsigned int *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const unsigned int *, unsigned int *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<long *, long *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const long *, long *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<unsigned long *, unsigned long *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const unsigned long *, unsigned long *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<float *, float *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const float *, float *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<double *, double *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const double *, double *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<long double *, long double *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const long double *, long double *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

 
#if 0
// DQ: removed
template<>
struct _Ptr_cat_helper<__int64 *, __int64 *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const __int64 *, __int64 *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<unsigned __int64 *, unsigned __int64 *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<>
struct _Ptr_cat_helper<const unsigned __int64 *, unsigned __int64 *>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};
#endif

 // #line 1455 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

template<class _Ty>
struct _Ptr_cat_helper<_Ty **, _Ty **>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<class _Ty>
struct _Ptr_cat_helper<_Ty **, const _Ty **>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<class _Ty>
struct _Ptr_cat_helper<_Ty *const *, _Ty **>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};

template<class _Ty>
struct _Ptr_cat_helper<_Ty *const *, const _Ty **>
	{	
	typedef _Scalar_ptr_iterator_tag _Ptr_cat;
	};


template<class _T1, class _T2> inline 
typename _Ptr_cat_helper<_T1, _T2>::_Ptr_cat __cdecl _Ptr_cat(_T1&, _T2&)
	{
	typename _Ptr_cat_helper<_T1, _T2>::_Ptr_cat _Cat;
	return (_Cat);
	}


 
// #line 1491 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"


 
// #line 1495 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

 





























































































































































































// #line 1687 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
 
 
 
 
 
 
 
 
 // #line 1696 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

 

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  


  
  
  
  
  
  
  
  

 






// #line 1734 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		

template<class _Iter> inline
	typename iterator_traits<_Iter>::value_type *__cdecl _Val_type(_Iter)
	{	
	return (0);
	}

		
template<class _InIt,
	class _Diff> inline
	void __cdecl advance(_InIt& _Where, _Diff _Off)
	{	
	_Advance(_Where, _Off, _Iter_cat(_Where));
	}

template<class _InIt,
	class _Diff> inline
	void __cdecl _Advance(_InIt& _Where, _Diff _Off, input_iterator_tag)
	{	

 


// #line 1760 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	for (; 0 < _Off; --_Off)
		++_Where;
	}

template<class _FI,
	class _Diff> inline
	void __cdecl _Advance(_FI& _Where, _Diff _Off, forward_iterator_tag)
	{	

 


// #line 1774 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	for (; 0 < _Off; --_Off)
		++_Where;
	}

#pragma warning(push)
#pragma warning(disable: 6295)
template<class _BI,
	class _Diff> inline
	void __cdecl _Advance(_BI& _Where, _Diff _Off, bidirectional_iterator_tag)
	{	
	for (; 0 < _Off; --_Off)
		++_Where;
	for (; _Off < 0; ++_Off)
		--_Where;
	}
#pragma warning(pop)

template<class _RI,
	class _Diff> inline
	void __cdecl _Advance(_RI& _Where, _Diff _Off, random_access_iterator_tag)
	{	
	_Where += _Off;
	}

		

template<class _Iter> inline
	typename iterator_traits<_Iter>::difference_type
		* __cdecl _Dist_type(_Iter)
	{	
	return (0);
	}

		
template<class _InIt,
	class _Diff> inline
		void __cdecl _Distance2(_InIt _First, _InIt _Last, _Diff& _Off,
			input_iterator_tag)
	{	
	for (; _First != _Last; ++_First)
		++_Off;
	}

template<class _FwdIt,
	class _Diff> inline
		void __cdecl _Distance2(_FwdIt _First, _FwdIt _Last, _Diff& _Off,
			forward_iterator_tag)
	{	
	for (; _First != _Last; ++_First)
		++_Off;
	}

template<class _BidIt,
	class _Diff> inline
		void __cdecl _Distance2(_BidIt _First, _BidIt _Last, _Diff& _Off,
			bidirectional_iterator_tag)
	{	
	for (; _First != _Last; ++_First)
		++_Off;
	}

template<class _RanIt,
	class _Diff> inline
		void __cdecl _Distance2(_RanIt _First, _RanIt _Last, _Diff& _Off,
			random_access_iterator_tag)
	{	

 





// #line 1849 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	_Off += _Last - _First;
	}

template<class _InIt> inline
	typename iterator_traits<_InIt>::difference_type
		__cdecl distance(_InIt _First, _InIt _Last)
	{	
	typename iterator_traits<_InIt>::difference_type _Off = 0;
	_Distance2(_First, _Last, _Off, _Iter_cat(_First));
	return (_Off);
	}


template<class _InIt,
	class _Diff> inline
		void __cdecl _Distance(_InIt _First, _InIt _Last, _Diff& _Off)
	{	
	_Distance2(_First, _Last, _Off, _Iter_cat(_First));
	}

		
template<class _RanIt,
	class _Base>
	class _Revranit
		: public _Base
	{	
public:
	typedef _Revranit<_RanIt, _Base> _Myt;
	typedef typename iterator_traits<_RanIt>::iterator_category iterator_category;
	typedef typename iterator_traits<_RanIt>::value_type value_type;
	typedef typename iterator_traits<_RanIt>::difference_type difference_type;
	typedef typename iterator_traits<_RanIt>::difference_type distance_type;	
	typedef typename iterator_traits<_RanIt>::pointer pointer;
	typedef typename iterator_traits<_RanIt>::reference reference;
	typedef _RanIt iterator_type;

	 _Revranit()
		{	
		}

	explicit  _Revranit(_RanIt _Right)
		: current(_Right)
		{	
		}

	template<class _RanIt2,
		class _Base2>
		_Revranit(const _Revranit<_RanIt2, _Base2>& _Right)
		: current(_Right.base())
		{	
		}

	_RanIt  base() const
		{	
		return (current);
		}

	reference  operator*() const
		{	
		_RanIt _Tmp = current;
		return (*--_Tmp);
		}

	pointer  operator->() const
		{	
		return (&**this);
		}

	_Myt&  operator++()
		{	
		--current;
		return (*this);
		}

	_Myt  operator++(int)
		{	
		_Myt _Tmp = *this;
		--current;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	
		++current;
		return (*this);
		}

	_Myt  operator--(int)
		{	
		_Myt _Tmp = *this;
		++current;
		return (_Tmp);
		}

	template<class _RanIt2,
		class _Base2>
		bool  _Equal(const _Revranit<_RanIt2, _Base2>& _Right) const
		{	
		return (current == _Right.base());
		}



	_Myt&  operator+=(difference_type _Off)
		{	
		current -= _Off;
		return (*this);
		}

	_Myt  operator+(difference_type _Off) const
		{	
		return (_Myt(current - _Off));
		}

	_Myt&  operator-=(difference_type _Off)
		{	
		current += _Off;
		return (*this);
		}

	_Myt  operator-(difference_type _Off) const
		{	
		return (_Myt(current + _Off));
		}

	reference  operator[](difference_type _Off) const
		{	
		return (*(*this + _Off));
		}

	template<class _RanIt2,
		class _Base2>
		bool  _Less(const _Revranit<_RanIt2, _Base2>& _Right) const
		{	
		return (_Right.base() < current);
		}

	template<class _RanIt2,
		class _Base2>
		difference_type  _Minus(const _Revranit<_RanIt2, _Base2>& _Right) const
		{	
		return (_Right.base() - current);
		}

protected:
	_RanIt current;	
	};

		
template<class _RanIt,
	class _Base,
	class _Diff> inline
	_Revranit<_RanIt, _Base>
		operator+(_Diff _Off,
		const _Revranit<_RanIt, _Base>& _Right)
	{	
	return (_Right + _Off);
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	typename _Base1::difference_type operator-(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (_Left._Minus(_Right));
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator==(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (_Left._Equal(_Right));
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator!=(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (!(_Left == _Right));
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator<(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (_Left._Less(_Right));
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator>(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (_Right < _Left);
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator<=(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (!(_Right < _Left));
	}

template<class _RanIt1,
	class _Base1,
	class _RanIt2,
	class _Base2> inline
	bool operator>=(
		const _Revranit<_RanIt1, _Base1>& _Left,
		const _Revranit<_RanIt2, _Base2>& _Right)
	{	
	return (!(_Left < _Right));
	}

		
template<class _RanIt>
	class reverse_iterator
		: public _Revranit<_RanIt, iterator<
			typename iterator_traits<_RanIt>::iterator_category,
			typename iterator_traits<_RanIt>::value_type,
			typename iterator_traits<_RanIt>::difference_type,
			typename iterator_traits<_RanIt>::pointer,
			typename iterator_traits<_RanIt>::reference> >
	{	
	typedef reverse_iterator<_RanIt> _Myt;
	typedef _Revranit<_RanIt, iterator<
			typename iterator_traits<_RanIt>::iterator_category,
			typename iterator_traits<_RanIt>::value_type,
			typename iterator_traits<_RanIt>::difference_type,
			typename iterator_traits<_RanIt>::pointer,
			typename iterator_traits<_RanIt>::reference> > _Mybase;

public:
	typedef typename iterator_traits<_RanIt>::iterator_category iterator_category;
	typedef typename iterator_traits<_RanIt>::value_type value_type;
	typedef typename iterator_traits<_RanIt>::difference_type difference_type;
	typedef typename iterator_traits<_RanIt>::difference_type distance_type;	
	typedef typename iterator_traits<_RanIt>::pointer pointer;
	typedef typename iterator_traits<_RanIt>::reference reference;
	typedef _RanIt iterator_type;

	 reverse_iterator()
		{	
		}

	explicit  reverse_iterator(_RanIt _Right)
		: _Mybase(_Right)
		{	
		}

	template<class _Other>
		 reverse_iterator(const reverse_iterator<_Other>& _Right)
		: _Mybase(_Right.base())
		{	
		}

	 reverse_iterator(_Mybase _Right)
		: _Mybase(_Right)
		{	
		}


	typedef typename ::std:: _Checked_iterator_category<_RanIt>::_Checked_cat _Checked_iterator_category;
	typedef reverse_iterator<typename _Checked_iterator_base_helper<_RanIt>::_Checked_iterator_base_type> _Checked_iterator_base_type;
 
	_Checked_iterator_base_type _Checked_iterator_base() const
	{
		typename _Checked_iterator_base_type _Base(::std:: _Checked_base(this->current));
		return _Base;
	}

	void _Checked_iterator_assign_from_base(_Checked_iterator_base_type _Base)
	{
		::std:: _Checked_assign_from_base(this->current, _Base.base());
	}
// #line 2148 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	_Myt& operator++()
		{	
		++*((_Mybase *)this);
		return (*this);
		}

	_Myt operator++(int)
		{	
		_Myt _Tmp = *this;
		++*this;
		return (_Tmp);
		}

	_Myt& operator--()
		{	
		--*((_Mybase *)this);
		return (*this);
		}

	_Myt operator--(int)
		{	
		_Myt _Tmp = *this;
		--*this;
		return (_Tmp);
		}

	_Myt& operator+=(difference_type _Off)
		{	
		*((_Mybase *)this) += _Off;
		return (*this);
		}

	_Myt operator+(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp += _Off);
		}

	_Myt& operator-=(difference_type _Off)
		{	
		*((_Mybase *)this) -= _Off;
		return (*this);
		}

	_Myt operator-(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp -= _Off);
		}
	};

		
template<class _RanIt,
	class _Diff> inline
	reverse_iterator<_RanIt> __cdecl operator+(_Diff _Off,
		const reverse_iterator<_RanIt>& _Right)
	{	
	return (_Right + _Off);
	}

template<class _RanIt1,
	class _RanIt2> inline
	typename reverse_iterator<_RanIt1>::difference_type
		__cdecl operator-(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (_Left._Minus(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator==(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (_Left._Equal(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator!=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (!(_Left == _Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator<(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (_Left._Less(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator>(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (_Right < _Left);
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator<=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (!(_Right < _Left));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool __cdecl operator>=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	
	return (!(_Left < _Right));
	}

		
template<class _BidIt,
	class _Ty,
	class _Reference = _Ty&,
	class _Pointer = _Ty *,
	class _Diff = ptrdiff_t>
	class reverse_bidirectional_iterator
		: public iterator<bidirectional_iterator_tag, _Ty, _Diff,
			_Pointer, _Reference>
	{	
public:
	typedef reverse_bidirectional_iterator<_BidIt, _Ty, _Reference,
		_Pointer, _Diff> _Myt;
	typedef _BidIt iterator_type;

	 reverse_bidirectional_iterator()
		{	
		}

	explicit  reverse_bidirectional_iterator(_BidIt _Right)
		: current(_Right)
		{	
		}

	_BidIt  base() const
		{	
		return (current);
		}

	_Reference  operator*() const
		{	
		_BidIt _Tmp = current;
		return (*--_Tmp);
		}

	_Pointer  operator->() const
		{	
		_Reference _Tmp = **this;
		return (&_Tmp);
		}

	_Myt&  operator++()
		{	
		--current;
		return (*this);
		}

	_Myt  operator++(int)
		{	
		_Myt _Tmp = *this;
		--current;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	
		++current;
		return (*this);
		}

	_Myt  operator--(int)
		{	
		_Myt _Tmp = *this;
		++current;
		return (_Tmp);
		}

	bool  operator==(const _Myt& _Right) const
		{	
		return (current == _Right.current);
		}

	bool  operator!=(const _Myt& _Right) const
		{	
		return (!(*this == _Right));
		}

protected:
	_BidIt current;	
	};

		
template<class _BidIt,
	class _BidIt2 = _BidIt>
	class _Revbidit
		: public iterator<
			typename iterator_traits<_BidIt>::iterator_category,
			typename iterator_traits<_BidIt>::value_type,
			typename iterator_traits<_BidIt>::difference_type,
			typename iterator_traits<_BidIt>::pointer,
			typename iterator_traits<_BidIt>::reference>
	{	
public:
	typedef _Revbidit<_BidIt, _BidIt2> _Myt;
	typedef typename iterator_traits<_BidIt>::difference_type _Diff;
	typedef typename iterator_traits<_BidIt>::pointer _Pointer;
	typedef typename iterator_traits<_BidIt>::reference _Reference;
	typedef _BidIt iterator_type;

	 _Revbidit()
		{	
		}

	explicit  _Revbidit(_BidIt _Right)
		: current(_Right)
		{	
		}

	 _Revbidit(const _Revbidit<_BidIt2>& _Other)
		: current (_Other.base())
		{	
		}

	_BidIt  base() const
		{	
		return (current);
		}

	_Reference  operator*() const
		{	
		_BidIt _Tmp = current;
		return (*--_Tmp);
		}

	_Pointer  operator->() const
		{	
		_Reference _Tmp = **this;
		return (&_Tmp);
		}

	_Myt&  operator++()
		{	
		--current;
		return (*this);
		}

	_Myt  operator++(int)
		{	
		_Myt _Tmp = *this;
		--current;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	
		++current;
		return (*this);
		}

	_Myt  operator--(int)
		{	
		_Myt _Tmp = *this;
		++current;
		return (_Tmp);
		}

	bool  operator==(const _Myt& _Right) const
		{	
		return (current == _Right.current);
		}

	bool  operator!=(const _Myt& _Right) const
		{	
		return (!(*this == _Right));
		}

protected:
	_BidIt current;
	};



		
template<class _InIt, class _OutIt, class _InOutItCat>
inline
	_OutIt __cdecl _Copy_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		_InOutItCat, _Nonscalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	
	;
	for (; _First != _Last; ++_Dest, ++_First)
		*_Dest = *_First;
	return (_Dest);
	}


template<class _InIt, class _OutIt>
inline
	_OutIt __cdecl _Copy_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		random_access_iterator_tag, _Nonscalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	
	
	_OutIt _Result = _Dest + (_Last - _First);
	_Copy_opt(_First, _Last, ::std:: _Checked_base(_Dest), 
		forward_iterator_tag(), _Nonscalar_ptr_iterator_tag(), _Range_checked_iterator_tag());
	return _Result;
	}
// #line 2463 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

template<class _InIt, class _OutIt, class _InOutItCat>
inline
	_OutIt __cdecl _Copy_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		_InOutItCat, _Scalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	

 



// #line 2475 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	ptrdiff_t _Off = _Last - _First;	
	
	_OutIt _Result = _Dest + _Off;
	if (_Off > 0)
		::memmove_s((&*_Dest), (_Off * sizeof (*_First)), (&*_First), (_Off * sizeof (*_First)));
	return _Result;
	}

template <bool _Cond, class _Result_type>
struct _Enable_if;

template <class _Result_type>
struct _Enable_if<true, _Result_type>
{
	typedef _Result_type _Result;
};

template <class _Checked_iter_cat>
struct _Is_checked_iterator_helper
{
	enum { _Result = false };
};

template <>
struct _Is_checked_iterator_helper<_Range_checked_iterator_tag>
{
	enum { _Result = true };
};

template <class _Iter>
struct _Is_checked_iterator
{
	enum { _Result = 
		_Is_checked_iterator_helper<typename _Checked_iterator_category<_Iter>::_Checked_cat>::_Result };
};





















template<class _InIt, class _OutIt>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return (_Copy_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest, 
		_Iter_random(_First, _Dest), _Ptr_cat(_First, _Dest), _Range_checked_iterator_tag()));
	}

template<class _InIt, class _OutElem, size_t _Size>
inline
_OutElem* __cdecl copy(_InIt _First, _InIt _Last, _OutElem (&_Dest)[_Size])
	{	
	return copy(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), ::stdext:: make_checked_array_iterator(_Dest, _Size)).base();
	}

template<class _InIt, class _OutIt>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return (_Copy_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest, 
		_Iter_random(_First, _Dest), _Ptr_cat(_First, _Dest), _Range_checked_iterator_tag()));
	}











// #line 2567 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt, class _OutIt, class _InOutItCat, class _MoveCatTy>
inline
	_OutIt __cdecl _Move_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		_InOutItCat _First_dest_cat, _MoveCatTy, _Range_checked_iterator_tag)
	{	
	return _Copy_opt(_First, _Last, _Dest,
		_First_dest_cat, _Ptr_cat(_First, _Dest), _Range_checked_iterator_tag());
	}

template<class _InIt, class _OutIt, class _InOutItCat>
inline
	_OutIt __cdecl _Move_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		_InOutItCat, _Swap_move_tag, _Range_checked_iterator_tag)
	{	
	;
	for (; _First != _Last; ++_Dest, ++_First)
		::std:: swap(*_Dest, *_First);
	return (_Dest);
	}


template<class _InIt, class _OutIt, class _InOutItCat>
inline
	_OutIt __cdecl _Move_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		random_access_iterator_tag, _Swap_move_tag _Move_cat, _Range_checked_iterator_tag)
	{	
	
	_OutIt _Result = _Dest + (_Last - _First);
	_Move_opt(_First, _Last, ::std:: _Checked_base(_Dest),
		forward_iterator_tag(), _Move_cat, _Range_checked_iterator_tag());
	return _Result;
	}
// #line 2602 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _InIt, class _OutIt>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl _Move(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return _Move_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt, class _OutElem, size_t _Size>
inline
_OutElem* __cdecl _Move(_InIt _First, _InIt _Last, _OutElem (&_Dest)[_Size])
	{	
	return _Move(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last),
		::stdext:: make_checked_array_iterator(_Dest, _Size)).base();
	}

template<class _InIt, class _OutIt>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl _Move(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return _Move_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}











// #line 2641 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _BidIt1, class _BidIt2, class _InOutItCat>
inline
	_BidIt2 __cdecl _Copy_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest,
		_InOutItCat, _Nonscalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	
	;
	while (_First != _Last)
		*--_Dest = *--_Last;
	return (_Dest);
	}


template<class _InIt, class _OutIt>
inline
	_OutIt __cdecl _Copy_backward_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		random_access_iterator_tag, _Nonscalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	
	
	_OutIt _Result = _Dest - (_Last - _First);
	_Copy_backward_opt(_First, _Last, ::std:: _Checked_base(_Dest),
		forward_iterator_tag(), _Nonscalar_ptr_iterator_tag(), _Range_checked_iterator_tag());
	return _Result;
	}
// #line 2667 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

template<class _InIt, class _OutIt, class _InOutItCat>
inline
	_OutIt __cdecl _Copy_backward_opt(_InIt _First, _InIt _Last, _OutIt _Dest,
		_InOutItCat, _Scalar_ptr_iterator_tag, _Range_checked_iterator_tag)
	{	

 



// #line 2679 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	ptrdiff_t _Off = _Last - _First;	
	


	_OutIt _Result = _Dest - _Off;
	if (_Off > 0)
		::memmove_s((&*_Result), (_Off * sizeof (*_First)), (&*_First), (_Off * sizeof (*_First)));
	return _Result;
	}



template<class _BidIt1,
	class _BidIt2> inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return _Copy_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _BidIt1,
	class _BidIt2> inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return _Copy_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag());
	}











// #line 2720 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _BidIt1, class _BidIt2, class _InOutItCat, class _MoveCatTy>
inline
	_BidIt2 __cdecl _Move_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest,
		_InOutItCat _First_dest_cat, _MoveCatTy, _Range_checked_iterator_tag)
	{	
	return _Copy_backward_opt(_First, _Last, _Dest,
		_First_dest_cat, _Ptr_cat(_First, _Dest), _Range_checked_iterator_tag());
	}

template<class _BidIt1, class _BidIt2, class _InOutItCat>
inline
	_BidIt2 __cdecl _Move_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest,
		_InOutItCat, _Swap_move_tag, _Range_checked_iterator_tag)
	{	
	;
	while (_First != _Last)
		::std:: swap(*--_Dest, *--_Last);
	return (_Dest);
	}


template<class _BidIt1, class _BidIt2>
inline
	_BidIt2 __cdecl _Move_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest,
		random_access_iterator_tag, _Swap_move_tag _Move_cat, _Range_checked_iterator_tag)
	{	
	
	_BidIt2 _Result = _Dest - (_Last - _First);
	_Move_backward_opt(_First, _Last, ::std:: _Checked_base(_Dest),
		forward_iterator_tag(), _Move_cat, _Range_checked_iterator_tag());
	return _Result;
	}
// #line 2755 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _BidIt1, class _BidIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl _Move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return _Move_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _BidIt1, class _BidIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl _Move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return _Move_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		_Iter_random(_First, _Dest), _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}











// #line 2786 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1, class _InIt2, class _InItCats>
inline
	pair<_InIt1, _InIt2>
		__cdecl _Mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2,
			_InItCats, _Range_checked_iterator_tag)
	{	

 



// #line 2800 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	for (; _First1 != _Last1 && *_First1 == *_First2; )
		++_First1, ++_First2;
	return (pair<_InIt1, _InIt2>(_First1, _First2));
	}


template<class _InIt1, class _InIt2>
inline
	pair<_InIt1, _InIt2>
		__cdecl _Mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2,
			random_access_iterator_tag, _Range_checked_iterator_tag)
	{	
	
	_InIt2 _Last2 = _First2 + (_Last1 - _First1); (_Last2);
	pair<_InIt1, typename ::std:: _Checked_iterator_base_helper<_InIt2>::_Checked_iterator_base_type> _Result =
		_Mismatch(_First1, _Last1, ::std:: _Checked_base(_First2),
			forward_iterator_tag(), _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First2, _Result.second);
	return (pair<_InIt1, _InIt2>(_Result.first, _First2));
	}
// #line 2822 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _InIt1, class _InIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		_Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
			_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InElem2, size_t _Size>
inline
pair<_InIt1, _InElem2*>
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size])
	{	
	pair<_InIt1, ::stdext:: checked_array_iterator<_InElem2*> > _Result =
		mismatch(_First1, _Last1, ::stdext:: make_checked_array_iterator(_First2, _Size));
	return (pair<_InIt1, _InElem2*>(_Result.first, _Result.second.base()));
	}

template<class _InIt1, class _InIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		_Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
			_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (pair<_InIt1, _InIt2>(_First1, _Result.second));
	}












// #line 2872 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1, class _InIt2, class _Pr, class _InItCats>
inline
	pair<_InIt1, _InIt2>
		__cdecl _Mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred,
			_InItCats, _Range_checked_iterator_tag)
	{	

 




// #line 2887 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	for (; _First1 != _Last1 && _Pred(*_First1, *_First2); )
		++_First1, ++_First2;
	return (pair<_InIt1, _InIt2>(_First1, _First2));
	}


template<class _InIt1, class _InIt2, class _Pr>
inline
	pair<_InIt1, _InIt2>
		__cdecl _Mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred,
			random_access_iterator_tag, _Range_checked_iterator_tag)
	{	
	
	_InIt2 _Last2 = _First2 + (_Last1 - _First1); (_Last2);
	pair<_InIt1, typename ::std:: _Checked_iterator_base_helper<_InIt2>::_Checked_iterator_base_type> _Result =
		_Mismatch(_First1, _Last1, ::std:: _Checked_base(_First2), _Pred,
			forward_iterator_tag(), _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First2, _Result.second);
	return (pair<_InIt1, _InIt2>(_Result.first, _First2));
	}
// #line 2909 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _InIt1, class _InIt2, class _Pr>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		_Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
			_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InElem2, class _Pr, size_t _Size>
inline
pair<_InIt1, _InElem2*>
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size], _Pr _Pred)
	{	
	pair<_InIt1, ::stdext:: checked_array_iterator<_InElem2*> > _Result =
		mismatch(_First1, _Last1, ::stdext:: make_checked_array_iterator(_First2, _Size), _Pred);
	return (pair<_InIt1, _InElem2*>(_Result.first, _Result.second.base()));
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		_Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
			_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (pair<_InIt1, _InIt2>(_First1, _Result.second));
	}












// #line 2959 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1, class _InIt2, class _InItCats>
inline
	bool __cdecl _Equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2,
		_InItCats, _Range_checked_iterator_tag)
	{	
	for (; _First1 != _Last1; ++_First1, ++_First2)
		if (!(*_First1 == *_First2))
			return (false);
	return (true);
	}

inline bool __cdecl _Equal(const char *_First1,
	const char *_Last1, const char *_First2,
	random_access_iterator_tag, _Range_checked_iterator_tag)
	{	
 



// #line 2981 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	return (::memcmp(_First1, _First2, _Last1 - _First1) == 0);
	}

inline bool __cdecl _Equal(const signed char *_First1,
	const signed char *_Last1, const signed char *_First2,
	random_access_iterator_tag, _Range_checked_iterator_tag)
	{	
 



// #line 2994 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	return (::memcmp(_First1, _First2, _Last1 - _First1) == 0);
	}

inline bool __cdecl _Equal(const unsigned char *_First1,
	const unsigned char *_Last1, const unsigned char *_First2,
	random_access_iterator_tag, _Range_checked_iterator_tag)
	{	
 



// #line 3007 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	return (::memcmp(_First1, _First2, _Last1 - _First1) == 0);
	}


template<class _InIt1, class _InIt2>
inline
	bool __cdecl _Equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2,
		random_access_iterator_tag, _Range_checked_iterator_tag)
	{
	
	_InIt2 _Last2 = _First2 + (_Last1 - _First1); (_Last2);
	return _Equal(_First1, _Last1, ::std:: _Checked_base(_First2),
		forward_iterator_tag(), _Range_checked_iterator_tag());
	}
// #line 3023 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _InIt1, class _InIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	return _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
		_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InElem2, size_t _Size>
inline
bool __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size])
	{	
	return equal(_First1, _Last1,
		::stdext:: make_checked_array_iterator(_First2, _Size));
	}

template<class _InIt1, class _InIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	return _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
		_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}











// #line 3062 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1, class _InIt2, class _Pr, class _InItCats>
inline
	bool __cdecl _Equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred,
		_InItCats, _Range_checked_iterator_tag)
	{	
	for (; _First1 != _Last1; ++_First1, ++_First2)
		if (!_Pred(*_First1, *_First2))
			return (false);
	return (true);
	}


template<class _InIt1, class _InIt2, class _Pr>
inline
	bool __cdecl _Equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred,
		random_access_iterator_tag, _Range_checked_iterator_tag)
	{
	
	_InIt2 _Last2 = _First2 + (_Last1 - _First1); (_Last2);
	return _Equal(_First1, _Last1, ::std:: _Checked_base(_First2), _Pred,
		forward_iterator_tag(), _Range_checked_iterator_tag());
	}
// #line 3087 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _InIt1, class _InIt2, class _Pr>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	return _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
		_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InElem2, class _Pr, size_t _Size>
inline
bool __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size], _Pr _Pred)
	{	
	return equal(_First1, _Last1,
		::stdext:: make_checked_array_iterator(_First2, _Size), _Pred);
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	return _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
		_Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}











// #line 3126 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _FwdIt, class _Ty> inline
	void __cdecl _Fill(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	
	;
	for (; _First != _Last; ++_First)
		*_First = _Val;
	}

inline void __cdecl _Fill([SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_First, 
	[SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char *_Last, int _Val)
	{	
	;
	::memset(_First, _Val, _Last - _First);
	}

inline void __cdecl _Fill([SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] signed char *_First, 
	[SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] signed char *_Last, int _Val)
	{	
	;
	::memset(_First, _Val, _Last - _First);
	}

inline void __cdecl _Fill(
	[SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] unsigned char *_First,
	[SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] unsigned char *_Last, int _Val)
	{	
	;
	::memset(_First, _Val, _Last - _First);
	}

template<class _FwdIt, class _Ty> inline
	void __cdecl fill(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	
	_Fill(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Val);
	}

		
template<class _OutIt,
	class _Diff,
	class _Ty> inline
	void __cdecl _Fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val,
		_Range_checked_iterator_tag)
	{	
	for (; 0 < _Count; --_Count, ++_First)
		*_First = _Val;
	}

inline void __cdecl _Fill_n([SA_Pre(Null=SA_Maybe,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_Count")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_First, 
		size_t _Count, int _Val, _Range_checked_iterator_tag)
	{	

 


// #line 3183 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	::memset(_First, _Val, _Count);
	}

inline void __cdecl _Fill_n([SA_Pre(Null=SA_Maybe,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_Count")] [SA_Post(Deref=1,Valid=SA_Yes)] signed char *_First, 
		size_t _Count, int _Val, _Range_checked_iterator_tag)
	{	

 


// #line 3195 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	::memset(_First, _Val, _Count);
	}

inline void __cdecl _Fill_n([SA_Pre(Null=SA_Maybe,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="_Count")] [SA_Post(Deref=1,Valid=SA_Yes)] unsigned char *_First, 
		size_t _Count, int _Val, _Range_checked_iterator_tag)
	{	

 


// #line 3207 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

	::memset(_First, _Val, _Count);
	}

template<class _OutIt, class _Diff, class _Ty, class _OutCat>
inline
	void __cdecl _Fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val,
		_OutCat, _Range_checked_iterator_tag)
	{
		_Fill_n(_First, _Count, _Val, _Range_checked_iterator_tag());
	}


template<class _OutIt, class _Diff, class _Ty>
inline
	void __cdecl _Fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val,
		random_access_iterator_tag, _Range_checked_iterator_tag)
	{
		
		_OutIt _Last = _First + _Count; (_Last);
		_Fill_n(::std:: _Checked_base(_First), _Count, _Val,
			_Range_checked_iterator_tag());
	}
// #line 3231 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



template<class _OutIt,
	class _Diff,
	class _Ty>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, void >::_Result __cdecl fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val)
	{	
		_Fill_n(_First, _Count, _Val, _Iter_cat(_First), ::std:: _Range_checked_iterator_tag());
	}

template<class _OutElem,
	class _Diff,
	class _Ty, size_t _Size>
inline
void __cdecl fill_n(_OutElem (&_First)[_Size], _Diff _Count, const _Ty& _Val)
	{	
		fill_n(::stdext:: make_checked_array_iterator(_First, _Size), _Count, _Val);
	}

template<class _OutIt,
	class _Diff,
	class _Ty>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, void >::_Result __cdecl fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val)
	{	
		_Fill_n(_First, _Count, _Val, _Iter_cat(_First), ::std:: _Range_checked_iterator_tag());
	}











// #line 3273 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1, class _InIt2> inline
	bool __cdecl _Lexicographical_compare(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2)
	{	
	;
	;
	for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2)
		if (((*_First1) < (*_First2)))
			return (true);
		else if (*_First2 < *_First1)
			return (false);
	return (_First1 == _Last1 && _First2 != _Last2);
	}

template<class _InIt1, class _InIt2> inline
	bool __cdecl lexicographical_compare(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2)
	{	
	return _Lexicographical_compare(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1),
		::std:: _Checked_base(_First2), ::std:: _Checked_base(_Last2));
	}

inline bool __cdecl lexicographical_compare(
	const unsigned char *_First1, const unsigned char *_Last1,
	const unsigned char *_First2, const unsigned char *_Last2)
	{	
	;
	;
	ptrdiff_t _Num1 = _Last1 - _First1;
	ptrdiff_t _Num2 = _Last2 - _First2;
	int _Ans = ::memcmp(_First1, _First2, _Num1 < _Num2 ? _Num1 : _Num2);
	return (_Ans < 0 || _Ans == 0 && _Num1 < _Num2);
	}

 











// #line 3322 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _InIt1,
	class _InIt2,
	class _Pr> inline
	bool __cdecl _Lexicographical_compare(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _Pr _Pred)
	{	
	;
	;
	;
	for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2)
		if (_Pred(*_First1, *_First2))
			return (true);
		else if (_Pred(*_First2, *_First1))
			return (false);
	return (_First1 == _Last1 && _First2 != _Last2);
	}

template<class _InIt1, class _InIt2, class _Pr> inline
	bool __cdecl lexicographical_compare(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2, _Pr _Pred)
	{	
	return _Lexicographical_compare(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1),
		::std:: _Checked_base(_First2), ::std:: _Checked_base(_Last2), _Pred);
	}

 
  
  
 // #line 3353 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

		
template<class _Ty> inline
	const _Ty& (__cdecl max)(const _Ty& _Left, const _Ty& _Right)
	{	
	return (((_Left) < (_Right)) ? _Right : _Left);
	}

		
template<class _Ty,
	class _Pr> inline
	const _Ty& (__cdecl max)(const _Ty& _Left, const _Ty& _Right, _Pr _Pred)
	{	
	return (_Pred(_Left, _Right) ? _Right : _Left);
	}

		
template<class _Ty> inline
	const _Ty& (__cdecl min)(const _Ty& _Left, const _Ty& _Right)
	{	
	return (((_Right) < (_Left)) ? _Right : _Left);
	}

		
template<class _Ty,
	class _Pr> inline
	const _Ty& (__cdecl min)(const _Ty& _Left, const _Ty& _Right, _Pr _Pred)
	{	
	return (_Pred(_Right, _Left) ? _Right : _Left);
	}

}

namespace stdext {


template <class _Iterator>
	class checked_array_iterator
		: public ::std:: iterator<
			typename ::std:: iterator_traits<_Iterator>::iterator_category, 
			typename ::std:: iterator_traits<_Iterator>::value_type, 
			typename ::std:: iterator_traits<_Iterator>::difference_type, 
			typename ::std:: iterator_traits<_Iterator>::pointer, 
			typename ::std:: iterator_traits<_Iterator>::reference>
	{
public:
	typedef checked_array_iterator<_Iterator> _Myt;
	typedef typename ::std:: iterator_traits<_Iterator>::difference_type difference_type;
	typedef typename ::std:: iterator_traits<_Iterator>::pointer pointer;
	typedef typename ::std:: iterator_traits<_Iterator>::reference reference;

	typedef ::std:: _Range_checked_iterator_tag _Checked_iterator_category;
	typedef _Iterator _Inner_type;

	typedef _Iterator _Checked_iterator_base_type;
 
	_Checked_iterator_base_type _Checked_iterator_base() const
	{
		return _Mycont + _Current;
	}

	void _Checked_iterator_assign_from_base(_Checked_iterator_base_type _Base)
	{
		_Current = _Base - _Mycont;
	}

	

	checked_array_iterator():
		_Size(0), _Current(0)
	{
	}

	checked_array_iterator(_Iterator _Cont, size_t _S, size_t _Index = 0)
	{
		{ if (!(_Index <= _S)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		_Mycont = _Cont;
		_Size = _S;
		_Current =_Index;
	}

	_Iterator base() const
	{
		return _Mycont + _Current;
	}

	size_t __Size() const
	{
		return _Size;
	}

	bool operator==(const _Myt& _Right) const
	{
		{ if (!(_Mycont == _Right._Mycont)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		return _Current == _Right._Current;
	}

	bool operator!=(const _Myt& _Right) const
	{
		return !(*this == _Right);
	}

	bool operator<(const _Myt& _Right) const
	{
		{ if (!(_Mycont == _Right._Mycont)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		return _Current < _Right._Current;
	}

	bool operator>(const _Myt& _Right) const
	{
		return _Right < *this;
	}

	bool operator<=(const _Myt& _Right) const
	{
		return !(_Right < *this);
	}

	bool operator>=(const _Myt& _Right) const
	{
		return !(*this < _Right);
	}

	reference operator*() const
	{
		{ if (!(_Current < _Size)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		return *(_Mycont + _Current);
	}

	pointer operator->() const
	{
		return (&**this);
	}

	checked_array_iterator& operator++()
	{
		{ if (!(_Current < _Size)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		++_Current;
		return *this;
	}

	_Myt operator++(int)
	{
		checked_array_iterator _Tmp = *this;
		++*this;
		return _Tmp;
	}

	_Myt& operator--()
	{
		{ if (!(_Current > 0)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		--_Current;
		return *this;
	}

	_Myt operator--(int)
	{
		checked_array_iterator _Tmp = *this;
		--*this;
		return _Tmp;
	}

	

	_Myt& operator+=(difference_type _Off)
	{
		{ if (!(_Current + _Off <= _Size && _Current + _Off >= 0)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		_Current += _Off;
		return *this;
	}

	_Myt operator+(difference_type _Off) const
	{
		checked_array_iterator _Tmp = *this;
		return (_Tmp += _Off);
	}

	_Myt& operator-=(difference_type _Off)
	{
		return (*this += -_Off);
	}

	_Myt operator-(difference_type _Off) const
	{
		checked_array_iterator _Tmp = *this;
		return (_Tmp -= _Off);
	}

	difference_type operator-(const checked_array_iterator& _Right) const
	{
		{ if (!(_Mycont == _Right._Mycont)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		return _Current - _Right._Current;
	}

	reference operator[](difference_type _Off) const
	{
		{ if (!(_Current + _Off < _Size && _Current + _Off >= 0)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		return *(_Mycont + _Current + _Off);
	}

protected:
	void _Xran() const
	{	
		throw ::std:: out_of_range("invalid checked_array_iterator<T> subscript");
	}

	void _Xinvarg() const
	{	
		throw ::std:: invalid_argument("invalid checked_array_iterator<T> argument");
	}

	_Iterator _Mycont; 
	size_t _Current; 
	size_t _Size; 
	};

template <class _Iter>
checked_array_iterator<_Iter> make_checked_array_iterator(_Iter _Ptr, size_t _Size)
{
	return checked_array_iterator<_Iter>(_Ptr, _Size);
}

template<class _InIt,
	class _OutIt> inline
	_OutIt __cdecl unchecked_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
		return (::std:: _Copy_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
			::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _InIt, class _OutIt>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl checked_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return (::std:: _Copy_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest, 
		::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _InIt, class _OutElem, size_t _Size>
inline
_OutElem* __cdecl checked_copy(_InIt _First, _InIt _Last, _OutElem (&_Dest)[_Size])
	{	
	return checked_copy(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), ::stdext:: make_checked_array_iterator(_Dest, _Size)).base();
	}

template<class _InIt, class _OutIt>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl checked_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return (::std:: _Copy_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest, 
		::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _BidIt1,
	class _BidIt2> inline
	_BidIt2 __cdecl unchecked_copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
		return (::std:: _Copy_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
			::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _BidIt1,
	class _BidIt2> inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl checked_copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return ::std:: _Copy_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _BidIt1,
	class _BidIt2> inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl checked_copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return ::std:: _Copy_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Ptr_cat(_First, _Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt,
	class _OutIt> inline
	_OutIt __cdecl _Unchecked_move(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
		return (::std:: _Move_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
			::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _InIt, class _OutIt>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl _Checked_move(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return ::std:: _Move_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt, class _OutElem, size_t _Size>
inline
_OutElem* __cdecl _Checked_move(_InIt _First, _InIt _Last, _OutElem (&_Dest)[_Size])
	{	
	return _Checked_move(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last),
		::stdext:: make_checked_array_iterator(_Dest, _Size)).base();
	}

template<class _InIt, class _OutIt>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, _OutIt >::_Result __cdecl _Checked_move(_InIt _First, _InIt _Last, _OutIt _Dest)
	{	
	return ::std:: _Move_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _BidIt1,
	class _BidIt2> inline
	_BidIt2 __cdecl _Unchecked_move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
		return (::std:: _Move_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
			::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag()));
	}

template<class _BidIt1, class _BidIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl _Checked_move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return ::std:: _Move_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _BidIt1, class _BidIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _BidIt2 >::_Result, _BidIt2 >::_Result __cdecl _Checked_move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
	{	
	return ::std:: _Move_backward_opt(::std:: _Checked_base(_First), ::std:: _Checked_base(_Last), _Dest,
		::std:: _Iter_random(_First, _Dest), ::std:: _Move_cat(_Dest), ::std:: _Range_checked_iterator_tag());
	}

template<class _OutIt,
	class _Diff,
	class _Ty> inline
	void __cdecl unchecked_fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val)
	{	
		::std:: _Fill_n(_First, _Count, _Val, ::std:: _Iter_cat(_First), ::std:: _Range_checked_iterator_tag());
	}

template<class _OutIt,
	class _Diff,
	class _Ty>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _OutIt >::_Result, void >::_Result __cdecl checked_fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val)
	{	
		_Fill_n(_First, _Count, _Val, ::std:: _Iter_cat(_First), ::std:: _Range_checked_iterator_tag());
	}

template<class _OutElem,
	class _Diff,
	class _Ty, size_t _Size>
inline
void __cdecl checked_fill_n(_OutElem (&_First)[_Size], _Diff _Count, const _Ty& _Val)
	{	
		checked_fill_n(::stdext:: make_checked_array_iterator( _First, _Size), _Count, _Val);
	}

template<class _OutIt,
	class _Diff,
	class _Ty>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _OutIt >::_Result, void >::_Result __cdecl checked_fill_n(_OutIt _First, _Diff _Count, const _Ty& _Val)
	{	
		_Fill_n(_First, _Count, _Val, ::std:: _Iter_cat(_First), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InIt2>
inline
	::std:: pair<_InIt1, _InIt2>
		__cdecl unchecked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	

	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));



// #line 3741 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
	}

template<class _InIt1, class _InIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InElem2, size_t _Size>
inline
::std:: pair<_InIt1, _InElem2*>
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size])
	{	
	::std:: pair<_InIt1, ::stdext:: checked_array_iterator<_InElem2*> > _Result =
		checked_mismatch(_First1, _Last1, ::stdext:: make_checked_array_iterator(_First2, _Size));
	return (::std:: pair<_InIt1, _InElem2*>(_Result.first, _Result.second.base()));
	}

template<class _InIt1, class _InIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
	::std:: pair<_InIt1, _InIt2>
		__cdecl unchecked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	

	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));



// #line 3793 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InElem2, class _Pr, size_t _Size>
inline
::std:: pair<_InIt1, _InElem2*>
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size], _Pr _Pred)
	{	
	::std:: pair<_InIt1, ::stdext:: checked_array_iterator<_InElem2*> > _Result =
		checked_mismatch(_First1, _Last1, ::stdext:: make_checked_array_iterator(_First2, _Size), _Pred);
	return (::std:: pair<_InIt1, _InElem2*>(_Result.first, _Result.second.base()));
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, ::std:: pair< _InIt1, _InIt2 > >::_Result
		__cdecl checked_mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	::std:: pair<typename ::std:: _Checked_iterator_base_helper<_InIt1>::_Checked_iterator_base_type, _InIt2> _Result =
		::std:: _Mismatch(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
			::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	::std:: _Checked_assign_from_base(_First1, _Result.first);
	return (::std:: pair<_InIt1, _InIt2>(_First1, _Result.second));
	}

template<class _InIt1, class _InIt2>
inline
	bool __cdecl unchecked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InIt2>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InElem2, size_t _Size>
inline
bool __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size])
	{	
	return checked_equal(_First1, _Last1,
		::stdext:: make_checked_array_iterator(_First2, _Size));
	}

template<class _InIt1, class _InIt2>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
	bool __cdecl unchecked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
typename ::std:: _Enable_if< ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

template<class _InIt1, class _InElem2, class _Pr, size_t _Size>
inline
bool __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InElem2 (&_First2)[_Size], _Pr _Pred)
	{	
	return checked_equal(_First1, _Last1,
		::stdext:: make_checked_array_iterator(_First2, _Size), _Pred);
	}

template<class _InIt1, class _InIt2, class _Pr>
inline
__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
typename ::std:: _Enable_if< ! ::std:: _Is_checked_iterator< _InIt2 >::_Result, bool >::_Result __cdecl checked_equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Pr _Pred)
	{	
	return ::std:: _Equal(::std:: _Checked_base(_First1), ::std:: _Checked_base(_Last1), _First2, _Pred,
		::std:: _Iter_random(_First1, _First2), ::std:: _Range_checked_iterator_tag());
	}

}



 
 

 
 

 





// #line 3913 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

 








 




 





















extern "C"
 void __cdecl _invalid_parameter_noinfo(void);

// #line 3954 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"



 #pragma warning(pop)
 #pragma pack(pop)
// #line 3960 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"

// #line 3962 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"
// #line 3963 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xutility"























// #line 10 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"


 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable: 4100)
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"


 
 
 
// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"

 

 

 

 


namespace std {
		
template<class _Ty> inline
	_Ty  *_Allocate(size_t _Count, _Ty  *)
	{	
	if (_Count <= 0)
		_Count = 0;
	else if (((size_t)(-1) / _Count) < sizeof (_Ty))
		throw std::bad_alloc(0);

		
	return ((_Ty  *)::operator new(_Count * sizeof (_Ty)));
	}

		
template<class _T1,
	class _T2> inline
	void _Construct(_T1  *_Ptr, const _T2& _Val)
	{	
	void  *_Vptr = _Ptr;
	::new (_Vptr) _T1(_Val);
	}

		
template<class _Ty> inline
	void _Destroy(_Ty  *_Ptr)
	{	
	(_Ptr)->~_Ty();
	}

template<> inline
	void _Destroy(char  *)
	{	
	}

template<> inline
	void _Destroy(wchar_t  *)
	{	
	}


		
template<class _Ty>
	struct _Allocator_base
	{	
	typedef _Ty value_type;
	};

		
template<class _Ty>
	struct _Allocator_base<const _Ty>
	{	
	typedef _Ty value_type;
	};

		
template<class _Ty>
	class allocator
		: public _Allocator_base<_Ty>
	{	
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type  *pointer;
	typedef value_type & reference;
	typedef const value_type  *const_pointer;
	typedef const value_type & const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class _Other>
		struct rebind
		{	
		typedef allocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	
		return (&_Val);
		}

	allocator() throw ()
		{	
		}

	allocator(const allocator<_Ty>&) throw ()
		{	
		}

	template<class _Other>
		allocator(const allocator<_Other>&) throw ()
		{	
		}

	template<class _Other>
		allocator<_Ty>& operator=(const allocator<_Other>&)
		{	
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	
		::operator delete(_Ptr);
		}

	pointer allocate(size_type _Count)
		{	
		return (_Allocate(_Count, (pointer)0));
		}

	pointer allocate(size_type _Count, const void  *)
		{	
		return (allocate(_Count));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	
		_Destroy(_Ptr);
		}

	size_t max_size() const throw ()
		{	
		size_t _Count = (size_t)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		
template<class _Ty,
	class _Other> inline
	bool operator==(const allocator<_Ty>&, const allocator<_Other>&) throw ()
	{	
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const allocator<_Ty>&, const allocator<_Other>&) throw ()
	{	
	return (false);
	}

		
template<> class  allocator<void>
	{	
public:
	typedef void _Ty;
	typedef _Ty  *pointer;
	typedef const _Ty  *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	
		typedef allocator<_Other> other;
		};

	allocator() throw ()
		{	
		}

	allocator(const allocator<_Ty>&) throw ()
		{	
		}

	template<class _Other>
		allocator(const allocator<_Other>&) throw ()
		{	
		}

	template<class _Other>
		allocator<_Ty>& operator=(const allocator<_Other>&)
		{	
		return (*this);
		}
	};

		
template<class _Alloc> inline
	void _Destroy_range(typename _Alloc::pointer _First,
		typename _Alloc::pointer _Last, _Alloc& _Al)
	{	
	_Destroy_range(_First, _Last, _Al, _Ptr_cat(_First, _Last));
	}

template<class _Alloc> inline
	void _Destroy_range(typename _Alloc::pointer _First,
		typename _Alloc::pointer _Last, _Alloc& _Al,
		_Nonscalar_ptr_iterator_tag)
	{	
	for (; _First != _Last; ++_First)
		_Al.destroy(_First);
	}

template<class _Alloc> inline
	void _Destroy_range(typename _Alloc::pointer _First,
		typename _Alloc::pointer _Last, _Alloc& _Al,
		_Scalar_ptr_iterator_tag)
	{	
	}
}


 #pragma warning(default: 4100)
 #pragma warning(pop)
 #pragma pack(pop)
// #line 250 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"

// #line 252 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"
// #line 253 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xmemory"






















// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"


 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable:4412)
// #line 13 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

 
 

namespace std {

  #pragma warning(disable:4251)

template<class _Elem,
	class _Traits = char_traits<_Elem>,
	class _Ax = allocator<_Elem> >
	class basic_string;

		
template<class _Elem,
	class _Traits,
	class _Alloc>
	class _String_const_iterator
		: public _Ranit_base<_Elem, typename _Alloc::difference_type,
			typename _Alloc::const_pointer, typename _Alloc::const_reference, _Iterator_base_secure>
	{	
public:
	typedef _String_const_iterator<_Elem, _Traits, _Alloc> _Myt;
	typedef basic_string<_Elem, _Traits, _Alloc> _Mystring;

	typedef random_access_iterator_tag iterator_category;
	typedef _Elem value_type;
	typedef typename _Alloc::difference_type difference_type;
	typedef typename _Alloc::const_pointer pointer;
	typedef typename _Alloc::const_reference reference;
	typedef _Elem* _Inner_type;


	typedef _Range_checked_iterator_tag _Checked_iterator_category;
// #line 48 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"


	typedef pointer _Checked_iterator_base_type;

	_Checked_iterator_base_type _Checked_iterator_base() const
	{
		return _Myptr;
	}

	void _Checked_iterator_assign_from_base(_Checked_iterator_base_type _Base)
	{
		this->_Myptr = _Base;
	}
// #line 62 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	 _String_const_iterator()
		{	
		_Myptr = 0;
		}

 












// #line 82 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
 
		 _String_const_iterator(pointer _Ptr, const _Container_base_secure *_Pstring)
			{	
			{ if (!(_Pstring != 0 && _Ptr != 0 && ((_Mystring *)_Pstring)->_Myptr() <= _Ptr && _Ptr <= (((_Mystring *)_Pstring)->_Myptr() + ((_Mystring *)_Pstring)->_Mysize))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
// #line 89 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
			this->_Mycont = _Pstring;
			_Myptr = _Ptr;
			}

 







// #line 102 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	reference  operator*() const
		{	

 









// #line 117 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
			{ if (!(_Myptr < (((_Mystring *)this->_Mycont)->_Myptr() + ((_Mystring *)(this->_Mycont))->_Mysize))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
 // #line 123 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (*_Myptr);
		}

	pointer  operator->() const
		{	
		return (&**this);
		}

	_Myt&  operator++()
		{	
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
			{ if (!(_Myptr < (((_Mystring *)this->_Mycont)->_Myptr() + ((_Mystring *)this->_Mycont)->_Mysize))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
		++_Myptr;
		return (*this);
		}

	_Myt  operator++(int)
		{	
		_Myt _Tmp = *this;
		++*this;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
			{ if (!(_Myptr > ((_Mystring *)this->_Mycont)->_Myptr())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
		--_Myptr;
		return (*this);
		}

	_Myt  operator--(int)
		{	
		_Myt _Tmp = *this;
		--*this;
		return (_Tmp);
		}

	_Myt&  operator+=(difference_type _Off)
		{	
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
			{ if (!(_Myptr + _Off <= (((_Mystring *)this->_Mycont)->_Myptr() + ((_Mystring *)this->_Mycont)->_Mysize) && _Myptr + _Off >= ((_Mystring *)this->_Mycont)->_Myptr())) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
// #line 176 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		}
		_Myptr += _Off;
		return (*this);
		}

	_Myt  operator+(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp += _Off);
		}

	_Myt&  operator-=(difference_type _Off)
		{	
		return (*this += -_Off);
		}

	_Myt  operator-(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp -= _Off);
		}

	difference_type  operator-(const _Myt& _Right) const
		{	

 

// #line 204 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container() && this->_Same_container(_Right))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
 // #line 209 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (_Myptr - _Right._Myptr);
		}

	reference  operator[](difference_type _Off) const
		{	
		return (*(*this + _Off));
		}

	bool  operator==(const _Myt& _Right) const
		{	

 

// #line 224 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container() && this->_Same_container(_Right))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
 // #line 229 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (_Myptr == _Right._Myptr);
		}

	bool  operator!=(const _Myt& _Right) const
		{	
		return (!(*this == _Right));
		}

	bool  operator<(const _Myt& _Right) const
		{	

 

// #line 244 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		if (this->_Mycont != ((const _Container_base_secure *)-4))
		{
			{ if (!(this->_Has_container() && this->_Same_container(_Right))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		}
 // #line 249 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (_Myptr < _Right._Myptr);
		}

	bool  operator>(const _Myt& _Right) const
		{	
		return (_Right < *this);
		}

	bool  operator<=(const _Myt& _Right) const
		{	
		return (!(_Right < *this));
		}

	bool  operator>=(const _Myt& _Right) const
		{	
		return (!(*this < _Right));
		}

 








// #line 278 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	static void __cdecl _Xlen()
		{
		_Mystring::_Xlen();
		}

	static void __cdecl _Xran()
		{
		_Mystring::_Xran();
		}

	static void __cdecl _Xinvarg()
		{
		_Mystring::_Xinvarg();
		}

	pointer _Myptr;	
	};

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	_String_const_iterator<_Elem, _Traits, _Alloc> __cdecl operator+(
		typename _String_const_iterator<_Elem, _Traits, _Alloc>
			::difference_type _Off,
		_String_const_iterator<_Elem, _Traits, _Alloc> _Next)
	{	
	return (_Next += _Off);
	}

		
template<class _Elem,
	class _Traits,
	class _Alloc>
	class _String_iterator
		: public _String_const_iterator<_Elem, _Traits, _Alloc>
	{	
public:
	typedef _String_iterator<_Elem, _Traits, _Alloc> _Myt;
	typedef _String_const_iterator<_Elem, _Traits, _Alloc> _Mybase;

	typedef random_access_iterator_tag iterator_category;
	typedef _Elem value_type;
	typedef typename _Alloc::difference_type difference_type;
	typedef typename _Alloc::pointer pointer;
	typedef typename _Alloc::reference reference;

	 _String_iterator()
		{	
		}

 







// #line 338 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
 
		 _String_iterator(pointer _Ptr, const _Container_base_secure *_Pstring)
			: _Mybase(_Ptr, _Pstring)
			{	
			}

 






// #line 352 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"


	typedef pointer _Checked_iterator_base_type;

	_Checked_iterator_base_type _Checked_iterator_base() const
	{
		return const_cast<pointer>(this->_Myptr);
	}

	void _Checked_iterator_assign_from_base(_Checked_iterator_base_type _Base)
	{
		this->_Myptr = _Base;
	}
// #line 366 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	reference  operator*() const
		{	
		return ((reference)**(_Mybase *)this);
		}

	pointer  operator->() const
		{	
		return (&**this);
		}

	_Myt&  operator++()
		{	
		++(*(_Mybase *)this);
		return (*this);
		}

	_Myt  operator++(int)
		{	
		_Myt _Tmp = *this;
		++*this;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	
		--(*(_Mybase *)this);
		return (*this);
		}

	_Myt  operator--(int)
		{	
		_Myt _Tmp = *this;
		--*this;
		return (_Tmp);
		}

	_Myt&  operator+=(difference_type _Off)
		{	
		(*(_Mybase *)this) += _Off;
		return (*this);
		}

	_Myt  operator+(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp += _Off);
		}

	_Myt&  operator-=(difference_type _Off)
		{	
		return (*this += -_Off);
		}

	_Myt  operator-(difference_type _Off) const
		{	
		_Myt _Tmp = *this;
		return (_Tmp -= _Off);
		}

	difference_type  operator-(const _Mybase& _Right) const
		{	
		return ((_Mybase)*this - _Right);
		}

	reference  operator[](difference_type _Off) const
		{	
		return (*(*this + _Off));
		}
	};

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	_String_iterator<_Elem, _Traits, _Alloc>  operator+(
		typename _String_iterator<_Elem, _Traits, _Alloc>
			::difference_type _Off,
		_String_iterator<_Elem, _Traits, _Alloc> _Next)
	{	
	return (_Next += _Off);
	}

		
class  _String_base
		: public _Container_base_secure
	{	
public:
	 static void __cdecl _Xlen();	

	 static void __cdecl _Xran();	

	 static void __cdecl _Xinvarg();
	};

		
template<class _Ty,
	class _Alloc>
	class _String_val
		: public _String_base
	{	
protected:
	typedef typename _Alloc::template
		rebind<_Ty>::other _Alty;

	 _String_val(_Alty _Al = _Alty())
		: _Alval(_Al)
		{	
		}

public:
	 _String_val(const _String_val &_Right)
		: _Alval(_Right._Alval)
		{	





// #line 485 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		}

protected:
	_Alty _Alval;	
	};

		
template<class _Elem,
	class _Traits,
	class _Ax>
	class basic_string
		: public _String_val<_Elem, _Ax>
	{	
public:
	typedef basic_string<_Elem, _Traits, _Ax> _Myt;
	typedef _String_val<_Elem, _Ax> _Mybase;
	typedef typename _Mybase::_Alty _Alloc;
	typedef typename _Alloc::size_type size_type;
	typedef typename _Alloc::difference_type _Dift;
	typedef _Dift difference_type;
	typedef typename _Alloc::pointer _Tptr;
	typedef typename _Alloc::const_pointer _Ctptr;
	typedef _Tptr pointer;
	typedef _Ctptr const_pointer;
	typedef typename _Alloc::reference _Reft;
	typedef _Reft reference;
	typedef typename _Alloc::const_reference const_reference;
	typedef typename _Alloc::value_type value_type;



	typedef _String_iterator<_Elem, _Traits, _Alloc> iterator;
	typedef _String_const_iterator<_Elem, _Traits, _Alloc> const_iterator;


	friend class _String_const_iterator<_Elem, _Traits, _Alloc>;

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    

































// #line 560 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

 

	 basic_string()
		: _Mybase()
		{	
		_Tidy();
		}

 



























// #line 598 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

 

	explicit  basic_string(const _Alloc& _Al)
		: _Mybase(_Al)
		{	
		_Tidy();
		}

	 basic_string(const _Myt& _Right, size_type _Roff,
		size_type _Count = npos)
		: _Mybase()
		{	





// #line 617 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		_Tidy();
		assign(_Right, _Roff, _Count);
		}

	 basic_string(const _Myt& _Right, size_type _Roff, size_type _Count,
		const _Alloc& _Al)
		: _Mybase(_Al)
		{	





// #line 631 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		_Tidy();
		assign(_Right, _Roff, _Count);
		}

	 basic_string(const _Elem *_Ptr, size_type _Count)
		: _Mybase()
		{	
		_Tidy();
		assign(_Ptr, _Count);
		}

	 basic_string(const _Elem *_Ptr, size_type _Count, const _Alloc& _Al)
		: _Mybase(_Al)
		{	
		_Tidy();
		assign(_Ptr, _Count);
		}

	 basic_string(const _Elem *_Ptr)
		: _Mybase()
		{	
		_Tidy();
		assign(_Ptr);
		}

	 basic_string(const _Elem *_Ptr, const _Alloc& _Al)
		: _Mybase(_Al)
		{	
		_Tidy();
		assign(_Ptr);
		}

	 basic_string(size_type _Count, _Elem _Ch)
		: _Mybase()
		{	
		_Tidy();
		assign(_Count, _Ch);
		}

	 basic_string(size_type _Count, _Elem _Ch, const _Alloc& _Al)
		: _Mybase(_Al)
		{	
		_Tidy();
		assign(_Count, _Ch);
		}

	template<class _It>
		 basic_string(_It _First, _It _Last)
		: _Mybase()
		{	
		_Tidy();
		_Construct(_First, _Last, _Iter_cat(_First));
		}

	template<class _It>
		 basic_string(_It _First, _It _Last, const _Alloc& _Al)
		: _Mybase(_Al)
		{	
		_Tidy();
		_Construct(_First, _Last, _Iter_cat(_First));
		}

	 basic_string(const_pointer _First, const_pointer _Last)
		: _Mybase()
		{	


// #line 699 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		_Tidy();
		if (_First != _Last)
			assign(&*_First, _Last - _First);
		}

	 basic_string(const_iterator _First, const_iterator _Last)
		: _Mybase()
		{	


// #line 710 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		_Tidy();
		if (_First != _Last)
			assign(&*_First, _Last - _First);
		}

 // #line 716 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	
	
	 basic_string(const _Myt& _Right)
		: _Mybase(_Right)
		{	
		_Tidy();
		assign(_Right, 0, npos);
		}

 



































































































































// #line 859 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	template<class _It>
		void  _Construct(_It _Count,
			_It _Ch, _Int_iterator_tag)
		{	
		assign((size_type)_Count, (_Elem)_Ch);
		}

	template<class _It>
		void  _Construct(_It _First,
			_It _Last, input_iterator_tag)
		{	
		try {
		for (; _First != _Last; ++_First)
			append((size_type)1, (_Elem)*_First);
		} catch (...) {
		_Tidy(true);
		throw;
		}
		}

	template<class _It>
		void  _Construct(_It _First,
			_It _Last, forward_iterator_tag)
		{	






// #line 891 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		size_type _Count = 0;
		_Distance(_First, _Last, _Count);
		reserve(_Count);

		try {
		for (; _First != _Last; ++_First)
			append((size_type)1, (_Elem)*_First);
		} catch (...) {
		_Tidy(true);
		throw;
		}
		}

	 ~basic_string()
		{	
		_Tidy(true);
		}

	typedef _Traits traits_type;
	typedef _Alloc allocator_type;

	 static const size_type npos;	

	_Myt&  operator=(const _Myt& _Right)
		{	
		return (assign(_Right));
		}

	_Myt&  operator=(const _Elem *_Ptr)
		{	
		return (assign(_Ptr));
		}

	_Myt&  operator=(_Elem _Ch)
		{	
		return (assign(1, _Ch));
		}

	_Myt&  operator+=(const _Myt& _Right)
		{	
		return (append(_Right));
		}

	_Myt&  operator+=(const _Elem *_Ptr)
		{	
		return (append(_Ptr));
		}

	_Myt&  operator+=(_Elem _Ch)
		{	
		return (append((size_type)1, _Ch));
		}

	_Myt&  append(const _Myt& _Right)
		{	
		return (append(_Right, 0, npos));
		}

	_Myt&  append(const _Myt& _Right,
		size_type _Roff, size_type _Count)
		{	
		if (_Right.size() < _Roff)
			_String_base::_Xran();	
		size_type _Num = _Right.size() - _Roff;
		if (_Num < _Count)
			_Count = _Num;	
		if (npos - _Mysize <= _Count || _Mysize + _Count < _Mysize)
			_String_base::_Xlen();	

		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Traits_helper::copy_s<_Traits>(_Myptr() + _Mysize, _Myres - _Mysize,
				_Right._Myptr() + _Roff, _Count);
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  append(const _Elem *_Ptr, size_type _Count)
		{	

 


// #line 976 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Inside(_Ptr))
			return (append(*this, _Ptr - _Myptr(), _Count));	
		if (npos - _Mysize <= _Count || _Mysize + _Count < _Mysize)
			_String_base::_Xlen();	

		size_type _Num;
		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Traits_helper::copy_s<_Traits>(_Myptr() + _Mysize, _Myres - _Mysize, _Ptr, _Count);
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  append(const _Elem *_Ptr)
		{	
		;
		return (append(_Ptr, _Traits::length(_Ptr)));
		}

	_Myt&  append(size_type _Count, _Elem _Ch)
		{	
			if (npos - _Mysize <= _Count)
			_String_base::_Xlen();	

		size_type _Num;
		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Chassign(_Mysize, _Count, _Ch);
			_Eos(_Num);
			}
		return (*this);
		}

	template<class _It>
		_Myt&  append(_It _First, _It _Last)
		{	
		return (_Append(_First, _Last, _Iter_cat(_First)));
		}

	template<class _It>
		_Myt&  _Append(_It _Count, _It _Ch, _Int_iterator_tag)
		{	
		return (append((size_type)_Count, (_Elem)_Ch));
		}

	template<class _It>
		_Myt&  _Append(_It _First, _It _Last, input_iterator_tag)
		{	
		return (replace(end(), end(), _First, _Last));
		}

	_Myt&  append(const_pointer _First, const_pointer _Last)
		{	
		return (replace(end(), end(), _First, _Last));
		}

	_Myt&  append(const_iterator _First, const_iterator _Last)
		{	
		return (replace(end(), end(), _First, _Last));
		}

	_Myt&  assign(const _Myt& _Right)
		{	
		return (assign(_Right, 0, npos));
		}

	_Myt&  assign(const _Myt& _Right,
		size_type _Roff, size_type _Count)
		{	
		if (_Right.size() < _Roff)
			_String_base::_Xran();	
		size_type _Num = _Right.size() - _Roff;
		if (_Count < _Num)
			_Num = _Count;	

		if (this == &_Right)
			erase((size_type)(_Roff + _Num)), erase(0, _Roff);	
		else if (_Grow(_Num))
			{	
			_Traits_helper::copy_s<_Traits>(_Myptr(), _Myres, _Right._Myptr() + _Roff, _Num);
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  assign(const _Elem *_Ptr, size_type _Count)
		{	

 


// #line 1070 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Inside(_Ptr))
			return (assign(*this, _Ptr - _Myptr(), _Count));	

		if (_Grow(_Count))
			{	
			_Traits_helper::copy_s<_Traits>(_Myptr(), _Myres, _Ptr, _Count);
			_Eos(_Count);
			}
		return (*this);
		}

	_Myt&  assign(const _Elem *_Ptr)
		{	
		;
		return (assign(_Ptr, _Traits::length(_Ptr)));
		}

	_Myt&  assign(size_type _Count, _Elem _Ch)
		{	
		if (_Count == npos)
			_String_base::_Xlen();	

		if (_Grow(_Count))
			{	
			_Chassign(0, _Count, _Ch);
			_Eos(_Count);
			}
		return (*this);
		}

	template<class _It>
		_Myt&  assign(_It _First, _It _Last)
		{	
		return (_Assign(_First, _Last, _Iter_cat(_First)));
		}

	template<class _It>
		_Myt&  _Assign(_It _Count, _It _Ch, _Int_iterator_tag)
		{	
		return (assign((size_type)_Count, (_Elem)_Ch));
		}

	template<class _It>
		_Myt&  _Assign(_It _First, _It _Last, input_iterator_tag)
		{	
		return (replace(begin(), end(), _First, _Last));
		}

	_Myt&  assign(const_pointer _First, const_pointer _Last)
		{	
		return (replace(begin(), end(), _First, _Last));
		}

	_Myt&  assign(const_iterator _First, const_iterator _Last)
		{	
		return (replace(begin(), end(), _First, _Last));
		}

	_Myt&  insert(size_type _Off, const _Myt& _Right)
		{	
		return (insert(_Off, _Right, 0, npos));
		}

	_Myt&  insert(size_type _Off,
		const _Myt& _Right, size_type _Roff, size_type _Count)
		{	
		if (_Mysize < _Off || _Right.size() < _Roff)
			_String_base::_Xran();	
		size_type _Num = _Right.size() - _Roff;
		if (_Num < _Count)
			_Count = _Num;	
		if (npos - _Mysize <= _Count)
			_String_base::_Xlen();	

		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off, _Mysize - _Off);	
			if (this == &_Right)
				_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
					_Myptr() + (_Off < _Roff ? _Roff + _Count : _Roff),
						_Count);	
			else
				_Traits_helper::copy_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
					_Right._Myptr() + _Roff, _Count);	
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  insert(size_type _Off,
		const _Elem *_Ptr, size_type _Count)
		{	

 


// #line 1169 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Inside(_Ptr))
			return (insert(_Off, *this,
				_Ptr - _Myptr(), _Count));	
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (npos - _Mysize <= _Count)
			_String_base::_Xlen();	
		size_type _Num;
		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off, _Mysize - _Off);	
			_Traits_helper::copy_s<_Traits>(_Myptr() + _Off, _Myres - _Off, _Ptr, _Count);	
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  insert(size_type _Off, const _Elem *_Ptr)
		{	
		;
		return (insert(_Off, _Ptr, _Traits::length(_Ptr)));
		}

	_Myt&  insert(size_type _Off,
		size_type _Count, _Elem _Ch)
		{	
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (npos - _Mysize <= _Count)
			_String_base::_Xlen();	
		size_type _Num;
		if (0 < _Count && _Grow(_Num = _Mysize + _Count))
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off, _Mysize - _Off);	
			_Chassign(_Off, _Count, _Ch);	
			_Eos(_Num);
			}
		return (*this);
		}

	iterator  insert(const_iterator _Where)
		{	
		return (insert(_Where, _Elem()));
		}

	iterator  insert(const_iterator _Where, _Elem _Ch)
		{	
		size_type _Off = _Pdif(_Where, begin());
		insert(_Off, 1, _Ch);
		return (begin() + _Off);
		}

	void  insert(const_iterator _Where, size_type _Count, _Elem _Ch)
		{	
		size_type _Off = _Pdif(_Where, begin());
		insert(_Off, _Count, _Ch);
		}

	template<class _It>
		void  insert(const_iterator _Where, _It _First, _It _Last)
		{	
		_Insert(_Where, _First, _Last, _Iter_cat(_First));
		}

	template<class _It>
		void  _Insert(const_iterator _Where, _It _Count, _It _Ch,
			_Int_iterator_tag)
		{	
		insert(_Where, (size_type)_Count, (_Elem)_Ch);
		}

	template<class _It>
		void  _Insert(const_iterator _Where, _It _First, _It _Last,
			input_iterator_tag)
		{	
		replace(_Where, _Where, _First, _Last);
		}

	void  insert(const_iterator _Where, const_pointer _First, const_pointer _Last)
		{	
		replace(_Where, _Where, _First, _Last);
		}

	void  insert(const_iterator _Where, const_iterator _First, const_iterator _Last)
		{	
		replace(_Where, _Where, _First, _Last);
		}

	_Myt&  erase(size_type _Off = 0,
		size_type _Count = npos)
		{	
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (_Mysize - _Off < _Count)
			_Count = _Mysize - _Off;	
		if (0 < _Count)
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off, _Myptr() + _Off + _Count,
				_Mysize - _Off - _Count);
			size_type _Newsize = _Mysize - _Count;
			_Eos(_Newsize);
			}
		return (*this);
		}

	iterator  erase(const_iterator _Where)
		{	
		size_type _Count = _Pdif(_Where, begin());
		erase(_Count, 1);
		return (iterator(_Myptr() + _Count, this));
		}

	iterator  erase(const_iterator _First, const_iterator _Last)
		{	
		size_type _Count = _Pdif(_First, begin());
		erase(_Count, _Pdif(_Last, _First));
		return (iterator(_Myptr() + _Count, this));
		}

	void  clear()
		{	
		erase(begin(), end());
		}

	_Myt&  replace(size_type _Off, size_type _N0, const _Myt& _Right)
		{	
		return (replace(_Off, _N0, _Right, 0, npos));
		}

	_Myt&  replace(size_type _Off,
		size_type _N0, const _Myt& _Right, size_type _Roff, size_type _Count)
		{	
		if (_Mysize < _Off || _Right.size() < _Roff)
			_String_base::_Xran();	
		if (_Mysize - _Off < _N0)
			_N0 = _Mysize - _Off;	
		size_type _Num = _Right.size() - _Roff;
		if (_Num < _Count)
			_Count = _Num;	
		if (npos - _Count <= _Mysize - _N0)
			_String_base::_Xlen();	

		size_type _Nm = _Mysize - _N0 - _Off;	
		size_type _Newsize = _Mysize + _Count - _N0;
		if (_Mysize < _Newsize)
			_Grow(_Newsize);

		if (this != &_Right)
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
			_Traits_helper::copy_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
				_Right._Myptr() + _Roff, _Count);	
			}
		else if (_Count <= _N0)
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
				_Myptr() + _Roff, _Count);	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
			}
		else if (_Roff <= _Off)
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
				_Myptr() + _Roff, _Count);	
			}
		else if (_Off + _N0 <= _Roff)
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
				_Myptr() + (_Roff + _Count - _N0), _Count);	
			}
		else
			{	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off, _Myres - _Off,
				_Myptr() + _Roff, _N0);	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _N0, _Myres - _Off - _N0, _Myptr() + _Roff + _Count,
				_Count - _N0);	
			}

		_Eos(_Newsize);
		return (*this);
		}

	_Myt&  replace(size_type _Off,
		size_type _N0, const _Elem *_Ptr, size_type _Count)
		{	

 


// #line 1369 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Inside(_Ptr))
			return (replace(_Off, _N0, *this,
				_Ptr - _Myptr(), _Count));	
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (_Mysize - _Off < _N0)
			_N0 = _Mysize - _Off;	
		if (npos - _Count <= _Mysize - _N0)
			_String_base::_Xlen();	
		size_type _Nm = _Mysize - _N0 - _Off;

		if (_Count < _N0)
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
		size_type _Num;
		if ((0 < _Count || 0 < _N0) && _Grow(_Num = _Mysize + _Count - _N0))
			{	
			if (_N0 < _Count)
				_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
					_Myptr() + _Off + _N0, _Nm);	
			_Traits_helper::copy_s<_Traits>(_Myptr() + _Off, _Myres - _Off, _Ptr, _Count);	
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  replace(size_type _Off, size_type _N0, const _Elem *_Ptr)
		{	
		;
		return (replace(_Off, _N0, _Ptr, _Traits::length(_Ptr)));
		}

	_Myt&  replace(size_type _Off,
		size_type _N0, size_type _Count, _Elem _Ch)
		{	
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (_Mysize - _Off < _N0)
			_N0 = _Mysize - _Off;	
		if (npos - _Count <= _Mysize - _N0)
			_String_base::_Xlen();	
		size_type _Nm = _Mysize - _N0 - _Off;

		if (_Count < _N0)
			_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
				_Myptr() + _Off + _N0, _Nm);	
		size_type _Num;
		if ((0 < _Count || 0 < _N0) && _Grow(_Num = _Mysize + _Count - _N0))
			{	
			if (_N0 < _Count)
				_Traits_helper::move_s<_Traits>(_Myptr() + _Off + _Count, _Myres - _Off - _Count,
					_Myptr() + _Off + _N0, _Nm);	
			_Chassign(_Off, _Count, _Ch);	
			_Eos(_Num);
			}
		return (*this);
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last, const _Myt& _Right)
		{	
		return (replace(
			_Pdif(_First, begin()), _Pdif(_Last, _First), _Right));
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last, const _Elem *_Ptr,
		size_type _Count)
		{	
		return (replace(
			_Pdif(_First, begin()), _Pdif(_Last, _First), _Ptr, _Count));
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last, const _Elem *_Ptr)
		{	
		return (replace(
			_Pdif(_First, begin()), _Pdif(_Last, _First), _Ptr));
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last,
		size_type _Count, _Elem _Ch)
		{	
		return (replace(
			_Pdif(_First, begin()), _Pdif(_Last, _First), _Count, _Ch));
		}

	template<class _It>
		_Myt&  replace(const_iterator _First, const_iterator _Last,
			_It _First2, _It _Last2)
		{	
		return (_Replace(_First, _Last,
			_First2, _Last2, _Iter_cat(_First2)));
		}

	template<class _It>
		_Myt&  _Replace(const_iterator _First, const_iterator _Last,
			_It _Count, _It _Ch, _Int_iterator_tag)
		{	
		return (replace(_First, _Last, (size_type)_Count, (_Elem)_Ch));
		}

	template<class _It>
		_Myt&  _Replace(const_iterator _First, const_iterator _Last,
			_It _First2, _It _Last2, input_iterator_tag)
		{	
		_Myt _Right(_First2, _Last2);
		replace(_First, _Last, _Right);
		return (*this);
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last,
		const_pointer _First2, const_pointer _Last2)
		{	
		if (_First2 == _Last2)
			erase(_Pdif(_First, begin()), _Pdif(_Last, _First));
		else
			replace(_Pdif(_First, begin()), _Pdif(_Last, _First),
				&*_First2, _Last2 - _First2);
		return (*this);
		}

	_Myt&  replace(const_iterator _First, const_iterator _Last,
		const_iterator _First2, const_iterator _Last2)
		{	
		if (_First2 == _Last2)
			erase(_Pdif(_First, begin()), _Pdif(_Last, _First));
		else
			replace(_Pdif(_First, begin()), _Pdif(_Last, _First),
				&*_First2, _Last2 - _First2);
		return (*this);
		}

	iterator  begin()
		{	
		return (iterator(_Myptr(), this));
		}

	const_iterator  begin() const
		{	
		return (const_iterator(_Myptr(), this));
		}

	iterator  end()
		{	
		return (iterator(_Myptr() + _Mysize, this));
		}

	const_iterator  end() const
		{	
		return (const_iterator(_Myptr() + _Mysize, this));
		}

	reverse_iterator  rbegin()
		{	
		return (reverse_iterator(end()));
		}

	const_reverse_iterator  rbegin() const
		{	
		return (const_reverse_iterator(end()));
		}

	reverse_iterator  rend()
		{	
		return (reverse_iterator(begin()));
		}

	const_reverse_iterator  rend() const
		{	
		return (const_reverse_iterator(begin()));
		}

	reference  at(size_type _Off)
		{	
		if (_Mysize <= _Off)
			_String_base::_Xran();	
		return (_Myptr()[_Off]);
		}

	const_reference  at(size_type _Off) const
		{	
		if (_Mysize <= _Off)
			_String_base::_Xran();	
		return (_Myptr()[_Off]);
		}

 

	reference  operator[](size_type _Off)
		{	

 









// #line 1570 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		{ if (!(_Off <= _Mysize)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
 // #line 1572 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (_Myptr()[_Off]);
		}

	const_reference  operator[](size_type _Off) const
		{	

 









// #line 1590 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
		{ if (!(_Off <= _Mysize)) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
 // #line 1592 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		return (_Myptr()[_Off]);
		}

 // #line 1597 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

 











































// #line 1643 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

	void  push_back(_Elem _Ch)
		{	
		insert(end(), _Ch);
		}

	const _Elem * c_str() const
		{	
		return (_Myptr());
		}

	const _Elem * data() const
		{	
		return (c_str());
		}

	size_type  length() const
		{	
		return (_Mysize);
		}

	size_type  size() const
		{	
		return (_Mysize);
		}

	size_type  max_size() const
		{	
		size_type _Num = _Mybase::_Alval.max_size();
		return (_Num <= 1 ? 1 : _Num - 1);
		}

	void  resize(size_type _Newsize)
		{	
		resize(_Newsize, _Elem());
		}

	void  resize(size_type _Newsize, _Elem _Ch)
		{	
		if (_Newsize <= _Mysize)
			erase(_Newsize);
		else
			append(_Newsize - _Mysize, _Ch);
		}

	size_type  capacity() const
		{	
		return (_Myres);
		}

	void  reserve(size_type _Newcap = 0)
		{	
		if (_Mysize <= _Newcap && _Myres != _Newcap)
			{	
			size_type _Size = _Mysize;
			if (_Grow(_Newcap, true))
				_Eos(_Size);
			}
		}

	bool  empty() const
		{	
		return (_Mysize == 0);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	size_type  copy(_Elem *_Dest,
		size_type _Count, size_type _Off = 0) const
		{	

 


// #line 1717 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		
		return _Copy_s(_Dest, _Count, _Count, _Off);
		}

	size_type  _Copy_s(_Elem *_Dest, size_type _Dest_size,
		size_type _Count, size_type _Off = 0) const
		{	
		;
		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (_Mysize - _Off < _Count)
			_Count = _Mysize - _Off;
		_Traits_helper::copy_s<_Traits>(_Dest, _Dest_size, _Myptr() + _Off, _Count);
		return (_Count);
		}

	void  swap(_Myt& _Right)
		{	
		if (this == &_Right)
			;	
		else if (_Mybase::_Alval == _Right._Alval)
			{	

 

// #line 1744 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

			_Bxty _Tbx = _Bx;
			_Bx = _Right._Bx, _Right._Bx = _Tbx;

			size_type _Tlen = _Mysize;
			_Mysize = _Right._Mysize, _Right._Mysize = _Tlen;

			size_type _Tres = _Myres;
			_Myres = _Right._Myres, _Right._Myres = _Tres;
			}
		else
			{	
			_Myt _Tmp = *this;

			*this = _Right;
			_Right = _Tmp;
			}
		}

	size_type  find(const _Myt& _Right, size_type _Off = 0) const
		{	
		return (find(_Right._Myptr(), _Off, _Right.size()));
		}

	size_type  find(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1776 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Count == 0 && _Off <= _Mysize)
			return (_Off);	

		size_type _Nm;
		if (_Off < _Mysize && _Count <= (_Nm = _Mysize - _Off))
			{	
			const _Elem *_Uptr, *_Vptr;
			for (_Nm -= _Count - 1, _Vptr = _Myptr() + _Off;
				(_Uptr = _Traits::find(_Vptr, _Nm, *_Ptr)) != 0;
				_Nm -= _Uptr - _Vptr + 1, _Vptr = _Uptr + 1)
				if (_Traits::compare(_Uptr, _Ptr, _Count) == 0)
					return (_Uptr - _Myptr());	
			}

		return (npos);	
		}

	size_type  find(const _Elem *_Ptr, size_type _Off = 0) const
		{	
		;
		return (find(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  find(_Elem _Ch, size_type _Off = 0) const
		{	
		return (find((const _Elem *)&_Ch, _Off, 1));
		}

	size_type  rfind(const _Myt& _Right, size_type _Off = npos) const
		{	
		return (rfind(_Right._Myptr(), _Off, _Right.size()));
		}

	size_type  rfind(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1818 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Count == 0)
			return (_Off < _Mysize ? _Off : _Mysize);	
		if (_Count <= _Mysize)
			{	
			const _Elem *_Uptr = _Myptr() +
				(_Off < _Mysize - _Count ? _Off : _Mysize - _Count);
			for (; ; --_Uptr)
				if (_Traits::eq(*_Uptr, *_Ptr)
					&& _Traits::compare(_Uptr, _Ptr, _Count) == 0)
					return (_Uptr - _Myptr());	
				else if (_Uptr == _Myptr())
					break;	
			}

		return (npos);	
		}

	size_type  rfind(const _Elem *_Ptr, size_type _Off = npos) const
		{	
		;
		return (rfind(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  rfind(_Elem _Ch, size_type _Off = npos) const
		{	
		return (rfind((const _Elem *)&_Ch, _Off, 1));
		}

	size_type  find_first_of(const _Myt& _Right,
		size_type _Off = 0) const
		{	
		return (find_first_of(_Right._Myptr(), _Off, _Right.size()));
		}

	size_type  find_first_of(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1861 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (0 < _Count && _Off < _Mysize)
			{	
			const _Elem *const _Vptr = _Myptr() + _Mysize;
			for (const _Elem *_Uptr = _Myptr() + _Off; _Uptr < _Vptr; ++_Uptr)
				if (_Traits::find(_Ptr, _Count, *_Uptr) != 0)
					return (_Uptr - _Myptr());	
			}

		return (npos);	
		}

	size_type  find_first_of(const _Elem *_Ptr, size_type _Off = 0) const
		{	
		;
		return (find_first_of(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  find_first_of(_Elem _Ch, size_type _Off = 0) const
		{	
		return (find((const _Elem *)&_Ch, _Off, 1));
		}

	size_type  find_last_of(const _Myt& _Right,
		size_type _Off = npos) const
		{	
		return (find_last_of(_Right._Myptr(), _Off, _Right.size()));
		}

	size_type  find_last_of(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1898 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (0 < _Count && 0 < _Mysize)
			for (const _Elem *_Uptr = _Myptr()
				+ (_Off < _Mysize ? _Off : _Mysize - 1); ; --_Uptr)
				if (_Traits::find(_Ptr, _Count, *_Uptr) != 0)
					return (_Uptr - _Myptr());	
				else if (_Uptr == _Myptr())
					break;	

		return (npos);	
		}

	size_type  find_last_of(const _Elem *_Ptr,
		size_type _Off = npos) const
		{	
		;
		return (find_last_of(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  find_last_of(_Elem _Ch, size_type _Off = npos) const
		{	
		return (rfind((const _Elem *)&_Ch, _Off, 1));
		}

	size_type  find_first_not_of(const _Myt& _Right,
		size_type _Off = 0) const
		{	
		return (find_first_not_of(_Right._Myptr(), _Off,
			_Right.size()));
		}

	size_type  find_first_not_of(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1937 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Off < _Mysize)
			{	
			const _Elem *const _Vptr = _Myptr() + _Mysize;
			for (const _Elem *_Uptr = _Myptr() + _Off; _Uptr < _Vptr; ++_Uptr)
				if (_Traits::find(_Ptr, _Count, *_Uptr) == 0)
					return (_Uptr - _Myptr());
			}
		return (npos);
		}

	size_type  find_first_not_of(const _Elem *_Ptr,
		size_type _Off = 0) const
		{	
		;
		return (find_first_not_of(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  find_first_not_of(_Elem _Ch, size_type _Off = 0) const
		{	
		return (find_first_not_of((const _Elem *)&_Ch, _Off, 1));
		}

	size_type  find_last_not_of(const _Myt& _Right,
		size_type _Off = npos) const
		{	
		return (find_last_not_of(_Right._Myptr(), _Off, _Right.size()));
		}

	size_type  find_last_not_of(const _Elem *_Ptr,
		size_type _Off, size_type _Count) const
		{	

 


// #line 1974 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (0 < _Mysize)
			for (const _Elem *_Uptr = _Myptr()
				+ (_Off < _Mysize ? _Off : _Mysize - 1); ; --_Uptr)
				if (_Traits::find(_Ptr, _Count, *_Uptr) == 0)
					return (_Uptr - _Myptr());
				else if (_Uptr == _Myptr())
					break;
		return (npos);
		}

	size_type  find_last_not_of(const _Elem *_Ptr,
		size_type _Off = npos) const
		{	
		;
		return (find_last_not_of(_Ptr, _Off, _Traits::length(_Ptr)));
		}

	size_type  find_last_not_of(_Elem _Ch, size_type _Off = npos) const
		{	
		return (find_last_not_of((const _Elem *)&_Ch, _Off, 1));
		}

	_Myt  substr(size_type _Off = 0, size_type _Count = npos) const
		{	
		return (_Myt(*this, _Off, _Count, get_allocator()));
		}

	int  compare(const _Myt& _Right) const
		{	
		return (compare(0, _Mysize, _Right._Myptr(), _Right.size()));
		}

	int  compare(size_type _Off, size_type _N0,
		const _Myt& _Right) const
		{	
		return (compare(_Off, _N0, _Right, 0, npos));
		}

	int  compare(size_type _Off,
		size_type _N0, const _Myt& _Right,
		size_type _Roff, size_type _Count) const
		{	
		if (_Right.size() < _Roff)
			_String_base::_Xran();	
		if (_Right._Mysize - _Roff < _Count)
			_Count = _Right._Mysize - _Roff;	
		return (compare(_Off, _N0, _Right._Myptr() + _Roff, _Count));
		}

	int  compare(const _Elem *_Ptr) const
		{	
		;
		return (compare(0, _Mysize, _Ptr, _Traits::length(_Ptr)));
		}

	int  compare(size_type _Off, size_type _N0, const _Elem *_Ptr) const
		{	
		;
		return (compare(_Off, _N0, _Ptr, _Traits::length(_Ptr)));
		}

	int  compare(size_type _Off,
		size_type _N0, const _Elem *_Ptr, size_type _Count) const
		{	

 


// #line 2044 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

		if (_Mysize < _Off)
			_String_base::_Xran();	
		if (_Mysize - _Off < _N0)
			_N0 = _Mysize - _Off;	

		size_type _Ans = _Traits::compare(_Myptr() + _Off, _Ptr,
			_N0 < _Count ? _N0 : _Count);
		return (_Ans != 0 ? (int)_Ans : _N0 < _Count ? -1
			: _N0 == _Count ? 0 : +1);
		}

	allocator_type  get_allocator() const
		{	
		return (_Mybase::_Alval);
		}

	enum
		{	
		_BUF_SIZE = 16 / sizeof (_Elem) < 1 ? 1
			: 16 / sizeof(_Elem)};

protected:
	enum
		{	
		_ALLOC_MASK = sizeof (_Elem) <= 1 ? 15
			: sizeof (_Elem) <= 2 ? 7
			: sizeof (_Elem) <= 4 ? 3
			: sizeof (_Elem) <= 8 ? 1 : 0};

	void  _Chassign(size_type _Off, size_type _Count, _Elem _Ch)
		{	
		if (_Count == 1)
			_Traits::assign(*(_Myptr() + _Off), _Ch);
		else
			_Traits::assign(_Myptr() + _Off, _Count, _Ch);
		}

	void  _Copy(size_type _Newsize, size_type _Oldlen)
		{	
		size_type _Newres = _Newsize | _ALLOC_MASK;
		if (max_size() < _Newres)
			_Newres = _Newsize;	
		else if (_Newres / 3 < _Myres / 2
			&& _Myres <= max_size() - _Myres / 2)
			_Newres = _Myres + _Myres / 2;	
		_Elem *_Ptr = 0;

		try {
			_Ptr = _Mybase::_Alval.allocate(_Newres + 1);
		} catch (...) {
			_Newres = _Newsize;	
			try {
				_Ptr = _Mybase::_Alval.allocate(_Newres + 1);
			} catch (...) {
			_Tidy(true);	
			throw;
			}
		}

		if (0 < _Oldlen)
			_Traits_helper::copy_s<_Traits>(_Ptr, _Newres + 1, _Myptr(), _Oldlen);	
		_Tidy(true);
		_Bx._Ptr = _Ptr;
		_Myres = _Newres;
		_Eos(_Oldlen);
		}

	void  _Eos(size_type _Newsize)
		{	
		_Traits::assign(_Myptr()[_Mysize = _Newsize], _Elem());
		}

	bool  _Grow(size_type _Newsize,
		bool _Trim = false)
		{	
			if (max_size() < _Newsize)
			_String_base::_Xlen();	
		if (_Myres < _Newsize)
			_Copy(_Newsize, _Mysize);	
		else if (_Trim && _Newsize < _BUF_SIZE)
			_Tidy(true,	
				_Newsize < _Mysize ? _Newsize : _Mysize);
		else if (_Newsize == 0)
			_Eos(0);	
		return (0 < _Newsize);	
		}

	bool  _Inside(const _Elem *_Ptr)
		{	
		if (_Ptr == 0 || _Ptr < _Myptr() || _Myptr() + _Mysize <= _Ptr)
			return (false);	
		else
			return (true);
		}

	static size_type __cdecl _Pdif(const_iterator _P2,
		const_iterator _P1)
		{	
		return ((_P2)._Myptr == 0 ? 0 : _P2 - _P1);
		}

	void  _Tidy(bool _Built = false,
		size_type _Newsize = 0)
		{	
		if (!_Built)
			;
		else if (_BUF_SIZE <= _Myres)
			{	
			_Elem *_Ptr = _Bx._Ptr;
			if (0 < _Newsize)
				_Traits_helper::copy_s<_Traits>(_Bx._Buf, _BUF_SIZE, _Ptr, _Newsize);
			_Mybase::_Alval.deallocate(_Ptr, _Myres + 1);
			}
		_Myres = _BUF_SIZE - 1;
		_Eos(_Newsize);
		}

	union _Bxty
		{	
		_Elem _Buf[_BUF_SIZE];
		_Elem *_Ptr;
		} _Bx;

	_Elem * _Myptr()
		{	
		return (_BUF_SIZE <= _Myres ? _Bx._Ptr : _Bx._Buf);
		}

	const _Elem * _Myptr() const
		{	
		return (_BUF_SIZE <= _Myres ? _Bx._Ptr : _Bx._Buf);
		}

	size_type _Mysize;	
	size_type _Myres;	
	};

	
template<class _Elem, class _Traits, class _Ax>
	class _Move_operation_category<basic_string<_Elem, _Traits, _Ax> >
	{
	public:
		typedef _Swap_move_tag _Move_cat;
	};

		
template<class _Elem,
	class _Traits,
	class _Alloc>
	 const typename basic_string<_Elem, _Traits, _Alloc>::size_type
		basic_string<_Elem, _Traits, _Alloc>::npos =
			(typename basic_string<_Elem, _Traits, _Alloc>::size_type)(-1);

		

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void __cdecl swap(basic_string<_Elem, _Traits, _Alloc>& _Left,
		basic_string<_Elem, _Traits, _Alloc>& _Right)
	{	
	_Left.swap(_Right);
	}

typedef basic_string<char, char_traits<char>, allocator<char> >
	string;
typedef basic_string<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wstring;

 













// #line 2229 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
}

 #pragma warning(default: 4251)
 #pragma warning(pop)
 #pragma pack(pop)
// #line 2235 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"

// #line 2237 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"
// #line 2238 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xstring"






// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 13 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"
namespace std {





		
class logic_error
	: public std:: exception
	{	
public:
	explicit  logic_error(const string& _Message)
		: _Str(_Message)
		{	
		}

	virtual  ~logic_error() throw ()
		{	
		}

	virtual const char * what() const throw ()
		{	
		return (_Str.c_str());
		}

 





// #line 45 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

private:
	string _Str;	
	};

		
class domain_error
	: public logic_error
	{	
public:
	explicit  domain_error(const string& _Message)
		: logic_error(_Message)
		{	
		}

	virtual  ~domain_error() throw ()
		{	
		}

 





// #line 71 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class invalid_argument
	: public logic_error
	{	
public:
	explicit  invalid_argument(const string& _Message)
		: logic_error(_Message)
		{	
		}

	virtual  ~invalid_argument() throw ()
		{	
		}

 





// #line 95 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class length_error
	: public logic_error
	{	
public:
	explicit  length_error(const string& _Message)
		: logic_error(_Message)
		{	
		}

	virtual  ~length_error() throw ()
		{	
		}

 





// #line 119 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class out_of_range
	: public logic_error
	{	
public:
	explicit  out_of_range(const string& _Message)
		: logic_error(_Message)
		{	
		}

	virtual  ~out_of_range() throw ()
		{	
		}

 





// #line 143 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class runtime_error
	: public std:: exception
	{	
public:
	explicit  runtime_error(const string& _Message)
		: _Str(_Message)
		{	
		}

	virtual  ~runtime_error() throw ()
		{	
		}

	virtual const char * what() const throw ()
		{	
		return (_Str.c_str());
		}

 





// #line 172 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

private:
	string _Str;	
	};

		
class overflow_error
	: public runtime_error
	{	
public:
	explicit  overflow_error(const string& _Message)
		: runtime_error(_Message)
		{	
		}

	virtual  ~overflow_error() throw ()
		{	
		}

 





// #line 198 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class underflow_error
	: public runtime_error
	{	
public:
	explicit  underflow_error(const string& _Message)
		: runtime_error(_Message)
		{	
		}

	virtual  ~underflow_error() throw ()
		{	
		}

 





// #line 222 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};

		
class range_error
	: public runtime_error
	{	
public:
	explicit  range_error(const string& _Message)
		: runtime_error(_Message)
		{	
		}

	virtual  ~range_error() throw ()
		{	
		}

 





// #line 246 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

	};























}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 276 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"

// #line 278 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"
// #line 279 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\stdexcept"





// #line 9 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"















#pragma once
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"







#pragma pack(push,8)
// #line 27 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"

 



 

// #line 35 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"

struct __type_info_node {
    void *memPtr;
    __type_info_node* next;
};

extern __type_info_node __type_info_root_node;

class type_info {
public:
    virtual ~type_info();
     bool  operator==(const type_info& rhs) const;
     bool  operator!=(const type_info& rhs) const;
     int  before(const type_info& rhs) const;
     const char*  name(__type_info_node* __ptype_info_node = &__type_info_root_node) const;
     const char*  raw_name() const;
private:
    void *_m_data;
    char _m_d_name[1];
     type_info(const type_info& rhs);
    type_info&  operator=(const type_info& rhs);
     static const char *__cdecl _Name_base(const type_info *,__type_info_node* __ptype_info_node);
     static void __cdecl _Type_info_dtor(type_info *);
};

 

 namespace std {

using ::type_info;

 }





 namespace std {

class  bad_cast : public exception {
public:










     bad_cast(const char * _Message = "bad cast");
     bad_cast(const bad_cast &);
    virtual  ~bad_cast();
// #line 90 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"
};

class  bad_typeid : public exception {
public:










     bad_typeid(const char * _Message = "bad typeid");
     bad_typeid(const bad_typeid &);
    virtual  ~bad_typeid();
// #line 108 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"

};

class  __non_rtti_object : public bad_typeid {
public:










     __non_rtti_object(const char * _Message);
     __non_rtti_object(const __non_rtti_object &);
    virtual  ~__non_rtti_object();
// #line 127 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"
};

 }
 

 
























































// #line 190 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"

// #line 192 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"


#pragma pack(pop)
// #line 196 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"

// #line 198 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\typeinfo"







// #line 10 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"

#pragma once






 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"

namespace std {
struct _DebugHeapTag_t
	{	
	int _Type;
	};
}

		

 


























































































































// #line 146 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"
   
   
   
   
  // #line 151 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"



 #pragma warning(pop)
 #pragma pack(pop)
// #line 157 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"


// #line 160 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"
// #line 161 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xdebug"





// #line 11 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"

#pragma once



// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"

#pragma once


// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"














#pragma once
// #line 17 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"


extern "C" {
// #line 26 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"











































// #line 70 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"










                                













[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isctype([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Type);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isctype_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Type, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl isalpha([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isalpha_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl isupper([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isupper_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl islower([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _islower_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl isdigit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isdigit_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl isxdigit([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isxdigit_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl isspace([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isspace_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl ispunct([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _ispunct_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl isalnum([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isalnum_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl isprint([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isprint_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl isgraph([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _isgraph_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl iscntrl([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _iscntrl_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl toupper([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl tolower([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl _tolower([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _tolower_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]   int __cdecl _toupper([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl _toupper_l([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C, [SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __isascii([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __toascii([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __iscsymf([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);
[returnvalue:SA_Post(MustCheck=SA_Yes)]  int __cdecl __iscsym([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _C);

// #line 130 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"



















































































// #line 214 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"












































// #line 259 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"






// #line 266 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"





























































































// #line 360 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"
















// #line 377 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"

// #line 379 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"


}
// #line 383 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"

// #line 385 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ctype.h"
// #line 6 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"














#pragma once
// #line 17 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"




// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\crtdefs.h"














 


















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// #line 22 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"






#pragma pack(push,8)
// #line 30 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"


extern "C" {
// #line 34 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"

























struct lconv {
        char *decimal_point;
        char *thousands_sep;
        char *grouping;
        char *int_curr_symbol;
        char *currency_symbol;
        char *mon_decimal_point;
        char *mon_thousands_sep;
        char *mon_grouping;
        char *positive_sign;
        char *negative_sign;
        char int_frac_digits;
        char frac_digits;
        char p_cs_precedes;
        char p_sep_by_space;
        char n_cs_precedes;
        char n_sep_by_space;
        char p_sign_posn;
        char n_sign_posn;
        };

// #line 81 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"




















// #line 102 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"

  int __cdecl _configthreadlocale([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Flag);
  char * __cdecl setlocale([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Category, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Locale);
  struct lconv * __cdecl localeconv(void);
  _locale_t __cdecl _get_current_locale(void);
  _locale_t __cdecl _create_locale([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Category, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Locale);
 void __cdecl _free_locale([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);


[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "_get_current_locale" " instead. See online help for details."))  _locale_t __cdecl __get_current_locale(void);
[returnvalue:SA_Post(MustCheck=SA_Yes)] __declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "_create_locale" " instead. See online help for details."))  _locale_t __cdecl __create_locale([SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] int _Category, [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] const char * _Locale);
__declspec(deprecated("This function or variable has been superceded by newer library or operating system functionality. Consider using " "_free_locale" " instead. See online help for details."))  void __cdecl __free_locale([SA_Pre(Null=SA_Maybe)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] _locale_t _Locale);











}
// #line 127 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"


#pragma pack(pop)
// #line 131 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"

// #line 133 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\locale.h"
// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"






#pragma pack(push,8)
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"



extern "C" {
// #line 20 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"

		











		



















typedef struct _Collvec
	{	
	unsigned long _Hand;	
	unsigned int _Page;		
	} _Collvec;

typedef struct _Ctypevec
	{	
	unsigned long _Hand;	
	unsigned int _Page;		
	const short *_Table;
	int _Delfl;
	} _Ctypevec;

typedef struct _Cvtvec
	{	
	unsigned long _Hand;	
	unsigned int _Page;		
	} _Cvtvec;

		
 _Collvec __cdecl _Getcoll();
 _Ctypevec __cdecl _Getctype();
 _Cvtvec __cdecl _Getcvt();

 int __cdecl _Getdateorder();

 int __cdecl _Mbrtowc([SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] wchar_t *, const char *, size_t,
	mbstate_t *, const _Cvtvec *);
 float __cdecl _Stof(const char *, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **, long);
 double __cdecl _Stod(const char *, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **, long);
 long double __cdecl _Stold(const char *, [SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **, 
  long);
 int __cdecl _Strcoll(const char *, const char *,
	const char *, const char *, const _Collvec *);
 size_t __cdecl _Strxfrm(
  [SA_Pre(Null=SA_No,WritableElements="\n@""_End1-_String1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="return")] [SA_Post(Deref=1,Valid=SA_Yes)] char * _String1, 
  [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char * _End1, const char *, const char *, const _Collvec *);
 int __cdecl _Tolower(int, const _Ctypevec *);
 int __cdecl _Toupper(int, const _Ctypevec *);
 int __cdecl _Wcrtomb([SA_Pre(Null=SA_No,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] char *, wchar_t, mbstate_t *,
	const _Cvtvec *);
 int __cdecl _Wcscoll(const wchar_t *, const wchar_t *,
	const wchar_t *, const wchar_t *, const _Collvec *);
 size_t __cdecl _Wcsxfrm(
  [SA_Pre(Null=SA_No,WritableElements="\n@""_End1-_String1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="return")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_String1, 
  [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_End1, const wchar_t *, const wchar_t *, const _Collvec *);

 short __cdecl _Getwctype(wchar_t, const _Ctypevec *);
 const wchar_t * __cdecl _Getwctypes(const wchar_t *, const wchar_t *,
	short*, const _Ctypevec*);
 wchar_t __cdecl _Towlower(wchar_t, const _Ctypevec *);
 wchar_t __cdecl _Towupper(wchar_t, const _Ctypevec *);

}
// #line 109 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"


extern "C" {
 void *__cdecl _Gettnames();
 char *__cdecl _Getdays();
 char *__cdecl _Getmonths();
 size_t __cdecl _Strftime(
  [SA_Pre(Null=SA_No,WritableElements="_Maxsize")] [SA_Pre(Deref=1,Valid=SA_No)] char *, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] size_t _Maxsize, 
  [SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)]  const char *, [SA_Pre(Null=SA_No)] [SA_Pre(Deref=1,Valid=SA_Yes,Access=SA_Read)] const struct tm *, void *);
}

extern "C" {
_locale_t __cdecl _GetLocaleForCP(unsigned int);
}


#pragma pack(pop)
// #line 127 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"

// #line 129 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo.h"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"




 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable:4412)
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"

namespace std {

		
class  _Timevec
	{	
public:
	 _Timevec(void *_Ptr = 0)
		: _Timeptr(_Ptr)
		{	
		}

	 _Timevec(const _Timevec& _Right)
		{	
		*this = _Right;
		}

	 ~_Timevec()
		{	
		free(_Timeptr);
		}

	_Timevec&  operator=(const _Timevec& _Right)
		{	
		_Timeptr = _Right._Timeptr;
		((_Timevec *)&_Right)->_Timeptr = 0;
		return (*this);
		}

	void * _Getptr() const
		{	
		return (_Timeptr);
		}

private:
	void *_Timeptr;	
	};

		


#pragma warning(push)
#pragma warning(disable:4412)
class  _Locinfo
	{	
public:
	typedef ::_Collvec _Collvec;
	typedef ::_Ctypevec _Ctypevec;
	typedef ::_Cvtvec _Cvtvec;
	typedef std::_Timevec _Timevec;

    static  void __cdecl _Locinfo_ctor(_Locinfo *, const char *);
	static  void __cdecl _Locinfo_ctor(_Locinfo *, const string&);
	static  void __cdecl _Locinfo_ctor(_Locinfo *, int, const char *);
    static  void __cdecl _Locinfo_dtor(_Locinfo *);
    static  _Locinfo& __cdecl _Locinfo_Addcats(_Locinfo *, int, const char *);

	 _Locinfo(const char *_Pch = "C")

        : _Lock(0)
// #line 76 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"
        {
        if (_Pch == 0)
            throw runtime_error("bad locale name");
        _Locinfo_ctor(this, _Pch);
        }

	 _Locinfo(int _I, const char *_Pch)

        : _Lock(0)
// #line 86 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"
        {
        if (_Pch == 0)
            throw runtime_error("bad locale name");
        _Locinfo_ctor(this, _I, _Pch);
        }

	 _Locinfo(const string& locstr)

        : _Lock(0)
// #line 96 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"
		{
        _Locinfo_ctor(this, locstr);
		}

	 ~_Locinfo()
        {
        _Locinfo_dtor(this);
        }

	_Locinfo&  _Addcats(int _I, const char *_Pch)
		{
		if (_Pch == 0)
			throw runtime_error("bad locale name");
		return _Locinfo_Addcats(this, _I, _Pch);
		}

	string  _Getname() const
		{	
		return (_Newlocname);
		}

	_Collvec  _Getcoll() const
		{	
		return (::_Getcoll());
		}

	_Ctypevec  _Getctype() const
		{	
		return (::_Getctype());
		}

	_Cvtvec  _Getcvt() const
		{	
		return (::_Getcvt());
		}

	const lconv * _Getlconv() const
		{	
		return (localeconv());
		}

	_Timevec  _Gettnames() const
		{	
		return (_Timevec(::_Gettnames()));
		}

	const char * _Getdays() const
		{	
		const char *_Ptr = ::_Getdays();
		if (_Ptr != 0)
			{	
			((_Locinfo *)this)->_Days = _Ptr;
			free((void *)_Ptr);
			}
		return (_Days.size() != 0 ? _Days.c_str()
			: ":Sun:Sunday:Mon:Monday:Tue:Tuesday:Wed:Wednesday"
				":Thu:Thursday:Fri:Friday:Sat:Saturday");
		}

	const char * _Getmonths() const
		{	
		const char *_Ptr = ::_Getmonths();
		if (_Ptr != 0)
			{	
			((_Locinfo *)this)->_Months = _Ptr;
			free((void *)_Ptr);
			}
		return (_Months.size() != 0 ? _Months.c_str()
			: ":Jan:January:Feb:February:Mar:March"
				":Apr:April:May:May:Jun:June"
				":Jul:July:Aug:August:Sep:September"
				":Oct:October:Nov:November:Dec:December");
		}

	const char * _Getfalse() const
		{	
		return ("false");
		}

	const char * _Gettrue() const
		{	
		return ("true");
		}

	int  _Getdateorder() const
		{	
		return ::_Getdateorder();
		}

private:





	_Lockit _Lock;	
// #line 193 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"
	string _Days;	
	string _Months;	
	string _Oldlocname;	
	string _Newlocname;	
	};
#pragma warning(pop)

		
template<class _Elem> inline
	int __cdecl _LStrcoll(const _Elem *_First1, const _Elem *_Last1,
		const _Elem *_First2, const _Elem *_Last2,
			const _Locinfo::_Collvec *)
	{	
	for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2)
		if (*_First1 < *_First2)
			return (-1);	
		else if (*_First2 < *_First1)
			return (+1);	
	return (_First2 != _Last2 ? -1 : _First1 != _Last1 ? +1 : 0);
	}

template<> inline
	int __cdecl _LStrcoll(const char *_First1, const char *_Last1,
		const char *_First2, const char *_Last2,
			const _Locinfo::_Collvec *_Vector)
	{	
	return (_Strcoll(_First1, _Last1, _First2, _Last2, _Vector));
	}

template<> inline
	int __cdecl _LStrcoll(const wchar_t *_First1, const wchar_t *_Last1,
		const wchar_t *_First2, const wchar_t *_Last2,
			const _Locinfo::_Collvec *_Vector)
	{	
	return (_Wcscoll(_First1, _Last1, _First2, _Last2, _Vector));
	}

		
template<class _Elem> inline
	size_t __cdecl _LStrxfrm(_Elem *_First1, _Elem *_Last1,
		const _Elem *_First2, const _Elem *_Last2,
			const _Locinfo::_Collvec *)
	{	
	size_t _Count = _Last2 - _First2;
	if (_Count <= (size_t)(_Last1 - _First1))
		::memcpy_s((_First1), ((_Last1 - _First1)* sizeof(_Elem)), (_First2), (_Count * sizeof (_Elem)));
	return (_Count);
	}

template<> inline
	size_t __cdecl _LStrxfrm(
		[SA_Pre(Null=SA_No,WritableElements="\n@""_Last1-_First1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="return")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_First1, 
		[SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] char *_Last1,
		const char *_First2, const char *_Last2,
			const _Locinfo::_Collvec *_Vector)
	{	
	return (_Strxfrm(_First1, _Last1, _First2, _Last2, _Vector));
	}

template<> inline
	size_t __cdecl _LStrxfrm(
		[SA_Pre(Null=SA_No,WritableElements="\n@""_Last1-_First1")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="return")] [SA_Post(Deref=1,Valid=SA_Yes)] wchar_t *_First1, 
		[SA_Pre(Null=SA_No,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *_Last1,
		const wchar_t *_First2, const wchar_t *_Last2,
			const _Locinfo::_Collvec *_Vector)
	{	
	return (_Wcsxfrm(_First1, _Last1, _First2, _Last2, _Vector));
	}
}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 266 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"

// #line 268 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"
// #line 269 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocinfo"





// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"


 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable:4412)
// #line 18 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

namespace std {

		
template<class _Dummy>
	class _Locbase
	{	
public:
	 static const int collate = ((1 << (1)) >> 1);
	 static const int ctype = ((1 << (2)) >> 1);
	 static const int monetary = ((1 << (3)) >> 1);
	 static const int numeric = ((1 << (4)) >> 1);
	 static const int time = ((1 << (5)) >> 1);
	 static const int messages = ((1 << (6)) >> 1);
	 static const int all = (((1 << (7)) >> 1) - 1);
	 static const int none = 0;
	};

template<class _Dummy>
	const int _Locbase<_Dummy>::collate;
template<class _Dummy>
	const int _Locbase<_Dummy>::ctype;
template<class _Dummy>
	const int _Locbase<_Dummy>::monetary;
template<class _Dummy>
	const int _Locbase<_Dummy>::numeric;
template<class _Dummy>
	const int _Locbase<_Dummy>::time;
template<class _Dummy>
	const int _Locbase<_Dummy>::messages;
template<class _Dummy>
	const int _Locbase<_Dummy>::all;
template<class _Dummy>
	const int _Locbase<_Dummy>::none;

		
class locale;
template<class _Facet>
	const _Facet& __cdecl use_facet(const locale&);


#pragma warning(push)
#pragma warning(disable:4412)
class  locale
	: public _Locbase<int>
	{	
public:
	typedef int category;

			
	class  id
		{	
	public:
		 id(size_t _Val = 0)
			: _Id(_Val)
			{	
			}

		 operator size_t()
			{	
			if (_Id == 0)
				{	
				{ ::std:: _Lockit _Lock(0);
					if (_Id == 0)
						_Id = ++_Id_cnt;
				}
				}
			return (_Id);
			}

	private:
		 id(const id&);	
		id&  operator=(const id&);	

		size_t _Id;	
		static  int& __cdecl _Id_cnt_func();



		 static int _Id_cnt;	
// #line 99 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
		};

	class _Locimp;

			
	class facet
		{	
		friend class locale;
		friend class _Locimp;

	public:
		 static size_t __cdecl _Getcat(const facet ** = 0,
			const locale * = 0)
			{	
			return ((size_t)(-1));
			}

		 void  _Incref()
			{	
			{ ::std:: _Lockit _Lock(0);
				if (_Refs < (size_t)(-1))
					++_Refs;
			}
			}

		 facet * _Decref()
			{	
			{ ::std:: _Lockit _Lock(0);
				if (0 < _Refs && _Refs < (size_t)(-1))
					--_Refs;
				return (_Refs == 0 ? this : 0);
			}
			}

		void  _Register()
			{


// #line 138 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
			facet_Register(this);
// #line 140 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
			}

  






















// #line 166 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"



		 virtual  ~facet()
			{	
			}

	protected:
		 explicit  facet(size_t _Initrefs = 0)
			: _Refs(_Initrefs)
			{	
			}

	private:


// #line 183 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
		static void __cdecl facet_Register(facet *);	
// #line 185 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

		 facet(const facet&);	
		facet&  operator=(const facet&);	

		size_t _Refs;	
		};

#pragma warning(push)
#pragma warning(disable:4275)
			
	class  _Locimp
		: public facet
		{	
	protected:
		 ~_Locimp()
			{
			_Locimp_dtor(this);
			}

	private:
		static  void __cdecl _Locimp_dtor(_Locimp *); 
		static void _Locimp_ctor(_Locimp *,const _Locimp&);	
		static  void __cdecl _Locimp_Addfac(_Locimp *,facet *, size_t);	
		friend class locale;

		 _Locimp(bool _Transparent = false)
			: locale::facet(1), _Facetvec(0), _Facetcount(0),
				_Catmask(none), _Xparent(_Transparent), _Name("*")
			{ }
		
		 _Locimp(const _Locimp& _Right)
			: locale::facet(1), _Facetvec(0), _Facetcount(_Right._Facetcount),
				_Catmask(_Right._Catmask), _Xparent(_Right._Xparent), _Name(_Right._Name)
			{
			_Locimp_ctor(this, _Right);
			}
		
		void  _Addfac(facet *_Pfacet, size_t _Id)
			{
			_Locimp_Addfac(this, _Pfacet, _Id);
			}

		static _Locimp * _Makeloc(const _Locinfo&,
			category, _Locimp *, const locale *);	

		static void _Makewloc(const _Locinfo&,
			category, _Locimp *, const locale *);	

 
		static void _Makeushloc(const _Locinfo&,
			category, _Locimp *, const locale *);	
 // #line 237 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

		static void _Makexloc(const _Locinfo&,
			category, _Locimp *, const locale *);	

		facet **_Facetvec;	
		size_t _Facetcount;	
		category _Catmask;	
		bool _Xparent;	
		string _Name;	

		static  _Locimp *& __cdecl _Clocptr_func();	



		 static _Locimp *_Clocptr;	
// #line 253 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
private:
		_Locimp&  operator=(const _Locimp&);	
	
		};
#pragma warning(pop)

	__declspec(deprecated("This is an obsolete part of the Standard C++ Library Implementation. Do not use it.")) locale&  _Addfac(facet *_Fac, size_t _Id,
		size_t _Catmask)
		{
		if (1 < this->_Ptr->_Refs)
			{	
			this->_Ptr->_Decref();
			this->_Ptr = new _Locimp(*this->_Ptr);
			}
		this->_Ptr->_Addfac(_Fac, _Id);

		if (_Catmask != 0)
			this->_Ptr->_Name = "*";
		return (*this);
		}


	template<class _Elem,
		class _Traits,
		class _Alloc>
		bool  operator()(const basic_string<_Elem, _Traits, _Alloc>& _Left,
			const basic_string<_Elem, _Traits, _Alloc>& _Right) const
		{	
		const std::collate<_Elem>& _Coll_fac =
			std::use_facet<std::collate<_Elem> >(*this);

		return (_Coll_fac.compare(_Left.c_str(), _Left.c_str() + _Left.size(),
			_Right.c_str(), _Right.c_str() + _Right.size()) < 0);
		}

	template<class _Facet>
		locale  combine(const locale& _Loc) const
		{	
		_Facet *_Facptr;

		try {
			_Facptr = (_Facet *)&std::use_facet<_Facet>(_Loc);
		} catch (...) {
			throw runtime_error("locale::combine facet missing");
		}

		_Locimp *_Newimp = new _Locimp(*_Ptr);
		_Newimp->_Addfac(_Facptr, _Facet::id);
		_Newimp->_Catmask = 0;
		_Newimp->_Name = "*";
		return (locale(_Newimp));
		}

	template<class _Facet>
		 locale(const locale& _Loc, const _Facet *_Facptr)
			: _Ptr(new _Locimp(*_Loc._Ptr))
		{	
		if (_Facptr != 0)
			{	
			_Ptr->_Addfac((_Facet *)_Facptr, _Facet::id);
			if (_Facet::_Getcat() != (size_t)(-1))
				{	
				_Ptr->_Catmask = 0;
				_Ptr->_Name = "*";
				}
			}
		}


	 locale() throw ()
		: _Ptr(_Init())
		{	
		_Getgloballocale()->_Incref();
		}

	 locale(_Uninitialized)
		{	
		}

	 locale(const locale& _Right) throw ()
		: _Ptr(_Right._Ptr)
		{	
		_Ptr->_Incref();
		}

	 locale(const locale& _Loc, const locale& _Other,
		category _Cat) 	
		: _Ptr(new _Locimp(*_Loc._Ptr))
		{	
		try {
			{ _Locinfo _Lobj(_Loc._Ptr->_Catmask, _Loc._Ptr->_Name.c_str());
				_Locimp::_Makeloc(_Lobj._Addcats(_Cat & _Other._Ptr->_Catmask,
					_Other._Ptr->_Name.c_str()), _Cat, _Ptr, &_Other);
			}
		} catch (...) {
			delete (_Ptr->_Decref());
			throw;
		}
		}

	explicit  locale(const char *_Locname,
		category _Cat = all) 	
		: _Ptr(new _Locimp)
		{	
		try {
		_Init();
		{ _Locinfo _Lobj(_Cat, _Locname);
			if (_Lobj._Getname().compare("*") == 0)
				throw runtime_error("bad locale name");
			_Locimp::_Makeloc(_Lobj, _Cat, _Ptr, 0);
		}
		} catch (...) {
		delete (_Ptr->_Decref());
		throw;
		}
		}

	 locale(const locale& _Loc, const char * _Locname,
		category _Cat) 	
		: _Ptr(new _Locimp(*_Loc._Ptr))
		{	
		try {
		{ _Locinfo _Lobj(_Loc._Ptr->_Catmask, _Loc._Ptr->_Name.c_str());
			bool _Hadname = _Lobj._Getname().compare("*") != 0;
			_Lobj._Addcats(_Cat, _Locname);

			if (_Hadname && _Lobj._Getname().compare("*") == 0)
				throw runtime_error("bad locale name");
			_Locimp::_Makeloc(_Lobj, _Cat, _Ptr, 0);
		}
		} catch (...) {
		delete (_Ptr->_Decref());
		throw;
		}
		}

	 ~locale() throw ()
		{	
		if (_Ptr != 0)
			delete (_Ptr->_Decref());
		}

	locale&  operator=(const locale& _Right) throw ()
		{	
		if (_Ptr != _Right._Ptr)
			{	
			delete (_Ptr->_Decref());
			_Ptr = _Right._Ptr;
			_Ptr->_Incref();
			}
		return (*this);
		}

	string  name() const
		{	
		return (_Ptr->_Name);
		}

	const facet * _Getfacet(size_t _Id) const 	
		{	
		const facet *_Facptr = _Id < _Ptr->_Facetcount
			? _Ptr->_Facetvec[_Id] : 0;	
		if (_Facptr != 0 || !_Ptr->_Xparent)
			return (_Facptr);	
		else
			{	
			locale::_Locimp *_Ptr = _Getgloballocale();
			return (_Id < _Ptr->_Facetcount
				? _Ptr->_Facetvec[_Id]	
				: 0);	
			}
		}


	bool  operator==(const locale& _Loc) const
		{	
		return (_Ptr == _Loc._Ptr
			|| name().compare("*") != 0 && name().compare(_Loc.name()) == 0);
		}

	bool  operator!=(const locale& _Right) const
		{	
		return (!(*this == _Right));
		}

	static  const locale& __cdecl classic();	

	static  locale __cdecl global(const locale&);	

	static  locale __cdecl empty();	

private:
	locale(_Locimp *_Ptrimp)
		: _Ptr(_Ptrimp)
		{	
		}

	static  _Locimp *__cdecl _Getgloballocale();
	static  _Locimp *__cdecl _Init();	
	static  void __cdecl _Setgloballocale(void *);

	_Locimp *_Ptr;	
	};
#pragma warning(pop)

		
template<class _Facet>
	struct _Facetptr
	{	
	 static const locale::facet *_Psave;
	};

template<class _Facet>
	 const locale::facet *_Facetptr<_Facet>::_Psave = 0;

template<class _Facet> inline __declspec(deprecated("This is an obsolete part of the Standard C++ Library Implementation. Do not use it."))
	locale _Addfac(locale _Loc, const _Facet *_Facptr)
		{	
		size_t _Cat = _Facet::_Getcat(0, 0);
		locale _Newloc = _Loc._Addfac((_Facet *)_Facptr, _Facet::id, _Cat);

		return (_Newloc);
		}

  

  


template<class _Facet> inline
	const _Facet& __cdecl use_facet(const locale& _Loc)


{	
	{ ::std:: _Lockit _Lock(0);	
		const locale::facet *_Psave =
			_Facetptr<_Facet>::_Psave;	

		size_t _Id = _Facet::id;
		const locale::facet *_Pf = _Loc._Getfacet(_Id);

		if (_Pf != 0)
			;	
		else if (_Psave != 0)
			_Pf = _Psave;	
		else if (_Facet::_Getcat(&_Psave, &_Loc) == (size_t)(-1))

 
		throw bad_cast();	

	

// #line 506 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

		else
			{	
			_Pf = _Psave;
			_Facetptr<_Facet>::_Psave = _Psave;

			locale::facet *_Pfmod = (_Facet *)_Psave;
			_Pfmod->_Incref();
			_Pfmod->_Register();
			}

		return ((const _Facet&)(*_Pf));	
	}
	}

template<class _Facet> inline __declspec(deprecated("This is an obsolete part of the Standard C++ Library Implementation. Do not use it."))
	const _Facet& __cdecl use_facet(const locale& _Loc, const _Facet *,
		bool = false)
	{	
	return use_facet<_Facet>(_Loc);
	}

		
template<class _Elem,
	class _InIt> inline
	int __cdecl _Getloctxt(_InIt& _First, _InIt& _Last, size_t _Numfields,
		const _Elem *_Ptr)
	{	
	for (size_t _Off = 0; _Ptr[_Off] != (_Elem)0; ++_Off)
		if (_Ptr[_Off] == _Ptr[0])
			++_Numfields;	
	string _Str(_Numfields, '\0');	

	int _Ans = -2;	
	for (size_t _Column = 1; ; ++_Column, ++_First, _Ans = -1)
		{	
		bool  _Prefix = false;	
		size_t _Off = 0;	
		size_t _Field = 0;	

		for (; _Field < _Numfields; ++_Field)
			{	
			for (; _Ptr[_Off] != (_Elem)0 && _Ptr[_Off] != _Ptr[0]; ++_Off)
				;	

			if (_Str[_Field] != '\0')
				_Off += _Str[_Field];	
			else if (_Ptr[_Off += _Column] == _Ptr[0]
				|| _Ptr[_Off] == (_Elem)0)
				{	
				_Str[_Field] = (char)(_Column < 127
					? _Column : 127);	
				_Ans = (int)_Field;	
				}
			else if (_First == _Last || _Ptr[_Off] != *_First)
				_Str[_Field] = (char)(_Column < 127
					? _Column : 127);	
			else
				_Prefix = true;	
			}

		if (!_Prefix || _First == _Last)
			break;	
		}
	return (_Ans);	
	}

		



template<class _Elem> inline
	char __cdecl _Maklocbyte(_Elem _Char,
		const _Locinfo::_Cvtvec&)
	{	
	return ((char)(unsigned char)_Char);
	}

template<> inline
	char __cdecl _Maklocbyte(wchar_t _Char,
		const _Locinfo::_Cvtvec& _Cvt)
	{	
	char _Byte = '\0';
	mbstate_t _Mbst1 = {0};
	_Wcrtomb(&_Byte, _Char, &_Mbst1, &_Cvt);
	return (_Byte);
	}


		



template<class _Elem> inline
	_Elem __cdecl _Maklocchr(char _Byte, _Elem *,
		const _Locinfo::_Cvtvec&)
	{	
	return ((_Elem)(unsigned char)_Byte);
	}

template<> inline
	wchar_t __cdecl _Maklocchr(char _Byte, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *,
		const _Locinfo::_Cvtvec& _Cvt)
	{	
	wchar_t _Wc = L'\0';
	mbstate_t _Mbst1 = {0};
	_Mbrtowc(&_Wc, &_Byte, 1, &_Mbst1, &_Cvt);
	return (_Wc);
	}


		



template<class _Elem> inline
	_Elem *__cdecl _Maklocstr(const char *_Ptr, _Elem *,
		const _Locinfo::_Cvtvec&)
	{	
	size_t _Count = ::strlen(_Ptr) + 1;
	_Elem *_Ptrdest = new _Elem[_Count];

#pragma warning(push)
#pragma warning(disable: 6011)
	
	for (_Elem *_Ptrnext = _Ptrdest; 0 < _Count; --_Count, ++_Ptrnext, ++_Ptr)
		*_Ptrnext = (_Elem)(unsigned char)*_Ptr;
	return (_Ptrdest);
#pragma warning(pop)
	}

template<> inline
	wchar_t *__cdecl _Maklocstr(const char *_Ptr, [SA_Pre(Null=SA_Maybe,NullTerminated=SA_Yes)] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Pre(Deref=1,Access=SA_Read)] wchar_t *,
		const _Locinfo::_Cvtvec& _Cvt)
	{	
	size_t _Count, _Count1;
	size_t _Wchars;
	const char *_Ptr1;
	int _Bytes;
	wchar_t _Wc;
	mbstate_t _Mbst1 = {0};

	_Count1 = ::strlen(_Ptr) + 1;
	for (_Count = _Count1, _Wchars = 0, _Ptr1 = _Ptr; 0 < _Count;
		_Count -= _Bytes, _Ptr1 += _Bytes, ++_Wchars)
		if ((_Bytes = _Mbrtowc(&_Wc, _Ptr1, _Count, &_Mbst1, &_Cvt)) <= 0)
			break;
	++_Wchars;	

	wchar_t *_Ptrdest = new wchar_t[_Wchars];
	wchar_t *_Ptrnext = _Ptrdest;
	mbstate_t _Mbst2 = {0};
#pragma warning(push)
#pragma warning(disable: 6011)
	
	for (; 0 < _Wchars;
		_Count -= _Bytes, _Ptr += _Bytes, --_Wchars, ++_Ptrnext)
		if ((_Bytes = _Mbrtowc(_Ptrnext, _Ptr, _Count1, &_Mbst2, &_Cvt)) <= 0)
			break;
	*_Ptrnext = L'\0';
#pragma warning(pop)
	return (_Ptrdest);
	}


#pragma warning(push)
#pragma warning(disable:4275)
		
class  codecvt_base
	: public locale::facet
	{	
public:
	enum
		{	
		ok, partial, error, noconv};
	typedef int result;

	 codecvt_base(size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		}

	bool  always_noconv() const throw ()
		{	
		return (do_always_noconv());
		}

	int  max_length() const throw ()
		{	
		return (do_max_length());
		}

	int  encoding() const throw ()
		{	
		return (do_encoding());
		}

	 ~codecvt_base()
		{	
		}

protected:
	virtual bool  do_always_noconv() const throw ()
		{	
		return (true);
		}

	virtual int  do_max_length() const throw ()
		{	
		return (1);
		}

	virtual int  do_encoding() const throw ()
		{	
		return (1);	
		}
	};
#pragma warning(pop)

		
template<class _Elem,
	class _Byte,
	class _Statype>
	class codecvt
		: public codecvt_base
	{	
public:
	typedef _Elem intern_type;
	typedef _Byte extern_type;
	typedef _Statype state_type;

	result  in(_Statype& _State,
		const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1,
		_Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
		{	
		return (do_in(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  out(_Statype& _State,
		const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_out(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  unshift(_Statype& _State,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_unshift(_State, _First2, _Last2, _Mid2));
		}

	int  length(const _Statype& _State, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	
		return (do_length(_State, _First1, _Last1, _Count));
		}

	 static locale::id id;	

	explicit  codecvt(size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 codecvt(const _Locinfo& _Lobj, size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new codecvt<_Elem, _Byte, _Statype>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~codecvt()
		{	
		}

protected:
	void  _Init(const _Locinfo&)
		{	
		}

	virtual result  do_in(_Statype&,
		const _Byte *_First1, const _Byte *, const _Byte *& _Mid1,
		_Elem *_First2, _Elem *, _Elem *& _Mid2) const
		{	
		_Mid1 = _First1, _Mid2 = _First2;
		return (noconv);	
		}

	virtual result  do_out(_Statype&,
		const _Elem *_First1, const _Elem *, const _Elem *& _Mid1,
		_Byte *_First2, _Byte *, _Byte *& _Mid2) const
		{	
		_Mid1 = _First1, _Mid2 = _First2;
		return (noconv);	
		}

	virtual result  do_unshift(_Statype&,
		_Byte *_First2, _Byte *, _Byte *&_Mid2) const
		{	
		_Mid2 = _First2;
		return (noconv);	
		}

	virtual int  do_length(const _Statype&, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	
		return ((int)(_Count < (size_t)(_Last1 - _First1)
			? _Count : _Last1 - _First1));	
		}
	};

		
template<class _Elem,
	class _Byte,
	class _Statype>
	 locale::id codecvt<_Elem, _Byte, _Statype>::id;

		
template<> class  codecvt<wchar_t, char, _Mbstatet>
	: public codecvt_base
	{	
public:
	typedef wchar_t _Elem;
	typedef char _Byte;
	typedef _Mbstatet _Statype;
	typedef _Elem intern_type;
	typedef _Byte extern_type;
	typedef _Statype state_type;

	result  in(_Statype& _State,
		const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1,
		_Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
		{	
		return (do_in(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  out(_Statype& _State,
		const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_out(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  unshift(_Statype& _State,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_unshift(_State,
			_First2, _Last2, _Mid2));
		}

	int  length(const _Statype& _State, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	
		return (do_length(_State, _First1, _Last1, _Count));
		}

	static  locale::id& __cdecl _Id_func();



	 static locale::id id;	
// #line 884 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

	explicit  codecvt(size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 codecvt(const _Locinfo& _Lobj, size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new codecvt<_Elem, _Byte, _Statype>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~codecvt()
		{	
		}

protected:
	void  _Init(const _Locinfo& _Lobj)
		{	
		_Cvt = _Lobj._Getcvt();
		}

	virtual result  do_in(_Statype& _State,
		const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1,
			_Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
		{	
		;
		;
		_Mid1 = _First1, _Mid2 = _First2;
		result _Ans = _Mid1 == _Last1 ? ok : partial;
		int _Bytes;

		while (_Mid1 != _Last1 && _Mid2 != _Last2)
			switch (_Bytes = _Mbrtowc(_Mid2, _Mid1, _Last1 - _Mid1,
				&_State, &_Cvt))
			{	
			case -2:	
				_Mid1 = _Last1;
				return (_Ans);

			case -1:	
				return (error);

			case 0:	
				if (*_Mid2 == (_Elem)0)
					_Bytes = (int)::strlen(_Mid1) + 1;
				

			default:	
				if (_Bytes == -3)
					_Bytes = 0;	
				_Mid1 += _Bytes;
				++_Mid2;
				_Ans = ok;
			}
		return (_Ans);
		}

	virtual result  do_out(_Statype& _State,
		const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1,
			_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		;
		;
		_Mid1 = _First1, _Mid2 = _First2;
		result _Ans = _Mid1 == _Last1 ? ok : partial;
		int _Bytes;

		while (_Mid1 != _Last1 && _Mid2 != _Last2)
			if ((int)___mb_cur_max_func() <= _Last2 - _Mid2)
				if ((_Bytes = _Wcrtomb(_Mid2, *_Mid1,
					&_State, &_Cvt)) < 0)
					return (error);	
				else
					++_Mid1, _Mid2 += _Bytes, _Ans = ok;
			else
				{	
				_Byte _Buf[5];
				_Statype _Stsave = _State;

				if ((_Bytes = _Wcrtomb(_Buf, *_Mid1,
					&_State, &_Cvt)) < 0)
					return (error);	
				else if (_Last2 - _Mid2 < _Bytes)
					{	
					_State = _Stsave;
					return (_Ans);
					}
				else
					{	
					::memcpy_s((_Mid2), (_Last2 - _Mid2), (_Buf), (_Bytes));
					++_Mid1, _Mid2 += _Bytes, _Ans = ok;
					}
				}
		return (_Ans);
		}

	virtual result  do_unshift(_Statype& _State,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		;
		_Mid2 = _First2;
		result _Ans = ok;
		int _Bytes;
		_Byte _Buf[5];
		_Statype _Stsave = _State;

		if ((_Bytes = _Wcrtomb(_Buf, L'\0', &_State, &_Cvt)) <= 0)
			_Ans = error;	
		else if (_Last2 - _Mid2 < --_Bytes)
			{	
			_State = _Stsave;
			_Ans = partial;
			}
		else if (0 < _Bytes)
			{	
			::memcpy_s((_Mid2), (_Last2 - _Mid2), (_Buf), (_Bytes));
			_Mid2 += _Bytes;
			}
		return (_Ans);
		}

	virtual int  do_length(const _Statype& _State, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	

 



// #line 1028 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
		;
		int _Wchars;
		const _Byte *_Mid1;
		_Statype _Mystate = _State;

		for (_Wchars = 0, _Mid1 = _First1;
			(size_t)_Wchars < _Count && _Mid1 != _Last1; )
			{	
			int _Bytes;
			_Elem _Ch;

			switch (_Bytes = _Mbrtowc(&_Ch, _Mid1, _Last1 - _Mid1,
				&_Mystate, &_Cvt))
				{	
			case -2:	
				return (_Wchars);

			case -1:	
				return (_Wchars);

			case 0:	
				if (_Ch == (_Elem)0)
					_Bytes = (int)::strlen(_Mid1) + 1;
				

			default:	
				if (_Bytes == -3)
					_Bytes = 0;	
				_Mid1 += _Bytes;
				++_Wchars;
				}
			}
		return (_Wchars);
 // #line 1062 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
		}

	virtual bool  do_always_noconv() const throw ()
		{	
		return (false);
		}

	virtual int  do_max_length() const throw ()
		{	
		return (5);
		}

private:
	_Locinfo::_Cvtvec _Cvt;	
	};


		
template<> class  codecvt<unsigned short, char, _Mbstatet>
	: public codecvt_base
	{	
public:
	typedef unsigned short _Elem;
	typedef char _Byte;
	typedef _Mbstatet _Statype;
	typedef _Elem intern_type;
	typedef _Byte extern_type;
	typedef _Statype state_type;

	result  in(_Statype& _State,
		const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1,
		_Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
		{	
		return (do_in(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  out(_Statype& _State,
		const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_out(_State,
			_First1, _Last1, _Mid1, _First2, _Last2, _Mid2));
		}

	result  unshift(_Statype& _State,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		return (do_unshift(_State,
			_First2, _Last2, _Mid2));
		}

	int  length(const _Statype& _State, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	
		return (do_length(_State, _First1, _Last1, _Count));
		}

	static  locale::id& __cdecl _Id_func();



	 static locale::id id;	
// #line 1126 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

	explicit  codecvt(size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 codecvt(const _Locinfo& _Lobj, size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new codecvt<_Elem, _Byte, _Statype>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~codecvt()
		{	
		}

protected:
	 codecvt(const char *_Locname, size_t _Refs = 0)
		: codecvt_base(_Refs)
		{	
		{ _Locinfo _Lobj(_Locname);
			_Init(_Lobj);
		}
		}

	void  _Init(const _Locinfo& _Lobj)
		{	
		_Cvt = _Lobj._Getcvt();
		}

	virtual result  do_in(_Statype& _State,
		const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1,
			_Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
		{	
		;
		;
		_Mid1 = _First1, _Mid2 = _First2;
		result _Ans = _Mid1 == _Last1 ? ok : partial;
		int _Bytes;

		while (_Mid1 != _Last1 && _Mid2 != _Last2)
			switch (_Bytes = _Mbrtowc((wchar_t *)_Mid2, _Mid1, _Last1 - _Mid1,
				&_State, &_Cvt))
			{	
			case -2:	
				_Mid1 = _Last1;
				return (_Ans);

			case -1:	
				return (error);

			case 0:	
				if (*_Mid2 == (_Elem)0)
					_Bytes = (int)::strlen(_Mid1) + 1;
				

			default:	
				if (_Bytes == -3)
					_Bytes = 0;	
				_Mid1 += _Bytes;
				++_Mid2;
				_Ans = ok;
			}
		return (_Ans);
		}

	virtual result  do_out(_Statype& _State,
		const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1,
			_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		;
		;
		_Mid1 = _First1, _Mid2 = _First2;
		result _Ans = _Mid1 == _Last1 ? ok : partial;
		int _Bytes;

		while (_Mid1 != _Last1 && _Mid2 != _Last2)
			if (5 <= _Last2 - _Mid2)
				if ((_Bytes = _Wcrtomb(_Mid2, *_Mid1,
					&_State, &_Cvt)) < 0)
					return (error);	
				else
					++_Mid1, _Mid2 += _Bytes, _Ans = ok;
			else
				{	
				_Byte _Buf[5];
				_Statype _Stsave = _State;

				if ((_Bytes = _Wcrtomb(_Buf, *_Mid1,
					&_State, &_Cvt)) < 0)
					return (error);	
				else if (_Last2 - _Mid2 < _Bytes)
					{	
					_State = _Stsave;
					return (_Ans);
					}
				else
					{	
					::memcpy_s((_Mid2), (_Last2 - _Mid2), (_Buf), (_Bytes));
					++_Mid1, _Mid2 += _Bytes, _Ans = ok;
					}
				}
		return (_Ans);
		}

	virtual result  do_unshift(_Statype& _State,
		_Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
		{	
		;
		_Mid2 = _First2;
		result _Ans = ok;
		int _Bytes;
		_Byte _Buf[5];
		_Statype _Stsave = _State;

		if ((_Bytes = _Wcrtomb(_Buf, L'\0', &_State, &_Cvt)) <= 0)
			_Ans = error;	
		else if (_Last2 - _Mid2 < --_Bytes)
			{	
			_State = _Stsave;
			_Ans = partial;
			}
		else if (0 < _Bytes)
			{	
			::memcpy_s((_Mid2), (_Last2 - _Mid2), (_Buf), (_Bytes));
			_Mid2 += _Bytes;
			}
		return (_Ans);
		}

	virtual int  do_length(const _Statype& _State, const _Byte *_First1,
		const _Byte *_Last1, size_t _Count) const
		{	
		;
		int _Wchars;
		const _Byte *_Mid1;
		_Statype _Mystate = _State;

		for (_Wchars = 0, _Mid1 = _First1;
			(size_t)_Wchars < _Count && _Mid1 != _Last1; )
			{	
			int _Bytes;
			_Elem _Ch;

			switch (_Bytes = _Mbrtowc((wchar_t *)&_Ch, _Mid1, _Last1 - _Mid1,
				&_Mystate, &_Cvt))
				{	
			case -2:	
				return (_Wchars);

			case -1:	
				return (_Wchars);

			case 0:	
				if (_Ch == (_Elem)0)
					_Bytes = (int)::strlen(_Mid1) + 1;
				

			default:	
				if (_Bytes == -3)
					_Bytes = 0;	
				_Mid1 += _Bytes;
				++_Wchars;
				}
			}
		return (_Wchars);
		}

	virtual bool  do_always_noconv() const throw ()
		{	
		return (false);
		}

	virtual int  do_max_length() const throw ()
		{	
		return (5);
		}

private:
	_Locinfo::_Cvtvec _Cvt;	
	};
// #line 1321 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

		
template<class _Elem,
	class _Byte,
	class _Statype>
	class codecvt_byname
		: public codecvt<_Elem, _Byte, _Statype>
	{	
public:
	explicit  codecvt_byname(const char *_Locname, size_t _Refs = 0)
		: codecvt<_Elem, _Byte, _Statype>(_Locname, _Refs)
		{	
		}

protected:
	virtual  ~codecvt_byname()
		{	
		}
	};

#pragma warning(push)
#pragma warning(disable:4275)
		
struct  ctype_base
	: public locale::facet
	{	
	enum
		{	
		alnum = 0x4|0x2|0x1|0x100, alpha = 0x2|0x1|0x100,
		cntrl = 0x20, digit = 0x4, graph = 0x4|0x2|0x10|0x1|0x100,
		lower = 0x2, print = 0x4|0x2|0x10|0x40|0x1|0x100|0x80,
		punct = 0x10, space = 0x8|0x40|0x000, upper = 0x1,
		xdigit = 0x80};
	typedef short mask;	

	 ctype_base(size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		}

	 ~ctype_base()
		{	
		}

protected:
	static void __cdecl _Xran()
		{	
		throw out_of_range("out_of_range in ctype<T>");
		}
	};
#pragma warning(pop)

		
template<class _Elem>
	class ctype
		: public ctype_base
	{	
public:
	typedef _Elem char_type;

	bool  is(mask _Maskval, _Elem _Ch) const
		{	
		return (do_is(_Maskval, _Ch));
		}

	const _Elem * is(const _Elem *_First, const _Elem *_Last,
		mask *_Dest) const
		{	
		return (do_is(_First, _Last, _Dest));
		}

	const _Elem * scan_is(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_is(_Maskval, _First, _Last));
		}

	const _Elem * scan_not(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_not(_Maskval, _First, _Last));
		}

	_Elem  tolower(_Elem _Ch) const
		{	
		return (do_tolower(_Ch));
		}

	const _Elem * tolower(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_tolower(_First, _Last));
		}

	_Elem  toupper(_Elem _Ch) const
		{	
		return (do_toupper(_Ch));
		}

	const _Elem * toupper(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_toupper(_First, _Last));
		}

	_Elem  widen(char _Byte) const
		{	
		return (do_widen(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const char * widen(const char *_First, const char *_Last,
		_Elem *_Dest) const
		{	
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_widen(_First, _Last, _Dest));
#pragma warning(pop)
		}

	const char * _Widen_s(const char *_First, const char *_Last,
		_Elem *_Dest, size_t _Dest_size) const
		{	
		return (_Do_widen_s(_First, _Last, _Dest, _Dest_size));
		}

	char  narrow(_Elem _Ch, char _Dflt = '\0') const
		{	
		return (do_narrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const _Elem * narrow(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_narrow(_First, _Last, _Dflt, _Dest));
#pragma warning(pop)
		}

	const _Elem * _Narrow_s(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, size_t _Dest_size) const
		{	
		return (_Do_narrow_s(_First, _Last, _Dflt, _Dest, _Dest_size));
		}

	 static locale::id id;	

	explicit  ctype(size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 ctype(const _Locinfo& _Lobj, size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new ctype<_Elem>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~ctype()
		{	
		if (_Ctype._Delfl)
			free((void *)_Ctype._Table);
		}

protected:
	void  _Init(const _Locinfo& _Lobj)
		{	
		_Ctype = _Lobj._Getctype();
		}

	virtual bool  do_is(mask _Maskval, _Elem _Ch) const
		{	
		return ((_Ctype._Table[(unsigned char)narrow(_Ch)]
			& _Maskval) != 0);
		}

	virtual const _Elem * do_is(const _Elem *_First, const _Elem *_Last,
		mask *_Dest) const
		{	
		;
		;
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Ctype._Table[(unsigned char)narrow(*_First)];
		return (_First);
		}

	virtual const _Elem * do_scan_is(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && !is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual const _Elem * do_scan_not(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual _Elem  do_tolower(_Elem _Ch) const
		{	
		unsigned char _Byte = (unsigned char)narrow(_Ch, '\0');
		if (_Byte == '\0')
			return (_Ch);
		else
			return (widen((char)_Tolower(_Byte, &_Ctype)));
		}

	virtual const _Elem * do_tolower(_Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			{	
			unsigned char _Byte = (unsigned char)narrow(*_First, '\0');
			if (_Byte != '\0')
				*_First = (widen((char)_Tolower(_Byte, &_Ctype)));
			}
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_toupper(_Elem _Ch) const
		{	
		unsigned char _Byte = (unsigned char)narrow(_Ch, '\0');
		if (_Byte == '\0')
			return (_Ch);
		else
			return (widen((char)_Toupper(_Byte, &_Ctype)));
		}

	virtual const _Elem * do_toupper(_Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			{	
			unsigned char _Byte = (unsigned char)narrow(*_First, '\0');
			if (_Byte != '\0')
				*_First = (widen((char)_Toupper(_Byte, &_Ctype)));
			}
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_widen(char _Byte) const
		{	
		return (_Maklocchr(_Byte, (_Elem *)0, _Cvt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const char * do_widen(const char *_First,
		const char *_Last, _Elem *_Dest) const
		{	
		
		return _Do_widen_s(_First, _Last, _Dest, _Last - _First);
		}

	virtual const char * _Do_widen_s(const char *_First,
		const char *_Last, _Elem *_Dest, size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Maklocchr(*_First, (_Elem *)0, _Cvt);
		return (_First);
		}

	char  _Donarrow(_Elem _Ch, char _Dflt) const
		{	
		char _Byte;
		if (_Ch == (_Elem)0)
			return ('\0');
		else if ((_Byte = _Maklocbyte((_Elem)_Ch, _Cvt)) == '\0')
			return (_Dflt);
		else
			return (_Byte);
		}

	virtual char  do_narrow(_Elem _Ch, char _Dflt) const
		{	
		return (_Donarrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const _Elem * do_narrow(const _Elem *_First,
		const _Elem *_Last, char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
		return _Do_narrow_s(_First, _Last, _Dflt, _Dest, _Last - _First);
		}

	virtual const _Elem * _Do_narrow_s(const _Elem *_First,
		const _Elem *_Last, char _Dflt, 
		[SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Donarrow(*_First, _Dflt);
		return (_First);
		}

private:
	_Locinfo::_Ctypevec _Ctype;	
	_Locinfo::_Cvtvec _Cvt;		
	};

		
template<class _Elem>
	 locale::id ctype<_Elem>::id;

		
template<> class  ctype<char>
	: public ctype_base
	{	
	typedef ctype<char> _Myt;

public:
	typedef char _Elem;
	typedef _Elem char_type;

	bool  is(mask _Maskval, _Elem _Ch) const
		{	
		return ((_Ctype._Table[(unsigned char)_Ch] & _Maskval) != 0);
		}

	const _Elem * is(const _Elem *_First,
		const _Elem *_Last, mask *_Dest) const
		{	
		;
		;
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Ctype._Table[(unsigned char)*_First];
		return (_First);
		}

	const _Elem * scan_is(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && !is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	const _Elem * scan_not(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	_Elem  tolower(_Elem _Ch) const
		{	
		return (do_tolower(_Ch));
		}

	const _Elem * tolower(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_tolower(_First, _Last));
		}

	_Elem  toupper(_Elem _Ch) const
		{	
		return (do_toupper(_Ch));
		}

	const _Elem * toupper(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_toupper(_First, _Last));
		}

	_Elem  widen(char _Byte) const
		{	
		return (do_widen(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const _Elem * widen(const char *_First, const char *_Last,
		_Elem *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_widen(_First, _Last, _Dest));
#pragma warning(pop)
		}

	const _Elem * _Widen_s(const char *_First, const char *_Last,
		_Elem *_Dest, size_t _Dest_size) const
		{	
		return (_Do_widen_s(_First, _Last, _Dest, _Dest_size));
		}

	_Elem  narrow(_Elem _Ch, char _Dflt = '\0') const
		{	
		return (do_narrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const _Elem * narrow(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_narrow(_First, _Last, _Dflt, _Dest));
#pragma warning(pop)
		}

	const _Elem * _Narrow_s(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		return (_Do_narrow_s(_First, _Last, _Dflt, _Dest, _Dest_size));
		}

	static  locale::id& __cdecl _Id_func();



	 static locale::id id;	
// #line 1765 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

	explicit  ctype(const mask *_Table = 0,
		bool _Deletetable = false,
		size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		if (_Table != 0)
			{	
			_Tidy();
			_Ctype._Table = _Table;
			_Ctype._Delfl = _Deletetable ? -1 : 0;
			}
		}

	 ctype(const _Locinfo& _Lobj, size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new ctype<_Elem>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

	 static const size_t table_size = 1 << 8;	

protected:
	virtual  ~ctype()
		{	
		_Tidy();
		}

protected:
	void  _Init(const _Locinfo& _Lobj)
		{	
		_Ctype = _Lobj._Getctype();
		}

	void  _Tidy()
		{	
		if (0 < _Ctype._Delfl)
			free((void *)_Ctype._Table);
		else if (_Ctype._Delfl < 0)
			delete[] (void *)_Ctype._Table;
		}

	virtual _Elem  do_tolower(_Elem _Ch) const
		{	
		return ((_Elem)_Tolower((unsigned char)_Ch, &_Ctype));
		}

	virtual const _Elem * do_tolower(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = (_Elem)_Tolower((unsigned char)*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_toupper(_Elem _Ch) const
		{	
		return ((_Elem)_Toupper((unsigned char)_Ch, &_Ctype));
		}

	virtual const _Elem * do_toupper(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = (_Elem)_Toupper((unsigned char)*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_widen(char _Byte) const
		{	
		return (_Byte);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const _Elem * do_widen(const char *_First,
		const char *_Last, _Elem *_Dest) const
		{	
		
		return _Do_widen_s(_First, _Last, _Dest, _Last - _First);
		}

	virtual const _Elem * _Do_widen_s(const char *_First,
		const char *_Last, _Elem *_Dest, size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		::memcpy_s((_Dest), (_Dest_size), (_First), (_Last - _First));
		return (_Last);
		}

	virtual _Elem  do_narrow(_Elem _Ch, char) const
		{	
		return (_Ch);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const _Elem * do_narrow(const _Elem *_First,
		const _Elem *_Last, char _Dflt, 
		[SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
		return _Do_narrow_s(_First, _Last, _Dflt, _Dest, _Last - _First);
		}

	virtual const _Elem * _Do_narrow_s(const _Elem *_First,
		const _Elem *_Last, char, [SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		::memcpy_s((_Dest), (_Dest_size), (_First), (_Last - _First));
		return (_Last);
		}

	const mask * table() const throw ()
		{	
		return (_Ctype._Table);
		}

	static const mask *__cdecl classic_table() throw ()
		{	
		const _Myt& _Ctype_fac = use_facet<_Myt >(locale::classic());
		return (_Ctype_fac.table());
		}

private:
	_Locinfo::_Ctypevec _Ctype;	
	};

		
template<> class  ctype<wchar_t>
	: public ctype_base
	{	
	typedef ctype<wchar_t> _Myt;

public:
	typedef wchar_t _Elem;
	typedef _Elem char_type;

	bool  is(mask _Maskval, _Elem _Ch) const
		{	
		return (do_is(_Maskval, _Ch));
		}

	const _Elem * is(const _Elem *_First, const _Elem *_Last,
		mask *_Dest) const
		{	
		return (do_is(_First, _Last, _Dest));
		}

	const _Elem * scan_is(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_is(_Maskval, _First, _Last));
		}

	const _Elem * scan_not(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_not(_Maskval, _First, _Last));
		}

	_Elem  tolower(_Elem _Ch) const
		{	
		return (do_tolower(_Ch));
		}

	const _Elem * tolower(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_tolower(_First, _Last));
		}

	_Elem  toupper(_Elem _Ch) const
		{	
		return (do_toupper(_Ch));
		}

	const _Elem * toupper(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_toupper(_First, _Last));
		}

	_Elem  widen(char _Byte) const
		{	
		return (do_widen(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const char * widen(const char *_First, const char *_Last,
		_Elem *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_widen(_First, _Last, _Dest));
#pragma warning(pop)
		}

	const char * _Widen_s(const char *_First, const char *_Last,
		_Elem *_Dest, size_t _Dest_size) const
		{	
		return (_Do_widen_s(_First, _Last, _Dest, _Dest_size));
		}

	char  narrow(_Elem _Ch, char _Dflt = '\0') const
		{	
		return (do_narrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const _Elem * narrow(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_narrow(_First, _Last, _Dflt, _Dest));
#pragma warning(pop)
		}

	const _Elem * _Narrow_s(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		return (_Do_narrow_s(_First, _Last, _Dflt, _Dest, _Dest_size));
		}

	static  locale::id& __cdecl _Id_func();



	 static locale::id id;	
// #line 2014 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

	explicit  ctype(size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 ctype(const _Locinfo& _Lobj, size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new ctype<_Elem>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~ctype()
		{	
		if (_Ctype._Delfl)
			free((void *)_Ctype._Table);
		}

protected:
	void  _Init(const _Locinfo& _Lobj)
		{	
		_Ctype = _Lobj._Getctype();
		_Cvt = _Lobj._Getcvt();
		}

	virtual bool  do_is(mask _Maskval, _Elem _Ch) const
		{	
		return ((::_Getwctype(_Ch, &_Ctype) & _Maskval) != 0);
		}

	virtual const wchar_t * do_is(const _Elem *_First,
		const _Elem *_Last, mask *_Dest) const
		{	
		;
		;
		return (::_Getwctypes(_First, _Last, _Dest, &_Ctype));
		}

	virtual const _Elem * do_scan_is(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && !is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual const _Elem * do_scan_not(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual _Elem  do_tolower(_Elem _Ch) const
		{	
		return (_Towlower(_Ch, &_Ctype));
		}

	virtual const _Elem * do_tolower(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = _Towlower(*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_toupper(_Elem _Ch) const
		{	
		return (_Towupper(_Ch, &_Ctype));
		}

	virtual const _Elem * do_toupper(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = _Towupper(*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	_Elem  _Dowiden(char _Byte) const
		{	
		mbstate_t _Mbst = {0};
		wchar_t _Wc;
		return (_Mbrtowc(&_Wc, &_Byte, 1, &_Mbst, &_Cvt) < 0
			? (wchar_t)(wint_t)(0xFFFF) : _Wc);
		}

	virtual _Elem  do_widen(char _Byte) const
		{	
		return (_Dowiden(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const char * do_widen(const char *_First,
		const char *_Last, _Elem *_Dest) const
		{	
		
		return _Do_widen_s(_First, _Last, _Dest, _Last - _First);
		}

	virtual const char * _Do_widen_s(const char *_First,
		const char *_Last, _Elem *_Dest, size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Dowiden(*_First);
		return (_First);
		}

	char  _Donarrow(_Elem _Ch, char _Dflt) const
		{	
		char _Buf[5];
		mbstate_t _Mbst = {0};
		return (_Wcrtomb(_Buf, _Ch, &_Mbst, &_Cvt) != 1
			? _Dflt : _Buf[0]);
		}

	virtual char  do_narrow(_Elem _Ch, char _Dflt) const
		{	
		return (_Donarrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const _Elem * do_narrow(const _Elem *_First,
		const _Elem *_Last, char _Dflt,  
		[SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
		return _Do_narrow_s(_First, _Last, _Dflt, _Dest, _Last - _First);
		}

	virtual const _Elem * _Do_narrow_s(const _Elem *_First,
		const _Elem *_Last, char _Dflt, 
		[SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Donarrow(*_First, _Dflt);
		return (_First);
		}

private:
	_Locinfo::_Ctypevec _Ctype;	
	_Locinfo::_Cvtvec _Cvt;		
	};


		
template<> class  ctype<unsigned short>
	: public ctype_base
	{	
	typedef ctype<unsigned short> _Myt;

public:
	typedef unsigned short _Elem;
	typedef _Elem char_type;

	bool  is(mask _Maskval, _Elem _Ch) const
		{	
		return (do_is(_Maskval, _Ch));
		}

	const _Elem * is(const _Elem *_First, const _Elem *_Last,
		mask *_Dest) const
		{	
		return (do_is(_First, _Last, _Dest));
		}

	const _Elem * scan_is(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_is(_Maskval, _First, _Last));
		}

	const _Elem * scan_not(mask _Maskval, const _Elem *_First,
		const _Elem *_Last) const
		{	
		return (do_scan_not(_Maskval, _First, _Last));
		}

	_Elem  tolower(_Elem _Ch) const
		{	
		return (do_tolower(_Ch));
		}

	const _Elem * tolower(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_tolower(_First, _Last));
		}

	_Elem  toupper(_Elem _Ch) const
		{	
		return (do_toupper(_Ch));
		}

	const _Elem * toupper(_Elem *_First, const _Elem *_Last) const
		{	
		return (do_toupper(_First, _Last));
		}

	_Elem  widen(char _Byte) const
		{	
		return (do_widen(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const char * widen(const char *_First, const char *_Last,
		_Elem *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_widen(_First, _Last, _Dest));
#pragma warning(pop)
		}

	const char * _Widen_s(const char *_First, const char *_Last,
		_Elem *_Dest, size_t _Dest_size) const
		{	
		return (_Do_widen_s(_First, _Last, _Dest, _Dest_size));
		}

	char  narrow(_Elem _Ch, char _Dflt = '\0') const
		{	
		return (do_narrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	const _Elem * narrow(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
#pragma warning(push)
#pragma warning(disable:4996)
		return (do_narrow(_First, _Last, _Dflt, _Dest));
#pragma warning(pop)
		}

	const _Elem * _Narrow_s(const _Elem *_First, const _Elem *_Last,
		char _Dflt, [SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		return (_Do_narrow_s(_First, _Last, _Dflt, _Dest, _Dest_size));
		}

	static  locale::id& __cdecl _Id_func();



	 static locale::id id;	
// #line 2288 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

	explicit  ctype(size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 ctype(const _Locinfo& _Lobj, size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		_Init(_Lobj);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new ctype<_Elem>(
				_Locinfo(_Ploc->name()));
		return (2);
		}

protected:
	virtual  ~ctype()
		{	
		if (_Ctype._Delfl)
			free((void *)_Ctype._Table);
		}

protected:
	 ctype(const char *_Locname, size_t _Refs = 0)
		: ctype_base(_Refs)
		{	
		{ _Locinfo _Lobj(_Locname);
			_Init(_Lobj);
		}
		}

	void  _Init(const _Locinfo& _Lobj)
		{	
		_Ctype = _Lobj._Getctype();
		_Cvt = _Lobj._Getcvt();
		}

	virtual bool  do_is(mask _Maskval, _Elem _Ch) const
		{	
		return ((::_Getwctype(_Ch, &_Ctype) & _Maskval) != 0);
		}

	virtual const _Elem * do_is(const _Elem *_First,
		const _Elem *_Last, mask *_Dest) const
		{	
		;
		;
		return ((const _Elem *)::_Getwctypes((const wchar_t *)_First,
			(const wchar_t *)_Last, _Dest, &_Ctype));
		}

	virtual const _Elem * do_scan_is(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && !is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual const _Elem * do_scan_not(mask _Maskval,
		const _Elem *_First, const _Elem *_Last) const
		{	
		;
		for (; _First != _Last && is(_Maskval, *_First); ++_First)
			;
		return (_First);
		}

	virtual _Elem  do_tolower(_Elem _Ch) const
		{	
		return (_Towlower(_Ch, &_Ctype));
		}

	virtual const _Elem * do_tolower(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = _Towlower(*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	virtual _Elem  do_toupper(_Elem _Ch) const
		{	
		return (_Towupper(_Ch, &_Ctype));
		}

	virtual const _Elem * do_toupper(_Elem *_First,
		const _Elem *_Last) const
		{	
		;
		for (; _First != _Last; ++_First)
			*_First = _Towupper(*_First, &_Ctype);
		return ((const _Elem *)_First);
		}

	_Elem  _Dowiden(char _Byte) const
		{	
		mbstate_t _Mbst = {0};
		unsigned short _Wc;
		return (_Mbrtowc((wchar_t *)&_Wc, &_Byte, 1, &_Mbst, &_Cvt) < 0
			? (unsigned short)(wint_t)(0xFFFF) : _Wc);
		}

	virtual _Elem  do_widen(char _Byte) const
		{	
		return (_Dowiden(_Byte));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const char * do_widen(const char *_First,
		const char *_Last, _Elem *_Dest) const
		{	
		
		return _Do_widen_s(_First, _Last, _Dest, _Last - _First);
		}

	virtual const char * _Do_widen_s(const char *_First,
		const char *_Last, _Elem *_Dest, size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Dowiden(*_First);
		return (_First);
		}

	char  _Donarrow(_Elem _Ch, char _Dflt) const
		{	
		char _Buf[5];
		mbstate_t _Mbst = {0};
		return (_Wcrtomb(_Buf, _Ch, &_Mbst, &_Cvt) != 1
			? _Dflt : _Buf[0]);
		}

	virtual char  do_narrow(_Elem _Ch, char _Dflt) const
		{	
		return (_Donarrow(_Ch, _Dflt));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual const _Elem * do_narrow(const _Elem *_First,
		const _Elem *_Last, char _Dflt, [SA_Pre(Null=SA_No,WritableElements="\n@""_Last-_First")] [SA_Pre(Deref=1,Valid=SA_No)] char *_Dest) const
		{	
		
		return _Do_narrow_s(_First, _Last, _Dflt, _Dest, _Last - _First);
		}

	virtual const _Elem * _Do_narrow_s(const _Elem *_First,
		const _Elem *_Last, char _Dflt, 
		[SA_Pre(Null=SA_No,WritableElements="_Dest_size")] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(ValidElements="\n@""_Last-_First")] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Dest, 
		size_t _Dest_size) const
		{	
		;
		;
		{ if (!(_Dest_size >= (size_t)(_Last - _First))) { ((void)0); ::_invalid_parameter_noinfo(); } ; };
		for (; _First != _Last; ++_First, ++_Dest)
			*_Dest = _Donarrow(*_First, _Dflt);
		return (_First);
		}

private:
	_Locinfo::_Ctypevec _Ctype;	
	_Locinfo::_Cvtvec _Cvt;		
	};
// #line 2465 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

		
template<class _Elem>
	class ctype_byname
	: public ctype<_Elem>
	{	
public:
	explicit  ctype_byname(const char *_Locname, size_t _Refs = 0)
		: ctype<_Elem>(_Locname, _Refs)
		{	
		}

protected:
	virtual  ~ctype_byname()
		{	
		}
	};

		
template<> class ctype_byname<char>
	: public ctype<char>
	{	
public:
	explicit  ctype_byname(const char *_Locname, size_t _Refs = 0)
		: ctype<char>(_Locname, _Refs)
		{	
		}

protected:
	virtual  ~ctype_byname()
		{	
		}
	};

 



// #line 2504 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
}


 #pragma warning(pop)
 #pragma pack(pop)
// #line 2510 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"

// #line 2512 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"
// #line 2513 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocale"






// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"
// #line 1 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\share.h"













#pragma once
// #line 16 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\share.h"






// #line 23 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\share.h"














// #line 38 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\share.h"

// #line 40 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\share.h"
// #line 8 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"


 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable:4412)
// #line 14 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"

namespace std {

 
 
 
 
 
 
 
 
 
 
 
 
 
 

 

 
 

 
 
 

		
template<class _Dummy>
	class _Iosb
	{	
public:
	enum _Dummy_enum {_Dummy_enum_val = 1};	

	enum _Fmtflags
		{	
		_Fmtmask = 0xffff, _Fmtzero = 0};

	static const _Fmtflags skipws = (_Fmtflags)0x0001;
	static const _Fmtflags unitbuf = (_Fmtflags)0x0002;
	static const _Fmtflags uppercase = (_Fmtflags)0x0004;
	static const _Fmtflags showbase = (_Fmtflags)0x0008;
	static const _Fmtflags showpoint = (_Fmtflags)0x0010;
	static const _Fmtflags showpos = (_Fmtflags)0x0020;
	static const _Fmtflags left = (_Fmtflags)0x0040;
	static const _Fmtflags right = (_Fmtflags)0x0080;
	static const _Fmtflags internal = (_Fmtflags)0x0100;
	static const _Fmtflags dec = (_Fmtflags)0x0200;
	static const _Fmtflags oct = (_Fmtflags)0x0400;
	static const _Fmtflags hex = (_Fmtflags)0x0800;
	static const _Fmtflags scientific = (_Fmtflags)0x1000;
	static const _Fmtflags fixed = (_Fmtflags)0x2000;

	static const _Fmtflags hexfloat =
		(_Fmtflags)0x3000;	

	static const _Fmtflags boolalpha = (_Fmtflags)0x4000;
	static const _Fmtflags _Stdio = (_Fmtflags)0x8000;
	static const _Fmtflags adjustfield = (_Fmtflags)(0x0040
		| 0x0080 | 0x0100);
	static const _Fmtflags basefield = (_Fmtflags)(0x0200
		| 0x0400 | 0x0800);
	static const _Fmtflags floatfield = (_Fmtflags)(0x1000
		| 0x2000);

	enum _Iostate
		{	
		_Statmask = 0x17};

	static const _Iostate goodbit = (_Iostate)0x0;
	static const _Iostate eofbit = (_Iostate)0x1;
	static const _Iostate failbit = (_Iostate)0x2;
	static const _Iostate badbit = (_Iostate)0x4;
	static const _Iostate _Hardfail = (_Iostate)0x10;

	enum _Openmode
		{	
		_Openmask = 0xff};

	static const _Openmode in = (_Openmode)0x01;
	static const _Openmode out = (_Openmode)0x02;
	static const _Openmode ate = (_Openmode)0x04;
	static const _Openmode app = (_Openmode)0x08;
	static const _Openmode trunc = (_Openmode)0x10;
	static const _Openmode _Nocreate = (_Openmode)0x40;
	static const _Openmode _Noreplace = (_Openmode)0x80;
	static const _Openmode binary = (_Openmode)0x20;

	enum _Seekdir
		{	
		_Seekmask = 0x3};
	static const _Seekdir beg = (_Seekdir)0;
	static const _Seekdir cur = (_Seekdir)1;
	static const _Seekdir end = (_Seekdir)2;

	enum
		{	
		_Openprot = 0x40};
	};

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::skipws;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::unitbuf;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::uppercase;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::showbase;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::showpoint;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::showpos;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::left;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::right;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::internal;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::dec;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::oct;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::hex;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::scientific;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::fixed;

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags
		_Iosb<_Dummy>::hexfloat;	

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::boolalpha;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::_Stdio;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::adjustfield;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::basefield;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Fmtflags _Iosb<_Dummy>::floatfield;

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::goodbit;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::eofbit;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::failbit;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::badbit;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::_Hardfail;

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::in;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::out;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::ate;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::app;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::trunc;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::_Nocreate;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::_Noreplace;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::binary;

template<class _Dummy>
	const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::beg;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::cur;
template<class _Dummy>
	const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::end;

		
class  ios_base
	: public _Iosb<int>
	{	
public:
	typedef int fmtflags;
	typedef int iostate;
	typedef int openmode;
	typedef int seekdir;

	typedef std::streamoff streamoff;
	typedef std::streampos streampos;

	enum event
		{	
		erase_event, imbue_event, copyfmt_event};

	typedef void (__cdecl *event_callback)(event, ios_base&, int);
	typedef unsigned int io_state, open_mode, seek_dir;

			
	class failure
		: public runtime_error
		{	
	public:
		explicit  failure(const string &_Message)
			: runtime_error(_Message)
			{	
			}

		virtual  ~failure() throw ()
			{	
			}








// #line 235 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"

		};

			
	class  Init
		{	
	public:
		 Init()
            {
            _Init_ctor(this);
            }

		 ~Init()
            {
            _Init_dtor(this);
            }

	private:
        static  void __cdecl _Init_ctor(Init *);
        static  void __cdecl _Init_dtor(Init *);

		 static int& __cdecl _Init_cnt_func();	



		 static int _Init_cnt;	
// #line 262 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"
		};

	ios_base&  operator=(const ios_base& _Right)
		{	
		if (this != &_Right)
			{	
			_Mystate = _Right._Mystate;
			copyfmt(_Right);
			}
		return (*this);
		}

	 operator void *() const
		{	
		return (fail() ? 0 : (void *)this);
		}

	bool  operator!() const
		{	
		return (fail());
		}

    void  clear(iostate _State, bool _Reraise)
        {	
        _Mystate = (iostate)(_State & _Statmask);
        if ((_Mystate & _Except) == 0)
            ;
        else if (_Reraise)
            throw;
        else if (_Mystate & _Except & badbit)
            throw failure("ios_base::badbit set");
        else if (_Mystate & _Except & failbit)
            throw failure("ios_base::failbit set");
        else
            throw failure("ios_base::eofbit set");
        }

	void  clear(iostate _State = goodbit)
		{	
		clear(_State, false);
		}

	void  clear(io_state _State)
		{	
		clear((iostate)_State);
		}

	iostate  rdstate() const
		{	
		return (_Mystate);
		}


	void  setstate(iostate _State, bool _Exreraise)
		{	
		if (_State != goodbit)
			clear((iostate)((int)rdstate() | (int)_State), _Exreraise);
		}

	void  setstate(iostate _State)
		{	
		if (_State != goodbit)
			clear((iostate)((int)rdstate() | (int)_State), false);
		}

	void  setstate(io_state _State)
		{	
		setstate((iostate)_State);
		}

	bool  good() const
		{	
		return (rdstate() == goodbit);
		}

	bool  eof() const
		{	
		return ((int)rdstate() & (int)eofbit);
		}

	bool  fail() const
		{	
		return (((int)rdstate()
			& ((int)badbit | (int)failbit)) != 0);
		}

	bool  bad() const
		{	
		return (((int)rdstate() & (int)badbit) != 0);
		}

	
	
	
	iostate  exceptions() const
		{	
		return (_Except);
		}

	void  exceptions(iostate _Newexcept)
		{	
		_Except = (iostate)((int)_Newexcept & (int)_Statmask);
		clear(_Mystate);
		}

	void  exceptions(io_state _State)
		{	
		exceptions((iostate)_State);
		}

	fmtflags  flags() const
		{	
		return (_Fmtfl);
		}

	fmtflags  flags(fmtflags _Newfmtflags)
		{	
		fmtflags _Oldfmtflags = _Fmtfl;
		_Fmtfl = (fmtflags)((int)_Newfmtflags & (int)_Fmtmask);
		return (_Oldfmtflags);
		}

	fmtflags  setf(fmtflags _Newfmtflags)
		{	
		ios_base::fmtflags _Oldfmtflags = _Fmtfl;
		_Fmtfl = (fmtflags)((int)_Fmtfl
			| (int)_Newfmtflags & (int)_Fmtmask);
		return (_Oldfmtflags);
		}

	fmtflags  setf(fmtflags _Newfmtflags, fmtflags _Mask)
		{	
		ios_base::fmtflags _Oldfmtflags = _Fmtfl;
		_Fmtfl = (fmtflags)(((int)_Fmtfl & (int)~_Mask)
			| ((int)_Newfmtflags & (int)_Mask & (int)_Fmtmask));
		return (_Oldfmtflags);
		}

	void  unsetf(fmtflags _Mask)
		{	
		_Fmtfl = (fmtflags)((int)_Fmtfl & (int)~_Mask);
		}

	streamsize  precision() const
		{	
		return (_Prec);
		}

	streamsize  precision(streamsize _Newprecision)
		{	
		streamsize _Oldprecision = _Prec;
		_Prec = _Newprecision;
		return (_Oldprecision);
		}

	streamsize  width() const
		{	
		return (_Wide);
		}

	streamsize  width(streamsize _Newwidth)
		{	
		streamsize _Oldwidth = _Wide;
		_Wide = _Newwidth;
		return (_Oldwidth);
		}

	locale  getloc() const
		{	
		return (*_Ploc);
		}

	locale  imbue(const locale& _Loc)
        {	
        locale _Oldlocale = *_Ploc;
        *_Ploc = _Loc;
        _Callfns(imbue_event);
        return (_Oldlocale);
        }

	static int __cdecl xalloc()
		{	
		{ ::std:: _Lockit _Lock(2);	
			return (_Index++);
		}
		}

	long&  iword(int _Idx)
		{	
		return (_Findarr(_Idx)._Lo);
		}

	void *&  pword(int _Idx)
		{	
		return (_Findarr(_Idx)._Vp);
		}

	void  register_callback(event_callback _Pfn, int _Idx)
        {	
        _Calls = new _Fnarray(_Idx, _Pfn, _Calls);
        }

	ios_base&  copyfmt(const ios_base& _Other)
        {	
        if (this != &_Other)
            {	
            _Tidy();
            *_Ploc = *_Other._Ploc;
            _Fmtfl = _Other._Fmtfl;
            _Prec = _Other._Prec;
            _Wide = _Other._Wide;
            _Iosarray *_Ptr = _Other._Arr;

            for (_Arr = 0; _Ptr != 0; _Ptr = _Ptr->_Next)
                if (_Ptr->_Lo != 0 || _Ptr->_Vp != 0)
                    {	
                    iword(_Ptr->_Index) = _Ptr->_Lo;
                    pword(_Ptr->_Index) = _Ptr->_Vp;
                    }

            for (_Fnarray *_Q = _Other._Calls; _Q != 0; _Q = _Q->_Next)
                register_callback(_Q->_Pfn, _Q->_Index);	

            _Callfns(copyfmt_event);	
            exceptions(_Other._Except);	
            }
        return (*this);
        }


	virtual  ~ios_base()
        {
        _Ios_base_dtor(this);
        }

	static bool __cdecl sync_with_stdio(bool _Newsync = true)
		{	
		{ ::std:: _Lockit _Lock(2);	
			const bool _Oldsync = _Sync;
			_Sync = _Newsync;
			return (_Oldsync);
		}
		}

	static  void __cdecl _Addstd(ios_base *);
	size_t _Stdstr;	

protected:
	 ios_base()
		{	
		}

	void  _Init()
        {	
        _Ploc = 0;
        _Stdstr = 0;
        _Except = goodbit;
        _Fmtfl = skipws | dec;
        _Prec = 6;
        _Wide = 0;
        _Arr = 0;
        _Calls = 0;
        clear(goodbit);
        _Ploc = new locale;
        }

private:
			
	struct _Iosarray
		{	
	public:
		 _Iosarray(int _Idx, _Iosarray *_Link)
			: _Next(_Link), _Index(_Idx), _Lo(0), _Vp(0)
			{	
			}

		_Iosarray *_Next;	
		int _Index;	
		long _Lo;	
		void *_Vp;	
		};

			
	struct _Fnarray
		{	
		 _Fnarray(int _Idx, event_callback _Pnew, _Fnarray *_Link)
			: _Next(_Link), _Index(_Idx), _Pfn(_Pnew)
			{	
			}

		_Fnarray *_Next;	
		int _Index;	
		event_callback _Pfn;	
		};

	void  _Callfns(event _Ev)
        {	
        for (_Fnarray *_Ptr = _Calls; _Ptr != 0; _Ptr = _Ptr->_Next)
            (*_Ptr->_Pfn)(_Ev, *this, _Ptr->_Index);
        }

	_Iosarray&  _Findarr(int _Idx)
        {	
        _Iosarray *_Ptr, *_Q;

        for (_Ptr = _Arr, _Q = 0; _Ptr != 0; _Ptr = _Ptr->_Next)
            if (_Ptr->_Index == _Idx)
                return (*_Ptr);	
            else if (_Q == 0 && _Ptr->_Lo == 0 && _Ptr->_Vp == 0)
                _Q = _Ptr;	

        if (_Q != 0)
            {	
            _Q->_Index = _Idx;
            return (*_Q);
            }

        _Arr = new _Iosarray(_Idx, _Arr);	
        return (*_Arr);
        }

	void __cdecl _Tidy()
        {	
        _Callfns(erase_event);
        _Iosarray *_Q1, *_Q2;

        for (_Q1 = _Arr; _Q1 != 0; _Q1 = _Q2)
            _Q2 = _Q1->_Next, delete (_Q1);	
        _Arr = 0;

        _Fnarray *_Q3, *_Q4;
        for (_Q3 = _Calls; _Q3 != 0; _Q3 = _Q4)
            _Q4 = _Q3->_Next, delete (_Q3);	
        _Calls = 0;
        }

    static  void __cdecl _Ios_base_dtor(ios_base *);

	iostate _Mystate;	
	iostate _Except;	
	fmtflags _Fmtfl;	
	streamsize _Prec;	
	streamsize _Wide;	
	_Iosarray *_Arr;	
	_Fnarray *_Calls;	
	locale *_Ploc;	




// #line 613 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"
	 static int _Index;	
	 static bool _Sync;	
// #line 616 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"
    static  int& __cdecl _Index_func();
    static  bool& __cdecl _Sync_func();
	};





}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 629 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"

// #line 631 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"
// #line 632 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xiosbase"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"
namespace std {

		
template<class _Elem,
	class _Traits>
	class basic_streambuf
	{	

	basic_streambuf(const basic_streambuf<_Elem, _Traits>&);	
	basic_streambuf<_Elem, _Traits>&
		operator=(const basic_streambuf<_Elem, _Traits>&);	


protected:
	basic_streambuf()
		: _Plocale(new(locale))
		{	
		_Init();
		}

	basic_streambuf(_Uninitialized)
		{	
		}

public:
	typedef basic_streambuf<_Elem, _Traits> _Myt;
	typedef _Elem char_type;
	typedef _Traits traits_type;

	virtual  ~basic_streambuf()
		{	
		delete (_Plocale);
		}

	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;

	pos_type pubseekoff(off_type _Off, ios_base::seekdir _Way,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	
		return (seekoff(_Off, _Way, _Mode));
		}

	pos_type pubseekoff(off_type _Off, ios_base::seek_dir _Way,
		ios_base::open_mode _Mode)
		{	
		return (pubseekoff(_Off, (ios_base::seekdir)_Way,
			(ios_base::openmode)_Mode));
		}

	pos_type pubseekpos(pos_type _Pos,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	
		return (seekpos(_Pos, _Mode));
		}

	pos_type pubseekpos(pos_type _Pos, ios_base::open_mode _Mode)
		{	
		return (seekpos(_Pos, (ios_base::openmode)_Mode));
		}

	_Myt *pubsetbuf(_Elem *_Buffer, streamsize _Count)
		{	
		return (setbuf(_Buffer, _Count));
		}

	locale pubimbue(const locale &_Newlocale)
		{	
		locale _Oldlocale = *_Plocale;
		imbue(_Newlocale);
		*_Plocale = _Newlocale;
		return (_Oldlocale);
		}

	locale getloc() const
		{	
		return (*_Plocale);
		}

	streamsize in_avail()
		{	
		streamsize _Res = _Gnavail();
		return (0 < _Res ? _Res : showmanyc());
		}

	int pubsync()
		{	
		return (sync());
		}

	int_type sbumpc()
		{	
		return (0 < _Gnavail()
			? _Traits::to_int_type(*_Gninc()) : uflow());
		}

	int_type sgetc()
		{	
		return (0 < _Gnavail()
			? _Traits::to_int_type(*gptr()) : underflow());
		}

	streamsize _Sgetn_s(_Elem *_Ptr, size_t _Ptr_size, streamsize _Count)
		{	
		return _Xsgetn_s(_Ptr, _Ptr_size, _Count);
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	streamsize sgetn(_Elem *_Ptr, streamsize _Count)
		{	
#pragma warning(push)
#pragma warning(disable:4996)
		return xsgetn(_Ptr, _Count);
#pragma warning(pop)
		}

	int_type snextc()
		{	
		return (1 < _Gnavail()
			? _Traits::to_int_type(*_Gnpreinc())
			: _Traits::eq_int_type(_Traits::eof(), sbumpc())
				? _Traits::eof() : sgetc());
		}

	int_type sputbackc(_Elem _Ch)
		{	
		return (gptr() != 0 && eback() < gptr()
			&& _Traits::eq(_Ch, gptr()[-1])
			? _Traits::to_int_type(*_Gndec())
			: pbackfail(_Traits::to_int_type(_Ch)));
		}

	void stossc()
		{	
		if (0 < _Gnavail())
			_Gninc();
		else
			uflow();
		}

	int_type sungetc()
		{	
		return (gptr() != 0 && eback() < gptr()
			? _Traits::to_int_type(*_Gndec()) : pbackfail());
		}

	int_type sputc(_Elem _Ch)
		{	
		return (0 < _Pnavail()
			? _Traits::to_int_type(*_Pninc() = _Ch)
			: overflow(_Traits::to_int_type(_Ch)));
		}

	streamsize sputn(const _Elem *_Ptr, streamsize _Count)
		{	
		return (xsputn(_Ptr, _Count));
		}

	void _Lock()
		{	
		_Mylock._Lock();
		}

	void _Unlock()
		{	
		_Mylock._Unlock();
		}


protected:
	_Elem *eback() const
		{	
		return (*_IGfirst);
		}

	_Elem *gptr() const
		{	
		return (*_IGnext);
		}

	_Elem *pbase() const
		{	
		return (*_IPfirst);
		}

	_Elem *pptr() const
		{	
		return (*_IPnext);
		}

	_Elem *egptr() const
		{	
		return (*_IGnext + *_IGcount);
		}

	void gbump(int _Off)
		{	
		*_IGcount -= _Off;
		*_IGnext += _Off;
		}

	void setg(_Elem *_First, _Elem *_Next, _Elem *_Last)
		{	
		*_IGfirst = _First;
		*_IGnext = _Next;
		*_IGcount = (int)(_Last - _Next);
		}

	_Elem *epptr() const
		{	
		return (*_IPnext + *_IPcount);
		}

	_Elem *_Gndec()
		{	
		++*_IGcount;
		return (--*_IGnext);
		}

	_Elem *_Gninc()
		{	
		--*_IGcount;
		return ((*_IGnext)++);
		}

	_Elem *_Gnpreinc()
		{	
		--*_IGcount;
		return (++(*_IGnext));
		}

	streamsize _Gnavail() const
		{	
		return (*_IGnext != 0 ? *_IGcount : 0);
		}

	void pbump(int _Off)
		{	
		*_IPcount -= _Off;
		*_IPnext += _Off;
		}

	void setp(_Elem *_First, _Elem *_Last)
		{	
		*_IPfirst = _First;
		*_IPnext = _First;
		*_IPcount = (int)(_Last - _First);
		}

	void setp(_Elem *_First, _Elem *_Next, _Elem *_Last)
		{	
		*_IPfirst = _First;
		*_IPnext = _Next;
		*_IPcount = (int)(_Last - _Next);
		}

	_Elem *_Pninc()
		{	
		--*_IPcount;
		return ((*_IPnext)++);
		}

	streamsize _Pnavail() const
		{	
		return (*_IPnext != 0 ? *_IPcount : 0);
		}

	void _Init()
		{	
		_IGfirst = &_Gfirst, _IPfirst = &_Pfirst;
		_IGnext = &_Gnext, _IPnext = &_Pnext;
		_IGcount = &_Gcount, _IPcount = &_Pcount;
		setp(0, 0), setg(0, 0, 0);
		}

	void _Init(_Elem **_Gf, _Elem **_Gn, int *_Gc,
		_Elem **_Pf, _Elem **_Pn, int *_Pc)
		{	
		_IGfirst = _Gf, _IPfirst = _Pf;
		_IGnext = _Gn, _IPnext = _Pn;
		_IGcount = _Gc, _IPcount = _Pc;
		}

	virtual int_type  overflow(int_type = _Traits::eof())
		{	
		return (_Traits::eof());
		}

	virtual int_type  pbackfail(int_type = _Traits::eof())
		{	
		return (_Traits::eof());
		}

	virtual streamsize  showmanyc()
		{	
		return (0);
		}

	virtual int_type  underflow()
		{	
		return (_Traits::eof());
		}

	virtual int_type  uflow()
		{	
		return (_Traits::eq_int_type(_Traits::eof(), underflow())
			? _Traits::eof() : _Traits::to_int_type(*_Gninc()));
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	virtual streamsize  xsgetn(_Elem * _Ptr,
		streamsize _Count)
		{	
		
		return _Xsgetn_s(_Ptr, (size_t)-1, _Count);
		}

	virtual streamsize  _Xsgetn_s(_Elem * _Ptr,
		size_t _Ptr_size, streamsize _Count)
		{	
		int_type _Meta;
		streamsize _Stream_size, _Size, _Copied;

		for (_Copied = 0; 0 < _Count; )
			if (0 < (_Stream_size = _Gnavail()))
				{	
				_Size = _Stream_size;
				if (_Count < _Size)
					_Size = _Count;
				_Traits_helper::copy_s<_Traits>(_Ptr, _Ptr_size, gptr(), _Size);
				_Ptr += _Size;
				_Copied += _Size;
				_Count -= _Size;
				gbump((int)_Size);
				}
			else if (_Traits::eq_int_type(_Traits::eof(), _Meta = uflow()))
				break;	
			else
				{	
				*_Ptr++ = _Traits::to_char_type(_Meta);
				++_Copied;
				--_Count;
				}

		return (_Copied);
		}

	virtual streamsize  xsputn(const _Elem *_Ptr,
		streamsize _Count)
		{	
		streamsize _Stream_size, _Size, _Copied;

		for (_Copied = 0; 0 < _Count; )
			if (0 < (_Stream_size = _Pnavail()))
				{	
				_Size = _Stream_size;
				if (_Count < _Size)
					_Size = _Count;
				_Traits_helper::copy_s<_Traits>(pptr(), _Stream_size, _Ptr, _Size);
				_Ptr += _Size;
				_Copied += _Size;
				_Count -= _Size;
				pbump((int)_Size);
				}
			else if (_Traits::eq_int_type(_Traits::eof(),
				overflow(_Traits::to_int_type(*_Ptr))))
				break;	
			else
				{	
				++_Ptr;
				++_Copied;
				--_Count;
				}

		return (_Copied);
		}

	virtual pos_type  seekoff(off_type, ios_base::seekdir,
		ios_base::openmode = ios_base::in | ios_base::out)
		{	
		return (streampos(_BADOFF));
		}

	virtual pos_type  seekpos(pos_type,
		ios_base::openmode = ios_base::in | ios_base::out)
		{	
		return (streampos(_BADOFF));
		}

	virtual _Myt * setbuf(_Elem *, streamsize)
		{	
		return (this);
		}

	virtual int  sync()
		{	
		return (0);
		}

	virtual void  imbue(const locale&)
		{	
		}

private:
	_Mutex _Mylock;	
	_Elem *_Gfirst;	
	_Elem *_Pfirst;	
	_Elem **_IGfirst;	
	_Elem **_IPfirst;	
	_Elem *_Gnext;	
	_Elem *_Pnext;	
	_Elem **_IGnext;	
	_Elem **_IPnext;	
	int _Gcount;	
	int _Pcount;	
	int *_IGcount;	
	int *_IPcount;	
	locale *_Plocale;	
	};

 






// #line 441 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

		
template<class _Elem,
	class _Traits>
	class istreambuf_iterator
		: public iterator<input_iterator_tag,
			_Elem, typename _Traits::off_type, _Elem *, _Elem&>
	{	
	typedef istreambuf_iterator<_Elem, _Traits> _Myt;
public:
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef basic_streambuf<_Elem, _Traits> streambuf_type;
	typedef basic_istream<_Elem, _Traits> istream_type;

	typedef typename traits_type::int_type int_type;


	typedef _Range_checked_iterator_tag _Checked_iterator_category;
// #line 461 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

	istreambuf_iterator(streambuf_type *_Sb = 0) throw ()
		: _Strbuf(_Sb), _Got(_Sb == 0)
		{	
		}

	istreambuf_iterator(istream_type& _Istr) throw ()
		: _Strbuf(_Istr.rdbuf()), _Got(_Istr.rdbuf() == 0)
		{	
		}

	_Elem operator*() const
		{	
		if (!_Got)
			((_Myt *)this)->_Peek();

 


// #line 481 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

		return (_Val);
		}

	_Myt& operator++()
		{	

 


// #line 492 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

		_Inc();
		return (*this);
		}

	_Myt operator++(int)
		{	
		if (!_Got)
			_Peek();
		_Myt _Tmp = *this;
		++*this;
		return (_Tmp);
		}

	bool equal(const _Myt& _Right) const
		{	
		if (!_Got)
			((_Myt *)this)->_Peek();
		if (!_Right._Got)
			((_Myt *)&_Right)->_Peek();
		return (_Strbuf == 0 && _Right._Strbuf == 0
			|| _Strbuf != 0 && _Right._Strbuf != 0);
		}

private:
	void _Inc()
		{	
		if (_Strbuf == 0
			|| traits_type::eq_int_type(traits_type::eof(),
				_Strbuf->sbumpc()))
			_Strbuf = 0, _Got = true;
		else
			_Got = false;
		}

	_Elem _Peek()
		{	
		int_type _Meta;
		if (_Strbuf == 0
			|| traits_type::eq_int_type(traits_type::eof(),
				_Meta = _Strbuf->sgetc()))
			_Strbuf = 0;
		else
			_Val = traits_type::to_char_type(_Meta);
		_Got = true;
		return (_Val);
		}

	streambuf_type *_Strbuf;	
	bool _Got;	
	_Elem _Val;	
	};

		
template<class _Elem,
	class _Traits> inline
	bool  operator==(
		const istreambuf_iterator<_Elem, _Traits>& _Left,
		const istreambuf_iterator<_Elem, _Traits>& _Right)
	{	
	return (_Left.equal(_Right));
	}

template<class _Elem,
	class _Traits> inline
	bool  operator!=(
		const istreambuf_iterator<_Elem, _Traits>& _Left,
		const istreambuf_iterator<_Elem, _Traits>& _Right)
	{	
	return (!(_Left == _Right));
	}

		
template<class _Elem,
	class _Traits>
	class ostreambuf_iterator
		: public _Outit
	{	
	typedef ostreambuf_iterator<_Elem, _Traits> _Myt;
public:
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef basic_streambuf<_Elem, _Traits> streambuf_type;
	typedef basic_ostream<_Elem, _Traits> ostream_type;


	typedef _Range_checked_iterator_tag _Checked_iterator_category;
// #line 580 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

	ostreambuf_iterator(streambuf_type *_Sb) throw ()
		: _Failed(false), _Strbuf(_Sb)
		{	
		}

	ostreambuf_iterator(ostream_type& _Ostr) throw ()
		: _Failed(false), _Strbuf(_Ostr.rdbuf())
		{	
		}

	_Myt& operator=(_Elem _Right)
		{	
		if (_Strbuf == 0
			|| traits_type::eq_int_type(_Traits::eof(),
				_Strbuf->sputc(_Right)))
			_Failed = true;
		return (*this);
		}

	_Myt& operator*()
		{	
		return (*this);
		}

	_Myt& operator++()
		{	
		return (*this);
		}

	_Myt& operator++(int)
		{	
		return (*this);
		}

	bool failed() const throw ()
		{	
		return (_Failed);
		}

private:
	bool _Failed;	
	streambuf_type *_Strbuf;	
	};
}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 629 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"

// #line 631 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"
// #line 632 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\streambuf"





// #line 10 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 15 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

		


extern "C" {
// #line 21 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
extern  float __cdecl _Stofx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **,
		long, int *);
extern  double __cdecl _Stodx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **,
		long, int *);
extern  long double __cdecl _Stoldx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **,
		long, int *);
extern  long __cdecl _Stolx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **, int,
		int *);
extern  unsigned long __cdecl _Stoulx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)] char **,
		int, int *);

  

extern  __int64 __cdecl _Stollx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)]  char **,
		int, int *);
extern  unsigned __int64 __cdecl _Stoullx(const char *,[SA_Pre(Null=SA_Maybe,WritableElementsConst=1)] [SA_Pre(Deref=1,Valid=SA_No)] [SA_Post(Deref=1,Null=SA_Maybe)] [SA_Post(Deref=2,Valid=SA_Yes)]  char **,
		int, int *);
  // #line 39 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"


}
// #line 43 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"


  
   
   
  // #line 49 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

namespace std {

 

#pragma warning(push)
#pragma warning(disable:4275)
		
template<class _Elem>
	class numpunct
		: public locale::facet
	{	
public:
	typedef basic_string<_Elem, char_traits<_Elem>, allocator<_Elem> >
		string_type;
	typedef _Elem char_type;

	 static locale::id id;	

	_Elem  decimal_point() const
		{	
		return (do_decimal_point());
		}

	_Elem  thousands_sep() const
		{	
		return (do_thousands_sep());
		}

	string  grouping() const
		{	
		return (do_grouping());
		}

	string_type  falsename() const
		{	
		return (do_falsename());
		}

	string_type  truename() const
		{	
		return (do_truename());
		}

	explicit  numpunct(size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
			if (_Kseparator == 0)
				_Kseparator =	
					_Maklocchr(',', (_Elem *)0, _Lobj._Getcvt());
		}
		}

	 numpunct(const _Locinfo& _Lobj, size_t _Refs = 0, bool _Isdef = false)
		: locale::facet(_Refs)
		{	
		_Init(_Lobj, _Isdef);
		}

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new numpunct<_Elem>(
				_Locinfo(_Ploc->name()), 0, true);
		return (4);
		}

protected:
	virtual  ~numpunct()
		{	
		_Tidy();
		}

protected:
	 numpunct(const char *_Locname, size_t _Refs = 0, bool _Isdef = false)
		: locale::facet(_Refs)
		{	
		{ _Locinfo _Lobj(_Locname);
			_Init(_Lobj, _Isdef);
		}
		}

	void  _Init(const _Locinfo& _Lobj, bool _Isdef = false)
		{	
		const lconv *_Ptr = _Lobj._Getlconv();

		_Grouping = 0;
		_Falsename = 0;
		_Truename = 0;

		try {
		_Grouping = _Maklocstr(_Ptr->grouping, (char *)0, _Lobj._Getcvt());
		_Falsename = _Maklocstr(_Lobj._Getfalse(), (_Elem *)0, _Lobj._Getcvt());
		_Truename = _Maklocstr(_Lobj._Gettrue(), (_Elem *)0, _Lobj._Getcvt());
		} catch (...) {
		_Tidy();
		throw;
		}

		_Dp = _Maklocchr(_Ptr->decimal_point[0], (_Elem *)0, _Lobj._Getcvt());
		_Kseparator =
			_Maklocchr(_Ptr->thousands_sep[0], (_Elem *)0, _Lobj._Getcvt());

		if (_Isdef)
			{	
			_Grouping = _Maklocstr("", (char *)0, _Lobj._Getcvt());
			_Dp = _Maklocchr('.', (_Elem *)0, _Lobj._Getcvt());
			_Kseparator = _Maklocchr(',', (_Elem *)0, _Lobj._Getcvt());
			}
		}

	virtual _Elem  do_decimal_point() const
		{	
		return (_Dp);
		}

	virtual _Elem  do_thousands_sep() const
		{	
		return (_Kseparator);
		}

	virtual string  do_grouping() const
		{	
		return (string(_Grouping));
		}

	virtual string_type  do_falsename() const
		{	
		return (string_type(_Falsename));
		}

	virtual string_type  do_truename() const
		{	
		return (string_type(_Truename));
		}

private:
	void  _Tidy()
		{	
		delete[] ((void *)_Grouping);
		delete[] ((void *)_Falsename);
		delete[] ((void *)_Truename);
		}

	const char *_Grouping;	
	_Elem _Dp;	
	_Elem _Kseparator;	
	const _Elem *_Falsename;	
	const _Elem *_Truename;	
	};

typedef numpunct<char> _Npc;
typedef numpunct<wchar_t> _Npwc;

		
template<class _Elem>
	class numpunct_byname
		: public numpunct<_Elem>
	{	
public:
	explicit  numpunct_byname(const char *_Locname, size_t _Refs = 0)
		: numpunct<_Elem>(_Locname, _Refs)
		{	
		}

protected:
	virtual  ~numpunct_byname()
		{	
		}
	};

		
template<class _Elem>
	 locale::id numpunct<_Elem>::id;

		
template<class _Elem,
	class _InIt = istreambuf_iterator<_Elem, char_traits<_Elem> > >
	class num_get
		: public locale::facet
	{	
public:
	typedef numpunct<_Elem> _Mypunct;
	typedef basic_string<_Elem, char_traits<_Elem>, allocator<_Elem> >
		_Mystr;

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new num_get<_Elem, _InIt>(
				_Locinfo(_Ploc->name()));
		return (4);
		}

	 static locale::id id;	

protected:
	virtual  ~num_get()
		{	
		}

protected:
	void _Init(const _Locinfo& _Lobj)
		{	
		_Cvt = _Lobj._Getcvt();
		}

	_Locinfo::_Cvtvec _Cvt;		

public:
	explicit  num_get(size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 num_get(const _Locinfo& _Lobj, size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		_Init(_Lobj);
		}

	typedef _Elem char_type;
	typedef _InIt iter_type;

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase,	ios_base::iostate& _State,
			_Bool& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase,	ios_base::iostate& _State,
			unsigned short& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase,	ios_base::iostate& _State,
			unsigned int& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			long& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned long& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

#if 0
// DQ: removed
	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			__int64& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned __int64& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}
#endif
 // #line 330 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			float& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			double& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			long double& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

	_InIt  get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			void *& _Val) const
		{	
		return (do_get(_First, _Last, _Iosbase, _State, _Val));
		}

protected:
	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			_Bool& _Val) const
		{	
		;
		int _Ans = -1;	

		if (_Iosbase.flags() & ios_base::boolalpha)
			{	
			typedef typename _Mystr::size_type _Mystrsize;
			const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Iosbase.getloc());
			_Mystr _Str((_Mystrsize)1, (char_type)0);
			_Str += _Punct_fac.falsename();
			_Str += (char_type)0;
			_Str += _Punct_fac.truename();	
			_Ans = _Getloctxt(_First, _Last, (size_t)2, _Str.c_str());
			}
		else
			{	
			char _Ac[32], *_Ep;
			int _Errno = 0;
			const unsigned long _Ulo = ::_Stoulx(_Ac, &_Ep,
				_Getifld(_Ac, _First, _Last, _Iosbase.flags(),
					_Iosbase.getloc()), &_Errno);
			if (_Ep != _Ac && _Errno == 0 && _Ulo <= 1)
				_Ans = _Ulo;
			}

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ans < 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans != 0;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned short& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		int _Base = _Getifld(_Ac, _First, _Last, _Iosbase.flags(),
			_Iosbase.getloc());	
		char *_Ptr = _Ac[0] == '-' ? _Ac + 1 : _Ac;	
		const unsigned long _Ans =
			::_Stoulx(_Ptr, &_Ep, _Base, &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ptr || _Errno != 0 || 0xffff < _Ans)
			_State |= ios_base::failbit;
		else
			_Val = (unsigned short)(_Ac[0] == '-'
				? 0 -_Ans : _Ans);	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned int& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		int _Base = _Getifld(_Ac, _First, _Last, _Iosbase.flags(),
			_Iosbase.getloc());	
		char *_Ptr = _Ac[0] == '-' ? _Ac + 1 : _Ac;	
		const unsigned long _Ans =
			::_Stoulx(_Ptr, &_Ep, _Base, &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ptr || _Errno != 0 || 0xffffffff < _Ans)
			_State |= ios_base::failbit;
		else
			_Val = _Ac[0] == '-' ? 0 -_Ans : _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			long& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		const long _Ans = ::_Stolx(_Ac, &_Ep,
			_Getifld(_Ac, _First, _Last, _Iosbase.flags(),
				_Iosbase.getloc()), &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned long& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		const unsigned long _Ans = ::_Stoulx(_Ac, &_Ep,
			_Getifld(_Ac, _First, _Last, _Iosbase.flags(),
				_Iosbase.getloc()), &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

 
#if 0
// DQ: removed
	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			__int64& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		const __int64 _Ans = ::_Stollx(_Ac, &_Ep, _Getifld(_Ac, _First, _Last, _Iosbase.flags(), _Iosbase.getloc()), &_Errno);	
// #line 493 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			unsigned __int64& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;
		const unsigned __int64 _Ans = ::_Stoullx(_Ac, &_Ep, _Getifld(_Ac, _First, _Last, _Iosbase.flags(), _Iosbase.getloc()), &_Errno);	
// #line 513 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}
#endif
 // #line 523 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			float& _Val) const
		{	
		;
		char _Ac[8 + 36 + 16], *_Ep;
		int _Errno = 0;
		const float _Ans = ::_Stofx(_Ac, &_Ep,
			_Getffld(_Ac, _First, _Last,
				_Iosbase.getloc()), &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			double& _Val) const
		{	
		;
		char _Ac[8 + 36 + 16], *_Ep;
		int _Errno = 0;
		const double _Ans = ::_Stodx(_Ac, &_Ep,
			_Getffld(_Ac, _First, _Last,
				_Iosbase.getloc()), &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			long double& _Val) const
		{	
		;
		char _Ac[8 + 36 + 16], *_Ep;
		int _Errno = 0;
		const long double _Ans = ::_Stoldx(_Ac, &_Ep,
			_Getffld(_Ac, _First, _Last,
				_Iosbase.getloc()), &_Errno);	

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = _Ans;	
		return (_First);
		}

	virtual _InIt  do_get(_InIt _First, _InIt _Last,
		ios_base& _Iosbase, ios_base::iostate& _State,
			void *& _Val) const
		{	
		;
		char _Ac[32], *_Ep;
		int _Errno = 0;

 
		int _Base = _Getifld(_Ac, _First, _Last, ios_base::hex,
			_Iosbase.getloc());	
		const unsigned __int64 _Ans =
			(sizeof (void *) == sizeof (unsigned long))
				? (unsigned __int64)::_Stoulx(_Ac, &_Ep, _Base, &_Errno)
				: ::_Stoullx(_Ac, &_Ep, _Base, &_Errno);

 



// #line 605 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

		if (_First == _Last)
			_State |= ios_base::eofbit;
		if (_Ep == _Ac || _Errno != 0)
			_State |= ios_base::failbit;
		else
			_Val = (void *)((char *)0 + _Ans);	
		return (_First);
		}

private:
	int __cdecl _Getifld([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Ac,
		_InIt& _First, _InIt& _Last, ios_base::fmtflags _Basefield,
			const locale& _Loc) const
		{	
		const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Loc);
		const string _Grouping = _Punct_fac.grouping();
		const _Elem _Kseparator = _Grouping.size() == 0
			? (_Elem)0 : _Punct_fac.thousands_sep();
		const _Elem _E0 = _Maklocchr('0', (_Elem *)0, _Cvt);
		char *_Ptr = _Ac;

		if (_First == _Last)
			;	
		else if (*_First == _Maklocchr('+', (_Elem *)0, _Cvt))
			*_Ptr++ = '+', ++_First;	
		else if (*_First == _Maklocchr('-', (_Elem *)0, _Cvt))
			*_Ptr++ = '-', ++_First;	

		_Basefield &= ios_base::basefield;
		int _Base = _Basefield == ios_base::oct ? 8
			: _Basefield == ios_base::hex ? 16
			: _Basefield == ios_base::_Fmtzero ? 0 : 10;

		bool _Seendigit = false;	
		bool _Nonzero = false;	

		if (_First != _Last && *_First == _E0)
			{	
			_Seendigit = true, ++_First;
			if (_First != _Last && (*_First == _Maklocchr('x', (_Elem *)0, _Cvt)
					|| *_First == _Maklocchr('X', (_Elem *)0, _Cvt))
				&& (_Base == 0 || _Base == 16))
				_Base = 16, _Seendigit = false, ++_First;
			else if (_Base == 0)
				_Base = 8;
			}

		int _Dlen = _Base == 0 || _Base == 10 ? 10
			: _Base == 8 ? 8 : 16 + 6;
		string _Groups((size_t)1, (char)_Seendigit);
		size_t _Group = 0;

		for (char *const _Pe = &_Ac[32 - 1];
			_First != _Last; ++_First)
			if (::memchr((const char *)"0123456789abcdefABCDEF",
				*_Ptr = _Maklocbyte((_Elem)*_First, _Cvt), _Dlen) != 0)
				{	
				if ((_Nonzero || *_Ptr != '0') && _Ptr < _Pe)
					++_Ptr, _Nonzero = true;
				_Seendigit = true;
				if (_Groups[_Group] != 127)
					++_Groups[_Group];
				}
			else if (_Groups[_Group] == '\0'
				|| _Kseparator == (_Elem)0
				|| *_First != _Kseparator)
				break;	
			else
				{	
				_Groups.append((string::size_type)1, '\0');
				++_Group;
				}

		if (_Group == 0)
			;	
		else if ('\0' < _Groups[_Group])
			++_Group;	
		else
			_Seendigit = false;	

		for (const char *_Pg = _Grouping.c_str(); _Seendigit && 0 < _Group; )
			if (*_Pg == 127)
				break;	
			else if (0 < --_Group && *_Pg != _Groups[_Group]
				|| 0 == _Group && *_Pg < _Groups[_Group])
				_Seendigit = false;	
			else if ('\0' < _Pg[1])
				++_Pg;	

		if (_Seendigit && !_Nonzero)
			*_Ptr++ = '0';	
		else if (!_Seendigit)
			_Ptr = _Ac;	
		*_Ptr = '\0';
		return (_Base);
		}

	int __cdecl _Getffld([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Ac,
		_InIt& _First, _InIt &_Last, const locale& _Loc) const
		{	
		const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Loc);
		const string _Grouping = _Punct_fac.grouping();
		const _Elem _E0 = _Maklocchr('0', (_Elem *)0, _Cvt);
		char *_Ptr = _Ac;
		bool _Bad = false;

		if (_First == _Last)
			;	
		else if (*_First == _Maklocchr('+', (_Elem *)0, _Cvt))
			*_Ptr++ = '+', ++_First;	
		else if (*_First == _Maklocchr('-', (_Elem *)0, _Cvt))
			*_Ptr++ = '-', ++_First;	

		bool _Seendigit = false;	
		int _Significant = 0;	
		int _Pten = 0;	

		if (*_Grouping.c_str() == 127 || *_Grouping.c_str() <= '\0')
			for (; _First != _Last
				&& _E0 <= *_First && *_First <= _E0 + 9;
					_Seendigit = true, ++_First)
				if (36 <= _Significant)
					++_Pten;	
				else if (*_First == _E0 && _Significant == 0)
					;	
				else
					{	
					*_Ptr++ = (char)((*_First - _E0) + '0');
					++_Significant;
					}
		else
			{	
			const _Elem _Kseparator = _Punct_fac.thousands_sep();

			string _Groups((size_t)1, '\0');
			size_t _Group = 0;

			for (; _First != _Last; ++_First)
				if (_E0 <= *_First && *_First <= _E0 + 9)
					{	
					_Seendigit = true;
					if (36 <= _Significant)
						++_Pten;	
					else if (*_First == _E0 && _Significant == 0)
						;	
					else
						{	
						*_Ptr++ = (char)((*_First - _E0) + '0');
						++_Significant;
						}
					if (_Groups[_Group] != 127)
						++_Groups[_Group];
					}
				else if (_Groups[_Group] == '\0'
					|| _Kseparator == (_Elem)0
					|| *_First != _Kseparator)
					break;	
				else
					{	
					_Groups.append((size_t)1, '\0');
					++_Group;
					}
			if (_Group == 0)
				;	
			else if ('\0' < _Groups[_Group])
				++_Group;	
			else
				_Bad = true;	

			for (const char *_Pg = _Grouping.c_str();
				!_Bad && 0 < _Group; )
				if (*_Pg == 127)
					break;	
				else if (0 < --_Group && *_Pg != _Groups[_Group]
					|| 0 == _Group && *_Pg < _Groups[_Group])
					_Bad = true;	
				else if ('\0' < _Pg[1])
					++_Pg;	
			}

		if (_Seendigit && _Significant == 0)
			*_Ptr++ = '0';	

		if (_First != _Last && *_First == _Punct_fac.decimal_point())
			*_Ptr++ = localeconv()->decimal_point[0], ++_First;	

		if (_Significant == 0)
			{	
			for (; _First != _Last && *_First == _E0;
				_Seendigit = true, ++_First)
				--_Pten;	
			if (_Pten < 0)
				*_Ptr++ = '0', ++_Pten;	
			}

		for (; _First != _Last
				&& _E0 <= *_First && *_First <= _E0 + 9;
				_Seendigit = true, ++_First)
			if (_Significant < 36)
				{	
				*_Ptr++ = (char)((*_First - _E0) + '0');
				++_Significant;
				}

		if (_Seendigit && _First != _Last
			&& (*_First == _Maklocchr('e', (_Elem *)0, _Cvt)
				|| *_First == _Maklocchr('E', (_Elem *)0, _Cvt)))
			{	
			*_Ptr++ = 'e', ++_First;
			_Seendigit = false, _Significant = 0;

			if (_First == _Last)
				;	
			else if (*_First == _Maklocchr('+', (_Elem *)0, _Cvt))
				*_Ptr++ = '+', ++_First;	
			else if (*_First == _Maklocchr('-', (_Elem *)0, _Cvt))
				*_Ptr++ = '-', ++_First;	
			for (; _First != _Last && *_First == _E0; )
				_Seendigit = true, ++_First;	
			if (_Seendigit)
				*_Ptr++ = '0';	
			for (; _First != _Last
				&& _E0 <= *_First && *_First <= _E0 + 9;
				_Seendigit = true, ++_First)
				if (_Significant < 8)
					{	
					*_Ptr++ = (char)((*_First - _E0) + '0');
					++_Significant;
					}
			}

		if (_Bad || !_Seendigit)
			_Ptr = _Ac;	
		*_Ptr = '\0';
		return (_Pten);
		}


	int _Hexdig(const _Elem _Dig, const _Elem _E0,
		const _Elem _Al, const _Elem _Au) const
		{	
		if (_E0 <= _Dig && _Dig <= _E0 + 9)
			return (_Dig - _E0);	
		else if (_Al <= _Dig && _Dig <= _Al + 5)
			return (_Dig - _Al + 10);	
		else if (_Au <= _Dig && _Dig <= _Au + 5)
			return (_Dig - _Au + 10);	
		else
			return (-1);
		}

	int __cdecl _Getffldx(char *_Ac,
		_InIt& _First, _InIt &_Last,
		ios_base& _Iosbase, int *_Phexexp) const
		{	
		const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Iosbase.getloc());
		const string _Grouping = _Punct_fac.grouping();
		const _Elem _E0 = _Maklocchr('0', (_Elem *)0, _Cvt);
		const _Elem _Al = _Maklocchr('a', (_Elem *)0, _Cvt);
		const _Elem _Au = _Maklocchr('A', (_Elem *)0, _Cvt);
		char *_Ptr = _Ac;
		bool _Bad = false;
		int _Dig;

		if (_First == _Last)
			;	
		else if (*_First == _Maklocchr('+', (_Elem *)0, _Cvt))
			*_Ptr++ = '+', ++_First;	
		else if (*_First == _Maklocchr('-', (_Elem *)0, _Cvt))
			*_Ptr++ = '-', ++_First;	

		*_Ptr++ = '0';
		*_Ptr++ = 'x';

		bool _Seendigit = false;	
		int _Significant = 0;	
		int _Phex = 0;	

		if (_First == _Last || *_First != _E0)
			;
		else if (++_First != _Last
			&& (*_First == _Maklocchr('x', (_Elem *)0, _Cvt)
				 || *_First == _Maklocchr('X', (_Elem *)0, _Cvt)))
			++_First;	
		else
			_Seendigit = true;	

		if (*_Grouping.c_str() == 127 || *_Grouping.c_str() <= '\0')
			for (; _First != _Last
				&& 0 <= (_Dig = _Hexdig(*_First, _E0, _Al, _Au));
					_Seendigit = true, ++_First)
				if (36 <= _Significant)
					++_Phex;	
				else if (*_First == _E0 && _Significant == 0)
					;	
				else
					{	
					*_Ptr++ = "0123456789abcdef"[_Dig];
					++_Significant;
					}
		else
			{	
			const _Elem _Kseparator = _Grouping.size() == 0
				? (_Elem)0 : _Punct_fac.thousands_sep();
			string _Groups((size_t)1, '\0');
			size_t _Group = 0;

			for (; _First != _Last; ++_First)
				if (0 <= (_Dig = _Hexdig(*_First, _E0, _Al, _Au)))
					{	
					_Seendigit = true;
					if (36 <= _Significant)
						++_Phex;	
					else if (*_First == _E0 && _Significant == 0)
						;	
					else
						{	
						*_Ptr++ = "0123456789abcdef"[_Dig];
						++_Significant;
						}
					if (_Groups[_Group] != 127)
						++_Groups[_Group];
					}
				else if (_Groups[_Group] == '\0'
					|| _Kseparator == (_Elem)0
					|| *_First != _Kseparator)
					break;	
				else
					{	
					_Groups.append((size_t)1, '\0');
					++_Group;
					}
			if (_Group == 0)
				;	
			else if ('\0' < _Groups[_Group])
				++_Group;	
			else
				_Bad = true;	

			for (const char *_Pg = _Grouping.c_str();
				!_Bad && 0 < _Group; )
				if (*_Pg == 127)
					break;	
				else if (0 < --_Group && *_Pg != _Groups[_Group]
					|| 0 == _Group && *_Pg < _Groups[_Group])
					_Bad = true;	
				else if ('\0' < _Pg[1])
					++_Pg;	
			}

		if (_Seendigit && _Significant == 0)
			*_Ptr++ = '0';	

		if (_First != _Last && *_First == _Punct_fac.decimal_point())
			*_Ptr++ = localeconv()->decimal_point[0], ++_First;	

		if (_Significant == 0)
			{	
			for (; _First != _Last && *_First == _E0;
				_Seendigit = true, ++_First)
				--_Phex;	
			if (_Phex < 0)
				*_Ptr++ = '0', ++_Phex;	
			}

		for (; _First != _Last
				&& 0 <= (_Dig = _Hexdig(*_First, _E0, _Al, _Au));
				_Seendigit = true, ++_First)
			if (_Significant < 36)
				{	
				*_Ptr++ = "0123456789abcdef"[_Dig];
				++_Significant;
				}

		if (_Seendigit && _First != _Last
			&& (*_First == _Maklocchr('p', (_Elem *)0, _Cvt)
				|| *_First == _Maklocchr('P', (_Elem *)0, _Cvt)))
			{	
			*_Ptr++ = 'p', ++_First;
			_Seendigit = false, _Significant = 0;

			if (_First == _Last)
				;	
			else if (*_First == _Maklocchr('+', (_Elem *)0, _Cvt))
				*_Ptr++ = '+', ++_First;	
			else if (*_First == _Maklocchr('-', (_Elem *)0, _Cvt))
				*_Ptr++ = '-', ++_First;	
			for (; _First != _Last && *_First == _E0; )
				_Seendigit = true, ++_First;	
			if (_Seendigit)
				*_Ptr++ = '0';	
			for (; _First != _Last
				&& _E0 <= *_First && *_First <= _E0 + 9;
				_Seendigit = true, ++_First)
				if (_Significant < 8)
					{	
					*_Ptr++ = (char)((*_First - _E0) + '0');
					++_Significant;
					}
			}

		if (_Bad || !_Seendigit)
			_Ptr = _Ac;	
		*_Ptr = '\0';
		*_Phexexp = _Phex;	
		return (0);	
		}
	};

		
template<class _Elem,
	class _InIt>
	 locale::id num_get<_Elem, _InIt>::id;

		
template<class _Elem,
	class _OutIt = ostreambuf_iterator<_Elem, char_traits<_Elem> > >
	class num_put
		: public locale::facet
	{	
public:
	typedef numpunct<_Elem> _Mypunct;
	typedef basic_string<_Elem, char_traits<_Elem>, allocator<_Elem> >
		_Mystr;

	static size_t __cdecl _Getcat(const locale::facet **_Ppf = 0,
		const locale *_Ploc = 0)
		{	
		if (_Ppf != 0 && *_Ppf == 0)
			*_Ppf = new num_put<_Elem, _OutIt>(
				_Locinfo(_Ploc->name()));
		return (4);
		}

	 static locale::id id;	

protected:
	virtual  ~num_put()
		{	
		}

protected:
	void  _Init(const _Locinfo& _Lobj)
		{	
		_Cvt = _Lobj._Getcvt();
		}

	_Locinfo::_Cvtvec _Cvt;		

public:
	explicit  num_put(size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		{ _Locinfo _Lobj;
			_Init(_Lobj);
		}
		}

	 num_put(const _Locinfo& _Lobj, size_t _Refs = 0)
		: locale::facet(_Refs)
		{	
		_Init(_Lobj);
		}

	typedef _Elem char_type;
	typedef _OutIt iter_type;

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, _Bool _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, long _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, unsigned long _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

 
#if 0
// DQ: removed
	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, __int64 _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, unsigned __int64 _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}
#endif
 // #line 1104 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, double _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, long double _Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

	_OutIt  put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, const void *_Val) const
		{	
		return (do_put(_Dest, _Iosbase, _Fill, _Val));
		}

protected:
	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, _Bool _Val) const
		{	
		;
		if (!(_Iosbase.flags() & ios_base::boolalpha))
			return (do_put(_Dest, _Iosbase, _Fill, (long)_Val));
		else
			{	
			const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Iosbase.getloc());
			_Mystr _Str;
			if (_Val)
				_Str.assign(_Punct_fac.truename());
			else
				_Str.assign(_Punct_fac.falsename());

			size_t _Fillcount = _Iosbase.width() <= 0
				|| (size_t)_Iosbase.width() <= _Str.size()
					? 0 : (size_t)_Iosbase.width() - _Str.size();

			if ((_Iosbase.flags() & ios_base::adjustfield) != ios_base::left)
				{	
				_Dest = _Rep(_Dest, _Fill, _Fillcount);
				_Fillcount = 0;
				}
			_Dest = _Put(_Dest, _Str.c_str(), _Str.size());	
			_Iosbase.width(0);
			return (_Rep(_Dest, _Fill, _Fillcount));	
			}
		}

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, long _Val) const
		{	
		const size_t _Buf_size = 2 * 32;
		char _Buf[_Buf_size], _Fmt[6];
		return (_Iput(_Dest, _Iosbase, _Fill, _Buf,
			::sprintf_s(_Buf, _Buf_size, _Ifmt(_Fmt, "ld",
				_Iosbase.flags()), _Val)));
		}

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, unsigned long _Val) const
		{	
		const size_t _Buf_size = 2 * 32;
		char _Buf[_Buf_size], _Fmt[6];
		return (_Iput(_Dest, _Iosbase, _Fill, _Buf,
			::sprintf_s(_Buf, _Buf_size, _Ifmt(_Fmt, "lu",
				_Iosbase.flags()), _Val)));
		}

 
#if 0
// DQ: removed
	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, __int64 _Val) const
		{	
		const size_t _Buf_size = 2 * 32;
		char _Buf[_Buf_size], _Fmt[8];
		return (_Iput(_Dest, _Iosbase, _Fill, _Buf,
			::sprintf_s(_Buf, _Buf_size, _Ifmt(_Fmt, "Ld",
				_Iosbase.flags()), _Val)));
		}

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, unsigned __int64 _Val) const
		{	
		const size_t _Buf_size = 2 * 32;
		char _Buf[_Buf_size], _Fmt[8];
		return (_Iput(_Dest, _Iosbase, _Fill, _Buf,
			::sprintf_s(_Buf, _Buf_size, _Ifmt(_Fmt, "Lu",
				_Iosbase.flags()), _Val)));
		}
#endif
 // #line 1195 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, double _Val) const
		{	
		const size_t _Buf_size = 8 + 36 + 64;
		char _Buf[_Buf_size], _Fmt[8];
		streamsize _Precision = _Iosbase.precision() <= 0
			&& !(_Iosbase.flags() & ios_base::fixed)
				? 6 : _Iosbase.precision();	
		int _Significance = 36 < _Precision
			? 36 : (int)_Precision;	
		_Precision -= _Significance;
		size_t _Beforepoint = 0;	
		size_t _Afterpoint = 0;	

		if ((_Iosbase.flags() & ios_base::floatfield) == ios_base::fixed
			&& _Val * 0.5 != _Val)	
			{	
			bool _Signed = _Val < 0;
			if (_Signed)
				_Val = -_Val;

			for (; 1e35 <= _Val && _Beforepoint < 5000; _Beforepoint += 10)
				_Val /= 1e10;	

			if (0 < _Val)
				for (; 10 <= _Precision && _Val <= 1e-35
					&& _Afterpoint < 5000; _Afterpoint += 10)
					{	
					_Val *= 1e10;
					_Precision -= 10;
					}

			if (_Signed)
				_Val = -_Val;
			}

		return (_Fput(_Dest, _Iosbase, _Fill, _Buf,
			_Beforepoint, _Afterpoint, _Precision,
				::sprintf_s(_Buf, _Buf_size, _Ffmt(_Fmt, 0, _Iosbase.flags()),
					_Significance, _Val)));	
		}

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, long double _Val) const
		{	
		const size_t _Buf_size = 8 + 36 + 64;
		char _Buf[_Buf_size], _Fmt[8];
		streamsize _Precision = _Iosbase.precision() <= 0
			&& !(_Iosbase.flags() & ios_base::fixed)
				? 6 : _Iosbase.precision();	
		int _Significance = 36 < _Precision
			? 36 : (int)_Precision;	
		_Precision -= _Significance;
		size_t _Beforepoint = 0;	
		size_t _Afterpoint = 0;	

		if ((_Iosbase.flags() & ios_base::floatfield) == ios_base::fixed)
			{	
			bool _Signed = _Val < 0;
			if (_Signed)
				_Val = -_Val;

			for (; 1e35 <= _Val && _Beforepoint < 5000; _Beforepoint += 10)
				_Val /= 1e10;	

			if (0 < _Val)
				for (; 10 <= _Precision && _Val <= 1e-35
					&& _Afterpoint < 5000; _Afterpoint += 10)
					{	
					_Val *= 1e10;
					_Precision -= 10;
					}

			if (_Signed)
				_Val = -_Val;
			}

		return (_Fput(_Dest, _Iosbase, _Fill, _Buf,
			_Beforepoint, _Afterpoint, _Precision,
				::sprintf_s(_Buf, _Buf_size, _Ffmt(_Fmt, 'L', _Iosbase.flags()),
					_Significance, _Val)));	
		}

	virtual _OutIt  do_put(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, const void *_Val) const
		{	
		const size_t _Buf_size = 2 * 32;
		char _Buf[_Buf_size];
		return (_Iput(_Dest, _Iosbase, _Fill, _Buf,
			::sprintf_s(_Buf, _Buf_size, "%p", _Val)));
		}

private:
	char *__cdecl _Ffmt([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Fmt,
		char _Spec, ios_base::fmtflags _Flags) const
		{	
		char *_Ptr = _Fmt;
		*_Ptr++ = '%';

		if (_Flags & ios_base::showpos)
			*_Ptr++ = '+';
		if (_Flags & ios_base::showpoint)
			*_Ptr++ = '#';
		*_Ptr++ = '.';
		*_Ptr++ = '*';	
		if (_Spec != '\0')
			*_Ptr++ = _Spec;	

		ios_base::fmtflags _Ffl = _Flags & ios_base::floatfield;
		*_Ptr++ = _Ffl == ios_base::fixed ? 'f'
			: _Ffl == ios_base::hexfloat ? 'a'	
			: _Ffl == ios_base::scientific ? 'e' : 'g';	
		*_Ptr = '\0';
		return (_Fmt);
		}

	_OutIt __cdecl _Fput(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, const char *_Buf,
			size_t _Beforepoint, size_t _Afterpoint,
				size_t _Trailing, size_t _Count) const
		{	
		;
		const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Iosbase.getloc());
		const string _Grouping = _Punct_fac.grouping();
		const _Elem _Kseparator = _Punct_fac.thousands_sep();
		string _Groupstring;
		const _Elem _E0 = _Maklocchr('0', (_Elem *)0, _Cvt);
		size_t _Prefix = _Buf[0] == '+' || _Buf[0] == '-' ? 1 : 0;

		char _Enders[3];
		_Enders[0] = ::localeconv()->decimal_point[0];
		_Enders[1] = 'e';
		_Enders[2] = '\0';

		const char *_Eptr = (const char *)::memchr(_Buf,
			'e', _Count);	
		const char *_Pointptr = (const char *)::memchr(_Buf,
			_Enders[0], _Count);	
		if (_Pointptr == 0)
			_Trailing = 0;

		if (*_Grouping.c_str() != 127 && '\0' < *_Grouping.c_str())
			{	
			_Groupstring.append(_Buf, _Count);	
			if (_Eptr == 0)
				_Groupstring.append(_Trailing, '0');
			else
				{	
				if (_Pointptr == 0)
					{	
					_Groupstring.append(_Beforepoint, '0');
					_Beforepoint = 0;
					}
				_Groupstring.insert(_Eptr - _Buf, _Trailing, '0');
				}
			_Trailing = 0;

			if (_Pointptr == 0)
				_Groupstring.append(_Beforepoint, '0');
			else
				{	
				_Groupstring.insert(_Pointptr - _Buf + 1, _Afterpoint, '0');
				_Groupstring.insert(_Pointptr - _Buf, _Beforepoint, '0');
				_Afterpoint = 0;
				}
			_Beforepoint = 0;

			const char *_Pg = _Grouping.c_str();
			size_t _Off = ::strcspn(&_Groupstring[0], &_Enders[0]);
			while (*_Pg != 127 && '\0' < *_Pg
				&& (size_t)*_Pg < _Off - _Prefix)
				{	
				_Groupstring.insert(_Off -= *_Pg, (size_t)1, '\0');
				if ('\0' < _Pg[1])
					++_Pg;	
				}

			_Buf = &_Groupstring[0];
			_Trailing = 0;
			_Count = _Groupstring.size();
			}

		size_t _Fillcount = _Beforepoint + _Afterpoint + _Trailing + _Count;
		_Fillcount = _Iosbase.width() <= 0
			|| (size_t)_Iosbase.width() <= _Fillcount
				? 0 : (size_t)_Iosbase.width() - _Fillcount;
		ios_base::fmtflags _Adjustfield =
			_Iosbase.flags() & ios_base::adjustfield;
		if (_Adjustfield != ios_base::left
			&& _Adjustfield != ios_base::internal)
			{	
			_Dest = _Rep(_Dest, _Fill, _Fillcount);
			_Fillcount = 0;
			}
		else if (_Adjustfield == ios_base::internal)
			{	
			if (0 < _Prefix)
				{	
				_Dest = _Putc(_Dest, _Buf, 1);
				++_Buf, --_Count;
				}
			_Dest = _Rep(_Dest, _Fill, _Fillcount);
			_Fillcount = 0;
			}

		_Pointptr = (const char *)::memchr(_Buf,
			_Enders[0], _Count);	
		if (_Pointptr != 0)
			{	
			size_t _Fracoffset = _Pointptr - _Buf + 1;
			_Dest = _Putgrouped(_Dest, _Buf, _Fracoffset - 1, _Kseparator);
			_Dest = _Rep(_Dest, _E0, _Beforepoint);
			_Dest = _Rep(_Dest, _Punct_fac.decimal_point(), 1);
			_Dest = _Rep(_Dest, _E0, _Afterpoint);
			_Buf += _Fracoffset, _Count -= _Fracoffset;
			}

		_Eptr = (const char *)::memchr(_Buf,
			'e', _Count);	
		if (_Eptr != 0)
			{	
			size_t _Expoffset = _Eptr - _Buf + 1;
			_Dest = _Putgrouped(_Dest, _Buf, _Expoffset - 1, _Kseparator);
			_Dest = _Rep(_Dest, _E0, _Trailing), _Trailing = 0;
			_Dest = _Putc(_Dest, _Iosbase.flags() & ios_base::uppercase
				? "E" : "e", 1);
			_Buf += _Expoffset, _Count -= _Expoffset;
			}

		_Dest = _Putgrouped(_Dest, _Buf, _Count,
			_Kseparator);	
		_Dest = _Rep(_Dest, _E0, _Trailing);	
		_Iosbase.width(0);
		return (_Rep(_Dest, _Fill, _Fillcount));	
		}

	char *__cdecl _Ifmt([SA_Pre(Null=SA_No)] [SA_Post(NullTerminated=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Fmt,
		const char *_Spec, ios_base::fmtflags _Flags) const
		{	
		char *_Ptr = _Fmt;
		*_Ptr++ = '%';

		if (_Flags & ios_base::showpos)
			*_Ptr++ = '+';
		if (_Flags & ios_base::showbase)
			*_Ptr++ = '#';
		if (_Spec[0] != 'L')
			*_Ptr++ = _Spec[0];	
		else
			{	
			*_Ptr++ = 'I';
			*_Ptr++ = '6';
			*_Ptr++ = '4';
			}

		ios_base::fmtflags _Basefield = _Flags & ios_base::basefield;
		*_Ptr++ = _Basefield == ios_base::oct ? 'o'
			: _Basefield != ios_base::hex ? _Spec[1]	
			: _Flags & ios_base::uppercase ? 'X' : 'x';
		*_Ptr = '\0';
		return (_Fmt);
		}

	_OutIt __cdecl _Iput(_OutIt _Dest,
		ios_base& _Iosbase, _Elem _Fill, [SA_Pre(Null=SA_No,WritableElements="_Count")] [SA_Pre(Deref=1,Valid=SA_Yes)] [SA_Post(Deref=1,Valid=SA_Yes)] char *_Buf, size_t _Count) const
		{	
		;
		const _Mypunct& _Punct_fac = use_facet<_Mypunct >(_Iosbase.getloc());
		const string _Grouping = _Punct_fac.grouping();
		const size_t _Prefix = *_Buf == '+' || *_Buf == '-' ? 1
			: *_Buf == '0' && (_Buf[1] == 'x' || _Buf[1] == 'X') ? 2
			: 0;

		if (*_Grouping.c_str() != 127 && '\0' < *_Grouping.c_str())
			{	
			const char *_Pg = _Grouping.c_str();
			size_t _Off = _Count;
			while (*_Pg != 127 && '\0' < *_Pg
				&& (size_t)*_Pg < _Off - _Prefix)
				{	
				_Off -= *_Pg;
				::memmove_s((&_Buf[_Off + 1]), (_Count + 1 - _Off), (&_Buf[_Off]), (_Count + 1 - _Off));
// #line 1479 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
				_Buf[_Off] = '\0', ++_Count;
				if ('\0' < _Pg[1])
					++_Pg;	
				}
			}

		size_t _Fillcount = _Iosbase.width() <= 0
			|| (size_t)_Iosbase.width() <= _Count
				? 0 : (size_t)_Iosbase.width() - _Count;

		ios_base::fmtflags _Adjustfield =
			_Iosbase.flags() & ios_base::adjustfield;
		if (_Adjustfield != ios_base::left
			&& _Adjustfield != ios_base::internal)
			{	
			_Dest = _Rep(_Dest, _Fill, _Fillcount);
			_Fillcount = 0;
			}
		else if (_Adjustfield == ios_base::internal)
			{	
			_Dest = _Putc(_Dest, _Buf, _Prefix);	
			_Buf += _Prefix, _Count -= _Prefix;
			_Dest = _Rep(_Dest, _Fill, _Fillcount), _Fillcount = 0;
			}

		_Dest = _Putgrouped(_Dest, _Buf, _Count,
			_Punct_fac.thousands_sep());	
		_Iosbase.width(0);
		return (_Rep(_Dest, _Fill, _Fillcount));	
		}

	_OutIt __cdecl _Put(_OutIt _Dest,
		const _Elem *_Ptr, size_t _Count) const
		{	
		for (; 0 < _Count; --_Count, ++_Dest, ++_Ptr)
			*_Dest = *_Ptr;
		return (_Dest);
		}

	_OutIt __cdecl _Putc(_OutIt _Dest,
		const char *_Ptr, size_t _Count) const
		{	
		for (; 0 < _Count; --_Count, ++_Dest, ++_Ptr)
			*_Dest = _Maklocchr(*_Ptr, (_Elem *)0, _Cvt);
		return (_Dest);
		}

	_OutIt __cdecl _Putgrouped(_OutIt _Dest,
		const char *_Ptr, size_t _Count, _Elem _Kseparator) const
		{	
		for (; ; ++_Ptr, --_Count)
			{	
			const char *_Pend =
				(const char *)::memchr(_Ptr, '\0', _Count);
			size_t _Groupsize = _Pend != 0 ? _Pend - _Ptr : _Count;

			_Dest = _Putc(_Dest, _Ptr, _Groupsize);
			_Ptr += _Groupsize, _Count -= _Groupsize;
			if (_Count == 0)
				break;
			if (_Kseparator != (_Elem)0)
				_Dest = _Rep(_Dest, _Kseparator, 1);
			}
		return (_Dest);
		}

	_OutIt __cdecl _Rep(_OutIt _Dest,
		_Elem _Ch, size_t _Count) const
		{	
		for (; 0 < _Count; --_Count, ++_Dest)
			*_Dest = _Ch;
		return (_Dest);
		}
	};

		
template<class _Elem,
	class _OutIt>
	 locale::id num_put<_Elem, _OutIt>::id;

 















// #line 1576 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

#pragma warning(pop)
}


 #pragma warning(pop)
 #pragma pack(pop)
// #line 1584 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"

// #line 1586 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"
// #line 1587 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\xlocnum"






// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"

namespace std {

		
template<class _Elem,
	class _Traits>
	class basic_ios
		: public ios_base
	{	
public:
	typedef basic_ios<_Elem, _Traits> _Myt;
	typedef basic_ostream<_Elem, _Traits> _Myos;
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	typedef ctype<_Elem> _Ctype;
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;

	explicit  basic_ios(_Mysb *_Strbuf)
		{	
		init(_Strbuf);
		}

	virtual  ~basic_ios()
		{	
		}

	void  clear(iostate _State = goodbit, bool _Reraise = false)
		{	
		ios_base::clear((iostate)(_Mystrbuf == 0
			? (int)_State | (int)badbit : (int)_State), _Reraise);
		}

	void  clear(io_state _State)
		{	
		clear((iostate)_State);
		}

	void  setstate(iostate _State, bool _Reraise = false)
		{	
		if (_State != goodbit)
			clear((iostate)((int)rdstate() | (int)_State), _Reraise);
		}

	void  setstate(io_state _State)
		{	
		setstate((iostate)_State);
		}

	_Myt&  copyfmt(const _Myt& _Right)
		{	
		_Tiestr = _Right.tie();
		_Fillch = _Right.fill();
		ios_base::copyfmt(_Right);
		return (*this);
		}

	_Myos * tie() const
		{	
		return (_Tiestr);
		}

	_Myos * tie(_Myos *_Newtie)
		{	
		_Myos *_Oldtie = _Tiestr;
		_Tiestr = _Newtie;
		return (_Oldtie);
		}

	_Mysb * rdbuf() const
		{	
		return (_Mystrbuf);
		}

	_Mysb * rdbuf(_Mysb *_Strbuf)
		{	
		_Mysb *_Oldstrbuf = _Mystrbuf;
		_Mystrbuf = _Strbuf;
		clear();
		return (_Oldstrbuf);
		}

	locale  imbue(const locale& _Loc)
		{	
		locale _Oldlocale = ios_base::imbue(_Loc);
		if (rdbuf() != 0)
			rdbuf()->pubimbue(_Loc);
		return (_Oldlocale);
		}

	_Elem  fill() const
		{	
		return (_Fillch);
		}

	_Elem  fill(_Elem _Newfill)
		{	
		_Elem _Oldfill = _Fillch;
		_Fillch = _Newfill;
		return (_Oldfill);
		}

	char  narrow(_Elem _Ch, char _Dflt = '\0') const
		{	
		const _Ctype& _Ctype_fac = use_facet<_Ctype >(getloc());
		return (_Ctype_fac.narrow(_Ch, _Dflt));
		}

	_Elem  widen(char _Byte) const
		{	
		const _Ctype& _Ctype_fac = use_facet<_Ctype >(getloc());
		return (_Ctype_fac.widen(_Byte));
		}

protected:
	void  init(_Mysb *_Strbuf = 0,
		bool _Isstd = false)
		{	
		_Init();	
		_Mystrbuf = _Strbuf;
		_Tiestr = 0;
		_Fillch = widen(' ');

		if (_Mystrbuf == 0)
			setstate(badbit);

		if (_Isstd)
			_Addstd(this);	
		else
		_Stdstr = 0;
		}

	 basic_ios()
		{	
		}

private:
	 basic_ios(const _Myt&);	
	_Myt&  operator=(const _Myt&);	

	_Mysb *_Mystrbuf;	
	_Myos *_Tiestr;	
	_Elem _Fillch;	
	};

 








// #line 169 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"

		
inline ios_base& __cdecl boolalpha(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::boolalpha);
	return (_Iosbase);
	}

inline ios_base& __cdecl dec(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::dec, ios_base::basefield);
	return (_Iosbase);
	}

inline ios_base& __cdecl fixed(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::fixed, ios_base::floatfield);
	return (_Iosbase);
	}

inline ios_base& __cdecl hex(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::hex, ios_base::basefield);
	return (_Iosbase);
	}

	namespace tr1 {	
inline std::ios_base& __cdecl hexfloat(std::ios_base& _Iosbase)
	{	
	_Iosbase.setf(std::ios_base::hexfloat, std::ios_base::floatfield);
	return (_Iosbase);
	}
	}	
using tr1::hexfloat;

inline ios_base& __cdecl internal(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::internal, ios_base::adjustfield);
	return (_Iosbase);
	}

inline ios_base& __cdecl left(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::left, ios_base::adjustfield);
	return (_Iosbase);
	}

inline ios_base& __cdecl noboolalpha(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::boolalpha);
	return (_Iosbase);
	}

inline ios_base& __cdecl noshowbase(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::showbase);
	return (_Iosbase);
	}

inline ios_base& __cdecl noshowpoint(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::showpoint);
	return (_Iosbase);
	}

inline ios_base& __cdecl noshowpos(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::showpos);
	return (_Iosbase);
	}

inline ios_base& __cdecl noskipws(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::skipws);
	return (_Iosbase);
	}

inline ios_base& __cdecl nounitbuf(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::unitbuf);
	return (_Iosbase);
	}

inline ios_base& __cdecl nouppercase(ios_base& _Iosbase)
	{	
	_Iosbase.unsetf(ios_base::uppercase);
	return (_Iosbase);
	}

inline ios_base& __cdecl oct(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::oct, ios_base::basefield);
	return (_Iosbase);
	}

inline ios_base& __cdecl right(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::right, ios_base::adjustfield);
	return (_Iosbase);
	}

inline ios_base& __cdecl scientific(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::scientific, ios_base::floatfield);
	return (_Iosbase);
	}

inline ios_base& __cdecl showbase(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::showbase);
	return (_Iosbase);
	}

inline ios_base& __cdecl showpoint(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::showpoint);
	return (_Iosbase);
	}

inline ios_base& __cdecl showpos(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::showpos);
	return (_Iosbase);
	}

inline ios_base& __cdecl skipws(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::skipws);
	return (_Iosbase);
	}

inline ios_base& __cdecl unitbuf(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::unitbuf);
	return (_Iosbase);
	}

inline ios_base& __cdecl uppercase(ios_base& _Iosbase)
	{	
	_Iosbase.setf(ios_base::uppercase);
	return (_Iosbase);
	}
}


 #pragma warning(pop)
 #pragma pack(pop)
// #line 317 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"

// #line 319 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"
// #line 320 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ios"





// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"


 #pragma pack(push,8)
 #pragma warning(push,3)
 #pragma warning(disable: 4390)
// #line 13 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

namespace std {

		

 
 

 



 



 





// #line 36 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

		
template<class _Elem,
	class _Traits>
	class basic_ostream
		: virtual public basic_ios<_Elem, _Traits>
	{	
public:
	typedef basic_ostream<_Elem, _Traits> _Myt;
	typedef basic_ios<_Elem, _Traits> _Myios;
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	typedef ostreambuf_iterator<_Elem, _Traits> _Iter;
	typedef num_put<_Elem, _Iter> _Nput;

	explicit  basic_ostream(basic_streambuf<_Elem, _Traits> *_Strbuf,
		bool _Isstd = false)
		{	
		_Myios::init(_Strbuf, _Isstd);
		}

	 basic_ostream(_Uninitialized, bool _Addit = true)
		{	
		if (_Addit)
			ios_base::_Addstd(this);	
		}

	virtual  ~basic_ostream()
		{	
		}

	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;

	class _Sentry_base
		{	
	public:
		 _Sentry_base(_Myt& _Ostr)
			: _Myostr(_Ostr)
			{	
			if (_Myostr.rdbuf() != 0)
				_Myostr.rdbuf()->_Lock();
			}

		 ~_Sentry_base()
			{	
			if (_Myostr.rdbuf() != 0)
				_Myostr.rdbuf()->_Unlock();
			}

		_Myt& _Myostr;	
		};

	class sentry
		: public _Sentry_base
		{	
	public:
		explicit  sentry(_Myt& _Ostr)
			: _Sentry_base(_Ostr)
			{	
			if (_Ostr.good() && _Ostr.tie() != 0)
				_Ostr.tie()->flush();
			_Ok = _Ostr.good();	
			}

		 ~sentry()
			{	

 
			if (!std:: uncaught_exception())
				this->_Myostr._Osfx();
			}

 


// #line 113 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

		 operator bool() const
			{	
			return (_Ok);
			}

	private:
		 sentry(const sentry&);	
		sentry&  operator=(const sentry&);	

		bool _Ok;	
		};

	bool  opfx()
		{	
		if (ios_base::good() && _Myios::tie() != 0)
			_Myios::tie()->flush();
		return (ios_base::good());
		}

	void  osfx()
		{	
		_Osfx();
		}

	void  _Osfx()
		{	
		try {
		if (ios_base::flags() & ios_base::unitbuf)
			flush();	
		} catch (...) {
		}
		}
























	_Myt&  operator<<(_Myt& (__cdecl *_Pfn)(_Myt&))
		{	
		;
		return ((*_Pfn)(*this));
		}

	_Myt&  operator<<(_Myios& (__cdecl *_Pfn)(_Myios&))
		{	
		;
		(*_Pfn)(*(_Myios *)this);
		return (*this);
		}

	_Myt&  operator<<(ios_base& (__cdecl *_Pfn)(ios_base&))
		{	
		;
		(*_Pfn)(*(ios_base *)this);
		return (*this);
		}

	_Myt&  operator<<(_Bool _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(short _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());
			ios_base::fmtflags _Bfl =
				ios_base::flags() & ios_base::basefield;
			long _Tmp = (_Bfl == ios_base::oct
				|| _Bfl == ios_base::hex)
				? (long)(unsigned short)_Val : (long)_Val;

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Tmp).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

        







	_Myt&  operator<<(unsigned short _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), (unsigned long)_Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(int __w64 _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());
			ios_base::fmtflags _Bfl =
				ios_base::flags() & ios_base::basefield;
			long _Tmp = (_Bfl == ios_base::oct
				|| _Bfl == ios_base::hex)
				? (long)(unsigned int)_Val : (long)_Val;

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Tmp).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(unsigned int __w64 _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), (unsigned long)_Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(long _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(unsigned long __w64 _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), (unsigned long)_Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

 
#if 0
// DQ: removed
	_Myt&  operator<<(__int64 _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(unsigned __int64 _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}
#endif
 // #line 389 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

	_Myt&  operator<<(float _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), (double)_Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(double _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(long double _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(const void *_Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nput& _Nput_fac = use_facet<_Nput >(ios_base::getloc());

			try {
			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
				_Myios::fill(), _Val).failed())
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator<<(_Mysb *_Strbuf)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		bool _Copied = false;
		const sentry _Ok(*this);

		if (_Ok && _Strbuf != 0)
			for (int_type _Meta = _Traits::eof(); ; _Copied = true)
				{	
				try {
				_Meta = _Traits::eq_int_type(_Traits::eof(), _Meta)
					? _Strbuf->sgetc() : _Strbuf->snextc();
				} catch (...) {
					_Myios::setstate(ios_base::failbit);
					throw;
				}

				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					break;	

				try {
					if (_Traits::eq_int_type(_Traits::eof(),
						_Myios::rdbuf()->sputc(
							_Traits::to_char_type(_Meta))))
						{	
						_State |= ios_base::badbit;
						break;
						}
				} catch (...) { _Myios::setstate(ios_base::badbit, true); }
				}

		ios_base::width(0);
		_Myios::setstate(_Strbuf == 0 ? ios_base::badbit
			: !_Copied ? _State | ios_base::failbit : _State);
		return (*this);
		}

	_Myt&  put(_Elem _Ch)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (!_Ok)
			_State |= ios_base::badbit;
		else
			{	
			try {
			if (_Traits::eq_int_type(_Traits::eof(),
				_Myios::rdbuf()->sputc(_Ch)))
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  write(const _Elem *_Str,
		streamsize _Count)
		{	
		if (0 < _Count)
			;
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (!_Ok)
			_State |= ios_base::badbit;
		else
			{	
			try {
			if (_Myios::rdbuf()->sputn(_Str, _Count) != _Count)
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  flush()
		{	
		ios_base::iostate _State = ios_base::goodbit;
		if (!ios_base::fail() && _Myios::rdbuf()->pubsync() == -1)
			_State |= ios_base::badbit;	
		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  seekp(pos_type _Pos)
		{	
		if (!ios_base::fail()
			&& (off_type)_Myios::rdbuf()->pubseekpos(_Pos,
				ios_base::out) == _BADOFF)
			_Myios::setstate(ios_base::failbit);
		return (*this);
		}

	_Myt&  seekp(off_type _Off, ios_base::seekdir _Way)
		{	
		if (!ios_base::fail()
			&& (off_type)_Myios::rdbuf()->pubseekoff(_Off, _Way,
				ios_base::out) == _BADOFF)
			_Myios::setstate(ios_base::failbit);
		return (*this);
		}

	pos_type  tellp()
		{	
		if (!ios_base::fail())
			return (_Myios::rdbuf()->pubseekoff(0,
				ios_base::cur, ios_base::out));
		else
			return (pos_type(_BADOFF));
		}
	};























































 






// #line 648 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

		

 template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>& __cdecl operator<<(
		basic_ostream<_Elem, _Traits>& _Ostr, const char *_Val)
	{	
	ios_base::iostate _State = ios_base::goodbit;
	streamsize _Count = (streamsize)::strlen(_Val);	
	streamsize _Pad = _Ostr.width() <= 0 || _Ostr.width() <= _Count
		? 0 : _Ostr.width() - _Count;
	const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

	if (!_Ok)
		_State |= ios_base::badbit;
	else
		{	
		try {
		const ctype<_Elem>& _Ctype_fac = use_facet<ctype<_Elem> >(_Ostr.getloc());
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}

		for (; _State == ios_base::goodbit && 0 < _Count; --_Count, ++_Val)
			if (_Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ctype_fac.widen(*_Val))))
					_State |= ios_base::badbit;

		if (_State == ios_base::goodbit)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}
		_Ostr.width(0);
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>& __cdecl operator<<(
		basic_ostream<_Elem, _Traits>& _Ostr, char _Ch)
	{	
	ios_base::iostate _State = ios_base::goodbit;
	const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

	if (_Ok)
		{	
		const ctype<_Elem>& _Ctype_fac = use_facet<ctype<_Elem> >(_Ostr.getloc());
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

		try {
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; _State == ios_base::goodbit && 0 < _Pad;
				--_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					_State |= ios_base::badbit;

		if (_State == ios_base::goodbit
			&& _Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ctype_fac.widen(_Ch))))
			_State |= ios_base::badbit;

		for (; _State == ios_base::goodbit && 0 < _Pad;
			--_Pad)	
			if (_Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ostr.fill())))
				_State |= ios_base::badbit;
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr,
		const char *_Val)
	{	
	typedef char _Elem;
	typedef basic_ostream<_Elem, _Traits> _Myos;
	ios_base::iostate _State = ios_base::goodbit;
	streamsize _Count = (streamsize)_Traits::length(_Val);	
	streamsize _Pad = _Ostr.width() <= 0 || _Ostr.width() <= _Count
		? 0 : _Ostr.width() - _Count;
	const typename _Myos::sentry _Ok(_Ostr);

	if (!_Ok)
		_State |= ios_base::badbit;
	else
		{	
		try {
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}

		if (_State == ios_base::goodbit
			&& _Ostr.rdbuf()->sputn(_Val, _Count) != _Count)
			_State |= ios_base::badbit;

		if (_State == ios_base::goodbit)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}
		_Ostr.width(0);
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr, char _Ch)
	{	
	typedef char _Elem;
	typedef basic_ostream<_Elem, _Traits> _Myos;
	ios_base::iostate _State = ios_base::goodbit;
	const typename _Myos::sentry _Ok(_Ostr);

	if (_Ok)
		{	
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

		try {
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; _State == ios_base::goodbit && 0 < _Pad;
				--_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					_State |= ios_base::badbit;

		if (_State == ios_base::goodbit
			&& _Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ch)))
			_State |= ios_base::badbit;

		for (; _State == ios_base::goodbit && 0 < _Pad;
			--_Pad)	
			if (_Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ostr.fill())))
				_State |= ios_base::badbit;
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>& __cdecl operator<<(
		basic_ostream<_Elem, _Traits>& _Ostr, const _Elem *_Val)
	{	
	typedef basic_ostream<_Elem, _Traits> _Myos;
	ios_base::iostate _State = ios_base::goodbit;
	streamsize _Count = (streamsize)_Traits::length(_Val);	
	streamsize _Pad = _Ostr.width() <= 0 || _Ostr.width() <= _Count
		? 0 : _Ostr.width() - _Count;
	const typename _Myos::sentry _Ok(_Ostr);

	if (!_Ok)
		_State |= ios_base::badbit;
	else
		{	
		try {
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}

		if (_State == ios_base::goodbit
			&& _Ostr.rdbuf()->sputn(_Val, _Count) != _Count)
			_State |= ios_base::badbit;

		if (_State == ios_base::goodbit)
			for (; 0 < _Pad; --_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	
					_State |= ios_base::badbit;
					break;
					}
		_Ostr.width(0);
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>& __cdecl operator<<(
		basic_ostream<_Elem, _Traits>& _Ostr, _Elem _Ch)
	{	
	typedef basic_ostream<_Elem, _Traits> _Myos;
	ios_base::iostate _State = ios_base::goodbit;
	const typename _Myos::sentry _Ok(_Ostr);

	if (_Ok)
		{	
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

		try {
		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
			for (; _State == ios_base::goodbit && 0 < _Pad;
				--_Pad)	
				if (_Traits::eq_int_type(_Traits::eof(),
					_Ostr.rdbuf()->sputc(_Ostr.fill())))
					_State |= ios_base::badbit;

		if (_State == ios_base::goodbit
			&& _Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ch)))
			_State |= ios_base::badbit;

		for (; _State == ios_base::goodbit && 0 < _Pad;
			--_Pad)	
			if (_Traits::eq_int_type(_Traits::eof(),
				_Ostr.rdbuf()->sputc(_Ostr.fill())))
				_State |= ios_base::badbit;
		} catch (...) { (_Ostr).setstate(ios_base::badbit, true); }
		}

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr, const signed char *_Val)
	{	
	return (_Ostr << (const char *)_Val);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr, signed char _Ch)
	{	
	return (_Ostr << (char)_Ch);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr, const unsigned char *_Val)
	{	
	return (_Ostr << (const char *)_Val);
	}

template<class _Traits> inline
	basic_ostream<char, _Traits>& __cdecl operator<<(
		basic_ostream<char, _Traits>& _Ostr, unsigned char _Ch)
	{	
	return (_Ostr << (char)_Ch);
	}

		
template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>&
		__cdecl endl(basic_ostream<_Elem, _Traits>& _Ostr)
	{	
	_Ostr.put(_Ostr.widen('\n'));
	_Ostr.flush();
	return (_Ostr);
	}

template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>&
		__cdecl ends(basic_ostream<_Elem, _Traits>& _Ostr)
	{	
	_Ostr.put(_Elem());
	return (_Ostr);
	}

template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>&
		__cdecl flush(basic_ostream<_Elem, _Traits>& _Ostr)
	{	
	_Ostr.flush();
	return (_Ostr);
	}

 inline basic_ostream<char, char_traits<char> >&
	__cdecl endl(basic_ostream<char, char_traits<char> >& _Ostr)
	{	
	_Ostr.put('\n');
	_Ostr.flush();
	return (_Ostr);
	}

 inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl endl(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _Ostr)
	{	
	_Ostr.put('\n');
	_Ostr.flush();
	return (_Ostr);
	}


 inline basic_ostream<char, char_traits<char> >&
	__cdecl ends(basic_ostream<char, char_traits<char> >& _Ostr)
	{	
	_Ostr.put('\0');
	return (_Ostr);
	}

 inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl ends(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _Ostr)
	{	
	_Ostr.put('\0');
	return (_Ostr);
	}


 inline basic_ostream<char, char_traits<char> >&
	__cdecl flush(basic_ostream<char, char_traits<char> >& _Ostr)
	{	
	_Ostr.flush();
	return (_Ostr);
	}

 inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl flush(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _Ostr)
	{	
	_Ostr.flush();
	return (_Ostr);
	}


 

// #line 1018 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

}


 #pragma warning(default: 4390)
 #pragma warning(pop)
 #pragma pack(pop)
// #line 1026 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"

// #line 1028 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"
// #line 1029 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\ostream"






// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

namespace std {

		
template<class _Elem,
	class _Traits>
	class basic_istream
		: virtual public basic_ios<_Elem, _Traits>
	{	
public:
	typedef basic_istream<_Elem, _Traits> _Myt;
	typedef basic_ios<_Elem, _Traits> _Myios;
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	typedef istreambuf_iterator<_Elem, _Traits> _Iter;
	typedef ctype<_Elem> _Ctype;
	typedef num_get<_Elem, _Iter> _Nget;


	explicit  basic_istream(_Mysb *_Strbuf, bool _Isstd = false)
		: _Chcount(0)
		{	
		_Myios::init(_Strbuf, _Isstd);
		}

	 basic_istream(_Uninitialized)
		{	
		ios_base::_Addstd(this);
		}

	virtual  ~basic_istream()
		{	
		}

	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;

		
	class _Sentry_base
		{	
	public:
		 _Sentry_base(_Myt& _Istr)
			: _Myistr(_Istr)
			{	
			if (_Myistr.rdbuf() != 0)
				_Myistr.rdbuf()->_Lock();
			}

		 ~_Sentry_base()
			{	
			if (_Myistr.rdbuf() != 0)
				_Myistr.rdbuf()->_Unlock();
			}

		_Myt& _Myistr;	
		};

	class sentry
		: public _Sentry_base
		{	
	public:
		explicit  sentry(_Myt& _Istr, bool _Noskip = false)
			: _Sentry_base(_Istr)
			{	
			_Ok = this->_Myistr._Ipfx(_Noskip);
			}

		 operator bool() const
			{	
			return (_Ok);
			}

	private:
		 sentry(const sentry&);	
		sentry&  operator=(const sentry&);	

		bool _Ok;	
		};

	bool  _Ipfx(bool _Noskip = false)
		{	
		if (ios_base::good())
			{	
			if (_Myios::tie() != 0)
				_Myios::tie()->flush();

			if (!_Noskip && ios_base::flags() & ios_base::skipws)
				{	
				const _Ctype& _Ctype_fac = use_facet<_Ctype >(ios_base::getloc());

				try {
				int_type _Meta = _Myios::rdbuf()->sgetc();

				for (; ; _Meta = _Myios::rdbuf()->snextc())
					if (_Traits::eq_int_type(_Traits::eof(), _Meta))
						{	
						_Myios::setstate(ios_base::eofbit);
						break;
						}
					else if (!_Ctype_fac.is(_Ctype::space,
						_Traits::to_char_type(_Meta)))
						break;	
				} catch (...) { _Myios::setstate(ios_base::badbit, true); }
				}

			if (ios_base::good())
				return (true);
			}
		_Myios::setstate(ios_base::failbit);
		return (false);
		}

	bool  ipfx(bool _Noskip = false)
		{	
		return _Ipfx(_Noskip);
		}

	void  isfx()
		{	
		}























	_Myt&  operator>>(_Myt& (__cdecl *_Pfn)(_Myt&))
		{	
		;
		return ((*_Pfn)(*this));
		}

	_Myt&  operator>>(_Myios& (__cdecl *_Pfn)(_Myios&))
		{	
		;
		(*_Pfn)(*(_Myios *)this);
		return (*this);
		}

	_Myt&  operator>>(ios_base& (__cdecl *_Pfn)(ios_base&))
		{	
		;
		(*_Pfn)(*(ios_base *)this);
		return (*this);
		}

	_Myt&  operator>>(_Bool& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(short& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			long _Tmp = 0;
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Tmp);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }

			if (_State & ios_base::failbit
				|| _Tmp < (-32768) || 32767 < _Tmp)
				_State |= ios_base::failbit;
			else
				_Val = (short)_Tmp;
			}

		_Myios::setstate(_State);
		return (*this);
		}

        







	_Myt&  operator>>(unsigned short& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(int& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			long _Tmp = 0;
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Tmp);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }

			if (_State & ios_base::failbit
				|| _Tmp < (-2147483647 - 1) || 2147483647 < _Tmp)
				_State |= ios_base::failbit;
			else
				_Val = _Tmp;
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(unsigned int& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(long& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());
			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(unsigned long __w64& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

 
#if 0
// DQ: removed
	_Myt&  operator>>(__int64& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(unsigned __int64& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}
#endif
 // #line 366 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

	_Myt&  operator>>(float& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(double& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(long double& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());
			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(void *& _Val)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const sentry _Ok(*this);

		if (_Ok)
			{	
			const _Nget& _Nget_fac = use_facet<_Nget >(ios_base::getloc());

			try {
			_Nget_fac.get(_Iter(_Myios::rdbuf()), _Iter(0),
				*this, _State, _Val);
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  operator>>(_Mysb *_Strbuf)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		bool _Copied = false;
		const sentry _Ok(*this);

		if (_Ok && _Strbuf != 0)
			{	
			try {
			int_type _Meta = _Myios::rdbuf()->sgetc();

			for (; ; _Meta = _Myios::rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else
					{	
					try {
						if (_Traits::eq_int_type(_Traits::eof(),
							_Strbuf->sputc(_Traits::to_char_type(_Meta))))
							break;
					} catch (...) {
						break;
					}
					_Copied = true;
					}
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(!_Copied ? _State | ios_base::failbit : _State);
		return (*this);
		}

	int_type  get()
		{	
		int_type _Meta = 0;
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (!_Ok)
			_Meta = _Traits::eof();	
		else
			{	
			try {
			_Meta = _Myios::rdbuf()->sbumpc();

			if (_Traits::eq_int_type(_Traits::eof(), _Meta))
				_State |= ios_base::eofbit | ios_base::failbit;	
			else
				++_Chcount;	
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (_Meta);
		}

	_Myt&  get(_Elem *_Str, streamsize _Count)
		{	
		return (get(_Str, _Count, _Myios::widen('\n')));
		}

	_Myt&  get(_Elem *_Str,
		streamsize _Count, _Elem _Delim)
		{	
		;
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok && 0 < _Count)
			{	
			try {
			int_type _Meta = _Myios::rdbuf()->sgetc();

			for (; 0 < --_Count; _Meta = _Myios::rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else if (_Traits::to_char_type(_Meta) == _Delim)
					break;	
				else
					{	
					*_Str++ = _Traits::to_char_type(_Meta);
					++_Chcount;
					}
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_Chcount == 0
			? _State | ios_base::failbit : _State);
		*_Str = _Elem();	
		return (*this);
		}

	_Myt&  get(_Elem& _Ch)
		{	
		int_type _Meta = get();
		if (!_Traits::eq_int_type(_Traits::eof(), _Meta))
			_Ch = _Traits::to_char_type(_Meta);
		return (*this);
		}

	_Myt&  get(_Mysb& _Strbuf)
		{	
		return (get(_Strbuf, _Myios::widen('\n')));
		}

	_Myt&  get(_Mysb& _Strbuf, _Elem _Delim)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok)
			{	
			try {
			int_type _Meta = _Myios::rdbuf()->sgetc();

			for (; ; _Meta = _Myios::rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else
					{	
					try {
						_Elem _Ch = _Traits::to_char_type(_Meta);
						if (_Ch == _Delim
							|| _Traits::eq_int_type(_Traits::eof(),
								_Strbuf.sputc(_Ch)))
							break;
					} catch (...) {
						break;
					}
					++_Chcount;
					}
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		if (_Chcount == 0)
			_State |= ios_base::failbit;
		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  getline(_Elem *_Str, streamsize _Count)
		{	
		return (getline(_Str, _Count, _Myios::widen('\n')));
		}

	_Myt&  getline(_Elem *_Str,
		streamsize _Count, _Elem _Delim)
		{	
		;
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok && 0 < _Count)
			{	
			int_type _Metadelim = _Traits::to_int_type(_Delim);

			try {
			int_type _Meta = _Myios::rdbuf()->sgetc();

			for (; ; _Meta = _Myios::rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else if (_Meta == _Metadelim)
					{	
					++_Chcount;
					_Myios::rdbuf()->sbumpc();
					break;
					}
				else if (--_Count <= 0)
					{	
					_State |= ios_base::failbit;
					break;
					}
				else
					{	
					++_Chcount;
					*_Str++ = _Traits::to_char_type(_Meta);
					}
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		*_Str = _Elem();	
		_Myios::setstate(_Chcount == 0 ? _State | ios_base::failbit : _State);
		return (*this);
		}

	_Myt&  ignore(streamsize _Count = 1,
		int_type _Metadelim = _Traits::eof())
		{	
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok && 0 < _Count)
			{	
			try {
			for (; ; )
				{	
				int_type _Meta;
				if (_Count != 2147483647 && --_Count < 0)
					break;	
				else if (_Traits::eq_int_type(_Traits::eof(),
					_Meta = _Myios::rdbuf()->sbumpc()))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else
					{	
					++_Chcount;
					if (_Meta == _Metadelim)
						break;	
					}
				}
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  _Read_s(_Elem *_Str, size_t _Str_size, streamsize _Count)
		{	
		;
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok)
			{	
			try {
			const streamsize _Num = _Myios::rdbuf()->_Sgetn_s(_Str, _Str_size, _Count);
			_Chcount += _Num;
			if (_Num != _Count)
				_State |= ios_base::eofbit | ios_base::failbit;	
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  read(_Elem *_Str, streamsize _Count)
		{
		return _Read_s(_Str, (size_t)-1, _Count);
		}

	streamsize  _Readsome_s(_Elem *_Str, size_t _Str_size, streamsize _Count)
		{	
		;
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		streamsize _Num;

		if (!_Ok)
			_State |= ios_base::failbit;	
		else if ((_Num = _Myios::rdbuf()->in_avail()) < 0)
			_State |= ios_base::eofbit;	
		else if (0 < _Num)
			_Read_s(_Str, _Str_size, _Num < _Count ? _Num : _Count);	

		_Myios::setstate(_State);
		return (gcount());
		}

	__declspec(deprecated("Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. " "To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'"))
	streamsize  readsome(_Elem *_Str, streamsize _Count)
		{
		return _Readsome_s(_Str, (size_t)-1, _Count);
		}

	int_type  peek()
		{	
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		int_type _Meta = 0;
		const sentry _Ok(*this, true);

		if (!_Ok)
			_Meta = _Traits::eof();	
		else
			{	
			try {
			if (_Traits::eq_int_type(_Traits::eof(),
				_Meta = _Myios::rdbuf()->sgetc()))
				_State |= ios_base::eofbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (_Meta);
		}

	_Myt&  putback(_Elem _Ch)
		{	
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok)
			{	
			try {
			if (_Traits::eq_int_type(_Traits::eof(),
				_Myios::rdbuf()->sputbackc(_Ch)))
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	_Myt&  unget()
		{	
		ios_base::iostate _State = ios_base::goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);

		if (_Ok)
			{	
			try {
			if (_Traits::eq_int_type(_Traits::eof(),
				_Myios::rdbuf()->sungetc()))
				_State |= ios_base::badbit;
			} catch (...) { _Myios::setstate(ios_base::badbit, true); }
			}

		_Myios::setstate(_State);
		return (*this);
		}

	streamsize  gcount() const
		{	
		return (_Chcount);
		}

	int  sync()
		{	
		ios_base::iostate _State = ios_base::goodbit;
		int _Ans;

		if (_Myios::rdbuf() == 0)
			_Ans = -1;	
		else if (_Myios::rdbuf()->pubsync() == -1)
			{	
			_State |= ios_base::badbit;
			_Ans = -1;
			}
		else
			_Ans = 0;	

		_Myios::setstate(_State);
		return (_Ans);
		}

	_Myt&  seekg(pos_type _Pos)
		{	
		if (!ios_base::fail()
			&& (off_type)_Myios::rdbuf()->pubseekpos(_Pos,
				ios_base::in) == _BADOFF)
			_Myios::setstate(ios_base::failbit);
		else
			_Myios::clear();	
		return (*this);
		}

	_Myt&  seekg(off_type _Off, ios_base::seekdir _Way)
		{	
		if (!ios_base::fail()
			&& (off_type)_Myios::rdbuf()->pubseekoff(_Off, _Way,
				ios_base::in) == _BADOFF)
			_Myios::setstate(ios_base::failbit);
		else
			_Myios::clear();	
		return (*this);
		}

	pos_type  tellg()
		{	
		if (!ios_base::fail())
			return (_Myios::rdbuf()->pubseekoff(0,
				ios_base::cur, ios_base::in));
		else
			return (pos_type(_BADOFF));
		}

private:
	streamsize _Chcount;	
	};









































 






// #line 896 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

		
template<class _Elem,
	class _Traits>
	class basic_iostream
	: public basic_istream<_Elem, _Traits>,
		public basic_ostream<_Elem, _Traits>
	{	
public:
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;

	explicit  basic_iostream(basic_streambuf<_Elem, _Traits> *_Strbuf)
		: basic_istream<_Elem, _Traits>(_Strbuf, false),
			basic_ostream<_Elem, _Traits>(_Noinit, false)
		{	
		}

	virtual  ~basic_iostream()
		{	
		}
	};

 






// #line 930 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

		
template<class _Elem,
	class _Traits> inline
	basic_istream<_Elem, _Traits>& __cdecl operator>>(
		basic_istream<_Elem, _Traits>& _Istr, _Elem *_Str)
	{	
	;
	typedef basic_istream<_Elem, _Traits> _Myis;
	typedef ctype<_Elem> _Ctype;
	ios_base::iostate _State = ios_base::goodbit;
	_Elem *_Str0 = _Str;
	const typename _Myis::sentry _Ok(_Istr);

	if (_Ok)
		{	
		const _Ctype& _Ctype_fac = use_facet<_Ctype >(_Istr.getloc());

		try {
		streamsize _Count = 0 < _Istr.width() ? _Istr.width() : 2147483647;
		typename _Myis::int_type _Meta = _Istr.rdbuf()->sgetc();
		_Elem _Ch;
		for (; 0 < --_Count; _Meta = _Istr.rdbuf()->snextc())
			if (_Traits::eq_int_type(_Traits::eof(), _Meta))
				{	
				_State |= ios_base::eofbit;
				break;
				}
			else if (_Ctype_fac.is(_Ctype::space,
				_Ch = _Traits::to_char_type(_Meta))
					|| _Ch == _Elem())
				break;	
			else
				*_Str++ = _Traits::to_char_type(_Meta);	
		} catch (...) { (_Istr).setstate(ios_base::badbit, true); }
		}

	*_Str = _Elem();	
	_Istr.width(0);
	_Istr.setstate(_Str == _Str0 ? _State | ios_base::failbit : _State);
	return (_Istr);
	}

template<class _Elem,
	class _Traits> inline
	basic_istream<_Elem, _Traits>& __cdecl  operator>>(
		basic_istream<_Elem, _Traits>& _Istr, _Elem& _Ch)
	{	
	typedef basic_istream<_Elem, _Traits> _Myis;
	typename _Myis::int_type _Meta;
	ios_base::iostate _State = ios_base::goodbit;
	const typename _Myis::sentry _Ok(_Istr);

	if (_Ok)
		{	
		try {
		_Meta = _Istr.rdbuf()->sbumpc();
		if (_Traits::eq_int_type(_Traits::eof(), _Meta))
			_State |= ios_base::eofbit | ios_base::failbit;	
		else
			_Ch = _Traits::to_char_type(_Meta);	
		} catch (...) { (_Istr).setstate(ios_base::badbit, true); }
		}

	_Istr.setstate(_State);
	return (_Istr);
	}


template<class _Traits> inline
	basic_istream<char, _Traits>& __cdecl  operator>>(
		basic_istream<char, _Traits>& _Istr, signed char *_Str)
	{	
	return (_Istr >> (char *)_Str);
	}

template<class _Traits> inline
	basic_istream<char, _Traits>& __cdecl  operator>>(
		basic_istream<char, _Traits>& _Istr, signed char& _Ch)
	{	
	return (_Istr >> (char&)_Ch);
	}

template<class _Traits> inline
	basic_istream<char, _Traits>& __cdecl  operator>>(
		basic_istream<char, _Traits>& _Istr, unsigned char *_Str)
	{	
	return (_Istr >> (char *)_Str);
	}

template<class _Traits> inline
	basic_istream<char, _Traits>& __cdecl  operator>>(
		basic_istream<char, _Traits>& _Istr, unsigned char& _Ch)
	{	
	return (_Istr >> (char&)_Ch);
	}

// #line 1028 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

		
template<class _Elem,
	class _Traits> inline
	basic_istream<_Elem, _Traits>&
		__cdecl ws(basic_istream<_Elem, _Traits>& _Istr)
	{	
	typedef basic_istream<_Elem, _Traits> _Myis;
	typedef ctype<_Elem> _Ctype;

	if (!_Istr.eof())
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const typename _Myis::sentry _Ok(_Istr, true);

		if (_Ok)
			{	
			const _Ctype& _Ctype_fac = use_facet<_Ctype >(_Istr.getloc());

			try {
			for (typename _Traits::int_type _Meta = _Istr.rdbuf()->sgetc(); ;
				_Meta = _Istr.rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else if (!_Ctype_fac.is(_Ctype::space,
					_Traits::to_char_type(_Meta)))
					break;	
			} catch (...) { (_Istr).setstate(ios_base::badbit, true); }
			}

		_Istr.setstate(_State);
		}
	return (_Istr);
	}

 inline basic_istream<char, char_traits<char> >&
	__cdecl ws(basic_istream<char, char_traits<char> >& _Istr)
	{	
	typedef char _Elem;
	typedef char_traits<_Elem> _Traits;

	if (!_Istr.eof())
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const basic_istream<_Elem, _Traits>::sentry _Ok(_Istr, true);

		if (_Ok)
			{	
			const ctype<_Elem>& _Ctype_fac =
				use_facet<ctype<_Elem> >(_Istr.getloc());

			try {
			for (_Traits::int_type _Meta = _Istr.rdbuf()->sgetc(); ;
				_Meta = _Istr.rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else if (!_Ctype_fac.is(ctype<_Elem>::space,
					_Traits::to_char_type(_Meta)))
					break;	
			} catch (...) { (_Istr).setstate(ios_base::badbit, true); }
			}

		_Istr.setstate(_State);
		}
	return (_Istr);
	}

 inline basic_istream<wchar_t, char_traits<wchar_t> >&
	__cdecl ws(basic_istream<wchar_t, char_traits<wchar_t> >& _Istr)
	{	
	typedef wchar_t _Elem;
	typedef char_traits<_Elem> _Traits;

	if (!_Istr.eof())
		{	
		ios_base::iostate _State = ios_base::goodbit;
		const basic_istream<_Elem, _Traits>::sentry _Ok(_Istr, true);

		if (_Ok)
			{	
			const ctype<_Elem>& _Ctype_fac =
				use_facet<ctype<_Elem> >(_Istr.getloc());

			try {
			for (_Traits::int_type _Meta = _Istr.rdbuf()->sgetc(); ;
				_Meta = _Istr.rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
					{	
					_State |= ios_base::eofbit;
					break;
					}
				else if (!_Ctype_fac.is(ctype<_Elem>::space,
					_Traits::to_char_type(_Meta)))
					break;	
			} catch (...) { (_Istr).setstate(ios_base::badbit, true); }
			}

		_Istr.setstate(_State);
		}
	return (_Istr);
	}



 



















// #line 1159 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"
}


 #pragma warning(pop)
 #pragma pack(pop)
// #line 1165 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"

// #line 1167 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"
// #line 1168 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\istream"






// #line 7 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"


 #pragma pack(push,8)
 #pragma warning(push,3)
// #line 12 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"
namespace std {

		

 extern  istream *_Ptr_cin;
 extern  ostream *_Ptr_cout;
 extern  ostream *_Ptr_cerr;
 extern  ostream *_Ptr_clog;
// #line 21 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"


		
class  _Winit {
public:
	__thiscall _Winit();
	__thiscall ~_Winit();
private:
	 static int _Init_cnt;
	};
// #line 32 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"

		

 extern  wistream *_Ptr_wcin;
 extern  wostream *_Ptr_wcout;
 extern  wostream *_Ptr_wcerr;
 extern  wostream *_Ptr_wclog;
// #line 40 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"












 extern  istream cin;
 extern  ostream cout;
 extern  ostream cerr;
 extern  ostream clog;

 extern  wistream wcin;
 extern  wostream wcout;
 extern  wostream wcerr;
 extern  wostream wclog;
// #line 62 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"
}

 #pragma warning(pop)
 #pragma pack(pop)
// #line 67 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"

// #line 69 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"
// #line 70 "c:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\INCLUDE\\iostream"





// #line 3 "hello2.cxx"

int main() {
  std::cout << "Hello, world!\n";
  return 0;
}
