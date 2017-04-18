
//////////////////////////////////////////////////  
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
//END MISC. VARIABLES
///////////////////////////////////////////////////////////
  
void setup(){
  Serial.begin(9600);
  latitude = latitude * pi/180;
}

void loop(){

  float month2;
  float day2;
  float hour2;
  float minute2;

  //SET TIME AND DATE HERE//////////////
  month2 = 4;
  day2 = 18;
  hour2 = 9;//Use 24hr clock (ex: 1:00pm = 13:00) and don't use day3light saving time.
  minute2 = 40;
  //END SET TIME AND DATE /////////////


  //START OF THE CODE THAT CALCULATES THE POSITION OF THE SUN
  float n = daynum(month2) + day2;//NUMBER OF dayS SINCE THE START OF THE YEAR. 
  delta = .409279 * sin(2 * pi * ((284 + n)/365.25));//SUN'S DECLINATION.
  day2 = dayToArrayNum(day2);//TAKES THE CURRENT day OF THE MONTH AND CHANGES IT TO A LOOK UP VALUE ON THE HOUR ANGLE TABLE.
  h = (FindH(day2,month2)) + longitude + (timezone * -1 * 15);//FINDS THE NOON HOUR ANGLE ON THE TABLE AND MODIFIES IT FOR THE USER'S OWN LOCATION AND TIME ZONE.
  h = ((((hour2 + minute2/60) - 12) * 15) + h)*pi/180;//FURTHER MODIFIES THE NOON HOUR ANGLE OF THE CURRENT day AND TURNS IT INTO THE HOUR ANGLE FOR THE CURRENT HOUR AND MINUTE.
  altitude = (asin(sin(latitude) * sin(delta) + cos(latitude) * cos(delta) * cos(h)))*180/pi;//FINDS THE SUN'S ALTITUDE.
  azimuth = ((atan2((sin(h)),((cos(h) * sin(latitude)) - tan(delta) * cos(latitude)))) + (northOrSouth*pi/180)) *180/pi;//FINDS THE SUN'S AZIMUTH.
  //END OF THE CODE THAT CALCULATES THE POSITION OF THE SUN

  Serial.println("Altitude");
  Serial.println(altitude);  
  Serial.println("Azimuth");
  Serial.println(azimuth);
  delay(5000);

}//End Void Loop


//THIS CODE TURNS THE MONTH INTO THE NUMBER OF day3S SINCE JANUARY 1ST.
//ITS ONLY PURPOSE IS FOR CALCULATING DELTA (DECLINATION), AND IS NOT USED IN THE HOUR ANGLE TABLE OR ANYWHERE ELSE.
      float daynum(float month){
       float day3;
       if (month == 1){day3=0;}
       if (month == 2){day3=31;}       
       if (month == 3){day3=59;}       
       if (month == 4){day3=90;}
       if (month == 5){day3=120;}
       if (month == 6){day3=151;}
       if (month == 7){day3=181;}
       if (month == 8){day3=212;}
       if (month == 9){day3=243;}
       if (month == 10){day3=273;}
       if (month == 11){day3=304;}
       if (month == 12){day3=334;} 
       return day3; 
      }

//THIS CODE TAKES THE day3 OF THE MONTH AND DOES ONE OF THREE THINGS: ADDS A day3, SUBTRACTS A day3, OR
//DOES NOTHING. THIS IS DONE SO THAT LESS VALUES ARE REQUIRED FOR THE NOON HOUR ANGLE TABLE BELOW.
       int dayToArrayNum(int day3){
            if ((day3 == 1) || (day3 == 2) || (day3 == 3)){day3 = 0;}
            if ((day3 == 4) || (day3 == 5) || (day3 == 6)){day3 = 1;}  
            if ((day3 == 7) || (day3 == 8) || (day3 == 9)){day3 = 2;}
            if ((day3 == 10) || (day3 == 11) || (day3 == 12)){day3 = 3;}
            if ((day3 == 13) || (day3 == 14) || (day3 == 15)){day3 = 4;}
            if ((day3 == 16) || (day3 == 17) || (day3 == 18)){day3 = 5;}
            if ((day3 == 19) || (day3 == 20) || (day3 == 21)){day3 = 6;}
            if ((day3 == 22) || (day3 == 23) || (day3 == 24)){day3 = 7;}
            if ((day3 == 25) || (day3 == 26) || (day3 == 27)){day3 = 8;}
            if ((day3 == 28) || (day3 == 29) || (day3 == 30) || (day3 == 31)){day3 = 9;}
          return day3;
       }

//////////////////////////////////////////////////////////////
//HERE IS THE TABLE OF NOON HOUR ANGLE VALUES. THESE VALUES GIVE THE HOUR ANGLE, IN DEGREES, OF THE SUN AT NOON (NOT SOLAR NOON)
//WHERE LONGITUDE = 0. dayS ARE SKIPPED TO SAVE SPACE, WHICH IS WHY THERE ARE NOT 365 NUMBERS IN THIS TABLE.
      float FindH(int day3, int month){
      float h;
      
      if (month == 1){
            float h_Array[10]={
            -1.038,-1.379,-1.703,-2.007,-2.289,-2.546,-2.776,-2.978,-3.151,-3.294,};
            h = h_Array[day3];}

      if (month == 2){
            float h_Array[10]={
            -3.437,-3.508,-3.55,-3.561,-3.545,-3.501,-3.43,-3.336,-3.219,-3.081,};
            h = h_Array[day3];}

      if (month == 3){
            float h_Array[10]={
            -2.924,-2.751,-2.563,-2.363,-2.153,-1.936,-1.713,-1.487,-1.26,-1.035,};
            h = h_Array[day3];}

      if (month == 4){
            float h_Array[10]={
            -0.74,-0.527,-0.322,-0.127,0.055,0.224,0.376,0.512,0.63,0.728,};
            h = h_Array[day3];}

      if (month == 5){
            float h_Array[10]={
            0.806,0.863,0.898,0.913,0.906,0.878,0.829,0.761,0.675,0.571,};
            h = h_Array[day3];}

      if (month == 6){
            float h_Array[10]={
            0.41,0.275,0.128,-0.026,-0.186,-0.349,-0.512,-0.673,-0.829,-0.977,};
            h = h_Array[day3];}
            
      if (month == 7){
            float h_Array[10]={
            -1.159,-1.281,-1.387,-1.477,-1.547,-1.598,-1.628,-1.636,-1.622,-1.585,};
            h = h_Array[day3];}

      if (month == 8){
            float h_Array[10]={
            -1.525,-1.442,-1.338,-1.212,-1.065,-0.9,-0.716,-0.515,-0.299,-0.07,};
            h = h_Array[day3];}

      if (month == 9){
            float h_Array[10]={
            0.253,0.506,0.766,1.03,1.298,1.565,1.831,2.092,2.347,2.593,};
            h = h_Array[day3];}

      if (month == 10){
            float h_Array[10]={
            2.828,3.05,3.256,3.444,3.613,3.759,3.882,3.979,4.049,4.091,};
            h = h_Array[day3];}

      if (month == 11){
            float h_Array[10]={
            4.1,4.071,4.01,3.918,3.794,3.638,3.452,3.236,2.992,2.722,};
            h = h_Array[day3];}

      if (month == 12){
            float h_Array[10]={
            2.325,2.004,1.665,1.312,0.948,0.578,0.205,-0.167,-0.534,-0.893,};
            h = h_Array[day3];}

return h;
      }
//////////////////////////////////////////////////////////////
