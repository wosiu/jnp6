#include "mojagrubaryba.h"
#include <string>

// TODO:
// add no except gdzie trza (i moze consty?)

const unsigned int POCZATKOWA_KASA = 1000;
const float KARA_ZA_SPRZEDAZ = 0.5;
const unsigned int MIN_GRACZY = 2;
const unsigned int MAX_GRACZY = 8;


// ========================= STRATEGIE GRACZA ==================================

class Strategia {
public:
	// domyslnie zwracaja false
	virtual bool wantSell(std::string nieruchomosc) {
		return false;
	}
	// funkcje wywolujemy WTW gracz g ma odpowiednia ilosc gotowki do kupna!
	virtual bool wantBuy(std::string nieruchomosc) {
		return false;
	}

	virtual void reset() {};
};


class Smartass : public Strategia {
public:
	bool wantBuy(std::string nieruchomosc ) { return true; }
};


class Dumb : public Strategia {
public:
	Dumb() {
		reset();
	}

	void reset() {
		licznik = 0;
	}

	bool wantBuy(std::string nieruchomosc  ) {
		return ( ( ++licznik ) %= MODULO ) == 1;
	}

private:
	int licznik;
	static const int MODULO = 3;
};


class Czlowiecza : public Strategia {
public:
	Czlowiecza(std::shared_ptr<Human> _czlowiek) : czlowiek(_czlowiek) {}

	bool wantSell(std::string nieruchomosc) {
		bool answer = czlowiek->wantSell( nieruchomosc );
		return answer;
	}
	bool wantBuy(std::string nieruchomosc) {
		bool answer = czlowiek->wantBuy( nieruchomosc );
		return answer;
	}

private:
	std::shared_ptr<Human> czlowiek;
};


// ================================ GRACZ ======================================

// forward declaration:
class Nieruchomosc;

class Gracz {
private:
	std::string nazwa;
	Strategia strategia;
	std::vector<Nieruchomosc*> posiadlosci;
	unsigned int _gotowka;
	unsigned int _ile_postoju;
	unsigned int _pozycja; // pozycja na planszy
	bool _bankrut;
	// sprawdza czy gracz chce sprzedac nieruchomosci. jesli tak, to
	// dodaje kase i wywala sprzedane nieruchomosci z wektora
	void wantSell();

public:
	Gracz(const std::string& nazwa, Strategia strategia) : nazwa(nazwa),
			strategia(strategia) {
		reset();
	}

	void reset() {
		_gotowka = POCZATKOWA_KASA;
		_ile_postoju = 0;
		_bankrut = false;
		strategia.reset();
	}
	// zwraca hajs jaki udalo sie zaplacic. Gdy brak hajsu -> sprawdza czy gracz
	// chce sprzedac posiadlosci. Gdy po sprzedaniu nadal brak kasy ->
	// gracz bankrutuje a funkcja zwraca pozostala kase
	unsigned int plac(unsigned int dlug);
	bool wantBuy(Nieruchomosc* n);
	void dodajGotowke(unsigned int zysk) { _gotowka += zysk; }
	unsigned int& gotowka() { return _gotowka; }
	unsigned int& postoj() { return _ile_postoju; }
	unsigned int& pozycja() { return _pozycja; }
	bool bankrut() const { return _bankrut; }
	std::string getNazwa() const { return nazwa; }
};


// ============================= POLE ==========================================

class Pole {
public:
	Pole( std::string nazwa ) : nazwa(nazwa) {}

	// gdy gracz przechodzi przez pole
	virtual void przejdz(std::shared_ptr<Gracz> g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void zostan(std::shared_ptr<Gracz> g) {}

	std::string getNazwa() { return nazwa; }

protected:
	std::string nazwa;
};


class Start : public Pole {
private:
	static const unsigned int nagroda = 50;
public:
	Start( std::string nazwa  ) : Pole( nazwa ) {}
	void zostan(std::shared_ptr<Gracz> g) { g->dodajGotowke(nagroda); }
	void przejdz(std::shared_ptr<Gracz> g) { this->zostan(g); }
};


class Akwarium : public Pole {
private:
	unsigned int ile_tur;
public:
	Akwarium( unsigned int ile_tur, std::string nazwa ) : Pole(nazwa),
			ile_tur(ile_tur) {}

	void zostan(std::shared_ptr<Gracz> g) { g->postoj() = ile_tur; }
};


class Depozyt : public Pole {
private:
	unsigned int gotowka = 0;
	unsigned int stawka;
public:
	Depozyt(unsigned int stawka, std::string nazwa) : Pole(nazwa),
			stawka(stawka) {}

