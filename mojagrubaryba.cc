// TO DO
/***
   Jak rozwiazac sprawe s Graczem, Humanem i komputerem?
   czy komputer powinien miec taki sami interface jak human ALE np
   dac mu jeszcze obiekt strategie: wtedy w computerze 
     bool wantbuy() { return strategia.wantbuy() }
	czy to bedzie ssalo bo interface human niby moze sie zmienic...
	nie do konca widze jak to zrobic
*/	

typedef std::string pole_key_t;

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

class Gracz {
public:
	Gracz() {}
private:
	int hajs;
	int ile_rund_postoju;
	std::string nazwa;
	int id; //pozycja w wektorze graczy czy cos takiego
};


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
};

class Kara : public Pole {
private:
	unsigned int kara;
public:
	Kara(unsigned int kara) : kara(kara) {}
};

class Nagroda : public Pole {
private:
	unsigned int nagroda;
public:
	Nagroda(unsigned int nagroda) : nagroda(nagroda) {}
};

class Start : public Pole {
};

class Nieruchomosc : public Pole {
	// czy nieruchomosc tez musi mie virutal destructor ?
	// czy znowu trzeba powtarzac virutal void passing i action?
public:
	virtual ~Nieruchomosc() {}
	virtual void passing(Gracz& g) {}
	virtual void action(Gracz& g) = 0; // tutaj kazdy ma swoja akcje!
protected:
	bool zajete;
	unsigned int wlasciciel; // jakies id wlasciciela
	unsigned int cena;
};

// TO DO
// Ob_uz_pub i Koralowiec maja takie same konstruktory, czy daloby sie tak jak
// w javie napisac konstruktor w Nieruchomosc i wywolywac ten konstruktor w
// klasach Koralowiec i Ob_uz_oub?
//
// Jesli nie to jakas protected funkcje do nieruchomosci dac (Init(int cena) czy cos)
class Ob_uz_pub : public Nieruchomosc {
public:
	Ob_uz_pub(unsigned int cena) : zajete(false), wlasciciel(0), cena(cena) {}
	// TO DO do zaimplementowania!
	void passing(Gracz& g);
	void action(Gracz& g);
};

class Koralowiec : public Nieruchomosc {
public:
	Koralowiec(unsigned int cena) : zajete(false), wlasciciel(0), cena(cena) {}
	// TO DO do zaimplementowania!
	void passing(Gracz& g);
	void action(Gracz& g);
};

class Plansza {
public:
	Plansza();
private:
	vector<Pole> pola;
	static Fabryka fabryka; // czy to na pewno tutaj?
							// jak zrobic zeby tylko jedna sie utworzyla?
};

Plansza::Plansza() {
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
