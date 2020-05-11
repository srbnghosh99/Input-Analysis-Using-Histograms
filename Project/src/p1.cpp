#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cfloat>
#include<cmath>
#include<gl/glew.h>
#include<GL/glut.h>
#include"utility.h"
#define glWindoe2i GLEW_GET_FUN(_glewWindowPos2i)
using namespace std;
char* fileName1, *fileName2, *fileName3, *fileName4;
string currentFile;
float mean, standard_dev;
float* data_;
int numDataPoints;
float minimum, maximum;
float chi;
int numIntervals = 30;
float* endPoints;
float* prob;
float maxProb = -1;
int bfit = 0;
int curveType = 0;
int numCurvePoints = 100;
float *curveX = new float[numCurvePoints];
float *curveY = new float[numCurvePoints];

// Parameters
float mu = 2.2, sigma = 1;
float lamda = 1.25;
float parameterStep = 0.05;

//Drawing Parameters
int width = 800, height = 600;
float world_x_min, world_x_max, world_y_min, world_y_max;
float axis_x_min, axis_x_max, axis_y_min, axis_y_max;




void displayText(float x, float y, int r, int g, int b, const char *string) {

	int j = strlen(string);
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	//glWindowPos2f(x, y);
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	printf("Lange: %i - Raster: %f %f", j, x, y);

}
void display(void)
{
	glClearColor(0.169,0.087,0.179,1.00);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_LINES);
	
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(1);
	glBegin(GL_LINES);

	//Draw the x -axis
	glVertex2f(axis_x_min - (axis_x_min - world_x_min) / 2, 0);
	glVertex2f(axis_x_max, 0);

	//Draw the y-axis
	glVertex2f(axis_x_min, 0);                   	
	glVertex2f(axis_x_min, world_y_max);

	//Draw the maximum probability density mark
	glVertex2f(axis_x_min, maxProb);
	glVertex2f(axis_x_min + (maximum - minimum)*.015, maxProb);
	glEnd();

	stringstream ss;
	ss << maxProb;
	glRasterPos2f(axis_x_min + (maximum - minimum) * 0.02, maxProb);
	printString(ss.str());
	glRasterPos2f(maximum + (maximum - minimum) * 0.01, maxProb*0.02);
	printString("Data");
	glRasterPos2f(axis_x_min + (maximum - minimum) * 0.015, maxProb*1.1);
	printString("Probability Desnsity");

	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2f(maximum * 0.8, maxProb*1.1);
	printString("File: " + currentFile);
	glRasterPos2f(maximum * 0.8, maxProb*1.06);
	printString("Min: " + to_string(minimum));
	glRasterPos2f(maximum * 0.8, maxProb*1.02);
	printString("Max: " + to_string(maximum));
	glRasterPos2f(maximum * 0.8, maxProb - .01);
	printString("Number of Intervals: " + to_string(numIntervals));
	//Draw the probability histogram
	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(1.5f);
	for (int i = 1; i <= numIntervals; i++) {
		glRectf(endPoints[i], 0, endPoints[i + 1], prob[i]);
		
	}
	// draw theoretical distribution 

	glLineWidth(2);
	glColor3f(1.0, 0.11, 0.68);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i<numCurvePoints; i++)
		glVertex2f(curveX[i], curveY[i]);
	glEnd();
	if (curveType == 0) {
		displayText(maximum * 0.8, maxProb - .1, 1.0, 0.11, 0.68, "Distribution:Normal");
		ss.str("");
		displayText(maximum * 0.8, maxProb - .12, 1.0, 0.11, 0.68, "Mu: ");
		ss << mu;
		printString(ss.str());
		ss.str("");
		displayText(maximum * 0.8, maxProb - .14, 1.0, 0.11, 0.68, "Sigma: ");
		ss << sigma;
		printString(ss.str());
		ss.str("");
		///////////////////// Prints the best fit parameters for normal distribution  ///////////////////////////
		if (bfit == 1) {
			displayText(maximum * 0.8, maxProb - .20, 0.0, 1.0, 1.0, "Mean: ");
			ss.str("");
			ss << mean;
			printString(ss.str());
			ss.str("");
			displayText(maximum * 0.8, maxProb - .22, 0.0, 1.0, 1.0, "Standard Deviation: ");
			ss << standard_dev;
			printString(ss.str());
			ss.str("");
			displayText(maximum * 0.8, maxProb - .16, 0.0, 1.0, 1.0, "Best Fitted Curve ");
			displayText(maximum * 0.8, maxProb - .18, 0.0, 1.0, 1.0, "Expected Error:  ");
			ss.flush();
			ss.clear();
			
			ss << chi;
			
			printString(ss.str());	
		}

	}

	if (curveType == 1) {
		displayText(maximum * 0.8, maxProb - .1, 1.0, 0.0, 0.0, "Distribution: Exponential");
		ss.str("");
		displayText(maximum* 0.8, maxProb - .12, 1.0, 0.0, 0.0, "Lamda: ");
		ss << lamda;
		printString(ss.str());
		ss.str("");
		///////////////////// Prints the best fit parameters for exponential  distribution  ///////////////////////////
		if (bfit == 1){
			displayText(maximum* 0.8, maxProb - .18, 0.0, 1.0, 1.0, "Lamda: ");
			ss << mean;
			printString(ss.str());
			ss.str("");
			displayText(maximum * 0.8, maxProb - .14, 0.0, 1.0, 1.0, "Best Fitted Curve ");
		   displayText(maximum * 0.8, maxProb - .16, 0.0, 1.0, 1.0, "Expected Error:  ");
		   ss.flush();
		   ss.clear();
		   ss << chi;
		   printString(ss.str());
		}
	}
	glColor3f(1.0, 0.5, 0.0);
	glWindowPos2f(maximum * 0.8, maxProb - .12);
	glFlush();
	glutSwapBuffers();

}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


}
void computeNormalFunc(float mu, float sigma)
{
	// Compute the  curveX and curveY for exponential  distribution
	float x,a;
	x = (maximum - minimum) / (numCurvePoints - 1);
	for (int i = 0; i < numCurvePoints; i++) {
		a= minimum + x*i;
		curveX[i] = minimum + x*i;
		curveY[i] = (1.0 / (sigma * sqrt(2 * 3.1416))) * exp(-((a - mu) * (a - mu)) / (2 * sigma * sigma));
	}
}

