#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <stack>
#include <vector>




struct wspolrzedne {
	int x;
	int y;
};

struct statystyki {
	int zycie;
	int punkty;
	int moc;
};

//sprawdza czy liczba jest juz w tablicy tab, ile - dlugosc
bool czyBylaWylosowana(int wylosowana, int tab[], int ile) {
	if (ile <= 0) {
		return false;
	}
	int i = 0;
	do {
		if (tab[i] == wylosowana) {
			return true;
		}
		i++;
	} while (i < ile);

	return false;


}

//klasa dziedziczona po sf sprite, aby dodac zmienn¹ uzytkow¹
class mySprite : public sf::Sprite {
private:
	bool czyWyswietlic = true;
public:
	void nieWyswietlaj();
	void wyswietlaj();
	bool czyWyswietlac();
};

void mySprite::nieWyswietlaj() {
	this->czyWyswietlic = false;
}

void mySprite::wyswietlaj() {
	this->czyWyswietlic = true;
}

bool mySprite::czyWyswietlac() {
	return this->czyWyswietlic;
}


//klasa planszy
class Plansza {
private:
	int rozmiar;
	bool** pola;
	bool*** pola_krawedzie;
	sf::RectangleShape*** krawedzie;//[y][x][góra/prawo/dó³/lewo]
	sf::Vector2f rozmiarokna;
public:
	Plansza(int jakduza,sf::Vector2f _rozmiarokna);
	~Plansza();
	void losuj_labirynt();
	void rysuj_labirynt(sf::RenderWindow* wind);
	void setZapis(bool*** polaKrawedzie);
	int getrozmiar();
	bool*** getPolaKrawedzie();
	
};

Plansza::Plansza(int jakduza,sf::Vector2f _rozmiarokna) {
	this->rozmiar = jakduza;
	
	
	//utworzenie i zainicjalizowanie tablic
	bool** temp = new bool* [jakduza];

	for (int i = 0; i < jakduza; i++) {
		temp[i] = new bool[jakduza];
	}
	this->pola = temp;

	bool*** t = new bool** [jakduza];

	for (int i = 0; i < jakduza; i++) {
		t[i] = new bool* [jakduza];
		for (int j = 0; j < jakduza; j++) {
			t[i][j] = new bool[4];
		}
	}
	
	this->pola_krawedzie = t;

	sf::RectangleShape*** kra = new sf::RectangleShape * *[jakduza];

	for (int i = 0; i < jakduza; i++) {
		kra[i] = new sf::RectangleShape* [jakduza];
		for (int j = 0; j < jakduza; j++) {
			kra[i][j] = new sf::RectangleShape [4] ;
		}
	}
	this->krawedzie = kra;
	

	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			this->pola[i][j] = false;
			for (int k = 0; k < 4; k++) {
				this->pola_krawedzie[i][j][k] = true;
				this->krawedzie[i][j][k].setSize(sf::Vector2f(25.f, 2.f));;
			}
		}
	}
	
	

}

void Plansza::losuj_labirynt() {
	//zmienne u³atwiaj¹ce
	int gora = -1, lewo = -1;
	int dol = 1, prawo = 1;

	//OGRANICZENIE ROZMIARU PLANSZY
	if (this->rozmiar > 20) {
		this->rozmiar = 20;
	}

	//PRZYPISANIE POCZATKOWE KRAWEDZI
	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			for (int k = 0; k < 4; k++) {
				this->krawedzie[i][j][k].setSize(sf::Vector2f(25.f, 2.f));
				this->krawedzie[i][j][k].setPosition(sf::Vector2f(250.f, 250.f));
				this->pola_krawedzie[i][j][k] = true;

			}
		}
	}

	//OBROCENIE PIONOWYCH KRAWEDZI
	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			for (int k = 1; k < 4; k += 2) {
				krawedzie[i][j][k].rotate(90.f);

			}
		}
	}
	//srodkowe polozenia labiryntu - skalowanie
	float msc_srodkowe_lab_y = 425 - 0.5 * rozmiar * 25;
	float msc_srodkowe_lab_x = 700 - 0.5 * rozmiar * 25;

	//ULOZENIE KRATEK
	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			krawedzie[i][j][0].setPosition(sf::Vector2f((msc_srodkowe_lab_x + j * 25), (msc_srodkowe_lab_y + i * 25)));
			krawedzie[i][j][1].setPosition(sf::Vector2f((msc_srodkowe_lab_x + 25 + j * 25), (msc_srodkowe_lab_y + i * 25)));
			krawedzie[i][j][2].setPosition(sf::Vector2f((msc_srodkowe_lab_x + j * 25), (msc_srodkowe_lab_y + 25 + i * 25)));
			krawedzie[i][j][3].setPosition(sf::Vector2f((msc_srodkowe_lab_x + j * 25), (msc_srodkowe_lab_y + i * 25)));
		}
	}
	
	

	//ALGORYTM DEFINICJE POMOCNICZYCH ZMIENNYCH
	wspolrzedne aktualny = { 0 };
	aktualny.x = 0;
	aktualny.y = 0;
	std::stack<wspolrzedne> stos;
	std::vector<int> sasiadujace;
	int ilosc_odwiedzonych = 0;
	int nastepne_pole = 0;

	//OKRESLENIE POCZATKOWEGO STANU KRATEK
	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			pola[i][j] = false;
		}
	}

	//INICJALIZACJA - ROZPOCZECIE
	stos.push(aktualny);
	ilosc_odwiedzonych = 1;
	pola[0][0] = true;

	//DZIALANIE ALGORYTMU
	while (ilosc_odwiedzonych < this->rozmiar * this->rozmiar/*³¹czna liczba kratek*/) {
		aktualny.x = stos.top().x;
		aktualny.y = stos.top().y; 

		//GORA
		if (stos.top().y > 0) {
			if (this->pola[stos.top().y - 1][stos.top().x] == false) {
				sasiadujace.push_back(0);
			}
		}
		//PRAWO
		if (stos.top().x < this->rozmiar-1) {
			if (this->pola[stos.top().y][stos.top().x + 1] == false) {
				sasiadujace.push_back(1);
			}
		}
		//DOL
		if (stos.top().y < this->rozmiar-1) {
			if (this->pola[stos.top().y + 1][stos.top().x] == false) {
				sasiadujace.push_back(2);
			}
		}
		//LEWO
		if (stos.top().x > 0) {
			if (this->pola[stos.top().y][stos.top().x - 1] == false) {
				sasiadujace.push_back(3);
			}
		}

		//JEZELI JEST POLE OBOK WCZESNIEJ NIEODWIEDZONE
		if (!sasiadujace.empty()) {
			nastepne_pole = sasiadujace[rand() % sasiadujace.size()];
			//std::cout << "  akt y - " << aktualny.y << "  akt x - " << aktualny.x << " nast pole " << nastepne_pole << " ilosc odwiedzonych - " << ilosc_odwiedzonych << "\n";

			//W ZALEZNOSCI OD KIERUNKU NASTEPNEGO POLA
			switch (nastepne_pole) {
			case 0:
				aktualny.y -= 1;
				//std::cout << aktualny.y;
				this->pola[aktualny.y][aktualny.x] = true;
				this->pola_krawedzie[aktualny.y][aktualny.x][2] = false;
				this->pola_krawedzie[aktualny.y + 1][aktualny.x][0] = false;
				stos.push(aktualny);
				break;
			case 1:
				aktualny.x += prawo;
				//std::cout << aktualny.x;
				this->pola[aktualny.y][aktualny.x] = true;
				this->pola_krawedzie[aktualny.y][aktualny.x][3] = false;
				this->pola_krawedzie[aktualny.y][aktualny.x - 1][1] = false;
				stos.push(aktualny);
				break;
			case 2:
				aktualny.y += dol;
				//std::cout << aktualny.y;
				this->pola[aktualny.y][aktualny.x] = true;
				this->pola_krawedzie[aktualny.y][aktualny.x][0] = false;
				this->pola_krawedzie[aktualny.y - 1][aktualny.x][2] = false;
				stos.push(aktualny);
				break;
			case 3:
				aktualny.x += lewo;
				//std::cout << aktualny.x;
				this->pola[aktualny.y][aktualny.x] = true;
				this->pola_krawedzie[aktualny.y][aktualny.x][1] = false;
				this->pola_krawedzie[aktualny.y][aktualny.x + 1][3] = false;
				stos.push(aktualny);
				break;
			}

			ilosc_odwiedzonych ++;
			sasiadujace.clear();
		}
		else {
			stos.pop();
		}


	}

}

void Plansza::rysuj_labirynt(sf::RenderWindow* wind) {
	//rysowanie krawêdzi jezeli maj¹ byæ narysowane (jezeli true)
	for (int i = 0; i < this->rozmiar; i++) {
		for (int j = 0; j < this->rozmiar; j++) {
			for (int k = 0; k < 4; k++) {
				if (this->pola_krawedzie[i][j][k] == true) {
					wind->draw(this->krawedzie[i][j][k]);
				}
			}
		}
	}

}

void Plansza::setZapis(bool***polaKrawedzie) {
	this->pola_krawedzie = polaKrawedzie;
}

bool*** Plansza::getPolaKrawedzie() {
	return this->pola_krawedzie;
}

int Plansza::getrozmiar() {
	return this->rozmiar;
}

Plansza::~Plansza() {
	delete[] pola;
	delete[] pola_krawedzie;
	delete[] krawedzie;
}


//klasa interfejsu/wyœwietlanie otoczenia w grze
class Interfejs {
private:
	sf::RectangleShape* okno_glowne;
	sf::RectangleShape* okno_labiryntu;
	sf::Text* tytul;
	sf::Text* punkty_txt;
	sf::Text* moc_txt;
	sf::Text* zycia_txt;
	sf::Vector2f rozmiarokna;
	sf::Font* czcionka;
	int rozmiarlabiryntu;
public:
	Interfejs(int _rozmlab, sf::Vector2f _rozmiarokna);
	~Interfejs();
	void init();
	void in_staty(statystyki* staty);
	void rysuj(sf::RenderWindow* wind);
};

Interfejs::Interfejs(int _rozmlab, sf::Vector2f _rozmiarokna) {
	this->rozmiarokna = _rozmiarokna;
	this->okno_glowne = new sf::RectangleShape;
	this->okno_labiryntu = new sf::RectangleShape;
	this->rozmiarlabiryntu = _rozmlab;
	this->tytul = new sf::Text;
	this->czcionka = new sf::Font;
	this->punkty_txt = new sf::Text;
	this->moc_txt = new sf::Text;
	this->zycia_txt = new sf::Text;
}

Interfejs::~Interfejs() {
	delete okno_glowne;
	delete okno_labiryntu;
	delete tytul;
	delete punkty_txt;
	delete moc_txt;
	delete zycia_txt;
	delete czcionka;
}

