#include "trace.h"

const TraceLevel Trace::traceLevel = Normal;

bool inline Trace::isTraceActive() {
  return Trace::traceLevel != None;
}

void Trace::trace(const char * str) {
  if (Trace::isTraceActive()) {
    Serial.print(str);
  }
}

void Trace::trace(byte b, int base) {
  if (Trace::isTraceActive()) {
    Serial.print(b, base);
  }
}

void Trace::traceln(const char * str) {
   if (Trace::isTraceActive()) {
    Serial.println(str);
  } 
}

void Trace::traceln(byte b) {
   if (Trace::isTraceActive()) {
    Serial.println(b);
  } 
}

void Trace::traceln(byte b, int base) {
  if (Trace::isTraceActive()) {
    Serial.println(b, base);
  }
}