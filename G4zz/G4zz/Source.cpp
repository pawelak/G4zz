/*************************************************************************************/
//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi  
//  uk³adu wspó³rzednych
/*************************************************************************************/

#define _USE_MATH_DEFINES

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <vector>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <iomanip>

using namespace std;

//do myszy-----------------------------------
typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };


static GLfloat thetax = 0.0;   // k?t obrotu obiektu
static GLfloat thetay = 0.0;   // k?t obrotu obiektu
static GLfloat thetaz = 0.0;   // k?t obrotu obiektu

static GLfloat pix2anglex;     // przelicznik pikseli na stopnie
static GLfloat pix2angley;
static GLfloat pix2anglez;
static GLint status = 0;      // stan klawiszy myszy 
static int x_pos_old = 0, y_pos_old = 0, z_pos_old = 0;       // poprzednia pozycja kursora myszy

static float delta_x = 0, delta_y = 0, delta_z = 0;       // ró?nica pomi?dzy pozycj? bie??c?

float alfa = 0, beta = 0;
float alfaR = 0, betaR = 0;
double r = 0;
int test;


//--------------------------------------------

const int liczbaPodzialow = 41;
typedef float point3[3];
point3 tab[liczbaPodzialow + 1][liczbaPodzialow + 1];	//tablica w której znajduj¹ siê punkty na p³aszczyŸnie
point3 tabNormalVector[liczbaPodzialow + 1][liczbaPodzialow + 1];

int u = 0, v = 1;
char model = 1;

point3 color = { 0.0, 0.0, 0.0 };
point3 color2 = { 1.0, 1.0, 1.0 };


/*****************************Funkcje jaja****************************************/
void pisz()			//wypisanie macie¿y jako sprawdzenie
{
	for (int i = 0; i < liczbaPodzialow; i++)
	{
		for (int j = 0; j < liczbaPodzialow; j++)
		{
			cout << setw(5) << setprecision(3) << tab[i][j][0] << " ";
			cout << setw(5) << setprecision(3) << tab[i][j][1] << " ";
			cout << setw(5) << setprecision(3) << tab[i][j][2] << " ";
			cout << " | " << endl;
		}
		cout << endl << endl;
	}

}

void Axes(void)		//rysowanie osi 
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz¹tek i koniec obrazu osi y
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

void fillMatrix()		//wype³nienie maciezy punktami w 2D jako przygotowanie do transformacji na 3D
{
	for (int i = 0; i < liczbaPodzialow; i++)
	{
		for (int j = 0; j < liczbaPodzialow; j++)
		{
			tab[i][j][0] = ((float)1 / (float)liczbaPodzialow)*i;
			tab[i][j][1] = ((float)1 / (float)liczbaPodzialow)*j;
			tab[i][j][2] = 0;
		}
	}
	cout << endl << endl;
}

void transformMatrix()	//przeliczenie punktów wed³ug podanych wzorów 
{
	float xu, xv, yu, yv, zu, zv;
	int i = 0, j = 0;
	float uL;
	for (i = 0; i < liczbaPodzialow; i++)
	{
		for (j = 0; j < liczbaPodzialow; j++)
		{
			if (j == 0)uL = tab[i][j][0];
			float vL = tab[i][j][1];

			tab[i][j][0] = (-90 * pow(uL, 5) + (225 * pow(uL, 4)) - (270 * pow(uL, 3)) + (180 * pow(uL, 2)) - (45 * uL))*cos(M_PI*vL);
			tab[i][j][1] = 160 * pow(uL, 4) - 320 * pow(uL, 3) + 160 * pow(uL, 2);
			tab[i][j][2] = (-90 * pow(uL, 5) + (225 * pow(uL, 4)) - (270 * pow(uL, 3)) + (180 * pow(uL, 2)) - (45 * uL))*sin(M_PI*vL);
			
			xu = (-450 * pow(uL, 4) + 900 * pow(uL, 3) - 810 * pow(uL, 2) + 360 * uL - 45)*cos(M_PI*vL);
			xv = M_PI*(90 * pow(uL, 5) - 225 * pow(uL, 4) + 270 * pow(uL, 3) - 180 * pow(uL, 2) + 45 * uL)*sin(M_PI*vL);
			yu = 640 * pow(uL, 3) - 960 * pow(uL, 2) + 320 * uL;
			yv = 0;
			zu = (-450 * pow(uL, 4) + 900 * pow(uL, 3) - 810 * pow(uL, 2) + 360 * uL - 45)*sin(M_PI*vL);
			zv = -M_PI*(90 * pow(uL, 5) - 225 * pow(uL, 4) + 270 * pow(uL, 3) - 180 * pow(uL, 2) + 45 * uL)*cos(M_PI*vL);
			
			tabNormalVector[i][j][0] = -1.0 * (yu*zv - zu*yv);
			tabNormalVector[i][j][1] = -1.0 * zu*xv - xu*zv;
			tabNormalVector[i][j][2] =  xu*yv - yu*xv;



			
		}
	}
	//pisz(); //sprawdzenie pozycji punktów w macierzy 
}

