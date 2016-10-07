#include "cimg_signal_drawer.h"


// *********************************************************************************
// *************************      CImgSignalDrawer       ***************************
// *********************************************************************************


CImgSignalDrawer::CImgSignalDrawer() : style(0), width(1), border(10), opacity(1.0), draw_axis(true)
{}



// **********************************************************************************
// *************************    the 'draw' function       ***************************
// **********************************************************************************


void CImgSignalDrawer::draw(CImg<double> &figure, CImg<double> &vals, const double* color, CImg<double>* abscis, CImg<double>* errorBars_v, CImg<double>* errorBars_h)
{

  int size = vals.width() ;


  //*************   Size checks  ***************************
  
  if( (figure.depth()!=1) || (figure.spectrum()!=3) ){
      cerr<<"Error in CImgSignalDrawer::draw() : "
              "The figure used for drawing must have a z-dimension of 1 (2D-figure)"
              " and a v-dimension of 3 (color figure). Exiting."<<endl;
      return;        
  }
  
  if (abscis)
    if ( abscis->width() != size )
    {
      cerr<<"error in CImgSignalDrawer::draw() : "
              "values and abscissas must have the same dimension"<<endl;
      return;
    }

  if (errorBars_v)
    if (errorBars_v->width() != size )
    {
      cerr<<"error in CImgSignalDrawer::draw() :"
              " values and errorBars must have the same dimension"<<endl;
      return;
    }

  if (errorBars_h)
    if (errorBars_h->width() != size )
    {
      cerr<<"error in CImgSignalDrawer::draw() : "
              "values and errorBars must have the same dimension"<<endl;
      return;
    }


  // ******************    Abscissas and axisX  **************************
  // *********************************************************************


  CImg<double>* abscissas = abscis ;

  bool untouchedX=(axisX.begin==0.0)&&(axisX.end==0.0);

  // If abscis==0, then we create a default abscissa from the axisX values
  if( abscis == 0 )
  {
    abscissas = new CImg<double> ( size ) ;
    if( untouchedX )
      axisX.end = size ;
    for( int i=0 ; i<size ; i++ )
      abscissas->at(i) = axisX.begin + i * (axisX.end - axisX.begin) / size ;
  }

  // If abscis!=0 and axisX was untouched by the user, we create axisX by finding min and max for *abscis
  else
    if (untouchedX)
    {
      double mx= -HUGE_VAL, mn=HUGE_VAL;
      for(int t=0 ; t<size ;++t)
      {
        mx = (mx> abscissas->at(t))? mx:abscissas->at(t);
        mn = (mn< abscissas->at(t))? mn:abscissas->at(t);
      }
      axisX.end = mx ;
      axisX.begin = mn ;
    }

  // If abscissas must be log-transformed !
  if( axisX.type != 0 )
  {
    CImg<double> *abstmp = abscissas ;
    if (abscis != 0 )		// new abscissas must also be created
      abscissas = new CImg<double> ( size ) ;

    for( int i=0 ; i<size ; i++ )
      if (axisX.type == 1 )
        abscissas->at(i) = log10 ( abstmp->at(i) ) ;
      else	//type==2
        abscissas->at(i) = log ( abstmp->at(i) ) ;
  }

  axisX.init() ;



  // ******************    Values and axisY  ***************************
  // *******************************************************************
  // type 0 if linear, 1 if log10(values), 2 if log(values)
  // It is important that it arrives AFTER initialization of axisX.


  bool untouchedY=(axisY.begin==0.0)&&(axisY.end==0.0);

  if(untouchedY)
  {
    double mx= -HUGE_VAL, mn=HUGE_VAL;
    for(int t=0 ; t<size ;++t)
      if ( ( abscissas->at(t) >= axisX.tBegin ) && ( abscissas->at(t) <= axisX.tEnd ) )
      {
        mx = (mx>vals(t))? mx:vals(t);
        mn = (mn<vals(t))? mn:vals(t);
      }
    if (mx== -HUGE_VAL )
    {
      cerr << "Problem in CImgSignalDrawer::draw() : there is no point in the range [axisX.begin , axisX.end ]. Exiting. "<<endl;
      return ;
    }
    axisY.end = mx ;
    axisY.begin = mn ;
  }

  // Possibly, we change to log values...

  CImg<double>* values = &vals ;	//by default, linear
  if( axisY.type != 0 )
  {
    values = new CImg<double> ( size ) ;		//else we create transformed values
    for( int i=0 ; i<size ; i++ )
      if (axisY.type == 1 )
        values->at(i) = log10 ( vals(i) ) ;
      else if  (axisY.type == 2 )	//type==2
        values->at(i) = log ( vals(i) ) ;
  }

  axisY.init() ;


  // ************ axis ratios    *********************************
  // *************************************************************


  // relative position (0 to 1) of the Y axis along the X axis:
  double ratioX=0.0 ;
  if ( (axisY.tOffsetVal!=HUGE_VAL) && ( axisX.tEnd != axisX.tBegin) )
    ratioX = (axisY.tOffsetVal-axisX.tBegin)/(axisX.tEnd-axisX.tBegin);

  // vice-versa
  double ratioY=1.0 ;
  if ( (axisX.tOffsetVal!=HUGE_VAL) && ( axisY.tEnd != axisY.tBegin) )
    ratioY = 1-(axisX.tOffsetVal-axisY.tBegin)/(axisY.tEnd-axisY.tBegin);	//trick: ratio from top of image.



  // ************** Borders and additional space for axes ******************
  // ***********************************************************************

  int graphSizeX=figure.width()-2*border, graphSizeY=figure.height()-2*border;
  int borderX=border, borderY=border;

  // additional borders due to the AXES... expressed in pixels
  int axisSpaceX_l=axisY.precision*8+10, axisSpaceX_r=10, grX=graphSizeX;
  int axisSpaceY_l=10, axisSpaceY_r=20, grY=graphSizeY;

  //**************** need to reserve some extra space for the axes  ??

  if( ratioX*grX < axisSpaceX_l )
  {
    graphSizeX=(int)floor((grX-axisSpaceX_l)/(1-ratioX)); axisSpaceX_l=grX-graphSizeX; grX=graphSizeX;
  }
  else
    axisSpaceX_l=0;

  if( (1-ratioX)*grX < axisSpaceX_r )
  {
    graphSizeX=(int)floor((grX-axisSpaceX_r)/ratioX); axisSpaceX_r=grX-graphSizeX;
  }
  else
    axisSpaceX_r=0;

  if( ratioY*grY < axisSpaceY_l )
  {
    graphSizeY=(int)floor((grY-axisSpaceY_l)/(1-ratioY)); axisSpaceY_l=grY-graphSizeY; grY=graphSizeY;
  }
  else
    axisSpaceY_l=0;

  if((1-ratioY)*grY<axisSpaceY_r)
  {
    graphSizeY=(int)floor((grY-axisSpaceY_r)/ratioY); axisSpaceY_r=grY-graphSizeY;
  }
  else
    axisSpaceY_r=0;


  //************  Update borders in consequence
  borderX=border+axisSpaceX_l;
  borderY=border+axisSpaceY_l;


  if((graphSizeX<=0)||(graphSizeY<=0))
  {cerr<<"ERROR in CImgSignalDrawer::draw() : For required borders and axis, drawing figure is too small: no place left to draw signal!"<<endl; exit(0);}



  CImg<double> sig(graphSizeX,graphSizeY,1,3,255);

  // ********************* Drawing graph in 'sig'  ******************************
  // ****************************************************************************

  // We re-write the graph-drawing function, with direct inspiration from the CImg::draw_graph function.

  double xmin= axisX.tBegin, xmax= axisX.tEnd, ymin= axisY.tBegin, ymax= axisY.tEnd ;


  //************* Drawing function inspired directly from CImg.h, except variable abscissas are possible

  if ( style==0 || style==1 )
  {
    int val=0, nVal=(int)floor( (graphSizeY-1) * (ymax-values->at(0))/(ymax-ymin) ) ; //top-down reverted
    int abs=0, nAbs=(int)floor( (graphSizeX-1) * ( abscissas->at(0)-xmin)/(xmax-xmin) ) ;

    // Colors as in CImg::draw_graph
    double *color1 = new double[3], *color2 = new double[3];
    for(int v=0 ; v<3 ; v++) { color1[v]=(double)(color[v]*0.6f); color2[v]=(double)(color[v]*0.3f); }

    // origin point
    int val0 = (int)floor( (graphSizeY-1) * ( ymax- 0.0 ) / (ymax-ymin) ) ;

    //Drawing the plot as in CImg::draw_graph ( PLUS VARIABLE ABSCISSAS )
    for (int i=1 ; i<size ; i++ )
    {
      val = nVal ;
      nVal = (int)floor( (graphSizeY-1) * ( ymax - values->at(i) )/(ymax-ymin) ) ; //top-down reverted
      abs = nAbs ;
      nAbs = (int)floor( (graphSizeX-1) * ( abscissas->at(i)- xmin )/(xmax-xmin) ) ;

      switch ( style )  //copied from CImg.h
      {
      case 0: // plot with segments
        sig.draw_line( abs,val, nAbs, nVal , color, opacity ) ;
        break;

      case 1:
        { // plot with bars
          sig.draw_rectangle( abs, val0, nAbs, val, color1, opacity);
          sig.draw_line( abs, val, abs, val0, color2, opacity);
          sig.draw_line( abs, val0, nAbs, val0, val<=val0? color2:color, opacity);
          sig.draw_line( nAbs, val, nAbs, val0, color, opacity);
          sig.draw_line( abs, val, nAbs, val, val<=val0? color:color2, opacity);
        }
        break;
      }
    }
    delete[] color1; delete[] color2;
  }


  //******* Cubic interpolation... abscissas are not used, so... let's directly use CImg for this one ! :-)
  if ( style==2 )
  {
    if (abscis)
      cerr<<"Sorry, signal drawer with type 2 (cubic interpolation) was not coded for use with varying abscissas. The CImg<double> *abscissas was not taken into account."<<endl;

    CImg<double> valuesTruncated=values->get_crop( (int)floor(axisX.begin) , (int)floor(axisY.end)-1 );
    sig.draw_graph( valuesTruncated, color, style, ymax, ymin);
  }



  // ********************    Draw ERROR BARS in 'sig' ***************************
  // ****************************************************************************

  if( errorBars_v || errorBars_h )
  {
    int val=0, abs=0 ;

    //Drawing the plot as in CImg::draw_graph (plus variable abscissas)
    for (int i=0 ; i<size ; i++ )
    {
      val = (int)floor( (graphSizeY-1) * (ymax-values->at(i))/(ymax-ymin) ) ; //top-down reverted
      abs = (int)floor( (graphSizeX-1) * ( abscissas->at(i)-xmin)/(xmax-xmin) ) ;

      if(errorBars_v)
        draw_errorBar(sig, abs, val, (graphSizeY-1) * errorBars_v->at(i) /(ymax-ymin)  ) ;
      if(errorBars_h)
        draw_errorBar(sig, abs, val,  (graphSizeX-1) * errorBars_h->at(i) /(xmax-xmin) , true);
    }
  }


  // *******************    re-draw 'sig' in 'figure'  **************************
  // ****************************************************************************

  cimg_forXYZ(sig,x,y,z)
  if ((sig(x,y,z,0)!=255)||(sig(x,y,z,1)!=255)||(sig(x,y,z,2)!=255))  //to avoid undrawn pixels !
    for(int col=0;col<3;++col)
    {
      figure(borderX+x,borderY+y,z,col)=opacity*sig(x,y,z,col)+(1-opacity)*figure(borderX+x,borderY+y,z,col);
      for(int w=1;w<width;++w)
      {
        figure(borderX+x+w,borderY+y,z,col)=opacity*sig(x,y,z,col)+(1-opacity)*figure(borderX+x+w,borderY+y,z,col);
        figure(borderX+x,borderY+y+w,z,col)=opacity*sig(x,y,z,col)+(1-opacity)*figure(borderX+x,borderY+y+w,z,col);
      }
    }



  // ****************     Drawing axes     ********************************
  // **********************************************************************

  if(draw_axis)
  {
    //drawing horizontal axis:

    int off=borderY+(int)ceil(ratioY*graphSizeY);
    CImg<double> ax( graphSizeX , figure.height() , 1,3,255);

    draw_axisX ( ax, axisX.scale, axisX.scaleLoc, off );
    for(int col=0;col<3;++col)
    {
      for(int y=0;y<figure.height();++y)
        for(int x=0;x<graphSizeX;++x)
          if ((ax(x,y,0,0)!=255)||(ax(x,y,0,1)!=255)||(ax(x,y,0,2)!=255))
            figure(borderX+x,y,0,col)=axisX.opacity*ax(x,y,0,col)+(1-axisX.opacity)*figure(borderX+x,y,0,col);
      for(int x=border;x<borderX;++x)
        figure(x,off,0,col)=axisX.opacity*axisX.color[col]+(1-axisX.opacity)*figure(x,off,0,col);
    }

    //drawing vertical axis:

    off=borderX+(int)ceil(ratioX*graphSizeX);
    ax=CImg<double>(figure.width(),graphSizeY,1,3,255);

    draw_axisY ( ax, axisY.scale, axisY.scaleLoc, off );
    for(int col=0;col<3;++col)
    {
      for(int x=0;x<figure.width();++x)
        for(int y=0;y<graphSizeY;++y)
          if ((ax(x,y,0,0)!=255)||(ax(x,y,0,1)!=255)||(ax(x,y,0,2)!=255))
            figure(x,borderY+y,0,col)=axisY.opacity*ax(x,y,0,col)+(1-axisY.opacity)*figure(x,borderY+y,0,col);
      for(int y=borderY+graphSizeY;y<figure.height()-border;++y)
        figure(off,y,0,col)=axisY.opacity*axisY.color[col]+(1-axisY.opacity)*figure(off,y,0,col);
    }
  }

  // to allow next call of the draw() function with automatic fitting of axis' scales.

if(untouchedX) {axisX.begin=0.0; axisX.end=0.0;}
  if(untouchedY) {axisY.begin=0.0; axisY.end=0.0;}

}

