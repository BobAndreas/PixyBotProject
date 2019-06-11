//definitions for vscode, can be deleted for arduino
#define uint8_t char
#define int16_t int
#define uint16_t uint

#ifdef unix
#include "Pixy2/Pixy2.h"
#include "Pixy2/Pixy2CCC.h"
#else
#include "Pixy2\Pixy2.h"
#include "Pixy2\Pixy2CCC.h"
#endif


#define DEVWITHOUTARDUINO

#ifdef DEVWITHOUTARDUINO

#define abs(s) (s < 0 ? -s : s)
#define max(a,b) (a < b ? b : a)
#define analogWrite(a,b); 
#define digitalWrite(a,b); 

#define println(a);
#define print(a);
#define delay(a)


#else 
#define print(a) Serial.print(a)
#define println(a) Serial.println(a)
#endif



void printBlock(Block* block) {
  
  //Print signature
  print("Signature = ");
  println(block->m_signature);

  //print x position
  print("x = ");
  println(block->m_x);

  //print y position
  print("y = ");
  println(block->m_y);

  //print tha wirdth of the object
  print("width = ");
  println(block->m_width);

  //print the height of the object
  print("height = ");
  println(block->m_height);

  //print perceived size
  print("perceived size = ");
  println(max(block->m_height, block->m_width));

}