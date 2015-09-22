//-------- ETHERNET SHIELD PROJECT -------
// ---------- STT TELKOM 2015 ------------
#include <Ethernet.h>
#include <SPI.h>

/*******************************************************************************************
 ********                             KONFIGURASI                                    ***
 *******************************************************************************************/
 
char server[] = "wisnuyo.ga"; // SERVER
String location = "/string.txt HTTP/1.0"; // NO SERIAL SERVER
byte mac[] = { 0x90,0xA2,0xDA,0x0F,0x73,0x17 }; // ALAMAT MAC ETHERNET SHIELD
IPAddress ip(192,168,1,178); // IP

EthernetClient client;       // defines our client
char lastState[32];          // string for incoming serial data
int lastStatePos = 0;        // index counter for lastState variable
boolean startRead = false;   // if it's true is reading the page
int stop_connection = 0;     // if the client is not available stops the connection
int light_1 = 6;             // "Light #1" terhubung ke pin digital 7 Arduino
int light_2 = 7;             // "Light #2" terhubung ke pin digital 8 Arduino
int light_3 = 8;             // "Light #3" terhubung ke pin digital 9 Arduino
int AC = 9;         // "Beban AC terhubung ke pin digital 6 Arduino
/*****************************
 ****     SETUP ***
 *****************************/
void setup()
{
  
  Serial.begin(9600); // baudrate
  pinMode(AC, OUTPUT);
  pinMode(light_1, OUTPUT); 
  pinMode(light_2, OUTPUT); 
  pinMode(light_3, OUTPUT);  
   
// ---------- Ethernet Connection ----------
  if (Ethernet.begin(mac) == 0) 
    {
      Serial.println("Failed to configure Ethernet using DHCP");
      Ethernet.begin(mac, ip);
     }
    delay(5000);
}

// --------------------- LOOP ------------------

void loop()
{
  stop_connection = 0; // Reset stop_connection counter
  String pageState = connect_and_read(); // konekeksi ke homeautomationserver  
  updateState(pageState); // updates kondisi Arduino
  Serial.print("Current state:  "); // print serial monitor
  Serial.println(pageState); 
  delay(5000); // delay 5 detik sebelum koneksi ulang
}
//--------------------------------------------------------

void updateState(String pageState)
{
    if(pageState=="1"){             // Turn on tombol #1
    digitalWrite(light_1, HIGH);
  }
  else if(pageState=="2"){         // Turn off tombol #1   
    digitalWrite(light_1, LOW);
  }
  else if(pageState=="3"){          // Turn on tombol #2 
    digitalWrite(light_2, HIGH);
  }
  else if(pageState=="4"){          // Turn off tombol #2
    digitalWrite(light_2, LOW);
  }
  
  else if(pageState=="5"){            // Turn on tombol #3
    digitalWrite(light_3, HIGH);
  }
  
  else if(pageState=="6"){            // Turn off tombol #3
    digitalWrite(light_3, LOW);
  }
  
  else if(pageState=="7"){            // Turn on tombol #4
    digitalWrite(AC, HIGH);
  }
  //when the button "Turn Off" from the "Button #4"" is Pressed
  else if(pageState=="8"){            // Turn off tombol #4
    digitalWrite(AC, LOW);
  }
  else if(pageState=="0"){            // Turn off tombol #4
    digitalWrite(light_1, HIGH);
    delay(1000);
    digitalWrite(light_2, HIGH);
    delay(1000);
    digitalWrite(light_3, HIGH);
    delay(1000);
    digitalWrite(AC, HIGH);
  }
}

//---------------------------------------------
// ---------     DEFAULT SETTING   ------------
//---------------------------------------------


String connect_and_read(){
  Serial.println("connecting..."); // connecting to the server
  if (client.connect(server, 80)) { // port 80 is typical of a www or http:// page
    Serial.println("connected");
    //the exact location you want to connect to
    client.print("GET ");
    client.println(location);
    //your host
    client.println("Host: 192.168.1.1");
    client.println("Connection: close");
    client.println();
    //it's Connected - So let's read the state on our page
    return readState();
    
  }
  //if the client can't connect to the server
  else{
    //restarts the ethernet
    restart();
    //tells the user that failed the connection
    return "connection failed"; 
  }
}
//reads the state of the button your pressed on the website
String readState(){
  //resets the position counter
  lastStatePos = 0;
  //clear lastState memory
  memset( &lastState, 0, 32 ); 
  
  while(true){
    //checks if the client is available
    if (client.available()) {
      //stores what is reading in the char c
      char a = client.read();
       //'<' is our begining character, our state is between '<>' for example: <1>
      if (a == '<' ) { 
        //Now is Ready to start reading the page
        startRead = true;
          
      }
      //after reading the '<'
      else if(startRead){
          //'>' is our ending character
          if(a != '>'){ 
          lastState[lastStatePos] = a;
          lastStatePos ++;
          }
          else{
            //got what we need here! We can disconnect now
            startRead = false;
            client.stop();
            client.flush();
            Serial.println("disconnecting.");    
            //returns the last button pressed    
            return lastState;
          }
        }
    }
    
 stop_connection ++;
 //if the client is not avaibable when the stop_connection reaches 10000
 //it will restart the ethernet connection
 if(stop_connection > 10000){
   //stops the client
   client.stop();
   delay(50);
   Serial.println("The Client is not available.");
   delay(50);
   //restarts the ethernet connection
   restart();
   delay(100);
   return "Error during connection, stop_connection was activated";
 }
  }//where the while(true) ends

}

//restarts the ethernet connection
void restart(){
  Serial.println("restarting...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}
