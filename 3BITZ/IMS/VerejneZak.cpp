/******************************************************************************
Projekt IMS - Workflow v administrative

Autori: Brychta Tomas (xbrych01@stud.fit.vutbr.cz)
		Zaklova Katerina (xzaklo00@stud.fit.vutbr.cz) - zastupce tymu

Zakladni casova jednotka je 1 den
Pro prevod na roky je pouzita konstanta ROK

******************************************************************************/


#include "simlib.h"

// pouzito kvuli std::atol
#include <cstdlib>

// pouzito pro std::string na jmeno souboru
#include <string>


#define ROK * 365

#define PRICH_PODN 0.351		// podnety a navrhy dle Vyrocni zpravy dochazi priblizne po 0.351 dne

//Defaultni hodnoty simulace
#define T_END 5 ROK	// doba behu simulace

#define ZAMESTNANCI 75		// orig val 75

#define KOMISE 12			// pocet zamestnancu komise

#define KOMISE_DOKU 15		// pocet paralelne zpracovanych dokumentu komise

#define ZAM_RIZENI 8		// pocet paralelne zpracovanych dokumentu zamestnancu u 1 inst. rizeni

#define MAX_DOKUM 2			// pocet paralelne zpracovanych dokumentu zamestnancu u prezkumu podnetu



// detailni vypisy... slouzily predevsim pro overeni validity modelu
#define DEBUG 0


Store Komise("Zamestnanci komise - 2 instancni rizeni", KOMISE * KOMISE_DOKU);		// kazdy clen komise muze zpracovavat vice dokumentu zaroven

// Zamestnanci jsou modelovani pomoci store protoze nas nezajima konkretne ktery zamestnanec zpracovava ktery podnet
Store Zamestnanci("Zamestnanci - rizeni", ZAMESTNANCI * ZAM_RIZENI);		// kazdy zamestnanec muze zpracovavat vice rizeni zaroven
Store Zam_prezkum("Zamestnanci - prezkum podnetu", ZAMESTNANCI*MAX_DOKUM);	// kazdy zamestnanec muze zpracovavat vice podnetu zaroven

Histogram celk_casH("Cas straveny podnetem v systemu", 0, 0.5 ROK, 10);

Histogram podnety_prijH("Podnety prijate", 0, 1 ROK, 5);

Histogram rozkladyH("Podane rozklady", 0, 1 ROK, 5);

Histogram zalobyH("Podane zaloby", 0, 1 ROK, 5);

double min_time = 5 ROK;
double max_time = 0;


long podnet = 0;
long hotovo = 0;

#if DEBUG
Histogram celk_cas_detailedH("Detailni histogram casu podnetu v systemu", 0, 5, 1 ROK);		// 1 rok z duvodu ze 5 let po 5 dnech je pocet hodnot 365

long odmitnuto = 0;
long zalob = 0;
long rozklad = 0;
long zmeneno = 0;
long vraceno = 0;
long zruseno_komise = 0;
long potvrzeno = 0;
long konec_rozkl = 0;
long konec_rizeni = 0;
#endif


class Podnet : public Process
{

	int zaloba;		// promenna indikujici zda byl podnet jiz u zaloby k soudu

	void Behavior()
	{
		double start_time = Time;	// promenna pro histogram
		podnet++;
		zaloba = 0;
		double aux = 0;		// pomocna promenna pro pravdepodobnost	
		
		Enter(Zam_prezkum,1);
		
		Wait(Exponential(30));		// probiha prezkoumani podnetu

		Leave(Zam_prezkum,1);
		

		if(Random() > 0.5105)		// 51.05% podnetu projde do rizeni 
		{
#if DEBUG
			odmitnuto++;
#endif
			goto konec;
		}
		podnety_prijH(Time);		// ulozime do histogramu udaj o prijmuti podnetu
zaber_rizeni:

		Enter(Zamestnanci, 1);
		
		Wait(Exponential(60));			// probiha rizeni u soudu

		Leave(Zamestnanci,1);
		
#if DEBUG 
		konec_rizeni++; 
#endif


		if(zaloba)			// pokud rizeni znovu probehlo na podnet soudu neni mozne podat rozklad ani zalobu
			goto konec;

		if(Random() > 0.549)			// proti 54,9% rozhodnuti byl podan rozklad
			goto konec;

#if DEBUG
		rozklad++;
#endif
		rozkladyH(Time);		// ulozime do histogramu udaj o podanem rozkladu

		// zabrani komise
		Enter(Komise,1);
		
		Wait(Exponential(50));			// probiha rozhodovani komise
#if DEBUG
		konec_rozkl++;
#endif

		aux = Random();
		if(aux < 0.005)		// 0.5% pripadu se vraci k rizeni 1 stupne
		{
#if DEBUG
			vraceno++;
#endif
			Leave(Komise,1);
			goto zaber_rizeni;
		}
		else if(aux < 0.20)		// 19,5% pripadu bylo zruseno rozhodnuti a podnet opousti system
		{
#if DEBUG
			zruseno_komise++;
#endif
			Leave(Komise,1);
			goto konec;
		}
		// v 80% pripadu bylo potvrzeno prvoinstancni rozhodnuti

		// pokud bylo potvrzeno prvoinstancni rozhodnuti 17.47% rozhodnuti bylo podano k soudu
		if(Random() > 0.1747)
		{
			Leave(Komise,1);
			goto konec;
		}
		zalobyH(Time);		// ulozime do histogramu udaj o podane zalobe
#if DEBUG
		zalob++;
#endif
		// probiha soud
		Wait(Exponential(1 ROK));

		aux = Random();
		if(aux > 0.99)		// 1% pripadu je vraceno na prezkoumani 
		{
#if DEBUG
			vraceno++;	
#endif
			zaloba++;
			goto zaber_rizeni;
		}
#if DEBUG		
		if(aux >= 0.652)		// v 33.8% pripadu je zmeneno rozhodnuti UOHS
			zmeneno++;
		else
			potvrzeno++;
#endif
konec:
		hotovo++;
#if DEBUG
		celk_cas_detailedH(Time - start_time);
#endif
		celk_casH(Time - start_time);

		// ziskavame nejmensi a nejvetsi cas straveny v systemu
		if((Time - start_time) > max_time)
			max_time = (Time - start_time);
		if((Time - start_time) < min_time)
			min_time = (Time - start_time);
	}
};

