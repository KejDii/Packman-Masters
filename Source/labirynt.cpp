//#include <iostream>
//#include <SFML/Graphics.hpp>
//#include <ctime>
//#include <cstdlib>
//#include <stack>
//#include <vector>
//
//struct wspolrzedne {
//	int x;
//	int y;
//};
//
//
//int main()
//{
//	srand(time(NULL));
//	sf::RenderWindow window(sf::VideoMode(1024, 768), "PACMAN MASTERS");
//	sf::RenderWindow* win;
//	win = &window;
//	int gora = -1, lewo = -1;
//	int dol = 1, prawo = 1;
//
//	bool pola[4][4] = { true };
//	bool pola_krawedzie[4][4][4] = { true };
//	sf::RectangleShape krawedzie[4][4][4];
//
//	//POCZATKOWE PRZYPISANIE
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			for (int k = 0; k < 4; k++) {
//				krawedzie[i][j][k].setSize(sf::Vector2f(25.f, 2.f));
//				krawedzie[i][j][k].setPosition(sf::Vector2f(250.f, 250.f));
//				pola_krawedzie[i][j][k] = true;
//				
//			}
//		}
//	}
//	
//	//OBROCENIE PIONOWYCH
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			for (int k = 1; k < 4; k+=2) {
//				krawedzie[i][j][k].rotate(90.f);
//
//			}
//		}
//	}
//
//	//ULOZENIE
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			krawedzie[i][j][0].setPosition(sf::Vector2f((250 + j * 25), (250 + i * 25)));
//			krawedzie[i][j][1].setPosition(sf::Vector2f((275 + j * 25), (250 + i * 25)));
//			krawedzie[i][j][2].setPosition(sf::Vector2f((250 + j * 25), (275 + i * 25)));
//			krawedzie[i][j][3].setPosition(sf::Vector2f((250 + j * 25), (250 + i * 25)));
//		}
//	}
//
//	
//
//	//ALGORYTM
//	wspolrzedne aktualny = { 0 };
//	aktualny.x = 0;
//	aktualny.y = 0;
//	std::stack<wspolrzedne> stos;
//	int ilosc_odwiedzonych = 0;
//	std::vector<int> sasiadujace;
//	int nastepne_pole = 0;
//	
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			pola[i][j] = false;
//		}
//	}
//
//	//INICJALIZACJA
//	stos.push(aktualny);
//	ilosc_odwiedzonych = 1;
//	pola[0][0] = true;
//
//	//DZIALANIE
//	while (ilosc_odwiedzonych<16) {
//		aktualny.x = stos.top().x;
//		aktualny.y = stos.top().y;
//
//		//GORA
//		if (stos.top().y > 0) {
//			if (pola[stos.top().y-1][stos.top().x] == false) {
//				sasiadujace.push_back(0);
//			}
//		}
//		//PRAWO
//		if (stos.top().x < 3) {
//			if (pola[stos.top().y][stos.top().x+1] == false) {
//				sasiadujace.push_back(1);
//			}
//		}
//		//DOL
//		if (stos.top().y < 3) {
//			if (pola[stos.top().y + 1][stos.top().x] == false) {
//				sasiadujace.push_back(2);
//			}
//		}
//		//LEWO
//		if (stos.top().x > 0) {
//			if (pola[stos.top().y ][stos.top().x-1] == false) {
//				sasiadujace.push_back(3);
//			}
//		}
//
//		//JEZELI JEST POLE OBOK WCZESNIEJ NIEODWIEDZONE
//		if (!sasiadujace.empty()) {
//			nastepne_pole = sasiadujace[rand() % sasiadujace.size()];
//			std::cout <<  "  akt y - " << aktualny.y << "  akt x - " << aktualny.x << " nast pole " << nastepne_pole << " ilosc odwiedzonych - " << ilosc_odwiedzonych << "\n";
//
//			//W ZALEZNOSCI OD KIERUNKU NASTEPNEGO POLA
//			switch (nastepne_pole) {
//			case 0:
//				aktualny.y -= 1;
//				//std::cout << aktualny.y;
//				pola[aktualny.y][aktualny.x] = true;
//				pola_krawedzie[aktualny.y][aktualny.x][2] = false;
//				pola_krawedzie[aktualny.y + 1][aktualny.x][0] = false;
//				stos.push(aktualny);
//				break;
//			case 1:
//				aktualny.x +=prawo;
//				//std::cout << aktualny.x;
//				pola[aktualny.y][aktualny.x] = true;
//				pola_krawedzie[aktualny.y][aktualny.x][3] = false;
//				pola_krawedzie[aktualny.y][aktualny.x-1][1] = false;
//				stos.push(aktualny);
//				break;
//			case 2:
//				aktualny.y += dol;
//				//std::cout << aktualny.y;
//				pola[aktualny.y][aktualny.x] = true;
//				pola_krawedzie[aktualny.y][aktualny.x][0] = false;
//				pola_krawedzie[aktualny.y-1][aktualny.x][2] = false;
//				stos.push(aktualny);
//				break;
//			case 3:
//				aktualny.x += lewo;
//				//std::cout << aktualny.x;
//				pola[aktualny.y][aktualny.x] = true;
//				pola_krawedzie[aktualny.y][aktualny.x][1] = false;
//				pola_krawedzie[aktualny.y][aktualny.x+1][3] = false;
//				stos.push(aktualny);
//				break;
//			}
//
//			ilosc_odwiedzonych += 1;
//			sasiadujace.clear();
//		}
//		else {
//			stos.pop();
//		}
//
//		
//	}
//
//
//	
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
//		for (int i = 0; i < 4; i++) {
//			for (int j = 0; j < 4; j++) {
//				for (int k = 0; k < 4; k++) {
//					if (pola_krawedzie[i][j][k] == true) {
//						window.draw(krawedzie[i][j][k]);
//					}
//				}
//			}
//		}
//	
//		window.display();
//	}
//	return 0;
//}