#define SET_POSITION(Pin0, Pin1, Pin2, Pin3, Position) \
  switch(Position){ \
    case 0: \
      digitalWrite(Pin0, LOW); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, HIGH); \
      break; \
    case 1: \
      digitalWrite(Pin0, LOW); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, HIGH); \ 
      digitalWrite(Pin3, HIGH); \
      break; \
    case 2: \
      digitalWrite(Pin0, LOW); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, HIGH); \
      digitalWrite(Pin3, LOW); \
      break; \
    case 3: \
      digitalWrite(Pin0, LOW); \
      digitalWrite(Pin1, HIGH); \
      digitalWrite(Pin2, HIGH); \
      digitalWrite(Pin3, LOW); \
      break; \
    case 4: \
      digitalWrite(Pin0, LOW); \
      digitalWrite(Pin1, HIGH); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, LOW); \
      break; \
    case 5: \
      digitalWrite(Pin0, HIGH); \
      digitalWrite(Pin1, HIGH); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, LOW); \
      break; \
    case 6: \
      digitalWrite(Pin0, HIGH); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, LOW); \
      break; \
    case 7: \
      digitalWrite(Pin0, HIGH); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, HIGH); \
      break; \
    default: \
      // Lock
      digitalWrite(Pin0, HIGH); \
      digitalWrite(Pin1, LOW); \
      digitalWrite(Pin2, LOW); \
      digitalWrite(Pin3, LOW); \
    break; \
}
