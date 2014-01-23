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
	virtual bool wantSell(std::shared_ptr<Nieruchomosc> n , Gracz* g) { return false; }
	// WAZNE!!
	// funkcje wywolujemy WTW gracz g ma odpowiednia ilosc gotowki do kupna!
	virtual bool wantBuy(std::shared_ptr<Nieruchomosc> n , Gracz* g) { return false; }
};

class Smartass : public Strategia {
public:
	// bo ma odpowiednia ilosc gotowki!
	bool wantBuy(std::shared_ptr<Nieruchomosc> n , Gracz* g) { true }
};

class Dumb : public Strategia {
public:
	Dumb() : licznik(0) {}
	bool wantBuy(std::shared_ptr<Nieruchomosc> n , Gracz* g) {
		nast();
		if( licznik == 1 )
			return true;
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
	bool wantSell(std::shared_ptr<Nieruchomosc> n) { return czlowiek->wantSell; }
	bool wantBuy(std::shared_ptr<Nieruchomosc> n) { return czlowiek->wantBuy; }
private:
	// TO DO
	// shr_ptr na czlowieka
	std::shared_ptr<Human> czlowiek;
};

// TO DO
//   void wantSell( ... )
// straszne rzeczy tu sie dzieja :<
class Gracz {
public:
	// TO DO
	// czy ten kontruktor dobry? (czy mozna tak strategie przekopiowac? )
	Gracz(std::string nazwa, unsigned int id, Strategia strategia) : nazwa(nazwa),
   							money(POCZATKOWA_KASA), id(id), ileRundPostoju(0),
							strategia(strategia) , bankrut(false) {}
	
	// zwraca hajs jaki udalo sie zaplacic. Gdy brak hajsu -> sprawdza czy gracz
	// chce sprzedac posiadlosci. Gdy po sprzedaniu nadal brak kasy ->
	// gracz bankrutuje a funkcja zwraca pozostala kase
	int plac(unsigned int cena);
	// true gdy gracz chce kupic nieruchomosc. Odejmuje od money cene
	// posiadlosci, dodaje posiadlasc do wektora 'posiadlosci'
	bool wantBuy(Nieruchomosc* n);

	void postoj(int postoj) { ileRundPostoju = postoj; }
	void giveMoney(unsigned int kasa) { money += kasa; }

	int getMoney() const { return money; }
	int getId() const { return id; }
	int getIleRundPostoju() { return ileRundPostoju; }

	int& pozycja() { return pozycja; }
	bool bankrut() { return bankrut; }
	// zwraca true gdy gracz nie ma postoju i nie jest bankrutem.
	// 		--zmniejsza o 1 ileRundPostoju!!
	bool moznaJechac() {
		if( ileRundPostoju > 0 ) {
			ileRundPostoju--;
			return false;
		}
		return bankrut;
	}

private:
	std::string nazwa;
	int money;
	unsigned int id; //pozycja w wektorze graczy
	unsigned int pozcja; // pozycja na planszy
	int ileRundPostoju;
	Strategia strategia;
	// albo vector ptr na te posiadlosci? chodzi o to zeby to byly TE same obiekty
	// ktore plansza przechowuje w sobie`
	std::vector<std::shared_ptr<Nieruchomosc> > posiadlosci;
	bool bankrut;
	// sprawdza czy gracz chce sprzedac nieruchomosci. jesli tak, to
	// dodaje kase i wywala sprzedane nieruchomosci z wektora
	void wantSell();
};
-
int Gracz::plac(unsigned int cena) {
	if( money >= cena ) {
		money -= cena;
		return cena;
	} else {
		// sprawdzamy czy chce sprzedac posiadlosci
		wantSell();

		if( money >= cena ) {
			money -= cena;
			return cena;
		} else {
			// BANKRUTUJEMY
			bankrut = true;
			return money;
		}
	}
}

bool Gracz::wantBuy(Nieruchomosc* n) {
	if( money < n->getCena() )
		wantSell();
	if(money < n->getCena() )
		return false;

	// tutaj mamy pewnosc ze gracz ma odpowiednia ilosc gotowki do kupna n!
	if( strategia.wantBuy(n, this) ) {
		// gdy strategia zwroci TRUE ==> mozemy kupic nieruchomosc (stac nas na to)
		money -= n.getCena();
		posiadlosci.push_back( std::shared_ptr (n) ); // czy to jest dobrze? TO DO
		return true;
	} else
		return false;
}

void Gracz::wantSell();
	for( auto n : posiadlosci )
		if( strategia.wantSell(n, this) ) {
			giveMoney(n->getCena() * KARA_ZA_SPRZEDAZ);
			n.sprzedana();
			// TO DO
			// wywalic nieruchomosc z wektora nieruchomosci!
			// jak porownywac pointery !?
		}
}

