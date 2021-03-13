#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void SteganografiBul(int n, int resimadres_org, int resimadres_ste, int steganografi_adres);

int main(void) {
	int M, N, Q, i, j;
	bool type;
	int efile;
	char org_resim[100], ste_resim[100], steganografi[100];
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	do {
		printf("Steganografik resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &ste_resim);
		system("CLS");
		efile = readImageHeader(ste_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_ste = resimOku(ste_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);
	printf("SteganografiK Resim Yolu: \t\t%s\n", ste_resim);

	short *resimdizi_org, *resimdizi_ste;
	resimdizi_org = (short*) malloc(N*M * sizeof(short));
	resimdizi_ste = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++) 
		for (j = 0; j < M; j++) {
			resimdizi_org[i*N + j] = (short) resim_org[i][j];
			resimdizi_ste[i*N + j] = (short) resim_ste[i][j];
		}

	int resimadres_org = (int) resimdizi_org;
	int resimadres_ste = (int) resimdizi_ste;
	int steganografi_adres = (int) steganografi;

	SteganografiBul(N*M, resimadres_org, resimadres_ste, steganografi_adres);

	printf("\nResim icerisinde gizlenmis kod: \t%s\n", steganografi);
	system("PAUSE");
	return 0;
}

void SteganografiBul(int n, int resim_org, int resim_ste, int steganografi_adres) {
	
	__asm {
		XOR EAX,EAX //EAX SIFIRLANIR
		XOR DX,DX //DX SIFIRLANIR
		XOR EBX,EBX //EBX SIFIRLANIR
		MOV ECX, n //DIZI BOYUTUNUN(WIDHTXHEIGHT) DE�ER� ECX'E TA�INMASI,AYNI ZAMANDA L1 LOOPU BU SAYI KADAR D�NER
		XOR ESI,ESI //ESI SIFIRLANIR

		L1:
			PUSH ECX //CX YAZMACI KULLANILMAK ISTENDIGINDEN DOLAYI DONGUYU BOZMAMASI ICIN STACK'E ATILIR
			MOV EBX, resim_org // EBX REGISTERINA GELEN PAREMETREDEKI DIZININ BASLANGIC ADRESI ATILIR
			MOV CX, WORD PTR[EBX + EAX] //CX REGISTERINA resim_org ADRESINDE EAX INDISINDEKI DEGER ATILIR
			MOV EBX,resim_ste//EBX REGISTERINA resim_ste DIZISININ ADRESI ATILIR
			MOV DX, WORD PTR [EBX+EAX] // BU ADRESDEKI EAX REGISTERINDAKI DEGERIN INDISI ATILIR

			CMP DX, CX //�UAN ELDE OLAN IKINCI DIZIDEKI PIXELIN DEGERININ BIRINCI DIZIDEKI PIXELIN DEGERIYLE KARSILASTIRILMASI ��LEM�
			JZ DEVAM //E�ER FARK YOK ISE DEVAM LABELINA G�D�LS�N(ZF=1 OLURSA)
			JS NEGATIF //CXIN DXDEN BUYUK OLMA DURUMU (COMPARE �IKARMA ��LEM�NDE NEGAT�F SONUC ELDE ED�L�RSE SIGN FLAG=1 OLUR BU DEMEK OLUYORKI CX,DXDEN BUYUK
			
			MOV EBX,steganografi_adres //(BU DURUMUN ��LENMESI=DX CXDEN BUYUK VE DE�ERLER� FARKLI)EBX REGISTERINA CHAR TIPINDEKI DIZININ BASLANGIC ADRESI ATILIR
			SUB DX, CX //DX-CX FARKI ALINIR DX'E ATILIR
			MOV WORD PTR[EBX+ESI], DX //BULUNAN FARK ��FRED�R VE BU ��FRE GELEN PAREMETREDEK� CHAR D�Z�S�N�N ESI.CI INDISINE ATILIR
			ADD ESI,1 //CHAR DIZISI OLMASINDAN SEBEB B�R SONRAK� �TERASYON ���N ESI 1 ARTTIRILIR
			JMP DEVAM //CXIN DXDEN BUYUK OLMA DURUMU ATLANILSIN VE ITERASYONA DEVAM EDILSIN (DEVAM LABELINA GIDER)

			//CXIN DXDEN BUYUK OLMA DURUMU
	NEGATIF:MOV EBX, steganografi_adres//EBX YAZMACINA CHAR TIPINDEKI SIFREYI TUTAN DIZININ BASLANGIC ADRESI ATILIR
			SUB DX, CX //0-255DEN SONRA TA�MA OLUCAGI DXE CX EKLENIR
			ADD DX,256 //VE BU DEGER 255DEN TA�TIRILIR
			MOV WORD PTR[EBX+ESI], DX //B�R �ST SATIRDAK� DXE ��LENEN(DX+256) DE�ER ARADI�IMIZ ��FREN�N KARAKTER�D�R.BU DE�ERDE CHAR TIPINDEKI DIZININ ESI.CI INDISINE ATILIR
			ADD ESI,1 //ITERASYON ICIN ESI 1 ARTTIRILIR(CHAR TIPINDE OLDUGUNDAN DOLAYI)
	  DEVAM:
			//E��T OLMA DURUMU
			ADD EAX,2 //EAXIE ITERASYON I�IN 2 EKLER
			POP ECX //YI�INA ATILAN ECX TEKRAR �EK�L�R 
			LOOP L1 //ECX=0 OLANA KADAR DONGU DEVAM EDER


			//OGRENCI NUMARASI KARAKTER KARAKTER SIRAYLA BELIRTILDIGI �EK�LDE D�Z�YE EKLENIR
			MOV EBX, steganografi_adres //��FREYI TUTAN CHAR T�P�NDEK� D�Z�N�N BA�LANGI� ADRES�N� EBX REGISTERINA ATAR
			MOV WORD PTR[EBX+ESI],' ' //TIRNAK I�INDEK� KARAKTERLER� SIRASIYLA(ESI+1,ESI+2,.....,ESI+10)��FREYI TUTAN DIZIYE ATAR(98-108.SATIRLAR)
			MOV WORD PTR[EBX + ESI+1], '-'
			MOV WORD PTR[EBX + ESI+2], ' '
			MOV WORD PTR[EBX + ESI+3], '1'
			MOV WORD PTR[EBX + ESI+4], '7'
			MOV WORD PTR[EBX + ESI+5], '0'
			MOV WORD PTR[EBX + ESI+6], '1'
			MOV WORD PTR[EBX + ESI+7], '1'
			MOV WORD PTR[EBX + ESI+8], '7'
			MOV WORD PTR[EBX + ESI+9], '0'
			MOV WORD PTR[EBX + ESI+10], '1'
	}
	
}
