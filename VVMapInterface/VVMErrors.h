#ifndef _VVM_ERRORS_H_INCLUDED__120105
#define _VVM_ERRORS_H_INCLUDED__120105


//////////////////////////////////////////////////////////////////////////////

#define	ERR_NO_ERROR				0		// operation has succeded
#define	ERR_OK						ERR_NO_ERROR

/// недостаточно памяти (никогда не вернет)
#define ERR_OUT_OF_MEMORY			(-1)

/// возникает, тогда когда в качестве параметра функции передается нулевой указатель, в то время как функция  требует валидный указатель
#define ERR_BAD_ADDRESS				-2
/// индекс вышел за пределы массива
#define ERR_INVALID_INDEX			-3
#define ERR_INVALID_BITMAP			-4
#define ERR_INVALID_SIZE			-5
#define ERR_MASTERS_UNDEFINED		-6
#define ERR_MERGE_DONT_MATCH		7
#define ERR_MERGE_UNABLE			8
#define ERR_NO_MERGE				9
#define ERR_TRANSITION_UNDEFINED	-10
#define ERR_NO_POINTS				-11
#define ERR_INVALID_FORMAT			-12
#define ERR_NO_LIST					-13
#define ERR_COMPILE_ERROR			-14
#define ERR_BITMAPINFO_REQUIRED		-15
#define ERR_BITMAP_REQUIRED			-16
#define ERR_TABLE_UNDEFINED			17
#define ERR_ID_NOT_PRESENT			18
#define ERR_NO_PROCESS_RUNNING		19
#define	ERR_PROCESS_STOPPED			20
#define	ERR_WRONG_OPTION			-21
#define	ERR_INVALID_RECT			22
#define	ERR_TRANSITION_INFINITY		23
#define ERR_TOO_MANY_OBJECTS		24
#define ERR_CANNOT_ALLOCATE			25
#define ERR_WRONG_SP				-26
#define ERR_BAD_SCALE				-27
#define ERR_COORDS_UNDEFINABLE		-28
#define ERR_WRONG_ZONE				29
#define ERR_POINT_INVISIBLE			30
#define ERR_SP_NOT_VISIBLE			31

#define	ERR_MAP_NOT_ALIGNED			-32
#define ERR_WRONG_RP				-33
#define ERR_COLORS_NOT_SET			34
#define ERR_NOT_PROCESSED			-35
#define ERR_MAP_NOT_COMPILED		-36
#define ERR_CANNOT_ALIGN			-37
#define ERR_EXCEPTION_OCCURED		-38


#define ERR_CRITSECTION_OCCUPIED	39 
#define ERR_HORIZONT_NOT_FOUND		40 

#define ERR_INVALID_MASTERS			-41

#define ERR_POINT_OUT_OF_RECT		44
#define ERR_POINT_OUT_OF_RECT_SRC	45
#define ERR_POINT_INVISIBLE_SRC		46

#define ERR_INVALID_ARGUMENT		-42
#define ERR_FILE_NOT_FOUND_IN_ZIP	-43

#define ERR_UNKNOWN					-47

//NOTENOTE: 10/06/2004
#define	ERR_NOT_IMPLEMENTED			100		// this function is not yet implemented
//#define ERR_OUT_OF_MEMORY			1099

//#ifndef READING_ERROR_CODES

#define VVMINT_EDITOR		1
#define VVMINT_COMMUTATOR	2
#define VVMINT_CLIENT		3
#define VVMINT_MASTER		4

//////////////////////////////////////////

#define 	GP_SAVE_MAP_IMAGE			10000	//	сохранять изображение карты

#define 	GP_SAVE_MASTERS_IMAGES		10001	//	сохранять изображения мастеров

#define		GP_ENABLE_VIS_MAP			10002	//	рассчитывать/отображать/сохранять карту видимости

#define		GP_ENABLE_EXTENTS			10003	// рассчитывать ExtentTable

#define		GP_NO_PARAM					-777

//////////////////////////////////////////

#endif