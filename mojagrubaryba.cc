#include "mojagrubaryba.h"

// TODO:
// add no except gdzie trza (i moze consty?)

typedef std::string pole_key_t;

const unsigned int POCZATKOWA_KASA = 1000;
const float KARA_ZA_SPRZEDAZ = 0.5;


// ========================= STRATEGIE GRACZA ==================================

class Strategia {
public:
	// domyslnie zwracaja false
	virtual bool wantSell(Nieruchomosc* n, Gracz* g = null) {
		return false;
	}
	// funkcje wywolujemy WTW gracz g ma odpowiednia ilosc gotowki do kupna!
	virtual bool wantBuy(Nieruchomosc* n, Gracz* g = null) {
		return false;
	}

	virtual void reset() {};
};

class Smartass : public Strategia {
public:
	bool wantBuy(Nieruchomosc* n, Gracz* g = null ) { return true }
};

class Dumb : public Strategia {
public:
	Dumb() {
		reset();
	}

	void reset() {
		licznik = 0;
	}

	bool wantBuy(Nieruchomosc* n , Gracz* g = null ) {
		(++licznik) %= MODULO
		return ( licznik == 1 )
	}

private:
	int licznik;
	static const int MODULO = 3;
};


class Czlowiecza : public Strategia {
public:
	Czlowiecza(std::shared_ptr<Human> _czlowiek) : czlowiek(_czlowiek);

	bool wantSell(Nieruchomosc* n , Gracz* g = null) {
		return czlowiek->wantSell(n->getNazwa());
	}
	bool wantBuy(Nieruchomosc* n , Gracz* g = null) {
		return czlowiek->wantBuy(n->getNazwa());
	}

private:
	std::shared_ptr<Human> czlowiek;
};

// ================================ GRACZ ======================================

class Gracz {
private:
	std::string nazwa;
	int _gotowka;
	int _ile_postoju;
	unsigned int _pozcja; // pozycja na planszy
	bool _bankrut;
	Strategia strategia;
	std::vector<Nieruchomosc*> posiadlosci;
	// sprawdza czy gracz chce sprzedac nieruchomosci. jesli tak, to
	// dodaje kase i wywala sprzedane nieruchomosci z wektora
	void wantSell();

public:
	Gracz(std::string nazwa, Strategia strategia) : nazwa(nazwa),
			strategia(strategia) {
		reset();
	}

	void reset() {
		gotowka = POCZATKOWA_KASA;
		ile_postoju = 0;
		bankrut = false;
		strategia.reset();
	}
	// zwraca hajs jaki udalo sie zaplacic. Gdy brak hajsu -> sprawdza czy gracz
	// chce sprzedac posiadlosci. Gdy po sprzedaniu nadal brak kasy ->
	// gracz bankrutuje a funkcja zwraca pozostala kase
	int plac(unsigned int dlug);
	bool wantBuy(Nieruchomosc* n);
	void dodajGotowki(unsigned int zysk) { _gotowka += zysk; }
	int& gotowka() { return _gotowka; }
	int& postoj() { return _ile_postoju; }
	int& pozycja() { return _pozycja; }
	bool bankrut() const { return _bankrut; }
};

int Gracz::plac(unsigned int cena) {
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
			bankrut = true;
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
	if ( strategia.wantBuy( n, this ) ) {
		_gotowka -= n->getCena();
		posiadlosci.push_back( n );
		return true;
	}

	return false;
}

void Gracz::wantSell();
	for( size_t i = 0; i < posiadlosci.size(); i++ )
		auto n = posiadlosci[i];
		if( strategia.wantSell(n, this) ) {
			_gotowka += n->getCena() * KARA_ZA_SPRZEDAZ;
			n -> wlasciciel = null;
			// usuwamy nieruchomosc z posiadlosci gracza O(1)
			posiadlosci[i] = posiadlosci.back();
			posiadlosci.pop_back();
		}
}


// ============================= POLE ==========================================

class Pole {
public:
	Pole( std::string nazwa ) : nazwa(nazwa);

	// gdy gracz przechodzi przez pole
	virtual void przejdz(Gracz* g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void zostan(Gracz* g) {}

	std::string getNazwa() { return nazwa; }

protected:
	std::string nazwa;
};


class Start : public Pole {
private:
	static const unsigned int nagroda = 50;
public:
	Start( std::string nazwa  ) : nazwa(nazwa) {}
	void zostan(Gracz* g) { g->dodajGotowke(nagroda); }
	void przejdz(Gracz* g) { this->zostan(g); }
};


class Akwarium : public Pole {
private:
	unsigned int ile_tur;
public:
	Akwarium( unsigned int ile_tur, std::string nazwa ) : ile_tur(ile_tur),
			ilenazwa(nazwa) {}

