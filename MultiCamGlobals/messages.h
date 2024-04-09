//! \file messages.h
//! \brief сообщения между компонентами системы (диспетчер, мастер, слейв)
//!
//! Создан в 2003 году, А. Малистов, А. Хамухин
//////////////////////////////////////////////////////////////////////////

#pragma once
 
//!структура для передачи сообщений в кластере
struct CAlgoMessage
{
	//! конструктор с перечислением параметров сообщения
	CAlgoMessage(long msg, long wp=0, long lp=0,long sID=-1): senderID(sID),message(msg),wParam(wp),lParam(lp){;};
	//! установить параметры сообщения
	void SetContent(long msg, long wp=0, long lp=0,long sID=-1) {senderID=sID;message=msg;wParam=wp;lParam=lp;};
	long message;//!< содержание сообщения (список приведен ниже)
	long wParam;//!<дополнительный параметр сообщения (по аналогии с оконными сообщениями)
	long lParam;//!<дополнительный параметр сообщения (по аналогии с оконными сообщениями)
	long senderID;//!< обычно - системный номер камеры в кластере, отославшей сообщение
	__int64 StartTime; //!<Параметр для оценки времени передачи сообщения (работает только на одном компьютере)
	BYTE data[1];//!<данные сообщения (могут быть произвольной длины)
};

//! Сообщение для завершения MainLoop класса CCommutator, а также для досрочного выхода из CSlaveManager::GetNextPhoto()
#define CMS_QUIT				0

//!\brief BroadCast сообщение для того, чтобы найти класс CCommutator в кластере.
//! wParam - ширина (HIWORD) и высота (LOWORD) мастера, lParam - тип камеры (1-мастер, 2-слейв).
#define CMS_NEWCAMERA			1
//! значение параметра lParam для сообщения CMS_NEWCAMERA при обозначении мастера
#define DVC_MASTER				1
//! значение параметра lParam для сообщения CMS_NEWCAMERA при обозначении слейва
#define DVC_SLAVE				2


//!\brief Сообщение от коммутатора только что подключенной камере с идентификатором CCommutator в wParam
//! wParam - dwID коммутатора, 
//! LOWORD(lParam) x HIWORD(wParam) - разрешение камеры.
#define CMS_COMMUTATORID		2

//!\brief Сообщение классу CCommunicator от класса CAnalyzer со списком объектов-претендентов на съемку.
//! wParam - количество объектов, lParam - за кем следят, data - long fps*16 + массив записей CObjectRecord
#define CMS_MASTEROBJECTTRANS	3
//! структура передачи данных об объекте
struct CObjectRecord {
	long realID;//!< внешний номер объекта на мастере

	//!@name границы рамки объекта
	//!@{
	WORD xmin,ymin,xmax,ymax;
	//!@}

	//!@name скорость объекта, измеряемая в пикселах за кадр
	//!@{
	float Vx,Vy;
	//!@}

	signed char flags; //!< битовое поле, бит 5 - за объектом установлено слежение, бит 4 - объект оставлен, биты 0-3 - гладкость траектории Smoothness
	signed char classID;//!< класс объекта по системе человек-машина-группа людей.
};

//!\brief Сообщение классу CSlaveManager от CCommutator с указанием переместиться в пресет
//! wParam - номер пресета
//! lParam - dwID мастера, которому принадлежит объект
//! data = GotoPresetMessageData;
#define CMS_GOTOPRESET			4

//! данные о пресете
struct GotoPresetMessageData {	
	long masterID;//!< номер мастера
	long realID;//!< внешний номер фотографируемого объекта
	long delay;//!< время упреждения
//	long dbID;//!< номер объекта в базе данных
	unsigned long YX;//!< координаты пресета в виде (y<<16)+x
	double zoom;//!< увеличение пресета
	long followFlag;//!< флаг установки слежения за объектом
};

//!сообщение от CSlaveManager к CCommunicator в подтверждение о выполнении установки пресета.
#define CMS_PRESETCALLMADE		5

//!\brief Сообщение классу CSlaveManager от CCommunicator c рамочкой
//!wParam = есть ли рамочка на экране
//!data = структура RECT
#define CMS_SENDRECT			6

//!\brief Сообщение от CSlaveManager к CCommunicator в подтверждение о получении рамочки.
//!wParam - статус сделанной фотографии 
#define CMS_RECTRECEIVED		7

//!\brief Сообщение классу CAnalyzer от класса CSlaveManager со запросом на получение маски
//! wParam=RealID объекта
//! lParam=dwID SlaveManager, которому высылать ответ
#define CMS_MASKQUERY			8

//!\brief Сообщение классу CSlavemanager от класса CAnalyzer c маской и рамкой
//! wParam=0, если объект не найден, 1, если найден, lParam - dbID объекта (если он есть)
#define CMS_SENDMASK			9

//!\brief Сообщение классу CCommutator от класса CAnalyzer c заданием перевести слэйв в указанную точку
//! wParam=(x<<16)|y, lParam=SlaveId (lParam=-1 если любой слэйв)
#define CMS_GOTOXY				10

//!\brief Сообщение об отключении/включении слейв-камеры
//! wParam - ID камеры, lParam -  состояние (0-отключить, 1- включить).
#define CMS_SWITCHSLAVE			11

//!\brief Широковещательное сообщение с запросом dwID 
//! wParam - ID dataWare, на который будет посылаться ответ
//! lParam - ID устройства, которое ищут (если lParam==-1, то ответят все устройства)
#define CMS_WHEREAREYOU			12

//!\brief Ответ на CMS_WHEREAREYOU и сообщение слейва о своем существовании. 
#define CMS_HEREIAM				13

//!\brief Сообщение для обмена объектами на пересекающихся камерах
//! wParam - количество переданных объектов в виде CObjectRecord
//! lParam - количество структур данных о совпадениях
//! data - буфер со структурами CObjectRecord и данных о совпадении
//! (сначала wParam структур CObjectRecord, затем lParam структур о совпадении)
#define CMS_COMPARABLEOBJ		14

//!\brief Cообщение о запуске нового коммутатора
//! wParam - количество камер, которые поддерживает коммутатор
//! data - массив long'ов с идентификаторами камер
#define	CMS_NEWCOMMUTATOR		15

//!Сообщение классу CSlaveManager от CCommunicator об отключении слейва
#define CMS_SLAVEOFF			16

//!\brief сообщение об отключении объекта от сопровождения слейвами. посылается сначала от 
//!мастера коммутатору, затем от коммутатора мастерам
#define CMS_FOLLOWOFF			17
