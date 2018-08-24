#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


class Entity
{
public:
  Entity() {
    quantity = 1.;
    income = 0.;
    taxrate = 0.;
    out = 0;
    sum = 0.;
  }
  
  double quantity;
  string name;
  double income;
  double taxrate;
  double out;
  double sum;
};



int main( int argc, char** argv )
{
  /*
  commandArg<string> fileCmmd("-i","input file");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the file parser.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  */

  svec<Entity> ent;

  Entity gov;
  gov.name = "Government";
  gov.taxrate = 0.;
  gov.sum = 0.;
  gov.out = 10000000;
  
  Entity emp;
  emp.name = "Employee";
  emp.taxrate = 0.32;
  emp.out = 2000;
  emp.quantity = 100;

  Entity ind;
  ind.name = "Industry";
  ind.taxrate = 0.4;
  ind.out = emp.quantity*3100;
  ind.quantity = 1;
  
  Entity un;
  un.name = "Unemployed";
  un.taxrate = 0.;
  un.out = 1200;
  un.quantity = 10;

  gov.out = emp.quantity*2000;
 
  int i;
  for (i=0; 1<200; i++) {
    cout << "Gov: " << gov.sum << " Emp: " << emp.sum << " Ind: " << ind.sum << " Unemp: " << un.sum << " SUM: " << gov.sum + emp.sum + ind.sum << endl;
    gov.sum += emp.sum*emp.taxrate + ind.sum*ind.taxrate;
    emp.sum -= emp.sum*emp.taxrate;
    ind.sum -= ind.sum*ind.taxrate;

    //cout << "Tax: " << emp.sum*emp.taxrate << endl;

    double salary = 1700;
    emp.sum += emp.quantity*salary;
    ind.sum -= emp.quantity*salary;
    
    emp.sum -= emp.quantity*emp.out;
    ind.sum += emp.quantity*emp.out;

    //cout << "Before: " << ind.sum << " " << endl;
    ind.sum += gov.out;
    gov.sum -= gov.out;
    //cout << "After: " << ind.sum << endl;

    un.sum -= un.quantity*un.out;
    ind.sum += un.quantity*un.out;    
    
    un.sum += un.quantity*1200;
    gov.sum -= un.quantity*1200;

    gov.sum -= emp.quantity*1200;
    emp.sum += emp.quantity*1200;

  }


  
  return 0;
}