// *****************************************************************************************
// ********************************   DONE WITH THE LONG ONE !!! ***************************
// *****************************************************************************************




// *************************    the 'min - max ' version     ***************************
// *************************************************************************************


void CImgSignalDrawer::draw(CImg<double> &figure, CImg<double> &vals, const double* color, double tmin, double tmax, CImg<double>* errorBars_v, CImg<double>* errorBars_h)
{
  int size = vals.width() ;
  CImg<double> new_abs ( size ) ;
  for( int i=0 ; i<size ; i++ )
    new_abs (i) = tmin + i*(tmax-tmin)/(size-1) ;

  draw(figure, vals, color, &new_abs, errorBars_v, errorBars_h) ;
}



//************   the file loading version... old stuff, but well... ********************
//**************************************************************************************


void CImgSignalDrawer::draw(CImg<double> &figure, const char *signal,  const double* color )
{
  FILE *file;
  int input_size, first_input, after_last_input;
  double step;
  double* fakepointer=new double;

  file=fopen( signal, "r");
  fscanf(file,"<html>\n\n<head>\nTemporal signal. Size %d Step %lf\n</head>\n\n<body>\n",&input_size,&step);

  CImg<double> values( input_size );

  for (int t=0 ; t<input_size ; t++)
    fscanf(file,"%lf\n",values.data(t));
  fclose(file);

  this->draw( figure, values, color );
}



