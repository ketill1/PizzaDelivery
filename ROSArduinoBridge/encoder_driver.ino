volatile long left_enc_pos = 0L;
volatile long right_enc_pos = 0L;

#define QUADRATURE_ENCODER
// #define RISING_EDGE_ENCODER

#ifdef QUADRATURE_ENCODER
static const int8_t ENC_STATES[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0}; // encoder lookup table
/* Interrupt routine for LEFT encoder, taking care of actual counting */
ISR(PCINT2_vect)
{
  static uint8_t enc_last = 0;

  enc_last <<= 2;                     // shift previous state two places
  enc_last |= (PIND & (3 << 2)) >> 2; // read the current state into lowest 2 bits

  left_enc_pos += ENC_STATES[(enc_last & 0x0f)];
}
/* Interrupt routine for RIGHT encoder, taking care of actual counting */
ISR(PCINT1_vect)
{
  static uint8_t enc_last = 0;

  enc_last <<= 2;                     // shift previous state two places
  enc_last |= (PINC & (3 << 4)) >> 4; // read the current state into lowest 2 bits

  right_enc_pos += ENC_STATES[(enc_last & 0x0f)];
}

#endif

#ifdef RISING_EDGE_ENCODER
ISR(PCINT2_vect)
{
  static uint8_t last_state_A = 0;
  uint8_t current_state_A = (PIND & (1 << PD2)) >> PD2;
  uint8_t current_state_B = (PIND & (1 << PD3)) >> PD3;

  if (current_state_A && !last_state_A)
  { // Check for rising edge on channel A
    if (current_state_B)
      left_enc_pos--; // If B is high, we're going backward
    else
      left_enc_pos++; // If B is low, we're going forward
  }

  last_state_A = current_state_A;
}

ISR(PCINT1_vect)
{
  static uint8_t last_state_A = 0;
  uint8_t current_state_A = (PIND & (1 << PC4)) >> PC4;
  uint8_t current_state_B = (PIND & (1 << PC5)) >> PC5;

  if (current_state_A && !last_state_A)
  { // Check for rising edge on channel A
    if (current_state_B)
      left_enc_pos--; // If B is high, we're going backward
    else
      left_enc_pos++; // If B is low, we're going forward
  }

  last_state_A = current_state_A;
}
#endif

/* Wrap the encoder reading function */
long readEncoder(int i)
{
  if (i == LEFT)
    return left_enc_pos;
  else
    return right_enc_pos;
}

/* Wrap the encoder reset function */
void resetEncoder(int i)
{
  if (i == LEFT)
  {
    left_enc_pos = 0L;
    return;
  }
  else
  {
    right_enc_pos = 0L;
    return;
  }
}

void resetEncoders()
{
  resetEncoder(LEFT);
  resetEncoder(RIGHT);
}