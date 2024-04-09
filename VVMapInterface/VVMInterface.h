#ifndef VVMINTERFACE_H_INCLUDED
#define VVMINTERFACE_H_INCLUDED

#ifndef VVMDLL_API
#define VVMDLL_API
#endif

#include <vector>
#include <list>

//--------------------------
#include "VVMErrors.h"
//Все макросы ошибок - теперь там
//--------------------------


struct CObjectRecord;

//NOTENOTE: Eugene's disclaiment 10/06/2004
//NOTENOTE: Все послдедующие функции добавляйте только в конец класса, 
//NOTENOTE: для совестимости с предыдущими бинарниками / VVMapEditor, DkClient /, 
//NOTENOTE: которые их не использует, но требуют предыдущий Vtbl
//NOTENOTE: это касается только CVVMapTrans, в производных в любом порядке

/// класс CVVMapTrans - Главный класс VVMapInterface
/**
	Все функции возвращают 0, если успех, не ноль, если ошибка.
	Текст, описывающий ошибку можно получить через функцию GetErrorString
*/
class CVVMapTrans 
{
public:



	/// получить строку с последней ошибкой
	virtual char* GetErrorString(long ErrorID)=0;

	/// удалить CVVMapTrans
	virtual long _Release()=0;
	
	/// GetInterfaceVersion() получить строку с версией интерфейса
	virtual const char * GetInterfaceVersion()=0;

	///Показать диалог настроек.
	/**
		hParent - окно-родитель (можно 0)
		Не возвращает управления, пока диалог не будет закрыт.
	*/
	virtual long RunOptionsDialog(HWND hParent)=0;
	
	
///////////////////////////////////////////////////////////////////////////
	/// Установливает число мастеров в системе
	/**
		N - число мастеров
	*/
	virtual long SetNumOfMasters(long N)=0;
	
	/// позволяет получить число мастеров в системе
	/**
		[OUT] *pN - число мастеров, установленных в системе.
	*/
	virtual long GetNumOfMasters(long* pN)=0;
	
	/// Установить идентификатор(ID) для мастера
	/** 
		[IN] index - порядковый номер мастера (от 0 до N-1, где N - число мастеров)
		[IN] ID - идентификатор мастера. Если -1, то мастер отсутствует.
	*/
	virtual long SetMasterID(long index, long ID)=0;
	
	/// Получить идентификатор для мастера (ID)
	/**
		[IN] index - порядковый номер мастера (от 0 до N-1, где N - число мастеров)
		[OUT] *pID - идентификатор мастера. Если -1, то мастер отсутствует.
	*/
	virtual long GetMasterID(long index, long* pID)=0;
	
	/// Установить изображение для мастера
	/**
		[IN] index - порядковый номер мастера (от 0 до N-1, где N - число мастеров)
		[IN] pbi, bits - изображение
		Позволяет записать Device-Independent Bitmap. 
	*/
	virtual long SetMasterBitmap(long index, PBITMAPINFO pbi, LPBYTE bits)=0;
	
	/// Получить изображение, записанное SetMasterBitmap
	/**
		[IN] index - порядковый номер мастера (от 0 до N-1, где N - число мастеров)
		[OUT] *ppbi, pbits - указатели на изображение (на внутренний буфер).
	*/
	virtual long GetMasterBitmap(long index, PBITMAPINFO* ppbi, LPBYTE* pbits)=0;
	
	/// Установить изображение для карты
	/**
		pbi, bits - изображение
		Позволяет записать Device-Independent Bitmap. 
		Т.к. записанное туда изображение никак не используется (кроме отображения карты видимости),
		можно эту функции не вызывать, однако, в таком случае, необходимо в ручную установить размер карты
		(см. SetMapSize)
	*/
	
	virtual long SetMapBitmap(PBITMAPINFO pbi, LPBYTE bits)=0;

