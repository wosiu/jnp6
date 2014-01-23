#include "mojagrubaryba.h"

// TODO:
// add no except gdzie trza

typedef std::string pole_key_t;

const unsigned int POCZATKOWA_KASA = 1000;
const float KARA_ZA_SPRZEDAZ = 1.0 / 2.0;

// do fabryki
const pole_key_t WYSPA = "wyspa";
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
// hierarchia Strategii
class Strategia;
class Czlowiecza;
class Dumb;
class Smartass;

class Strategia {
public:
	// domyslnie zwracaja false
	virtual bool wantSell(const Nieruchomosc& n , const Gracz& g) { return false; }
	virtual bool wantBuy(const Nieruchomosc& n , const Gracz& g) { return false; }
protected:
	bool mozliweDoKupienia(int money, int cena) {
		return money >= cena;
	}
};

class Smartass : public Strategia {
public:
	bool wantBuy(const Nieruchomosc& n , const Gracz& g) {
		return mozliweDoKupienia(g.getMoney(), n.getCena());
	}
};

class Dumb : public Strategia {
public:
	Dumb() : licznik(0) {}
	bool wantBuy(const Nieruchomosc& n , const Gracz& g) {
		nast();
		if( licznik == 1 )
			return mozliweDoKupienia(g.getMoney(), n.getCena());
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

// TO DO
// - konstruktor + ptr / shrd_ptr na czlowieka
// do tego gdzies trzeba tworzyc te strategie ( w MojaGrubaRyba)
// mozna je tworzyc w fabryce
class Czlowiecza : public Strategia {
public:
	bool wantSell(const Nieruchomosc& n) { return czlowiek->wantSell; }
	bool wantBuy(const Nieruchomosc& n) { return czlowiek->wantBuy; }
private:
	// TO DO
	// shr_ptr na czlowieka
};

// TO DO
//   int plac( ... ) (wywolywana przez pola gdy gracz musi zaplacic)
//   	sprawdza czy czlowiek nie bankrutuje, zwraca hajs jaki udalo sie zaplacic graczowi
//   bool wantBuy( ...)
//   void wantSell( ... )
// straszne rzeczy tu sie dzieja :<
class Gracz {
public:
	// TO DO
	// czy ten kontruktor dobry? (czy mozna tak strategie przekopiowac? )
	Gracz(std::string nazwa, unsigned int id, Strategia strategia) : nazwa(nazwa),
   							money(POCZATKOWA_KASA), id(id), ile_rund_postoju(0),
							strategia(strategia) {}
	// TO DO
	int plac(unsigned int cena);
	// TO DO
	bool wantBuy(const Nieruchomosc& n) {

		if( strategia.wantBuy(n, this) ) {
			// gdy strategia zwroci TRUE ==> mozemy kupic nieruchomosc (stac nas na to)
			// tutaj dodanie nieruchomosci do vektora!
			// i odjecie kasy!
			return true;
		} else
			return false;
	}
	// TO DO
	void wantSell(Nieruchomosc& n) {
		if( strategia.wantSell(n, this) ) {
			giveMoney(n.getCena() * KARA_ZA_SPRZEDAZ);
			n.sprzedana();
			// TO DO
			// wywalic nieruchomosc z wektora nieruchomosci!
		}
	}

	void postoj(int postoj) { ile_rund_postoju = postoj; }
	void giveMoney(unsigned int kasa) { money += kasa; }
	int getMoney() const { return money; }
	int getId() const { return id; }

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
	// odp: tak.
public:
	// gdy gracz przechodzi przez pole
	virtual void przejdz(Gracz& g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void stan(Gracz& g) {}
};

class Wyspa : public Pole {
	// Wyspa nic nie robi
};

class Akwarium : public Pole {
	static const int ile_tur = 3;
	void stan(Gracz& g) { g.postoj(ile_tur); }
};

class Depozyt : public Pole {
private:
	unsigned int gotowka;
	static const unsigned int stawka = 15;
public:
	Depozyt() : gotowka(0) {}
	void stan(Gracz& g) {
		g.giveMoney(gotowka);
		gotowka = 0;
	}
	void przejdz(Gracz& g) { g.plac(stawka); }
};

class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara) : kara(kara) {}
	void stan(Gracz& g) { g.plac(kara); }
};

class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda) : nagroda(nagroda) {}
	void stan(Gracz& g) { g.giveMoney(nagroda); }
};