void Interfejs::init() {
	czcionka->loadFromFile("ARIBL0.ttf");
	sf::Texture* txt = new sf::Texture;
	txt->loadFromFile("dark.jpg");
	sf::Texture* txt2 = new sf::Texture;
	txt2->loadFromFile("rusty.jpg");

	okno_glowne->setSize(rozmiarokna);
	okno_glowne->setFillColor(sf::Color::Blue);
	okno_glowne->setTexture(txt2);
	okno_labiryntu->setSize(sf::Vector2f(rozmiarlabiryntu * 25, rozmiarlabiryntu * 25));
	okno_labiryntu->setPosition(sf::Vector2f(700 - 0.5 * rozmiarlabiryntu * 25, 425 - 0.5 * rozmiarlabiryntu * 25));
	//okno_labiryntu->setFillColor(sf::Color::Black);
	okno_labiryntu->setFillColor(sf::Color(107, 99, 99));
	okno_labiryntu->setTexture(txt);
	

	tytul->setPosition(sf::Vector2f(185, 10));
	tytul->setString("PACKMAN MASTERS");
	tytul->setCharacterSize(60);
	tytul->setFillColor(sf::Color(166, 25, 25));
	tytul->setStyle(sf::Text::Bold);
	tytul->setOutlineThickness(1.5);
	tytul->setOutlineColor(sf::Color(250, 230, 230));
	tytul->setFont(*czcionka);
	
	
}

void Interfejs::in_staty(statystyki *staty) {
	
	std::string pkt = "Punkty : " + std::to_string(staty->punkty);
	punkty_txt->setString(pkt);
	punkty_txt->setCharacterSize(40);
	punkty_txt->setFont(*czcionka);
	punkty_txt->setFillColor(sf::Color::White);
	punkty_txt->setPosition(sf::Vector2f(50, 200));

	std::string hp = "Zycia : " + std::to_string(staty->zycie);
	zycia_txt->setString(hp);
	zycia_txt->setCharacterSize(40);
	zycia_txt->setFont(*czcionka);
	zycia_txt->setFillColor(sf::Color::White);
	zycia_txt->setPosition(sf::Vector2f(50, 300));

	std::string pow = "Supermoc : " + std::to_string(staty->moc);
	moc_txt->setString(pow);
	moc_txt->setCharacterSize(40);
	moc_txt->setFont(*czcionka);
	moc_txt->setFillColor(sf::Color::White);
	moc_txt->setPosition(sf::Vector2f(50, 400));
}

void Interfejs::rysuj(sf::RenderWindow* wind) {
	wind->draw(*this->okno_glowne);
	wind->draw(*this->okno_labiryntu);
	wind->draw(*this->punkty_txt);
	wind->draw(*this->zycia_txt);
	wind->draw(*this->moc_txt);
	wind->draw(*this->tytul);
	
}


//klasa gracza
class Gracz {
private:
	int rozmiar;
	int zwrot;
	statystyki* staty;
	sf::Sprite* sprite;
	sf::IntRect* ksztalt_tekstury;
	sf::Font* czcionka;
	bool*** pola_krawedzie;
public:
	Gracz(int rozmiar_labiryntu,bool ***pk);
	~Gracz();
	void init();
	void rysuj(sf::RenderWindow* wind);
	void przesun(sf::Event event,sf::Clock* zegar);
	void ustawtxt(sf::Event event);
	void supermoc(sf::Event event,sf::Clock*zegar);
	void kolizje(mySprite*punkty,mySprite*przeciwnicy, sf::RenderWindow* okno,sf::Clock*zegarAlert);
	void deadAlert(sf::RenderWindow* okno,sf::Clock*zegar);
	void getZapis(float* pozycja_gracz_x, float* pozycja_gracz_y, int* moc, int* pkt, int* hp);
	void setZapis(float pozycja_gracz_x, float pozycja_gracz_y, int moc, int pkt, int hp);
	sf::Vector2f getPozycjaGracza();
	statystyki* getstaty();
};

Gracz::Gracz(int rozmiar_labiryntu,bool***pk) {
	staty = new statystyki;
	sprite = new sf::Sprite;
	ksztalt_tekstury = new sf::IntRect(16, 69, 22, 22);
	rozmiar = rozmiar_labiryntu;
	this->zwrot = 0;
	this->pola_krawedzie = pk;
	this->czcionka = new sf::Font;
}

Gracz::~Gracz() {
	delete staty;
	delete sprite;
	delete ksztalt_tekstury;
	delete czcionka;
	delete* pola_krawedzie;
}

statystyki* Gracz::getstaty() {
	return staty;
}

void Gracz::init() {
	staty->punkty = 0;
	staty->zycie = 3;
	staty->moc = 5;
	
	sf::Texture* tekstura = new sf::Texture;
	
	this->czcionka->loadFromFile("ARIBL0.ttf");
	
	tekstura->loadFromFile("player.png");
	if (!tekstura->loadFromFile("player.png")) {
		std::cout << "NIE DZIALA";
	}
	
	sprite->setTexture(*tekstura);
	
	sprite->setPosition(sf::Vector2f(700 - 0.5 * rozmiar * 25, 425 - 0.5 * rozmiar * 25));
	
	sprite->setTextureRect(*ksztalt_tekstury);
	
}

void Gracz::przesun(sf::Event event,sf::Clock *zegar) {
	int x = 0, y = 0;

	int pos_wkratce_x = 0, pos_wkratce_y = 0;

	pos_wkratce_x = (sprite->getPosition().x - (700 - 0.5 * rozmiar * 25));//bezwzgledna pozycja w labiryncie x
	pos_wkratce_y = (sprite->getPosition().y - (425 - 0.5 * rozmiar * 25));//bezwzgledna pozycja w labiryncie y
	
	if (zegar->getElapsedTime().asMilliseconds() > 20) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Right) {
				x = ((sprite->getPosition().x - (700 - 0.5 * rozmiar * 25)) / 25);//okreslenie wspolrzednej x aktualnej kratki w labiryncie
				y = ((sprite->getPosition().y - (425 - 0.5 * rozmiar * 25)) / 25);//okreslenie wspolrzednej y aktualnej kratki w labiryncie

				if (pola_krawedzie[y][x][1] == false && pos_wkratce_y % 25 <= 10) {//jezeli nie ma krawedzi z prawej strony oraz jest odpowiednia pozycja do ruchu w prawo
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(1, 0);
					this->zwrot = 1;

					zegar->restart();//ruch + przesuniecie tekstury - animacja ruchu + reset zegara, aby bylo ograniczenie czasowe wykonania jednego ruchu
				}
				else if (pola_krawedzie[y][x][1] == true && (pos_wkratce_y % 25 <= 10 && pos_wkratce_x % 25 <= 10)) {//jezeli jest krawedz z prawej strony oraz jest odpowiednia pozycja do ruchu w prawo
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(1, 0);
					this->zwrot = 1;

					//poprawa krañcowego ustawienia po prawej stronie
					if (pos_wkratce_x % 25 == 10) {
						sprite->move(-1, 0);
					}

					zegar->restart();//ruch + ...
				}
				
			}
			
			if (event.key.code == sf::Keyboard::Left) {
				x = ((sprite->getPosition().x - (700 - 0.5 * rozmiar * 25)) / 25);
				y = ((sprite->getPosition().y - (425 - 0.5 * rozmiar * 25)) / 25);

				if (pola_krawedzie[y][x][3] == false && pos_wkratce_y % 25 <= 10) {
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(-1, 0);
					this->zwrot = 3;
					zegar->restart();
					
				}
				else if (pola_krawedzie[y][x][3] == true && (pos_wkratce_y % 25 <= 10 &&pos_wkratce_x % 25 > 0)) {
					//std::cout << x << " " << y << " ";
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(-1, 0);
					this->zwrot = 3;
					zegar->restart();
				}
				
			}
	
			if (event.key.code == sf::Keyboard::Up) {
				x = ((sprite->getPosition().x - (700 - 0.5 * rozmiar * 25)) / 25);
				y = ((sprite->getPosition().y - (425 - 0.5 * rozmiar * 25)) / 25);

				if (pola_krawedzie[y][x][0] == false && pos_wkratce_x % 25 <= 10) {
					//std::cout << x << " " << y << " ";
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(0, -1);
					this->zwrot = 0;
					zegar->restart();
				}

				else if (pola_krawedzie[y][x][0] == true && (pos_wkratce_x % 25 <= 10 && pos_wkratce_y % 25 > 0)) {

					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 17;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(0, -1);
					this->zwrot = 0;
					zegar->restart();
				}
			}

			if (event.key.code == sf::Keyboard::Down) {
				x = ((sprite->getPosition().x - (700 - 0.5 * rozmiar * 25)) / 25);
				y = ((sprite->getPosition().y - (425 - 0.5 * rozmiar * 25)) / 25);

				//std::cout << " x- " << x << " xpos- " << sprite->getPosition().x << " y- " << y << " ypos - " << sprite->getPosition().y << " tr - " << pos_wkratce_x % 25 << " "<<pos_wkratce_y % 25;

				if (pola_krawedzie[y][x][2] == false && pos_wkratce_x % 25 <= 10) {
					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 16;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(0, 1);
					this->zwrot = 2;
					zegar->restart();
				}

				else if (pola_krawedzie[y][x][2] == true && (pos_wkratce_x % 25 <= 10 && pos_wkratce_y % 25 <= 6)) {

					ksztalt_tekstury->left += 48;
					if (ksztalt_tekstury->left > 258) {
						ksztalt_tekstury->left = 17;
					}
					sprite->setTextureRect(*ksztalt_tekstury);
					sprite->move(0, 1);
					this->zwrot = 2;
					zegar->restart();
				}
			}
		}
	}
}