// Interface pola
class Pole {
	// czy tutaj tak jak w javie funkcje abstrakcyjen moga cos robic?
	// odp: tak.
public:
	// gdy gracz przechodzi przez pole
	virtual void przejdz(std::shared_ptr<Gracz> g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void stan(std::shared_ptr<Gracz> g) {}
};

class Wyspa : public Pole {
	// Wyspa nic nie robi
};

class Akwarium : public Pole {
	static const unsigned int ile_tur = 2;
	void stan(std::shared_ptr<Gracz> g) { g->postoj(ile_tur); }
};

class Depozyt : public Pole {
private:
	unsigned int gotowka;
	static const unsigned int stawka = 15;
public:
	Depozyt() : gotowka(0) {}
	void stan(std::shared_ptr<Gracz> g) {
		g->giveMoney(gotowka);
		gotowka = 0;
	}
	void przejdz(std::shared_ptr<Gracz> g) { g->plac(stawka); }
};

class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara) : kara(kara) {}
	void stan(std::shared_ptr<Gracz> g) { g->plac(kara); }
};

class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda) : nagroda(nagroda) {}
	void stan(std::shared_ptr<Gracz> g) { g->giveMoney(nagroda); }
};

class Start : public Pole {
private:
	static const unsigned int nagroda = 50;
public:
	void stan(std::shared_ptr<Gracz> g) { g->giveMoney(nagroda); }
	void przejdz(std::shared_ptr<Gracz> g) { this->stan(g); }
};

// Interface nieruchomosci
class Nieruchomosc : public Pole {
	// czy nieruchomosc tez musi miec virutal destructor ?
public:
	virtual ~Nieruchomosc() {}
	virtual void przejdz(std::shared_ptr<Gracz> g) {} //domyslnie nic
	virtual void stan(std::shared_ptr<Gracz> g) = 0;

	unsigned int getCena() const { return cena; }
	unsigned int getNazwa() const { return nazwa; }
	void sprzedana() { zajete = false; }

protected:
	bool zajete;
	unsigned int id_wlasciciela; // pozycja wlasciciela w vektorze graczy
	std::shared_ptr<Gracz> wlasciciel;

	void oferujKupno(std::shared_ptr<Gracz> g);
	bool tenSamWlasciciel(const std::shared_ptr<Gracz> g) const { return g->getId() == id_wlasciciela; }
private:
	unsigned int cena;
	unsigned int stawka;
	std::string nazwa; //potrzebne do przekazywania Humanowi czy chce kupic
};

void Nieruchomosc::oferujKupno(std::shared_ptr<Gracz> g) {
	if( g->wantBuy(this) ) {
		zajete = true;
		id_wlasciciela = g->getId();
		// czy to jest dobrze? z tym shared_ptr przypisanie?
		wlasciciel = g;
	}
}

class Ob_uz_pub : public Nieruchomosc {
public:
	Ob_uz_pub(unsigned int cena) : zajete(false), wlasciciel(0),
   								cena(cena) , stawka(cena * procent) {}
	void stan(std::shared_ptr<Gracz> g);
private:
	static const float procent = 4.0 / 10;
};

void Ob_uz_pub::stan(std::shared_ptr<Gracz> g) {
	if( !zajete )
		oferujKupno(g);
	else if( !tenSamWlasciciel(g) ) {
		wlasciciel->giveMoney( g->plac(stawka) );
	}
}

class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena) : zajete(false), wlasciciel(0),
   								cena(cena), stawka(cena * procent) {}
	void stan(std::shared_ptr<Gracz> g);
private:
	static const float procent 2.0 / 10;
};

void Koralowiec::stan(std::shared_ptr<Gracz> g) {
	if( !zajete )
		oferujKupno(g);
	else if( !tenSamWlasciciel ) {
		wlasciciel->giveMoney( g->plac(stawka) );
	}
}

class Plansza {
public:
	Plansza() { stworzPlansze_1(); }

	// Rusza graczem, wywoluje odpowiednie funkcje na polach (oferuje kupno,
	// itd).
	// ZMIENIA pozycje gracza na mapie !
	void ruszGracza(std::shared_ptr<Gracz> gracz , int oczka) {
		if( !gracz->moznaJechac() ) {
			// wypisujemy status, konczymy funkcje
			// funkcja moznaJechac zmiejsza ilosc rund postoju danego gracza!
			// (o ile takie istnieja)
			wypiszStatus(gracz , true);
			return;
		}

		int ost_pozycja gracz->pozycja();
		for( int i = 1 ; i <= oczka-1 ; ++i ) {
			ost_pozycja = (ost_pozycja++) % pola.size();
			if( !gracz->bankrut() )
				// gdy nie bankrut to robimy!
				// mogl zbankrutowac po drodze na depozycie!
				pola[ost_pozycja]->przejdz(gracz);
		}
		ost_pozycja = (ost_pozycja++) % pola.size();
		// referencja
		gracz->pozycja() = ost_pozycja;
		pola[ost_pozycja]->stan(gracz);
		
		// na koncu wypisujemy status!
		wypiszStatus(gracz , false);
	}

private:
	vector<std::shared_ptr<Pole> >pola;
	static Fabryka fabryka; // czy to na pewno tutaj?
							// jak zrobic zeby tylko jedna sie utworzyla?

