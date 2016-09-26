// очистка EEPROM - первый запуск
void clear_eeprom()
 {
 for(int i=0;i<512;i++)
   EEPROM.write(i,0);
 } 
// запись в EEPROM - инициализация
void ini_eeprom()
 {
 EEPROM.write(0,3);
 for(int i=1;i<4;i++)
   {
    EEPROM.write(i*15+0,0);
    EEPROM.write(i*15+1,0);
    EEPROM.write(i*15+2,0);
    EEPROM.write(i*15+3,0);
    EEPROM.write(i*15+4,0);
    EEPROM.write(i*15+5,1);
    EEPROM.write(i*15+6,1);
    EEPROM.write(i*15+7,16);
    EEPROM.write(i*15+8,1);
    EEPROM.write(i*15+9,0);
    EEPROM.write(i*15+10,0);
    EEPROM.write(i*15+11,0);
    EEPROM.write(i*15+12,0);
    EEPROM.write(i*15+13,0);
    EEPROM.write(i*15+14,0);
   }
 } 
// загрузить значения для выбранной программы из EEPROM
void get_prg_eeprom(int prg)
 {
 for(int i=0;i<15;i++)
   {
   alarms[prg-1][i]=EEPROM.read(prg*15+i);  
   }
 }
// записать значения выбранной программы в память
void put_prg_eeprom(int prg)
 {
 for(int i=0;i<15;i++)
   {
   EEPROM.write(prg*15+i,lowByte(alarms[prg-1][i]));  
   }
 }