void Gracz::supermoc(sf::Event event,sf::Clock*zegar) {
	int pos_wkratce_x = 0, pos_wkratce_y = 0;

	pos_wkratce_x = (sprite->getPosition().x - (700 - 0.5 * rozmiar * 25));//bezwzgledna pozycja w labiryncie x
	pos_wkratce_y = (sprite->getPosition().y - (425 - 0.5 * rozmiar * 25));//bezwzgledna pozycja w labiryncie y

	if (zegar->getElapsedTime().asMilliseconds() > 100) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::End) {
				if (this->pola_krawedzie[pos_wkratce_y / 25][pos_wkratce_x / 25][this->zwrot] == true && this->staty->moc > 0) {//czy krawedz istnieje,czy moc>0
					switch (this->zwrot) {//w zaleznosci zwrotu bohatera
					case 0:
						if (pos_wkratce_y / 25 != 0) {//gdy patrzy w gore, nie moze byc w gornym rzêdzie
							this->pola_krawedzie[pos_wkratce_y / 25][pos_wkratce_x / 25][this->zwrot] = false;//usuniecie krawedzi w aktualnej kratce
							this->pola_krawedzie[(pos_wkratce_y / 25) - 1][pos_wkratce_x / 25][2] = false;//usuniecie krawedzi w kratce obok
							this->staty->moc--;//obnizenie ilosci mocy
						}
						break;

					case 1:
						if (pos_wkratce_x / 25 != this->rozmiar - 1) {
							this->pola_krawedzie[pos_wkratce_y / 25][pos_wkratce_x / 25][this->zwrot] = false;
							this->pola_krawedzie[pos_wkratce_y / 25][(pos_wkratce_x / 25) + 1][3] = false;
							this->staty->moc--;
						}
						break;

					case 2:
						if (pos_wkratce_y / 25 != this->rozmiar - 1) {
							this->pola_krawedzie[pos_wkratce_y / 25][pos_wkratce_x / 25][this->zwrot] = false;
							this->pola_krawedzie[(pos_wkratce_y / 25) + 1][pos_wkratce_x / 25][0] = false;
							this->staty->moc--;
						}
						break;
						
					case 3:
						if (pos_wkratce_x / 25 != 0) {
							this->pola_krawedzie[pos_wkratce_y / 25][pos_wkratce_x / 25][this->zwrot] = false;
							this->pola_krawedzie[pos_wkratce_y / 25][(pos_wkratce_x / 25) - 1][1] = false;
							this->staty->moc--;
						}
						break;
					}


				}
			}
		}
	}

}

void Gracz::ustawtxt(sf::Event event) {
	//funkcja ustawiaj¹ca wysokoœæ w teksturze odpowiadaj¹ca danej grupie klatek - ograniczenie liczby intrukcji w funkcji przesun
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Right) {
			ksztalt_tekstury->top = 69;
		}
		else if (event.key.code == sf::Keyboard::Left) {
			ksztalt_tekstury->top = 211;
		}
		else if (event.key.code == sf::Keyboard::Up) {
			ksztalt_tekstury->top = 117;
		}
		else if (event.key.code == sf::Keyboard::Down) {
			ksztalt_tekstury->top = 163;
		}
	}
}

void Gracz::rysuj(sf::RenderWindow* wind) {
	wind->draw(*sprite);
}

void Gracz::kolizje(mySprite* punkty,mySprite* przeciwnicy,sf::RenderWindow*okno,sf::Clock*zegarAlert) {
	//kolizje z punktami
	for (int i = 0; i < this->rozmiar; i++) {
		if (punkty[i].czyWyswietlac() == true) {
			if (abs(punkty[i].getPosition().x - sprite->getPosition().x) < 12 && abs(punkty[i].getPosition().y - sprite->getPosition().y) < 12) {
				punkty[i].nieWyswietlaj();
				this->staty->punkty++;
			}
		}
	}
	//kolizje z przeciwnikami
	for (int i = 0; i < 3; i++) {
		if (przeciwnicy[i].czyWyswietlac() == true) {
			if (abs(przeciwnicy[i].getPosition().x - sprite->getPosition().x) < 14 && abs(przeciwnicy[i].getPosition().y - sprite->getPosition().y) < 14) {
				staty->zycie--;
				przeciwnicy[i].nieWyswietlaj();
				zegarAlert->restart();
				
			}
		}
	}

	this->deadAlert(okno, zegarAlert);
}

void Gracz::deadAlert(sf::RenderWindow* okno, sf::Clock* zegar) {
	sf::Text* alert = new sf::Text;
	alert->setString("UWAZAJ, STRACILES ZYCIE");
	alert->setFont(*this->czcionka);
	alert->setCharacterSize(40);
	alert->setPosition(sf::Vector2f(260, 200));
	alert->setFillColor(sf::Color::Red);
	alert->setOutlineColor(sf::Color::White);
	alert->setOutlineThickness(1.5);
	if (zegar->getElapsedTime().asSeconds() < 1.5) {
		okno->draw(*alert);
	}
	delete alert;
}

void Gracz::getZapis(float* pozycja_gracz_x, float* pozycja_gracz_y, int* moc, int* pkt, int* hp) {
	*pozycja_gracz_x = this->sprite->getPosition().x;
	*pozycja_gracz_y = this->sprite->getPosition().y;
	
	*moc = this->getstaty()->moc;
	*hp = this->getstaty()->zycie;
	*pkt = this->getstaty()->punkty;
}

void Gracz::setZapis(float pozycja_gracz_x, float pozycja_gracz_y, int moc,int pkt,int hp) {
	this->staty->moc = moc;
	this->staty->punkty = pkt;
	this->staty->zycie = hp;
	this->sprite->setPosition(sf::Vector2f(pozycja_gracz_x, pozycja_gracz_y));
}

sf::Vector2f Gracz::getPozycjaGracza() {
	return this->sprite->getPosition();
}


//klasa punktów
class Punkty {
private:
	int rozmiar;
	mySprite* sprites;
	sf::Texture* tekstura;
	sf::IntRect* ksztalt_tekstury;
public:
	Punkty(int rozmiar_lab);
	~Punkty();
	void init();
	void ustaw();
	void rysuj(sf::RenderWindow* wind);
	void czyUstawZnowu();
	void getZapis(float* pozycje_punkty_x, float* pozycje_punkty_y, bool* wyswietlanie_punkty,int rozm);
	void setZapis(float* pozycje_punkty_x, float* pozycje_punkty_y, bool* wyswietlanie_punkty);
	mySprite* getSprites();
};

Punkty::Punkty(int rozmiar_lab) {
	this->rozmiar = rozmiar_lab;
	this->sprites = new mySprite[this->rozmiar];
	this->tekstura = new sf::Texture;
	this->ksztalt_tekstury = new sf::IntRect;
	

}

Punkty::~Punkty() {
	delete[] sprites;
	delete tekstura;
	delete ksztalt_tekstury;
}

void Punkty::init() {
	this->tekstura->loadFromFile("resources_basic.png");
	this->ksztalt_tekstury->top = 101;
	this->ksztalt_tekstury->left = 2;
	this->ksztalt_tekstury->width = 19;
	this->ksztalt_tekstury->height = 12;

	for (int i = 0; i < this->rozmiar; i++) {
		this->sprites[i].setTexture(*this->tekstura);
		this->sprites[i].setTextureRect(*this->ksztalt_tekstury);
		
	}
	
	
}

void Punkty::ustaw() {
	//LOSOWANIE BEZ POWTORZEN PAR LICZB
	int* wylosowane_y = new int[this->rozmiar];
	int* wylosowane_x = new int[this->rozmiar];
	int wylosowanych_y = 0;
	int wylosowanych_x = 0;

	do {
int los_y = rand() % this->rozmiar;
int los_x = rand() % this->rozmiar;
if (czyBylaWylosowana(los_x, wylosowane_x, this->rozmiar) == false || czyBylaWylosowana(los_y, wylosowane_y, this->rozmiar) == false) {
	wylosowane_y[wylosowanych_y] = los_y;
	wylosowane_x[wylosowanych_x] = los_x;
	wylosowanych_x++;
	wylosowanych_y++;
}
	} while (wylosowanych_x < this->rozmiar);


	//PRZYPISANIE WYSWIETLANIA I POZYCJI W LABIRYNCIE
	for (int i = 0; i < this->rozmiar; i++) {
		this->sprites[i].wyswietlaj();
		this->sprites[i].setPosition(sf::Vector2f(1 + (700 - 0.5 * rozmiar * 25) + wylosowane_x[i] * 25, 6 + (425 - 0.5 * rozmiar * 25) + wylosowane_y[i] * 25));
	}

	delete[] wylosowane_x;
	delete[] wylosowane_y;

}

void Punkty::rysuj(sf::RenderWindow* wind) {
	for (int i = 0; i < this->rozmiar; i++) {
		if (this->sprites[i].czyWyswietlac() == true) {
			wind->draw(this->sprites[i]);
		}
	}
}

void Punkty::czyUstawZnowu() {
	int j = 0;
	for (int i = 0; i < this->rozmiar; i++) {
		if (this->sprites[i].czyWyswietlac() != false) {
			j++;
		}
	}
	if (j == 0) {
		this->ustaw();
	}
}

void Punkty::getZapis(float* pozycje_punkty_x, float* pozycje_punkty_y, bool* wyswietlanie_punkty,int rozm) {
	for (int i = 0; i < rozm; i++) {
		pozycje_punkty_x[i] = this->sprites[i].getPosition().x;
		pozycje_punkty_y[i] = this->sprites[i].getPosition().y;
		wyswietlanie_punkty[i] = this->sprites[i].czyWyswietlac();
		
	}
}

void Punkty::setZapis(float* pozycje_punkty_x, float* pozycje_punkty_y, bool* wyswietlanie_punkty) {
	for (int i = 0; i < this->rozmiar; i++) {
		sprites[i].setPosition(sf::Vector2f(pozycje_punkty_x[i], pozycje_punkty_y[i]));
		if (wyswietlanie_punkty[i] == true) {
			this->sprites[i].wyswietlaj();
		}
		else {
			this->sprites[i].nieWyswietlaj();
		}
	}
}

mySprite* Punkty::getSprites() {
	return this->sprites;
}


//klasa przeciwników
class Przeciwnik {
private:
	int rozmiar;
	int* ostatni;//ostatni ruch slima
	bool*** pola_krawedzie;
	mySprite* sprites;
	sf::Texture* tekstura;
	sf::IntRect* ksztalt_tekstury;
public:
	Przeciwnik(int rozmiar_lab, bool*** krawedzie);
	~Przeciwnik();
	void init();
	void przesun(sf::Clock* zegar,statystyki*staty, sf::Vector2f poz_gracza);
	void rysuj(sf::RenderWindow* okno);
	void kolizjePkt(mySprite* punkty);
	void getZapis(int last[3], float poz_x[3], float poz_y[3]);
	void setZapis(int* ostatni, float* pozycje_przeciwnik_x, float* pozycje_przeciwnik_y);
	mySprite* getPrzeciwnicy();
};

Przeciwnik::Przeciwnik(int rozmiar_lab, bool*** krawedzie) {
	this->rozmiar = rozmiar_lab;
	this->sprites = new mySprite[3];
	this->tekstura = new sf::Texture;
	this->ksztalt_tekstury = new sf::IntRect;
	this->pola_krawedzie = krawedzie;
	this->ostatni = new int[3];
}

Przeciwnik::~Przeciwnik() {
	delete ostatni;
	
	delete[] sprites;
	delete tekstura;
	delete ksztalt_tekstury;
}

