#include <config.h>

#if 0
#include "cf_gmp.h"

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_berlekamp.h"
#include "fac_cantzass.h"
#include "fac_univar.h"
#include "fac_multivar.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"

#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/GF2XFactoring.h>
#include "int_int.h"
#include <limits.h>
#include <NTL/ZZ_pEXFactoring.h>
#include <NTL/GF2EXFactoring.h>
#include "NTLconvert.h"

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertFacCF2NTLZZpX                                                     //
//                                                                                //
// DESCRIPTION:                                                                   //
// Conversion routine for Factory-type canonicalform into ZZpX of NTL,            //
// i.e. polynomials over F_p. As a precondition for correct execution,            //
// the characteristic has to a a prime number.                                    //
//                                                                                //
// INPUT:  A canonicalform f                                                      //
// OUTPUT: The converted NTL-polynomial over F_p of type ZZpX                     //
//////////////////////////////////////////////////////////////////////////////////// 


ZZ_pX convertFacCF2NTLZZpX(CanonicalForm f)
{  
  ZZ_pX ntl_poly;

    CFIterator i;
    i=f;

    int j=0;
    int NTLcurrentExp=i.exp();
    int largestExp=i.exp();
    int k;

    // we now build up the NTL-polynomial
   ntl_poly.SetMaxLength(largestExp+1);
    
   for (i;i.hasTerms();i++)
    {
      
      for (k=NTLcurrentExp;k>i.exp();k--)
      {
        SetCoeff(ntl_poly,k,0);
      }
      NTLcurrentExp=i.exp();

      if (!i.coeff().isImm())
      {  //This case will never happen if the characteristic is in fact a prime number, since
         //all coefficients are represented as immediates 
	 #ifndef NOSTREAMIO
          cout << "convertFacCF2NTLZZ_pX: coefficient not immediate! : " << f << "\n";
	 #endif
          exit(1);
        }
      else
      {
        SetCoeff(ntl_poly,NTLcurrentExp,i.coeff().intval());
      }
      
      NTLcurrentExp--;
      
    }

   //Set the remaining coefficients of ntl_poly to zero. This is necessary, because NTL internally 
   //also stores powers with zero coefficient, whereas factory stores tuples of degree and coefficient
   //leaving out tuples if the coefficient equals zero 
    for (k=NTLcurrentExp;k>=0;k--)
      {
        SetCoeff(ntl_poly,k,0);
      }

    //normalize the polynomial and return it
    ntl_poly.normalize();

    return ntl_poly;
}


////////////////////////////////////////////////////////////////////////////////////
// NAME: convertFacCF2NTLGF2X                                                     //
//                                                                                //
// DESCRIPTION:                                                                   //
// Conversion routine for Factory-type canonicalform into GF2X of NTL,            //
// i.e. polynomials over F_2. As precondition for correct execution,              //
// the characteristic must equal two.                                             //
// This is a special case of the more general conversion routine for              //
// canonicalform to ZZpX. It is included because NTL provides additional          //
// support and faster algorithms over F_2, moreover the conversion code           //
// can be optimized, because certain steps are either completely obsolent         //
// (like normalizing the polynomial) or they can be made significantly            //
// faster (like building up the NTL-polynomial).                                  //
//                                                                                //
// INPUT:  A canonicalform f                                                      //
// OUTPUT: The converted NTL-polynomial over F_2 of type GF2X                     //
////////////////////////////////////////////////////////////////////////////////////

GF2X convertFacCF2NTLGF2X(CanonicalForm f)
{  
    GF2X ntl_poly;

    CFIterator i;
    i=f;

    int j=0;
    int NTLcurrentExp=i.exp();
    int largestExp=i.exp();
    int k;

    //building the NTL-polynomial
    ntl_poly.SetMaxLength(largestExp+1);
    
    for (i;i.hasTerms();i++)
    {
      
      for (k=NTLcurrentExp;k>i.exp();k--)
      {
        SetCoeff(ntl_poly,k,0);
      }
      NTLcurrentExp=i.exp();

      if (!i.coeff().isImm())
      {
	#ifndef NOSTREAMIO
        cout << "convertFacCF2NTLZZ_pX: coefficient not immidiate! : " << f << "\n";
	#endif
        exit(1);
      }
      else
      {
        SetCoeff(ntl_poly,NTLcurrentExp,i.coeff().intval());
      }
      
      NTLcurrentExp--;
      
    }
    for (k=NTLcurrentExp;k>=0;k--)
      {
        SetCoeff(ntl_poly,k,0);
      }
    //normalization is not necessary of F_2

    return ntl_poly;
}


