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


Mat filtroNegativo (Mat image)
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

    return imageNegative;
}

Mat filtroMedia (Mat image)
{
	Mat imageGray = image.clone();
	//Função que passa a imagem para tons de cinza
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	Mat imageAverage = imageGray.clone();

	//	Kernel já normalizado
	double kernel[3][3] = {	1.0/9, 1.0/9, 1.0/9, 
							1.0/9, 1.0/9, 1.0/9, 
							1.0/9, 1.0/9, 1.0/9};
	
	for (int y=1; y < imageAverage.rows; y++)
    	for (int x=1; x < imageAverage.cols; x++)
   		{
   			imageAverage.at<uchar>(y,x)=0;
   			for(int i=-1; i<=1; i++)
   				for(int j=-1; j<=1; j++)
   					imageAverage.at<uchar>(y,x) += imageGray.at<uchar>(y+j,x+i)*kernel[i+1][j+1];	
   		}

	return imageAverage;
}

Mat filtroGaussiano (Mat image)
{
	Mat imageGray = image.clone();
	//Função que passa a imagem para tons de cinza
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	Mat imageGaussian = imageGray.clone();

	//	Kernel já normalizado
	double kernel[3][3] = {	1.0/16, 2.0/16, 1.0/16, 
							2.0/16, 4.0/16, 2.0/16, 
							1.0/16, 2.0/16, 1.0/16};
	
	for (int y=1; y < imageGaussian.rows; y++)
    	for (int x=1; x < imageGaussian.cols; x++)
   		{
   			imageGaussian.at<uchar>(y,x)=0;
   			for(int i=-1; i<=1; i++)
   				for(int j=-1; j<=1; j++)
   					imageGaussian.at<uchar>(y,x) += imageGray.at<uchar>(y+j,x+i)*kernel[i+1][j+1];	
   		}

	return imageGaussian;
}