	/// Получить изображение карты
	/**
		*ppbi, pbits - указатели на изображение (на внутренний буфер)
		Позволяет записать Device-Independent Bitmap. 
		Т.к. записанное туда изображение никак не используется (кроме отображения карты видимости),
		можно эту функции не вызывать, однако, в таком случае, необходимо в ручную установить размер карты
		(см. SetMapSize)
	*/
	virtual long GetMapBitmap(PBITMAPINFO* ppbi, LPBYTE* pbits)=0;
	

	/// Установить опорных список точек 
	/**
		N - количество точек
		arr - массив координат. Размер массива size=N*(NumOfMasters+1)*2
		Структура массива. N блоков по 2*(NumOfMasters+1) элементов типа long
		Структура блока. { XMap,YMap, X1,Y1, X2,Y2, ..., Xn, Yn},  где:
				n=NumOf Masters,
				XMap, YMap - координаты точки на карте,
				Xk,Yk - координаты на мастере с порядковым номеров k.
		Если информация отсутсвует для мастера или для карты, то обе координаты равны по -1.
		Информация копируется из массива arr.
	*/
	virtual long SetPointList(long N, long *arr)=0;
	
	/// Получить опорных список точек
	/**
		*pN - количестов точек
		*parr - массив координат. Размер массива size=N*(NumOfMasters+1)*2.
		Полученные указатели ссылаются на внутренний буфер, поэтому валидны пока существует класс
		или не вызваны SetPointList или GetPointList (тогда сформируется другой буфер).
	*/
	virtual long GetPointList(long* pN, long **parr)=0;
	
	
/////////////////////////////////////////////////////////////////////////////
	// Склеить автоматически два изображения (не работает)
	//[IN] indexleft - Индекс левого мастера
	//[IN] indexright - Индекс правого мастера
	virtual long Merge(long indexleft, long indexright)=0;
		
	// Удалить автоматическую склейку двух изображений
	//[IN] indexleft - Индекс левого мастера
	//[IN] indexright - Индекс правого мастера
	virtual long DeleteMerge(long indexleft, long indexright)=0;
	
	// Проверить существует ли автоматическая склейка двух изображений
	// Возвращает 0, если существет
	//[IN] indexleft - Индекс левого мастера
	//[IN] indexright - Индекс правого мастера
	virtual long CheckMerge(long indexleft, long indexright)=0;
	
	///Получить координаты *px2, *py2 на масере indexto по координатам x1,y1 мастера indexfrom
	/**
		Полученные координаты могут выходить за пределы картинки.
		Если indexfrom или indexto равны NumOfMasters, то имеется в виду карта.
		См. руководство пользователя.
	*/
	virtual long GetXY(long indexfrom, long x1, long y1, long indexto, long *px2, long *py2)=0;
	
	//	Преобразовать массив координат с мастера indexfrom на мастер indexto (не работает)
	
	virtual long GetXYSeries(long num, long indexfrom, long indexto, POINT *From,POINT *To)=0;
	
	///Получить показатель "качества" преобразования, от 0 до 100

	/**
		0 - наихудшее, 100 - наилучшее.
		См. руководство пользователя.
	*/
	virtual long GetTransQuality(long indexfrom,long indexto, long *pQ)=0;
	

/////////////////////////////////////////////////////////////////////////////

	/// компиляция. может занимать некоторое время в зависимости от настроек
	virtual long Compile()=0;
	//Склеить карту и мастера по массиву точек, установленных с помощью SetPointList,
	//и с учетом реузльтатов произведенной автоматической склейки.
	//А также создать таблицу размеров.

	/// Подготовить буфер для записи в файл. 
	/**
		[OUT] *pbits - информация
		[OUT] *pN - размер информации в байтах
		указатели ссылаются на внутренний буфер.
		Буфер имеет структуру zip-файла
	*/
	virtual long Save(void **pbits, long *pN)=0;
	
	/// Подготовить буфер для записи в файл по возможности наименьшего размера.
	/**
		аналогично Save. Но не записывает изображения, переданные SetMasterBitmap и SetMapBitmap
	*/
	virtual long SaveShort(void **pbits, long *pN)=0;
	
	/// Загрузить из буфера
	/**
		bits - информация
		N - размер информации в байтах
		поддерживает форматы vme начиная с сентября 2004 года
	*/
	virtual long Load(void *bits, long N)=0;
	//
	