void paint()
{

	glPointSize(1); // podajemy wielkoœæ punktu
	switch (model)
	{
	case '1':
	{
		glColor3fv(color2);
		for (int i = 0; i < liczbaPodzialow; i++)
		{

			for (int j = 0; j < liczbaPodzialow; j++)
			{
				glBegin(GL_POINTS);
				glVertex3fv(tab[i][j]);
				glNormal3fv(tabNormalVector[i][j]);
				glEnd();
			}
		}
		break;
	}
	case '2':
	{

		for (int i = 0; i < liczbaPodzialow; i++)
		{
			glColor3f(1, 0, 0);
			for (int j = 0; j < liczbaPodzialow - 1; j++)
			{
				glBegin(GL_LINES); //poziom
				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i][j + 1]);

				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i + 1][j]);

				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i + 1][j + 1]);

				glEnd();

			}
			glBegin(GL_LINES); //tu uzupe³niem brakuj¹ce elementy których nie dosrysowa³o przy tworzeniu jajka

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //poziom uzupe³nienie
			glVertex3fv(tab[liczbaPodzialow - i][0]);

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //pion uzupe³nienie 
			glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //poziom uzupe³nienie
			glVertex3fv(tab[liczbaPodzialow - i - 1][0]);


			glEnd();

		}


		break;
	}
	case '3':
	{
		glColor3fv(color);
		for (int i = 0; i < liczbaPodzialow; i++)
		{
			for (int j = 0; j < liczbaPodzialow; j++)
			{
				//W jedna strone
				glBegin(GL_TRIANGLES);
				

				glNormal3fv(tabNormalVector[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				glNormal3fv(tabNormalVector[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glNormal3fv(tabNormalVector[i + 1][j + 1]);
				glVertex3fv(tab[i + 1][j + 1]);

				
				glEnd();

				//W druga strone
				glBegin(GL_TRIANGLES);
				glNormal3fv(tabNormalVector[i][j]);
				glVertex3fv(tab[i][j]);

				glNormal3fv(tabNormalVector[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glNormal3fv(tabNormalVector[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				
				
				
				glEnd();
			}
			glBegin(GL_TRIANGLES);
			if (i>i - 1 && i >= 0)
			{
				glNormal3fv(tabNormalVector[i][liczbaPodzialow - 1]);
				glVertex3fv(tab[i][liczbaPodzialow - 1]);

				glNormal3fv(tabNormalVector[liczbaPodzialow - i][0]);
				glVertex3fv(tab[liczbaPodzialow - i][0]);

				glNormal3fv(tabNormalVector[i + 1][liczbaPodzialow - 1]);
				glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);

				glNormal3fv(tabNormalVector[i][liczbaPodzialow - 1]);
				glVertex3fv(tab[i][liczbaPodzialow - 1]);

				glNormal3fv(tabNormalVector[liczbaPodzialow - i][0]);
				glVertex3fv(tab[liczbaPodzialow - i][0]);

				glNormal3fv(tabNormalVector[liczbaPodzialow - i + 1][0]);
				glVertex3fv(tab[liczbaPodzialow - i + 1][0]);

			}
			glEnd();
		}
		break;
	}
	case '4':
	{
		glColor3fv(color);
		int i, j;
		float help1, help2;
		for (i = 0; i < liczbaPodzialow; i++)
		{
			for (j = 0; j < liczbaPodzialow; j++)
			{

				if (i > liczbaPodzialow / 2)help1 = (float)(liczbaPodzialow - i) / liczbaPodzialow;
				else help1 = (float)i / liczbaPodzialow;
				if (j > liczbaPodzialow / 2)help2 = (float)(liczbaPodzialow - j) / liczbaPodzialow;
				else help2 = (float)j / liczbaPodzialow;
				//glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
				glColor3f(0, help2, help1);
				glBegin(GL_POLYGON);
				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i][j + 1]);
				glVertex3fv(tab[i + 1][j + 1]);
				glVertex3fv(tab[i + 1][j]);
				glEnd();
			}

			glBegin(GL_POLYGON);

			glColor3f(0, help2, help1);
			glVertex3fv(tab[i][liczbaPodzialow - 1]);
			glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);
			glVertex3fv(tab[liczbaPodzialow - i - 1][0]);
			glVertex3fv(tab[liczbaPodzialow - i][0]);
			glEnd();

		}
		break;
	}
	case '5':
	{
		{
			srand(time(NULL));
			for (int i = 0; i < liczbaPodzialow; i++)
			{
				for (int j = 0; j < liczbaPodzialow; j++)
				{
					//W jedna strone
					glBegin(GL_TRIANGLES);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i][j + 1]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i + 1][j]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i + 1][j + 1]);
					glEnd();

					//W druga strone
					glBegin(GL_TRIANGLES);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i][j]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i + 1][j]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i][j + 1]);
					glEnd();




				}
				glBegin(GL_TRIANGLES);
				//glColor3f(1.0, 0, 0);
				if (i>i - 1 && i >= 0)
				{
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i][liczbaPodzialow - 1]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[liczbaPodzialow - i][0]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);

					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[i][liczbaPodzialow - 1]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[liczbaPodzialow - i][0]);
					glColor3f(((rand() % 100)*0.01), ((rand() % 100)*0.01), ((rand() % 100)*0.01));
					glVertex3fv(tab[liczbaPodzialow - i + 1][0]);
				}


				glEnd();

				//glBegin(GL_LINE);
				//glColor3f(0, 0, 1.0);
				//glVertex3fv(tab[i][liczbaPodzialow - 1]); //poziom
				//glVertex3fv(tab[liczbaPodzialow - i][0]);
				//glEnd();
			}
			break;
		}
	}
	case '6':
	{
		glColor3fv(color);
		for (int i = 0; i < liczbaPodzialow; i++)
		{
			for (int j = 0; j < liczbaPodzialow; j++)
			{
				int r = rand() % +3;
				color[r] = (rand() % 100)*0.01;
				glBegin(GL_POLYGON);
				glColor3fv(color);
				glVertex3fv(tab[i][j]);
				glColor3fv(color);
				glVertex3fv(tab[i][j + 1]);
				glColor3fv(color);
				glVertex3fv(tab[i + 1][j + 1]);
				glColor3fv(color);
				glVertex3fv(tab[i + 1][j]);
				glEnd();
			}
		}
		break;
	}


	default:
		break;
	}



}


