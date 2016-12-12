#define _USE_MATH_DEFINES

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <time.h>
#include <iostream>

using namespace std;

//Definicja typu point3 - punkt w przestrzeni 3D
typedef float point3[3];


const int N = 50;

//Tablica na punkty
point3 pointsTab[N + 1][N + 1];

//Tablic na wektory normalne
point3 pointsNorms[N + 1][N + 1];

static GLfloat viewer[] = { 0.0, 0.0, 20.0 };
// inicjalizacja po�o�enia obserwatora

static GLfloat light_position0[] = { -10.0, 0.0, 0.0, 0 };
static GLfloat light_position1[] = { 10.0, 0.0, 0.0, 0 };
// inicnacja po�o�enie �wita�a wraz z parametrem okre�laj�cy typ o�wieltenie

static GLfloat	fi = 0.0, theta = 0.0 , fi2 = 0.0, theta2 = 0.0;			// k�ty obrotu, elewacja i azymut

static GLfloat	pix2angle_x = 0.0,	// przelicznik pikseli na stopnie
pix2angle_y = 0.0;
static GLint	status = 0;			// stan klawiszy myszy
									// 0 - nie naci�ni�to �adnego klawisza
									// 1 - naci�ni�ty zosta� lewy klawisz, 2 - prawy
static float x_pos_old = 0;			// poprzednia pozycja kursora myszy
static float y_pos_old = 0;
static float x_pos_old2 = 0;
static float y_pos_old2 = 0;
static float delta_x = 0;			// r�nica pomi�dzy pozycj� bie��c�
static float delta_y = 0;			// i poprzedni� kursora myszy
static float delta_x2 = 0;			
static float delta_y2 = 0;

									//Parametry programu
									//Liczba punktow na jaka dzielimy kwadrat jednostkowy
float dlugosc = 1.0;  //Dlugosc boku kwadratu
float viewerR = 20.0, viewerX = 0, viewerY = 0;	//Promien sfery obserwatora


												//Tablica katow obrotu
static GLfloat angle[] = { 0.0, 0.0, 0.0 };

//Funkcja wyliczajaca wspolrzedna X punktu (u,v) w przestrzeni 3D
float calc3Dx(float u, float v)
{
	float x, a = v*M_PI;

	x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(a);
	return x;
}

//Funkcja wyliczajaca wspolrzedna Y punktu (u,v) w przestrzeni 3D
float calc3Dy(float u, float v)
{
	float y;

	y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
	return y - 5;
}

//Funkcja wyliczajaca wspolrzedna Z punktu (u,v) w przestrzeni 3D
float calc3Dz(float u, float v)
{
	float z, a = v*M_PI;

	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a);
	return z;
}

//Obliczenie wspolrzednej X wektora normalnego do powierzchni w punkcie
float calcNormX(float u, float v)
{
	float x, a = v*M_PI;

	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
	float yv = 0;
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a);
	float zv = -M_PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a);

	x = (GLfloat)(yu*zv - zu*yv);
	return x;
}

//Obliczenie wspolrzednej Y wektora normalnego do powierzchni w punkcie
float calcNormY(float u, float v)
{
	float y, a = v*M_PI;

	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a);
	float xv = M_PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a);
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(a);
	float zv = -M_PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(a);

	y = (GLfloat)(zu*xv - xu*zv);
	return y;
}

//Obliczenie wspolrzednej Z wektora normalnego do powierzchni w punkcie
float calcNormZ(float u, float v)
{
	float z, a = v*M_PI;

	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(a);
	float xv = M_PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(a);
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
	float yv = 0;

	z = (GLfloat)(xu*yv - yu*xv);
	return z;
}