void Przeciwnik::init() {
	this->tekstura->loadFromFile("slime.png");
	this->ksztalt_tekstury->top = 11;
	this->ksztalt_tekstury->left = 7;
	this->ksztalt_tekstury->width = 17;
	this->ksztalt_tekstury->height = 13;

	this->ostatni[0] = -1;
	this->ostatni[1] = -1;
	this->ostatni[2] = -1;

	for (int i = 0; i < 3; i++) {
		this->sprites[i].setTexture(*this->tekstura);
		this->sprites[i].setTextureRect(*this->ksztalt_tekstury);
		this->sprites[i].wyswietlaj();
	}

	this->sprites[0].setPosition(sf::Vector2f(704 - 0.5 * this->rozmiar * 25, 406 + 12.5 * this->rozmiar));
	this->sprites[1].setPosition(sf::Vector2f(679 + 12.5 * this->rozmiar, 406 + 12.5 * this->rozmiar));
	this->sprites[2].setPosition(sf::Vector2f(679 + 12.5 * this->rozmiar, 431 - 0.5 * this->rozmiar * 25));

}

void Przeciwnik::przesun(sf::Clock* zegar,statystyki*staty,sf::Vector2f poz_gracza) {
	for (int i = 0; i < 3; i++) {
		if (sprites[i].czyWyswietlac() == false) {
			//jezeli gracz jest w prawej dolnej cwiartce
			if (poz_gracza.x > 700 && poz_gracza.y > 425) {
				this->sprites[i].setPosition(sf::Vector2f(704 - 0.5 * rozmiar * 25, 431 - 0.5 * rozmiar * 25));
				this->sprites[i].wyswietlaj();
			}
			//jezeli gracz jest w lewej dolnej cwiartce
			else if (poz_gracza.x < 700 && poz_gracza.y > 425) {
				this->sprites[i].setPosition(sf::Vector2f(679 + 12.5 * this->rozmiar, 431 - 0.5 * this->rozmiar * 25));
				this->sprites[i].wyswietlaj();
			}
			//jezeli gracz jest w prawej gornej cwiartce
			else if (poz_gracza.x > 700  && poz_gracza.y < 425) {
				this->sprites[i].setPosition(sf::Vector2f(704 - 0.5 * this->rozmiar * 25, 406 + 12.5 * this->rozmiar));
				this->sprites[i].wyswietlaj();
			}
			//else, jezeli jest w lewej gornej cwiartce
			else {
				this->sprites[i].setPosition(sf::Vector2f(679 + 12.5 * this->rozmiar, 406 + 12.5 * this->rozmiar));
				sprites[i].wyswietlaj();
			}
		}
	}
	//delay ruchu w zaleznosci od punktów
	float x = 1;
	if (staty->punkty > 10) {
		x = 0.7;
	}
	if (staty->punkty > 20) {
		x = 0.4;
	}
	if (staty->punkty > 30) {
		x = 0.2;
	}
	if (staty->punkty > 40) {
		x = 0.1;
	}
	if (staty->punkty > 50) {
		x = 0.075;
	}


	if (zegar->getElapsedTime().asSeconds() > x) {
		//dla 3 slimów
		for (int i = 0; i < 3; i++) {
			//jezeli slime zyje
			if (this->sprites[i].czyWyswietlac() == true) {
				std::vector<int> sasiadujace;
				int nastepne_pole = 0;
				float akt_y = 0;
				float akt_x = 0;

				//pozycja wzglêdem labiryntu
				akt_y = ((sprites[i].getPosition().y)-6 - 425 + 0.5 * this->rozmiar * 25);
				akt_x = ((sprites[i].getPosition().x)-4 - 700 + 0.5 * this->rozmiar * 25);
				//nr kratki
				int akt_y_in = akt_y / 24;
				int akt_x_in = akt_x / 24;
				

				//GÓRA
				//je¿eli wiersz nie jest 0 i nie ma krawêdzi u góry
				if (akt_y_in > 0 && this->pola_krawedzie[akt_y_in][akt_x_in][0] == false) {
					//jezeli ostatni ruch by³ w dó³ dodaj tylko jedno zero
					if (this->ostatni[i] == 0) {
						sasiadujace.push_back(0);
					}
					//jezeli nie, dodaj 30 zer
					else {
						for (int j = 0; j < 30; j++) {
							sasiadujace.push_back(0);
						}
					}
				}
				//PRAWO
				if (akt_x_in < this->rozmiar - 1 && this->pola_krawedzie[akt_y_in][akt_x_in][1]==false) {
					if (this->ostatni[i] == 1) {
						sasiadujace.push_back(1);
					}
					else {
						for (int j = 0; j < 30; j++) {
							sasiadujace.push_back(1);
						}
					}	
				}
				//DÓ£
				if (akt_y_in < this->rozmiar - 1 && this->pola_krawedzie[akt_y_in][akt_x_in][2] == false) {
					if (this->ostatni[i] == 2) {
						sasiadujace.push_back(2);
					}
					else {
						for (int j = 0; j < 30; j++) {
							sasiadujace.push_back(2);
						}
					}
				}
				//LEWO
				if (akt_x_in > 0 && this->pola_krawedzie[akt_y_in][akt_x_in][3] == false) {
					if (this->ostatni[i] == 3) {
						sasiadujace.push_back(3);
					}
					else {
						for (int j = 0; j < 30; j++) {
							sasiadujace.push_back(3);
						}
					}
				}

				//std::cout << " kratki : "<<akt_x_in << " " << akt_y_in << " pozycje: "<<akt_x<<" "<<akt_y;
				//losowanie nastêpnego pola
				nastepne_pole = sasiadujace[rand() % sasiadujace.size()];
				//w zaleznosci od nastêpnego pola ustawiam ostatni ruch
				if (nastepne_pole == 0) {
					this->ostatni[i] = 2;
				}
				else if (nastepne_pole == 1) {
					this->ostatni[i] = 3;
				}
				else if (nastepne_pole == 2) {
					this->ostatni[i] = 0;
				}
				else if (nastepne_pole == 3) {
					this->ostatni[i] = 1;
				}
				
				//std::cout << nastepne_pole << " " << this->ostatni[i] << "  next ";
				//ruch w zaleznoœci od nastêpnego pola
				switch (nastepne_pole) {
				case 0:
					for (int j = 0; j < 4; j++) {
						
						sprites[i].move(sf::Vector2f(0, -6.25));
						
					}
					break;
				case 1:
					for (int j = 0; j < 5; j++) {
						sprites[i].move(sf::Vector2f(5, 0));
					}
					break;
				case 2:
					for (int j = 0; j < 5; j++) {
						sprites[i].move(sf::Vector2f(0, 5));
					}
					break;
				case 3:
					for (int j = 0; j < 5; j++) {
						sprites[i].move(sf::Vector2f(-5, 0));
					}
					break;
				}
				sasiadujace.clear();

			}
		}
		zegar->restart();
	}
	
	
}

mySprite* Przeciwnik::getPrzeciwnicy() {
	return this->sprites;
}

void Przeciwnik::kolizjePkt(mySprite* punkty) {

	for (int i = 0; i < this->rozmiar; i++) {
		for(int j=0;j<3;j++){
			if (punkty[i].czyWyswietlac() == true) {
				if (abs(punkty[i].getPosition().x - sprites[j].getPosition().x) < 12 && abs(punkty[i].getPosition().y - sprites[j].getPosition().y) < 12) {
					punkty[i].nieWyswietlaj();
				}
			}
		}
	}

}

void Przeciwnik::rysuj(sf::RenderWindow* okno) {
	for (int i = 0; i < 3; i++) {
		if (sprites[i].czyWyswietlac() == true) {
			okno->draw(this->sprites[i]);
		}
	}
}

void Przeciwnik::getZapis(int *last, float *poz_x, float *poz_y) {
	last[0] = this->ostatni[0];
	last[1] = this->ostatni[1];
	last[2] = this->ostatni[2];
	
	
	poz_x[0] = this->sprites[0].getPosition().x;
	poz_x[1] = this->sprites[1].getPosition().x;
	poz_x[2] = this->sprites[2].getPosition().x;
	poz_y[0] = this->sprites[0].getPosition().y;
	poz_y[1] = this->sprites[1].getPosition().y;
	poz_y[2] = this->sprites[2].getPosition().y;

	
}

void Przeciwnik::setZapis(int*ostatni, float*pozycje_przeciwnik_x, float*pozycje_przeciwnik_y) {
	this->ostatni = ostatni;
	for (int i = 0; i < 3; i++) {
		this->sprites[i].setPosition(sf::Vector2f(pozycje_przeciwnik_x[i], pozycje_przeciwnik_y[i]));
	}
}



class Menu {
private:
	int akt_dzialanie;
	int flaga_ranking;
	int ile_ranking;
	int x;
	int y;
	sf::Vector2f* rozmiar_okna;
	sf::RectangleShape* glowne_tlo;
	sf::Text* tytul;
	sf::Text* nowa_gra;
	sf::Text* wczytaj_gre;
	sf::Text* instrukcja;
	sf::Text* wyjscie;
	sf::Text* kontrola;
	sf::Text* powrot;
	sf::Text* pomocf1;
	sf::Text* rekordy;
	sf::Font* czcionka;
	sf::Sprite* glowny_bohater;
	sf::IntRect* tekstura_bohatera;
	sf::Texture* txt_rusty;
	sf::RenderWindow* okno;
	Plansza* mapa;
	Gracz* player;
	Interfejs* interfejs;
	Punkty* punkty;
	Przeciwnik* przeciwnicy;
public:
	Menu(sf::RenderWindow* window);
	void init();
	void ustawTeksty();
	void dzialanie(sf::Event event,sf::Clock *zegar,sf::Clock*zegar2,sf::Clock*zegarAlert);
	void menu_main(sf::Event event, sf::Clock* zegar);
	void nowagra(sf::Event event, sf::Clock* zegar);
	void gra(sf::Event event, sf::Clock* zegar,sf::Clock*zegar2,sf::Clock*zegarAlert);
	void utworz_obiekty();
	void ekranInstrukcji(sf::Event event);
	void GameOver(sf::Event event,sf::Clock*zegar);
	void czyGameOver(statystyki* staty);
	void czyWyjsc(sf::Event event, sf::Clock* zegar, sf::Clock* zegarEnemies);
	void Zapis(sf::Clock*zegar);
	void ZapisWyjdz(sf::Clock* zegar);
	void wczytajGre();
	void instrukcjaF1(sf::Event event);
	void ranking(sf::Event event);
	
};

