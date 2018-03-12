#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int negativo(int iMin, int iMax, int I)
{
	if(I<iMin)
	{
		return iMax;
	}
	else if(iMin <= I && I < iMax)
	{
		return ((iMin-iMax)/(iMax-iMin))*(I-iMin) + iMax;
	}
	else if(I >= iMax)
	{
		return iMin;
	}
	else
		printf("Não entrou em if\n");
	return 0;
}

void filtroNegativo (Mat image)
{
	Mat imageNegative = image.clone();

	int bMin = 300, bMax = -1, gMin = 300, gMax = -1, rMin = 300, rMax = -1; 

    for (int y=0; y < imageNegative.rows; y++)
    	for (int x=0; x < imageNegative.cols; x++) 
		{
			Vec3b intensity = imageNegative.at<Vec3b>(y, x);
			if(bMin > (int)(intensity.val[0]))
				bMin = (int)(intensity.val[0]);
			else if(bMax < (int)(intensity.val[0]))
				bMax = (int)(intensity.val[0]);
			if(gMin > (int)(intensity.val[1]))
				gMin = (int)(intensity.val[1]);
			else if(gMax < (int)(intensity.val[1]))
				gMax = (int)(intensity.val[1]);
			if(rMin > (int)(intensity.val[2]))
				rMin = (int)(intensity.val[2]);
			else if(rMax < (int)(intensity.val[2]))
				rMax = (int)(intensity.val[2]);			
		}

	for (int y=0; y < imageNegative.rows; y++)
    	for (int x=0; x < imageNegative.cols; x++) 
		{
			Vec3b intensity = imageNegative.at<Vec3b>(y, x);
			uchar blue = negativo(bMin, bMax, ((int)(intensity.val[0])));
			uchar green = negativo(gMin, gMax, ((int)(intensity.val[1])));
			uchar red = negativo(rMin, rMax, ((int)(intensity.val[2])));
			imageNegative.at<Vec3b>(y,x) = Vec3b(blue,green,red);
		}
	
	namedWindow("Display Processed Image Saturated", WINDOW_AUTOSIZE );
    imshow("Display Processed Image Saturated", imageNegative);

    return;
}

