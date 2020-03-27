//#include <math.h>
//#include <stdlib.h>
//void df(int w){
//}

struct {
    double stepSize;
    int clockTick0;
  } Timing;


int main()
{
	//int e=1;
	int df[30];
	
	int gw=2.0,gt=3.0f;
	//int r;
	//r = df[gt-1];
	gw = Timing.stepSize * 3;
	int re=8/gw;
	
	/*
	if(!isnan(0)){		
		int g=5/(gt-gw);
	}
	int g = gt / gw;
	int y = sqrt(-2.0 * log(gw) / gt) * gw;
	if(g!=0){
		g = gt / gw;
	}
	if(g!=0){
		g = gt * gw;
	}
	*/
	//int r=fabs(7);
	//int w=abs(3);
}