Menu::Menu(sf::RenderWindow* window) {
	this->akt_dzialanie = 0;
	this->flaga_ranking = 0;
	this->ile_ranking = 0;
	this->x = 0;
	this->y = 1;
	this->glowne_tlo = new sf::RectangleShape;
	this->tytul = new sf::Text;
	this->nowa_gra = new sf::Text;
	this->wczytaj_gre = new sf::Text;
	this->instrukcja = new sf::Text;
	this->wyjscie = new sf::Text;
	this->kontrola = new sf::Text;
	this->powrot = new sf::Text;
	this->pomocf1 = new sf::Text;
	this->rekordy = new sf::Text;
	this->czcionka = new sf::Font;
	this->txt_rusty = new sf::Texture;
	this->glowny_bohater = new sf::Sprite;
	this->tekstura_bohatera = new sf::IntRect(16, 69, 22, 22);
	this->rozmiar_okna = new sf::Vector2f(1024, 768);
	this->okno = window;
	this->mapa = new Plansza(20, sf::Vector2f(1024, 768));
	this->player = new Gracz(20, mapa->getPolaKrawedzie());
	this->interfejs = new Interfejs(20, sf::Vector2f(1024, 768));
	this->punkty = new Punkty(20);
	this->przeciwnicy = new Przeciwnik(20,mapa->getPolaKrawedzie());
}

void konfiguracjaPoziomowtxt(sf::Text* txt, int N, sf::Font* czcionka) {
	//przypisanie cyferek przy wyborze levelu
	for (int i = 0; i < N; i++) {
		txt[i].setPosition(sf::Vector2f(120 + i * 75, 350));
		txt[i].setCharacterSize(30);
		txt[i].setFillColor(sf::Color::White);
		txt[i].setString(std::to_string(10 + i));
		txt[i].setFont(*czcionka);
		txt[i].setOutlineThickness(1.5);
		txt[i].setOutlineColor(sf::Color(77, 5, 5));
	}
}

void Menu::init() {
	sf::Texture* tekstura;
	tekstura = new sf::Texture;

	this->txt_rusty->loadFromFile("rusty.jpg");

	glowne_tlo->setSize(sf::Vector2f(1024, 768));
	glowne_tlo->setTexture(this->txt_rusty);
	glowne_tlo->setFillColor(sf::Color(18, 18, 18));

	tekstura->loadFromFile("player.png");
	if (!tekstura->loadFromFile("player.png")) {
		std::cout << "NIE DZIALA";
	}
	

	glowny_bohater->setTexture(*tekstura);
	glowny_bohater->setTextureRect(*tekstura_bohatera);
	glowny_bohater->setPosition(sf::Vector2f(700, 450));
	glowny_bohater->setScale(sf::Vector2f(12, 12));

	this->ustawTeksty();
	
}

void Menu::ustawTeksty() {
	//design tekstów
	czcionka->loadFromFile("ARIBL0.ttf");

	tytul->setPosition(sf::Vector2f(180, 10));
	tytul->setString("PACKMAN MASTERS");
	tytul->setCharacterSize(60);
	tytul->setFillColor(sf::Color(166, 25, 25));
	tytul->setStyle(sf::Text::Bold);
	tytul->setOutlineThickness(1.5);
	tytul->setOutlineColor(sf::Color(250, 230, 230));
	tytul->setFont(*czcionka);

	nowa_gra->setPosition(sf::Vector2f(100, 200));
	nowa_gra->setString("NOWA GRA");
	nowa_gra->setCharacterSize(40);
	nowa_gra->setFillColor(sf::Color::White);
	nowa_gra->setOutlineThickness(1.5);
	nowa_gra->setOutlineColor(sf::Color(77,5,5));
	nowa_gra->setFont(*czcionka);

	wczytaj_gre->setPosition(sf::Vector2f(100, nowa_gra->getPosition().y + 90));
	wczytaj_gre->setString("WCZYTAJ GRE");
	wczytaj_gre->setCharacterSize(40);
	wczytaj_gre->setFillColor(sf::Color::White);
	wczytaj_gre->setOutlineThickness(1.5);
	wczytaj_gre->setOutlineColor(sf::Color(77, 5, 5));
	wczytaj_gre->setFont(*czcionka);

	instrukcja->setPosition(sf::Vector2f(100, wczytaj_gre->getPosition().y + 90));
	instrukcja->setString("INSTRUKCJA GRY");
	instrukcja->setCharacterSize(40);
	instrukcja->setFillColor(sf::Color::White);
	instrukcja->setOutlineThickness(1.5);
	instrukcja->setOutlineColor(sf::Color(77, 5, 5));
	instrukcja->setFont(*czcionka);

	wyjscie->setPosition(sf::Vector2f(100, instrukcja->getPosition().y + 90));
	wyjscie->setString("WYJDZ Z GRY");
	wyjscie->setCharacterSize(40);
	wyjscie->setFillColor(sf::Color::White);
	wyjscie->setOutlineThickness(1.5);
	wyjscie->setOutlineColor(sf::Color(77, 5, 5));
	wyjscie->setFont(*czcionka);

	kontrola->setPosition(sf::Vector2f(50, 740));
	kontrola->setString("WCISNIJ CTRL ABY PRZEJSC DALEJ");
	kontrola->setCharacterSize(20);
	kontrola->setFillColor(sf::Color::White);
	kontrola->setOutlineThickness(1.5);
	kontrola->setOutlineColor(sf::Color(77, 5, 5));
	kontrola->setFont(*czcionka);

	powrot->setPosition(sf::Vector2f(640, 740));
	powrot->setString("WCISNIJ ALT ABY SIE COFNAC");
	powrot->setCharacterSize(20);
	powrot->setFillColor(sf::Color::White);
	powrot->setOutlineThickness(1.5);
	powrot->setOutlineColor(sf::Color(77, 5, 5));
	powrot->setFont(*czcionka);

	pomocf1->setPosition(sf::Vector2f(50, 740));
	pomocf1->setString("WCISNIJ F1 ABY WYSWIETLIC POMOC");
	pomocf1->setCharacterSize(20);
	pomocf1->setFillColor(sf::Color::White);
	pomocf1->setOutlineThickness(1.5);
	pomocf1->setOutlineColor(sf::Color(77, 5, 5));
	pomocf1->setFont(*czcionka);
}

void Menu::dzialanie(sf::Event event, sf::Clock* zegar,sf::Clock*zegar2,sf::Clock*zegarAlert) {
	//dzia³anie w zale¿noœci od aktualnego dzia³ania
	switch (this->akt_dzialanie) {
	case 0:
		this->menu_main(event, zegar);
		break;
	case 1:
		this->nowagra(event,zegar);
		break;
	case 2:
		this->wczytajGre();
		break;
	case 3:
		this->ekranInstrukcji(event);
		break;
	case 4:
		this->okno->close();
		break;
	case 5:
		this->gra(event, zegar, zegar2,zegarAlert);
		break;
	case 6:
		this->GameOver(event,zegar);
		break;
	case 7:
		this->czyWyjsc(event, zegar, zegar2);
		break;
	case 8:
		this->Zapis(zegar);
		break;
	case 9:
		this->ZapisWyjdz(zegar);
		break;
	case 10:
		this->instrukcjaF1(event);
		zegar2->restart();
		zegar->restart();
		break;
	case 11:
		this->ranking(event);
		break;
	}

	
}

void Menu::menu_main(sf::Event event, sf::Clock* zegar) {
	sf::Text* rank = new sf::Text;
	rank->setCharacterSize(20);
	rank->setString("WCISNIJ F3 ABY ZOBACZYC RANKING");
	rank->setPosition(sf::Vector2f(550, 740));
	rank->setOutlineColor(sf::Color(77, 5, 5));
	rank->setOutlineThickness(1.5);
	rank->setFont(*czcionka);

	//wyœwietlenie
	okno->draw(*glowne_tlo);
	okno->draw(*glowny_bohater);
	okno->draw(*tytul);
	okno->draw(*nowa_gra);
	okno->draw(*wczytaj_gre);
	okno->draw(*instrukcja);
	okno->draw(*wyjscie);
	okno->draw(*kontrola);
	okno->draw(*rank);
	//utworzenie tablicy tekstów wyboru
	sf::Text* teksty;
	teksty = new sf::Text[4];

	teksty[0] = *nowa_gra;
	teksty[1] = *wczytaj_gre;
	teksty[2] = *instrukcja;
	teksty[3] = *wyjscie;


	if (zegar->getElapsedTime().asSeconds() > 0.3) {
		if (event.type == sf::Event::KeyPressed) {
			//std::cout << event.type;
			if (event.key.code == sf::Keyboard::Down) {
				x += 1;
				y = x - 1;
				if (x == 4) {
					x = 0;
					y = 3;
				}
				zegar->restart();
			}
			if (event.key.code == sf::Keyboard::Up) {
				x -= 1;
				y = x + 1;
				if (x == -1) {
					x = 3;
					y = 0;
				}
				zegar->restart();
			}
		}
	}

	teksty[x].setCharacterSize(60);
	teksty[x].setFillColor(sf::Color(255, 179, 179));
	teksty[y].setCharacterSize(40);
	teksty[y].setFillColor(sf::Color::White);

	*nowa_gra = teksty[0];
	*wczytaj_gre = teksty[1];
	*instrukcja = teksty[2];
	*wyjscie = teksty[3];

	if (zegar->getElapsedTime().asSeconds() > 0.3) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::RControl || event.key.code == sf::Keyboard::LControl) {
				switch (x) {
				case 0:
					this->akt_dzialanie = 1;
					zegar->restart();
					break;
				case 1:
					this->akt_dzialanie = 2;
					zegar->restart();
					break;
				case 2:
					this->akt_dzialanie = 3;
					zegar->restart();
					break;
				case 3:
					this->akt_dzialanie = 4;
					std::cout << ":)";
					break;
				}
			}
			if (event.key.code == sf::Keyboard::F3) {
				this->akt_dzialanie = 11;
				zegar->restart();
			}
		}
	}

	
	delete rank;
	delete[] teksty;
}

