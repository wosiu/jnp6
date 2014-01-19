#include "grubaryba.h"

typedef std::string pole_key_t;

const unsigned int POCZATKOWA_KASA = 1000;
const float KARA_ZA_SPRZEDAZ = 1/2;

// do fabryki
const pole_key_t AKWARIUM = "akwarium";
const pole_key_t DEPOZYT = "depozyt";
const pole_key_t KARA_REKIN_180 = "kara_rekin";
const pole_key_t NAGRODA_BLAZENKI_120 = "nagroda_blazenki";
const pole_key_t START = "start";
const pole_key_t OB_UZ_PUB_GROTA_300 = "ob_uz_pub_grota";
const pole_key_t OB_UZ_PUB_STATEK_250 = "ob_uz_pub_statek";
const pole_key_t KORALOWIEC_ANEMONIA_160 = "koralowiec_anemonia";
const pole_key_t KORALOWIEC_APORINA_220 = "koralowiec_aporina";
const pole_key_t KORALOWIEC_MENELLA_280 = "koralowiec_menella";
const pole_key_t KORALOWIEC_PENNATULA_400 = "koralowiec_pennatula";

// hierarchia Pola
class Pole;
	class Akwarium;
	class Depozyt;
	class Kara;
	class Nagroda;
	class Start;
	class Nieruchomosc;
		class Ob_uz_pub;
		class Koralowiec;
// Strategia
class Strategia;
	class Czlowiecza;
	class Dumb;
	class Smartass;

class Strategia {
public:
	// domyslnie zwracaja false
	virtual bool wantSell(const Nieruchomosc& n) { return false; }
	virtual bool wantBuy(const Nieruchomosc& n , int money) { return false; }
protected:
	bool mozliweDoKupienia(int money, int cena) {
		return money >= cena;
	}
};

class Smartass : public Strategia {
public:
	bool wantBuy(const Nieruchomosc& n , int money) {
		return mozliweDoKupienia(money , n.getCena());
	}
};

class Dumb : public Strategia {
public:
	Dumb() : licznik() {}
	bool wantBuy(const Nieruchomosc& n , int money) {
		nast();
		if( licznik == 1 ) 
			return mozliweDoKupienia(money , n.getCena());
		else
			return false;
	}
private:
	void nast() {
		licznik++;
		licznik %= MODULO;
	}
	int licznik;
	static const int MODULO = 3;
};

// strategia czlowieka! wazne
// TO DO
class Czlowiecza : public Strategia {
public:
private:
	// tutaj trzymajmy pointer na czlowieka! wtedy wystarczy zwracac:
	// return ptr -> wantBuy itd ...
}

class Gracz {
public:
	Gracz(std::string nazwa, unsigned int id, Strategia strategia) : nazwa(nazwa),
   							money(POCZATKOWA_KASA), id(id), ile_rund_postoju(0),
							strategia(strategia) {}


	// plac zwraca ilosc kasy jaka graczowi udalo sie zaplacic
	// ponadto sprawdza czy gracz ma gotowke, i czy czasem nie zbankrutuje
	// jezeli zbankrutuje to funkcja moze zwrocic wynik t, ze:
	//		wynik < cena
	int plac(unsigned int cena); // TO DO

	void giveMoney(unsigned int kasa) { money += kasa; }
	bool wantBuy(const Nieruchomosc& n) {
		return strategia.wantBuy(n, money);
	}
	void wantSell(Nieruchomosc& n) {
		if( strategia.wantSell(n) ) {
			giveMoney(n.getCena() * KARA_ZA_SPRZEDAZ);
			n.sprzedana();
		}
	}

private: 
	std::string nazwa;
	int money;
	unsigned int id; //pozycja w wektorze graczy

	int ile_rund_postoju;
	Strategia strategia;
	// albo vector ptr na te posiadlosci? chodzi o to zeby to byly TE same obiekty
	// ktore plansza przechowuje w sobie`
	std::vector<Nieruchomosc> posiadlosci;
};


// Interface pola
class Pole {
	// czy tutaj tak jak w javie funkcje abstrakcyjen moga cos robic?
public:
	virtual ~Pole() {}
	// gdy gracz przechodzi przez pole
	virtual void passing(Gracz& g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void action(Gracz& g) {}
};

class Akwarium : public Pole {
};

class Depozyt : public Pole {
private:
	unsigned int gotowka;
	static const unsigned int stawka = 15;
public:
	Depozyt() : gotowka(0) {}
	void action(Gracz& g) {
		g.giveMoney(gotowka);
		gotowka = 0;
	}
	void passing(Gracz& g) { g.plac(stawka); }
};

class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara) : kara(kara) {}
	void action(Gracz& g) { g.plac(kara); }
};

class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda) : nagroda(nagroda) {}
	void action(Gracz& g) { g.giveMoney(nagroda); }
};

class Start : public Pole {
private:
	// czy to tak sie pisało?
	static const unsigned int nagroda = 50;
public:
	void passing(Gracz& g) { this->action(g); }
	void action(Gracz& g) { g.giveMoney(nagroda); }
};

