//#include <iostream>
//#include <SFML/Graphics.hpp>
//#include <ctime>
//#include <cstdlib>
//
//
//class Plansza {
//private:
//	float rozmiar_x;
//	float rozmiar_y;
//	float x;
//	float y;
//	int ln_pion[20][20];
//	int ln_poz[20][20];
//	sf::RectangleShape linie_pion[20][20];
//	sf::RectangleShape linie_poz[20][20];
//public:
//	Plansza(float rx, float ry, float xin, float yin);
//	void rysuj_obwodke(sf::RenderWindow* wind);
//	void rysuj_labirynt(sf::RenderWindow* wind);
//	void losuj_labirynt();
//	void wyglad_labiryntu();
//};
//
//Plansza::Plansza(float rx, float ry, float xin, float yin) {
//	rozmiar_x = rx;
//	rozmiar_y = ry;
//	x = xin;
//	y = yin;
//	for (int i = 0; i < 20; i++) {
//		for (int j = 0; j < 20; j++) {
//			this->ln_pion[i][j] = 0;
//			this->ln_poz[i][j] = 0;
//		}
//	}
//}
//
//void Plansza::rysuj_obwodke(sf::RenderWindow *wind) {
//	sf::RectangleShape obw(sf::Vector2f(rozmiar_x, rozmiar_y));
//	obw.setOutlineThickness(8.f);
//	obw.setFillColor(sf::Color::Black);
//	obw.setOutlineColor(sf::Color::White);
//	obw.setPosition(sf::Vector2f(x, y));
//	wind->draw(obw);
//	
//}
//
//void Plansza::wyglad_labiryntu() {
//	for (int i = 0; i < 20; i++) {
//		for (int j = 0; j < 20; j++) {
//
//			this->linie_pion[i][j].setSize(sf::Vector2f(25.f, 2.f));
//			this->linie_pion[i][j].rotate(90);
//			this->linie_pion[i][j].setPosition(sf::Vector2f(275 + (i * 25), 125 + (j * 25)));
//
//			this->linie_poz[i][j].setSize(sf::Vector2f(25.f, 2.f));
//
//			this->linie_poz[i][j].setPosition(sf::Vector2f(250 + (j * 25), 150 + (i * 25)));
//		}
//	}
//}
//
//void Plansza::rysuj_labirynt(sf::RenderWindow* wind) {
//
//	for (int i = 0; i < 20; i++) {
//		for (int j = 0; j < 20; j++) {
//			
//			if (this->ln_pion[i][j] == 1) {
//
//				wind->draw(this->linie_pion[i][j]);
//
//			}
//			if (this->ln_poz[i][j] == 1) {
//				wind->draw(this->linie_poz[i][j]);
//			}
//		}
//	}
//	
//}
////lnpion[i]  | | | 
////lnpoz[i]  -
////          -
//void Plansza::losuj_labirynt() {
//
//	//POCZATKOWE WARTOSCI
//	for (int i = 0; i < 20; i++) {
//		for (int j = 0; j < 20; j++) {
//			this->ln_pion[i][j] = rand() % 2;
//			 this->ln_poz[i][j] = rand() % 2;
//		}
//	}
//	//USUNIECIE SKRAJNYCH Z DOLU I Z PRAWEJ
//	for (int i = 0; i < 20; i++) {
//		this->ln_pion[19][i] = 0;
//		this->ln_poz[19][i] = 0;
//	}
//
//	int los = 0;
//	//USUNIECIE JEDNEJ KRESKI JEZELI SASIADUJE Z PIONOW¥
//	for (int i = 0; i < 20; i++) {
//		for (int j = 0; j < 20; j++) {
//			if (this->ln_pion[i][j] == this->ln_poz[j][i] && this->ln_poz[j][i] == 1) {
//				los = rand() % 10;
//				if (los > 5) {
//					this->ln_poz[j][i] = 0;
//				}
//				else {
//					this->ln_poz[i][j] = 0;
//				}
//			}
//		}
//	}
//
//	
//	//USUWANIE SAMOTNYCH PIONOWYCH LINII
//	for (int i = 0; i < 19; i++) {
//		for (int j = 0; j < 18; j++) {
//			
//			if (this->ln_pion[i][j + 1] == 1 && ln_pion[i][j] == 0 && ln_pion[i][j + 2] == 0 && ln_poz[j][i] == 0 && ln_poz[j + 1][i] == 0 && ln_poz[j][i + 1] == 0 && ln_poz[j + 1][i + 1] == 0) {
//				this->ln_pion[i][j + 1] = 0;
//				this->linie_pion[i][j + 1].setOutlineColor(sf::Color::Red);
//				this->linie_pion[i][j + 1].setFillColor(sf::Color::Red);
//				
//			}
//		}
//	}
//
//	//USUWANIE SAMOTNYCH POZIOMYCH LINII
//	for (int i = 0; i < 18; i++) {
//		for (int j = 0; j < 19; j++) {
//			if (this->ln_poz[i + 1][j+1] == 1 && ln_poz[i+1][j] == 0 ) {
//				this->ln_poz[i + 1][j+1] = 1;
//				this->linie_poz[i + 1][j+1].setFillColor(sf::Color::Red);
//			}
//		}
//	}
//
//
//}
//
//
//int main()
//{
//	srand(time(NULL));
//	sf::RenderWindow window(sf::VideoMode(1024, 768), "PACMAN MASTERS");
//	sf::RenderWindow* win;
//	win = &window;
//	
//
//	Plansza plansza(500.f, 500.f, 250.f, 125.f);
//	
//	
//	plansza.losuj_labirynt();
//	plansza.wyglad_labiryntu();
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//		window.clear();
//		
//		plansza.rysuj_obwodke(win);
//		plansza.rysuj_labirynt(win);
//		
//		window.display();
//	}
//	return 0;
//}