void Menu::nowagra(sf::Event event, sf::Clock* zegar) {
	//ekran nowej gry
	okno->draw(*glowne_tlo);
	okno->draw(*tytul);
	okno->draw(*glowny_bohater);
	okno->draw(*kontrola);
	okno->draw(*powrot);

	sf::Text* poziomy;
	poziomy = new sf::Text[11];
	konfiguracjaPoziomowtxt(poziomy, 11, this->czcionka);

	sf::Texture* txt_lab = new sf::Texture;
	txt_lab->loadFromFile("labirynt.png");
	sf::RectangleShape* lab = new sf::RectangleShape(sf::Vector2f(220, 220));
	lab->setTexture(txt_lab);
	lab->setPosition(sf::Vector2f(160, 460));
	okno->draw(*lab);
	
	sf::Text* wybor = new sf::Text;
	wybor->setCharacterSize(35);
	wybor->setFont(*this->czcionka);
	wybor->setFillColor(sf::Color(60, 10, 255));
	wybor->setPosition(sf::Vector2f(195, 250));
	wybor->setString("WYBIERZ ROZMIAR LABIRYNTU");
	wybor->setOutlineThickness(1);
	wybor->setOutlineColor(sf::Color::White);
	okno->draw(*wybor);

	//x - œledzenie aktualnie wybranego tekstu, y - œledzenie poprzednio wybranego tekstu
	if (zegar->getElapsedTime().asSeconds() > 0.3) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Right) {
				x++;
				y = x - 1;
				if (x == 11) {
					x = 0;
					y = 10;
				}
				
				zegar->restart();
			}
			if (event.key.code == sf::Keyboard::Left) {
				x--;
				y = x + 1;
				if (x == -1) {
					x = 10;
					y = 0;
				}

				zegar->restart();
			}
		}	
	}

	
	// w zale¿noœci od wybranego tekstu zmieñ rozmiar i kolor
	poziomy[x].setCharacterSize(35);
	poziomy[x].setFillColor(sf::Color(sf::Color::Red));
	poziomy[y].setCharacterSize(30);
	poziomy[y].setFillColor(sf::Color::White);

	if (zegar->getElapsedTime().asSeconds() > 0.5) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::RControl || event.key.code == sf::Keyboard::LControl) {
				//+10 aby rozmiar labiryntu wybraæ, zmiana akt dzia³ania na grê, tworzenie obiektów, neutralizacja x,y
				this->x += 10;
				this->akt_dzialanie = 5;
				
				this->utworz_obiekty();
				this->x = 0;
				this->y = 1;
			}
			if (event.key.code == sf::Keyboard::LAlt || event.key.code == sf::Keyboard::RAlt) {
				this->akt_dzialanie = 0;
				this->x = 0;
				this->y = 1;
			}
		}
	}

	for (int i = 0; i < 11; i++) {
		okno->draw(poziomy[i]);
	}

	delete[] poziomy;
	delete wybor;
	delete lab;
	delete txt_lab;
}

void Menu::utworz_obiekty() {
	delete this->mapa;
	delete this->player;
	delete this->interfejs;
	delete this->punkty;
	delete this->przeciwnicy;


	Plansza* mapa = new Plansza(this->x, sf::Vector2f(1024, 768));
	mapa->losuj_labirynt();
	Gracz* player = new Gracz(this->x, mapa->getPolaKrawedzie());
	player->init();
	Interfejs* interfejs = new Interfejs(this->x, sf::Vector2f(1024, 768));
	interfejs->init();
	Punkty* pkt = new Punkty(this->x);
	pkt->init();
	pkt->ustaw();
	Przeciwnik* oponent = new Przeciwnik(this->x, mapa->getPolaKrawedzie());
	oponent->init();


	this->mapa = mapa;
	this->player = player;
	this->interfejs = interfejs;
	this->punkty = pkt;
	this->przeciwnicy = oponent;

}

void Menu::gra(sf::Event event, sf::Clock* zegar, sf::Clock* zegar2,sf::Clock*zegarAlert) {
	//funcje funkcjonalne gry
	
	interfejs->rysuj(this->okno);
	interfejs->in_staty(this->player->getstaty());
	mapa->rysuj_labirynt(this->okno);
	punkty->rysuj(this->okno);
	punkty->czyUstawZnowu();
	przeciwnicy->rysuj(this->okno);
	przeciwnicy->przesun(zegar2,player->getstaty(),this->player->getPozycjaGracza());
	przeciwnicy->kolizjePkt(punkty->getSprites());
	player->rysuj(this->okno);
	player->ustawtxt(event);
	player->przesun(event, zegar);
	player->supermoc(event, zegar);
	player->kolizje(punkty->getSprites(), przeciwnicy->getPrzeciwnicy(), this->okno, zegarAlert);
	this->czyGameOver(player->getstaty());
	this->okno->draw(*this->powrot);
	this->okno->draw(*this->pomocf1);
	
	//klawisze
	if (zegar->getElapsedTime().asSeconds() > 0.2) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::LAlt || event.key.code == sf::Keyboard::RAlt) {
				this->akt_dzialanie = 7;
				this->x = 0;
				this->y = 1;
				zegar->restart();
			}
			if (event.key.code == sf::Keyboard::F1) {
				this->akt_dzialanie = 10;
				this->x = 0;
				this->y = 0;
				zegar->restart();
			}
		}
	}

}

void Menu::ekranInstrukcji(sf::Event event) {
	sf::RectangleShape* tlo = new sf::RectangleShape;
	tlo->setSize(sf::Vector2f(1024,768));
	tlo->setFillColor(sf::Color(50, 0, 0));
	tlo->setTexture(this->txt_rusty);
	

	sf::Text* opis = new sf::Text;
	opis->setFont(*this->czcionka);
	opis->setCharacterSize(20);
	opis->setPosition(sf::Vector2f(80, 300));
	opis->setString("TWOIM ZADANIEM JEST ZDOBYCIE JAK NAJWIEKSZEJ LICZBY PUNKTOW.\nUNIKAJ GROZNYCH POTWOROW I ZBIERZ JAK NAJWIECEJ BRYLANTOW\n\nUZYWAJ STRZALEK ABY PORUSZAC SIE W LABIRYNCIE\nDZIEKI KLAWISZOWI END UZYJESZ ZGROMADZONEJ MOCY\nZEBY PRZEDOSTAC SIE PRZEZ SCIANY LABIRYNTU\nPORUSZAJAC SIE PO MENU UZYWAJ ALT I CTRL\nPOWODZENIA!");


	this->okno->draw(*tlo);
	this->okno->draw(*this->tytul);
	this->okno->draw(*opis);
	this->okno->draw(*this->glowny_bohater);
	this->okno->draw(*this->powrot);

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::RAlt || event.key.code == sf::Keyboard::LAlt) {
			this->akt_dzialanie = 0;
		}
	}



	delete tlo;
	delete opis;

}

void Menu::czyGameOver(statystyki* staty) {
	if (staty->zycie <= 0) {
		this->akt_dzialanie = 6;
	}
}

void Menu::GameOver(sf::Event event,sf::Clock*zegar) {
	sf::Text* ranking = new sf::Text;
	ranking->setCharacterSize(20);
	ranking->setFont(*czcionka);
	ranking->setString("WCISNIJ F3 ABY ZAPISAC SWOJ WYNIK");
	ranking->setPosition(sf::Vector2f(550, 740));
	ranking->setOutlineColor(sf::Color(77, 5, 5));
	ranking->setOutlineThickness(1.5);

	this->interfejs->in_staty(this->player->getstaty());
	this->interfejs->rysuj(this->okno);
	this->mapa->rysuj_labirynt(this->okno);
	this->punkty->rysuj(this->okno);
	this->przeciwnicy->rysuj(this->okno);
	this->player->rysuj(this->okno);
	this->okno->draw(*this->kontrola);
	this->okno->draw(*ranking);
	

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl) {
			this->akt_dzialanie = 0;
			zegar->restart();
		}
		//JEZELI MA BYC ZAPISANE
		if (event.key.code == sf::Keyboard::F3) {
			sf::Text* wpisz = new sf::Text;
			wpisz->setCharacterSize(40);
			wpisz->setFont(*czcionka);
			wpisz->setString("WPISZ NAZWE GRACZA W KONSOLI");
			wpisz->setPosition(sf::Vector2f(130, 100));
			wpisz->setFillColor(sf::Color(100, 255, 255));
			wpisz->setOutlineColor(sf::Color::White);
			wpisz->setOutlineThickness(1);

			this->okno->draw(*wpisz);
			this->okno->display();

			char* nazwa = new char[30];
			std::cout << "Wpisz nazwe gracza: ";
			fgets(nazwa, 30, stdin);
			//USUNIÊCIE "\n"
			int ts = 0;
			while (nazwa[ts] != '\n') {
				ts++;
			}
			nazwa[ts] = '\0';

			int points = (6 * this->player->getstaty()->punkty) / (sqrt(this->mapa->getrozmiar()));
			int ile = 0;
			

			FILE* fp;
			fp = fopen("ranking.txt", "r+");
			if (fp == NULL) {
				fp = fopen("ranking.txt", "w+");
				fwrite(&ile, sizeof(int), 1, fp);
				fclose(fp);
				fp = fopen("ranking.txt", "r+");
			}
			fread(&ile, sizeof(int), 1, fp);
			fclose(fp);
			std::string spacja = " \0";
			std::string pts = "pkt\0";
			
			//dopisywanie
			if (ile < 5) {
				fp = fopen("ranking.txt", "a+");
				fwrite(nazwa, 30 * sizeof(char), 1, fp);
				fwrite(&points, sizeof(int), 1, fp);
				ile++;
				fclose(fp);

				fp = fopen("ranking.txt", "r+");
				fwrite(&ile, sizeof(int), 1, fp);
				fclose(fp);
			
			

				char** nicki = new char* [ile];
				for (int i = 0; i < ile; i++) {
					nicki[i] = new char[30];
				}
				int* pkt_nicki = new int[ile];

				//zapisanie do tablic
				fp = fopen("ranking.txt", "r+");
				fseek(fp, sizeof(int), SEEK_SET);
				for (int i = 0; i < ile; i++) {
					fread(nicki[i], 30 * sizeof(char), 1, fp);
					fread(&pkt_nicki[i], sizeof(int), 1, fp);
				}
				fclose(fp);
				//sortowanie wg punktów
				if (ile > 1) {
					for (int i = 0; i < ile; i++) {
						for (int j = 0; j < ile - 1; j++) {
							if (pkt_nicki[j] < pkt_nicki[j + 1]) {
								std::swap(pkt_nicki[j], pkt_nicki[j + 1]);
								std::swap(nicki[j], nicki[j + 1]);
							}
						}
					}
				}
				//przypisanie do obiektów sftext
				this->rekordy = new sf::Text[ile];
				
				for (int i = 0; i < ile; i++) {
					int k = pkt_nicki[i];
					this->rekordy[i].setString(std::to_string(i + 1) + spacja + nicki[i] + spacja + std::to_string(k) + spacja + pts);
					this->rekordy[i].setPosition(sf::Vector2f(250, 300 + 80 * i));
					this->rekordy[i].setFont(*this->czcionka);
					this->rekordy[i].setCharacterSize(20);
					
				}
				
				delete[] nicki;
				delete[] pkt_nicki;
				
			}
			else {
				//nowe zmienne ile=5
				char** nicki = new char* [5];
				for (int i = 0; i < 5; i++) {
					nicki[i] = new char[30];
				}
				int* pkt_nicki = new int[5];
				//odczytanie zesz³ych wyników z pliku
				fp = fopen("ranking.txt", "r+");
				fseek(fp, sizeof(int), SEEK_SET);
				for (int i = 0; i < 5; i++) {
					fread(nicki[i], 30 * sizeof(char), 1, fp);
					fread(&pkt_nicki[i], sizeof(int), 1, fp);
				}
				fclose(fp);
				//sortowanie zesz³ych wyników
				for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 4; j++) {
						if (pkt_nicki[j] < pkt_nicki[j + 1]) {
							std::swap(pkt_nicki[j], pkt_nicki[j + 1]);
							std::swap(nicki[j], nicki[j + 1]);
						}
					}
				}
				//ewentualna zamiana+zapis i ponowne posortowanie
				if (pkt_nicki[4] < points) {
					pkt_nicki[4] = points;
					nicki[4] = nazwa;
					fp = fopen("ranking.txt", "r+");
					fseek(fp, sizeof(int), SEEK_SET);
					for (int i = 0; i < 5; i++) {
						fwrite(nicki[i], 30 * sizeof(char), 1, fp);
						fwrite(&pkt_nicki[i], sizeof(int), 1, fp);
					}
					fclose(fp);
				}
				for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 4; j++) {
						if (pkt_nicki[j] < pkt_nicki[j + 1]) {
							std::swap(pkt_nicki[j], pkt_nicki[j + 1]);
							std::swap(nicki[j], nicki[j + 1]);
						}
					}
				}
				//przypisanie do obiektów sftext
				this->rekordy = new sf::Text[5];

				for (int i = 0; i < 5; i++) {
					int k = pkt_nicki[i];
					this->rekordy[i].setString(std::to_string(i+1) + spacja + nicki[i] + spacja + std::to_string(k) + spacja + pts);
					this->rekordy[i].setPosition(sf::Vector2f(250, 300 + 80 * i));
					this->rekordy[i].setFont(*this->czcionka);
					this->rekordy[i].setCharacterSize(20);
					
					
				}
				
				
				delete[] nicki;
				delete[] pkt_nicki;
				

			}
			
			
			this->akt_dzialanie = 0;
			this->flaga_ranking = 0;
			
			delete[] nazwa;
			delete wpisz;
		}
	}

	sf::Text* G_O = new sf::Text;
	G_O->setString("PRZEGRALES!");
	G_O->setCharacterSize(70);
	G_O->setFillColor(sf::Color::Red);
	G_O->setOutlineColor(sf::Color::White);
	G_O->setOutlineThickness(1);
	G_O->setFont(*this->czcionka);
	G_O->setPosition(260, 300);

	this->okno->draw(*G_O);
	delete G_O;
	delete ranking;
	
}

