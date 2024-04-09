// функция Stabilize - пример стабилизации видеопотока. Ее нужно вызывать для каждого нового кадра, чтобы она
// преобразовывала картинку lpbits


PBYTE sstabBits=NULL;				// буфер картинки-шаблона
PBYTE sstabBuffer=NULL;				// буфер для временных значений
long sflags=0;
long sdxPrev=0,sdyPrev=0;			// предыдущее смещение
DWORD sstabBufSize=-1;				// размер буфера для промежуточного хранения
CFilters flt;						// набор фильтров. Здесь используется только функция CFilters::GetDisplacement,
									// которая полностью эквивалентна CFourierTransform::GetDisplacement, а также
									// функция CFilters::GetDisplacedBits, которая просто смещает картинку (при этом
									// появляется черная каемка) 
long smaxDisp=20;					// максимально допустимое смещение
long dx,dy;							// текущее смещение
CAlgoComplex *sfourierBuffer=NULL;	// буфер комплексных чисел
long ssx=50,ssy=50,sN=64;			// параметры окна
long sdeStabCount=0;				// счетчик сбоев стабилизации. Если превышает порог, то обновляется исходный кадр.
									// Сейчас достаточно одного сбоя.

void Stabilize(PBYTE lpbits, PBITMAPINFO pbi)
	DWORD &size=pbi->bmiHeader.biSizeImage;
	if (sstabBits==NULL) {
		// функция вызвана первый раз
		sstabBits=AlgoNewA(BYTE,size,AlgoMemVisor,"For Filters"); //AlgoNewA(<type>,size,<mv>,<desc>) - эквивалент
																	//new <type>[size]
		CopyMemory(sstabBits,lpbits,size);
		sflags=0; sdxPrev=sdyPrev=0;
		flt.GetDisplacedBits(sstabBits,lpbits,&(pbi->bmiHeader),0,0,smaxDisp,smaxDisp);
	} else {
		if (size!=sstabBufSize){

			// изменился размер картинки, требуется переинициализация
			if (sstabBuffer) AlgoDeleteA(sstabBuffer,AlgoMemVisor);
			sstabBuffer=AlgoNewA(BYTE,sstabBufSize=size,AlgoMemVisor,"For Filters");
		}
		if (sfourierBuffer==NULL) sfourierBuffer=AlgoNewA(CAlgoComplex,sN*sN*4,AlgoMemVisor,"For Filters");
		flt.GetDisplacement(sstabBits,lpbits,&(pbi->bmiHeader),1,ssx,ssy,sN,sdxPrev,sdyPrev,&dx,&dy,sfourierBuffer,sflags);
		sflags=GD_USEOLDTRANS; // после первого вызова можно использовать преобразование в sfourierBuffer
		long flagRightDisp=((abs(dx)<sN/3) && (abs(dy)<sN/3)); // проверка на корректность результата
		if (flagRightDisp) {
			sdxPrev+=dx; sdyPrev+=dy;
		} else {
			sdeStabCount++;
		}
		dx=sdxPrev;dy=sdyPrev;
		if(1==flt.GetDisplacedBits(lpbits,sstabBuffer,&(pbi->bmiHeader),-dx,-dy,smaxDisp,smaxDisp)){
			sdeStabCount++; // GetDisplacedBits возвращает 1, если dx или dy превышают по модулю smaxDisp
		} else {
			if (flagRightDisp) sdeStabCount=0;
		}
		if (sdeStabCount>0) { // смещения слишком велики, необходим новый шаблонный кадр
			sdeStabCount=0; 
			CopyMemory(sstabBits,lpbits,size);	
			sflags=0; sdxPrev=sdyPrev=0;
		}
		CopyMemory(lpbits,sstabBuffer,size);
	}
}