void computeExponentialFunc(float lamda)
{
	// Compute the  curveX and curveY for normal distribution
	float y,b;
	y= (maximum - minimum) / (numCurvePoints - 1);
	for (int i = 0; i<numCurvePoints; i++)
	{
		
		b = minimum + y*i;
		curveX[i] = minimum + y*i;
		curveY[i] = (1.0 / lamda) * exp(-(b/ lamda));

	}

}

void computeProbability(int numIntervals)
{
	// Compute the  histogram and update window size  according to the histogram size

	if (endPoints = NULL)
		delete endPoints;
	if (prob = NULL)
		delete prob;
	endPoints = new float[numIntervals + 1];
	maxProb = -1;
	prob = new float[numIntervals];
	for (int i = 1; i <= numIntervals; i++)
	{
		endPoints[i] = minimum + ((maximum - minimum) / numIntervals) * i;
	}

	float histSize = (maximum - minimum) / numIntervals;
	
	for (int j = 1; j <= numIntervals; j++)
	{

		prob[j] = 0;
		for (int k = 0; k <= numDataPoints; k++)
		{
			if (data_[k] >= endPoints[j] && data_[k] < endPoints[j + 1])
			{
				prob[j]++;
			}
		}
		prob[j] = prob[j] / numDataPoints;  
		prob[j] = prob[j] / histSize;        // calculates height of each bar of the histogram
		if (prob[j] > maxProb)
			maxProb = prob[j];				// update the maximum  probability from the dataset 
		
	}
	world_y_max = maxProb *1.4 ;
	world_y_min = -maxProb * 0.04;

}


