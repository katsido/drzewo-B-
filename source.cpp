/////////////////////////////////////
// B* drzewo
//autor: katsidor
//
////////////////////////////////////////



#include <iostream>
#include <string>
#include<fstream>
#include<conio.h>
#include<stack>
#include<list>

using namespace std;

#define ZAKRES_A 2000
#define ZAKRES_Y 2000
#define ZAKRES_C 2000
#define ZAKRES_Z 2000
#define ZAKRES_X 2000

#define TEST 0

#define dwaD 4
#define D 2  //minimalna liczba rekordow w weżle

#define dwaR 8
#define R 4	//minimalna liczba rekorwów w liściu

#define ROZMIAR_REKORDU (8+5*8)     //8 to klucz, 5 liczb jako double(8bajtow) 

class Wezel_posredni;
class Lisc;
class Wezel;


int licznik=0;
int licznik_lisci = 0;
int licznik_wezlow_pom = 0;
int operacje_odczytu = 0;
int operacje_zapisu = 0;



Wezel *korzen = NULL;

char rekordyDrzewa[40] = "rekordy.bin";
char pliksamychrekordow[40] = "test.bin";
bool nowa = true;//potrzebne do zerowania licznika nr stron w funkcji
list <int> wolne_adresy2;

//szeregowanie wezla
unsigned long long int pom_tablica_kluczy_wezel[2*dwaD + 1] = { 0 };
Wezel *pom_wsk[2*dwaD + 2];


 //szeregowanie liscia
 unsigned long long int pom_tablica_kluczy_lisc[dwaR+dwaR+1] = { 0 };
 int adresy_pom[dwaR+dwaR+1];//do rozszczepienia


 //szeregowanie liscia
 unsigned long long int pom_tablica_kluczy_lisc_duze[dwaR+dwaR + 1] = { 0 };
 int adresy_pom_duze[dwaR+dwaR + 1];

 unsigned long long int klucz_do_kasowania = 0;

 


void zapisz_pod_adresem(int adres, unsigned long long int klucz, double a, double y, double c, double z, double x);



