#define _CRT_SECURE_NO_WARNINGS
#pragma once

void  rninit   (unsigned long long iufir);
void  rnrest   ();
void  rnconst  (unsigned long long iufir);
//void  rnconfix (unsigned nmb);

unsigned long long rnfirst ();
unsigned long long rnlast  ();
//unsigned long rnconrd ();

double  rnunif ();
//double  rnexp  ();
//double  rnnorm ( );