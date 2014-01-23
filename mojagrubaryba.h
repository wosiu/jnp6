#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <memory>
#include <vector>
#include "grubaryba.h"

class Gracz;
class Plansza;

// MojaGrubaRyba traktujemy jako 'zarzadce' gry
class MojaGrubaRyba : public GrubaRyba {
public:
	MojaGrubaRyba() {}

    // Przekazuje prototypową kostkę do gry.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    void setDie(std::shared_ptr<Die> die);

    // Dodaje nowego gracza komputerowego, który gra na zdefiniowanym poziomie.
    // Nowy gracz komputerowy nazywa się Gracz<Numer>, gdzie <Numer> to numer pozycji,
    // na której został dodany (numeracja pozycji od 1).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    void addComputerPlayer(ComputerLevel level);

    // Dodaje nowego gracza czlowieka.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    void addHumanPlayer(std::shared_ptr<Human> human);

    // Przeprowadza rozgrywkę co najwyżej podanej liczby rund (rozgrywka może
    // skończyć się wcześniej).
    // Jedna runda obejmuje po jednym ruchu każdego gracza.
    // Gracze ruszają się w kolejności, w której zostali dodani.
    // Na początku każdej rundy wypisywany jest numer rundy,
    // a na zakończenie informacje podsumowujące dla każdego gracza (format w przykładzie).
    // Rzuca NoDieException, jeśli nie przekazano kostki.
    // Rzuca TooFewPlayersException, jeśli liczba graczy nie pozwala na rozpoczęcie gry.
    void play(unsigned int rounds);

private:
	std::shared_ptr<Die> die = nullptr;
	std::vector<Gracz> gracze;
	Plansza plansza;
	int ile_bankructw;

	void init_play();
	bool czy_wygrana();
	// Sprawdza status gracza w kontekscie nadchodzacego / zakonczonego ruchu.
	// Zwraca: true - moze sie ruszyc, false - czeka / zbankrutowal.
	// Wypisuje stosowne komunikaty.
	bool status( Gracz* gracz );
};

#endif
