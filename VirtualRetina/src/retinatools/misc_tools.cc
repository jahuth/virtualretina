
#include "misc_tools.h"
#include <cstring>

// The extCommand constructors ;

    extCommand::~extCommand()
    {
      for (int i=0;i<argc;++i)
    	delete[] argv[i] ; 
    }
    
    void extCommand::assign ( const vector<string> & commands )
    {
      for (int i=0;i<argc;++i)
    	delete[] argv[i] ; 
    		
      argc=commands.size();
      argv=new char*[argc];
      for(int i=0;i<argc;++i)
      {
        argv[i]=new char[commands[i].size()+1];
        strcpy( argv[i] , commands[i].c_str() );
      }
    }
    
    void extCommand::assign (const string & total_command  )
    {
      istringstream ist( total_command );
      vector<string> argVect;
      string mymymy;
      while( !ist.eof() )
      {
        ist>>mymymy;
        argVect.push_back(mymymy);
      }
      assign( argVect ) ;
    }
    
    extCommand::extCommand ( const vector<string> & commands ) : argc(0), argv(0)
    { assign( commands );}
    
    extCommand::extCommand ( const string & total_command )  : argc(0), argv(0)
    { assign( total_command );}
	
    
// Get current working directory... ugly, but compiler independent?
std::string my_pwd(void)
{
    system("pwd > youllneverusethisnamewillyouGRHWPJNG");
    ifstream fin("youllneverusethisnamewillyouGRHWPJNG");
    string s;
    getline(fin,s);
    system("rm youllneverusethisnamewillyouGRHWPJNG");
    return s;
}
    

//Approximation of instantaneous firing rate for a conductance-driven neuron:
double instantFiringRate(double I, double tauRefr, double gLeak, double Ginh, double Einh, double  Gexc, double Eexc)
{
  double gInf=gLeak+Gexc+Ginh;
  double Itot=I+Gexc*Eexc+Ginh*Einh;
  if(gInf<Itot)
    return 1.0/( tauRefr-log(1.0-gInf/Itot)/gInf ) ;
  else
    return 0.0;
}


