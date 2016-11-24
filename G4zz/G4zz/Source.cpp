/*************************************************************************************/
//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi  
//  uk�adu wsp�rzednych
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

static float delta_x = 0, delta_y = 0, delta_z = 0;       // r�?nica pomi?dzy pozycj? bie??c?

float alfa = 0, beta = 0;
float alfaR = 0, betaR = 0;
double r = 0;

//--------------------------------------------

const int liczbaPodzialow = 41;
typedef float point3[3];
point3 tab[liczbaPodzialow + 1][liczbaPodzialow + 1];	//tablica w kt�rej znajduj� si� punkty na p�aszczy�nie


int u = 0, v = 1;
char model = 1;

point3 color = { 0.0, 0.0, 0.0 };
point3 color2 = { 1.0, 1.0, 1.0 };


/*****************************Funkcje jaja****************************************/
void pisz()			//wypisanie macie�y jako sprawdzenie
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
	// pocz�tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz�tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
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

void fillMatrix()		//wype�nienie maciezy punktami w 2D jako przygotowanie do transformacji na 3D
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

void transformMatrix()	//przeliczenie punkt�w wed�ug podanych wzor�w 
{
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
		}
	}
	//pisz(); //sprawdzenie pozycji punkt�w w macierzy 
}

void paint()
{

	glPointSize(1); // podajemy wielko�� punktu
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
			glBegin(GL_LINES); //tu uzupe�niem brakuj�ce elementy kt�rych nie dosrysowa�o przy tworzeniu jajka

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //poziom uzupe�nienie
			glVertex3fv(tab[liczbaPodzialow - i][0]);

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //pion uzupe�nienie 
			glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);

			glVertex3fv(tab[i][liczbaPodzialow - 1]); //poziom uzupe�nienie
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

				glVertex3fv(tab[i][j + 1]);
				glVertex3fv(tab[i + 1][j]);
				glVertex3fv(tab[i + 1][j + 1]);
				glEnd();

				//W druga strone
				glBegin(GL_TRIANGLES);
				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i + 1][j]);
				glVertex3fv(tab[i][j + 1]);
				glEnd();
			}
			glBegin(GL_TRIANGLES);
			if (i>i - 1 && i >= 0)
			{
				glVertex3fv(tab[i][liczbaPodzialow - 1]);
				glVertex3fv(tab[liczbaPodzialow - i][0]);
				glVertex3fv(tab[i + 1][liczbaPodzialow - 1]);

				glVertex3fv(tab[i][liczbaPodzialow - 1]);
				glVertex3fv(tab[liczbaPodzialow - i][0]);
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
// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana gdy trzeba 
// przerysowa� scen�)

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym

	glLoadIdentity();
	// Czyszczenie macierzy bie��cej
	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy�ej

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

	}                                 // do r�?nicy po?o?e? kursora myszy
	else if (status == 2)                    // je?li lewy klawisz myszy wci?ni?ty
	{
		viewer[2] += delta_z / 10;
	}

	//glRotated(20.0, 1.0, 0.0, 0.0);  // Obr�t o 60 stopni


	paint();

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania

	glutSwapBuffers();
	//
}

/*************************************************************************************/
// Funkcja ustalaj�ca stan renderowania

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny
	fillMatrix();
	transformMatrix();
}
/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych 
// w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� 
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2anglex = 360.0 / (float)horizontal; // przeliczenie pikseli na stopnie
	pix2angley = 360.0 / (float)vertical; // przeliczenie pikseli na stopnie
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okre�laj�cej proporcj�
	// wymiar�w okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji 
	glLoadIdentity();

	gluPerspective(100, 1.0, 1.0, 30.0);
	// Ustawienie parametr�w dla rzutu perspektywicznego

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
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli

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
	delta_x = x - x_pos_old;    // obliczenie r�?nicy po?o?enia kursora myszy
	x_pos_old = x;            // podstawienie bie?acego po?o?enia jako poprzednie
	delta_y = y - y_pos_old;    // obliczenie r�?nicy po?o?enia kursora myszy
	y_pos_old = y;            // podstawienie bie?acego po?o?enia jako poprzednie
	delta_z = x - z_pos_old;    // obliczenie r�?nicy po?o?enia kursora myszy
	z_pos_old = x;
	glutPostRedisplay();    // przerysowanie obrazu sceny
}

void main(void)
{



	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);

	glutCreateWindow("Uk�ad wsp�rz�dnych 3-D");

	glutKeyboardFunc(keys);


	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
	// (callback function).  Bedzie ona wywo�ywana za ka�dym razem 
	// gdy zajdzie potrzba przeryswania okna 


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn�
	// zazmiany rozmiaru okna      
	MyInit();



	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W��czenie mechanizmu usuwania powierzchni niewidocznych

	glutMouseFunc(Mouse);
	// Ustala funkcj? zwrotn? odpowiedzialn? za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcj? zwrotn? odpowiedzialn? za badanie ruchu myszy


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
/*************************************************************************************/




