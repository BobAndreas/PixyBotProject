//definitions for vscode, can be deleted for arduino
#define uint8_t char
#define int16_t int
#define abs(s) (s < 0 ? -s : s)

int16_t combine(int16_t prop1, int16_t prop2) {
  return (prop1 * 2 + prop2) / 3;
}

Block fusion(Block* current, Block* last) {
  Block combined;
  combined.m_x = combine(current->m_x, last->m_x);
  combined.m_y = combine(current->m_y, last->m_y);
  combined.m_width = combine(current->m_width, last->m_width);
  combined.m_height = combine(current->m_height, last->m_height);
  return combined;
}

int16_t CalcSpeed(int width, int height) {
  int perceivedsize = max(height, width);
  return -(SpeedController->next(perceivedsize));

}

void printBlock(Block* block) {
  
  //Print signature
  Serial.print("Signature = ");
  Serial.println(block->m_signature);

  //print x position
  Serial.print("x = ");
  Serial.println(block->m_x);

  //print y position
  Serial.print("y = ");
  Serial.println(block->m_y);

  //print tha wirdth of the object
  Serial.print("width = ");
  Serial.println(block->m_width);

  //print the height of the object
  Serial.print("height = ");
  Serial.println(block->m_height);

  Serial.print("perceived size = ");
  Serial.println(max(block->m_height, block->m_width));

}