	//Информация копируется из массива bits.

/////////////////////////////////////////////////////////////////////////////
	/// получить многоуголник на карте, соответсвующий мастеру с порядковым номером masterIndex
	/**
		masterIndex - порядковый номер мастера
		[OUT] *pN - число точек в полигоне
		[OUT] *pp - массив точек полигона
		Указатели ссылаются на внутренний буфер. Содержимое валидно, пока существует класс 
		или не сделана перекомпиляция. (тогда будет создан другой буфер)
	*/
	virtual long GetMapPolygon(long masterIndex, long *pN, POINT **pp)=0;
	
	/// получить многоугольник отображения области видимости мастера с порядковым masterIndex на мастер masterIndexTo
	/**
		masterIndexFrom - порядковый номер мастера, область видимости которого хотим увидеть
		masterIndexTo - порядковый номер мастера на котором строится многоугольник
		*pN - число точек в массиве
		**pp - массив точек
		Указатели ссылаются на внутренний буфер. Содержимое валидно, пока существует класс 
		или не сделана перекомпиляция. (тогда будет создан другой буфер)
	*/
	virtual long GetMasterPolygon(long masterIndexFrom, long masterIndexTo, long *pN, POINT **pp)=0;
		
	/// Установить список рамок человека для таблицы размеров
	/**
		[IN] masterIndex - номер мастера
		[IN] N - количество рамок.
		[IN] rectArr - массив рамок
	*/
	virtual long SetExtentList(long masterIndex, long N, PRECT rectArr)=0;
	
	/// Установить список рамок человека для таблицы размеров
	/**
		[IN] masterIndex - номер мастера
		[OUT] pN - указатель на количество рамок
		[OUT] *prectArr - массив рамок.
		Указатели ссылаются на внутренний буфер.
		Массив удаляется вместе с экземляром класса или при вызове функций GetExtentList,SetExtentList
	*/
	virtual long GetExtentList(long masterIndex, long* pN, PRECT *prectArr)=0;
	
	// не реализовано
	virtual long SaveExtentTable(long masterIndex, void **pbits, long *pN)=0;

	//Сохранить таблицу размеров в сжатом виде (без изображений) для мастера masterIndex	
	// не реализовано
	virtual long SaveExtentTableShort(long masterIndex, void **pbits, long *pN)=0;

	///Загрузить таблицу размеров для мастера masterIndex
	/**	Если masterIndex=-1, то таблица загружается только если в ней есть инофрмация об ID мастера и
		этот ID присутствует в списке мастеров.
		не реализовано
	*/
	virtual long LoadExtentTable(long masterIndex, void *bits, long N)=0;
	
	/// Получить таблицу размеров
	/**
		[IN] masterIndex - номер мастера
		[OUT] *ptable - таблица размеров
		[OUT] pSize - указатель на размер таблицы в байтах

		Примечание:
			(*ptable)[0] и (*ptable)[1] - ширина и высота таблицы соответсвенно

	*/
	virtual long GetExtentTable(long masterIndex, long **ptable, long *pSize)=0;
	


///////////////////////////////////////////////////////////////////////////

	/// Возвращает прогресс работы функции Compile, от 0 до 100
	virtual long GetProgress(long *pP)=0;
	
	///Останавливает запущенную функцию (работает только с Compile)
	virtual long StopProcess()=0;
	

///////////////////////////////////////////////////////////////////////////

	/// Возвращает список объектов на мастере index2, соответствующих
	/**
		Возвращает список объектов на мастере index2, соответствующих объектам на index1
		[IN]index1, index2 - номера мастеров
		[IN]Obj1,Obj2 - массивы, содержащие координаты объектов на мастерах 1 и 2 соотв.
		[IN]N1,N2 - размеры массивов obj1 и Obj2
		[OUT]IDs - массив размера N1. IDs[i] равен номеру объекта в массиве Obj2,
		соответствующего i-тому объекту в Obj1; или -1, если соответствия не нашлось
		Массив удаляется вместе с классом или при следующем вызове GetSameObjects
	*/
	virtual long GetSameObjects(long index1,long index2,CObjectRecord *Obj1,CObjectRecord *Obj2,long N1,long N2,long **IDs)=0;
	
	
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////
// По "дополнению 1"
///////////////////////////////////