////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLZZpX2CF                                                        //
//                                                                                //
// DESCRIPTION:                                                                   //
// Conversion routine for NTL-Type ZZpX to Factory-Type canonicalform.            //
// Additionally a variable x is needed as a parameter indicating the              //
// main variable of the computed canonicalform. To guarantee the correct          // 
// execution of the algorithm, the characteristic has a be an arbitrary           // 
// prime number.                                                                  //
//                                                                                //
// INPUT:  A canonicalform f, a variable x                                        //
// OUTPUT: The converted Factory-polynomial of type canonicalform,                //
//         built by the main variable x                                           //
//////////////////////////////////////////////////////////////////////////////////// 

CanonicalForm convertNTLZZpX2CF(ZZ_pX poly,Variable x)
{
  CanonicalForm bigone;


  if (deg(poly)>0)
  {
    // poly is non-constant
    bigone=0;
    // Compute the canonicalform coefficient by coefficient, bigone summarizes the result.
    for (int j=0;j<deg(poly)+1;j++)
    {
      if (coeff(poly,j)!=0) bigone=bigone + (CanonicalForm(x,j)*CanonicalForm(to_long(rep(coeff(poly,j)))));
    }
  }
  else
  {
    // poly is immediate
    bigone=CanonicalForm(to_long(rep(coeff(poly,0))));
  }
  
  

  return bigone;
}


