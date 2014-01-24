/*
* JNP 6
* Pawel Krawczyk 334683
* Michal Wos 336071
*/

#include "mojagrubaryba.h"
#include <string>
#include <map>

using namespace std;

const unsigned int POCZATKOWA_KASA = 1000;
const float KARA_ZA_SPRZEDAZ = 0.5;
const unsigned int MIN_GRACZY = 2;
const unsigned int MAX_GRACZY = 8;


// ========================= STRATEGIE GRACZA ==================================

class Strategia {
public:
	// domyslnie zwracaja false
	virtual bool wantSell(string nieruchomosc) { return false; }
	// funkcje wywolujemy WTW gracz g ma odpowiednia ilosc gotowki do kupna!
	virtual bool wantBuy(string nieruchomosc) { return false; }
	virtual void resetuj() {};
};


class Smartass : public Strategia {
public:
	bool wantBuy(string nieruchomosc) { return true; }
};


class Dumb : public Strategia {
public:
	Dumb() { resetuj(); }
	void resetuj() { licznik = 0; }
	bool wantBuy(string nieruchomosc ) {
		return ((++licznik) %= MODULO) == 0;
	}

private:
	int licznik;
	static const int MODULO = 3;
};


class Czlowiecza : public Strategia {
public:
	Czlowiecza(shared_ptr<Human> _czlowiek) : czlowiek(_czlowiek) {}

	bool wantSell(string nieruchomosc) {
		bool answer = czlowiek->wantSell(nieruchomosc);
		return answer;
	}
	bool wantBuy(string nieruchomosc) {
		bool answer = czlowiek->wantBuy(nieruchomosc);
		return answer;
	}

private:
	shared_ptr<Human> czlowiek;
};


// ================================ GRACZ ======================================

// forward declaration:
class Nieruchomosc;

class Gracz {
private:
	string nazwa;
	shared_ptr<Strategia> strategia;
	int licznik_nier = 0;
	map<int, Nieruchomosc*> posiadlosci;
	unsigned int _gotowka;
	unsigned int _ile_postoju;
	unsigned int _pozycja; // pozycja na planszy
	bool _bankrut;
	// sprawdza czy gracz chce sprzedac nieruchomosci. jesli tak, to
	// dodaje kase i wywala sprzedane nieruchomosci z wektora
	void wantSell();

public:
	Gracz(const string& nazwa, shared_ptr<Strategia> strategia) :
	   		nazwa(nazwa), strategia(strategia) {
		resetuj();
	}

	void resetuj() {
		strategia->resetuj();
		licznik_nier = 0;
		posiadlosci.clear();
		_gotowka = POCZATKOWA_KASA;
		_pozycja = 0;
		_ile_postoju = 0;
		_bankrut = false;
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
	const string& getNazwa() const { return nazwa; }
};


// ============================= POLE ==========================================

class Pole {
public:
	Pole(string nazwa) : nazwa(nazwa) {}

	// gdy gracz przechodzi przez pole
	virtual void przejdz(shared_ptr<Gracz> g) {}
	// gdy gracz zatrzymuje sie na polu
	virtual void zostan(shared_ptr<Gracz> g) {}

	string getNazwa() { return nazwa; }
private:
	string nazwa;
};


class Start : public Pole {
private:
	static const unsigned int nagroda = 50;
public:
	Start(string nazwa ) : Pole(nazwa) {}
	virtual void zostan(shared_ptr<Gracz> g) { g->dodajGotowke(nagroda); }
	void przejdz(shared_ptr<Gracz> g) { this->zostan(g); }
};


class Akwarium : public Pole {
private:
	unsigned int ile_tur;
public:
	Akwarium(unsigned int ile_tur, string nazwa) : Pole(nazwa),
			ile_tur(ile_tur) {}

	void zostan(shared_ptr<Gracz> g) { g->postoj() = ile_tur; }
};


class Depozyt : public Pole {
private:
	unsigned int gotowka = 0;
	unsigned int stawka;
public:
	Depozyt(unsigned int stawka, string nazwa) : Pole(nazwa),
			stawka(stawka) {}

	void zostan(shared_ptr<Gracz> g) {
		g->dodajGotowke(gotowka);
		gotowka = 0;
	}

	void przejdz(shared_ptr<Gracz> g) {
		gotowka += g->plac(stawka);
	}
};


class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara, string nazwa) : Pole(nazwa), kara(kara) {}
	void zostan(shared_ptr<Gracz> g) { g->plac(kara); }
};