//************** The CImgList drawing versions *****************
//**************************************************************

// ********* (1) With abscissas

void CImgSignalDrawer::draw( CImg<double> &figure, CImgList<double> &values, int startColor, CImg<double>* abscissas )
{

  // ************ SIZE CHECKS *****************

  if (values.width()==0)
  {
    cerr<<" ERROR : Asked to draw signal from an empty list."<<endl;
    return ;
  }

  int size = values[0].width() ;
  for(uint ind=0;ind<values.width();++ind)
    if (values[ind].width() != size)
      cerr<<" Probable ERROR : signals in the list to draw have different sizes."<<endl;


  // ********** Finding shared boundaries for the values to represent :

  // We don't want to bother too much :
  // we take a maximum for ALL values (even those not in the axisX's range )

  bool untouchedY=(axisY.begin==0.0)&&(axisY.end==0.0);

  if(untouchedY)
  {
    double mx=-HUGE_VAL, mn=HUGE_VAL;
    for(uint ind=0;ind<values.width();++ind)
    {

      cimg_foroff(values[ind],t)
      {
        mx = (mx>values[ind][t])? mx:values[ind][t];
        mn = (mn<values[ind][t])? mn:values[ind][t];
      }
    }
    axisY.end = mx ;
    axisY.begin = mn ;
  }

  // *************** Drawing signals :-)

  for(uint ind=0;ind<values.width();++ind)
    this->draw( figure, values[ind], colorSerie(startColor+ind) , abscissas );

  // Back to untouched if that was the case :-)
  if(untouchedY) 
  {axisY.begin=0.0; axisY.end=0.0;}

}


