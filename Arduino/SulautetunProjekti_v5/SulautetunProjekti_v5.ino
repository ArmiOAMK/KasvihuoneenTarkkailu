int sensorTemp = A1;
int sensorHumid = A0;

float calibTemp[8] = {-10,0,10,20,25,30,40,50}; //lämpötila-taulukko
float calibResist[8] = {747,815,886,961,1000,1040,1122,1209}; //lämpötila-vastukset
float calibVolt[8] = {2.862049227, 2.754820937, 2.651113468, 2.549719531, 2.5, 2.450980392, 2.356267672, 2.263467632}; //Vin 5V
float calibHumidTemp[6] = {10,15,20,25,30,35}; //kosteus-lämpötila-taulukko
float calibHumidPercent[10] = {20,25,30,35,40,45,50,55,60,65}; //kosteusprosentti-taulukko
float calibHumid[10][6] ={ //kosteusmittarin vastus-taulukko
{30000,21000,13500,9800,8000,6300},
{16000,10500,6700,4803,3900,3100},
{7200,5100,3300,2500,2000,1500},
{3200,2350,1800,1300,980,750},
{1400,1050,840,630,470,385},
{386,287,216,166,131,104},
{211,159,123,95,77,63},
{118,91,70,55,44,38},
{64,51,40,31,25,21},
{38,31,25,20,17,13}
};
float combinedHumid[10]; //yhdistetty taulukko painotetulle keskiarvolle lämpötilasta kosteusmittausta varten

float tempAvg;
float x1,x2,y1,y2,kulmakerroin,B;
float temperature,humidity;

float humResistor = 1000; //kosteusmittarin-vastus kohm, eli 1Mohm

void setup() {
  Serial.begin(9600);
}

void loop() {
  temperature = tempAverage(); //ensin lasketaan lämpötila
  humidity = humidityCalc();  //lasketaan kosteusmittarin vastus
  float humidResult = measureHumidity(temperature, humidity); //lämpötilan ja kosteumittarin vastuksen perusteella lasketaan kosteusprosentti
  Serial.print(temperature, 2);
  Serial.print(",");
  Serial.println(humidResult);
  delay(3000);
}

//kosteusmittarin vastus
float humidityCalc(){
  float humidInput = analogRead(sensorHumid);
  float humVolt = (humidInput  * 5 / 1023);
  float humCur = (5 - humVolt) / humResistor;
  float humOhm = humVolt / humCur;
  return humOhm;
}

//lasketaan kosteusprosentti
float measureHumidity(float temp, float humOhm){
  float tableTemp = 0;
  int i = 0;
  do {
    tableTemp = calibHumidTemp[i];
    i = i + 1;
  } while (tableTemp < temp);
  i = i-1; //kumotaan viimeisin muutos i-muuttujaan

  //luodaan keskivertotaulukko
  for (int j = 0; j < 10; j = j+1){
    float weighedAverage = ((tableTemp-temp)/5*calibHumid[j][i-1]) + ((temp-calibHumidTemp[i-1])/5*calibHumid[j][i]);
    combinedHumid[j] = weighedAverage;
  }
  float tableHum = 0;
  int k = 0;
  do {
    tableHum = combinedHumid[k];
    k = k + 1;
  } while (humOhm < tableHum);
  x1 = combinedHumid[k - 1];
  x2 = combinedHumid[k];
  y1 = calibHumidPercent[k - 1];
  y2 = calibHumidPercent[k];
  kulmakerroin = (y2-y1)/(x2-x1);
  B = y1 - kulmakerroin * x1;
  float humidResult = kulmakerroin * humOhm + B;
  return humidResult;
}

//Lasketaan lämpötila 20 kertaa 0,1s välein ja palautetaan keskiarvo
float tempAverage(){
    tempAvg = 0;
    for (int i = 0; i < 20; i = i +1){
      tempAvg = tempAvg + measureTemperature();
      delay(100);
    }  
    return (tempAvg / 20);
}

//Lämpötilan laskenta volttimäärän ja taulukon avulla
float measureTemperature(){
  float sensorValue = analogRead(sensorTemp);
  float voltage = sensorValue * (5.0 / 1023.0);
  float tableVolt = 0;
  int i = 0;
  do{
    tableVolt = calibVolt[i];
    i = i + 1;
  } while (tableVolt < voltage);
  x1 = calibVolt[i -1];
  x2 = calibVolt[i];
  y1 = calibTemp[i -1];
  y2 = calibTemp[i];
  kulmakerroin = (y2-y1)/(x2-x1);
  B = y1 - kulmakerroin * x1;
  float tempResult = kulmakerroin * voltage + B;
  return tempResult;
}