	///Добавляет опорную точку с измеренными координатами.
	/**
		В pN получаем номер точки
	*/
virtual long SP_AddAbsolute(long *pN,double X,double Y,double Z)=0;
	
	///Задать (именно задать, а не добавить!) список опорных точек.
	/**
		N - число точек, Coords - координаты X,Y,Z для всех точек подряд
		Calc - массив номеров - если Calc[i]>=0, то i-тая точка задана
		относительными координатами, отн. точки Calc[i]
		Уничтожает ранее заданные точки; номера после SP_AddList идут подряд
	*/	
virtual long SP_AddList(long N, double *Coords, long *Calc)=0;

	///Добавляет опорную точку с координатами, измеренными относительно уже заданной точки с номером P
	/**
		В pN получаем номер точки
		Угол Psi задается не относительно какого-то там выбранного 
		направления, а относительно оси Y!
	*/
virtual long SP_AddRelative(long *pN,long P,double Ro,double Phi,double Psi)=0;
	
	///Получить абсолютные координаты опорной точки с номером N
	/**
		Если С>=0, то координаты были не заданы, а вычислены относительно
		точки с номером С
	*/
virtual long SP_GetAbsolute(long N,double *X,double *Y,double *Z,long *C)=0;
	
	/// Получить координаты точки N1 относительно N2.
virtual long SP_GetRelative(long N1,long N2,double *Ro,double *Phi,double *Psi)=0;

	/// Удалить точку с номером N из списка.
virtual long SP_Remove(long N)=0;
	
	///Удалить все точки.
virtual long SP_Clear()=0;
	
	///Заранее выделить буфер на N опорных точек
	/**
		Буфер будет увеличиваться автоматически, функция на случай если число известно заранее
	*/
virtual long SP_PreAllocate(long N)=0;
	
