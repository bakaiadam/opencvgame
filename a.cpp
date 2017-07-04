#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <ctype.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "convexhull.h"


using namespace cv;
using namespace std;

static void help()
{
    // print a welcome message, and the OpenCV version
    cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
            "Using OpenCV version " << CV_VERSION << endl;
    cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
    cout << "\nHot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n" << endl;
}

Point2f point;
bool addRemovePt = false;

static void onMouse( int event, int x, int y, int /*flags*/, void* /*param*/ )
{
    if( event == EVENT_LBUTTONDOWN )
    {
        point = Point2f((float)x, (float)y);
        addRemovePt = true;
    }
}

float euclideanDist(cv::Point2f& a, cv::Point2f& b)
{
    cv::Point2f diff = a - b;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

int main( int argc, char** argv )
{
  srand (time(NULL));

int index=-1;

    VideoCapture cap;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(4,4), winSize(31,31);

    const int MAX_COUNT = 500;
    bool needToInit = false;
    bool nightMode = false;

    cv::CommandLineParser parser(argc, argv, "{@input||}{help h||}");
    string input = parser.get<string>("@input");
    //if (parser.has("help"))
    if (0)
    {
        help();
        return 0;
    }
    if( input.empty() )
        cap.open(0);
    else if( input.size() == 1 && isdigit(input[0]) )
        cap.open(input[0] - '0');
    else
        cap.open(input);

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }

    namedWindow( "LK Demo", 1 );
    setMouseCallback( "LK Demo", onMouse, 0 );

    Mat gray, prevGray, image, frame;
    vector<Point2f> points[2];

    for(;;)
    {
        cap >> frame;
        if( frame.empty() )
            break;

        frame.copyTo(image);
        cvtColor(image, gray, COLOR_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

      /*  if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
            addRemovePt = false;
        }
        else if( !points[0].empty() )*/
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);

points[0].clear();
int dist=20;
vector<int> elems;
for (int i=0;i<(gray.size().width/dist-1)*(gray.size().height/dist-1)  ;i++)
  elems.push_back(i);

if (index==-1)
{
index=rand()%( (gray.size().width/dist-1)*(gray.size().height/dist-1) ) ;
}

for (int i=dist;i<gray.size().width ;i+=dist)
for (int j=dist;j<gray.size().height ;j+=dist)
{
points[0].push_back( Point2f(i,j) );
if (points[0].size()-1==index)
{
//cout<<points[0].back() <<"\n";
                circle( image, points[0].back() , 30, Scalar(255,0,0), -1, 8);
}
}



            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 30, termcrit, 0, 0.001);
            size_t i, k;
//if (status[index] && ( points[0][index].x!=points[1][index].x || points[0][index].y!=points[1][index].y ) )
//if (!status[index] || euclideanDist(points[0][index],points[1][index])<2 )
if (!status[index] || euclideanDist(points[0][index],points[1][index])>2 )
{
//while (!status[index] || euclideanDist(points[0][index],points[1][index])<2 )
std::random_shuffle(elems.begin(),elems.end() );
index=rand()%( (gray.size().width/dist-1)*(gray.size().height/dist-1) ) ;
for (int i:elems)
{
if (status[i] && euclideanDist(points[0][i],points[1][i])<2 )
  {index=i; 
cout<<"talaltam\n";
break;}

}

/*{
cout<<"e "<<euclideanDist(points[0][index],points[1][index]) <<"\n";
  index=rand()%( (gray.size().width/dist-1)*(gray.size().height/dist-1) ) ;
cout<<index<<"\n";
}*/

}
vector<Pointq> ppp;

            for( i = k = 0; i < points[1].size(); i++ )
            {
                if( addRemovePt )
                {
                    if( norm(point - points[1][i]) <= 5 )
                    {
                        addRemovePt = false;
                        continue;
                    }
                }

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i];
if (euclideanDist(points[0][i],points[1][i])>2 )
{
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
ppp.push_back(Pointq{points[1][i].x,points[1][i].y});
}


            }
            points[1].resize(k);
if (ppp.size()>2) 
{
auto aaa=convex_hull(ppp);
for (int i=0;i<aaa.size();i++)
{
line(image,Point2f(aaa[i].x,aaa[i].y),Point2f(aaa[ (i+1)%aaa.size()].x,aaa[(i+1)%aaa.size()].y), Scalar(0,0,255),10 ); 
}


}


        }

        if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }

        needToInit = false;
        imshow("LK Demo", image);
cv::Mat dst;
cv::flip(image, dst, 1);
        imshow("LK Demo2", dst);

        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch( c )
        {
        case 'r':
            needToInit = true;
            break;
        case 'c':
            points[0].clear();
            points[1].clear();
            break;
        case 'n':
            nightMode = !nightMode;
            break;
        }

        std::swap(points[1], points[0]);
//       std::cout<<points[0].size()<<"\n";
        cv::swap(prevGray, gray);
    }

    return 0;
}