void Menu::czyWyjsc(sf::Event event, sf::Clock* zegar,sf::Clock*zegarEnemies) {
	this->interfejs->in_staty(this->player->getstaty());
	this->interfejs->rysuj(this->okno);
	this->mapa->rysuj_labirynt(this->okno);
	this->punkty->rysuj(this->okno);
	this->przeciwnicy->rysuj(this->okno);
	this->player->rysuj(this->okno);
	this->okno->draw(*this->kontrola);

	sf::Text* teksty = new sf::Text[4];
	sf::RectangleShape* obwodka = new sf::RectangleShape(sf::Vector2f(400, 260));
	sf::Texture* txt_dark = new sf::Texture;
	txt_dark->loadFromFile("dark.jpg");

	for (int i = 0; i < 4; i++) {
		teksty[i].setFont(*this->czcionka);
		teksty[i].setCharacterSize(35);
		teksty[i].setOutlineThickness(1.5);
		teksty[i].setOutlineColor(sf::Color(77, 5, 5));
	}
	
	teksty[0].setString("Wroc do gry");
	teksty[1].setString("Zapisz gre");
	teksty[2].setString("Zapisz i wyjdz");
	teksty[3].setString("Wyjdz do menu");

	teksty[0].setPosition(sf::Vector2f(340, 310));
	teksty[1].setPosition(sf::Vector2f(340, 360));
	teksty[2].setPosition(sf::Vector2f(340, 410));
	teksty[3].setPosition(sf::Vector2f(340, 460));

	obwodka->setTexture(txt_dark);
	obwodka->setFillColor(sf::Color(90, 99, 99));
	obwodka->setPosition(sf::Vector2f(310, 290));
	obwodka->setOutlineThickness(3);
	obwodka->setOutlineColor(sf::Color::White);


	teksty[x].setCharacterSize(40);
	teksty[x].setFillColor(sf::Color(255, 179, 179));
	teksty[y].setCharacterSize(35);
	teksty[y].setFillColor(sf::Color::White);


	if (zegar->getElapsedTime().asSeconds() > 0.2) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Down) {
				y = x;
				x++;
				if (x == 4) {
					x = 0;
					y = 3;
				}
				zegar->restart();
			}
			if (event.key.code == sf::Keyboard::Up) {
				y = x;
				x--;
				if (x == -1) {
					x = 3;
					y = 0;
				}
				zegar->restart();
			}
			
		}
	}
	



	if (zegar->getElapsedTime().asSeconds() > 0.2) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::RControl || event.key.code == sf::Keyboard::LControl) {
				
				switch (x) {
				case 0:
					this->akt_dzialanie = 5;
					zegar->restart();
					break;
				case 1:
					this->akt_dzialanie = 8;
					zegar->restart();
					break;
				case 2:
					this->akt_dzialanie = 9;
					zegar->restart();
					break;
				case 3:
					this->akt_dzialanie = 0;
					x = 0;
					y = 1;
					zegar->restart();
					break;
				}

			}
		}
	}


	this->okno->draw(*obwodka);
	for (int i = 0; i < 4; i++) {
		this->okno->draw(teksty[i]);
	}
	
	zegarEnemies->restart();

	delete[] teksty;
	delete obwodka;
	delete txt_dark;
}

void Menu::Zapis(sf::Clock*zegar) {
	int rozm = this->mapa->getrozmiar();
	int moc = 0;
	int pts = 0;
	int hp = 0;
	if (rozm < 10) {
		rozm = 10;
	}
	if (rozm > 20) {
		rozm = 20;
	}

	//INICJALIZACJA I ZAPIS DANYCH DO NOWOUTWORZONYCH ZMIENNYCH

	bool*** pola_k = new bool** [rozm];
	for (int i = 0; i < rozm; i++) {
		pola_k[i] = new bool* [rozm];
		for (int j = 0; j < rozm; j++) {
			pola_k[i][j] = new bool[4];
		}
	}
	
	for (int i = 0; i < rozm; i++) {
		for (int j = 0; j < rozm; j++) {
			for (int k = 0; k < 4; k++) {
				pola_k[i][j][k] = this->mapa->getPolaKrawedzie()[i][j][k];
			}
		}
	}
	


	int* lastprzec = new int[3];
	float* pozprzec_x = new float[3];
	float* pozprzec_y = new float[3];
	this->przeciwnicy->getZapis(lastprzec, pozprzec_x, pozprzec_y);

	
	bool* wysw_pkt = new bool[rozm];
	float* pozpkt_x = new float[rozm];
	float* pozpkt_y = new float[rozm];
	this->punkty->getZapis(pozpkt_x, pozpkt_y, wysw_pkt,rozm);

	float pozgracz_x = 0;
	float pozgracz_y = 0;
	
	this->player->getZapis(&pozgracz_x, &pozgracz_y, &moc, &pts, &hp);
	
	//ZAPIS DO PLIKU

	FILE* fp;
	fp = fopen("zapis.txt", "w+"); 
	if (fp == NULL) {
		std::cout << "PROBLEM Z UTWORZENIEM ZAPISU";
	}
	if (fp) {
		fwrite(&rozm, sizeof(int), 1, fp);
		
		for (int i = 0; i < rozm; i++) {
			for (int j = 0; j < rozm; j++) {
				for (int k = 0; k < 4; k++) {
					fwrite(&pola_k[i][j][k], sizeof(bool), 1, fp);
				}
			}
		}
		
		fwrite(lastprzec, 3*sizeof(int), 1, fp);
		

		
		fwrite(pozprzec_x, 3*sizeof(float), 1, fp);
		fwrite(pozprzec_y, 3*sizeof(float), 1, fp);

		fwrite(wysw_pkt, rozm*sizeof(bool), 1, fp);
		fwrite(pozpkt_x, rozm*sizeof(float), 1, fp);
		fwrite(pozpkt_y, rozm*sizeof(float), 1, fp);
		//std::cout << pozgracz_x << " " << pozgracz_y;
		fwrite(&pozgracz_x, sizeof(float), 1, fp);
		fwrite(&pozgracz_y, sizeof(float), 1, fp);
		fwrite(&moc, sizeof(int), 1, fp);
		fwrite(&pts, sizeof(int), 1, fp);
		fwrite(&hp, sizeof(int), 1, fp);
		
		
		
		fclose(fp);
	}
	

	
	//POWROT DO MENU IN GAME
	this->akt_dzialanie = 7;
	this->x = 0;
	this->y = 1;
	zegar->restart();
}

void Menu::ZapisWyjdz(sf::Clock* zegar) {
	int rozm = this->mapa->getrozmiar();
	int moc = 0;
	int pts = 0;
	int hp = 0;
	if (rozm < 10) {
		rozm = 10;
	}
	if (rozm > 20) {
		rozm = 20;
	}

	//INICJALIZACJA I ZAPIS DANYCH DO NOWOUTWORZONYCH ZMIENNYCH

	bool*** pola_k = new bool** [rozm];
	for (int i = 0; i < rozm; i++) {
		pola_k[i] = new bool* [rozm];
		for (int j = 0; j < rozm; j++) {
			pola_k[i][j] = new bool[4];
		}
	}

	for (int i = 0; i < rozm; i++) {
		for (int j = 0; j < rozm; j++) {
			for (int k = 0; k < 4; k++) {
				pola_k[i][j][k] = this->mapa->getPolaKrawedzie()[i][j][k];
			}
		}
	}



	int* lastprzec = new int[3];
	float* pozprzec_x = new float[3];
	float* pozprzec_y = new float[3];
	this->przeciwnicy->getZapis(lastprzec, pozprzec_x, pozprzec_y);


	bool* wysw_pkt = new bool[rozm];
	float* pozpkt_x = new float[rozm];
	float* pozpkt_y = new float[rozm];
	this->punkty->getZapis(pozpkt_x, pozpkt_y, wysw_pkt, rozm);

	float pozgracz_x = 0;
	float pozgracz_y = 0;

	this->player->getZapis(&pozgracz_x, &pozgracz_y, &moc, &pts, &hp);

	//ZAPIS DO PLIKU

	FILE* fp;
	fp = fopen("zapis.txt", "w+");
	if (fp == NULL) {
		std::cout << "PROBLEM Z UTWORZENIEM ZAPISU";
	}
	if (fp) {
		fwrite(&rozm, sizeof(int), 1, fp);

		for (int i = 0; i < rozm; i++) {
			for (int j = 0; j < rozm; j++) {
				for (int k = 0; k < 4; k++) {
					fwrite(&pola_k[i][j][k], sizeof(bool), 1, fp);
				}
			}
		}

		fwrite(lastprzec, 3 * sizeof(int), 1, fp);



		fwrite(pozprzec_x, 3 * sizeof(float), 1, fp);
		fwrite(pozprzec_y, 3 * sizeof(float), 1, fp);

		fwrite(wysw_pkt, rozm * sizeof(bool), 1, fp);
		fwrite(pozpkt_x, rozm * sizeof(float), 1, fp);
		fwrite(pozpkt_y, rozm * sizeof(float), 1, fp);
		//std::cout << pozgracz_x << " " << pozgracz_y;
		fwrite(&pozgracz_x, sizeof(float), 1, fp);
		fwrite(&pozgracz_y, sizeof(float), 1, fp);
		fwrite(&moc, sizeof(int), 1, fp);
		fwrite(&pts, sizeof(int), 1, fp);
		fwrite(&hp, sizeof(int), 1, fp);



		fclose(fp);
	}



	//POWROT DO MENU MAIN
	this->akt_dzialanie = 0;
	this->x = 0;
	this->y = 1;
	zegar->restart();
}