// ********* (2) With abscissa boundaries

void CImgSignalDrawer::draw( CImg<double> &figure, CImgList<double> &values, int startColor, double tmin, double tmax  )
{
  int size = values[0].width() ;
  CImg<double> new_abs ( size ) ;
  for( int i=0 ; i<size ; i++ )
    new_abs (i) = tmin + i*(tmax-tmin)/(size-1) ;

  draw(figure, values, startColor, &new_abs ) ;
}



// ********************* Helper functions **************
// *****************************************************

void CImgSignalDrawer::draw_errorBar (CImg<double> &figure, int x_cen, int y_cen, double length , bool horizontal )
{
  int len = (int)floor(length) ;
  if ( !horizontal)
  {
    figure.draw_line (x_cen , y_cen - len , x_cen , y_cen + len , errorBar.color );
    figure.draw_line ( x_cen - errorBar.end ,  y_cen - len , x_cen + errorBar.end ,  y_cen - len , errorBar.color );
    figure.draw_line ( x_cen - errorBar.end ,  y_cen + len , x_cen + errorBar.end ,  y_cen + len , errorBar.color );
  }
  else
  {
    figure.draw_line (x_cen - len , y_cen , x_cen + len , y_cen , errorBar.color );
    figure.draw_line ( x_cen - len ,  y_cen - errorBar.end , x_cen - len ,  y_cen + errorBar.end , errorBar.color );
    figure.draw_line ( x_cen + len ,  y_cen - errorBar.end , x_cen + len ,  y_cen + errorBar.end , errorBar.color );
  }
}