void readFile(string fileName)
{
	ifstream inFile(fileName);
	if (!inFile.is_open())
	{
		cout << fileName << "could't be opened.\n";
		system("pause");
		exit(1);
	}
	currentFile = fileName;
	inFile >> numDataPoints;
	if (data_ != NULL)
		delete data_;
	data_ = new float[numDataPoints];

	minimum = FLT_MAX;
	maximum = -FLT_MAX;
	float sum = 0;
	for (int i = 0; i < numDataPoints; i++)
	{
		///Calculates the maximum and minimum value of the data set
		sum = sum + data_[i];
		inFile >> data_[i];
		if (data_[i] < minimum)      
			minimum = data_[i];
		if (data_[i] > maximum)
			maximum = data_[i];


	}
	// computes the world variables
	world_x_max = (maximum + (maximum - minimum) *0.2f);
	world_x_min = (minimum - (maximum - minimum) *0.1f);
	axis_x_max = (maximum + (maximum - minimum) *0.15f);
	axis_x_min = (minimum - (maximum - minimum) *0.03f);

	// calls these function from readfile//
	computeProbability(numIntervals);
	if (curveType == 0)
		computeNormalFunc(mu, sigma);
	else if (curveType == 1)
		computeExponentialFunc(lamda);


}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q' || key == 27)
		exit(0);
}

void specialKey(int key, int x, int y)
{
	if (curveType == 0)
	{
		if (key == GLUT_KEY_RIGHT)
		{
			mu = mu + parameterStep;
			if (mu >= 5.0) 
				mu = 5.00;          //	keeping the value of mu not more than five
		}

		if (key == GLUT_KEY_LEFT)
		{
			mu = mu - parameterStep;
		
			if (mu < 0.0) 
				mu = 0.0;            	//	keeping the value of mu not leass than zero
		}

		if (key == GLUT_KEY_UP)
		{
			sigma = sigma + parameterStep;
			
			if (sigma >= 3.0) 
				sigma = 3.00;             //	keeping the value of sigma is less than 3.0
		}

		if (key == GLUT_KEY_DOWN)
		{
			sigma = sigma - parameterStep;
			
			if (sigma <= 0.02)
				sigma = 0.02;             //	keeping the value of sigma is greater than 0.02
		}
		//	call to Compute the Normal distribution function
		computeNormalFunc(mu, sigma);
	}
	else if (curveType == 1)
	{
		if (key == GLUT_KEY_RIGHT)
		{
			lamda = lamda + parameterStep;
			
			if (lamda >= 6.0) 
				lamda = 6.00;                   //	keeping the value of beta is lesser than 6.0
		}

		if (key == GLUT_KEY_LEFT)
		{
			lamda = lamda - parameterStep;
			 
			if (lamda <= 0.1) 
				lamda = 0.1;                      //	keeping the value of beta is greater than 0.1
		}
		//	Call to compute the Exponential distribution function
		computeExponentialFunc(lamda);
	}

	glutPostRedisplay();
	
}
void bestFit()
{
	bfit = 1; 
	float sum = 0.0;
	for (int i = 0; i < numDataPoints; i++)
	{
		sum = sum + data_[i];
	}
	mean = sum / numDataPoints;          //calculate the mean value of the real data
	sum = 0.0;
	for (int i = 0; i < numDataPoints; i++)
	{
		sum = sum + ((data_[i] - mean)*(data_[i] - mean));
	}
	standard_dev = sqrt(sum / (numDataPoints - 1));        //calculate the standard deviation  of the real data

	// call these funcitons using the current mean and standard deviation
	if (curveType == 0)
		computeNormalFunc(mean, standard_dev);         
	if (curveType == 1)
		computeExponentialFunc(mean);
	glutPostRedisplay();
}

