
#ifndef _SUN_CALC_H_
#define _SUN_CALC_H_


//#define I2CDEV_DEFAULT_READ_TIMEOUT     1000

class Sun_calc {
    public:
      //PUT YOUR LATITUDE, LONGITUDE, AND TIME ZONE HERE
  float latitude = 55.98;
  float longitude = 37.21;
  float timezone = 3;
//////////////////////////////////////////////////  
  
//If you live in the southern hemisphere, it would probably be easier
//for you if you make north as the direction where the azimuth equals
//0 degrees. To do so, switch the 0 below with 180.  
  float northOrSouth = 180;

/////////////////////////////////////////////////////////// 
//MISC. VARIABLES
///////////////////////////////////////////////////////////  
  float pi = 3.14159265;
  float altitude;
  float azimuth;
  float delta;
  float h;
/////////////////////////////////////////////////////////// 
  private:
  
   float month2;
   float day;
   float hour2;
   float minute2;
   
  
};

#endif /* _SUN_CALC_H_ */