	///Получить число заданных опорных точек
	/**
		Не обязательно равно максимальному номеру точки
		Непосредственно после загрузки файла - число точек равно 
		максимальному номеру, т.е. номера идут подряд
	*/
virtual long SP_GetNumOfPoints(long *pN)=0;
	

//////////////////////////////////
// По "Дополнению 2"
//////////////////////////////////

////////////// Устарели, сейчас неиспльзуются /////////////

/// [Deprecated] Задать масштаб карты: X(метры)*sX=Xm(пиксели)
virtual long SetMapScale(double sX,double sY)=0;

/// [Deprecated] Получить масштаб карты
virtual long GetMapScale(double *pX,double *pY)=0;
	
/// [Deprecated] Задать положение начала координат в пикселях
virtual long SetMapOrigin(long X,long Y)=0;
	
/// [Deprecated] Получить положение начала координат в пикселях
virtual long GetMapOrigin(long *pX,long *pY)=0;
	

////////////// ////////////////// /////////////

///Автоматически выравнивает карту (т.е., масштаб, полодение нуля и тп) по заданным "экранным" координатам трех опорных точек
/**
	N - число точек, по которым выравниваем (трех достаточно, но, может, кому больше понадобится?)
	SP_nums - номера точек (в списке SP)
	MapCoords - координаты этих точек на карте
	*/
virtual long AlignMap(long N,long *SP_nums,long *MapCoords)=0;
	
///Переводит физические координаты в координаты на карте (в пикселях)
/**
	Если AlignMap не вызывался - использует заданные Scale и Origin,
	иначе - найденное преобразование
*/
virtual long GetMapCoords(double X,double Y,double Z,long *px,long *py)=0;
	
/// Переводит координаты на карте (x,y - в пикселях) в физические 
/**
	Координата Z устанавливается в 0
*/
virtual long GetCoordsFromMap(double *X,double *Y,double*Z,long x,long y)=0;
	
	

///////////////////////////////////////////////////
/*
	- Эти все устарели и их я щас выкину!
	- Я те выкину!
*/
///////////////////////////////////////////////////

//Задать номера опорных точек, по расстояниям до которых
//определяются координаты на карте
//pNums - массив из 4-х номеров
// не реализовано
virtual long SP_SetDefining(long * /*pNums*/) {  return ERR_NOT_IMPLEMENTED; }
	
//Получить номера этих опорных точек
// не реализовано
virtual long SP_GetDefining(long ** /*ppNums*/) {  return ERR_NOT_IMPLEMENTED; }

// Получить номера этих опорных точек
// не реализовано
virtual long GetCoordsFromDistances(double * /*R*/,long * /*pX*/,long * /*pY*/) {  return ERR_NOT_IMPLEMENTED; }

//Получить координаты точки, заданной расстояниями до
//четырех опорных
//R - массив расстояний, pX, pY - координаты точки на карте
// не реализовано
virtual long GetAbsCoordsFromDistances(double * /*R*/,double * /*pX*/,double * /*pY*/, double * /*pZ*/) {  return ERR_NOT_IMPLEMENTED; }
	

////////////////// А работать будем с этим
/// Устанавливает список точек, заданных расстояниями до опорных
/**
	N - число точек
	ND[i] - число заданных расстояний для i-той точки
	SP - массив номеров опорных точек, расстояних до которых и задают наши точки. Сначала все для первой, потом все для второй итд
	R - собственно расстояния, до соотв. точек
*/
virtual long RP_AddList(long N, long *ND, long *SP,double *R)=0;


///	Получает информацию об N-той такой точке
/** 
	ND - число заданных расстояний
	SP - номера оп. точек, до которых эти расстояния
	R - сами расстояния
	*/
virtual long RP_Get(long N, long *ND, long **SP,double **R)=0;
	
/// Получает число точек
virtual long RP_GetNumOfPoints(long *pN)=0;
	
///Получает абс. координаты точки, заданной расстояниями
/**
	N - номер точки
*/
virtual long RP_GetAbsCoords(long N,double *pX,double *pY,double *pZ)=0;

///Получает координаты точки на карте, заданной расстояниями
/**
	N - номер точки	
	*/
virtual long RP_GetMapCoords(long N,long *pX,long *pY)=0;
	

//////////////////////////////////
// По "дополнению 3"
//////////////////////////////////

///Добавить темную зону для мастера Master.
/**
	Зона задается выпуклым многоугольником с числом вершин NumPts
	pPoints - массив вершин многоугольника
	В pN получаем номер зоны
	*/
virtual long IZ_Add(long Master,long NumPts, POINT *pPoints, long *pN)=0;

///Задать список темных зон.
/**
	N - число зон, M - мастера, NP - число вершин в многоугольнике
	pPoints - массив многоугольников.
	Уничтожает ранее заданные зоны; номера после IZ_AddList идут подряд
	*/
virtual long IZ_AddList(long N, long *M, long *NP, POINT *pPoints)=0;
	
/// Получить зону с номером N
virtual long IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints)=0;

///Получить номера всех темных зон для мастера Master	
virtual long IZ_GetMaster(long Master, long *pN,long **ppNums)=0;

///Удалить зону с номером N	
virtual long IZ_Remove(long N)=0;

///Удалить все темные зоны для мастера Master
virtual long IZ_RemoveMaster(long Master)=0;

///Удалить все темных зоны	
virtual long IZ_Clear()=0;

///Заранее выделить буфер на N темных зон	
virtual long IZ_PreAllocate(long N)=0;

	///Получить число заданных темных зон
	/**
		Непосредственно после загрузки файла - число зон равно 
		максимальному номеру, т.е. номера идут подряд
		*/
virtual long IZ_GetNumOfZones(long *pN)=0;
	

////////////////////////////////////////////////////////////
// Еще дополнительные функции - точно понадобятся, когда
//	придется честно считать неплоский рельеф
// ни одна из ниже 4-х приведенных функций не реализована. 
// Пожалуйста не используйте их.
////////////////////////////////////////////////////////////
	