//Funkcja generujaca siatke puntow, najpierw w 2D, potem w 3D
void genPointsMesh()
{
	float stepXY = dlugosc / N;

	//Przypisanie punktom wspolrzednych
	for (int i = 0; i<N + 1; i++)
	{
		for (int j = 0; j<N + 1; j++)
		{
			pointsTab[i][j][0] = j*stepXY;
			pointsTab[i][j][1] = i*stepXY;
		}
	}

	//Przeksztalcenie wspolrzednych z dziedziny parametrycznej
	//w przestrzen 3D
	float u, v;
	for (int i = 0; i<N + 1; i++) {
		for (int j = 0; j<N + 1; j++) {
			v = pointsTab[i][j][0];
			u = pointsTab[i][j][1];
			pointsTab[i][j][0] = calc3Dx(u, v);
			pointsTab[i][j][1] = calc3Dy(u, v);
			pointsTab[i][j][2] = calc3Dz(u, v);

			//Wyliczenie wspolrzednych wektorow normalnych do powierzchni jajka
			float x = calcNormX(u, v);
			float y = calcNormY(u, v);
			float z = calcNormZ(u, v);

			//Normalizacja wektorow normalnych do powierzchni jajka
			//Wektory na bokach jajka
			if (i < N / 2) {
				pointsNorms[i][j][0] = x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][1] = y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][2] = z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			}
			if (i > N / 2) {
				pointsNorms[i][j][0] = -1.0*x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][1] = -1.0*y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
				pointsNorms[i][j][2] = -1.0*z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			}
			//Wektory na "szczycie" jajka
			if (i == N / 2) {
				pointsNorms[i][j][0] = 0;
				pointsNorms[i][j][1] = 1;
				pointsNorms[i][j][2] = 0;
			}
			//Wektory na "dnie" jajka
			if (i == 0 || i == N)
			{
				pointsNorms[i][j][0] = 0;
				pointsNorms[i][j][1] = -1;
				pointsNorms[i][j][2] = 0;
			}
		}
	}
}

//Funkcja renderujaca okreslony model jajka
void Egg(void)
{
	//Wygenerowanie siatki 3D punktow
	genPointsMesh();

	//Parametry rysowania
	glColor3f(1.0, 1.0, 0.0);

	//Rysowanie jajka - trojkaty
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			//W jedna strone
			glBegin(GL_TRIANGLES);
			glNormal3fv(pointsNorms[i][j + 1]);
			glVertex3fv(pointsTab[i][j + 1]);
			glNormal3fv(pointsNorms[i + 1][j]);
			glVertex3fv(pointsTab[i + 1][j]);
			glNormal3fv(pointsNorms[i + 1][j + 1]);
			glVertex3fv(pointsTab[i + 1][j + 1]);
			glEnd();
			//W druga strone
			glBegin(GL_TRIANGLES);
			glNormal3fv(pointsNorms[i][j]);
			glVertex3fv(pointsTab[i][j]);
			glNormal3fv(pointsNorms[i + 1][j]);
			glVertex3fv(pointsTab[i + 1][j]);
			glNormal3fv(pointsNorms[i][j + 1]);
			glVertex3fv(pointsTab[i][j + 1]);
			glEnd();
		}
	}
}

// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych
void Axes(void)
{
	point3  x_min = { -2.0, 0.0, 0.0 };
	point3  x_max = { 2.0, 0.0, 0.0 };
	// pocz�tek i koniec obrazu osi x

	point3  y_min = { 0.0, -2.0, 0.0 };
	point3  y_max = { 0.0,  2.0, 0.0 };
	// pocz�tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -2.0 };
	point3  z_max = { 0.0, 0.0,  2.0 };
	//  pocz�tek i koniec obrazu osi y

	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void light()
{

	// Definicja materia�u z jakiego zrobiony jest przedmiot
	//-------------------------------------------------------
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego                

	GLfloat mat_shininess = 100.0;
	// wsp�czynnik n opisuj�cy po�ysk powierzchni


	// Definicja �r�d�a �wiat�a
	//-------------------------------------------------------
	//GLfloat light_position[] = { 10.0, 0.0, 0.0, 0.0 };
	//GLfloat light_position2[] = { -10.0, 0.0, 0.0, 0.0 };
	// po�o�enie �r�d�a
	if (status == 1)
	{
		// je�li lewy klawisz myszy wci�ni�ty
		theta += -1*delta_x*pix2angle_x / 5;    // modyfikacja k?ta obrotu o k?t proporcjonalny
		fi += -1*delta_y*pix2angle_y / 5;
		light_position0[0] = 10 * cos(theta) * cos(fi);
		light_position0[1] = 10 * sin(fi);
		light_position0[2] = 10 * sin(theta) * cos(fi);
	}
	else if (status == 2)
	{
		// je�li lewy klawisz myszy wci�ni�ty
		theta2 += -1 * delta_x*pix2angle_x / 5;    // modyfikacja k?ta obrotu o k?t proporcjonalny
		fi2 += -1 * delta_y*pix2angle_y / 5;
		light_position1[0] = 10 * cos(theta2) * cos(fi2);
		light_position1[1] = 10 * sin(fi2);
		light_position1[2] = 10 * sin(theta2) * cos(fi2);
	}

	//Wspolrzedne obserwatora - wzorki z ZSK
	

	GLfloat light_ambient0[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 0.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse0[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular0[] = { 1.0, 0.5, 0.0, 0.6 };
	GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 0.6 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant0 = 1.0;
	GLfloat att_constant1 = 1.0;
	// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji 
	// odleg�o�ci od �r�d�a

	GLfloat att_linear0 = 0.05;
	GLfloat att_linear1 = 0.05;
	// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji 
	// odleg�o�ci od �r�d�a

	GLfloat att_quadratic0 = 0.001;
	GLfloat att_quadratic1 = 0.001;
	// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a


	// Ustawienie patrametr�w materia�u 
	//-------------------------------------------------------
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Ustawienie parametr�w �r�d�a �wiat�a
	//-------------------------------------------------------
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic0);



	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant1);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear1);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic1);

	// Ustawienie opcji systemu o�wietlania sceny 
	//-------------------------------------------------------
	glShadeModel(GL_SMOOTH); // w�aczenie �agodnego cieniowania
	glEnable(GL_LIGHTING);   // w�aczenie systemu o�wietlenia sceny 
	glEnable(GL_LIGHT0);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_LIGHT1);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_DEPTH_TEST); // w��czenie mechanizmu z-bufora 
}


// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana gdy trzeba 
// przerysowa� scen�)
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	glLoadIdentity();
	// Czyszczenie macierzy bie��cej

	light();
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0);
	// Zdefiniowanie po�o�enia obserwatora

	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powy�ej

	//Rotacje
	/*glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);*/

	//Renderowanie jajka
	Egg();

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania

	glutSwapBuffers();
}


//Funkcja callback dla obrotu
void spinEgg()
{

	angle[0] -= 0.5;
	if (angle[0] > 360.0) angle[0] -= 360.0;
	angle[1] -= 0.5;
	if (angle[1] > 360.0) angle[1] -= 360.0;
	angle[2] -= 0.5;
	if (angle[2] > 360.0) angle[2] -= 360.0;

	glutPostRedisplay(); //od�wie�enie zawarto�ci aktualnego okna
}

// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

}


// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych 
// w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� 
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle_x = 360.0*0.1 / (float)horizontal; // przeliczenie pikseli na stopnie
	pix2angle_y = 360.0*0.1 / (float)vertical;

	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bie��cej

	gluPerspective(70.0, 1.0, 1.0, 30.0);
	// Ustawienie parametr�w dla rzutu perspektywicznego

	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
	// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna

	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie��cej
}

// Funkcja "bada" stan myszy i ustawia wartosci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y) 
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;		// przypisanie aktualnie odczytanej pozycji kursora
							// jako pozycji poprzedniej
		status = 1;			//wci�ni�ty zosta� prawy klawisz myszy
	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old2 = x;		// przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old2 = y;
		// jako pozycji poprzedniej
		status = 2;			//wci�ni�ty zosta� prawy klawisz myszy
	}
	else
		status = 0;         // nie zosta� wci�ni�ty �aden klawisz
}

// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich 
// zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;    // obliczenie r�nicy po�o�enia kursora myszy
	x_pos_old = x;	// podstawienie bie�acego po�o�enia jako poprzednie

	delta_y = y - y_pos_old;	// obliczenie r�nicy po�o�enia kursora myszy
	y_pos_old = y;	// podstawienie bie�acego po�o�enia jako poprzednie

	glutPostRedisplay();	// przerysowanie obrazu sceny
}

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
void main(void)
{
	//Ziarno losowosci
	srand((unsigned)time(NULL));



	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Oswietlone, ruchome jajko");

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
	// (callback function).  Bedzie ona wywo�ywana za ka�dym razem 
	// gdy zajdzie potrzba przeryswania okna

	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn�
	// zazmiany rozmiaru okna  
	glutReshapeFunc(ChangeSize);

	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania 

	// W��czenie mechanizmu usuwania powierzchni niewidocznych
	glutMouseFunc(Mouse);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy

	//Rejestracja funkcji zwrotnej (obrot)
	glutIdleFunc(spinEgg);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}