int kolejny_ades() {
	static int pierwsza = 0;
	if (nowa == true)
	{
		pierwsza = 0;
		nowa = false;
	}
	
	int numer;
	if (!wolne_adresy2.empty())
	{
		wolne_adresy2.sort();
		numer = wolne_adresy2.front();
		wolne_adresy2.pop_front();
		return numer;
	}
	else
	{
		int numer = pierwsza;
		pierwsza=pierwsza+ ROZMIAR_REKORDU;
		return numer;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class  Wezel {
public:
	int ile_zapelnionych = 0;

	unsigned long long int *tablica_kluczy;
	

	Wezel_posredni *rodzic;

	virtual void wyswietl(int poziom) {};
	virtual	void podziel(int klucznik) {};
	virtual void zastap_klucze(unsigned long long int stary, unsigned long long int nowy) {};
	virtual void dodaj_rekord_z_zewnatzr(unsigned long long int klucz,double a, double y, double c, double z, double x) {};
	virtual unsigned long long int szukaj(unsigned long long int klucz) { return 1; };
	virtual unsigned long long int usun(unsigned long long int klucz) { return 1; };
	virtual void kompersuj_z(Lisc *stary, int kierunek, unsigned long long int klucz_do_wywalenia) {};
	virtual void wpisz_do_liscia(unsigned long long int klucz, int adresik) {};

	virtual int ilewolnych() {};

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class  Wezel_posredni:public  Wezel {
	public:

	 Wezel* wsk[dwaD+1];
	 
	 void kompersuj_z(Lisc *stary, int kierunek, unsigned long long int klucz_do_wywalenia)
	 {
		 cout << "wywolane dla wezła" << endl;
	 };

	 Wezel_posredni() {
		tablica_kluczy = new unsigned long long int[dwaD];
		for (int i = 0; i < dwaD; i++)
		{
			tablica_kluczy[i] =0;
			wsk[i] = NULL;
		}
		wsk[dwaD] = NULL;
		ile_zapelnionych = 0;
	}

	 int ilewolnych() {
		 return dwaD - ile_zapelnionych;
	 }


	int ktory_element(unsigned long long int klucz) {
		for (int i = 0; i < dwaD; i++)
		{
			if (tablica_kluczy[i] == klucz)
				return i;
		}
	}


	void wyswietl(int poziom) {
		if (ile_zapelnionych == 0)return;

		for (int i = 0; i < ile_zapelnionych; i++) {
			if(wsk[i]!=NULL)
				wsk[i]->wyswietl(poziom + 1);
			for (int j = 0; j < poziom; j++)
			{
				cout << "\t";
			}
			cout << tablica_kluczy[i] << endl;
		}
		if (wsk[ile_zapelnionych ] != NULL)
			wsk[ile_zapelnionych]->wyswietl(poziom + 1);
	};
	
	void wpisz_do_wierzcholka(unsigned long long int kluczyk, Wezel *dostalem)
	{
		int i = ile_zapelnionych-1;
		for (; i>=0; i--)
		{
			if (kluczyk > tablica_kluczy[i ])//czy jest sens przepisywać w ogóle, jak nie ma to dopisz na nastepnej pozycji;
			{
				tablica_kluczy[i + 1] = kluczyk;
				wsk[i + 2] = dostalem;
				this->ile_zapelnionych++;
				return;
			}
			wsk[i + 2] = wsk[i+1];
			tablica_kluczy[i + 1] = tablica_kluczy[i];
		}
		tablica_kluczy[0] = kluczyk;
		wsk[1] = dostalem;
		this->ile_zapelnionych++;
	};

	
	void dodaj_paczke(unsigned long long int kluczyk,  Wezel *dostalem) {
		if( ile_zapelnionych==dwaD && rodzic == NULL)
		{
			podziel_korzen(kluczyk,dostalem);
		}
		else if (ile_zapelnionych == dwaD) {
			podziel_Wezel(kluczyk,dostalem);
		}
		else
		{
			wpisz_do_wierzcholka(kluczyk,dostalem);
		}
	}
	
	//mediana z klucza i tab_kluczy, wypelnia obiekt globalny pom_tab_kluczy i pom_wsk wieksze o 1 od normalnych
	void  Wezel_pomocnicze_tabele(unsigned long long int kluczyk,  Wezel *dostalem) {
		int ktory = 0;
		pom_wsk[0] = this->wsk[0];

		for (int ile_uzupelnionych = 0; ile_uzupelnionych < dwaD+1; ile_uzupelnionych++)
		{
			if (kluczyk < tablica_kluczy[ktory])
			{
				pom_tablica_kluczy_wezel[ile_uzupelnionych] = kluczyk;
				pom_wsk[ile_uzupelnionych + 1] = dostalem;
				kluczyk = ULLONG_MAX;
			}
			else
			{
				pom_tablica_kluczy_wezel[ile_uzupelnionych] = tablica_kluczy[ktory];
				pom_wsk[ile_uzupelnionych + 1] = this->wsk[ktory+1];
				ktory++;
			}
		}
	};

	

	void podziel_korzen(unsigned long long int kluczyk,  Wezel *dostalem) {
		 Wezel_posredni* nowy = new  Wezel_posredni();
		 Wezel_posredni* korzonek = new  Wezel_posredni();
		korzonek->rodzic = NULL;
		korzen = korzonek;
		nowy->rodzic = korzonek;
		this->rodzic = korzonek;
		korzonek->wsk[0] = this;
		korzonek->wsk[1] = nowy;

		 Wezel_pomocnicze_tabele(kluczyk,dostalem);
		korzonek->tablica_kluczy[0] = pom_tablica_kluczy_wezel[D];

		//przepisz odpowiednio tablice_pomocnicze
		nowy->wsk[0] = pom_wsk[D+1];
		nowy->wsk[0]->rodzic = nowy;//
		for (int i = 0; i < dwaD; i++)
		{
			if (i < D)
			{
				this->tablica_kluczy[i] = pom_tablica_kluczy_wezel[i];
				nowy->tablica_kluczy[i ] = pom_tablica_kluczy_wezel[i+D+1 ];
				this->wsk[i + 1] = pom_wsk[i + 1];
				nowy->wsk[i + 1] = pom_wsk[i + 2+D];
				nowy->wsk[i + 1]->rodzic = nowy;
			}
			else if(i >= D)
			{
				this->tablica_kluczy[i] = 0;
				this->wsk[i + 1] = NULL;
			}
		}
		this->ile_zapelnionych = D;
		nowy->ile_zapelnionych = D;
		korzonek->ile_zapelnionych = 1;
	}

	void zastap_klucze(unsigned long long int stary, unsigned long long int nowy)
	{
			int i;
			for (i = 0; i < ile_zapelnionych; i++)
			{
				if (stary < this->tablica_kluczy[i])
				{
					 wsk[i]->zastap_klucze(stary,nowy);
					 return;
				}
				if (stary == this->tablica_kluczy[i])
				{
					this->tablica_kluczy[i] = nowy;
					return ;
				}
			}
			return wsk[i]->zastap_klucze(stary, nowy);
	};



	bool kompersacja_od_liscia(unsigned long long int klucz, Lisc* wywolujacy )
	{
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz == this->tablica_kluczy[i])
			{
				if (wsk[i + 1]->ilewolnych())
				{
					wsk[i + 1]->kompersuj_z(wywolujacy,1,tablica_kluczy[i]);
					return true;
				}
				if(i!=0)
					if (wsk[i - 1]->ilewolnych())
					{
					wsk[i - 1]->kompersuj_z(wywolujacy,-1,tablica_kluczy[i-1]);
					return true;
					}
			}
		}
		if (wsk[ile_zapelnionych]->ilewolnych())
		{
			wsk[i + 1]->kompersuj_z(wywolujacy, -1,tablica_kluczy[ile_zapelnionych-1]);
			return true;
		}





		return false;
	};





	void podziel_Wezel(unsigned long long int kluczyk,  Wezel *dostalem) {
		 Wezel_posredni* nowy = new  Wezel_posredni();
		nowy->rodzic = this->rodzic;
		 Wezel_pomocnicze_tabele(kluczyk,dostalem);
		unsigned long long int klucznik = pom_tablica_kluczy_wezel[D];
		if (TEST)
		{
			cout << "rozpoczynamy rozszczepienie wierzcholka o kluczach:";
			for (int i = 0; i < dwaD; i++)
				cout << this->tablica_kluczy[i] << " ";
			cout << endl;
			cout << "paczka zawiera klucz do doczepienia=" << kluczyk << endl;

			cout << "paczka pomocnicza wygląda tak:" << endl;
			for (int i = 0; i < dwaD + 1; i++)
			{
				pom_wsk[i]->wyswietl(1);
				cout << pom_tablica_kluczy_wezel[i] << endl;
			}
			pom_wsk[dwaD + 1]->wyswietl(1);
			cout << "-----------------------" << endl;
			//delete dostalam;
			cout << "z wierzcholka wyjdzie wyzej klucz=" << kluczyk << endl;
		}

		//wysylam->klucz = kluczyk;
		
		nowy->wsk[0] = pom_wsk[D+1];
		nowy->wsk[0]->rodzic = nowy;
		for (int i = 0; i < dwaD; i++)
		{
			if (i < D)
			{
				this->tablica_kluczy[i] = pom_tablica_kluczy_wezel[i];
				nowy->tablica_kluczy[i] = pom_tablica_kluczy_wezel[i+ D+1];

				this->wsk[i + 1] = pom_wsk[i + 1];
				nowy->wsk[i +  1] = pom_wsk[i + D +2];
				nowy->wsk[i  + 1]->rodzic = nowy;
			}
			else if (i >= D)
			{
				this->tablica_kluczy[i] = 0;
				this->wsk[i + 1] = NULL;
			}
		}
		this->ile_zapelnionych = D;
		nowy->ile_zapelnionych = D;
		if (TEST)
		{
			cout << "wierz a:" << endl;
			wyswietl(4);
			cout << "wierz b:-----------" << endl;
			nowy->wyswietl(4);
		}

		rodzic->dodaj_paczke(klucznik,nowy);///na pewno?
	}


	void dodaj_rekord_z_zewnatzr(unsigned long long int klucz, double a, double y, double c, double z, double x) {
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz < this->tablica_kluczy[i])
			{
				wsk[i]->dodaj_rekord_z_zewnatzr(klucz, a, y, c, z, x);
				return;
			}
		}
		wsk[i]->dodaj_rekord_z_zewnatzr(klucz, a, y, c, z, x);
	};


	int nowe_miejce_na_klucz(unsigned long long int klucz) {
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz < this->tablica_kluczy[i])
			{
				return wsk[i-1]->ilewolnych();
			}
		}
		return _I64_MAX;
	};




	unsigned long long int szukaj(unsigned long long int klucz) {
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz <= this->tablica_kluczy[i])
			{
				return wsk[i]->szukaj(klucz);
			}
		}
		return wsk[i]->szukaj(klucz);;
	}
	
	unsigned long long int usun(unsigned long long int klucz) { 
		int i;
		unsigned long long int nowa_wartosc;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz <= this->tablica_kluczy[i])
			{
				nowa_wartosc=(wsk[i]->usun(klucz));
				if(nowa_wartosc==0)
					return nowa_wartosc;
				else
				{
					korzen->zastap_klucze(klucz, nowa_wartosc);
					return 0;
				}
			}
		}
		return wsk[i]->usun(klucz);;
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Lisc :public  Wezel {
public:
	int adres_w_pliku[dwaR];

	Lisc() {
		licznik++;
		ile_zapelnionych = 0;
		tablica_kluczy = new unsigned long long int[dwaR];
		for (int i = 0; i < dwaR; i++)
		{
			tablica_kluczy[i] = 0;
			adres_w_pliku[i] = 0;
		}
	}

	int ilewolnych() {
		return dwaR - ile_zapelnionych;
	}


	void wyswietl(int poziom)
	{
		if (ile_zapelnionych == 0)return;
		if (TEST)
		{
			for (int j = 0; j < poziom; j++)
			{
				cout << "\t";
			}
			cout << "******** mój rodzic ma klucze: ";
				for (int i = 0; i < dwaR; i++)
					cout << rodzic->tablica_kluczy[i] << " ";
				cout << endl;
		}
		for (int i = 0; i < ile_zapelnionych; i++) {
			for (int j = 0; j < poziom; j++)
			{
				cout << "\t";
			}
			cout << "" << tablica_kluczy[i] << "\t rekord zawiera sie pod adresem " << this->adres_w_pliku[i] << endl;
		}
	}

	//dwa liscie przepisuje w  pom_tablica_kluczy
	int lisc_pomocnicze_tabele(Lisc *podany) {
		int n = this->ile_zapelnionych + podany->ile_zapelnionych;
		int ktory_z_tego = 0;
		int ktory_z_podanego = 0;
		for (int ile_uzupelnionych = 0; ile_uzupelnionych < n; ile_uzupelnionych++)
		{
			if (ktory_z_tego >= this->ile_zapelnionych)
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = podany->tablica_kluczy[ktory_z_podanego];
				adresy_pom[ile_uzupelnionych] = podany->adres_w_pliku[ktory_z_podanego];
				ktory_z_podanego++;
			}
			else if (ktory_z_podanego >= podany->ile_zapelnionych)
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = tablica_kluczy[ktory_z_tego];
				adresy_pom[ile_uzupelnionych] = adres_w_pliku[ktory_z_tego];
				ktory_z_tego++;
			}
			else if (podany->tablica_kluczy[ktory_z_podanego] < tablica_kluczy[ktory_z_tego])
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = podany->tablica_kluczy[ktory_z_podanego];
				adresy_pom[ile_uzupelnionych] = podany->adres_w_pliku[ktory_z_podanego];
				ktory_z_podanego++;
			}
			else
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = tablica_kluczy[ktory_z_tego];
				adresy_pom[ile_uzupelnionych] =adres_w_pliku[ktory_z_tego];
				ktory_z_tego++;
			}
		}
		return n;
	};





	//pom_tab_kluczy gdy daodamy rekord ponad stan
	void  Wezel_pomocnicze_tabele_lisc(unsigned long long int kluczyk,int adres) {
		int ktory = 0;
		for (int ile_uzupelnionych = 0; ile_uzupelnionych < dwaR + 1; ile_uzupelnionych++)
		{
			if (kluczyk < tablica_kluczy[ktory])
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = kluczyk;
				adresy_pom[ile_uzupelnionych] = adres;
				kluczyk = ULLONG_MAX;
			}
			else
			{
				pom_tablica_kluczy_lisc[ile_uzupelnionych] = tablica_kluczy[ktory];
				adresy_pom[ile_uzupelnionych] = this->adres_w_pliku[ktory];
				ktory++;
			}
		}
	};

	void kompersuj_z(Lisc* stary,int kierunek, unsigned long long int klucz_do_wywalenia) {
		int n=lisc_pomocnicze_tabele(stary);
		//int ktory_tego = 0;
		//int ktory_z_tamtego = 0;
		//for(int wsk_zap =0;wsk_zap<ile_zapelnionych + dwaR + 1;wsk_zap++){
		//	if (ktory_z_tamtego >= dwaR + 1)
		//	{
		//		pom_tablica_kluczy_lisc_duze[wsk_zap] = this->tablica_kluczy[ktory_tego];
		//		adresy_pom_duze[wsk_zap] = this->adres_w_pliku[ktory_tego];
		//		ktory_tego++;
		//	}			
		//	else if (ktory_z_tamtego >= ile_zapelnionych)
		//	{
		//		pom_tablica_kluczy_lisc_duze[wsk_zap] = pom_tablica_kluczy_lisc[ktory_z_tamtego];
		//		adresy_pom_duze[wsk_zap] = adresy_pom[ktory_z_tamtego];
		//		ktory_z_tamtego++;
		//	}
		//	else if(this->tablica_kluczy[ktory_tego]>pom_tablica_kluczy_lisc[ktory_z_tamtego])
		//	{
		//		pom_tablica_kluczy_lisc_duze[wsk_zap] = pom_tablica_kluczy_lisc[ktory_z_tamtego];
		//		adresy_pom_duze[wsk_zap] = adresy_pom[ktory_z_tamtego];
		//		ktory_z_tamtego++;
		//	}
		//	else
		//	{
		//		pom_tablica_kluczy_lisc_duze[wsk_zap] = this->tablica_kluczy[ktory_tego];
		//		adresy_pom_duze[wsk_zap] = this->adres_w_pliku[ktory_tego];
		//		ktory_tego++;
		//	}
		//}//posortowana
		int srodkowy = (ile_zapelnionych + dwaR + 1)/2;
		Lisc *mniejszy;
		Lisc *wiekszy;

		if (kierunek == 1)
		{
			wiekszy = this;
			mniejszy = stary;
		}
		else
		{
			wiekszy = stary;
			mniejszy = this;
		}
		for (int i = 0;i<=srodkowy;i++)
		{
			mniejszy->tablica_kluczy[i] = pom_tablica_kluczy_lisc_duze[i];
			mniejszy->adres_w_pliku[i] = adresy_pom_duze[i];
		}
		int a = 0;
		for (int i = srodkowy+1; i < ile_zapelnionych + dwaR + 1; i++)
		{
			wiekszy->tablica_kluczy[a] = pom_tablica_kluczy_lisc_duze[i];
			wiekszy->adres_w_pliku[a] = adresy_pom_duze[i];
			a++;
		}
		wiekszy->ile_zapelnionych = a;
		mniejszy->ile_zapelnionych = srodkowy;

		zastap_klucze(klucz_do_wywalenia, pom_tablica_kluczy_lisc_duze[srodkowy]);
	
	
	
	};





	void wpisz_do_liscia(unsigned long long int klucz, int adresik)
	{
		if (ile_zapelnionych == 0)
		{
			tablica_kluczy[0] = klucz;
			adres_w_pliku[0] = adresik;
			ile_zapelnionych++;
			return;
		}
		int i = ile_zapelnionych - 1;
		for (; i >= 0; i--)
		{
			if (klucz > tablica_kluczy[i ])//czy jest sens przepisywać w ogóle, jak nie ma to dopisz na nastepnej pozycji;
			{
				tablica_kluczy[i + 1] = klucz;
				adres_w_pliku[i+1] = adresik;
				this->ile_zapelnionych++;
				return;
			}
			tablica_kluczy[i + 1] = tablica_kluczy[i];
			adres_w_pliku[i + 1] = adres_w_pliku[i];
		}

		tablica_kluczy[0] = klucz;
		adres_w_pliku[0] = adresik;
		this->ile_zapelnionych++;
	};



	//oDaje wskaznik na nowy lisc oraz poprzedzajacy go klucz;
	void podziel_lisc(unsigned long long int klucz_, int adresik) {
		Lisc *listek = new Lisc();
		for (int i = R; i < dwaR; i++)//dla i=2,i=3
		{
			listek->tablica_kluczy[i - R] = tablica_kluczy[i];
			listek->adres_w_pliku[i - R] = adres_w_pliku[i];
			tablica_kluczy[i] = 0;
		}
		listek->ile_zapelnionych = R;
		this->ile_zapelnionych = R;
		unsigned long long int klucz__ = tablica_kluczy[R - 1];
		if (korzen == this)
		{
			Wezel_posredni *korzonek = new Wezel_posredni();
			korzen = korzonek;
			korzonek->rodzic = NULL;
			this->rodzic = korzonek;
			listek->rodzic = korzonek;
			korzonek->tablica_kluczy[0] = klucz__;
			korzonek->wsk[0] = this;
			korzonek->wsk[1] = listek;
			korzonek->ile_zapelnionych++;
		}
		else
		{
			listek->rodzic = this->rodzic;
			rodzic->dodaj_paczke(klucz__,listek);
		}
		if (klucz_ <= tablica_kluczy[R - 1])//do ktorego liscia wpisujemy nowy rekord
		{
			this->wpisz_do_liscia(klucz_, adresik);
		}
		else
		{
			listek->wpisz_do_liscia(klucz_, adresik);
		}
	};








	void dodaj_rekord_z_zewnatzr(unsigned long long int klucz_, double a, double y, double c, double z, double x) {
		int adresik = kolejny_ades();
		zapisz_pod_adresem(adresik, klucz_, a, y, c, z, x);

		if(ile_zapelnionych == dwaR)
		{
			Wezel_pomocnicze_tabele_lisc(klucz_, adresik);

			bool a = false;//this->rodzic->kompersacja_od_liscia(pom_tablica_kluczy_lisc[dwaR], this);
			if (a == true)
				return;
			else
				this->podziel_lisc(klucz_, adresik);
		}
		else
		{
			this->wpisz_do_liscia(klucz_, adresik);
		}
	};


	unsigned long long int szukaj(unsigned long long int klucz) {
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz == this->tablica_kluczy[i])
			{
				return adres_w_pliku[i];
			}
		}
		return _I64_MAX;
	}

	void zastap_klucze(unsigned long long int stary, unsigned long long int nowy)
	{
		return;
	}



	unsigned long long int usun(unsigned long long int klucz) {
		int i;
		for (i = 0; i < ile_zapelnionych; i++)
		{
			if (klucz == this->tablica_kluczy[i])
			{
				zapisz_pod_adresem(adres_w_pliku[i], 0, 0, 0, 0, 0, 0);
				//wolne_adresy.push(adres_w_pliku[i]);
				wolne_adresy2.push_front(adres_w_pliku[i]);
				tablica_kluczy[i] = 0;
				adres_w_pliku[i] = 0;
				if (ile_zapelnionych == 1)
				{
					ile_zapelnionych--;
					return klucz;//lisc do usuniecia
				}
				else if (i == ile_zapelnionych - 1 )//usuwamy ostatni element,przekazujemy liczbe wyzej
				{
					ile_zapelnionych--;
					return tablica_kluczy[i - 1];//nowa liczba do zmizny
				}
				else//usuwamy cos ze srodka
				{
					int j = i;
					for (; j < ile_zapelnionych-1; j++)
					{
						tablica_kluczy[j] = tablica_kluczy[j+1];
						adres_w_pliku[j] = adres_w_pliku[j+1];
					}
					tablica_kluczy[j] = 0;
					adres_w_pliku[j] = 0;
					ile_zapelnionych--;
					return 0;//na pewno tej liczby nie ma
				}
			}
		}
	};


};