////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLGF2X2CF                                                        //
//                                                                                //
// DESCRIPTION:                                                                   //
// Conversion routine for NTL-Type GF2X to Factory-Type canonicalform,            //
// the routine is again an optimized special case of the more general             //
// conversion to ZZpX. Additionally a variable x is needed as a                   //
// parameter indicating the main variable of the computed canonicalform.          //
// To guarantee the correct execution of the algorithm the characteristic         //
// has a be an arbitrary prime number.                                            //
//                                                                                //
// INPUT:  A canonicalform f, a variable x                                        //
// OUTPUT: The converted Factory-polynomial of type canonicalform,                //
//         built by the main variable x                                           //
////////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLGF2X2CF(GF2X poly,Variable x)
{
  CanonicalForm bigone;

  if (deg(poly)>0)
  {
    // poly is non-constant
    bigone=0;
    // Compute the canonicalform coefficient by coefficient, bigone summarizes the result.
    // In constrast to the more general conversion to ZZpX 
    // the only possible coefficients are zero and one yielding the following simplified loop
    for (int j=0;j<deg(poly)+1;j++)
    {
      if (coeff(poly,j)!=0) bigone=bigone + CanonicalForm(x,j); 
     // *CanonicalForm(to_long(rep(coeff(poly,j))))) is not necessary any more;
    }
  }
  else
  {
    // poly is immediate
    bigone=CanonicalForm(to_long(rep(coeff(poly,0))));
  }

  return bigone;
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLvec_pair_ZZpX_long2FacCFFList                                  //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for converting a vector of polynomials from ZZpX to                    //
// a CFFList of Factory. This routine will be used after a successful             //
// factorization of NTL to convert the result back to Factory.                    //
//                                                                                //
// Additionally a variable x and the computed multiplicity, as a type ZZp         //
// of NTL, is needed as parameters indicating the main variable of the            //
// computed canonicalform and the multiplicity of the original polynomial.        //
// To guarantee the correct execution of the algorithm the characteristic         //
// has a be an arbitrary prime number.                                            //
//                                                                                //
// INPUT:  A vector of polynomials over ZZp of type vec_pair_ZZ_pX_long and       //
//         a variable x and a multiplicity of type ZZp                            //
// OUTPUT: The converted list of polynomials of type CFFList, all polynomials     //
//         have x as their main variable                                          //
////////////////////////////////////////////////////////////////////////////////////

CFFList convertNTLvec_pair_ZZpX_long2FacCFFList(vec_pair_ZZ_pX_long e,ZZ_p multi,Variable x)
{
  CFFList rueckgabe;
  ZZ_pX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted
  

  // Start by appending the multiplicity 
  if (!IsOne(multi)) rueckgabe.append(CFFactor(CanonicalForm(to_long(rep(multi))),1));
  

  // Go through the vector e and compute the CFFList
  // again bigone summarizes the result
  for (int i=e.length()-1;i>=0;i--)
  {
    rueckgabe.append(CFFactor(convertNTLZZpX2CF(e[i].a,x),e[i].b));
  }
  
  return rueckgabe;
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLvec_pair_GF2X_long2FacCFFList                                  //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for converting a vector of polynomials of type GF2X from               //
// NTL to a list CFFList of Factory. This routine will be used after a            //
// successful factorization of NTL to convert the result back to Factory.         // 
// As usual this is simply a special case of the more general conversion          // 
// routine but again speeded up by leaving out unnecessary steps.                 //
// Additionally a variable x and the computed multiplicity, as type               // 
// GF2 of NTL, are needed as parameters indicating the main variable of the       //
// computed canonicalform and the multiplicity of the original polynomial.        // 
// To guarantee the correct execution of the algorithm the characteristic         //
// has a be an arbitrary prime number.                                            //
//                                                                                //
// INPUT:  A vector of polynomials over GF2 of type vec_pair_GF2X_long and        //
//         a variable x and a multiplicity of type GF2                            //
// OUTPUT: The converted list of polynomials of type CFFList, all                 //
//         polynomials have x as their main variable                              //
//////////////////////////////////////////////////////////////////////////////////// 

CFFList convertNTLvec_pair_GF2X_long2FacCFFList(vec_pair_GF2X_long e,GF2 multi,Variable x)
{
  CFFList rueckgabe;
  GF2X polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted.

  //We do not have to worry about the multiplicity in GF2 since it equals one.

  // Go through the vector e and compute the CFFList
  // bigone summarizes the result again
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;
    
    polynom=e[i].a;
    exponent=e[i].b;
    for (int j=0;j<deg(polynom)+1;j++)
    {
      if (coeff(polynom,j)!=0) bigone=bigone + (CanonicalForm(x,j)*CanonicalForm(to_long(rep(coeff(polynom,j)))));
    }

    //append the converted polynomial to the CFFList
    rueckgabe.append(CFFactor(bigone,exponent));
  }
  
  return rueckgabe;
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertZZ2CF                                                             //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for conversion of integers represented in NTL as Type ZZ to            //
// integers in Factory represented as canonicalform.                              //
// To guarantee the correct execution of the algorithm the characteristic         //
// has to equal zero.                                                             //
//                                                                                //
// INPUT:  The value coefficient of type ZZ that has to be converted              //
// OUTPUT: The converted Factory-integer of type canonicalform                    //
////////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertZZ2CF(ZZ coefficient)
{ 
  long coeff_long;
  char stringtemp[5000]="";
  char stringtemp2[5000]="";
  char dummy[2];
  int minusremainder=0;
  
  coeff_long=to_long(coefficient);

  //Test whether coefficient can be represented as an immediate integer in Factory
  if ( (NumBits(coefficient)<=NTL_ZZ_NBITS) && (coeff_long>MINIMMEDIATE) && (coeff_long<MAXIMMEDIATE))
  { 
      
    // coefficient is immediate --> return the coefficient as canonicalform
    return CanonicalForm(coeff_long);     
  }
  else 
  {   
    // coefficient is not immediate (gmp-number)
      
    // convert coefficient to char* (input for gmp)
    dummy[1]='\0';
      
    if (coefficient<0)
    { 
      // negate coefficient, but store the sign in minusremainder
      minusremainder=1;
      coefficient=-coefficient;
    }

    while (coefficient>9)
    {
      ZZ quotient,remaind;
      ZZ ten;
      ten=10;
      DivRem(quotient,remaind,coefficient,ten);
      dummy[0]=(char)(to_long(remaind)+'0');
        
      strcat(stringtemp,dummy);
        
      coefficient=quotient;
    }
    //built up the string in dummy[0]
    dummy[0]=(char)(to_long(coefficient)+'0');
    strcat(stringtemp,dummy);
      
    if (minusremainder==1)
    {
      //Check whether coefficient has been negative at the start of the procedure
      stringtemp2[0]='-';
    }
      
    //reverse the list to obtain the correct string
    for (int i=strlen(stringtemp)-1;i>=0;i--)
    {
      stringtemp2[strlen(stringtemp)-i-1+minusremainder]=stringtemp[i];
        
    }
    stringtemp2[strlen(stringtemp)+minusremainder]='\0';
      
      
  }

  //convert the string to canonicalform using the char*-Constructor
  return CanonicalForm(stringtemp2);
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertFacCF2NTLZZX                                                     //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for conversion of canonicalforms in Factory to polynomials             //
// of type ZZX of NTL. To guarantee the correct execution of the                  //
// algorithm the characteristic has to equal zero.                                //
//                                                                                //
// INPUT:  The canonicalform that has to be converted                             //
// OUTPUT: The converted NTL-polynom of type ZZX                                  //
////////////////////////////////////////////////////////////////////////////////////

ZZX convertFacCF2NTLZZX(CanonicalForm f)
{  
    ZZX ntl_poly;

        
    CFIterator i;
    i=f;

    int j=0;
    int NTLcurrentExp=i.exp();
    int largestExp=i.exp();
    int k;

    //set the length of the NTL-polynomial
    ntl_poly.SetMaxLength(largestExp+1);
    
    
    //Go through the coefficients of the canonicalform and build up the NTL-polynomial
    for (i;i.hasTerms();i++)
    {
      
      for (k=NTLcurrentExp;k>i.exp();k--)
      {
        SetCoeff(ntl_poly,k,0);
      }
      NTLcurrentExp=i.exp();

      if (!i.coeff().isImm())
        {  
          //Coefficient is a gmp-number
          mpz_t gmp_val;
          ZZ temp;
          char* stringtemp;
          
          gmp_val[0]=getmpi(i.coeff().getval());
          
          stringtemp=mpz_get_str(NULL,10,gmp_val);
          conv(temp,stringtemp);

          //set the computed coefficient
          SetCoeff(ntl_poly,NTLcurrentExp,temp);
          
        }
      else
      {
        //Coefficient is immediate --> use its value
        SetCoeff(ntl_poly,NTLcurrentExp,i.coeff().intval());
      }
      
      NTLcurrentExp--;
      
    }
    for (k=NTLcurrentExp;k>=0;k--)
      {
        SetCoeff(ntl_poly,k,0);
      }

    //normalize the polynomial
    ntl_poly.normalize();
    
    return ntl_poly;
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLvec_pair_ZZX_long2FacCFFList                                   //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for converting a vector of polynomials from ZZ to a list               //
// CFFList of Factory. This routine will be used after a successful               //
// factorization of NTL to convert the result back to Factory.                    //
// Additionally a variable x and the computed multiplicity, as a type             //
// ZZ of NTL, is needed as parameters indicating the main variable of the         //
// computed canonicalform and the multiplicity of the original polynomial.        // 
// To guarantee the correct execution of the algorithm the characteristic         //
// has to equal zero.                                                             //
//                                                                                //
// INPUT:  A vector of polynomials over ZZ of type vec_pair_ZZX_long and          //
//         a variable x and a multiplicity of type ZZ                             //
// OUTPUT: The converted list of polynomials of type CFFList, all                 //
//         have x as their main variable                                          //
////////////////////////////////////////////////////////////////////////////////////


CFFList convertNTLvec_pair_ZZX_long2FacCFFList(vec_pair_ZZX_long e,ZZ multi,Variable x)
{
  CFFList rueckgabe;
  ZZX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted
  

  // Start by appending the multiplicity 
  
  rueckgabe.append(CFFactor(convertZZ2CF(multi),1));
  

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;
    ZZ coefficient;
    polynom=e[i].a;
    exponent=e[i].b;
    long coeff_long;
    
    for (int j=0;j<deg(polynom)+1;j++)
    {
      coefficient=coeff(polynom,j);
      if (!IsZero(coefficient))
      {
        bigone=bigone + (CanonicalForm(x,j)*convertZZ2CF(coefficient));
      }  
    }

    //append the converted polynomial to the list
    rueckgabe.append(CFFactor(bigone,exponent));
  }
  //return the converted list
  return rueckgabe;
}


////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLZZpX2CF                                                        //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for conversion of elements of arbitrary extensions of ZZp,             // 
// having type ZZpE, of NTL to their corresponding values of type                 //
// canonicalform in Factory.                                                      //
// To guarantee the correct execution of the algorithm the characteristic         //
// has to be an arbitrary prime number and Factory has to compute in an           //
// extension of F_p.                                                              //
//                                                                                //
// INPUT:  The coefficient of type ZZpE and the variable x indicating the main    //
//         variable of the computed canonicalform                                 //
// OUTPUT: The converted value of coefficient as type canonicalform               //
////////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLZZpE2CF(ZZ_pE coefficient,Variable x)
{
  return convertNTLZZpX2CF(rep(coefficient),x);
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLvec_pair_ZZpEX_long2FacCFFList                                 //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for converting a vector of polynomials from ZZpEX to a CFFList         //
// of Factory. This routine will be used after a successful factorization         //
// of NTL to convert the result back to Factory.                                  //
// Additionally a variable x and the computed multiplicity, as a type             //
// ZZpE of NTL, is needed as parameters indicating the main variable of the       //
// computed canonicalform and the multiplicity of the original polynomial.        // 
// To guarantee the correct execution of the algorithm the characteristic         //
// has a be an arbitrary prime number p and computations have to be done          // 
// in an extention of F_p.                                                        //
//                                                                                //
// INPUT:  A vector of polynomials over ZZpE of type vec_pair_ZZ_pEX_long and     //
//         a variable x and a multiplicity of type ZZpE                           //
// OUTPUT: The converted list of polynomials of type CFFList, all polynomials     //
//         have x as their main variable                                          //
//////////////////////////////////////////////////////////////////////////////////// 

CFFList convertNTLvec_pair_ZZpEX_long2FacCFFList(vec_pair_ZZ_pEX_long e,ZZ_pE multi,Variable x,Variable alpha)
{
  CFFList rueckgabe;
  ZZ_pEX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted
  

  // Start by appending the multiplicity
   if (!IsOne(multi)) rueckgabe.append(CFFactor(convertNTLZZpE2CF(multi,x),1));
  

  // Go through the vector e and build up the CFFList
   // As usual bigone summarizes the result during every loop
   for (int i=e.length()-1;i>=0;i--)
   {
     bigone=0;
    
     polynom=e[i].a;
     exponent=e[i].b;

     for (int j=0;j<deg(polynom)+1;j++)
     {
       if (IsOne(coeff(polynom,j)))
       {  
         bigone=bigone+CanonicalForm(x,j);
       }      
       else        
       {    
         CanonicalForm coefficient=convertNTLZZpE2CF(coeff(polynom,j),alpha);
         if (coeff(polynom,j)!=0) 
         {           
           bigone=bigone + (CanonicalForm(x,j)*coefficient);
         }
       }
     }

     //append the computed polynomials together with its exponent to the CFFList
     rueckgabe.append(CFFactor(bigone,exponent));
     
   }
   //return the computed CFFList
  return rueckgabe;
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLGF2E2CF                                                        //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for conversion of elements of extensions of GF2, having type           //
// GF2E, of NTL to their corresponding values of type canonicalform in            //
// Factory.                                                                       //
// To guarantee the correct execution of the algorithm, the characteristic        //
// must equal two and Factory has to compute in an extension of F_2.              //
// As usual this is an optimized special case of the more general conversion      //
// routine from ZZpE to Factory.                                                  //
//                                                                                //
// INPUT:  The coefficient of type GF2E and the variable x indicating the         //
//         main variable of the computed canonicalform                            //
// OUTPUT: The converted value of coefficient as type canonicalform               //
////////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLGF2E2CF(GF2E coefficient,Variable x)
{
  return convertNTLGF2X2CF(rep(coefficient),x);
}

////////////////////////////////////////////////////////////////////////////////////
// NAME: convertNTLvec_pair_GF2EX_long2FacCFFList                                 //
//                                                                                //
// DESCRIPTION:                                                                   //
// Routine for converting a vector of polynomials from GF2EX to a CFFList         // 
// of Factory. This routine will be used after a successful factorization         // 
// of NTL to convert the result back to Factory.                                  // 
// This is a special, but optimized case of the more general conversion           // 
// from ZZpE to canonicalform.                                                    // 
// Additionally a variable x and the computed multiplicity, as a type GF2E        // 
// of NTL, is needed as parameters indicating the main variable of the            // 
// computed canonicalform and the multiplicity of the original polynomial.        // 
// To guarantee the correct execution of the algorithm the characteristic         // 
// has to equal two and computations have to be done in an extention of F_2.      // 
//                                                                                // 
// INPUT:  A vector of polynomials over GF2E of type vec_pair_GF2EX_long and      // 
//         a variable x and a multiplicity of type GF2E                           // 
// OUTPUT: The converted list of polynomials of type CFFList, all polynomials     // 
//         have x as their main variable                                          // 
//////////////////////////////////////////////////////////////////////////////////// 

CFFList convertNTLvec_pair_GF2EX_long2FacCFFList(vec_pair_GF2EX_long e,GF2E multi,Variable x,Variable alpha)
{
  CFFList rueckgabe;
  GF2EX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted
 
  // multiplicity is always one, so we do not have to worry about that

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result during every loop 
   for (int i=e.length()-1;i>=0;i--)
   {
     bigone=0;
    
     polynom=e[i].a;
     exponent=e[i].b;

     for (int j=0;j<deg(polynom)+1;j++)
     {
       if (IsOne(coeff(polynom,j)))
       {  
         bigone=bigone+CanonicalForm(x,j);
       }
       else        
       {    
         CanonicalForm coefficient=convertNTLGF2E2CF(coeff(polynom,j),alpha);
         if (coeff(polynom,j)!=0) 
         {           
           bigone=bigone + (CanonicalForm(x,j)*coefficient);
         }
       }
     }
     
     // append the computed polynomial together with its multiplicity
     rueckgabe.append(CFFactor(bigone,exponent));
     
   }
   // return the computed CFFList
  return rueckgabe;
}
#endif