// generuje podnety
class Generator : public Event
{
	void Behavior()
	{
		(new Podnet)->Activate();
		Activate(Time + Exponential(PRICH_PODN));
	}
};

int main(int argc, char *argv[])
{

	// promenne pro ukladani argumentu prikazove radky
	std::string filename = "VerejneZak.out";
	long zam_kapacita = ZAMESTNANCI;
	long zam_dokumenty = MAX_DOKUM;
	long runtime = T_END;
	long kom_dokumenty = KOMISE_DOKU;
	long zam_rizeni = ZAM_RIZENI;
	
	// primitivni zpracovani argumentu, pro vice vystupu s ruznymy hodnotami u 'make run'
	if(argc >= 2)
	{
		if(argv[1][0] != 'd')			// prvni argument udava pocet zamestnancu
		{
			zam_kapacita = std::atol(argv[1]);
			Zamestnanci.SetCapacity(zam_kapacita);
		}
	}
	if(argc >= 3)
	{
		if(argv[2][0] != 'd')			// druhy argument udava pocet dokumenu ktery muze jeden zamestnanec zpracovavat paralelne
			zam_dokumenty = std::atol(argv[2]);
	}
	if(argc >= 4)
	{
		if(argv[3][0] != 'd')
			kom_dokumenty = std::atol(argv[3]);		// kolik rozkladu muze maximalne zpracovavat jeden clen komise zaroven
	}

	if(argc >=5)		// pocet paralelne zpracovavanych rizeni u 1 zamestnance
	{
		if(argv[4][0] != 'd')
			zam_rizeni = std::atol(argv[4]);
	}
	if(argc >= 6)
	{
		if(argv[5][0] != 'd')			// udava jak dlouho simulace pobezi(roky), akceptuje pouze celociselne hodnoty
			runtime = (std::atol(argv[5])) ROK;
	}
	if(argc >= 7)		// jmeno souboru do ktereho bude tisknut vystup simulace(pokud nebude zadan je vystup 'VerejneZak.out')
	{
		filename = argv[6];
		filename.append(".out");	
	}
	// konec zpracovani argumentu

	// nastavime kapacitu Store, v pripade ze nebyly zadany argumenty budou tyto hodnoty defaultni
	Zam_prezkum.SetCapacity(zam_dokumenty * zam_kapacita);
	Zamestnanci.SetCapacity(zam_rizeni * zam_kapacita);
	Komise.SetCapacity(KOMISE * kom_dokumenty);
	
	SetOutput(filename.c_str());		// vystupni soubor, defaultne 'VerejneZak.out'
	
	Init(0, runtime);		// nastavime konec simulace, defaultne 2 roky
	
	(new Generator)->Activate();	// budeme generovat jednotlive podnety
	
	Run();	// spustime simulaci

	Print("Zobrazene udaje byly ziskany za casove obdobi[roky]: %d\n\n", runtime/ (1 ROK));
	Print("Pocet prijatych podnetu: %d\n", podnet);
	Print("Pocet ukoncenych podnetu: %d\n\n", hotovo);
	Print("Maximalni cas straveny zpracovanym podnetem v systemu [dny]: %g\n", max_time);
	Print("Minimalni cas straveny zpracovanym podnetem v systemu [dny]: %g\n\n\n", min_time);
	
	// tiskneme statistiky Store vcetne front
	Zamestnanci.Output();
	Zam_prezkum.Output();
	Komise.Output();

	// tiskneme histogramy
	celk_casH.Output();
	podnety_prijH.Output();
	rozkladyH.Output();
	zalobyH.Output();

// detailni vypisy deju v systemu
#if DEBUG
	Print("\n\nDebug output\n\n***********************************************************************************\n\n");
	SIMLIB_statistics.Output();
	Print("\n\n");
	Print("Runtime : %d rok\n", runtime/(1 ROK));
	Print("Pocet zamestnancu: %d\n", zam_kapacita);
	Print("Pocet dokumentu zpracovavanych zaroven prezkum: %d\n",zam_dokumenty);
	Print("Pocet dokumentu zpracovavanych zaroven 2inst rizeni: %d\n",kom_dokumenty);
	Print("Pocet podnetu celkem: %d\n",podnet);
	Print("Pocet odmitnutych podnetu: %d\n",odmitnuto);
	Print("Pocet podanych zalob: %d\n",zalob);
	Print("Pocet podanych rozkladu: %d\n",rozklad);
	Print("Pocet zmenenych rozhodnuti: %d\n", zmeneno);
	Print("Pocet vracenych podnetu: %d\n", vraceno);
	Print("Pocet ukoncenych: %d\n", hotovo);
	Print("Konec rizeni: %d\n", konec_rizeni);
	Print("Konec rozkladu: %d\n", konec_rozkl);
	Print("Pocet neukoncenych podnetu: %d\n\n\n\n", podnet-hotovo);


	// tiskneme detailni histogram casu straveneho podnetem v systemu.... 365 radku, proto neni soucasti bezneho vypisu
	celk_cas_detailedH.Output();
#endif

}