//Устанавливает, где на изображении с мастера Master находится опорная точка с номером SP
virtual	long SP_SetVisible(long SP,long Master,POINT P)=0;

//Получает координаты опорной точки с номером SP на мастере Master
virtual	long SP_GetVisible(long SP,long Master,POINT *P)=0; 

//Устанавливает, в какой опорной точке находится мастер Master
virtual	long SetMasterSP(long Master,long SP)=0;
	
//Получает номер опорной точки, в которой находится мастер Master (-1, если не задана)
virtual	long GetMasterSP(long Master,long *SP)=0;
	

/////////////////////////////////////////////////////////////
// Функции для получения карты видимости
/////////////////////////////////////////////////////////////
///Устанавливает массив цветов, которыми заполнять карту
/**
	цвета - четверки RGBA, числом NumOfMasters+1
	Последняя четверка - для точек, невидимых ниоткуда.
	*/
virtual long SetColors(long *pColors)=0;
	
/// Получает этот самый массив цветов
virtual long GetColors(long **ppColors)=0;
	
/// Отрисовать карту видимости. 
/**
	Если задан аргумент,
	то в качестве массива цветов используется именно он,
	а не заданный функцией SetColors
	*/
virtual long ProcessMapBitmap(long *pColors=NULL)=0;
	
///Получить изображение карты видимости
/**
	как обычно, получаем указатель на внутренний буфер
*/
virtual long GetProcessedBitmap(PBITMAPINFO *pbi,PBYTE *pbits)=0;
	

/////////// Доп-функциии /////////////////////////////////////

///Получить номера sp-точек, с помощью которых построена сетка координат карты, и их координаты на карте	
/**
	Возвращает указатели на внутренний кэш, результат валиден до следующего вызова AlignMap или Load
	но рекумендуется копировать массивы после вызова ф-и
	*/
virtual long GetAlignMap( long* pNumber, const long** pIndex, const long** pPoint ) = 0;
	

	/// UNICODE-версия  GetErrorString
virtual const wchar_t * GetErrorStringW(long ErrorID)=0;
	

	/// отбражает горизонт на мастере master_index - порядковый номер мастера
	/** горизонтов может быть несколько (теоретически, практически всегда 1)
		points - возврашает указатель на массив точек размера ( points_count*sizeof(POINT) )
		points_count - вернет количество точек.. 
	
		если (points_count == 0 ) возвращается ERR_HORIZONT_NOT_FOUND
		Горизонт строится след образом: 
		прямыми попарно соединяются точки из массива POINT *points 
		0-ая с 1-ой, 2-ая с 3-ей.. и тп.. 
		points_count - всегда четное
		*/
virtual	long GetHoriz(int master_index, /*out*/ const POINT **points, /*out*/ int * points_count) = 0;
	
	
	/// Запоминает буфер для последующего сохранения в общем буфере.
	/**
		* данная функция копирует буффер для последующей запаковки в архив функцией Save
		* вызов этой функции должен быть до вызова Save
		* вызов Save сбрасывает все данные внутри. т.е. необходимо обновлять данные перед каждым Save
		(просто других способов сбосить к-л буффер не предусмотрено :) )
		* функцию можно вызывать монгократно для разных данных
		* если имена совпадают, старые данные будут перезаписаны новыми. ошибки не будет
		* результат действия Save - zip-архив. запиханные данные должны появиться в файле с именем name
		* не использовать имя "Main.vme" в корневом каталоге, 
		а также папки data/, vismap/, images/, extent_tables/.


		возвращаемые значения:
		  ERR_BAD_ADRESS, (один из переданных указателей ==0)
		  ERR_INVALID_ARGUMENT (если strlen(name) == 0), 
		  ERR_OK ( данные скопированы)
	*/