void filtroSaturacao (Mat image)
{
	Mat imageSaturated = image.clone();
	//Declaracao dos histogramas
	double histImg1blue[256] = {0}; double histImg1green[256] = {0}; double histImg1red[256] = {0};
	int bMin = 300, bMax = -1, gMin = 300, gMax = -1, rMin = 300, rMax = -1; 

	//Preenchimento dos histogramas
    for (int y=0; y < imageSaturated.rows; y++)
    	for (int x=0; x < imageSaturated.cols; x++) 
		{
			Vec3b intensity = imageSaturated.at<Vec3b>(y, x);
			histImg1blue[(int)(intensity.val[0])]++;
			histImg1green[(int)(intensity.val[1])]++;
			histImg1red[(int)(intensity.val[2])]++;

			if(bMin > (int)(intensity.val[0]))
				bMin = (int)(intensity.val[0]);
			else if(bMax < (int)(intensity.val[0]))
				bMax = (int)(intensity.val[0]);
			if(gMin > (int)(intensity.val[1]))
				gMin = (int)(intensity.val[1]);
			else if(gMax < (int)(intensity.val[1]))
				gMax = (int)(intensity.val[1]);
			if(rMin > (int)(intensity.val[2]))
				rMin = (int)(intensity.val[2]);
			else if(rMax < (int)(intensity.val[2]))
				rMax = (int)(intensity.val[2]);			
		}

	//Impressão dos histogramas obtidos
	/*
	for(int i=0; i<256; i++)
	{	
		printf("%d: b: %lf, g: %lf, r: %lf\n", i, histImg1blue[i], histImg1green[i], histImg1red[i]);
	}
	*/

	//Normalização dos histogramas
	for(int i=0; i<256; i++)
	{	
		histImg1blue[i] /= imageSaturated.rows * imageSaturated.cols;
		histImg1green[i] /= imageSaturated.rows * imageSaturated.cols;
		histImg1red[i] /= imageSaturated.rows * imageSaturated.cols;
	}

	//Criação dos histogramas de valores acumulados
	double acumHistImg1blue[256] = {0}, acumHistImg1green[256] = {0}, acumHistImg1red[256] = {0};
	acumHistImg1blue[0] = histImg1blue[0];
	acumHistImg1green[0] = histImg1green[0];
	acumHistImg1red[0] = histImg1red[0];	

	//Preenchimendo dos histogramas de valores acumulados
	for(int i=1; i<256; i++)
	{	
		acumHistImg1blue[i] = acumHistImg1blue[i-1] + histImg1blue[i];
		acumHistImg1green[i] = acumHistImg1green[i-1] + histImg1green[i];
		acumHistImg1red[i] = acumHistImg1red[i-1] + histImg1red[i];
		
		//Impressão dos histogramas acumulados		
		//printf("%d: b: %lf, g: %lf, r: %lf\n", i, acumHistImg1blue[i], acumHistImg1green[i], acumHistImg1red[i]);
	}

	for (int y=0; y < imageSaturated.rows; y++)
    	for (int x=0; x < imageSaturated.cols; x++) 
		{
			Vec3b intensity = imageSaturated.at<Vec3b>(y, x);
			uchar blue = acumHistImg1blue[intensity.val[0]]*255;
			uchar green = acumHistImg1green[intensity.val[1]]*255;
			uchar red = acumHistImg1red[intensity.val[2]]*255;
			imageSaturated.at<Vec3b>(y,x) = Vec3b(blue,green,red);
		}

	namedWindow("Display Processed Image Saturated", WINDOW_AUTOSIZE );
    imshow("Display Processed Image Saturated", imageSaturated);

    return;
}

void filtroMediana (Mat image)
{
	Mat imageGray = image.clone();
	//Função que passa a imagem para tons de cinza
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	namedWindow("Display Gray Image 2", WINDOW_AUTOSIZE );
    imshow("Display Gray Image 2", imageGray);


	Mat imageMean = imageGray.clone();

	//	Kernel já normalizado
	double kernel[3][3] = {	1.0/16, 2.0/16, 1.0/16, 
							2.0/16, 4.0/16, 2.0/16, 
							1.0/16, 2.0/16, 1.0/16};
	
	for (int x=1; x < imageMean.rows; x++)
    	for (int y=1; y < imageMean.cols; y++)
   		{
   			imageMean.at<uchar>(x,y)=0;
   			for(int i=-1; i<=1; i++)
   				for(int j=-1; j<=1; j++)
   					imageMean.at<uchar>(x,y) += imageGray.at<uchar>(x+i,y+i)*kernel[i+1][j+1];	
   		}

   	namedWindow("Display Processed Mean Image", WINDOW_AUTOSIZE );
    imshow("Display Processed Mean Image", imageMean);

	return;
}