Mat filtroSaturacao (Mat image)
{
	Mat imageSaturated = image.clone();
	//Declaracao dos histogramas
	double histImg1blue[256] = {0}; double histImg1green[256] = {0}; double histImg1red[256] = {0};

	//Preenchimento dos histogramas
    for (int y=0; y < imageSaturated.rows; y++)
    	for (int x=0; x < imageSaturated.cols; x++) 
		{
			Vec3b intensity = imageSaturated.at<Vec3b>(y, x);
			histImg1blue[(int)(intensity.val[0])]++;
			histImg1green[(int)(intensity.val[1])]++;
			histImg1red[(int)(intensity.val[2])]++;
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

    return imageSaturated;
}


Mat filtroSobel (Mat image)
{	
	//Mat imageGray = image.clone();
	//Função que passa a imagem para tons de cinza
	//cvtColor(imageGray, imageGray, CV_RGB2GRAY);
	Mat imageGray = filtroGaussiano(image).clone();

	Mat imageSobelx = imageGray.clone();
	Mat imageSobely = imageGray.clone();
	Mat imageSobelGrad = imageGray.clone();
	

	//	Kernel já normalizado
	double kernelx[3][3] = {-1.0,		0, 			1.0, 
							-2.0, 		0, 			2.0, 
							-1.0,		0, 			1.0};
	
	double kernely[3][3] = {-1.0,		-2.0, 		-1.0, 
							0, 			0,	 		0, 
							1.0, 		2.0, 		1.0};

	
	for (int y=1; y < imageSobelx.rows; y++)
    	for (int x=1; x < imageSobelx.cols; x++)
   		{
   			double sumx = 0, sumy=0;
   			for(int i=-1; i<=1; i++)
   				for(int j=-1; j<=1; j++)
   				{
   					sumy += imageGray.at<uchar>(y+j,x+i)*kernely[i+1][j+1];
   					sumx += imageGray.at<uchar>(y+j,x+i)*kernelx[i+1][j+1];
   				}
   			
   			sumx = sumx > 255? 255: sumx;
   			sumx = sumx < 0 ? 0: sumx;

   			sumy = sumy > 255? 255: sumy;
   			sumy = sumy < 0 ? 0: sumy;
   			
   			imageSobelx.at<uchar>(y,x) = sumx;
   			imageSobely.at<uchar>(y,x) = sumy;
   		}

	for (int y=1; y < imageSobelGrad.rows; y++)
    	for (int x=1; x < imageSobelGrad.cols; x++)
   			//imageSobelGrad.at<uchar>(y,x)= abs(imageSobelx.at<uchar>(y,x)) + abs(imageSobely.at<uchar>(y,x));
			imageSobelGrad.at<uchar>(y,x)= sqrt(imageSobelx.at<uchar>(y,x)*imageSobelx.at<uchar>(y,x) + imageSobely.at<uchar>(y,x)*imageSobely.at<uchar>(y,x));
		
	/*
   	namedWindow("Display Processed Image - Sobel x1", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel x1", imageSobelx);

	namedWindow("Display Processed Image - Sobel y1", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel y1", imageSobely);

	namedWindow("Display Processed Image - Sobel Gradiente1", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel Gradiente1", imageSobelGrad);


    Sobel( imageGray, imageSobelx, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
    Sobel( imageGray, imageSobely, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
	convertScaleAbs( imageSobelx, imageSobelx );
	convertScaleAbs( imageSobely, imageSobely );
	addWeighted( imageSobelx, 0.5, imageSobely, 0.5, 0, imageSobelGrad );
	*/

/*
   	namedWindow("Display Processed Image - Sobel x", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel x", imageSobelx);

	namedWindow("Display Processed Image - Sobel y", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel y", imageSobely);

	namedWindow("Display Processed Image - Sobel Gradiente", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel Gradiente", imageSobelGrad);
*/
	return imageSobelGrad;
}

void filtroLaplaciano(Mat image)
{
	Mat imageGray = image.clone();
	//Função que passa a imagem para tons de cinza
	cvtColor(imageGray, imageGray, CV_RGB2GRAY);

	Mat imageLaplacian = imageGray.clone();

	/*
	double kernel[3][3] = {	0, 		-1,		0, 
							-1, 	4, 		-1, 
							0, 		-1, 	0};
	*/
	
	double kernel[3][3] = {	-1, 	-1,		-1, 
							-1, 	8, 		-1, 
							-1, 	-1, 	-1};
	
	/*
	double kernel[3][3] = {	1, 		-2,		1, 
							-2, 	4, 		-2, 
							1, 		-2, 	1};
	*/

	for (int y=1; y < imageLaplacian.rows; y++)
    	for (int x=1; x < imageLaplacian.cols; x++)
   		{
   			int sum = 0;
   			for(int i=-1; i<=1; i++)
   				for(int j=-1; j<=1; j++)
   					sum += imageGray.at<uchar>(y+j,x+i)*kernel[i+1][j+1];
   			
   			sum = sum > 255? 255: sum;
   			sum = sum < 0? 0: sum;
   			
   			imageLaplacian.at<uchar>(y,x) = sum;	
   		}

   	namedWindow("Display Processed Image - Laplaciano", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Laplaciano", imageLaplacian);
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

    namedWindow("Display Original Image", WINDOW_AUTOSIZE );
    imshow("Display Original Image", image);

	Mat aux;
	
	/*
	aux = image.clone();
	cvtColor(aux, aux, CV_RGB2GRAY);
	namedWindow("Display Processed Image - Gray", WINDOW_AUTOSIZE );
    imshow("Display Processed Image - Gray", aux);
    */
	
	aux = filtroSaturacao(image).clone();
	namedWindow("Display Processed Image - Saturated", WINDOW_AUTOSIZE );
    imshow("Display Processed Image - Saturated", aux);
	
	
	aux = filtroNegativo(image).clone();
	namedWindow("Display Processed Image - Negative", WINDOW_AUTOSIZE );
    imshow("Display Processed Image - Negative", aux);
	
	/*
	aux = filtroMedia(image).clone();
   	namedWindow("Display Processed Image - Average", WINDOW_AUTOSIZE );
    imshow("Display Processed Image - Average", aux);
	*/
    /*
    aux = filtroGaussiano(image).clone();
   	namedWindow("Display Processed Image - Gaussian", WINDOW_AUTOSIZE );
    imshow("Display Processed Image - Gaussian", aux);
	*/	

	aux = filtroSobel(image).clone();
	namedWindow("Display Processed Image - Sobel Gradiente", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Sobel Gradiente", aux);

    filtroLaplaciano(image);
    /*
	aux = filtroLaplaciano(image).clone();
	namedWindow("Display Processed Image - Laplaciano", WINDOW_AUTOSIZE);
    imshow("Display Processed Image - Laplaciano", aux);
	*/
	
    waitKey(0);

    return 0;
}

