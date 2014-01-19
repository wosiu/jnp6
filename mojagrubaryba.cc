// gdzie tego typedefa? czy nei dałoby sie go wsadzic gdzies do klasy?
typedef std::string pole_key_t;

// tutaj pewnie trzeba bedzie jeszcze dodac: np
// KORALOWIEC_TYP_A , KORALOWIEC_TYP_B itd ...
const pole_key_t AKWARIUM = "akwarium";
const pole_key_t DEPOZYT = "depozyt";
const pole_key_t KARA = "kara";
const pole_key_t NAGRODA = "nagroda";
const pole_key_t START = "start";
const pole_key_t OB_UZ_PUB = "obiekt";
const pole_key_t KORALOWIEC = "koralowiec";

// hierarchia Pola
class Pole;
	class Akwarium;
	class Depozyt;
	class Kara;
	class Nagroda;
	class Start;
	class Nieruchomosc;
		class Ob_uz_pub; // obiekt uzytecznosci publicznej
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
};
class Nagroda : public Pole {
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
	if( k.compare(KARA) == 0 )
		return new Kara();
	if( k.compare(NAGRODA) == 0 )
		return new Nagroda();
	if( k.compare(START) == 0 )
		return new Start();
	if( k.compare(OB_UZ_PUB) == 0 )
		return new Ob_uz_pub();
	if( k.compare(KORALOWIEC) == 0 )
		return new Koralowiec();
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