int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image 1", WINDOW_AUTOSIZE );
    imshow("Display Image 1", image);

	double histImg1blue[256];
	double histImg1green[256];
	double histImg1red[256];
	int bMin = 300, bMax = -1, gMin = 300, gMax = -1, rMin = 300, rMax = -1; 

	for(int i=0; i<256; i++)
	{
		histImg1blue[i] = 0;
		histImg1green[i] = 0;
		histImg1red[i] = 0;
	}

    for (int y=0; y < image.rows; y++)
    	for (int x=0; x < image.cols; x++) 
	{
		Vec3b intensity = image.at<Vec3b>(y, x);
		histImg1blue[(int)(intensity.val[0])]++;
		histImg1green[(int)(intensity.val[1])]++;
		histImg1red[(int)(intensity.val[2])]++;

		if(bMin > (int)(intensity.val[0]))
			bMin = (int)(intensity.val[0]);
		else if(bMax < (int)(intensity.val[0]))
			bMax = (int)(intensity.val[0]);
		if(gMin > (int)(intensity.val[1]))
			gMin = (int)(intensity.val[1]);
		else if(gMax < (int)(intensity.val[1]))
			gMax = (int)(intensity.val[1]);
		if(rMin > (int)(intensity.val[2]))
			rMin = (int)(intensity.val[2]);
		else if(rMax < (int)(intensity.val[2]))
			rMax = (int)(intensity.val[2]);			
	}
	//Mat histPlot = cvCreateMat(256, 500, CV_8UC1);

	for(int i=0; i<256; i++)
	{	
		//int mag = histImg1blue[i];
		//line(histPlot,Point(i,0),Point(i,mag),Scalar(255,0,0));

		printf("%d: b: %lf, g: %lf, r: %lf\n", i, histImg1blue[i], histImg1green[i], histImg1red[i]);
	}

	for(int i=0; i<256; i++)
	{	
		histImg1blue[i] /= image.rows * image.cols;
		histImg1green[i] /= image.rows * image.cols;
		histImg1red[i] /= image.rows * image.cols;
	}

	double acumHistImg1blue[256] = {0}, acumHistImg1green[256] = {0}, acumHistImg1red[256] = {0};
	acumHistImg1blue[0] = histImg1blue[0];
	acumHistImg1green[0] = histImg1green[0];
	acumHistImg1red[0] = histImg1red[0];	
	for(int i=1; i<256; i++)
	{	
		acumHistImg1blue[i] = acumHistImg1blue[i-1] + histImg1blue[i];
		acumHistImg1green[i] = acumHistImg1green[i-1] + histImg1green[i];
		acumHistImg1red[i] = acumHistImg1red[i-1] + histImg1red[i];
				
		printf("%d: b: %lf, g: %lf, r: %lf\n", i, acumHistImg1blue[i], acumHistImg1green[i], acumHistImg1red[i]);
	}

	for (int y=0; y < image.rows; y++)
    	for (int x=0; x < image.cols; x++) 
	{
		//Vec3b intensity = image.at<Vec3b>(y, x);
		

		///////////////////////////		IMAGEM EQUALIZADA		///////////////////////////
		/*
		uchar blue = acumHistImg1blue[intensity.val[0]]*255;
		uchar green = acumHistImg1green[intensity.val[1]]*255;
		uchar red = acumHistImg1red[intensity.val[2]]*255;
		*/
		///////////////////////////////////////////////////////////////////////////////////
		
		
		///////////////////////////		IMAGEM NEGATIVA			///////////////////////////
		/*
		uchar blue = negativo(bMin, bMax, ((int)(intensity.val[0])));
		uchar green = negativo(gMin, gMax, ((int)(intensity.val[1])));
		uchar red = negativo(rMin, rMax, ((int)(intensity.val[2])));
		*/
		///////////////////////////////////////////////////////////////////////////////////

		///////////////////////////		Filtro Médio			///////////////////////////
		


		//uchar blue = negativo(bMin, bMax, ((int)(intensity.val[0])));
		//uchar green = negativo(gMin, gMax, ((int)(intensity.val[1])));
		//uchar red = negativo(rMin, rMax, ((int)(intensity.val[2])));

		///////////////////////////////////////////////////////////////////////////////////

		///////////////////////////		Gaussiano			   ///////////////////////////
		
		//uchar blue = negativo(bMin, bMax, ((int)(intensity.val[0])));
		//uchar green = negativo(gMin, gMax, ((int)(intensity.val[1])));
		//uchar red = negativo(rMin, rMax, ((int)(intensity.val[2])));

		///////////////////////////////////////////////////////////////////////////////////

		//image.at<Vec3b>(y,x) = Vec3b(blue,green,red);
	}

	filtroMediana(image);



	//namedWindow("Display Processed Image 4", WINDOW_AUTOSIZE );
    //imshow("Display Processed Image 4", image);	

	//namedWindow("Hist",1);
	//imshow("Hist",histPlot);

    waitKey(0);

    return 0;
}

