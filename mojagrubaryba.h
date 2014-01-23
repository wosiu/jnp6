#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include <memory>
#include "grubaryba.h"

// MojaGrubaRyba traktujemy jako 'zarzadce' gry
class MojaGrubaRyba : public GrubaRyba {
public:
	MojaGrubaRyba();

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
	// Zmienione - Pablo
	//std::vector<Gracz> gracze;
	std::vector<std::shared_ptr<Gracz> > gracze;
	//std::vector<Pole> plansza;
	Plansza plansza;
	int ile_bankructw;

	int liczBankructwa();
	void init_play();
	bool czy_wygrana();

	/*  To do planszy wrzuce! */
	// Sprawdza status gracza w kontekscie nadchodzacego / zakonczonego ruchu.
	// Zwraca: true - moze sie ruszyc, false - czeka / zbankrutowal.
	// Wypisuje stosowne komunikaty.
	//bool status( Gracz* gracz );
};

#endif
