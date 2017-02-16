using namespace vaiven;

uint64_t INT_BITS_SET_MASK  = 0x0001000000000000;
uint64_t DBL_BITS_SET_MASK  = 0x0002000000000000;
uint64_t VOID_BITS_SET_MASK = 0x0004000000000000;
uint64_t PTR_BITS_NOT_SET_MASK = INT_BITS_SET_MASK & DBL_BITS_SET_MASK & VOID_BITS_SET_MASK;

bool isInt(uint64_t value) {
  return value & INT_BITS_SET_MASK;
}

bool isDbl(uint64_t value) {
  return value & DBL_BITS_SET_MASK;
}

bool isVoid(uint64_t value) {
  return value & VOID_BITS_SET_MASK;
}

bool isPtr(uint64_t value) {
  return value & PTR_BITS_NOT_SET_MASK == 0;
}