/*************************************************************************************/
// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba 
// przerysowaæ scenê)

void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej
	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	Axes();
	glTranslated(0, -5, 0);

	if (status == 1)                    // je?li lewy klawisz myszy wci?ni?ty
	{
		r = sqrt(pow(viewer[0], 2) + pow(viewer[1], 2) + pow(viewer[2], 2));
		alfa += delta_x*pix2anglex / 50;    // modyfikacja k?ta obrotu o k?t proporcjonalny
		beta += delta_y*pix2angley / 50;
		alfaR = (alfa * M_PI) / 180.0f;
		betaR = (beta * M_PI) / 180.0f;
		if (alfaR > 2 * M_PI)alfaR -= 2 * M_PI;
		if (betaR > 2 * M_PI)betaR -= 2 * M_PI;
		cout << delta_x << " " << delta_y << endl;
		cout << alfa << " " << beta << endl;

		viewer[0] = (r * cos(alfa) * cos(beta));
		viewer[1] = (r*sin(beta));
		viewer[2] = (r * sin(alfa) * cos(beta));

	}                                 // do ró?nicy po?o?e? kursora myszy
	else if (status == 2)                    // je?li lewy klawisz myszy wci?ni?ty
	{
		viewer[2] += delta_z / 10;
	}
	

	//glRotated(20.0, 1.0, 0.0, 0.0);  // Obrót o 60 stopni


	paint();

	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania

	glutSwapBuffers();
	//
}