// 'enhanced' copies of the draw_axis function from CImg.h ...
// allowing values and locations of the ticks to be decorrelated (for log, exp, etc)

void CImgSignalDrawer::draw_axisX (CImg<double> &figure, CImg<double>& tickValues, CImg<double>& tickLocations, int y )
{
  int siz = (int)tickValues.size()-1;
  if (siz<=0) figure.draw_line(0, y, figure.width()-1, y, axisX.color, axisX.opacity);
  else
  {
    if (tickValues[0]<tickValues[siz]) figure.draw_arrow(0, y, figure.width()-1, y, axisX.color, 30, 5, axisX.opacity);
    else figure.draw_arrow(figure.width()-1, y, 0, y, axisX.color, 30, 5, axisX.opacity);

    const int yt = (y+14)< figure.height()? (y+3):(y-14);
    char txt[32];
    cimg_foroff( tickValues , x )
    {
      if ( axisX.precision>=0 ) std::sprintf(txt,"%.*g", axisX.precision ,(double) tickValues(x) );
      else std::sprintf( txt,"%g",(double)tickValues(x) );

      //only difference with CImg.h : tickLocations
      const int xi=(int)( tickLocations(x)*( figure.width()-1) ), xt = xi-(int)std::strlen(txt)*3;
      figure.draw_point(xi, y-1, axisX.color, axisX.opacity )
      .draw_point(xi, y+1, axisX.color, axisX.opacity )
      .draw_text(xt<0? 0:xt, yt, txt, axisX.color, 0, axisX.opacity, axisX.fontSize );
    }
  }
}


