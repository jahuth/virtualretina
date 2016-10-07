
#include <retinatools/my_tools.h>

int main(int argc, char **argv) { 
    
    cimg_usage(" Convert an image into a positive luminance pattern, with a defined contrast. This also allows you to create a uniform image the same size of an input sequence, by setting contrast at zero.");
    
    const char *inputName  = cimg_option("-i",(char*)0,"name (and path) for input image.");   
    const char *outputName  = cimg_option("-o",(char*)0,"name (and path) for created image (please, .[format]  at the end!). WATCH OUT! The format must allow floating values (.inr is OK)");  
    const double Lum  = cimg_option("-Lum",1.0,"mean luminance of the new image. Always positive!! We suggest you normalise it (default value)");
    const double Cont  = cimg_option("-Cont",1.0,"(max-min)/(2*mean) for the new image. Cont cannot exceed the maximum value for which min of the image is 0");
    
// **************************************************************************
    
    if(inputName==0){
        cerr<<"Please specify an input image"<<endl;
        return 0;}
    if(outputName==0){
        cerr<<"Please specify an output image"<<endl;
        return 0;}
    
    CImg<double> in(inputName);
    double Min=in.min();
    double Max=in.max();
    double Mean=in.mean();
    double MaximumCont=(Max-Min)/2/(Mean-Min);
    char answer;
    double contrast;
    if(Cont>MaximumCont){
        cout<<"a too high contrast was required. Maximum possible value for Cont is: "<<MaximumCont<<endl;
        cout<<"use this value? (y for yes)"<<endl;
        cin>>answer;
        if(answer=='y'){
            cout<<"using maximum available value as new contrast."<<endl;
            contrast=MaximumCont;
        }
        else{
            cout<<"OK, launch program again, then."<<endl;
            return 0;
        }
    }
    else contrast=Cont;
    
    CImg<double> out(inputName);
    
    cimg_forXYZC(in,x,y,z,v)
        out(x,y,z,v)=Lum *(1 + 2*contrast* (double)(in(x,y,z,v)-Mean)/(double)(Max-Min)); 
    
    cout<<"Statistics of the new created image:"<<endl;
    out.print();
    out.save(outputName);
    
}