/*************************************************************************************/
// Funkcja ustalaj¹ca stan renderowania

void nVector()
{

}

void MyInit(void)
{
	
	/*************************************************************************************/
	//  Definicja materia³u z jakiego zrobiony jest czajnik 
	//  i definicja Ÿród³a œwiat³a
	/*************************************************************************************/

	/*************************************************************************************/
	// Definicja materia³u z jakiego zrobiony jest czajnik 

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego                

	GLfloat mat_shininess = { 20.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni

	/*************************************************************************************/
	// Definicja Ÿród³a œwiat³a

	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	// po³o¿enie Ÿród³a

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant =  1.0 ;
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji 
	// odleg³oœci od Ÿród³a

	GLfloat att_linear =  0.05 ;
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji 
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic =  0.001 ;
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	/*************************************************************************************/
	// Ustawienie parametrów materia³u i Ÿród³a œwiat³a
	/*************************************************************************************/
	// Ustawienie patrametrów materia³u

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametrów Ÿród³a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
	// Ustawienie opcji systemu oœwietlania sceny 

	glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny 
	glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora 

	
	
}
/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych 
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ 
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2anglex = 360.0 / (float)horizontal; // przeliczenie pikseli na stopnie
	pix2angley = 360.0 / (float)vertical; // przeliczenie pikseli na stopnie

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okreœlaj¹cej proporcjê
	// wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji 
	glLoadIdentity();

	gluPerspective(100, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego

	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielko?ci okna okna widoku (viewport) w zale?no?ci
	// relacji pomi?dzy wysoko?ci? i szeroko?ci? okna

	glMatrixMode(GL_MODELVIEW);
	// Prze??czenie macierzy bie??cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej 
}
/*************************************************************************************/
// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli

void keys(unsigned char key, int x, int y)
{
	if (key == '1') model = '1';
	if (key == '2') model = '2';
	if (key == '3') model = '3';
	if (key == '4') model = '4';
	if (key == '5') model = '5';
	if (key == '6') model = '6';
	if (key == 'r')
	{
		color[0] += 0.05;
		if (color[0] > 1)color[0] = 0;
	}
	if (key == 'g')
	{
		color[1] += 0.05;
		if (color[1] > 1)color[1] = 0;
	}
	if (key == 'b')
	{
		color[2] += 0.05;
		if (color[2] > 1)color[2] = 0;
	}


	RenderScene(); // przerysowanie obrazu sceny
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora 
							  // jako pozycji poprzedniej
		status = 1;         // wci?ni?ty zosta? lewy klawisz myszy
	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		y_pos_old = y;        // przypisanie aktualnie odczytanej pozycji kursora 
							  // jako pozycji poprzedniej
		status = 1;         // wci?ni?ty zosta? lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		z_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora 
							  // jako pozycji poprzedniej
		status = 2;         // wci?ni?ty zosta? lewy klawisz myszy
	}
	else
		status = 0;         // nie zosta? wci?ni?ty ?aden klawisz 
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;    // obliczenie ró?nicy po?o?enia kursora myszy
	x_pos_old = x;            // podstawienie bie?acego po?o?enia jako poprzednie
	delta_y = y - y_pos_old;    // obliczenie ró?nicy po?o?enia kursora myszy
	y_pos_old = y;            // podstawienie bie?acego po?o?enia jako poprzednie
	delta_z = x - z_pos_old;    // obliczenie ró?nicy po?o?enia kursora myszy
	z_pos_old = x;
	glutPostRedisplay();    // przerysowanie obrazu sceny
}

void main(void)
{
	fillMatrix();
	transformMatrix();


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);

	glutCreateWindow("Uk³ad wspó³rzêdnych 3-D");

	glutKeyboardFunc(keys);


	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
	// (callback function).  Bedzie ona wywo³ywana za ka¿dym razem 
	// gdy zajdzie potrzba przeryswania okna 


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
	// zazmiany rozmiaru okna      
	MyInit();



	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst¹pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych

	glutMouseFunc(Mouse);
	// Ustala funkcj? zwrotn? odpowiedzialn? za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcj? zwrotn? odpowiedzialn? za badanie ruchu myszy


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
/*************************************************************************************/