void CImgSignalDrawer::draw_axisY (CImg<double> &figure, CImg<double>& tickValues, CImg<double>& tickLocations, int x )
{
  int siz = (int)tickValues.size()-1;
  if (siz<=0) figure.draw_line(x, 0, x, figure.height()-1, axisY.color, axisY.opacity );
  else
  {
    if (tickValues[0]<tickValues[siz]) figure.draw_arrow(x, figure.height()-1, x, 0, axisY.color, 30, 5, axisY.opacity);
    else figure.draw_arrow(x, 0, x, figure.height()-1, axisY.color, 30, 5, axisY.opacity);

    char txt[32];
    cimg_foroff( tickValues , y )
    {
      if (axisY.precision>=0) std::sprintf(txt,"%.*g",axisY.precision,(double) tickValues(y) );
      else std::sprintf( txt,"%g",(double)tickValues(y) );

      //only difference with CImg.h : tickLocations
      const int yi=(int)( ( 1-tickLocations(y) ) * ( figure.height()-1) ),  //reversed tickLocation !!
                   tmp = yi-5,
                         nyi = tmp<0?0:(tmp>=(int)figure.height()-11?(int)figure.height()-11:tmp),
                               xt = x-(int)std::strlen(txt)*7;

      figure.draw_point(x-1, yi, axisY.color, axisY.opacity )
      .draw_point(x+1, yi, axisY.color, axisY.opacity ) ;
      if (xt>0) figure.draw_text(xt,nyi,txt,axisY.color,0,axisY.opacity, axisY.fontSize );
      else figure.draw_text(x+3,nyi,txt,axisY.color,0,axisY.opacity, axisY.fontSize );
    }
  }
}



// *********************************************************************************
// *************************      CImgAxis       ***********************************
// *********************************************************************************


// Handles boundaries of representation for the signal, and also draws the axes.
// type 0 if linear, 1 if log10(values), 2 if log(values),  (LATER... if ever : 3 if exp(values), 4 if 10^(values) )


CImgAxis::CImgAxis(): fontSize(13)
{type=0; begin=0.0; end=0.0; nTicks_max=10; precision=4; offsetVal=HUGE_VAL; opacity=1.0; color=black; tOffsetVal=HUGE_VAL; }

// Build nice-looking scales .......... ARF !!!!!