class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda, string nazwa) : Pole(nazwa),
			nagroda(nagroda) {}

	void zostan(shared_ptr<Gracz> g) { g->dodajGotowke(nagroda); }
};

// ---------------------- nieruchomosci ----------------------------------------

class Nieruchomosc : public Pole {
public:
	Nieruchomosc(unsigned int cena, unsigned int stawka, string nazwa)
		: Pole(nazwa), cena(cena), stawka(stawka) {}

	shared_ptr<Gracz> wlasciciel = nullptr;

	virtual void przejdz(shared_ptr<Gracz> g) {}
	virtual void zostan(shared_ptr<Gracz> g);
	unsigned int getCena() const { return cena; }
	unsigned int getStawka() const { return stawka; }

private:
	unsigned int cena;
	unsigned int stawka;
};


class ObiektUzytPublicznej : public Nieruchomosc {
public:
	ObiektUzytPublicznej(unsigned int cena, string nazwa) :
			Nieruchomosc(cena, cena * 0.4, nazwa) {}
};


class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena, string nazwa) :
			Nieruchomosc(cena, cena * 0.2, nazwa) {}
};


// ======================== PLANSZA ============================================

class Plansza {
private:
	vector<shared_ptr<Pole> > pola;

public:
	Plansza() { resetuj(); }

	unsigned int size() const { return pola.size(); }
	Pole& pole(unsigned int poz) { return *pola[poz]; }

	void resetuj() {
		pola.clear();

		pola.push_back(make_shared<Start>("Start"));
		pola.push_back(make_shared<Koralowiec>(160, "Anemonia"));
		pola.push_back(make_shared<Pole>("Wyspa"));
		pola.push_back(make_shared<Koralowiec>(220, "Aporina"));
		pola.push_back(make_shared<Akwarium>(3, "Akwarium"));
		pola.push_back(make_shared<ObiektUzytPublicznej>(300, "Grota"));
		pola.push_back(make_shared<Koralowiec>(280, "Menella"));
		pola.push_back(make_shared<Depozyt>(15, "Laguna"));
		pola.push_back(make_shared<ObiektUzytPublicznej>(250, "Statek"));
		pola.push_back(make_shared<Nagroda>(120, "Blazenki"));
		pola.push_back(make_shared<Koralowiec>(400, "Pennatula"));
		pola.push_back(make_shared<Kara>(180, "Rekin"));
	}
};


// ============================ IMPLEMENTACJE ==================================

void Nieruchomosc::zostan(shared_ptr<Gracz> g) {
	// czy na polu stanal wlasciciel pola
	if (wlasciciel == g) { return; }
	// czy nieruchomosc na sprzedaz
	if (wlasciciel == nullptr) {
		// oferujemy graczowi zakupienie nieruchomosci
		if (g->wantBuy(this)) {
		// chce i moze kupic
			wlasciciel = g;
		}
	} else {
		wlasciciel->dodajGotowke(g->plac(stawka));
	}
}