void chisquaretest()
{
	//  calculates the error between the best fit curve and the common  curve , the bigger the chi value  the more error is there
	float suma = 0.0;
	
	for (int i = 0; i < numIntervals; i++)
	
		suma = suma+ (((prob[i] - endPoints[i])*(prob[i] - endPoints[i]))/ endPoints[i]);
	
	chi = suma;
}
void topMenuFunc(int id)
{

	switch (id)
	{
	case 1:
		bestFit();
		chisquaretest();
		break;
	case 2:
		exit(0);
		break;

	}
}
void fileMenuFunction(int id)
{
	// choosing the file from the four options
	bfit = 0;
	switch (id)
	{
	case 1:
		currentFile = fileName1;
		curveType = 0;                // curveType is normal distribution
		break;
	case 2:
		currentFile = fileName2;
		curveType = 1;                // curveType is exponential  distribution
		break;
	case 3:
		currentFile = fileName3;
		curveType = 0;
		break;
	case 4:
		currentFile = fileName4;
		curveType = 1;
		break;

	}
	readFile(currentFile);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);
	glutPostRedisplay();

}
void FuncMenuFunction(int id)
{
	// choosing the distribution curve for the corresponding histogram from the two  options
	switch (id)
	{
	case 1:
		
		computeNormalFunc(mu, sigma);
		curveType = 0;
		break;
	case 2:
	
		computeExponentialFunc(lamda);
		curveType = 1;
	}
	readFile(currentFile);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);
	glutPostRedisplay();
}

void histogramMenuFunction(int id)

{
	// choosing the number of bars need to draw the histogram 
	switch (id)
	{
	case 1:
		numIntervals = 30;

		break;
	case 2:
		numIntervals = 40;
		break;
	case 3:
		numIntervals = 50;
		break;


	}
	glutPostRedisplay();
	computeProbability(numIntervals);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);
	glutPostRedisplay();
}

void parameterStepMeuFunction(int id)
{
	// choosing the parameter step value  
	switch (id)
	{
	case 1:
		parameterStep = .01;
		break;
	case 2:
		parameterStep = .02;
		break;
	case 3:
		parameterStep = .05;
		break;


	}
	glutPostRedisplay();

}


void createMenu()
{
	// creating sub menus
	int file_option = glutCreateMenu(fileMenuFunction);
	glutAddMenuEntry(fileName1, 1);
	glutAddMenuEntry(fileName2, 2);
	glutAddMenuEntry(fileName3, 3);
	glutAddMenuEntry(fileName4, 4);


	int param_option = glutCreateMenu(parameterStepMeuFunction);
	glutAddMenuEntry("0.01", 1);
	glutAddMenuEntry("0.02", 2);
	glutAddMenuEntry("0.05", 3);


	int histogram_option = glutCreateMenu(histogramMenuFunction);
	glutAddMenuEntry("30", 1);
	glutAddMenuEntry("40", 2);
	glutAddMenuEntry("50", 3);


	int distribution_option = glutCreateMenu(FuncMenuFunction);
	glutAddMenuEntry("Normal", 1);
	glutAddMenuEntry("Exponential", 2);



	int main_option = glutCreateMenu(topMenuFunc);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddSubMenu("File", file_option);
	glutAddSubMenu("Distribution", distribution_option);
	glutAddSubMenu("Histogram", histogram_option);
	glutAddSubMenu("Parameter step", param_option);
	glutAddMenuEntry("Best Fit", 1);
	glutAddMenuEntry("Exit", 2);
	

}

void reshape(int w, int h)
{

	width = w, height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);

}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Input Analysis");
	fileName1 = "normal.dat";
	fileName2 = "expo.dat";
	fileName3 = "10.dat";
	fileName4 = "18.dat";
	currentFile = "normal.dat";
	readFile(currentFile);
	glewInit();
	init();
	createMenu();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);

	glutMainLoop();
	return 0;
}