	void zostan(Gracz* g) { g->postoj(ile_tur); }
};

class Depozyt : public Pole {
private:
	unsigned int gotowka = 0;
	unsigned int stawka;
public:
	Depozyt(unsigned int stawka, std::string nazwa) : stawa(stawka),
			nazwa(nazwa) {}

	void zostan(Gracz* g) {
		g->dodajGotowke(gotowka);
		gotowka = 0;
	}

	void przejdz(Gracz* g) {
		gotowka += g->plac(stawka);
	}
};

class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara, std::string nazwa) : kara(kara), nazwa(nazwa) {}
	void zostan(Gracz* g) { g->plac(kara); }
};

class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda, std::string nazwa) : nagroda(nagroda),
			nazwa(nazwa) {}

	void zostan(Gracz* g) { g->dodajGotowke(nagroda); }
};

// ---------------------- Interfejs nieruchomosci ------------------------------

class Nieruchomosc : public Pole {
public:
	Nieruchomosc( unsigned int cena, unsigned int stawka, std::string nazwa )
		: cena(cena), stawka(stawka), nazwa(nazwa) {}

	virtual void przejdz(Gracz* g) {}

	virtual void zostan(Gracz* g) {
		// czy na polu stanal wlasciciel pola
		if ( wlasciciel == g ) {
			return
		}
		// czy nieruchomosc na sprzedaz
		if ( wlasciel == null ) {
			// oferujemy graczowi zakupienie nieruchomosci
			if ( g->wantBuy( this ) ) {
				// chce i moze kupic
				wlasciciel = g;
			}
		} else {
			wlasciciel->dodajGotowke( g->plac(stawka) );
		}
	}

	unsigned int getCena() const { return cena; }

protected:
	Gracz* wlasciciel = null;
	unsigned int cena;
	unsigned int stawka;
};


class ObiektUzytPublicznej : public Nieruchomosc {
public:
	ObiektUzytPublicznej(unsigned int cena, std::string nazwa ) :
		cena(cena) , stawka(cena * procent), nazwa(nazwa) {}
private:
	static const float procent = 0.4;
};

class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena, std::string nazwa ) : cena(cena),
			stawka(cena * procent), nazwa(nazwa) {}
private:
	static const float procent = 0.2;
};

// ======================== PLANSZA ============================================

class Plansza {

public:
	Plansza() {
		reset();
	}

	void reset() {
		pola.clear();
		pola.push_back(Start("Start"));
		pola.push_back(Koralowic(160, "Anemonia"));
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

	private:
		vector<Pole> > pola;
};


// ============================ MojaGrubaRyba ==================================
MojaGrubaRyba::MojaGrubaRyba() {

}

void MojaGrubaRyba::setDie(std::shared_ptr<Die> die) {

}

void MojaGrubaRyba::addComputerPlayer(ComputerLevel level) {

}

void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human) {

}

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
	// resetujemy stany z poprzedniej rozgrywki
	ile_bankructw = 0;
	for ( size_t i = 0; i < gracze.size(); i++ ) {
		gracze[i].reset();
	}
	plansza.reset();
}


bool MojaGrubaRyba::czy_wygrana() {
	return ile_bankructw == gracze.size() - 1;
}

void MojaGrubaRyba::play(unsigned int rounds) {

	init_play();

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
				for ( int i = 1; i < rzut && !gracz->czyBankrut(); i++ ) {
					(++poz) %= plansza.size();
					plansza[ poz ].przejdz( gracz );
				}

				// Doszedl pomyslnie do celu lub zbankrutowal po drodze
				// (np na polu Depozyt)
				if ( !gracz->czyBankrut() ) {
					(++poz) %= plansza.size();
					gracz.pozycja() = poz;
					// pole moze zmienic wyzej domyslnie ustawiona pozycje gracza
					// takim polem mogloby byc w przyszlosci np "Idź do akwarium"
					// Ponizej gracz takze moze stac sie bankrutem
					plansza[poz].zostan( gracz );
				}

				if ( gracz->czyBankrut() ) {
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
