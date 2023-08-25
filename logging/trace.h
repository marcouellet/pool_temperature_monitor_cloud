#ifndef TRACE_H
#define TRACE_H

enum TraceLevel {
    None,
    Normal
};

class Trace {
public:
    static const TraceLevel traceLevel;
    static void trace(const char * str);
    static void trace(byte b, int base);
    static void traceln(const char * str);
    static void traceln(byte b);
    static void traceln(byte b, int base);
private:
    static bool inline isTraceActive();
};

inline void trace(const char * str) { Trace::trace(str);}
inline void trace(byte b, int base) { Trace::trace(b, base);}
inline void traceln(const char * str) { Trace::traceln(str);}
inline void traceln(byte b) { Trace::traceln(b);}
inline void traceln(byte b, int base) { Trace::traceln(b, base);}

#endif