class Start : public Pole {
private:
	static const unsigned int nagroda = 50;
public:
	void stan(Gracz& g) { g.giveMoney(nagroda); }
	void przejdz(Gracz& g) { this->stan(g); }
};

// Interface nieruchomosci
class Nieruchomosc : public Pole {
	// czy nieruchomosc tez musi miec virutal destructor ?
public:
	virtual ~Nieruchomosc() {}
	virtual void przejdz(Gracz& g) {} //domyslnie nic
	virtual void stan(Gracz& g) = 0;

	unsigned int getCena() const { return cena; }
	unsigned int getNazwa() const { return nazwa; }
	void sprzedana() { zajete = false; }

protected:
	bool zajete;
	unsigned int id_wlasciciela; // pozycja wlasciciela w vektorze graczy
	Gracz& wlasciciel; // CZy to tak? moze zamiast tego shr_ptr?

	// TO DO
	// oferujac kupno przekazujemy samo pole
	// gracz sam wyciagnie cene i nazwe przez gety
	void oferujKupno(Gracz& g);
	bool tenSamWlasciciel(const Gracz& g) const { return g.getId() == id_wlasciciela; }
private:
	unsigned int cena;
	unsigned int stawka;
	std::string nazwa; //potrzebne do przekazywania Humanowi czy chce kupic
};

class Ob_uz_pub : public Nieruchomosc {
public:
	Ob_uz_pub(unsigned int cena) : zajete(false), wlasciciel(0),
   								cena(cena) , stawka(cena * procent) {}
	void stan(Gracz& g);
private:
	static const float procent = 4.0 / 10;
};

void Ob_uz_pub::stan(Gracz& g) {
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
	void stan(Gracz& g);
private:
	static const float procent 2.0 / 10;
};

void Koralowiec::stan(Gracz& g) {
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
	//
	// nieruchomosci oprocz ceny powinny miec jeszzcze nazwe!
	// trzeba to dodac w createPole do nieruchomosci
	Pole createPole(const pole_key_t& klucz);
};

Pole Fabryka::createPole(const pole_key_t& k) {
	if( k.compare(WYSPA) == 0 )
		return new Wyspa();
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


// ============================ MojaGrubaRyba ==================================
MojaGrubaRyba::MojaGrubaRyba() {

}

void MojaGrubaRyba::setDie(std::shared_ptr<Die> die) {

}

void MojaGrubaRyba::addComputerPlayer(ComputerLevel level) {

}

void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human) {

}

void MojaGrubaRyba::play(unsigned int rounds) {
	if ( die = nullptr ) {
		throw NoDieException();
	}
	if ( gracze.size() < MIN_GRACZY ) {
		throw TooFewPlayersException(MIN_GRACZY);
	}
	// iterujemy po rundach
	for ( auto r = 1; r <= rounds; r++ ) {
		printf("Runda: %d\n", r);
		// iterujemy po kolejnych graczach w rundzie
		for ( size_t g = 0; g < gracze.size(); g++ ) {
			Gracz* gracz = &gracze[g];

			if ( gracz->getPostoj() > 0 ) {
				gracz->getPostoj()--;
				continue;
			}

			auto rzut = die.roll() + die.roll();
			auto poz = gracz.getPozycja();

			// mijamy rzut - 1 pol
			for ( int i = 1; i < rzut; i++ ) {
				plansza[( poz + i ) % plansza.size()].przejdz( gracz );
			}
			plansza[(poz + rzut ) % plansza.size()].stan( gracz );
		}
	}
}