void wyczysc_stos_i_plik() {
	if (wolne_adresy2.size() != 0)
	{
		wolne_adresy2.clear();
	}
	remove(rekordyDrzewa);
	ofstream plik;
	plik.open(rekordyDrzewa, ios::out);
	plik.close();

};
void wyswietl_drzewo(  Wezel* korzonek) {
	cout << "--------Początek wyswietlania--------------------------------------------------------" << endl;
	korzonek->wyswietl(0);
	cout << "--------Koniec   wyswietlania--------------------------------------------------------" << endl;
}
void utworzPlikRekordow(int liczba) {
	ofstream binarka;
	static int pierwsza = 1;
	binarka.open(pliksamychrekordow, ios::binary | ios::out);
	double bufor[5];
	double a, y, c, z, x;
	unsigned long long int klucz;
	for (int i = 0; i < liczba; i++)
	{
		a = rand()*ZAKRES_A / (double)RAND_MAX;
		y = (rand()*ZAKRES_Y) / (double)RAND_MAX;
		c = (rand()*ZAKRES_C) / (double)RAND_MAX;
		z = (rand()*ZAKRES_Z) / (double)RAND_MAX;
		x = (rand()*ZAKRES_X) / (double)RAND_MAX;
		klucz = rand()*RAND_MAX +rand();
		binarka.write((char*)&klucz, sizeof(klucz));
		bufor[0] = a;
		bufor[1] = y;
		bufor[2] = c;
		bufor[3] = z;
		bufor[4] = x;
		binarka.write((char*)&bufor, sizeof(bufor));
		pierwsza=pierwsza+6;
	}
	binarka.close();
	cout << "wygenerowano "<<pliksamychrekordow <<" zawierajacy"<< liczba<<" rekordow"<<endl;
}
int ile_rekordow_bin(string nazwa) {
	ifstream ccc(nazwa, ios::binary | ios::ate);
	int wielkosc = (ccc.tellg()) / ROZMIAR_REKORDU;//rekord to 5 doubli każdy po 8 bajtów + klucz na 8 bajtach
	ccc.close();
	return wielkosc;
}
void wyswietl_plik_bin_adresy(char *nazwa, bool wyswietlanie) {
	int ilosc_rekordow_pozostala_do_wyswietlenia = ile_rekordow_bin(nazwa);
	int licznik = 0;
	double wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x;
	unsigned long long int klucz;
	ifstream binarka;
	binarka.open(nazwa, ios::binary | ios::in);

	if (binarka.is_open())
	{
		while (ilosc_rekordow_pozostala_do_wyswietlenia>0)
		{
			cout << "Adres: " << licznik *ROZMIAR_REKORDU ;
			binarka.read((char*)& klucz, sizeof(klucz));
			binarka.read((char*)& wartosc_a, sizeof(wartosc_a));
			binarka.read((char*)& wartosc_y, sizeof(wartosc_y));
			binarka.read((char*)& wartosc_c, sizeof(wartosc_c));
			binarka.read((char*)& wartosc_z, sizeof(wartosc_z));
			binarka.read((char*)& wartosc_x, sizeof(wartosc_x));
			ilosc_rekordow_pozostala_do_wyswietlenia--;
			licznik++;

			if (wyswietlanie == true)
				cout<<"\t" << klucz << "\t\t" << wartosc_a << " " << wartosc_y << " " << wartosc_c << " " << wartosc_z << " " << wartosc_x << endl;
		}
		if (wyswietlanie == true)cout << endl;
		if (wyswietlanie == true)cout << "Plik zawiera " << licznik << " rekordów" << endl;
		binarka.close();
	}
	else
	{
		cout << "Blad z otwarciem pliku w wyswietlaniu pliku " << nazwa << endl;;
	}
}
void stworz_czyste_drzewo(unsigned long long int klucz, double a, double y, double c, double z, double x) {

	//korzen = new  Wezel_posredni(); 
	//Lisc *lewy = new Lisc();
	//Lisc *prawy = new Lisc();
	//lewy->rodzic = korzen;
	//prawy->rodzic = korzen;
	//korzen->wsk[0] = lewy;
	//korzen->wsk[1] = prawy;
	//korzen->tablica_kluczy[0] = klucz;

	//int adresik = kolejny_ades();

	//lewy->wpisz_do_liscia(klucz, adresik);
	//korzen->ile_zapelnionych++;
	//korzen->rodzic = NULL;


	int adresik = kolejny_ades();
	zapisz_pod_adresem(adresik, klucz, a, y, c, z, x);
	korzen = new Lisc();
	korzen->wpisz_do_liscia(klucz, adresik);
	korzen->rodzic = NULL;
		



}
void dodaj_rekord(unsigned long long int klucz, double a, double y, double c, double z, double x) {
	cout << "do drzewa dodajemy rekord o kluczu = " << klucz << endl;
	if (korzen==NULL) {//brak drzewa
		stworz_czyste_drzewo(klucz, a, y, c, z, x);
	}
	else if(true)//drzewo juz jest
	{
		korzen->dodaj_rekord_z_zewnatzr(klucz, a, y, c, z, x);
	}
}
void tworze_Bg_na_podstawie_rekordow_z_pliku(char *nazwa) {
	
	int ilosc_pozostalych_rekordow = ile_rekordow_bin(nazwa);
	double wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x;
	unsigned long long int klucz;
	std::ifstream obrabiany(nazwa,ios::binary | ios::in);
	if (obrabiany.is_open())
	{
		
		while (ilosc_pozostalych_rekordow > 0)
		{
			obrabiany.read((char*)& klucz, sizeof(klucz));
			obrabiany.read((char*)& wartosc_a, sizeof(wartosc_a));
			obrabiany.read((char*)& wartosc_y, sizeof(wartosc_y));
			obrabiany.read((char*)& wartosc_c, sizeof(wartosc_c));
			obrabiany.read((char*)& wartosc_z, sizeof(wartosc_z));
			obrabiany.read((char*)& wartosc_x, sizeof(wartosc_x));
			
			dodaj_rekord(klucz, wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x);

			//wyswietl_drzewo(korzen);
			ilosc_pozostalych_rekordow--;
		}
		obrabiany.close();
		return;
	}
	else
	{
		cout << "Blad z otwarciem pliku z rekordami " << endl;
		return;
	}
}
bool znajdz(unsigned long long int klucz) {
	if (korzen == NULL)
		return false;

	unsigned long long int adres = korzen->szukaj(klucz);
	if (adres == _I64_MAX)
		return false;
	else
	{
		return true;
	}
}
void dodawanie_reczne_rekordu(bool uproszczone) {
	double wartosc_a = 0;
	double wartosc_y = 0;
	double wartosc_c = 0;
	double wartosc_z = 0;
	double wartosc_x = 0;
	unsigned long long int klucz;
	bool warunek=true;
	//if (korzen == NULL) {//brak drzewa		//chyba niepotrzebne
	//	cout << "Podaj wartość klucza: ";
	//	cin >> klucz;
	//	if (uproszczone == false)
	//	{
	//		cout << "\nPodaj wartosc a, a= ";
	//		cin >> wartosc_a;
	//		cout << "\nPodaj wartosc y, y= ";
	//		cin >> wartosc_y;
	//		cout << "\nPodaj wartosc c, c= ";
	//		cin >> wartosc_c;
	//		cout << "\nPodaj wartosc z, z= ";
	//		cin >> wartosc_z;
	//		cout << "\nPodaj wartosc x, x= ";
	//		cin >> wartosc_x;
	//	}
	//	stworz_czyste_drzewo(klucz, wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x);
	//	return;
	//}
	while (warunek)
	{
		cout << "Podaj wartość klucza: ";
		cin >> klucz;
		if (znajdz(klucz)==true)
			cout << "Wprowadz ponownie dane, taki klucz już istnieje" << endl;
		else
		{
			cout << "klucz ok" << endl;
			warunek = false;
		}
	}
	if (uproszczone == false)
	{
		cout << "\nPodaj wartosc a, a= ";
		cin >> wartosc_a;
		cout << "\nPodaj wartosc y, y= ";
		cin >> wartosc_y;
		cout << "\nPodaj wartosc c, c= ";
		cin >> wartosc_c;
		cout << "\nPodaj wartosc z, z= ";
		cin >> wartosc_z;
		cout << "\nPodaj wartosc x, x= ";
		cin >> wartosc_x;
	}
	dodaj_rekord(klucz, wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x);
};
void wyswietl_zawartosc_adresu(unsigned long long int adres, char *nazwa) {
	double wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x;
	unsigned long long int klucz;
	ifstream binarka;
	binarka.open(nazwa, ios::binary | ios::in);

	if (binarka.is_open())
	{
		binarka.seekg(adres, ios_base::beg);
		binarka.read((char*)& klucz, sizeof(klucz));
		binarka.read((char*)& wartosc_a, sizeof(wartosc_a));
		binarka.read((char*)& wartosc_y, sizeof(wartosc_y));
		binarka.read((char*)& wartosc_c, sizeof(wartosc_c));
		binarka.read((char*)& wartosc_z, sizeof(wartosc_z));
		binarka.read((char*)& wartosc_x, sizeof(wartosc_x));
		cout << klucz << "\t\t" << wartosc_a << " " << wartosc_y << " " << wartosc_c << " " << wartosc_z << " " << wartosc_x << endl;
		binarka.close();
	}
	else
	{
		cout << "Blad z otwarciem pliku w wyswietlaniem zawartosci adresu z pliku" << nazwa << endl;;
	}
}
void zapisz_pod_adresem(int adres, unsigned long long int klucz, double a, double y, double c, double z, double x) {
	fstream plik;
	plik.open(rekordyDrzewa, ios::in | ios::out | ios::binary);
	if (plik.is_open())
	{
		double bufor[5];
		plik.seekp(adres, ios_base::beg);
		plik.write((char*)&klucz, sizeof(klucz));
		bufor[0] = a;
		bufor[1] = y;
		bufor[2] = c;
		bufor[3] = z;
		bufor[4] = x;
		plik.write((char*)&bufor, sizeof(bufor));
		plik.close();
	}
	else
	{
		cout << "Blad z otwarciem pliku podczas zapisywaniu rekordu " << rekordyDrzewa << endl;;
	}
}
void podejrzyj_rekord() {
	unsigned long long int klucz;
	cout << "podaj klucz rekordu do odczytania" << endl;
	cin >> klucz;
	unsigned long long int adres = korzen->szukaj(klucz);
	if (adres == _I64_MAX)
		cout << "Niestety taki klucz nie istnieje" << endl;
	else
	{
		cout << "Znaleziono rekord pod adresem" << adres <<endl;
		wyswietl_zawartosc_adresu(adres, rekordyDrzewa);
	}
};
void aktualizuj_rekord() {
	unsigned long long int klucz,klucz2;
	double wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x;
	cout << "podaj klucz rekordu do aktualizacji" << endl;
	cin >> klucz;
	unsigned long long int adres = korzen->szukaj(klucz);
	if (adres == _I64_MAX)
		cout << "Niestety taki klucz nie istnieje" << endl;
	else
	{
		cout << "Rekord jest pod adresem" << adres <<"i zawiera nastepujące dane"<< endl;
		wyswietl_zawartosc_adresu(adres, rekordyDrzewa);
		cout << "podaj nowy klucz" << endl;
		cin >> klucz2;
		cout << "\nPodaj wartosc a, a= ";
		cin >> wartosc_a;
		cout << "\nPodaj wartosc y, y= ";
		cin >> wartosc_y;
		cout << "\nPodaj wartosc c, c= ";
		cin >> wartosc_c;
		cout << "\nPodaj wartosc z, z= ";
		cin >> wartosc_z;
		cout << "\nPodaj wartosc x, x= ";
		cin >> wartosc_x;
		if (klucz == klucz2)
		{
			zapisz_pod_adresem(adres, klucz2, wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x);
		}
		else
		{
			korzen->usun(klucz);
			dodaj_rekord(klucz2, wartosc_a, wartosc_y, wartosc_c, wartosc_z, wartosc_x);
		}
	}
};
void usuwanie_rekordu() {
	unsigned long long int klucz;
	cout << "podaj klucz rekordu do usuniecia " << endl;
	cin >> klucz;
	korzen->usun(klucz);
};
void wyswietl_licznik()
{
	cout << "licznik= " << licznik << endl;
	cout << "licznik lisci=" << licznik_lisci << endl;
	cout << "licznik licznik_wezlow_pom=" << licznik_wezlow_pom << endl;
	cout << "licznik loperacje_odczytu=" << operacje_odczytu << endl;
	cout << "licznik operacje_zapisu=" << operacje_zapisu << endl;
};
void usun_drzewo() {
	wyczysc_stos_i_plik();
	korzen = NULL;
	licznik = 0;
	licznik_lisci = 0;
	licznik_wezlow_pom = 0;
	operacje_odczytu = 0;
	operacje_zapisu = 0;
};