void Menu::wczytajGre() {
	int rozm = 0;
	int hp = 0;
	int moc = 0;
	int pts = 0;


	//CZYTANIE Z PLIKU
	FILE* fp;
	fp = fopen("zapis.txt", "r+");
	if (fp) {
		//ZCZYTANIE ROZMIARU
		fread(&rozm, sizeof(int), 1, fp);
		if (rozm < 10) {
			rozm = 10;
		}
		if (rozm > 20) {
			rozm = 20;
		}


		//INICJALIZACJA ZMIENNYCH DO ZAPISU
		bool*** pola_k = new bool** [rozm];
		for (int i = 0; i < rozm; i++) {
			pola_k[i] = new bool* [rozm];
			for (int j = 0; j < rozm; j++) {
				pola_k[i][j] = new bool[4];
			}
		}
		

		int* lastprzec = new int[3];
		float* pozprzec_x = new float[3];
		float* pozprzec_y = new float[3];
		

		bool* wysw_pkt = new bool[rozm];
		float* pozpkt_x = new float[rozm];
		float* pozpkt_y = new float [rozm] ;
		
		float pozgracz_x = 0;
		float pozgracz_y = 0;
		statystyki* stats = new statystyki;
		int xda = 0;
		

		//ENDINIT
		//ZCZYTANIE
		for (int i = 0; i < rozm; i++) {
			for (int j = 0; j < rozm; j++) {
				for (int k = 0; k < 4; k++) {
					fread(&pola_k[i][j][k], sizeof(bool), 1, fp);
				}
			}
		}


		
		fread(lastprzec, 3*sizeof(int), 1, fp);
		

		
		fread(pozprzec_x, 3*sizeof(float), 1, fp);
		fread(pozprzec_y, 3*sizeof(float), 1, fp);

		fread(wysw_pkt, rozm*sizeof(bool), 1, fp);
		fread(pozpkt_x, rozm*sizeof(float), 1, fp);
		fread(pozpkt_y, rozm*sizeof(float), 1, fp);

		fread(&pozgracz_x, sizeof(float), 1, fp);
		fread(&pozgracz_y, sizeof(float), 1, fp);
		fread(&moc, sizeof(int), 1, fp);
		fread(&pts, sizeof(int), 1, fp);
		fread(&hp, sizeof(int), 1, fp);
		

		fclose(fp);
		
		//std::cout << " " << rozm << " " << pola_k[0][0][1] << " " << pola_k[0][0][2] << " " << lastprzec[0] << " " << lastprzec[1] << " " << lastprzec[2] << " pozycje " << pozprzec_y[1] << " " << pozpkt_x[5] << " " << pozpkt_y[2] << " pozycje gracz " << pozgracz_x << " " << pozgracz_y << " " << wysw_pkt[3] << " staty " << moc << " " << hp << " " << pts;
		//USUNIECIE POPRZEDNICH OBIEKTOW
		delete this->mapa;
		delete this->player;
		delete this->przeciwnicy;
		delete this->interfejs;
		delete this->punkty;
		//UTWORZENIE OBIEKTÓW NA PODSTAWIE DANYCH OCZYTANYCH
		Plansza* map = new Plansza(rozm, sf::Vector2f(1024, 768));
		map->losuj_labirynt();
		map->setZapis(pola_k);
		this->mapa = map;
		bool*** mas= new bool** [4];
		mas = this->mapa->getPolaKrawedzie();
	
		Interfejs* inter = new Interfejs(rozm, sf::Vector2f(1024, 768));
		inter->init();
		this->interfejs = inter;

		Gracz* grcz = new Gracz(rozm, this->mapa->getPolaKrawedzie());
		grcz->init();
		grcz->setZapis(pozgracz_x, pozgracz_y, moc, pts, hp);
		this->player = grcz;

		Punkty* pkts = new Punkty(rozm);
		pkts->init();
		pkts->setZapis(pozpkt_x, pozpkt_y, wysw_pkt);
		this->punkty = pkts;

		Przeciwnik* przec = new Przeciwnik(rozm, this->mapa->getPolaKrawedzie());
		przec->init();
		przec->setZapis(lastprzec, pozprzec_x, pozprzec_y);
		this->przeciwnicy = przec;
		
		//ZAPISANIE OBIEKTÓW I WEJŒCIE DO PRZEBIEGU GRY
		
		
	}
	this->akt_dzialanie = 5;
	
}

void Menu::instrukcjaF1(sf::Event event) {
	sf::RectangleShape* tlo = new sf::RectangleShape;
	tlo->setSize(sf::Vector2f(1024, 768));
	tlo->setFillColor(sf::Color(50, 0, 0));
	tlo->setTexture(this->txt_rusty);

	sf::Text* opis = new sf::Text;
	opis->setFont(*this->czcionka);
	opis->setCharacterSize(20);
	opis->setPosition(sf::Vector2f(80, 300));
	opis->setString("TWOIM ZADANIEM JEST ZDOBYCIE JAK NAJWIEKSZEJ LICZBY PUNKTOW.\nUNIKAJ GROZNYCH POTWOROW I ZBIERZ JAK NAJWIECEJ BRYLANTOW\n\nUZYWAJ STRZALEK ABY PORUSZAC SIE W LABIRYNCIE\nDZIEKI KLAWISZOWI END UZYJESZ ZGROMADZONEJ MOCY\nZEBY PRZEDOSTAC SIE PRZEZ SCIANY LABIRYNTU\nPORUSZAJAC SIE PO MENU UZYWAJ ALT I CTRL\nPOWODZENIA!");


	this->okno->draw(*tlo);
	this->okno->draw(*this->tytul);
	this->okno->draw(*opis);
	this->okno->draw(*this->glowny_bohater);
	this->okno->draw(*this->powrot);
	

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::RAlt || event.key.code == sf::Keyboard::LAlt) {
			this->akt_dzialanie = 5;
		}
	}



	delete tlo;
	delete opis;
}

void Menu::ranking(sf::Event event) {
	sf::RectangleShape* tlo = new sf::RectangleShape;
	tlo->setSize(sf::Vector2f(1024, 768));
	tlo->setFillColor(sf::Color(50, 0, 0));
	tlo->setTexture(this->txt_rusty);
	std::string spacja = " ";
	std::string pts = "pkt";
	sf::Text* ranktekst = new sf::Text;
	ranktekst->setString("RANKING");
	ranktekst->setPosition(sf::Vector2f(300, 200));
	ranktekst->setCharacterSize(60);
	ranktekst->setOutlineThickness(1.5);
	ranktekst->setOutlineColor(sf::Color(77, 5, 5));
	ranktekst->setFont(*czcionka);
	

	this->okno->draw(*tlo);
	this->okno->draw(*tytul);
	this->okno->draw(*ranktekst);
	this->okno->draw(*glowny_bohater);
	this->okno->draw(*powrot);

	delete ranktekst;
	delete tlo;

	if (this->flaga_ranking == 0) {
		FILE* fp;
		fp = fopen("ranking.txt", "r+");
		
		if (fp != NULL) {
			fread(&this->ile_ranking, sizeof(int), 1, fp);
		}
		if (this->ile_ranking > 0 && fp != NULL) {
			sf::Text* lista = new sf::Text[this->ile_ranking];

			char** nicki = new char* [this->ile_ranking];
			for (int i = 0; i < this->ile_ranking; i++) {
				nicki[i] = new char[30];
			}
			int* pkt_nicki = new int[this->ile_ranking];

			for (int i = 0; i < this->ile_ranking; i++) {
				fread(nicki[i], 30 * sizeof(char), 1, fp);
				fread(&pkt_nicki[i], sizeof(int), 1, fp);
			}
			fclose(fp);

			for (int i = 0; i < this->ile_ranking; i++) {
				for (int j = 0; j < this->ile_ranking - 1; j++) {
					if (pkt_nicki[j] < pkt_nicki[j + 1]) {
						std::swap(pkt_nicki[j], pkt_nicki[j + 1]);
						std::swap(nicki[j], nicki[j + 1]);
					}
				}
			}

			for (int i = 0; i < this->ile_ranking; i++) {
				int k = pkt_nicki[i];
				lista[i].setString(std::to_string(i + 1) + spacja + nicki[i] + spacja + std::to_string(k) + spacja + pts);
				lista[i].setPosition(sf::Vector2f(250, 300 + 80 * i));
				lista[i].setFont(*this->czcionka);
				lista[i].setCharacterSize(50);
				//this->okno->draw(lista[i]);
			}
			this->rekordy = lista;
			
			delete[] nicki;
			delete[] pkt_nicki;
			this->flaga_ranking = 1;
		}
		else {
			sf::Text* zero = new sf::Text;
			zero->setString("NIE MA JESZCZE ZAPISANYCH WYNIKOW");
			zero->setCharacterSize(30);
			zero->setPosition(sf::Vector2f(160, 400));
			zero->setFont(*this->czcionka);
			this->okno->draw(*zero);
			delete zero;
		}
		
	}
	
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::RAlt || event.key.code == sf::Keyboard::LAlt) {
			this->akt_dzialanie = 0;
		}
	}

	if (this->flaga_ranking == 1) {
		for (int i = 0; i < this->ile_ranking; i++) {
			this->okno->draw(this->rekordy[i]);
		}
	}
	
	
	
}



int main() {
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(1024, 768), "PACKMAN MASTERS");
	sf::RenderWindow* win;
	win = &window;
	sf::Vector2f rozmokna(1024,768);
	sf::Clock* zegar = new sf::Clock;
	sf::Clock* zegar2 = new sf::Clock;
	sf::Clock* zegarAlert = new sf::Clock;
	

	Menu menu(win);
	menu.init();



	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}


		window.clear();
		

		menu.dzialanie(event,zegar,zegar2,zegarAlert);
		
		
		window.display();
	}
	return 0;

}