virtual long PutBuffer (const char * name, const void * buff_ptr, size_t buff_size) = 0;
	

	/// позволяет получить буфер по имени файла
	/**
		* Load распаковывает архив, получает все файлы и помещает их в во внутреннюю структуру
		* вызов этой функции должен быть после вызова Load
		* вызов Load сбрасывает предыдущие данные.
		* файл "Main.vme" содержит данные VVMInterface'а

		возвращаемые значения:
			ERR_BAD_ADRESS, (один из переданных указателей ==0)
			ERR_INVALID_ARGUMENT (если strlen(name) == 0),
			ERR_FILE_NOT_FOUND_IN_ZIP (файл с указанным именем не найден)
			ERR_OK ( данные скопированы)
	*/
virtual long GetBuffer (const char * name, const void ** pbuff_ptr, size_t * pbuff_size) = 0;
	

///		устанавливает значения параметров
/**		сейчас поддерживаются следующие параметры:

		____________________________________________________________________________					
					  id					значение по умолчанию	назначение
			
				GP_SAVE_MAP_IMAGE (*)				1			сохранять изображение карты

				GP_SAVE_MASTERS_IMAGES (*)			1			сохранять изображения мастеров

				GP_ENABLE_VIS_MAP (**)				1			рассчитывать/отображать/сохранять 
																карту видимости

				GP_ENABLE_EXTENTS (**)				1			рассчитывать/сохранять/загружать таблицу Extent'ов
		____________________________________________________________________________
		
		 Возвращаемые значения: текущее значение указанного параметра, если параметр с укзнным id
		 существует, или GP_NO_PARAM иначе...

		* note необходимо вызывать перед Save
		** note чтобы вступило в силу требуется перекомпиляция.
				
	*/
virtual long SetParameter(long param_id, long param) = 0;

/// Получает значения параметров
/**
	см. SetParameter
*/
virtual long GetParameter(long param_id) = 0;
	

/// позволяет устанавливать размер карты не устанавливая изображения.
/**	
	note: вызов этой функции не отключает карту видимости
*/
virtual long SetMapSize (unsigned long width, unsigned long height) = 0;
	

/// Получить буфер с изображением карты
/**
		Тоже самое что и GetBuffer, но предназначено для сохранения изображения карты
		Как и раньше получаемы указатели ссылаются на внутренний буфер
	*/
virtual long GetMapBuffer	( const void ** pbuff_ptr,	size_t *	pbuff_size ) = 0;
	
/// Записать в буфер изображение карты
/**
		Тоже самое что и PutBuffer, но предназначено для сохранения изображения карты
	*/
virtual long PutMapBuffer	( const void * buff_ptr,	size_t	buff_size  ) = 0;
	

/// Для составления зон алармов
/**
	зона аларма задается на мастере indexfrom набором прямоугольников. 
	результат - перевод этих пямоугольников на карту (indexto должен быть равен "-1") - набор многоугольников
	многоугольник - это последовательность точек(std::vector<POINT>), соединенных циклически. 
*/
virtual long TranslatePolygons
				(
					long indexfrom, 
					long indexto, 
					const std::vector<RECT>& rects,
					std::list < std::vector<POINT> >& results
				)=0;

/// Построить таблицу размеров
virtual long CompileExtents() = 0;

/// Вписать прямоугольник в многоугольник видимости для мастера master_index на карту.
/// Необходимо для того, чтобы грамотно разместить надпись, описывающую полигон
/**

	int master_index - индекс мастера
	long desired_width, 
	long desired_height - начальная высота и ширина многоугольника (их отношение будет сохранено)
	RECT& result - ссылка на результат
*/
virtual long InscribeRect(int master_index, long desired_width, long desired_height, RECT& result ) = 0;

};

/* 
 // пересены в VVMErrors.h
	#define VVMINT_EDITOR		1
	#define VVMINT_COMMUTATOR	2
	#define VVMINT_CLIENT		3
	#define VVMINT_MASTER		4
*/

/// получить реализацию интерфейса для редактора 
/**
	(options=VVMINT_EDITOR), коммутатора (options=VVMINT_COMMUTATOR)
	 или клиента (options=VVMINT_CLIENT)
 */
VVMDLL_API long __cdecl GetVVMInterface(CVVMapTrans **vvmi, DWORD options=VVMINT_EDITOR);




#endif //#ifndef VVMINTERFACE_H_INCLUDED