unsigned int Gracz::plac(unsigned int cena) {
	if (_gotowka >= cena) {
		_gotowka -= cena;
		return cena;
	} else {
		// brak kasy - sprzedaje nieruchomosci jesli chce
		wantSell();

		if (_gotowka >= cena) {
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
	if (_gotowka < n->getCena()) {
		// nie stac wiec gracz sprzedaje wybrane nieruchomosci
		wantSell();
	}
	if (_gotowka < n->getCena()) {
		// jesli potencjalne sprzedane nieruchomosci nadal nie wystarczaja na
		// pokrycie to prawdopodobnie jest idiota, bo niepotrzebnie sprzedal
		// nieruchomosci
		return false;
	}
	// tutaj mamy pewnosc ze gracz ma odpowiednia ilosc gotowki do kupna n
	if (strategia->wantBuy(n->getNazwa())) {
		_gotowka -= n->getCena();
		posiadlosci[licznik_nier++] = n;
		return true;
	}

	return false;
}


void Gracz::wantSell() {

	for (auto it = posiadlosci.begin(); it != posiadlosci.end(); it++) {
		auto n = it->second;
		if (strategia->wantSell(n->getNazwa())) {
			_gotowka += n->getCena() * KARA_ZA_SPRZEDAZ;
			n->wlasciciel = nullptr;
			// usuwamy nieruchomosc z posiadlosci gracza O(1)
			posiadlosci.erase( it );
		}
	}
}

MojaGrubaRyba::MojaGrubaRyba() {
	plansza = make_shared<Plansza>();
}


void MojaGrubaRyba::addComputerPlayer(ComputerLevel level) {

	if (gracze.size() == MAX_GRACZY) {
		throw TooManyPlayersException(MAX_GRACZY);
	}

	string nazwa = "Gracz" + to_string(gracze.size() + 1);

	switch (level) {
		case ComputerLevel::DUMB:
			//TO DO sprawdzic czy pobiernaie referencji z tak stworzonego
			// obiekty strategii sie nie wysypuje
			gracze.push_back(make_shared<Gracz>(nazwa,
					   	make_shared<Dumb>()));
			break;
		case ComputerLevel::SMARTASS:
			gracze.push_back(make_shared<Gracz>(nazwa,
					   	make_shared<Smartass>()));
			break;
	}
}


void MojaGrubaRyba::addHumanPlayer(shared_ptr<Human> human) {
	if (gracze.size() == MAX_GRACZY) {
		throw TooManyPlayersException(MAX_GRACZY);
	}
	gracze.push_back(
				make_shared<Gracz>(human->getName(),
				   	make_shared<Czlowiecza>(human)));
}


bool MojaGrubaRyba::status(shared_ptr<Gracz> gracz) {
	if (gracz->bankrut()) {
		printf("%s *** bankrut ***\n", gracz->getNazwa().c_str());
		return false;
	}

	if (gracz->postoj() > 0) {
		printf("%s pole: %s *** czekanie: %d ***\n", gracz->getNazwa().c_str(),
			plansza->pole(gracz->pozycja()).getNazwa().c_str(),
			gracz->postoj());
		return false;
	}

	printf("%s pole: %s gotowka: %d\n", gracz->getNazwa().c_str(),
		plansza->pole(gracz->pozycja()).getNazwa().c_str(),
		gracz->gotowka());

	return true;
}


void MojaGrubaRyba::init_play() {
	if (die == nullptr) {
		throw NoDieException();
	}
	if (gracze.size() < MIN_GRACZY) {
		throw TooFewPlayersException(MIN_GRACZY);
	}
	// resetujemy stany z poprzedniej rozgrywki
	ile_bankructw = 0;
	for (size_t i = 0; i < gracze.size(); i++) {
		gracze[i]->resetuj();
	}
	plansza->resetuj();
}


bool MojaGrubaRyba::czy_wygrana() {
	return (ile_bankructw == gracze.size() - 1);
}


void MojaGrubaRyba::play(unsigned int rounds) {
	init_play();

	// iterujemy po rundach
	for (unsigned int r = 1; r <= rounds && !czy_wygrana(); r++) {
		printf("Runda: %d\n", r);
		// iterujemy po kolejnych graczach w rundzie
		for (size_t g = 0; g < gracze.size(); g++) {
			shared_ptr<Gracz> gracz = gracze[g];

			// sprawdza czy gracz moze sie ruszyc w tej rundzie,
			if (gracz->bankrut()) {
				continue;
			}

			if (czy_wygrana()) {
				continue;
			}

			if (gracz->postoj() > 0 ) {
				gracz->postoj()--;
				if (gracz->postoj() > 0) {
				   	continue;
				}
			}

			// jesli mozna wylonic zwyciezce, to takowy nie wykonuje
			// juz ruchu
			if (!czy_wygrana()) {
				auto rzut = die->roll() + die->roll();
				auto poz = gracz->pozycja();

				// iterujemy po (rzut mniej 1) polach - idziemy do celu
				for (int i = 1; i < rzut && !gracz->bankrut(); i++) {
					(++poz) %= plansza->size();
					plansza->pole(poz).przejdz(gracz);
				}

				// Doszedl pomyslnie do celu lub zbankrutowal po drodze
				// (np na polu Depozyt)
				if (!gracz->bankrut()) {
					(++poz) %= plansza->size();
					gracz->pozycja() = poz;
					// pole moze zmienic wyzej ustawiona pozycje gracza, takim
					// polem mogloby byc w przyszlosci np "Idź do akwarium"
					// Ponizej gracz takze moze stac sie bankrutem
					plansza->pole(poz).zostan(gracz);
				}

				if (gracz->bankrut()) {
					ile_bankructw++;
				}
			}
		}

		// po zakonczonej rundzie wypisujemy status graczy
		for (size_t g = 0; g < gracze.size(); g++) {
			status(gracze[g]);
		}
	}
}