// Interface nieruchomosci
class Nieruchomosc : public Pole {
	// czy nieruchomosc tez musi miec virutal destructor ?
public:
	virtual ~Nieruchomosc() {}
	virtual void passing(Gracz& g) {}
	virtual void action(Gracz& g) = 0; // tutaj kazdy ma swoja akcje!
	unsigned int getCena() const { return cena; }
	unsigned int getNazwa() const { return nazwa; }
	void sprzedana() { zajete = false; }

protected:
	bool zajete;
	unsigned int id_wlasciciela; // pozycja wlasciciela w vektorze graczy
	// tutaj moze dodac wskaznik na wlasciciela?
	Gracz& wlasciciel; //, zeby wiedziec komu sie placi

	// TO DO
	bool tenSamWlasciciel(const Gracz& g) const;
	// oferujac kupno przekazujemy samo pole
	// gracz sam wyciagnie cene i nazwe przez gety
	void oferujKupno(Gracz& g);

private:
	unsigned int cena;
	unsigned int stawka;
	std::string nazwa; //potrzebne do przekazywania Humanowi czy chce kupic
};

class Ob_uz_pub : public Nieruchomosc {
public:
	Ob_uz_pub(unsigned int cena) : zajete(false), wlasciciel(0),
   								cena(cena) , stawka(cena * procent) {}
	void action(Gracz& g);
private:
	static const float procent = 4/10;
};

void Ob_uz_pub::action(Gracz& g) {
	if( !zajete )
		oferujKupno(g);
	else if( !tenSamWlasciciel(g) ) {
		wlasciciel.giveMoney( g.plac(stawka) );
	}
}

class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena) : zajete(false), wlasciciel(0),
   								cena(cena), stawka(cena * procent) {}
	void action(Gracz& g);
private:
	static const float procent 2/10;
};

void Koralowiec::action(Gracz& g) {
	if( !zajete )
		oferujKupno(g);
	else if( !tenSamWlasciciel ) {
		wlasciciel.giveMoney( g.plac(stawka) );
	}
}

class Plansza {
public:
	Plansza() { stworzPlansze_1(); }
private:
	vector<Pole> pola;
	static Fabryka fabryka; // czy to na pewno tutaj?
							// jak zrobic zeby tylko jedna sie utworzyla?
	void stworzPlansze_1();
};

void Plansza::stworzPlansze_1() {
	pola.clear(); // czy jak to tam sie nazywa
	pola.push_back(f.createPole(START)); // itd!
	// itd ... moze to da sie lepiej niz tak recznie?
}

class Fabryka {
public:
	// TO DO :
	// pytanie - gdzie zdefiniowac stale ID_AKWARIUM itd..
	// beda uzywane i w fabryce i w planszy przy jej tworzeniu
	Pole createPole(const pole_key_t& klucz);
};

Pole Fabryka::createPole(const pole_key_t& k) {
	if( k.compare(AKWARIUM) == 0 )
		return new Akwarium();
	if( k.compare(DEPOZYT) == 0 )
		return new Depozyt();
	if( k.compare(KARA_REKIN_180) == 0 )
		return new Kara(180);
	if( k.compare(NAGRODA_BLAZENKI_120) == 0 )
		return new Nagroda(120);
	if( k.compare(START) == 0 )
		return new Start();
	// Obiekty uzyteczonsci publicznej:
	if( k.compare(OB_UZ_PUB_STATEK_250) == 0 )
		return new Ob_uz_pub(250);
	if( k.compare(OB_UZ_PUB_GROTA_300) == 0 )
		return new Ob_uz_pub(300);
	// Koralowce:
	if( k.compare(KORALOWIEC_ANEMONIA_160) == 0 )
		return new Koralowiec(160);
	if( k.compare(KORALOWIEC_APORINA_220) == 0 )
		return nwe Koralowiec(220);
	if( k.compare(KORALOWIEC_MENELLA_280) == 0 )
		return new Koralowiec(280);
	if( k.compare(KORALOWIEC_PENNATULA_400) == 0 )
		return new Koralowiec(400);

	return 0; // tutaj mozna cos domyslnie zwrocic
}

class MojaGrubaRyba : public GrubaRyba {
public:
    // Przekazuje prototypową kostkę do gry.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    virtual void setDie(std::shared_ptr<Die> die) = 0;

    // Dodaje nowego gracza komputerowego, który gra na zdefiniowanym poziomie.
    // Nowy gracz komputerowy nazywa się Gracz<Numer>, gdzie <Numer> to numer pozycji,
    // na której został dodany (numeracja pozycji od 1).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    virtual void addComputerPlayer(ComputerLevel level) = 0;

    // Dodaje nowego gracza czlowieka.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    virtual void addHumanPlayer(std::shared_ptr<Human> human) = 0;

    // Przeprowadza rozgrywkę co najwyżej podanej liczby rund (rozgrywka może
    // skończyć się wcześniej).
    // Jedna runda obejmuje po jednym ruchu każdego gracza.
    // Gracze ruszają się w kolejności, w której zostali dodani.
    // Na początku każdej rundy wypisywany jest numer rundy,
    // a na zakończenie informacje podsumowujące dla każdego gracza (format w przykładzie).
    // Rzuca NoDieException, jeśli nie przekazano kostki.
    // Rzuca TooFewPlayersException, jeśli liczba graczy nie pozwala na rozpoczęcie gry.
    virtual void play(unsigned int rounds) = 0;
private:
	Die die;
};