void CImgAxis::init()
{

  // (1) *************  (possibly) log-transformed begin and end *****************
  switch( type )
  {
  case 0:
    tBegin = begin ; tEnd = end ; if (offsetVal!=HUGE_VAL) tOffsetVal = offsetVal ;
    break ;
  case 1:
    tBegin = log10(begin); tEnd = log10(end) ; if (offsetVal!=HUGE_VAL)  tOffsetVal = log10(offsetVal) ;
    break ;
  case 2:
    tBegin = log(begin); tEnd = log(end) ;  if (offsetVal!=HUGE_VAL) tOffsetVal = log(offsetVal) ;
    break ;
  default:
    break ;
  }

  // adding a 'nice-looking' margin :
  double tDif = tEnd - tBegin ;
  tBegin -= 0.1 * tDif ;
  tEnd += 0.1 * tDif ;


  // (2) ***** fixing the number and locations of ticks in consequence ***************


  if ( tBegin < tEnd )
  {
    double factor = 1.0 ;
    int diff = 0;
    int depth=0 ;   // A change of scale *10 every time depth +=2. Determines the power of 10 that 'best fits' scales of the graph... a power 0.5 (when depth is odd) has no fixed signification but denotes 'a number halfway between 1 and 10'

    bool found_good_factor = false ;
    bool was_too_big = false ;

    int count = 0;
    while ( ! found_good_factor )
    {
      diff = (int)ceil( tEnd /factor ) - (int)floor( tBegin/factor ) + 1 ; //number of ticks using a scale with the curent depth/factor ;

      if (diff > nTicks_max )
      {	//depth is too big... must take bigger scales to make diff smaller
        if (depth%2 == 0)
          factor *= 5 ;
        else
          factor *= 2 ;
        depth -= 1 ;
        was_too_big = true ;
      }
      else
      {	//depth might be too small... must take smaller scales
        if (was_too_big)
          found_good_factor = true ;
        else
        {
          if (depth%2 == 0)
            factor /= 2 ;
          else
            factor /= 5 ;
          depth += 1 ;
        }
      }
      count++;
    if(count==30) { cerr<<"problem in CImgAxis::init() : could not find proper scale."<<endl; found_good_factor = true ;}
    }

    // OK ! We found the good factor to have the correct number of ticks (diff+1). Now we build scale and scaleLoc.

    scale = CImg<double>( diff ) ;
    scaleLoc = CImg<double>( diff ) ;

    // boundaries 0 and diff are special... and easy !
    // scale(0) = begin ; scale(diff-1) = end ;
    // scaleLoc(0)=0.0 ; scaleLoc(diff-1) = 1.0 ;

    // LET'S GOOO ! FILLING scale and scaleLoc.
    //We must ONLY work on tBegin and tEnd (not directly on 'begin' and 'end', since margins have been modified by 10%)

    for(int i=0 ; i<diff ; i++)
    {
      if(i==0)
        scale(i) = tBegin ;
      else if(i==diff-1)
        scale(i) = tEnd ;
      else
        scale(i) = ( floor ( tBegin/factor ) + i ) * factor ;
      scaleLoc(i) = ( scale(i) - tBegin) / (tEnd -tBegin) ;
      if ( type==1 )
        scale(i) = pow( 10, scale(i) ) ;	//back to original value
      else if ( type==2 )
        scale(i) = exp ( scale(i) ) ;
    }
  }

  else //tBegin >= tEnd
    if ( tBegin==tEnd )
    {
      scale = CImg<double> (3) ;
      if (begin != 0)
      {scale(0) = min ( 0 , 2*begin ) ;  scale(1)=begin ; scale(2)=max ( 0 , 2*begin ) ;}
      else
      {scale(0) = -1 ;  scale(1)=0 ; scale(2)=1;}

      scaleLoc=CImg<double> (3) ;
      scaleLoc(0)=0.0; scaleLoc(1)=0.5; scaleLoc(2)=1.0;
    }
    else
      cerr<< " ERROR from CImgAxis : end is smaller than begin !! "<<endl ;

}



// *********************************************************************************
// *************************      CImgErrorBar       *******************************
// *********************************************************************************

//just a stupid constructor
CImgErrorBar::CImgErrorBar()
{color=black; end=2; }