	void zostan(std::shared_ptr<Gracz> g) {
		g->dodajGotowke(gotowka);
		gotowka = 0;
	}

	void przejdz(std::shared_ptr<Gracz> g) {
		gotowka += g->plac(stawka);
	}
};


class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara, std::string nazwa) : Pole(nazwa), kara(kara) {}
	void zostan(std::shared_ptr<Gracz> g) { g->plac(kara); }
};


class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda, std::string nazwa) : Pole(nazwa),
			nagroda(nagroda) {}

	void zostan(std::shared_ptr<Gracz> g) { g->dodajGotowke(nagroda); }
};

// ---------------------- nieruchomosci ----------------------------------------

class Nieruchomosc : public Pole {
public:
	Nieruchomosc( unsigned int cena, unsigned int stawka, std::string nazwa )
		: Pole(nazwa), cena(cena), stawka(stawka) {}

	std::shared_ptr<Gracz> wlasciciel = nullptr;

	virtual void przejdz(std::shared_ptr<Gracz> g) {}
	virtual void zostan(std::shared_ptr<Gracz> g);
	unsigned int getCena() const { return cena; }
// TODO private i geter to stawki
protected:
	unsigned int cena;
	unsigned int stawka;
};


class ObiektUzytPublicznej : public Nieruchomosc {
public:
	ObiektUzytPublicznej(unsigned int cena, std::string nazwa ) :
			Nieruchomosc( cena, cena * procent, nazwa ) {}
private:
	const float procent = 0.4;
};


class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena, std::string nazwa ) :
			Nieruchomosc( cena, cena * procent, nazwa ) {}
private:
	const float procent = 0.2;
};


// ======================== PLANSZA ============================================

class Plansza {
private:
	std::vector<Pole> pola;

public:
	Plansza() { reset(); }

	unsigned int size() { return pola.size(); }
	Pole& pole(unsigned int poz) { return pola[poz]; }

	void reset() {
		pola.clear();
		pola.push_back(Start("Start"));
		pola.push_back(Koralowiec(160, "Anemonia"));
		pola.push_back(Pole("Wyspa"));
		pola.push_back(Koralowiec(200, "Aporina"));
		pola.push_back(Akwarium(3, "Akwarium"));
		pola.push_back(ObiektUzytPublicznej(300, "Grota"));
		pola.push_back(Koralowiec(280, "Menella"));
		pola.push_back(Depozyt(15, "Laguna"));
		pola.push_back(ObiektUzytPublicznej(250, "Statek"));
		pola.push_back(Nagroda(120, "Blazenki"));
		pola.push_back(Koralowiec(400, "Pennatula"));
		pola.push_back(Kara(180, "Rekin"));
	}
};


// ============================ IMPLEMENTACJE ==================================

void Nieruchomosc::zostan(std::shared_ptr<Gracz> g) {
	// czy na polu stanal wlasciciel pola
	if ( wlasciciel == g ) {
		return;
	}
	// czy nieruchomosc na sprzedaz
	if ( wlasciciel == nullptr ) {
		// oferujemy graczowi zakupienie nieruchomosci
		if ( g->wantBuy( this ) ) {
		// chce i moze kupic
			wlasciciel = g;
		}
	} else {
		wlasciciel->dodajGotowke( g->plac(stawka) );
	}
}


unsigned int Gracz::plac(unsigned int cena) {
	if ( _gotowka >= cena ) {
		_gotowka -= cena;
		return cena;
	} else {
		// brak kasy - sprzedaje nieruchomosci jesli chce
		wantSell();

		if ( _gotowka >= cena ) {
			_gotowka -= cena;
			return cena;
		} else {
			// bankrutuje
			_bankrut = true;
			return _gotowka;
		}
	}
}


// wantBuy wywolywane jest w nieruchomosciach w funkcji zostan(...);
// zwraca true gdy gracz chce kupic nieruchomosc i stac go na to
bool Gracz::wantBuy(Nieruchomosc* n) {
	// sprawdzamy czy gracza na to stac
	if ( _gotowka < n->getCena() ) {
		// nie stac wiec gracz sprzedaje wybrane nieruchomosci
		wantSell();
	}
	if ( _gotowka < n->getCena() ) {
		// jesli potencjalne sprzedane nieruchomosci nadal nie wystarczaja na
		// pokrycie to prawdopodobnie jest idiota, bo niepotrzebnie sprzedal
		// nieruchomosci
		return false;
	}
	// tutaj mamy pewnosc ze gracz ma odpowiednia ilosc gotowki do kupna n
	if ( strategia.wantBuy( n->getNazwa() ) ) {
		_gotowka -= n->getCena();
		posiadlosci.push_back( n );
		return true;
	}

	return false;
}


