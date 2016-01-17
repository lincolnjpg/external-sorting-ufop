#include "file.h"

typedef struct
{
  short size;
  tStudent *student;
} tRAM;

void createRAM(tRAM *);
void insertRAM(tRAM *, tStudent, short);
short getSize(tRAM *);
void cleanRAM(tRAM *);
tStudent getStudent(tRAM *, short);
void removeStudent(tRAM *, short);
void sortRAM(tRAM *, short);
short getPriority(tRAM *, short);
void setPriority(tRAM *, short, short);
float getGrade(tRAM *, short);
void swapStudents(tRAM *, short, short);