	// wypisuje te dziwne rzeczy na wyjscie
	// WAZNE! gdy czyMozliwyPostoj = true to musimy NAJPEIRW sprawdzic czy gracz
	// jest bankrutem (i wypisac komunikat o bankrc=uctwie)
	// jezeli gracz != bankrut i czyMozliwyPostoj == true to
	//    gracz wlasnie stoi! (postoj nr gracz->getIleRundPostoju() + 1)`
	void wypiszStatus(std::shared_ptr<Gracz> gracz , bool czyMozliwyPostoj);
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

// TO DO
// tutaj tworzymy jednak shared_ptr!!;
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

// Uwagi:
// gracz ma miec int& pozycja(), int& gotowka(), postoj() to samo


bool MojaGrubaRyba::status( Gracz* gracz ) {
	if ( gracz->czyBankrut() ) {
		printf("%s *** bankrut ***\n", gracz->getNazwa() );
		return false;
	}

	if ( gracz->postoj() > 0 ) {
		printf( "%s pole: %s *** czekanie: %d ***\n", gracz->getNazwa(),
			plansza[gracz->getPozycja()].getNazwa(),
			gracz->postuj() );
		gracz->postoj()--;
		return false;
	}

	return true;
}

void MojaGrubaRyba::init_play() {
	if ( die = nullptr ) {
		throw NoDieException();
	}
	if ( gracze.size() < MIN_GRACZY ) {
		throw TooFewPlayersException(MIN_GRACZY);
	}
	ile_bankructw = 0;
	// resetujemy stany z poprzedniej rozgrywki
	for ( size_t i = 0; i < gracze.size(); i++ ) {
		gracze[i].reset();
	}
	for ( size_t i = 0; i < plansza.size(); i++ ) {
		plansza[i].reset();
	}
}

bool MojaGrubaRyba::czy_wygrana() {
	return ile_bankructw == gracze.size() - 1;
}

void MojaGrubaRyba::play(unsigned int rounds) {

	init_play();

	/*
	   TUTAJ IMO:
	   iterujemy po rundach
	   		iterujemy po graczach
				rzucamy koscmi, wywolujemy plansza.ruszGracza(Gracz , wyrzucone_oczka)
				ta funkcja zajmuje sie juz wszystkim!
		*/
	// iterujemy po rundach
	for ( auto r = 1; r <= rounds && !czy_wygrana(); r++ ) {
		printf("Runda: %d\n", r);
		// iterujemy po kolejnych graczach w rundzie
		for ( size_t g = 0; g < gracze.size(); g++ ) {
			Gracz* gracz = &gracze[g];

			// sprawdza czy gracz moze sie ruszyc w tej rundzie,
			// wypisuje stosowny komunikat, gdy nie moze
			if ( !status( gracz ) ) {
				continue;
			}

			// jesli mozna wylonic zwyciezce, to takowy nie wykonuje
			// juz ruchu
			if ( !czy_wygrana() ) {
				auto rzut = die.roll() + die.roll();
				auto poz = gracz.pozycja();
				// zmienna boolowa mowi, czy udalo sie przejsc / stanac na polu
				// false oznacza bankructwo w wyniku przejscia / staniecia
				bool ruch = true;
				// mijamy (rzut - 1) pol
				for ( int i = 1; i < rzut && ruch; i++ ) {
					ruch = plansza[( poz + i ) % plansza.size()].
									przejdz( gracz );
				}

				// Jesli doszedl pomyslnie do celu.
				// Wazne, bo np mogl zbankrutowac na polu Depozyt.
				if ( ruch ) {
					poz = ( poz + rzut ) % plansza.size();
					gracz.pozycja() = poz;
					// pole moze zmienic wyzej domyslnie ustawiona pozycje gracza
					// takim polem mogloby byc w przyszlosci np "Idź do akwarium"
					ruch = plansza[poz].stan( gracz );
				}

				if ( !ruch ) {
					// zbankrutowal przez ktorys z ruchow w tej turze
					gracz->czyBankrut = true;
					ile_bankructw++;
				}
			}

			// sprawdza status po wykonaniu ruchu
			if ( status( gracz ) ) {
				printf( "%s pole: %s gotowka: %d\n", gracz->getNazwa(),
						plansza[gracz->getPozycja()].getNazwa(),
						gracz->gotowka() );
			}
		}
	}
}