void Gracz::wantSell() {
	for ( size_t i = 0; i < posiadlosci.size(); i++ ) {
		auto n = posiadlosci[i];
		if ( strategia.wantSell(n->getNazwa()) ) {
			_gotowka += n->getCena() * KARA_ZA_SPRZEDAZ;
			n->wlasciciel = nullptr;
			// usuwamy nieruchomosc z posiadlosci gracza O(1)
			posiadlosci[i] = posiadlosci.back();
			posiadlosci.pop_back();
		}
	}
}


void MojaGrubaRyba::addComputerPlayer(ComputerLevel level) {

	if ( gracze.size() == MAX_GRACZY ) {
		throw TooManyPlayersException( MAX_GRACZY );
	}

	std::string nazwa = "Gracz" + std::to_string( gracze.size() );

	switch ( level ) {
		case ComputerLevel::DUMB:
			//TO DO sprawdzic czy pobiernaie referencji z tak stworzonego
			// obiekty strategii sie nie wysypuje
			gracze.push_back( std::make_shared<Gracz>(nazwa, Dumb()) );
			break;
		case ComputerLevel::SMARTASS:
			gracze.push_back( std::make_shared<Gracz>(nazwa, Smartass()) );
			break;
	}
}


void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human) {
	if ( gracze.size() == MAX_GRACZY ) {
		throw TooManyPlayersException( MAX_GRACZY );
	}
	gracze.push_back(
				std::make_shared<Gracz>(human->getName(), Czlowiecza(human)) );
}


bool MojaGrubaRyba::status( std::shared_ptr<Gracz> gracz ) {
	if ( gracz->bankrut() ) {
		printf("%s *** bankrut ***\n", gracz->getNazwa().c_str() );
		return false;
	}

	if ( gracz->postoj() > 0 ) {
		printf( "%s pole: %s *** czekanie: %d ***\n", gracz->getNazwa().c_str(),
			plansza->pole( gracz->pozycja() ).getNazwa().c_str(),
			gracz->postoj() );
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
	// resetujemy stany z poprzedniej rozgrywki
	ile_bankructw = 0;
	for ( size_t i = 0; i < gracze.size(); i++ ) {
		gracze[i].reset();
	}
	plansza->reset();
}


bool MojaGrubaRyba::czy_wygrana() {
	return ( ile_bankructw == gracze.size() - 1 );
}


void MojaGrubaRyba::play(unsigned int rounds) {

	init_play();

	// iterujemy po rundach
	for ( unsigned int r = 1; r <= rounds && !czy_wygrana(); r++ ) {
		printf("Runda: %d\n", r);
		// iterujemy po kolejnych graczach w rundzie
		for ( size_t g = 0; g < gracze.size(); g++ ) {
			std::shared_ptr<Gracz> gracz = gracze[g];

			// sprawdza czy gracz moze sie ruszyc w tej rundzie,
			// wypisuje stosowny komunikat, gdy nie moze
			if ( !status( gracz ) ) {
				continue;
			}

			// jesli mozna wylonic zwyciezce, to takowy nie wykonuje
			// juz ruchu
			if ( !czy_wygrana() ) {
				auto rzut = die->roll() + die->roll();
				auto poz = gracz->pozycja();

				// iterujemy po (rzut mniej 1) polach - idziemy do celu
				for ( int i = 1; i < rzut && !gracz->bankrut(); i++ ) {
					(++poz) %= plansza->size();
					plansza->pole(poz).przejdz( gracz );
				}

				// Doszedl pomyslnie do celu lub zbankrutowal po drodze
				// (np na polu Depozyt)
				if ( !gracz->bankrut() ) {
					(++poz) %= plansza->size();
					gracz->pozycja() = poz;
					// pole moze zmienic wyzej domyslnie ustawiona pozycje gracza
					// takim polem mogloby byc w przyszlosci np "Idź do akwarium"
					// Ponizej gracz takze moze stac sie bankrutem
					plansza->pole(poz).zostan( gracz );
				}

				if ( gracz->bankrut() ) {
					ile_bankructw++;
				}
			}

			// sprawdza status po wykonaniu ruchu
			if ( status( gracz ) ) {
				printf( "%s pole: %s gotowka: %d\n", gracz->getNazwa().c_str(),
						plansza->pole( gracz->pozycja() ).getNazwa().c_str(),
						gracz->gotowka() );
			}
		}
	}
}
/**/
