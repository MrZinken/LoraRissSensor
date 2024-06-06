#include <main.h>

void setup()
{
  debug_begin(115200);
  pinMode(Vext, OUTPUT);
  initLorawanModule();
}

void loop()
{
  loraLoopHandler();
}