///////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	char znak=10;
	unsigned long long int klucz;
	int ilosc;

	while (znak != 'k')
	{
		cout << "------------------------------------------------------------" << endl;
		cout << "plik z rekordami wygenerowanymi: " << pliksamychrekordow << endl;
		cout << "plik z rekordami w drzewie: " << rekordyDrzewa << endl;
		cout << "Co chcesz zrobić?" << endl<<endl;
		cout << "u - utwórz plik z losowymi rekordami" << endl;
		cout << "p - przegladaj wygenerowany plik " << endl<<endl;

		cout << "t - stworzyc B* na podstawie wygenerownych rekordów" << endl<<endl;

		cout << "d - dodawanie rekordu do obecnego drzewa" << endl;
		cout << "m - dodawanie rekordu uproszczone" << endl;
		cout << "a - aktualizacja rekordu" << endl;
		cout << "o - odczyt rekordu" << endl;
		cout << "v - usuwanie rekordu" << endl;
		cout << "  - reorganizacja" << endl << endl;

		cout << "q - przegladaj plik rekordow drzewa" << endl;
		cout << "w - wyswietl obecne drzewo" << endl;
		cout << "s - wykonaj sekwencje operacji (z kodu) dodania" << endl;
		cout << "x - usun drzewo z pamieci" << endl;
		cout << "k - koniec" << endl;
		cout << "l - wyswietl licznik" << endl;


		do {
			znak = getchar();
		} while (znak == 10);
		switch (znak)
		{
		case 'u':
			cout << "podaj ilosc rekordow "  <<endl;
			cin >> ilosc;
			utworzPlikRekordow(ilosc);
			wyswietl_plik_bin_adresy(pliksamychrekordow, true);
			break;

		case 'p':
			wyswietl_plik_bin_adresy(pliksamychrekordow, true);
			break;

		case 'q':
			wyswietl_plik_bin_adresy(rekordyDrzewa, true);
			break;
		case 't':
			cout << "tworze B* na podstawie " << pliksamychrekordow << endl;
			nowa = true;
			wyczysc_stos_i_plik();
			tworze_Bg_na_podstawie_rekordow_z_pliku(pliksamychrekordow);
			break;
		case 'd':
			cout << "dodawanie rekordu do drzewa" << endl;
			dodawanie_reczne_rekordu(false);
			break;
		case 'a':
			aktualizuj_rekord();
			break;
		case 'v':
			usuwanie_rekordu();
			break;
		case 'o':
			podejrzyj_rekord();
			break;
		case 'm'://uproszczone
			dodawanie_reczne_rekordu(true);
			break;
		case 'w':
			wyswietl_drzewo(korzen);
			break;
		case 'x':
			usun_drzewo();
			break;
		case 'l':
			wyswietl_licznik();
			break;

		case 's':
			//sekwencja do testowania
			dodaj_rekord(12, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(8, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(4, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(13, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(17, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(19, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(22, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(1, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(44, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(15, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(38, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			dodaj_rekord(50, 35234, 464, 6346, 64, 646);
			wyswietl_drzewo(korzen);
			//dodaj_rekord(7, 35234, 464, 6346, 64, 646);
			//wyswietl_drzewo(korzen);
			//dodaj_rekord(21, 35234, 464, 6346, 64, 646);
			//wyswietl_drzewo(korzen);
			//dodaj_rekord(_I64_MAX, 35234, 464, 6346, 64, 646);
			//wyswietl_drzewo(korzen);
			//dodaj_rekord(20, 35234, 464, 6346, 64, 646);
			//wyswietl_drzewo(korzen);

			break;
		}
	}
	return 0;
}
